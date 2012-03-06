/*
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* NOTE: this code should be folded into a library as part of syslink.. but
 * for now, this ugly hack of duplicating code from syslink and domx..
 */

/*============================================================================
 *  @file   MemMgr.c
 *
 *  @brief  TILER Client Sample application for TILER module between MPU & Ducati
 *
 *  ============================================================================
 */


/* OS-specific headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

/* Standard headers */
#include <Std.h>

/* OSAL & Utils headers */
#include <OsalPrint.h>
#include <String.h>
#include <Trace.h>

/* IPC headers */
#include <IpcUsr.h>
#include <ProcMgr.h>

/* RCM headers */
#include <RcmServer.h>

/* TILER headers */
#include <tilermgr.h>

#include "dce_priv.h"

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */


static RcmServer_Handle rcmServerHandle;


/*
 * Remote function argument structures
 *
 * All fields are simple UInts and Ptrs.  Some arguments may be smaller than
 * these fields, which is okay, as long as they are correctly "unpacked" by the
 * server.
 */

typedef struct {
    UInt    pixelFormat;
    UInt    width;
    UInt    height;
    UInt    length;
    UInt    stride;
    Ptr     ptr;
    UInt  * reserved;
} AllocParams;

typedef struct {
    UInt        numBuffers;
    AllocParams params [1];
} AllocArgs;

typedef struct {
    Ptr bufPtr;
} FreeArgs, ConvertPageModeToTilerSpaceArgs;

typedef struct {
    Ptr     bufPtr;
    UInt    rotationAndMirroring;
} ConvertToTilerSpaceArgs;

typedef struct {
    UInt32 code;
} DebugArgs;


typedef struct  {  /* TODO: remove */
    enum pixel_fmt_t pixelFormat;  /* pixel format */
    union {
        struct {
            pixels_t width;  /* width of 2D buffer */
            pixels_t height; /* height of 2D buffer */
        };
        struct {
            bytes_t  length;  /* length of 1D buffer.  Must be multiple of
                                stride if stride is not 0. */
        };
    };
    unsigned long   stride;    /* must be multiple of page size.  Can be 0 only
                                if pixelFormat is KPAGE. */
    void          * ptr;               /* pointer to beginning of buffer */
    unsigned long   reserved;  /* system space address (used internally) */
} MemMgrBlock;

/*
 *  ======== getAccessMode ========
 *  helper func to determine bit mode
 */
static
UInt getAccessMode (Ptr bufPtr)
{
    UInt addr = (UInt)bufPtr;

    /*
     * The access mode decoding is as follows:
     *
     * 0x60000000 - 0x67FFFFFF : 8-bit
     * 0x68000000 - 0x6FFFFFFF : 16-bit
     * 0x70000000 - 0x77FFFFFF : 32-bit
     * 0x77000000 - 0x7FFFFFFF : Page mode
     */
    switch (addr & 0xf8000000) {   /* Mask out the lower bits */
    case 0x60000000:
        return PIXEL_FMT_8BIT;
    case 0x68000000:
        return PIXEL_FMT_16BIT;
    case 0x70000000:
        return PIXEL_FMT_32BIT;
    case 0x78000000:
        return PIXEL_FMT_PAGE;
    default:        /* TODO: How to handle invalid case? */
        return 0;
    }
}

/*
 *  ======== getStride ========
 *  helper func to determine stride length
 */
static
UInt getStride (Ptr bufPtr)
{
    switch (getAccessMode (bufPtr)) {
    case PIXEL_FMT_8BIT:
        return 0x4000;  /* 16 KB of stride */
    case PIXEL_FMT_16BIT:
    case PIXEL_FMT_32BIT:
        return 0x8000;  /* 32 KB of stride */
    default:
        return 0;       /* Stride not applicable */
    }
}

/*
 *  ======== fxnMemMgr_Debug ========
 *     RCM function for debugging
*/
static
Int32 fxnMemMgr_Debug (UInt32 dataSize, UInt32 *data)
{
    DebugArgs * args = (DebugArgs *)data;
    DEBUG ("Executing MemMgr_Debug: %08x\n", args->code);
}

