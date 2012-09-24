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
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <xf86drm.h>
#include <omap_drm.h>
#include <omap_dce.h>
#include <omap_drmif.h>
#include <dce.h>
#include <xdc/std.h>
#include <ti/dce/dce_priv.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/video3/viddec3.h>
#include <ti/sdo/ce/video2/videnc2.h>

#ifdef HAVE_X11
int dce_auth_x11(int *fd);
#endif
#ifdef HAVE_WAYLAND
int dce_auth_wayland(int *fd);
#endif

static int init(void);
static void deinit(void);

static int fd = -1;
static struct omap_device *dev;
static int ioctl_base;
#define CMD(name) (ioctl_base + DRM_OMAP_DCE_##name)

uint32_t dce_debug = 3;

void dce_set_fd(int dce_fd) { fd = dce_fd; }
int dce_get_fd () { return fd; }

/*
 * Memory allocation/mapping
 */

/**
 * Allocate a memory block that can be passed as an argument to any of the
 * CE functions.
 */
void * dce_alloc(int sz)
{
    struct omap_bo *bo = omap_bo_new(dev, sz + sizeof(MemHeader), OMAP_BO_WC);
    MemHeader *h;

    if (!bo) {
        ERROR("allocation failed");
        return NULL;
    }

    h = omap_bo_map(bo);
    memset(H2P(h), 0, sz);
    h->size = sz;
    h->bo = bo;

    DEBUG("%p:%p, size=%d, bo=%p", h, H2P(h), h->size, h->bo);

    return H2P(h);
}

/**
 * Free a block allocated by dce_alloc()
 */
void dce_free(void *ptr)
{
    if (ptr) {
        MemHeader *h = P2H(ptr);
        DEBUG("%p:%p, size=%d, bo=%p", h, ptr, h->size, h->bo);
        omap_bo_del(h->bo);
    }
}

/**
 * Translate pointer to handle.. block should have been allocated
 * with dce_alloc().
 */
static uint32_t virt2bo(void *ptr)
{
    if (ptr) {
        MemHeader *h = P2H(ptr);
        DEBUG("%p:%p, size=%d, bo=%p", h, ptr, h->size, h->bo);
        return omap_bo_handle(h->bo);
    }
    return 0;
}

/*
 * Engine_open:
 */
Engine_Handle Engine_open(String name, Engine_Attrs *attrs, Engine_Error *ec)
{
    Engine_Handle engine = NULL;
    struct drm_omap_dce_engine_open req = {};
    int ret;

    if (init()) {
        ERROR("init failed");
        goto out;
    }

    DEBUG(">> name=%s, attrs=%p", name, attrs);

    strncpy(req.name, name, sizeof(req.name));

    ret = drmCommandWriteRead(fd, CMD(ENGINE_OPEN), &req, sizeof(req));
    if (ret) {
        ERROR("ioctl failed: %d", ret);
        goto out;
    }

    engine = (Engine_Handle)req.eng_handle;
    *ec = (Engine_Error)req.error_code;

out:
    DEBUG("<< engine=%p, ec=%d", engine, *ec);
    return engine;
}

/*
 * Engine_close:
 */
Void Engine_close(Engine_Handle engine)
{
    struct drm_omap_dce_engine_close req = {
            .eng_handle = (uint32_t)engine
    };
    int ret;

    DEBUG(">> engine=%p", engine);

    ret = drmCommandWrite(fd, CMD(ENGINE_CLOSE), &req, sizeof(req));
    if (ret) {
        ERROR("ioctl failed: %d", ret);
        goto out;
    }

out:
    DEBUG("<<");
    deinit();
}

/*
 * Common codec glue.. function signatures are the same between
 * VIDDEC3/VIDENC2
 */

static void * create(Engine_Handle engine, String name, void *params,
        enum omap_dce_codec codec_id)
{
    struct drm_omap_dce_codec_create req = {
            .codec_id = codec_id,
            .eng_handle = (uint32_t)engine,
            .sparams_bo = virt2bo(params),
    };
    void *codec = NULL;
    int ret;

    strncpy(req.name, name, sizeof(req.name));

