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

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "wayland-drm-client-protocol.h"

#include <ti/dce/dce_priv.h>

struct display
{
    struct wl_display *display;
    struct wl_drm *drm;

    int error;
    int fd;
    int authenticated;
};


static void
drm_handle_device(void *data, struct wl_drm *drm, const char *device)
{
    struct display *d = data;
    drm_magic_t magic;

    /* only open the device if we don't already have an fd.. see
     * dce_set_fd().  Need to sort out a better way to handle this
     * but GEM buffer handles are only valid within the context of
     * a given file-open.  Switching to dmabuf or flink handles
     * would solve this.
     */
    if (d->fd == -1) {
        d->fd = open(device, O_RDWR | O_CLOEXEC);
        if (d->fd == -1) {
            ERROR("could not open %s: %m", device);
            d->error = -1;
            return;
        }
    }

    drmGetMagic (d->fd, &magic);
    wl_drm_authenticate (d->drm, magic);
}

static void
drm_handle_format(void *data, struct wl_drm *drm, uint32_t format)
{
}

static void
drm_handle_authenticated(void *data, struct wl_drm *drm)
{
    struct display *d = data;
    INFO("authenticated");
    d->authenticated = 1;
}

static const struct wl_drm_listener drm_listener = {
        drm_handle_device,
        drm_handle_format,
        drm_handle_authenticated
};


static void
display_handle_global (struct wl_display *display, uint32_t id,
        const char *interface, uint32_t version, void *data)
{
    struct display *d = data;

    DEBUG("global: %s", interface);

    if (strcmp(interface, "wl_drm") == 0) {
        d->drm = wl_display_bind(display, id, &wl_drm_interface);
        wl_drm_add_listener(d->drm, &drm_listener, d);
    }
}

int dce_auth_wayland(int *fd)
{
    struct display disp = {0};
    struct display *d = &disp;

    d->fd = *fd;

    INFO("attempting to open wayland connection");
    d->display = wl_display_connect(NULL);
    if (!d->display) {
        ERROR("Could not open display");
        return -1;
    }
    wl_display_add_global_listener (d->display,
            display_handle_global, d);

    while (!(d->authenticated || d->error)) {
        INFO("waiting for authentication");
        wl_display_roundtrip (d->display);
    }

    /* and then close display now that we are authenticated: */
    wl_drm_destroy (d->drm);
    wl_display_flush (d->display);
    wl_display_disconnect (d->display);

    if (!d->error)
        *fd = d->fd;

    return d->error;
}
