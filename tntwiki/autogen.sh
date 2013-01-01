#! /bin/sh
# Regenerate the files autoconf / automake

echo execute libtoolize
libtoolize --force --automake

rm -f config.cache config.log

echo execute aclocal
aclocal -I m4

echo execute autoheader
autoheader

echo execute autoconf
autoconf

echo execute automake
automake -a
