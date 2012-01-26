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

#include <dce.h>
#include <xdc/std.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/video3/viddec3.h>

#include "util.h"

/*
 * A very simple VIDDEC3 client which will decode h264 frames (one per file),
 * and write out raw (unstrided) nv12 frames (one per file).
 */

int width, height, padded_width, padded_height, num_buffers;
Engine_Handle           engine    = NULL;
VIDDEC3_Handle          codec     = NULL;
VIDDEC3_Params         *params    = NULL;
VIDDEC3_DynamicParams  *dynParams = NULL;
VIDDEC3_Status         *status    = NULL;
XDM2_BufDesc           *inBufs    = NULL;
XDM2_BufDesc           *outBufs   = NULL;
VIDDEC3_InArgs         *inArgs    = NULL;
VIDDEC3_OutArgs        *outArgs   = NULL;

/* Padding for width as per Codec Requirement (for h264) */
#define PADX  32
/* Padding for height as per Codec requirement (for h264)*/
#define PADY  24

int output_allocate(XDM2_BufDesc *outBufs, int cnt,
        int width, int height, int stride)
{
    outBufs->numBufs = 2;

    outBufs->descs[0].memType = XDM_MEMTYPE_BO;
    outBufs->descs[1].memType = XDM_MEMTYPE_BO;

    return yuv_allocate(cnt, width, height, stride);
}

/* get file path.. return path is only valid until next time this is called */
static const char * get_path(const char *pattern, int cnt)
{
    static int len = 0;
    static char *path = NULL;

    /* TODO it would be better to not assume the pattern doesn't expand to
     * less than 10 chars it's original length..
     */
    if ((strlen(pattern) + 10) > len) {
        len  = strlen(pattern) + 10;
        path = realloc(path, len);
    }

    snprintf(path, len-1, pattern, cnt);

    return path;
}

/* helper to read one frame of input */
int read_input(const char *pattern, int cnt, char *input)
{
    int sz = 0, n = 0;
    const char *path = get_path(pattern, cnt);
    int fd = open(path, O_RDONLY);

    /* if we can't find the file, then at the end of stream */
    if (fd < 0) {
        return 0;
    }

    while ((n = read(fd, input, width * height)) > 0) {
        sz += n;
        input += n;
    }

    close(fd);

    return sz;
}

/* helper to write one frame of output */
int write_output(const char *pattern, int cnt, char *y, char *uv, int stride)
{
    int sz = 0, n, i;
    const char *path = get_path(pattern, cnt);
    int fd = open(path, O_WRONLY|O_CREAT, 0644);

    if (fd < 0) {
        ERROR("could open output file: %s (%d)", path, errno);
        return 0;
    }

    for (i = 0; i < height; i++) {
        char *p = y;
        int len = width;
        while (len && ((n = write(fd, p, len)) > 0)) {
            sz  += n;
            p   += n;
            len -= n;
        }
        y += stride;
    }

    for (i = 0; i < height/2; i++) {
        char *p = uv;
        int len = width;
        while (len && ((n = write(fd, p, len)) > 0)) {
            sz  += n;
            p   += n;
            len -= n;
        }
        uv += stride;
    }

    close(fd);

    return sz;
}

