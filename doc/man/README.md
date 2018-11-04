The man pages are maintained in markdown format and converted with go-md2man to
a man page.

To generate e.g. tntnet.xml.7 this command was used:

    go-md2man -in tntnet.xml.7.markdown -out tntnet.xml.7

To generate all man pages run this:

    for a in *.markdown; do go-md2man -in $a -out $(basename $a .markdown); done
