#! /bin/sh
# Regenerate the files autoconf / automake

libtoolize --force --automake || libtool

rm -f config.cache
rm -f config.log
aclocal -I m4
autoheader
autoconf
automake -a
