Format: 1.0
Source: tntnet
Binary: tntnet, tntnet-doc, tntnet-demos, libtntnet12, libtntnet-dev, tntnet-runtime, tntnet-dbg
Architecture: any all
Version: 2.2.1-1etn5
Maintainer: Kari Pahula <kaol@debian.org>
Homepage: http://www.tntnet.org/
Standards-Version: 3.9.5
Build-Depends: cdbs, debhelper (>= 9), libcxxtools-dev (>= 2.2.1), libltdl-dev, zip, zlib1g-dev, pkg-config, dh-autoreconf, libtool, libssl-dev
Package-List: 
 libtntnet-dev deb libdevel extra
 libtntnet12 deb libs extra
 tntnet deb httpd extra
 tntnet-demos deb doc extra
 tntnet-doc deb doc extra
 tntnet-runtime deb httpd extra
 tntnet-dbg deb debug extra
DEBTRANSFORM-TAR: tntnet-2.2.1.tar.gz
