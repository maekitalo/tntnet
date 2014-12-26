tntnet-project 1 "2014-09-09" Tntnet "Tntnet users guide"
========================================================

NAME
----
tntnet-project - project builder for tntnet(8)

SYNOPSIS
--------
*tntnet-project* [`OPTIONS`] <*project name*>

*tntnet-project* [`-t <type>`] [`--type=<type>`] [`-m <model>`] [`--model=<model>`] <*project name*>

DESCRIPTION
-----------
This manual page documents the `tntnet-project` command.

`tntnet-project` creates projects for tntnet(8).

OPTIONS
-------
This programs options start with one dash (`-`) or two dashes (`--`). A summary
of options is included below.

`-t <type>, --type=<type>`

  specifies application type

`-m <model>, --model=<model>`

  specifies application model

### Types:

`classic`
  create a simple application with one page (default)

`mvc`
  create a application with the mvc pattern

### Models:

`standalone`
  create a standalone web application (default)

`module`
  create a module to be loaded into the tntnet application server

EXAMPLES
--------
*tntnet-project* *myApp*

  create a *default* tntnet project

*tntnet-project* `-t mvc` *myApp*

*tntnet-project* `--type=mvc` *myApp*

  create a tntnet *standalone application* project with *mvc* pattern

*tntnet-project* `-m module` *myApp*

*tntnet-project* `--model=module` *myApp*

  create a tntnet *application server* project

*tntnet-project* `-t mvc -m module` *myApp*

*tntnet-project* `--type=mvc --model=module` *myApp*

  create a *mvc* pattern *application server* project

AUTHOR
------
tntnet was written by Tommi Mäkitalo <tommi@tntnet.org>.

This manual page was written by Ralf Schülke <ralf.schuelke@gmail.com>

SEE ALSO
--------
tntnet(8), ecppc(1), tntnet.xml(7).

More documentation can be found in /usr/share/doc/tntnet-doc/.
