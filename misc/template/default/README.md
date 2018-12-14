Tntnet project @PROJECT@
===========================

This is a simple tntnet web project, which runs as a standalone application.

Autoconf, automake and libtool, commonly known as the autotools, is used for
building the application.

Building
--------

The `autotools` are set up and `autoreconf -i` is run already.

To build run `./configure && make`. To rebuild after changing code run just
`make`.

To add new source files to the project add them to `Makefile.am`.

Adding files
------------
When you create ecpp files, don't forget to add them to `Makefile.am`.

Static files belong to to the _resources_ directory and also to `Makefile.am`.

Deployment
----------

To deploy a application add your e-mail address into configure.ac and possibly
change the version number.

Calling `make dist` creates a tar.gz archive with source of the application.

Building that application autotools are not needed any more.
