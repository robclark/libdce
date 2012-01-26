## Process this file with automake to produce Makefile.in

CE_CFLAGS                    = \
                               -I$(top_srcdir) \
                               -I$(top_srcdir)/ducati \
                               -I$(top_srcdir)/packages/codec_engine \
                               -I$(top_srcdir)/packages/ivahd_codecs \
                               -I$(top_srcdir)/packages/xdctools \
                               -I$(top_srcdir)/packages/xdais

lib_LTLIBRARIES              = libtestdec.la

libtestdec_la_SOURCES        = viddec3.c util.c
libtestdec_la_CFLAGS         = $(CE_CFLAGS) $(DRM_CFLAGS)
libtestdec_la_LIBADD         = $(DRM_LIBS)

# TODO add encoder test..