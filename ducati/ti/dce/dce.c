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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <ti/sdo/fc/global/FCSettings.h>
#include <ti/sdo/ce/global/CESettings.h>
#include <xdc/runtime/knl/Thread.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/rpmsg/VirtQueue.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/video3/viddec3.h>
#include <ti/sdo/ce/video2/videnc2.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/sysbios/hal/Cache.h>

#include <ti/ipc/rpmsg/MessageQCopy.h>
#include <ti/srvmgr/NameMap.h>
#include <ti/resmgr/IpcResource.h>

#include "dce_priv.h"
#include "dce_rpc.h"

#define DCE_PORT 42     /* so long, and thanks for all the fish */

uint32_t dce_debug = 1;

#define MEMORYSTATS_DEBUG

/* AFAIK both TILER and heap are cached on ducati side.. so from wherever a9
 * allocates, we need to deal with cache to avoid coherency issues..
 *
 * Hmm, when block is allocated, we need to somehow invalidate it.
 */

static void dce_clean(void *ptr)
{
    Cache_wbInv (ptr, P2H(ptr)->size, Cache_Type_ALL, TRUE);
}

enum omap_dce_codec {
    OMAP_DCE_VIDENC2 = 1,
    OMAP_DCE_VIDDEC3 = 2,
};

typedef void * (*CreateFxn)(Engine_Handle, String, void *);
typedef Int32  (*ControlFxn)(void *, int, void *, void *);
typedef Int32  (*ProcessFxn)(void *, void *, void *, void *, void *);
typedef Int32  (*RelocFxn)(void *, uint8_t *ptr, uint32_t len);
typedef void   (*DeleteFxn)(void *);

static int videnc2_reloc(VIDDEC3_Handle handle, uint8_t *ptr, uint32_t len);
static int viddec3_reloc(VIDDEC3_Handle handle, uint8_t *ptr, uint32_t len);

static struct {
    CreateFxn  create;
    ControlFxn control;
    ProcessFxn process;
    DeleteFxn  delete;
    RelocFxn   reloc;   /* handle buffer relocation table */
} codec_fxns[] = {
        [OMAP_DCE_VIDENC2] = {
                (CreateFxn)VIDENC2_create,   (ControlFxn)VIDENC2_control,
                (ProcessFxn)VIDENC2_process, (DeleteFxn)VIDENC2_delete,
                (RelocFxn)videnc2_reloc,
        },
        [OMAP_DCE_VIDDEC3] = {
                (CreateFxn)VIDDEC3_create,   (ControlFxn)VIDDEC3_control,
                (ProcessFxn)VIDDEC3_process, (DeleteFxn)VIDDEC3_delete,
                (RelocFxn)viddec3_reloc,
        },
};

static int videnc2_reloc(VIDENC2_Handle handle, uint8_t *ptr, uint32_t len)
{
    return -1; // TODO
}

static int viddec3_reloc(VIDDEC3_Handle handle, uint8_t *ptr, uint32_t len)
{
    static VIDDEC3_DynamicParams params = {
            .size = sizeof(VIDDEC3_DynamicParams),
    };
    VIDDEC3_Status status = {
            .size = sizeof(VIDDEC3_Status),
            .data = {
                    .buf = (XDAS_Int8 *)ptr,
                    .bufSize = (XDAS_Int32)len,
            },
    };
INFO("status.size=%d", status.size);
    return VIDDEC3_control(handle, XDM_UPDATEBUFS, &params, &status);
}


/*
 * RPC message handlers
 */

