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

Configuration
-------------

The application has a xml configuration file, which is a extended tntnet.xml.
See the man page tntnet.xml(7) for the content.

You can extend the configuration by extending the class `Configuration` in
`configuration.h` and `configuration.cpp`. Two additional configuration
parameters are there as examples but also for use.

The variable _htdocs_ specifies the source for static files for runtime. If set,
the application looks for static files in that directory before looking into
compiled resources. Static resources should be compiled into the binary using
the _staticSources_ entry in _Makefile.am_. For development time it is
frequently convenient to load the static resources from the file system, so that
changing the resources do not need a compile cycle.

The other variable _dburl_ can be used to configure a database connection. The
configuration file already has some examples for database urls for _tntdb_. To
actually use it, you need to uncomment the tntdb check in _configure.ac_ and add
the link flag _-ltntdb_ into Makefile.am. Then you can get a database connection
using this in your code:

    tntdb::Connection conn = tntdb::connectCached(Configuration::it().dburl());

