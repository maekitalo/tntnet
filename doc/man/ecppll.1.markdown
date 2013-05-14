ecppll 1 "2006-07-23" Tntnet "Tntnet users guide"
================================================

NAME
----

ecppll - language-linker for ecpp

SYNOPSIS
--------

`ecppll` [`-I` *dir*] [`-F`] [`-o` *output-filename*] *ecpp-filename* [`translated-data`]

DESCRIPTION
-----------

Ecppll is the language-linker for ecpp. This program combines the data of a
ecpp-file with translated phrases from a source-file and generates a data-file
used by tntnet to look up translated data. The translated data file must contain
the original phrase and the translated version separated by tab line by line.

To generate a language file the generated files of all components of a
component-library must be zipped. The resulting zip-file must have the same
basename as the component-library and the language-code as the extension. Tntnet
tries to find the langage library by first appending the value of the query
parameter LANG to the name of the componentlibrary. If the file is not found,
the environment-variable LANG is used. If that is neither found, the default
data from the original component is used.

OPTIONS
-------

`-F`
  Fail on warning. If a phrase is not found in the language-data or in
  language-data is a phrase not used it the ecpp-file, ecppll warns about it to
  standard output. If this flag is set, ecppll returns a non-zero return-code.

`-I` *dir*
  Search include-files in directory. This option can be passed multiple times.
  All specified directories are searched in turn for include-files.

`-o` *filename*
  Specify output filename

AUTHOR
------

This manual page was written by Tommi Mäkitalo <tommi@tntnet.org>.

SEE ALSO
--------

tntnet(8), ecpp(7), ecppl(1)