static int connect(void *msg)
{
    struct dce_rpc_connect_req *req = msg;

    DEBUG(">> chipset_id=0x%x, debug=%d", req->chipset_id, req->debug);
    dce_debug = req->debug;

    if (dce_debug <= 1) {
        DEBUG("enabling extra debug");

        FCSettings_init();
        Diags_setMask(FCSETTINGS_MODNAME"+12345678LEXAIZFS");
        CESettings_init();
        Diags_setMask(CESETTINGS_MODNAME"+12345678LEXAIZFS");

        /*
         * Enable use of runtime Diags_setMask per module:
         *
         * Codes: E = ENTRY, X = EXIT, L = LIFECYCLE, F = INFO, S = STATUS
         */
        Diags_setMask("ti.ipc.rpmsg.MessageQCopy=EXLFS");
        Diags_setMask("ti.ipc.rpmsg.VirtQueue=EXLFS");
    }

    ivahd_init(req->chipset_id);
    DEBUG("<<");

    return 0;
}

static int engine_open(void *msg)
{
    struct dce_rpc_engine_open_req *req = msg;
    struct dce_rpc_engine_open_rsp *rsp = msg;
    Engine_Error ec;

    DEBUG(">> name=%s", req->name);
    rsp->engine = (uint32_t)Engine_open(req->name, NULL, &ec);
    rsp->error_code = ec;
    DEBUG("<< engine=%08x, ec=%d", rsp->engine, rsp->error_code);

    return sizeof(*rsp);
}

static int engine_close(void *msg)
{
    struct dce_rpc_engine_close_req *req = msg;
    DEBUG(">> engine=%08x", req->engine);
    Engine_close((Engine_Handle)req->engine);
    DEBUG("<<");
    return 0;
}

static int codec_create(void *msg)
{
#ifdef MEMORYSTATS_DEBUG
    Memory_Stats stats;
#endif
    struct dce_rpc_codec_create_req *req = msg;
    struct dce_rpc_codec_create_rsp *rsp = msg;
    void *sparams = H2P((MemHeader *)req->sparams);

    DEBUG(">> engine=%08x, name=%s, sparams=%p, codec_id=%d",
            req->engine, req->name, sparams, req->codec_id);
    DEBUG("   sparams size: %d", ((int32_t *)sparams)[0]);
    rsp->codec = (uint32_t)codec_fxns[req->codec_id].create(
            (Engine_Handle)req->engine, req->name, sparams);
    dce_clean(sparams);
    DEBUG("<< codec=%08x", rsp->codec);

#ifdef MEMORYSTATS_DEBUG
    Memory_getStats(NULL, &stats);
    INFO("Total: %d\tFree: %d\tLargest: %d", stats.totalSize,
         stats.totalFreeSize, stats.largestFreeSize);
#endif

    return sizeof(*rsp);
}

static int codec_control(void *msg)
{
    struct dce_rpc_codec_control_req *req = msg;
    struct dce_rpc_codec_control_rsp *rsp = msg;
    void *dparams = H2P((MemHeader *)req->dparams);
    void *status  = H2P((MemHeader *)req->status);

    DEBUG(">> codec=%08x, cmd_id=%d, dynParams=%p, status=%p, codec_id=%d",
            req->codec, req->cmd_id, dparams, status, req->codec_id);
    DEBUG("   dparams size: %d", ((int32_t *)dparams)[0]);
    DEBUG("   status size:  %d", ((int32_t *)status)[0]);
    rsp->result = codec_fxns[req->codec_id].control(
            (void *)req->codec, req->cmd_id, dparams, status);
    dce_clean(dparams);
    dce_clean(status);
    DEBUG("<< ret=%d", rsp->result);

    return sizeof(*rsp);
}

