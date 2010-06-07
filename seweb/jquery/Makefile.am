lib_LTLIBRARIES = libtnt-jquery-ui-1.8.2.la

libtnt_jquery_ui_1_8_2_la_SOURCES = jquery.cpp
libtnt_jquery_ui_1_8_2_la_LIBADD = -ltntnet

jquery.cpp: jquery-ui-1.8.2.custom.zip
	mkdir tmp
	(cd tmp; unzip ../jquery-ui-1.8.2.custom.zip)
	$(ECPPC) -bb -o jquery.cpp -n jquery -z -p -Itmp `find tmp -type f|sed 's,^tmp/,,'`

clean-local:
	-rm -rf tmp jquery.cpp
