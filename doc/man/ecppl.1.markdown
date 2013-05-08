ecppl 1 "2006-07-23" Tntnet "Tntnet users guide"
================================================

NAME
----

ecppl - language-extractor for ecpp(7)

SYNOPSIS
--------

`ecppl` [`-I` *dir*] [`-ln`] [`-o` *output-filename*] *ecpp-filename* ...


DESCRIPTION
-----------

`ecppl` is the language extractor for ecpp. Ecpp - the template language used
with tntnet - supports internationalized applications. In ecpp templates a tag
&lt;i18n&gt; changes the meaning of curly braces. A phrase, which is enclosed in curly
braces, can be translated. At runtime the phrase is looked up in a
language-library. This mode can be quit with the tag &lt;/i18n&gt;. Phrases must not
have newlines or tabs.

Every phrase, which is marked as translatable, is extracted with ecppl and
written to standard output or to a specified output-filename line by line.

OPTIONS
-------

`-I` *dir*
  Search include-files in directory. This option can be passed multiple times.
  All specified directories are searched in turn for include-files.

`-l`
  Extract language-phrases (the default)

`-n`
  Extract non-language-phrases

`-o` *filename*
  Specify output filename

AUTHOR
------

This manual page was written by Tommi Mäkitalo <tommi@tntnet.org>.

SEE ALSO
--------

tntnet(1), ecpp(7), ecppll(1).
