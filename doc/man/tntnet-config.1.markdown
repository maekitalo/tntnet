tntnet-config 1 "2006-07-23" Tntnet "Tntnet users guide"
========================================================



NAME
----
tntnet-config - output compiler flags for tntnet(8) usage

SYNOPSIS
--------
`tntnet-config` [`--libs`] [`--cxxflags`] [`--config`[=*app*]] [`--makefile`[=*app*]] [`--project`=*app*] [`--auto-project`=*app*] [`--help`] [`--version`]

DESCRIPTION
-----------
This manual page documents briefly the tntnet-config command.

OPTIONS
-------
This program's options start with two dashes ('-'). A summary of options is
included below.

`--libs`
  Output linker flags.

`--cxxflags`
  Output C++ preprocessor and compiler flags.

`--config`[=`app`]
  Print default configuration file.

`--makefile`[=*app*]
  Print a simple makefile for an ecpp project.

`--project`=*app*
  Create a simple ecpp project directory.

`--autoproject`=*app*
  Create a autotools based ecpp project.

`--help`
  Show summary of options.

`--version`
  Show version of program.

AUTHOR
------
tntnet was written by Tommi Mäkitalo <tommi@tntnet.org>.

This manual page was written by Kari Pahula <kaol@debian.org>, for the Debian
project (but may be used by others).

SEE ALSO
--------
tntnet(8), ecppc(1), ecppl(1), ecppll(1).

More documentation can be found in /usr/share/doc/tntnet-doc/.
