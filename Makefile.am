## Process this file with automake to produce Makefile.in

SUBDIRS                      = \
                               packages/codec_engine \
                               packages/ivahd_codecs \
                               packages/xdctools \
                               packages/xdais

lib_LTLIBRARIES              = libdce.la

CE_CFLAGS                    = \
                               -I$(top_srcdir)/ducati \
                               -I$(top_srcdir)/packages/codec_engine \
                               -I$(top_srcdir)/packages/ivahd_codecs \
                               -I$(top_srcdir)/packages/xdctools \
                               -I$(top_srcdir)/packages/xdais

libdce_la_SOURCES            = libdce.c
libdce_la_CFLAGS             = $(WARN_CFLAGS) $(CE_CFLAGS) $(DRM_CFLAGS) $(X11_CFLAGS) $(WAYLAND_CFLAGS)
libdce_la_LDFLAGS            = -no-undefined
libdce_la_LIBADD             = $(DRM_LIBS) $(X11_LIBS) $(WAYLAND_LIBS)

if ENABLE_X11
libdce_la_SOURCES           += libdce-x11.c
endif
if ENABLE_WAYLAND
libdce_la_SOURCES           += libdce-wayland.c wayland-drm-protocol.c
endif

libdce_la_includedir         = $(includedir)/dce/
libdce_la_include_HEADERS    = dce.h

pkgconfig_DATA               = libdce.pc
pkgconfigdir                 = $(libdir)/pkgconfig
