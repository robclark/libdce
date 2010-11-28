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

/* Note: this is currently a pretty simplistic implementation.. no attempt
 * is made at tracking clients or resource cleanup if the client crashes
 * unexpectedly.  That can be added easily enough later.
 */

#ifndef CLIENT
#  define SERVER 1
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef SERVER
#  include <xdc/std.h>
#  include <xdc/runtime/System.h>
#  include <ti/sysbios/knl/Task.h>
#  include <ti/ipc/MultiProc.h>
#  include <ti/sdo/rcm/RcmServer.h>
#  define Rcm_Handle         RcmServer_Handle
#  define Rcm_Params         RcmServer_Params
#  define Rcm_init           RcmServer_init
#  define Rcm_Params_init    RcmServer_Params_init
#  define Rcm_create         RcmServer_create
#  define Rcm_delete         RcmServer_delete
#  define Rcm_exit           RcmServer_exit
#  define SETUP_FXN(handle, name) do {                                         \
        UInt32 _f = 0;                                                         \
        int _e = RcmServer_addSymbol((handle), #name, rpc_##name, &_f);        \
        if ((_e < 0) || (_f == 0xffffffff)) {                                  \
            ERROR("failed to register function " #name ": %08x", _e);          \
            return _e;                                                         \
        }                                                                      \
    } while (0)
#else
#  include <Std.h>
/* arrrg..  why can't people use stdint types!! */
typedef UInt32 Uint32;
typedef UInt16 Uint16;
typedef UInt8  Uint8;
typedef UInt32 Uns;  /* WTF? */
#  include <MultiProc.h>
#  include <RcmClient.h>
#  include <IpcUsr.h>
#  include <sys/types.h>
#  include <unistd.h>
#  include <stdint.h>
#  include <pthread.h>
#  include <memmgr.h>
#  include <tilermem.h>
#  define Rcm_Handle         RcmClient_Handle
#  define Rcm_Params         RcmClient_Params
#  define Rcm_init           RcmClient_init
#  define Rcm_Params_init    RcmClient_Params_init
#  define Rcm_create         RcmClient_create
#  define Rcm_delete         RcmClient_delete
#  define Rcm_exit           RcmClient_exit
#  define SETUP_FXN(handle, name) do {                                         \
        int _e = RcmClient_getSymbolIndex((handle), #name, &idx_##name);       \
        if (_e < 0) {                                                          \
            ERROR("failed to get function " #name ": %08x", _e);               \
            return _e;                                                         \
        }                                                                      \
    } while (0)
#  define System_printf      printf
#  define System_flush()     do { } while (0)
static void init(void);
static void deinit(void);
#endif

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/video3/viddec3.h>

#ifndef   DIM
#  define DIM(a) (sizeof((a)) / sizeof((a)[0]))
#endif

#ifdef DEBUG
#  define TRACE(FMT,...)  do { \
        System_printf("%s:%d:\t%s\t" FMT "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        System_flush(); \
    } while (0)
#else
#  define TRACE(FMT,...) do { } while (0)
#endif
#define ERROR(FMT,...)   TRACE("error: " FMT, ##__VA_ARGS__)
#define DEBUG(FMT,...)   TRACE("debug: " FMT, ##__VA_ARGS__)


static Rcm_Handle handle = NULL;

/* XXX append a git hash, or version # or something like this, to ensure
 * server and client are built from same version of this code.
 */
#define SERVER_NAME "dCE"


/*
 * Memory allocation/mapping
 */

typedef struct {
    Uint32  size;
    Uint32  ducati_addr;
} MemHeader;


#define P2H(p) (&(((MemHeader *)(p))[-1]))
#define H2P(h) ((void *)&(h)[1])

#ifndef SERVER

/**
 * Allocate a memory block that can be passed as an argument to any of the
 * CE functions.
 */
void * dce_alloc(int sz)
{
    /* TODO: for now, allocate in tiler paged mode (1d) container.. until DMM
     * is enabled on ducati, this would make the physical address the same as
     * the virtual address on ducati, which simplifies some things.  Maybe
     * later use ducati heap instead..
     */
    MemAllocBlock block = {
            .pixelFormat = PIXEL_FMT_PAGE,
            .dim = {
                    .len = sz + sizeof(MemHeader),
            }
    };
    MemHeader *h = MemMgr_Alloc(&block, 1);

    h->size = sz;
    h->ducati_addr = TilerMem_VirtToPhys(H2P(h));

    memset(H2P(h), 0, sz);

    return H2P(h);
}

/**
 * Free a block allocated by dce_alloc()
 */
void dce_free(void *ptr)
{
    MemMgr_Free(P2H(ptr));
}

/**
 * Translate pointer address to ducati.. block should have been allocated
 * with dce_alloc().
 */
static Uint32 virt2ducati(void *ptr)
{
    if (ptr)
        return P2H(ptr)->ducati_addr;
    return 0;
}

#else

/* AFAIK both TILER and heap are cached on ducati side.. so from wherever a9
 * allocates, we need to deal with cache to avoid coherency issues..
 *
 * Hmm, when block is allocated, we need to somehow invalidate it.
 */
#include <ti/sysbios/hal/Cache.h>

static void dce_clean(void *ptr)
{
    Cache_wbInv (ptr, P2H(ptr)->size, Cache_Type_ALL, TRUE);
}
#endif

/*
 * Tracking of memmgr's.. one per client process, so that codec's memory
 * allocates are tracked per client, and memory is freed if client crashes
 * or exits badly
 */

#ifdef SERVER

#include <ti/sdo/rcm/RcmClient.h>
#include <ti/omap/mem/MemMgr.h>

static struct {
    Int pid;
    Int refs;
    RcmClient_Handle handle;
} clients[10] = {0}; /* adjust size per max-clients .. */

static void dce_register_pid(Int pid)
{
    int i, idx = -1;

    // TODO register/unregister should have critical section..

    for (i = 0; i < DIM(clients); i++) {
        if (clients[i].pid == pid) {
            DEBUG("found mem client: refs=%d", clients[i].refs);
            clients[i].refs++;
            return;
        } else if (! clients[i].refs) {
            idx = i;
        }
    }

    if (idx > 0) {
        int err;
        char name[20];
        RcmClient_Params params = {0};

        snprintf (name, DIM(name), "memsrv-%08x", pid);

        DEBUG("creating mem client: %d (%s)", idx, name);

        RcmClient_init();
        RcmClient_Params_init(&params);

        params.heapId = 1; // XXX 1==appm3, but would be nice not to hard-code

        err = RcmClient_create(name, &params, &clients[idx].handle);
        if (err < 0) {
            ERROR("fail: %08x", err);
            return;
        }

        err = MemMgr_RegisterRCMClient(pid, clients[idx].handle);
        if (err < 0) {
            ERROR("fail: %08x", err);
            return;
        }

        clients[idx].refs = 1;
        return;
    }

    ERROR("could not register client");
}

static void dce_unregister_pid(Int pid)
{
    int i;

    for (i = 0; i < DIM(clients); i++) {
        if (clients[i].pid == pid) {
            DEBUG("found mem client: refs=%d", clients[i].refs);
            clients[i].refs--;

            if (! clients[i].refs) {
                DEBUG("deleting mem client: %d", i);
                RcmClient_delete(&clients[i].handle);
            }

            return;
        }
    }

    ERROR("did not find mem client: %d", pid);
}

#else
static Int pid;
#endif

/*
 * Engine_open:
 */

typedef union {
    struct {
        Int    pid;
        Char   name[25];
        /* attrs not supported/needed yet */
    } in;
    struct {
        Int    ec;
        Uint32 engine;
    } out;
} Engine_open__args;

#ifdef SERVER
static Int32 rpc_Engine_open(UInt32 size, UInt32 *data)
{
    Engine_open__args *args = (Engine_open__args *)data;
    Engine_Error ec;

    dce_register_pid(args->in.pid);

    DEBUG(">> name=%s", args->in.name);
    Task_setEnv(Task_self(), (Ptr) args->in.pid);
    args->out.engine = (Uint32)Engine_open(args->in.name, NULL, &ec);
    args->out.ec = ec;
    DEBUG("<< engine=%08x, ec=%d", args->out.engine, args->out.ec);

    return 0;
}
#else
static UInt32 idx_Engine_open;
Engine_Handle Engine_open(String name, Engine_Attrs *attrs, Engine_Error *ec)
{
    int err;
    Engine_Handle ret = NULL;
    Engine_open__args *args;
    RcmClient_Message *msg = NULL;

    init();

    DEBUG(">> name=%s, attrs=%p", name, attrs);

    err = RcmClient_alloc(handle, sizeof(Engine_open__args), &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    msg->fxnIdx = idx_Engine_open;
    args = (Engine_open__args *)&(msg->data);
    args->in.pid = pid;
    strncpy(args->in.name, name, DIM(args->in.name)-1);

    err = RcmClient_exec(handle, msg, &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    args = (Engine_open__args *)&(msg->data);
    if (ec) {
        *ec = args->out.ec;
    }

    ret = (Engine_Handle)args->out.engine;

    DEBUG("<< engine=%p, ec=%d", ret, args->out.ec);

out:
    if (msg) {
        RcmClient_free (handle, msg);
    }

    return ret;
}
#endif

/*
 * Engine_close:
 */

typedef union {
    struct {
        Int    pid;
        Uint32 engine;
    } in;
} Engine_close__args;

#ifdef SERVER
static Int32 rpc_Engine_close(UInt32 size, UInt32 *data)
{
    Engine_close__args *args = (Engine_close__args *)data;

    DEBUG(">> engine=%08x", args->in.engine);
    Task_setEnv(Task_self(), (Ptr) args->in.pid);
    Engine_close((Engine_Handle)(args->in.engine));
    DEBUG("<<");

    dce_unregister_pid(args->in.pid);

    return 0;
}
#else
static UInt32 idx_Engine_close;
Void Engine_close(Engine_Handle engine)
{
    int err;
    Engine_close__args *args;
    RcmClient_Message *msg = NULL;

    DEBUG(">> engine=%p", engine);

    err = RcmClient_alloc(handle, sizeof(Engine_close__args), &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    msg->fxnIdx = idx_Engine_close;
    args = (Engine_close__args *)&(msg->data);
    args->in.pid    = pid;
    args->in.engine = (Uint32)engine;

    err = RcmClient_exec(handle, msg, &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    DEBUG("<<");

out:
    if (msg) {
        RcmClient_free (handle, msg);
    }

    deinit();
}
#endif

/*
 * VIDDEC3_create
 */

typedef union {
    struct {
        Int    pid;
        Uint32 engine;
        Char   name[25];
        Uint32 params;
    } in;
    struct {
        Uint32 codec;
    } out;
} VIDDEC3_create__args;

#ifdef SERVER
static Int32 rpc_VIDDEC3_create(UInt32 size, UInt32 *data)
{
    VIDDEC3_create__args *args = (VIDDEC3_create__args *)data;
    VIDDEC3_Params *params = (VIDDEC3_Params *)args->in.params;

    DEBUG(">> engine=%08x, name=%s, params=%p", args->in.engine, args->in.name, params);
    Task_setEnv(Task_self(), (Ptr) args->in.pid);
    args->out.codec = (Uint32)
            VIDDEC3_create((Engine_Handle)args->in.engine, args->in.name, params);
    dce_clean (params);
    DEBUG("<< codec=%08x", args->out.codec);

    return 0;
}
#else
static UInt32 idx_VIDDEC3_create;
VIDDEC3_Handle VIDDEC3_create(Engine_Handle engine, String name,
        VIDDEC3_Params *params)
{
    int err;
    VIDDEC3_Handle ret;
    VIDDEC3_create__args *args;
    RcmClient_Message *msg = NULL;

    DEBUG(">> engine=%p, name=%s, params=%p", engine, name, params);

    err = RcmClient_alloc(handle, sizeof(VIDDEC3_create__args), &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    msg->fxnIdx = idx_VIDDEC3_create;
    args = (VIDDEC3_create__args *)&(msg->data);
    args->in.pid    = pid;
    args->in.engine = (Uint32)engine;
    strncpy(args->in.name, name, DIM(args->in.name)-1);
    args->in.params = virt2ducati(params);

    err = RcmClient_exec(handle, msg, &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    args = (VIDDEC3_create__args *)&(msg->data);
    ret = (VIDDEC3_Handle)args->out.codec;

    DEBUG("<< codec=%p", ret);

out:
    if (msg) {
        RcmClient_free (handle, msg);
    }

    return ret;
}
#endif

/*
 * VIDDEC3_control
 */

typedef union {
    struct {
        Int             pid;
        Uint32          codec;
        VIDDEC3_Cmd     id;
        Uint32          dynParams;
        Uint32          status;
    } in;
    struct {
        XDAS_Int32      ret;
    } out;
} VIDDEC3_control__args;

#ifdef SERVER
static Int32 rpc_VIDDEC3_control(UInt32 size, UInt32 *data)
{
    VIDDEC3_control__args *args = (VIDDEC3_control__args *)data;
    VIDDEC3_DynamicParams *dynParams =
            (VIDDEC3_DynamicParams *)args->in.dynParams;
    VIDDEC3_Status *status = (VIDDEC3_Status *)args->in.status;

    DEBUG(">> codec=%p, id=%d, dynParams=%p, status=%p",
            args->in.codec, args->in.id, dynParams, status);
    Task_setEnv(Task_self(), (Ptr) args->in.pid);
    args->out.ret = (Uint32)VIDDEC3_control(
            (VIDDEC3_Handle)args->in.codec, args->in.id, dynParams, status);
    dce_clean (dynParams);
    dce_clean (status);
    DEBUG("<< ret=%d", args->out.ret);

    return 0;
}
#else
static UInt32 idx_VIDDEC3_control;
XDAS_Int32 VIDDEC3_control(VIDDEC3_Handle codec, VIDDEC3_Cmd id,
        VIDDEC3_DynamicParams *dynParams, VIDDEC3_Status *status)
{
    int err;
    XDAS_Int32 ret;
    VIDDEC3_control__args *args;
    RcmClient_Message *msg = NULL;

    DEBUG(">> codec=%p, id=%d, dynParams=%p, status=%p",
            codec, id, dynParams, status);

    err = RcmClient_alloc(handle, sizeof(VIDDEC3_control__args), &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    msg->fxnIdx = idx_VIDDEC3_control;
    args = (VIDDEC3_control__args *)&(msg->data);
    args->in.pid        = pid;
    args->in.codec      = (Uint32)codec;
    args->in.id         = id;
    args->in.dynParams  = virt2ducati(dynParams);
    args->in.status     = virt2ducati(status);

    err = RcmClient_exec(handle, msg, &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    args = (VIDDEC3_control__args *)&(msg->data);
    ret = args->out.ret;

    DEBUG("<< ret=%d", ret);

out:
    if (msg) {
        RcmClient_free (handle, msg);
    }

    return ret;
}
#endif

/*
 * VIDDEC3_process
 */

typedef union {
    struct {
        Int        pid;
        Uint32     codec;
        Uint32     inBufs;
        Uint32     outBufs;
        Uint32     inArgs;
        Uint32     outArgs;
    } in;
    struct {
        XDAS_Int32 ret;
    } out;
} VIDDEC3_process__args;

#ifdef SERVER
static Int32 rpc_VIDDEC3_process(UInt32 size, UInt32 *data)
{
    VIDDEC3_process__args *args = (VIDDEC3_process__args *)data;
    XDM2_BufDesc    *inBufs  = (XDM2_BufDesc *)args->in.inBufs;
    XDM2_BufDesc    *outBufs = (XDM2_BufDesc *)args->in.outBufs;
    VIDDEC3_InArgs  *inArgs  = (VIDDEC3_InArgs *)args->in.inArgs;
    VIDDEC3_OutArgs *outArgs = (VIDDEC3_OutArgs *)args->in.outArgs;

    DEBUG(">> codec=%p, inBufs=%p, outBufs=%p, inArgs=%p, outArgs=%p",
            args->in.codec, inBufs, outBufs, inArgs, outArgs);
    Task_setEnv(Task_self(), (Ptr) args->in.pid);
    args->out.ret = (Uint32)VIDDEC3_process(
            (VIDDEC3_Handle)args->in.codec, inBufs, outBufs, inArgs, outArgs);
    dce_clean (inBufs);
    dce_clean (outBufs);
    dce_clean (inArgs);
    dce_clean (outArgs);
    DEBUG("<< ret=%d", args->out.ret);

    return 0;
}
#else
static UInt32 idx_VIDDEC3_process;
XDAS_Int32 VIDDEC3_process(VIDDEC3_Handle codec,
        XDM2_BufDesc *inBufs, XDM2_BufDesc *outBufs,
        VIDDEC3_InArgs *inArgs, VIDDEC3_OutArgs *outArgs)
{
    int err;
    XDAS_Int32 ret;
    VIDDEC3_process__args *args;
    RcmClient_Message *msg = NULL;

    DEBUG(">> codec=%p, inBufs=%p, outBufs=%p, inArgs=%p, outArgs=%p",
            codec, inBufs, outBufs, inArgs, outArgs);

    err = RcmClient_alloc(handle, sizeof(VIDDEC3_process__args), &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    msg->fxnIdx = idx_VIDDEC3_process;
    args = (VIDDEC3_process__args *)&(msg->data);
    args->in.pid     = pid;
    args->in.codec   = (Uint32)codec;
    args->in.inBufs  = virt2ducati(inBufs);
    args->in.outBufs = virt2ducati(outBufs);
    args->in.inArgs  = virt2ducati(inArgs);
    args->in.outArgs = virt2ducati(outArgs);

    err = RcmClient_exec(handle, msg, &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    args = (VIDDEC3_process__args *)&(msg->data);
    ret = args->out.ret;

    DEBUG("<< ret=%d", ret);

out:
    if (msg) {
        RcmClient_free (handle, msg);
    }

    return ret;
}
#endif

/*
 * VIDDEC3_delete
 */

typedef union {
    struct {
        Int    pid;
        Uint32 codec;
    } in;
} VIDDEC3_delete__args;

#ifdef SERVER
static Int32 rpc_VIDDEC3_delete(UInt32 size, UInt32 *data)
{
    VIDDEC3_delete__args *args = (VIDDEC3_delete__args *)data;

    DEBUG(">> codec=%08x", args->in.codec);
    Task_setEnv(Task_self(), (Ptr) args->in.pid);
    VIDDEC3_delete((VIDDEC3_Handle)(args->in.codec));
    DEBUG("<<");

    return 0;
}
#else
static UInt32 idx_VIDDEC3_delete;
Void VIDDEC3_delete(VIDDEC3_Handle codec)
{
    int err;
    VIDDEC3_delete__args *args;
    RcmClient_Message *msg = NULL;

    DEBUG(">> codec=%p", codec);

    err = RcmClient_alloc(handle, sizeof(VIDDEC3_delete__args), &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    msg->fxnIdx = idx_VIDDEC3_delete;
    args = (VIDDEC3_delete__args *)&(msg->data);
    args->in.pid   = pid;
    args->in.codec = (Uint32)codec;

    err = RcmClient_exec(handle, msg, &msg);
    if (err < 0) {
        ERROR("fail: %08x", err);
        goto out;
    }

    DEBUG("<<");

out:
    if (msg) {
        RcmClient_free (handle, msg);
    }
}
#endif

/*
 * Startup/Shutdown
 */

int dce_init(void)
{
    int err = 0;
    Rcm_Params params = {0};

#ifdef SERVER
    RcmServer_ThreadPoolDesc pool = {
            .name = "General Pool",
            .count = 3,
            .priority = Thread_Priority_NORMAL,
    };
#endif

    Rcm_init();
    Rcm_Params_init(&params);

#ifdef SERVER
    params.workerPools.length = 1;
    params.workerPools.elem = &pool;
#else
    params.heapId = 1; //XXX do I need this?
#endif

    err = Rcm_create(SERVER_NAME, &params, &handle);
    if (err < 0) {
        ERROR("failed to create " SERVER_NAME ": 0x%08x", err);
        return err;
    }

    /* Local Function Registration starts on  RCM server */
    SETUP_FXN(handle, Engine_open);
    SETUP_FXN(handle, Engine_close);
    SETUP_FXN(handle, VIDDEC3_create);
    SETUP_FXN(handle, VIDDEC3_control);
    SETUP_FXN(handle, VIDDEC3_process);
    SETUP_FXN(handle, VIDDEC3_delete);

#ifdef SERVER
    RcmServer_start(handle);
#endif

    DEBUG(SERVER_NAME " running");

    return err;
}

int dce_deinit(void)
{
    int err = 0;

    DEBUG("shutdown");

    if (handle) {
        err = Rcm_delete(&handle);
        if (err < 0) {
            ERROR("failed to delete " SERVER_NAME ": %08x", err);
        }
        handle = NULL;
    }

    Rcm_exit();

    DEBUG("deleted " SERVER_NAME);

    return err;
}

#ifndef SERVER
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int count = 0;

int memsrv_init (char *name);
int memsrv_deinit (void);

/* TODO: don't do this on library load.. do it on Engine_open()/Engine_close().. */
static void init(void)
{
    int err;
    Ipc_Config config = {0};
    char name[20];

    pthread_mutex_lock(&mutex);

    if (count > 0) {
        goto out;
    }

    count++;

    Ipc_getConfig(&config);

    err = Ipc_setup(&config);
    DEBUG("Ipc_setup() -> %08x", err);

    pid = getpid();
    snprintf (name, DIM(name), "memsrv-%08x", pid);

    err = memsrv_init(name);
    DEBUG("memsrv_init() -> %08x", err);

    err = dce_init();
    DEBUG("dce_init() -> %08x", err);

out:
    pthread_mutex_unlock(&mutex);
}

static void deinit(void)
{
    int err;

    pthread_mutex_lock(&mutex);

    count--;

    if (count > 0) {
        goto out;
    }

    err = dce_deinit();
    DEBUG("dce_deinit() -> %08x", err);

    err = memsrv_deinit();
    DEBUG("memsrv_deinit() -> %08x", err);

out:
    pthread_mutex_unlock(&mutex);
}
#endif
