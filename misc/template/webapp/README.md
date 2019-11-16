Tntnet project @PROJECT@
===========================

This is a tntnet web application, which runs as a single page application.
Changing pages is done my loading html fragments using ajax calls and placing
in the content div. Data is typically loaded using json.

Autoconf, automake and libtool, commonly known as the autotools, is used for
building the application.

The project uses jquery for manipulating the DOM and executing ajax calls.
Require.js is used for handling js modules. Noty.js is used for notifications.

Building
--------

The `autotools` are set up and `autoreconf -i` is run already.

To build run `./configure && make`. To rebuild after changing code run just
`make`.

To add new source files to the project add them to `Makefile.am`.

Directory structure and url mapping
-----------------------------------

Static files are put into the `resources` directory and added to the
`staticSources` variable in `Makefile.am`.

Dynamic files are put into sub directories in the source directory. The name of
the sub directory is used as the extension in the url. The actual file gets the
extension `.ecpp`. Add the files to the variable `ecppSources` in the top level
`Makefile.am`. They are compiled with ecpp and the code there is executed at
runtime.

e.g. the url `/foo.json` calls the component `json/foo.ecpp`.
