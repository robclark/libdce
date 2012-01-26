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

#include "util.h"

struct omap_device *dev;

void util_init(void)
{
    dev = dce_init();
}

void util_deinit(void)
{
    dce_deinit(dev);
}

struct omap_bo * bitstream_alloc(int sz)
{
    return omap_bo_new(dev, sz, OMAP_BO_WC);
}

/* list of free buffers, not locked by codec! */
static YUVBuffer *head = NULL;

int yuv_allocate(int cnt, int width, int height, int stride)
{
    while (cnt) {
        YUVBuffer *buf = calloc(sizeof(YUVBuffer), 1);

        if (stride != 4096) {
            buf->y_bo = omap_bo_new(dev, width*height, OMAP_BO_WC);
            buf->uv_bo = omap_bo_new(dev, width*height/2, OMAP_BO_WC);
        } else {
            buf->y_bo = omap_bo_new_tiled(dev, width, height,
                    OMAP_BO_WC|OMAP_BO_TILED_8);
            buf->uv_bo = omap_bo_new_tiled(dev, width, height/2,
                    OMAP_BO_WC|OMAP_BO_TILED_16);
        }

        buf->y  = omap_bo_map(buf->y_bo);
        buf->uv = omap_bo_map(buf->uv_bo);

        DEBUG("buf=%p, y=%p, uv=%p", buf, buf->y, buf->uv);

        buf->next = head;
        head = buf;

        cnt--;
    }

    return 0;

}

void yuv_free(void)
{
    YUVBuffer *buf = head;
    while ((buf=head)) {
        omap_bo_del(buf->y_bo);
        omap_bo_del(buf->uv_bo);
        head = buf->next;
        free(buf);
    }
}

YUVBuffer * yuv_get(void)
{
    YUVBuffer *buf = head;
    if (buf) {
        head = buf->next;
    }
    DEBUG("get: %p", buf);
    return buf;
}

void yuv_release(YUVBuffer *buf)
{
    DEBUG("release: %p", buf);
    buf->next = head;
    head = buf;
}
