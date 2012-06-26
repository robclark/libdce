/*
 * Copyright (c) 2011, Texas Instruments Incorporated
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

/* This is a dummy implementation of "ti.sdo.fc.ires.tiledmemory"
 * that just allocates from the heap:
 */

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Assert.h>

#include <ti/xdais/ires.h>

#include <ti/sdo/fc/ires/iresman.h>
#include <ti/sdo/fc/ires/tiledmemory/ires_tiledmemory.h>

#include <ti/dce/dce_priv.h>


static IRESMAN_PersistentAllocFxn * allocFxn;   /* Memory alloc function */
static IRESMAN_PersistentFreeFxn * freeFxn;     /* Memory free function */

static void * allocRes(int size)
{
    IALG_MemRec rec = {
            .size = size,
            .alignment = 0x4,
            .space = IALG_EXTERNAL,
            .attrs = IALG_PERSIST,
    };
    if (! allocFxn(&rec, 1)) {
        return NULL;
    }
    return rec.base;
}

static void freeRes(void *base, int size)
{
    IALG_MemRec rec = {
            .base = base,
            .size = size,
            .alignment = 0x4,
            .space = IALG_EXTERNAL,
            .attrs = IALG_PERSIST,
    };

    freeFxn(&rec, 1);
}

static String getProtocolName()
{
    return "ti.sdo.fc.ires.tiledmemory";
}

static IRES_ProtocolRevision * getProtocolRevision()
{
    static IRES_ProtocolRevision revision = {1, 0, 0};
    return &revision;
}

static IRES_Status init(IRESMAN_Params * initArgs)
{
    allocFxn = initArgs->allocFxn;
    freeFxn = initArgs->freeFxn;
    return IRES_OK;
}

static IRES_Status exit()
{
    return IRES_OK;
}

static void getStaticProperties(IRES_Handle resourceHandle,
        IRES_Properties * resourceProperties)
{
    /* unused */
}

static IRES_Handle getHandles(IALG_Handle algHandle,
        IRES_ResourceDescriptor *resDesc, Int scratchGroupId,
        IRES_Status *status)
{
    IRES_TILEDMEMORY_ProtocolArgs *args =
            (IRES_TILEDMEMORY_ProtocolArgs *)resDesc->protocolArgs;
    IRES_TILEDMEMORY_Handle handle = NULL;
    Void *ptr = NULL;
    int size;

    Assert_isTrue(args, NULL);
    Assert_isTrue(algHandle, NULL);

    size = args->sizeDim0;
    if (args->sizeDim1)
        size *= args->sizeDim1;

    DEBUG("alloc: %dx%d (%d)", args->sizeDim0, args->sizeDim1, size);

    ptr = allocRes(size);
    if (!ptr) {
        ERROR("could not allocate buffer: %dx%d (%d)",
                args->sizeDim0, args->sizeDim1, size);
        goto fail;
    }

    handle = allocRes(sizeof(*handle));
    if (!handle) {
        ERROR("could not allocate handle");
        goto fail;
    }

    handle->ires.getStaticProperties = getStaticProperties;
    handle->ires.persistent = IRES_PERSISTENT;
    handle->memoryBaseAddress = ptr;  /* MMU set up for 0x0 offset */
    handle->systemSpaceBaseAddress = ptr;
    handle->isTiledMemory = FALSE;
    handle->accessUnit = IRES_TILEDMEMORY_RAW;
    handle->tilerBaseAddress = NULL;

    DEBUG("allocation succeeded: %dx%d", args->sizeDim0, args->sizeDim1);

    return (IRES_Handle)handle;

    fail:
    if (ptr)	freeRes(ptr, size);
    if (handle)	freeRes(handle, sizeof(*handle));
    return NULL;
}

static IRES_Status freeHandles(IALG_Handle algHandle,
        IRES_Handle algResourceHandle,
        IRES_ResourceDescriptor *resDesc,
        Int scratchGroupId)
{
    IRES_TILEDMEMORY_ProtocolArgs *args =
            (IRES_TILEDMEMORY_ProtocolArgs *)resDesc->protocolArgs;
    IRES_TILEDMEMORY_Handle handle =
            (IRES_TILEDMEMORY_Handle)algResourceHandle;
    int size;

    Assert_isTrue(args, NULL);
    Assert_isTrue(handle, NULL);

    size = args->sizeDim0;
    if (args->sizeDim1)
        size *= args->sizeDim1;

    DEBUG("free: %dx%d (%d)", args->sizeDim0, args->sizeDim1, size);

    freeRes(handle->memoryBaseAddress, size);
    freeRes(handle, sizeof(*handle));

    return IRES_OK;
}


IRESMAN_Fxns IRESMAN_TILEDMEMORY =  {
        getProtocolName,
        getProtocolRevision,
        init,
        exit,
        getHandles,
        freeHandles,
};
