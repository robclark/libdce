## Process this file with automake to produce Makefile.in

SUBDIRS                      = \
                               packages/codec_engine \
                               packages/ivahd_codecs \
                               packages/xdctools \
                               packages/xdais

lib_LTLIBRARIES              = libdce.la

CE_CFLAGS                    = \
                               -I$(top_srcdir)/packages/codec_engine \
                               -I$(top_srcdir)/packages/ivahd_codecs \
                               -I$(top_srcdir)/packages/xdctools \
                               -I$(top_srcdir)/packages/xdais

libdce_la_SOURCES            = dce.c
libdce_la_CFLAGS             = -DCLIENT=1 $(WARN_CFLAGS) $(CE_CFLAGS) \
                               $(SYSLINK_CFLAGS) \
                               $(MEMMGR_CFLAGS)
libdce_la_LDFLAGS            = -no-undefined
libdce_la_LIBADD             = $(SYSLINK_LIBS) $(MEMMGR_LIBS) $(D2CMAP_LIBS)

libdce_la_includedir         = $(includedir)/dce/
libdce_la_include_HEADERS    = dce.h

bin_PROGRAMS                 = dcetest
dcetest_SOURCES              = test.c
dcetest_CFLAGS               = $(CE_CFLAGS) $(MEMMGR_CFLAGS)
dcetest_LDADD                = libdce.la

pkgconfig_DATA               = libdce.pc
pkgconfigdir                 = $(libdir)/pkgconfig