/*
 *  ======== fxnMemMgr_Alloc ========
 *     RCM function for MemMgr_Alloc function
*/
static
Int32 fxnMemMgr_Alloc (UInt32 dataSize, UInt32 *data)
{
    AllocArgs     * args = (AllocArgs *)data;
    Int             i;
    MemMgrBlock   * params;
    Ptr             allocedPtr;

    DEBUG ("Executing MemMgr_Alloc with params:\n");
    DEBUG ("\tnumBuffers = %d\n", args->numBuffers);
    for(i = 0; i < args->numBuffers; i++) {
        DEBUG ("\tparams [%d].pixelFormat = %d\n", i,
                        args->params [i].pixelFormat);
        DEBUG ("\tparams [%d].width = %d\n", i, args->params [i].width);
        DEBUG ("\tparams [%d].height = %d\n", i, args->params [i].height);
        DEBUG ("\tparams [%d].length = %d\n", i, args->params [i].length);
    }

    params = (MemMgrBlock *) malloc (sizeof(MemMgrBlock) * args->numBuffers);

    if (params == NULL) {
        DEBUG ("Error allocating array of MemMgrBlock params.\n");
        return (Int32)NULL;
    }

    for(i = 0; i < args->numBuffers; i++) {
        params [i].pixelFormat = args->params [i].pixelFormat;

        params [i].width = args->params [i].width;
        /* TODO: provide length support on Ducati */
        params [i].height = args->params [i].height;
        params [i].length = args->params [i].length;
    }

    /* Allocation */
    for (i = 0; i < args->numBuffers; i++) {
        switch (params [i].pixelFormat) {
        case PIXEL_FMT_8BIT:
        case PIXEL_FMT_16BIT:
        case PIXEL_FMT_32BIT:
            DEBUG ("fxnMemMgr_Alloc: calling TilerMgr_Alloc.\n");
            args->params [i].ptr = (Ptr)TilerMgr_Alloc (params [i].pixelFormat,
                                        params [i].width, params [i].height);
            break;
        case PIXEL_FMT_PAGE:
            DEBUG ("fxnMemMgr_Alloc: calling TilerMgr_PageModeAlloc.\n");
            args->params [i].ptr = \
                                (Ptr)TilerMgr_PageModeAlloc (params [i].length);
            break;
        default:    /* Invalid case */
            DEBUG ("fxnMemMgr_Alloc: Invalid pixel format.\n");
            args->params [i].ptr = NULL;
            break;
        }
        args->params [i].stride = getStride (args->params [i].ptr);
    }

    allocedPtr = args->params [0].ptr;
    free (params);

    DEBUG ("fxnMemMgr_Alloc done.\n");
    return (Int32) allocedPtr; /* Return first buffer pointer */
}



/*
 *  ======== fxnMemMgr_Free ========
 *     RCM function for MemMgr_Free
 */
static
Int32 fxnMemMgr_Free (UInt32 dataSize, UInt32 *data)
{
    FreeArgs  * args    = (FreeArgs *)data;
    UInt32      status  = 0;

    DEBUG ("Executing MemMgr_Free with params:\n");
    DEBUG ("\tbufPtr = %p\n", args->bufPtr);

    switch (getAccessMode (args->bufPtr)) {
    case PIXEL_FMT_8BIT:
    case PIXEL_FMT_16BIT:
    case PIXEL_FMT_32BIT:
        DEBUG ("fxnMemAlloc_Free: calling TilerMgr_Free.\n");
        status = TilerMgr_Free ((Int)args->bufPtr);
        break;
    case PIXEL_FMT_PAGE:
        DEBUG ("fxnMemAlloc_Free: calling TilerMgr_PageModeFree.\n");
        status = TilerMgr_PageModeFree ((Int)args->bufPtr);
        break;
    default:    /* Invalid case */
        DEBUG ("fxnMemAlloc_Free: Invalid pointer.\n");
        break;
    }

    DEBUG ("fxnMemMgr_Free done.\n");
    return status;
}

/*
 *  ======== fxnTilerMem_ConvertToTilerSpace ========
 *     RCM function for TilerMem_ConvertToTilerSpace
 */
static
Int32 fxnTilerMem_ConvertToTilerSpace (UInt32 dataSize, UInt32 *data)
{
    ConvertToTilerSpaceArgs   * args = (ConvertToTilerSpaceArgs *)data;
    UInt32                      addr;

    DEBUG ("Executing TilerMem_ConvertToTilerSpace with params:\n");
    DEBUG ("\tbufPtr = %p\n", args->bufPtr);
    DEBUG ("\trotationAndMirroring = 0x%x\n", args->rotationAndMirroring);

    //Stubbed out pending implementation
    /*addr = TilerMem_ConvertToTilerSpace (args->bufPtr,
                                            args->rotationAndMirroring);*/
    addr = TRUE;

    return addr;
}