    ret = drmCommandWriteRead(fd, CMD(CODEC_CREATE), &req, sizeof(req));
    if (ret) {
        ERROR("ioctl failed: %d", ret);
        goto out;
    }

    codec = (void *)req.codec_handle;

out:
    return codec;
}

XDAS_Int32 control(void *codec, int id, void *dynParams, void *status)
{
    struct drm_omap_dce_codec_control req = {
            .codec_handle = (uint32_t)codec,
            .cmd_id = id,
            .dparams_bo = virt2bo(dynParams),
            .status_bo = virt2bo(status),
    };
    int ret;

    ret = drmCommandWriteRead(fd, CMD(CODEC_CONTROL), &req, sizeof(req));
    if (ret) {
        ERROR("ioctl failed: %d", ret);
        goto out;
    }

    ret = req.result;

out:
    return ret;
}

XDAS_Int32 get_version(void *codec, void *dynParams, void *status)
{
    struct drm_omap_dce_codec_get_version req = {
            .codec_handle = (uint32_t)codec,
            .dparams_bo = virt2bo(dynParams),
            .status_bo = virt2bo(status),
            .version_bo = virt2bo(((IVIDDEC3_Status*)status)->data.buf),
    };
    int ret;

    ret = drmCommandWriteRead(fd, CMD(CODEC_GET_VERSION), &req, sizeof(req));
    if (ret) {
        ERROR("ioctl failed: %d", ret);
        goto out;
    }

    ret = req.result;

out:
    return ret;
}

XDAS_Int32 process(void *codec, void *inBufs, void *outBufs,
        void *inArgs, void *outArgs)
{
    struct drm_omap_dce_codec_process req = {
            .codec_handle = (uint32_t)codec,
            .out_args_bo = virt2bo(outArgs),
            .in_args = (uint64_t)inArgs,
            .out_bufs = (uint64_t)outBufs,
            .in_bufs = (uint64_t)inBufs,
    };
    int ret;

    ret = drmCommandWriteRead(fd, CMD(CODEC_PROCESS), &req, sizeof(req));
    if (ret) {
        ERROR("ioctl failed: %d", ret);
        goto out;
    }

    ret = req.result;

out:
    return ret;
}

void delete(void *codec)
{
    struct drm_omap_dce_codec_delete req = {
            .codec_handle = (uint32_t)codec,
    };
    int ret;

    ret = drmCommandWrite(fd, CMD(CODEC_DELETE), &req, sizeof(req));
    if (ret) {
        ERROR("ioctl failed: %d", ret);
    }
}


/*
 * VIDDEC3_create
 */
VIDDEC3_Handle VIDDEC3_create(Engine_Handle engine, String name,
        VIDDEC3_Params *params)
{
    VIDDEC3_Handle codec;
    DEBUG(">> engine=%p, name=%s, params=%p", engine, name, params);
    codec = create(engine, name, params, OMAP_DCE_VIDDEC3);
    DEBUG("<< codec=%p", codec);
    return codec;
}

/*
 * VIDDEC3_control
 */
XDAS_Int32 VIDDEC3_control(VIDDEC3_Handle codec, VIDDEC3_Cmd id,
        VIDDEC3_DynamicParams *dynParams, VIDDEC3_Status *status)
{
    XDAS_Int32 ret;
    DEBUG(">> codec=%p, id=%d, dynParams=%p, status=%p",
            codec, id, dynParams, status);
    if (id == XDM_GETVERSION)
        ret = get_version(codec, dynParams, status);
    else
        ret = control(codec, id, dynParams, status);
    DEBUG("<< ret=%d", ret);
    return ret;
}

/*
 * VIDDEC3_process
 */
XDAS_Int32 VIDDEC3_process(VIDDEC3_Handle codec,
        XDM2_BufDesc *inBufs, XDM2_BufDesc *outBufs,
        VIDDEC3_InArgs *inArgs, VIDDEC3_OutArgs *outArgs)
{
    XDAS_Int32 ret;
    DEBUG(">> codec=%p, inBufs=%p, outBufs=%p, inArgs=%p, outArgs=%p",
            codec, inBufs, outBufs, inArgs, outArgs);
    ret = process(codec, inBufs, outBufs, inArgs, outArgs);
    DEBUG("<< ret=%d", ret);
    return ret;
}