/* decoder body */
int main(int argc, char **argv)
{
    Engine_Error ec;
    XDAS_Int32 err;
    char *input = NULL;
    struct omap_bo *input_bo = NULL;
    char *in_pattern, *out_pattern;
    int in_cnt = 0, out_cnt = 0;
    int oned, stride;

    if ((argc >= 2) && !strcmp(argv[1],"-1")) {
        oned = TRUE;
        argc--;
        argv++;
    } else {
        oned = FALSE;
    }

    if (argc != 5) {
        printf("usage:   %s width height inpattern outpattern\n", argv[0]);
        printf("example: %s 320 240 in.%%d.h264 out.%%04d.yuv\n", argv[0]);
        return 1;
    }

    /* error checking? */
    width  = atoi(argv[1]);
    height = atoi(argv[2]);
    in_pattern  = argv[3];
    out_pattern = argv[4];

    DEBUG ("width=%d, height=%d", width, height);

    /* calculate output buffer parameters: */
    width  = ALIGN2 (width, 4);        /* round up to MB */
    height = ALIGN2 (height, 4);       /* round up to MB */
    padded_width  = ALIGN2 (width + (2*PADX), 7);
    padded_height = height + 4*PADY;
    num_buffers   = MIN(16, 32768 / ((width/16) * (height/16))) + 3;

    if (oned) {
        stride = padded_width;
    } else {
        stride = 4096;
    }

    DEBUG ("padded_width=%d, padded_height=%d, stride=%d, num_buffers=%d",
            padded_width, padded_height, stride, num_buffers);

    engine = Engine_open("ivahd_vidsvr", NULL, &ec);

    if (!engine) {
        ERROR("fail");
        goto out;
    }

    util_init();

    params = dce_alloc(sizeof(IVIDDEC3_Params));
    params->size = sizeof(IVIDDEC3_Params);

    params->maxWidth         = width;
    params->maxHeight        = height;
    params->maxFrameRate     = 30000;
    params->maxBitRate       = 10000000;
    params->dataEndianness   = XDM_BYTE;
    params->forceChromaFormat= XDM_YUV_420SP;
    params->operatingMode    = IVIDEO_DECODE_ONLY;
    params->displayDelay     = IVIDDEC3_DISPLAY_DELAY_AUTO;
    params->displayBufsMode  = IVIDDEC3_DISPLAYBUFS_EMBEDDED;
    params->inputDataMode    = IVIDEO_ENTIREFRAME;
    params->metadataType[0]  = IVIDEO_METADATAPLANE_NONE;
    params->metadataType[1]  = IVIDEO_METADATAPLANE_NONE;
    params->metadataType[2]  = IVIDEO_METADATAPLANE_NONE;
    params->numInputDataUnits= 0;
    params->outputDataMode   = IVIDEO_ENTIREFRAME;
    params->numOutputDataUnits = 0;
    params->errorInfoMode    = IVIDEO_ERRORINFO_OFF;

    codec = VIDDEC3_create(engine, "ivahd_h264dec", params);

    if (!codec) {
        ERROR("fail");
        goto out;
    }

    dynParams = dce_alloc(sizeof(IVIDDEC3_DynamicParams));
    dynParams->size = sizeof(IVIDDEC3_DynamicParams);

    dynParams->decodeHeader  = XDM_DECODE_AU;

    /*Not Supported: Set default*/
    dynParams->displayWidth  = 0;
    dynParams->frameSkipMode = IVIDEO_NO_SKIP;
    dynParams->newFrameFlag  = XDAS_TRUE;


    status = dce_alloc(sizeof(IVIDDEC3_Status));
    status->size = sizeof(IVIDDEC3_Status);

    err = VIDDEC3_control(codec, XDM_SETPARAMS, dynParams, status);
    if (err) {
        ERROR("fail: %d", err);
        goto out;
    }

    /* not entirely sure why we need to call this here.. just copying omx.. */
    err = VIDDEC3_control(codec, XDM_GETBUFINFO, dynParams, status);
    if (err) {
        ERROR("fail: %d", err);
        goto out;
    }

    inBufs = malloc(sizeof(XDM2_BufDesc));
    inBufs->numBufs = 1;
    input_bo = bitstream_alloc(width * height);
    input = omap_bo_map(input_bo);
    inBufs->descs[0].buf = (XDAS_Int8 *)omap_bo_handle(input_bo);
    inBufs->descs[0].memType = XDM_MEMTYPE_BO;

    outBufs = malloc(sizeof(XDM2_BufDesc));

    err = output_allocate(outBufs, num_buffers,
            padded_width, padded_height, stride);
    if (err) {
        ERROR("fail: %d", err);
        goto out;
    }

    inArgs = dce_alloc(sizeof(IVIDDEC3_InArgs));
    inArgs->size = sizeof(IVIDDEC3_InArgs);

    outArgs = dce_alloc(sizeof(IVIDDEC3_OutArgs));
    outArgs->size = sizeof(IVIDDEC3_OutArgs);

    while (inBufs->numBufs && outBufs->numBufs) {
        YUVBuffer *buf;
        int n, i;
        suseconds_t t;

        buf = yuv_get();
        if (!buf) {
            ERROR("fail: out of buffers");
            goto shutdown;
        }

        n = read_input(in_pattern, in_cnt, input);
        if (n) {
            inBufs->descs[0].bufSize.bytes = n;
            inArgs->numBytes = n;
            DEBUG("push: %d (%d bytes) (%p)", in_cnt, n, buf);
            in_cnt++;
        } else {
            /* end of input.. do we need to flush? */
            inBufs->numBufs = 0;
            inArgs->inputID = 0;
        }

        inArgs->inputID = (XDAS_Int32)buf;
        outBufs->descs[0].buf = (XDAS_Int8 *)omap_bo_handle(buf->y_bo);
        outBufs->descs[1].buf = (XDAS_Int8 *)omap_bo_handle(buf->uv_bo);

        t = mark(NULL);
        err = VIDDEC3_process(codec, inBufs, outBufs, inArgs, outArgs);
        DEBUG("processed returned in: %ldus", (long int)mark(&t));
        if (err) {
            ERROR("process returned error: %d", err);
            ERROR("extendedError: %08x", outArgs->extendedError);
            goto shutdown;
        }

        for (i = 0; outArgs->outputID[i]; i++) {
            /* calculate offset to region of interest */
            XDM_Rect *r = &(outArgs->displayBufs.bufDesc[0].activeFrameRegion);

            /* get the output buffer and write it to file */
            buf = (YUVBuffer *)outArgs->outputID[i];
            DEBUG("pop: %d (%p)", out_cnt, buf);
            write_output(out_pattern, out_cnt++, buf->y, buf->uv, stride);
        }

        for (i = 0; outArgs->freeBufID[i]; i++) {
            buf = (YUVBuffer *)outArgs->freeBufID[i];
            yuv_release(buf);
        }

        if (outArgs->outBufsInUseFlag) {
            DEBUG("TODO... outBufsInUseFlag"); // XXX
        }
    }

shutdown:

    VIDDEC3_delete(codec);

out:
    if (engine)         Engine_close(engine);
    if (params)         dce_free(params);
    if (dynParams)      dce_free(dynParams);
    if (status)         dce_free(status);
    if (inBufs)         free(inBufs);
    if (outBufs)        free(outBufs);
    if (inArgs)         dce_free(inArgs);
    if (outArgs)        dce_free(outArgs);
    if (input_bo)       omap_bo_del(input_bo);

    yuv_free();
    util_deinit();

    return 0;
}
