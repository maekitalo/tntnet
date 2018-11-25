ecppc 1 "2006-07-23" Tntnet "Tntnet users guide"
================================================

NAME
----

ecppc - compiler for ecpp(7)

SYNOPSIS
--------

`ecppc` [`-bhszvtM`] [`-s-`] [`-o` *filename*]  [`-n` *name*] [`-m` *mimetype*] [`--mimetypes` *filename*] [`-I` *dir*] [`-l` *log-category*] *filename*

`ecppc` `-bb` *filename* ...

`ecppc` [`OPTION`]

DESCRIPTION
-----------

`ecppc` is the compiler for the ecpp-language. `ecpp` is a template language,
which lets the user embed C++-code  into HTML  for  use  in  tntnet(8).
`ecppc` generates a C++-class from a ecpp template. It can also compile binary
data into a C++-class, which makes it possible to integrate them in a tntnet
application.

OPTIONS
-------

`-b`
  This enables binary-mode. Ecppc does not look for ecpp-tags, but creates a
  class, which just copies the data

`-bb`
  This enables multi-binary-mode. Every binary-file has some overhead, when
  packed into a tntnet-application. This overhead can be quite significant, when
  binary-files are small, like small icons in a web application. To reduce this
  overhead, multiple binaries can be packed into a single class, which removes
  the per-binary overhead completely.

  When the component is called, it uses the path-info-parameter
  (request.getPathInfo()) from the request, to decide, which binary to send. If
  no filename matches the path-info, processing is declined. The binaries need
  not be of same mime-type, since the mime-type is looked automatically from the
  mime-database by file- extension of the source-file.

`-i` *filename*
  In multi binary mode (option `-bb`) filenames can be read from the file
  specified with this option. This can be useful when the command line gets too
  long or just for convenience.

`-I` *dir*
  Search include-files in directory. This option can be passed multiple times.
  All specified directories are searched in turn for include-files.

`-l` *log-category*
  Set log category. Default is *component.componentname*.

`-L`
  Disable generation of #line-directives

`-m` *mimetype*
  Set mimetype of output. This is the mimetype, sent by the component to the
  browser in the Content-Type- header. Without this parameter the mimetype is
  looked up from the mime-database of your system using the file-extension of
  the source-file.

`--mimetypes` *file*
  Read mimetypes from file (default: /etc/mime.types).

`-M`
  This disables normal processing and prints just the ecpp-dependencies from
  this component. The output can be included into a Makefile. Ecpp-dependencies
  are introduces by the <%include>-tag.

`-C, --cmake`
  Prints ecpp dependencies in a syntax, which `cmake` understands.

`-n` *name*
  Set the name of the component. Normally this is derived from the
  source-file-name by removing the path and .ecpp-extension.

`-o` *filename*
  Write the generated file to the specified file instead of deriving the
  filename from the source-file-name.  The outputfilename is normally the
  source-file where the extension is replaced by .cpp.

`-p`
  Keep path name when deriving name of component from input file name.

`-s`
  Generate singleton. Normally ecppc decides automatically, if the template is
  suitable for a singleton.  This option force ecppc to generate a singleton.

`-s-`
  Do not generate a singleton.

`-v`
  Enable verbose mode. This prints additional information about the processing
  on the standard-output.

`-z`
  Compress the data in the component. Compressed data is automatically
  decopressed on first use. This reduces the code-size, but slightly slows down
  the first call of the component.
  
`-h, --help`
  display this information
  
`-V, --version`
  display program version

AUTHOR
------

This manual page was written by Tommi Mäkitalo <tommi@tntnet.org>.

SEE ALSO
--------

tntnet(8), ecpp(7)
