nobase_dist_noinst_DATA = \
    tntnet/tntnet.xml.in \
    init.d/tntnet.in

nobase_sysconf_DATA = \
    tntnet/tntnet.xml \
    init.d/tntnet

tntnet/tntnet.xml: tntnet/tntnet.xml.in
	sed -e 's,@libdir\@,@libdir@,g;' \
	    -e 's,@pkglibdir\@,@pkglibdir@,g;' \
	    -e 's,@sysconfdir\@,@sysconfdir@,g;' \
	    -e 's,@localstatedir\@,@localstatedir@,g;' \
		-e 's,@datadir\@,@datadir@,g;' <tntnet/tntnet.xml.in >tntnet/tntnet.xml

init.d/tntnet: init.d/tntnet.in
	sed -e 's,@libdir\@,@libdir@,g;' \
	    -e 's,@pkglibdir\@,@pkglibdir@,g;' \
	    -e 's,@sysconfdir\@,@sysconfdir@,g;' \
	    -e 's,@localstatedir\@,@localstatedir@,g;' \
		-e 's,@datadir\@,@datadir@,g;' <init.d/tntnet.in >init.d/tntnet