/* Notes about serialization of process command:
 *
 * Since codec_process code on kernel side is doing buffer mapping/unmapping,
 * and keeping track of codec's locked buffers, it is necessary for it to
 * look into the contents of some of the parameter structs, and in some cases
 * re-write them.  For this reason inArgs/outBufs/inBufs are serialized within
 * the rpmsg rather than just passed by pointer.

XDAS_Int32 VIDDEC3_process(VIDDEC3_Handle handle, XDM2_BufDesc *inBufs,
    XDM2_BufDesc *outBufs, VIDDEC3_InArgs *inArgs, VIDDEC3_OutArgs *outArgs);

  REQ:
    struct dce_rpc_hdr hdr   -> 4
    codec_id                 -> 4
    codec                    -> 4
    reloc length             -> 1   (length/4)
    inArgs length            -> 1   (length/4)
    outBufs length           -> 1   (length/4)
    inBufs length            -> 1   (length/4)
    VIDDEC3_OutArgs *outArgs -> 4   (pass by pointer)
    reloc table              -> 12 * nreloc (typically <= 16)
    VIDDEC3_InArgs   inArgs  -> 12  (need inputID from userspace)
    XDM2_BufDesc     outBufs -> 44  (4 + 2 * 20)
    XDM2_BufDesc     inBufs  -> 24  (4 + 1 * 20)
    -------------------------------
                               99

  RSP
    struct dce_rpc_hdr hdr   -> 4
    result                   -> 4
    inBufs length            -> 1   (length/4)
    XDAS_Int32 freeBufID[]   -> 4*n (n typically 0 or 2, but could be up to 20)
    -------------------------------
                               9-89
    Note: freeBufID[] duplicates what is returned in outArgs, but avoids
    needing to create kernel mappings of these objects which are to big
    to copy inline.  Also it avoids differences between VIDDEC3/VIDDENC2.


XDAS_Int32 VIDENC2_process(VIDENC2_Handle handle, IVIDEO2_BufDesc *inBufs,
    XDM2_BufDesc *outBufs, IVIDENC2_InArgs *inArgs, IVIDENC2_OutArgs *outArgs);

  REQ:
    struct dce_rpc_hdr hdr   -> 4
    codec_id                 -> 4
    codec                    -> 4
    reloc length             -> 1   (length/4)
    inArgs length            -> 1   (length/4)
    outBufs length           -> 1   (length/4)
    inBufs length            -> 1   (length/4)
    VIDENC2_OutArgs *outArgs -> 4   (pass by pointer)
    reloc table              -> ???
    VIDENC2_InArgs   inArgs  -> 12  (need inputID from userspace)
    XDM2_BufDesc     outBufs -> 24  (4 + 1 * 20)
    IVIDEO2_BufDesc  inBufs  -> 252
    -------------------------------
                              307

  RSP
    struct dce_rpc_hdr hdr   -> 4
    result                   -> 4
    inBufs length            -> 1   (length/4)
    XDAS_Int32 freeBufID[]   -> 4*n (n typically 0 or 2, but could be up to 20)
    -------------------------------
                               9-89
 */

static int codec_process(void *msg)
{
    struct dce_rpc_codec_process_req *req = msg;
    struct dce_rpc_codec_process_rsp *rsp = msg;
    void *out_args = H2P((MemHeader *)req->out_args);
    uint32_t codec_id = req->codec_id;
    uint8_t *ptr = req->data;
    void *reloc, *in_args, *out_bufs, *in_bufs;

    reloc = ptr;
    ptr += (req->reloc_len * 4);

    in_args = ptr;
    ptr += (req->in_args_len * 4);

    out_bufs = ptr;
    ptr += (req->out_bufs_len * 4);

    in_bufs = ptr;
    ptr += (req->in_bufs_len * 4);

    DEBUG(">> codec=%p, inBufs=%p, outBufs=%p, inArgs=%p, outArgs=%p, codec_id=%d",
            req->codec, in_bufs, out_bufs, in_args, out_args, codec_id);

    rsp->result = IALG_EOK;

    if (req->reloc_len)
        rsp->result = codec_fxns[codec_id].reloc(
                (void *)req->codec, reloc, (req->reloc_len * 4));

    if (rsp->result == IALG_EOK) {
        ivahd_acquire();
        rsp->result = codec_fxns[codec_id].process(
                (void *)req->codec, in_bufs, out_bufs, in_args, out_args);
        ivahd_release();
    } else {
        DEBUG("reloc failed");
    }

    DEBUG("<< ret=%d", rsp->result);

    rsp->count = 0;

    if (rsp->result == IALG_EOK) {
        int32_t *free_buf_ids;
        int i;

        switch(codec_id) {
        case OMAP_DCE_VIDENC2:
            free_buf_ids = &((VIDENC2_OutArgs *)out_args)->freeBufID[0];
            break;
        case OMAP_DCE_VIDDEC3:
            free_buf_ids = &((VIDDEC3_OutArgs *)out_args)->freeBufID[0];
            break;
        }

        for (i = 0; free_buf_ids[i] && (i < IVIDEO2_MAX_IO_BUFFERS); i++) {
            rsp->freebuf_ids[i] = free_buf_ids[i];
        }
        rsp->count = i;
    }

    DEBUG("freebuf count: %d", rsp->count);
    dce_clean(out_args);

    return sizeof(*rsp) + (4 * rsp->count);
}

