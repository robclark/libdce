#! /bin/sh

cd `dirname $0`

# on some platforms, you have "g" versions of some of these tools instead,
# ie glibtoolize instead of libtoolize..
find_tool() {
	which $1 2> /dev/null || which g$1 2> /dev/null
}

aclocal=`find_tool aclocal`
libtoolize=`find_tool libtoolize`
automake=`find_tool automake`
autoconf=`find_tool autoconf`

mkdir -p config && $aclocal && $libtoolize --copy --force && $automake --copy --add-missing --foreign && $autoconf

test -n "$NOCONFIGURE" && {
  echo "skipping configure stage as requested."
  echo "autogen.sh done."
  exit 0
}

CONFIGURE_DEF_OPT="--enable-maintainer-mode"
echo ./configure $CONFIGURE_DEF_OPT $*
./configure $CONFIGURE_DEF_OPT $* || {
        echo "  configure failed"
        exit 1
}

echo "Now type 'make' to compile"