/*
 * VIDDEC3_delete
 */
Void VIDDEC3_delete(VIDDEC3_Handle codec)
{
    DEBUG(">> codec=%p", codec);
    delete(codec);
    DEBUG("<<");
}

/*
 * VIDENC2_create
 */
VIDENC2_Handle VIDENC2_create(Engine_Handle engine, String name,
        VIDENC2_Params *params)
{
    VIDENC2_Handle codec;
    DEBUG(">> engine=%p, name=%s, params=%p", engine, name, params);
    codec = create(engine, name, params, OMAP_DCE_VIDENC2);
    DEBUG("<< codec=%p", codec);
    return codec;
}

/*
 * VIDENC2_control
 */
XDAS_Int32 VIDENC2_control(VIDENC2_Handle codec, VIDENC2_Cmd id,
        VIDENC2_DynamicParams *dynParams, VIDENC2_Status *status)
{
    XDAS_Int32 ret;
    DEBUG(">> codec=%p, id=%d, dynParams=%p, status=%p",
            codec, id, dynParams, status);
    ret = control(codec, id, dynParams, status);
    DEBUG("<< ret=%d", ret);
    return ret;
}

/*
 * VIDENC2_process
 */
XDAS_Int32 VIDENC2_process(VIDENC2_Handle codec,
        IVIDEO2_BufDesc *inBufs, XDM2_BufDesc *outBufs,
        VIDENC2_InArgs *inArgs, VIDENC2_OutArgs *outArgs)
{
    XDAS_Int32 ret;
    DEBUG(">> codec=%p, inBufs=%p, outBufs=%p, inArgs=%p, outArgs=%p",
            codec, inBufs, outBufs, inArgs, outArgs);
    ret = process(codec, inBufs, outBufs, inArgs, outArgs);
    DEBUG("<< ret=%d", ret);
    return ret;
}

/*
 * VIDDEC3_delete
 */
Void VIDENC2_delete(VIDENC2_Handle codec)
{
    DEBUG(">> codec=%p", codec);
    delete(codec);
    DEBUG("<<");
}

/*
 * Startup/Shutdown/Cleanup
 */

struct omap_device * dce_init(void)
{
    if (init())
        return NULL;
    return dev;
}

void dce_deinit(struct omap_device *dev)
{
    deinit();
}

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int count = 0;


static int init(void)
{
    int authenticated = 0;

    pthread_mutex_lock(&mutex);

    if (count++ > 0) {
        goto out;
    }

#ifdef HAVE_X11
    if (!authenticated) {
        int ret = dce_auth_x11(&fd);
        if (!ret)
            authenticated = 1;
    }
#endif
#ifdef HAVE_WAYLAND
    if (!authenticated) {
        int ret = dce_auth_wayland(&fd);
        if (!ret)
            authenticated = 1;
    }
#endif

    if ((fd == -1) && !authenticated) {
        INFO("no X11/wayland, fallback to opening DRM device directly");
        fd = drmOpen("omapdrm", "platform:omapdrm:00");
    }

    if (fd >= 0) {
        struct drm_omap_get_base req = {};
        int ret;

        strcpy(req.plugin_name, "dce");

        ret = drmCommandWriteRead(fd, DRM_OMAP_GET_BASE, &req, sizeof(req));
        if (ret) {
            ERROR("could not get plugin ioctl base: %d", ret);
            close(fd);
            fd = -1;
            goto out;
        }

        ioctl_base = req.ioctl_base;

        dev = omap_device_new(fd);
    }

out:
    pthread_mutex_unlock(&mutex);
    return (fd >= 0) ? 0 : -1;
}

static void deinit(void)
{
    pthread_mutex_lock(&mutex);

    if (--count > 0) {
        goto out;
    }

    omap_device_del(dev);
    dev = NULL;
    close(fd);
    fd = -1;

out:
    pthread_mutex_unlock(&mutex);
}
