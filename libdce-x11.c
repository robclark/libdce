/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
#include <xf86drm.h>
#include <omap_drm.h>
#include <omap_drmif.h>

#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/extensions/dri2proto.h>
#include <X11/extensions/dri2.h>

#include <ti/dce/dce_priv.h>

int dce_auth_x11(int fd)
{
    Display *dpy;
    Window root;
    drm_magic_t magic;
    int eventBase, errorBase, major, minor;
    char *driver, *device;

    INFO("attempting to open X11 connection");
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        ERROR("Could not open display");
        goto no_x11;
    }

    if (!DRI2InitDisplay(dpy, NULL)) {
        ERROR("DRI2InitDisplay failed");
        goto no_x11;
    }

    if (!DRI2QueryExtension(dpy, &eventBase, &errorBase)) {
        ERROR("DRI2QueryExtension failed");
        goto no_x11;
    }

    DEBUG("DRI2QueryExtension: eventBase=%d, errorBase=%d", eventBase, errorBase);

    if (!DRI2QueryVersion(dpy, &major, &minor)) {
        ERROR("DRI2QueryVersion failed");
        goto no_x11;
    }

    DEBUG("DRI2QueryVersion: major=%d, minor=%d", major, minor);

    root = RootWindow(dpy, DefaultScreen(dpy));

    if (!DRI2Connect(dpy, root, DRI2DriverDRI, &driver, &device)) {
        DEBUG("DRI2Connect failed");
        goto no_x11;
    }

    DEBUG("DRI2Connect: driver=%s, device=%s", driver, device);

    /* only open the device if we don't already have an fd.. see
     * dce_set_fd().  Need to sort out a better way to handle this
     * but GEM buffer handles are only valid within the context of
     * a given file-open.  Switching to dmabuf or flink handles
     * would solve this.
     */
    if (fd == -1) {
        fd = open(device, O_RDWR);
        if (fd < 0) {
            ERROR("open failed");
            goto no_x11_free;
        }
    }

    if (drmGetMagic(fd, &magic)) {
        ERROR("drmGetMagic failed");
        goto no_x11_free;
    }

    if (!DRI2Authenticate(dpy, root, magic)) {
        ERROR("DRI2Authenticate failed");
        goto no_x11_free;
    }

no_x11_free:
    XFree(driver);
    XFree(device);
no_x11:
    return fd;
}
