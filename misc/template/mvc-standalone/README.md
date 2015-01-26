Tntnet project @PROJECT@
===========================

This is a tntnet web project, which follows the mvc pattern and runs as a
standalone application.

Autoconf, automake and libtool, commonly known as the autotools, is used for
building the application.

Building
--------

The `autotools` are set up and `autoreconf -i` is run already.

To build run `./configure && make`. To rebuild after changing code run just
`make`.

To add new source files to the project add them to `Makefile.am`.

Static files are put into the `resources` directory and added to the
`staticSources` variable in `Makefile.am`.

New pages are added to the `view` directory. Those components are embedded into
the html page found in `webmain.ecpp`.

Html fragments, which are loaded with ajax are created in the `html` directory.
They get the ecpp extension but are requested with the filename and the
extension .html appended.

e.g. the url `/foo.html` calls the component `html/foo.ecpp`.

Json requests are created in the `json` directory. A example file can be found
there. Those files are requested like html fragments but with the extension
.json appended.

Other directories may be created and added to the mapping found in `main.cpp`.

Deployment
----------

To deploy a application add your e-mail address into configure.ac and possibly
change the version number.

Calling `make dist` creates a tar.gz archive with source of the application.

Building that application autotools are not needed any more.
