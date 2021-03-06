﻿Tntnet quick start guide
========================

Authors: Tommi Mäkitalo, Andreas Welchlin


This quick start guide includes:

 * How to install tntnet
 * Build and run your first application
 * Explanation of this first application
 * Further reading

Tntnet is developed and tested on GNU/Linux. It is known to run on Sun Solaris,
IBM AIX and FreeBSD.

Installation
------------

You can install through the package manager in your operating system, if those packages are outdated (< cxxtools, tntnet 2.3), follow this instruction:

To install Tntnet you will first need to install cxxtools.

You can find cxxtools on the tntnet [homepage]
(http://www.tntnet.org/download.html) and install it with:

    $ tar xzf cxxtools-x.x.tar.gz
    $ cd cxxtools-2.x
    $ ./configure
    $ make
    $ sudo make install
    $ sudo ldconfig

The same installation procedure is used for tntnet. Install it with:

    $ tar xzf tntnet-x.x.tar.gz
    $ cd tntnet-x.x
    $ ./configure
    $ make
    $ sudo make install
    $ sudo ldconfig

Now you have a working Tntnet environment.

How to create your first web application
----------------------------------------

To create a web application we need to create a project. The easiest way is to
use the helper script `tntnet-project`. We execute on the command line:

    $ tntnet-project myfirstproject

This creates a initial web project, which uses autotools as a build system. It
is created in a directory named myfirstproject. The most interesting files,
which are created are:

 * configure.ac                 - the configuration file for autoconf
 * Makefile.am                  - the rules to build the project with automake
 * main.cpp                     - the file with the `main` function
 * myfirstproject.ecpp          - our fist web page
 * log.properties               - configuration file for logging
 * resources/myfirstproject.css - a static file of our web application

For quick information about autotools using, read this mini howto:
http://www.niksula.hut.fi/~mkomu/docs/autohowto.html


To build and execute your first application enter the following commands:

    $ cd myfirstproject
    $ make
    $ tntnet

Now you can start your web browser and navigate to
`http://localhost:8000/myfirstproject`.

You can see the result of your first running tntnet application, which prints
the name of the application.

What have we done?
------------------

The source file myfirstproject.ecpp has been translated to C++. This C++ program
was used to build a shared library which contains the whole web application.

A tntnet web application is a simple web page with special tags like `<$ ...$>`.
The ecpp compiler `ecppc` creates a C++ source file and a header file
with the same base name. These contain a class which has also the same name as
the file.  You can look into the generated code if you want, and sometimes it
is useful to read it for further understanding of tntnet applications. If the
C++ compiler has problems with your application it is a good idea to first
check the generated code.

The tags `<$ ... $>` output a C++ expression. The result of the expression is
printed into the resulting page when the page is requested (on runtime).  For
this purpose, a `std::ostream` is used, so that the type of the result can be
any object, which has an output operator `operator<<(ostream&, T)` defined.

The configuration file `tntnet.xml` contains the settings. The two most
important ones are the definitions of the listeners and URL mappings.

The listeners define, on which interface tntnet waits for requests. You define
an IP address of the local interface and a port. The IP address may be empty or
omitted. If omitted tntnet listens on all local interfaces.

The mappings tell tntnet what to do with incoming requests. Without this entry
tntnet answers every request with `http error 404 – not found`. A mapping maps
the URL - which is sent from a web browser - to a tntnet component. A component
is the piece of code, which is normally generated by the ecpp compiler (ecppc).

That's what we did above with `myfirstproject.ecpp`. Components are identified
by their (class) name and the shared library which contains this class. We
named our class "myfirstproject" and our shared library "myfirstproject.so".
The component identifier is then `myfirstproject@myfirstproject`.

So the mapping tells tntnet to call this component, when the URL `/test.html` is
requested.

How to add static files to your web application
-----------------------------------------------
Web applications typically contain static and dynamic content. Static files are
images, css or javascript source files.

In the example application we already have a css file. All static files are
located in the resources directory. When you add new files, add the to the top
level `Makefile.am` under the variable `staticSources` also so that they are
compiled into the web application.

All files specified this way are sent to the browser as is. No ecpp processing
takes place.

After adding the files you have to compile the application and rerun it. Also
after each change of a static file the application must be recompiled. Note also
that the browser may cache static files so that you may need to deactivate or
clear the cache to get the changed pages.

Adding dynamic files
--------------------
Dynamic files are compiled into components and may contain all ecpp tags
documented in the ecpp(7) man page.

Put them into your project and add them to the `ecppSources` variable into the
`Makefile.am`.

Here also a recompile and rerun is needed after every change.

Where to go from here
---------------------
The tool `tntnet-project` creates a project template. You may specify a
different template for more sophisticated applications. Try `tntnet-project -h`
to get help and `tntnet-project -l` to list the available templates. The
templates contain a REAME.md, which shortly describes the structure of the
created projects.
