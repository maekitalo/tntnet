tntnet 8 "2006-07-23" Tntnet "Tntnet users guide"
================================================

NAME
----

tntnet - web application server for c++

SYNOPSIS
--------
`tntnet` [`-c` *file*] [`--logall`]

`tntnet`-C [`-q` *query-string*] *componentId* ...

DESCRIPTION
-----------
This manual page documents briefly the `tntnet` command.

`tntnet` is a web server that can generate dynamic content via precompiled C++ modules.

OPTIONS
-------
A summary of options is included below.

`-c` *file*
  Use file as the configuration file. If the -c is omitted, the first nonoption
  argument is used as the config file.

`--logall`
  Initialize logging earlier. Normally logging is initialized after starting the
  listeners and changing user so that logfiles are created with the right owner.
  For debugging reasons this option forces to do that earlier.

`-C`
  Call components and print the result to stdout instead of starting tntnet as a webserver

`-q` *query-string*
  In -C mode pass this query string to the component. The option may be
  repeated multiple times to pass name-value-pairs easily.

EXAMPLE
-----------
`tntnet` `-C` `-q` `arg1=35` `-q` `arg2=7` `-q` `op=/` *calc@calc*

  calls the component *calc@calc* with query parameters and prints the result to stdout.

FILES
-----------
*/etc/tntnet/tntnet.xml*
  The default configuration file.

ENVIRONMENT
-----------
`TNTNET_CONF`
  This is checked for the configuration file name if it is not specified on the
  command line.

AUTHOR
------
tntnet was written by Tommi Mäkitalo <tommi@tntnet.org>.

This manual page was written by Kari Pahula <kaol@debian.org>, for the Debian
project (but may be used by others).

SEE ALSO
--------
tntnet-project(1), ecpp(7), ecppc(1), tntnet.xml(7), tntnet-defcomp(1)
[Tntnet homepage] (http://www.tntnet.org). More
documentation can be found in /usr/share/doc/tntnet-doc/.



Tntnet July 23, 2006 tntnet(8)
