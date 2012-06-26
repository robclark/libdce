AC_PREREQ(2.52)

dnl initialize autoconf
AC_INIT([libdce], [1.0.0], [http://www.ti.com])

dnl define PACKAGE_VERSION_* variables
dnl AS_VERSION

dnl can autoconf find the source ?
AC_CONFIG_SRCDIR([libdce.c])

dnl set the autotools aux directory
AC_CONFIG_AUX_DIR([config])

dnl AM_MAINTAINER_MODE only provides the option to configure to enable it
AM_MAINTAINER_MODE

dnl sets host_*, target_ variables
AC_CANONICAL_SYSTEM

dnl initialize automake
AM_INIT_AUTOMAKE([foreign])

dnl use pretty build output with automake >= 1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
  [AM_DEFAULT_VERBOSITY=1
   AC_SUBST(AM_DEFAULT_VERBOSITY)])

dnl find a compiler
AC_PROG_CC
AM_PROG_AS
AM_PROG_LIBTOOL

dnl *** autotools stuff ****    
dnl allow for different autotools
dnl AS_AUTOTOOLS_ALTERNATE

dnl *** checks for platform ***
AC_DEFINE_UNQUOTED(CPU_`echo $target_cpu | tr a-z- A-Z_`,[1])
AC_DEFINE_UNQUOTED(OS_` echo $target_os  | tr a-z- A-Z_`,[1])

dnl *** checks for programs ***
AC_PROG_INSTALL

dnl Check for pkgconfig first
AC_CHECK_PROG([HAVE_PKGCONFIG], [pkg-config], [yes], [no])

dnl *** checks for libraries ***
dnl Check for libdrm
PKG_CHECK_MODULES(DRM, libdrm libdrm_omap)

dnl Check for X11/libdri2
PKG_CHECK_MODULES(X11, x11 dri2, [HAVE_X11=yes], [HAVE_X11=no])
if test "x$HAVE_X11" = "xyes"; then
  AC_DEFINE(HAVE_X11, 1, [Have X11 support])
else
  AC_MSG_WARN([Building without X11 support])
fi

dnl *** checks for header files ***
dnl check if we have ANSI C header files
AC_HEADER_STDC

dnl *** checks for types/defines ***

dnl *** checks for structures ***

dnl *** checks for compiler characteristics ***

dnl *** checks for library functions ***

dnl *** checks for dependancy libraries ***

dnl *** set variables based on configure arguments ***


AC_CONFIG_FILES([
libdce.pc
Makefile
packages/codec_engine/Makefile
packages/ivahd_codecs/Makefile
packages/xdais/Makefile
packages/xdctools/Makefile
])
AC_OUTPUT
