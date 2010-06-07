dist_sysconf_DATA = seweb.xml.in
dist_noinst_DATA = seweb.xml.in
sysconf_DATA = seweb.xml

seweb.xml: seweb.xml.in
	sed 's,@DBURL\@,@DBURL@,' <seweb.xml.in >seweb.xml