static int codec_delete(void *msg)
{
#ifdef MEMORYSTATS_DEBUG
    Memory_Stats stats;
#endif
    struct dce_rpc_codec_delete_req *req = msg;

    DEBUG(">> codec=%08x, codec_id=%d", req->codec, req->codec_id);
    codec_fxns[req->codec_id].delete((void *)req->codec);
    DEBUG("<<");

#ifdef MEMORYSTATS_DEBUG
    Memory_getStats(NULL, &stats);
    INFO("Total: %d\tFree: %d\tLargest: %d", stats.totalSize,
         stats.totalFreeSize, stats.largestFreeSize);
#endif

    return 0;
}

/*
 * RPC msg dispatch table
 */

#define FXN(f) { .name = #f, .fxn = (f) }
static struct {
    int (*fxn)(void *msg);
    const char *name;
} fxns[] = {
        [DCE_RPC_CONNECT]       = FXN(connect),
        [DCE_RPC_ENGINE_OPEN]   = FXN(engine_open),
        [DCE_RPC_ENGINE_CLOSE]  = FXN(engine_close),
        [DCE_RPC_CODEC_CREATE]  = FXN(codec_create),
        [DCE_RPC_CODEC_CONTROL] = FXN(codec_control),
        [DCE_RPC_CODEC_PROCESS] = FXN(codec_process),
        [DCE_RPC_CODEC_DELETE]  = FXN(codec_delete),
};

static void dce_main(uint32_t arg0, uint32_t arg1)
{
    MessageQCopy_Handle handle;
    UInt32 ep, rep;
    Uint16 dst;

    INFO("Creating DCE MessageQ...");

    dst = MultiProc_getId("HOST");

    /* Create the messageQ for receiving (and get our endpoint for sending). */
    handle = MessageQCopy_create(DCE_PORT, &ep);
    NameMap_register("rpmsg-dce", DCE_PORT);

    INFO("Ready to receive requests");

    /* Dispatch loop */
    while (TRUE) {
        UInt16 len;
        static char buffer[512];
        struct dce_rpc_hdr *msg = (struct dce_rpc_hdr *)buffer;
        int ret;

        ret = MessageQCopy_recv(handle, buffer, &len, &rep, MessageQCopy_FOREVER);
        if (ret) {
            ERROR("MessageQ recv error: %d", ret);
            break;
        }

        if (msg->msg_id >= DIM(fxns)) {
            ERROR("invalid msg id: %d", msg->msg_id);
            break;
        }

        DEBUG(">>> %s", fxns[msg->msg_id].name);
        len = fxns[msg->msg_id].fxn(msg);
        DEBUG("<<< %s -> %d", fxns[msg->msg_id].name, len);

        if (len > 0) {
            ret = MessageQCopy_send(dst, rep, ep, buffer, len);
            if (ret) {
                ERROR("MessageQ send error: %d", ret);
                break;
            }
        }
    }

    /* Teardown our side: */
    MessageQCopy_delete(&handle);
}

Bool dce_init(void)
{
    Task_Params params;

    INFO("Creating DCE server thread...");

    /* Create DCE task. */
    Task_Params_init(&params);
    params.instance->name = "dce-server";
    params.priority = Thread_Priority_ABOVE_NORMAL;
    Task_create(dce_main, &params, NULL);

    return TRUE;
}