/*
 *  ======== fxnTilerMem_ConvertPageModeToTilerSpace ========
 *     RCM function for TilerMem_ConvertPageModeToTilerSpace
 */
static
Int32 fxnTilerMem_ConvertPageModeToTilerSpace (UInt32 dataSize, UInt32 *data)
{
    ConvertPageModeToTilerSpaceArgs   * args = \
                                        (ConvertPageModeToTilerSpaceArgs *)data;
    UInt32                              addr;

    DEBUG ("Executing TilerMem_ConvertPageModeToTilerSpace with params:");
    DEBUG ("\n\tbufPtr = %p\n", args->bufPtr);

    //Stubbed out pending implementation
    //addr = TilerMem_ConvertPageModeToTilerSpace (args->bufPtr);
    addr = TRUE;

    return addr;
}

struct MemMgr_funcInfo {
    RcmServer_MsgFxn fxnPtr;
    String           name;
};

static struct MemMgr_funcInfo memMgrFxns [] =
{
    { fxnMemMgr_Alloc,                        "MemMgr_Alloc"},
    { fxnMemMgr_Free,                         "MemMgr_Free"},
    { fxnMemMgr_Debug,                        "MemMgr_Debug"},
    { fxnTilerMem_ConvertToTilerSpace,        "TilerMem_ConvertToTilerSpace"},
    { fxnTilerMem_ConvertPageModeToTilerSpace,
                                        "TilerMem_ConvertPageModeToTilerSpace"},
};

/*
 *  ======== MemMgrThreadFxn ========
 *     TILER server thread function
 */

int
memsrv_init (char *name)
{
    Int                 status;
    RcmServer_Params    rcmServerParams;
    UInt                fxnIdx;
    Int                 i;

    /* RCM Server module init */
    DEBUG ("RCM Server module init.\n");
    RcmServer_init ();

    /* rcm client module params init*/
    DEBUG ("RCM Server module params init.\n");
    status = RcmServer_Params_init (&rcmServerParams);
    if (status < 0) {
        DEBUG ("Error in RCM Server instance params init \n");
        return status;
    }

    /* create the RcmServer instance */
    DEBUG ("Creating RcmServer instance: %s\n", name);
    status = RcmServer_create (name, &rcmServerParams,
                                &rcmServerHandle);
    if (status < 0) {
        DEBUG ("Error in RCM Server create.\n");
        return status;
    }

    for (i = 0; i < DIM (memMgrFxns); i++) {
        status = RcmServer_addSymbol (rcmServerHandle, memMgrFxns [i].name,
                            memMgrFxns [i].fxnPtr, &fxnIdx);
        /* Register the remote functions */
        DEBUG ("Registering remote function %s with index %d\n",
                        memMgrFxns [i].name, fxnIdx);
        if (status < 0) {
            DEBUG ("Add symbol failed with status 0x%08x.\n", status);
            return status;
        }
    }

    status = TilerMgr_Open ();
    if (status < 0) {
        DEBUG ("Error in TilerMgr_Open: status = 0x%x\n", status);
        return status;
    }

    DEBUG ("Start RCM server thread \n");
    RcmServer_start (rcmServerHandle);

    DEBUG ("\nDone initializing RCM server.  Ready to receive requests "
                    "from Ducati.\n");

    return status;
}

int
memsrv_deinit (void)
{
    Int                 status;
    Int                 i;

    if (! rcmServerHandle) {
        return 0;
    }

    for (i = 0; i < DIM (memMgrFxns); i++) {
        /* Unregister the remote functions */
        status = RcmServer_removeSymbol (rcmServerHandle, memMgrFxns [i].name);
        if (status < 0) {
            DEBUG ("Remove symbol %s failed.\n", memMgrFxns [i].name);
        }
    }

    status = RcmServer_delete (&rcmServerHandle);
    if (status < 0) {
        DEBUG ("Error in RcmServer_delete: status = 0x%x\n", status);
        return status;
    }

    RcmServer_exit ();

    return 0;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
