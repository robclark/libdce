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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

/* TODO.. a bit more cleanup so the decoder/encoder test part doesn't really
 * need to know about omapdrm..   it really just needs the int buffer handle
 * and (in bitstream case) the buffer ptr.. this should be all stuff it could
 * get from dmabuf..
 */
#include <omap_drm.h>
#include <omap_drmif.h>

#define ERROR(FMT,...)  printf("%s:%d:\t%s\terror: " FMT "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DEBUG(FMT,...)  printf("%s:%d:\t%s\tdebug: " FMT "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

/* align x to next highest multiple of 2^n */
#define ALIGN2(x,n)   (((x) + ((1 << (n)) - 1)) & ~((1 << (n)) - 1))

void util_init(void);
void util_deinit(void);

struct omap_bo * bitstream_alloc(int sz);

/* ************************************************************************* */
/* utilities to allocate/manage yuv buffers */

typedef struct YUVBuffer YUVBuffer;

struct YUVBuffer {
    struct omap_bo *y_bo, *uv_bo;
    char *y, *uv;
    YUVBuffer *next;      /* next free buffer */
};

int yuv_allocate(int cnt, int width, int height, int stride);
void yuv_free(void);
YUVBuffer * yuv_get(void);
void yuv_release(YUVBuffer *buf);


/* for timing decode time */
static suseconds_t mark(suseconds_t *last)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    if (last) {
        return t.tv_usec - *last;
    }
    return t.tv_usec;
}

#endif /* __UTIL_H__ */
