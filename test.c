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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

#include <tilermem.h>
#include <memmgr.h>
#include <xdc/std.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/video3/viddec3.h>

#include "dce.h"

#define ERROR(FMT,...)  printf("%s:%d:\t%s\terror: " FMT "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DEBUG(FMT,...)  printf("%s:%d:\t%s\tdebug: " FMT "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

/* align x to next highest multiple of 2^n */
#define ALIGN2(x,n)   (((x) + ((1 << (n)) - 1)) & ~((1 << (n)) - 1))


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

/*! Padding for width as per Codec Requirement */
#define PADX  32
/*! Padding for height as per Codec requirement */
#define PADY  24

static void * tiler_alloc(int width, int height)
{
    int dimensions;
    MemAllocBlock block[2] = {0};

    if (!height) {
        /* 1d allocation: */
        dimensions = 1;
        block[0].pixelFormat = PIXEL_FMT_PAGE;
        block[0].dim.len = width;
    } else {
        /* 2d allocation: */
        dimensions = 2;
        block[0].pixelFormat = PIXEL_FMT_8BIT;
        block[0].dim.area.width  = width;
        block[0].dim.area.height = height;
        block[1].pixelFormat = PIXEL_FMT_16BIT;
        block[1].dim.area.width  = width;
        block[1].dim.area.height = height / 2;
    }

    return MemMgr_Alloc(block, dimensions);
}

/* ************************************************************************* */
/* utilities to allocate/manage 2d output buffers */

typedef struct OutputBuffer OutputBuffer;

struct OutputBuffer {
    char *buf;     /* virtual address for local access, 4kb stride */
    SSPtr y, uv;   /* physical addresses of Y and UV for remote access */
    OutputBuffer *next;      /* next free buffer */
};

/* list of free buffers, not locked by codec! */
static OutputBuffer *head = NULL;

int output_allocate(int cnt, int width, int height)
{
    while (cnt) {
        OutputBuffer *buf = calloc(sizeof(OutputBuffer), 1);

        buf->buf = tiler_alloc(width, height);
        buf->y   = TilerMem_VirtToPhys(buf->buf);
        buf->uv  = TilerMem_VirtToPhys(buf->buf + (height * 4096));

        DEBUG("buf=%p, y=%08x, uv=%08x", buf, buf->y, buf->uv);

        buf->next = head;
        head = buf;

        cnt--;
    }

    return 0;
}

void output_free(void)
{
    OutputBuffer *buf = head;
    while ((buf=head)) {
        MemMgr_Free(buf->buf);
        head = buf->next;
        free(buf);
    }
}

OutputBuffer * output_get(void)
{
    OutputBuffer *buf = head;
    if (buf) {
        head = buf->next;
    }
    DEBUG("get: %p", buf);
    return buf;
}

void output_release(OutputBuffer *buf)
{
    DEBUG("release: %p", buf);
    buf->next = head;
    head = buf;
}

/* ************************************************************************* */

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
int write_output(const char *pattern, int cnt, char *y, char *uv)
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
        y += 4096;
    }

    for (i = 0; i < height/2; i++) {
        char *p = uv;
        int len = width;
        while (len && ((n = write(fd, p, len)) > 0)) {
            sz  += n;
            p   += n;
            len -= n;
        }
        uv += 4096;
    }

    close(fd);

    return sz;
}

/* for timing decode time */
suseconds_t mark(suseconds_t *last)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    if (last) {
        return t.tv_usec - *last;
    }
    return t.tv_usec;
}

/* decoder body */
int main(int argc, char **argv)
{
    Engine_Error ec;
    XDAS_Int32 err;
    char *input = NULL;
    char *in_pattern, *out_pattern;
    int in_cnt = 0, out_cnt = 0;

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

    DEBUG ("padded_width=%d, padded_height=%d, num_buffers=%d",
            padded_width, padded_height, num_buffers);

    engine = Engine_open("ivahd_vidsvr", NULL, &ec);

    if (!engine) {
        ERROR("fail");
        goto out;
    }

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

    inBufs = dce_alloc(sizeof(XDM2_BufDesc));
    inBufs->numBufs = 1;
    input = tiler_alloc(width * height, 0);
    inBufs->descs[0].buf = (XDAS_Int8 *)TilerMem_VirtToPhys(input);
    inBufs->descs[0].memType = XDM_MEMTYPE_RAW;

    outBufs = dce_alloc(sizeof(XDM2_BufDesc));
    outBufs->numBufs = 2;
    outBufs->descs[0].memType = XDM_MEMTYPE_TILED8;
    outBufs->descs[0].bufSize.tileMem.width  = padded_width;
    outBufs->descs[0].bufSize.tileMem.height = padded_height;
    outBufs->descs[1].memType = XDM_MEMTYPE_TILED16;
    outBufs->descs[1].bufSize.tileMem.width  = padded_width; /* UV interleaved width is same a Y */
    outBufs->descs[1].bufSize.tileMem.height = padded_height / 2;

    err = output_allocate(num_buffers, padded_width, padded_height);
    if (err) {
        ERROR("fail: %d", err);
        goto out;
    }

    inArgs = dce_alloc(sizeof(IVIDDEC3_InArgs));
    inArgs->size = sizeof(IVIDDEC3_InArgs);

    outArgs = dce_alloc(sizeof(IVIDDEC3_OutArgs));
    outArgs->size = sizeof(IVIDDEC3_OutArgs);

    while (inBufs->numBufs && outBufs->numBufs) {
        OutputBuffer *buf;
        int n, i;
        suseconds_t t;

        buf = output_get();
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
        outBufs->descs[0].buf = (XDAS_Int8 *)buf->y;
        outBufs->descs[1].buf = (XDAS_Int8 *)buf->uv;

        t = mark(NULL);
        err = VIDDEC3_process(codec, inBufs, outBufs, inArgs, outArgs);
        DEBUG("processed returned in: %dus", (int)mark(&t));
        if (err) {
            ERROR("process returned error: %d", err);
            ERROR("extendedError: %08x", outArgs->extendedError);
            goto shutdown;
        }

        for (i = 0; outArgs->outputID[i]; i++) {
            /* calculate offset to region of interest */
            XDM_Rect *r = &(outArgs->displayBufs.bufDesc[0].activeFrameRegion);
            int yoff  = (r->topLeft.y * 4096) + r->topLeft.x;
            int uvoff = (r->topLeft.y * 4096 / 2) + r->topLeft.x;

            /* get the output buffer and write it to file */
            buf = (OutputBuffer *)outArgs->outputID[i];
            DEBUG("pop: %d (%p)", out_cnt, buf);
            write_output(out_pattern, out_cnt++, buf->buf + yoff,
                    buf->buf + uvoff + 4096 * padded_height);
        }

        for (i = 0; outArgs->freeBufID[i]; i++) {
            buf = (OutputBuffer *)outArgs->freeBufID[i];
            output_release(buf);
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
    if (inBufs)         dce_free(inBufs);
    if (outBufs)        dce_free(outBufs);
    if (inArgs)         dce_free(inArgs);
    if (outArgs)        dce_free(outArgs);
    if (input)          MemMgr_Free(input);

    output_free();

    return 0;
}
