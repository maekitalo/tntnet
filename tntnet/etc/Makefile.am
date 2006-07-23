nobase_dist_noinst_DATA = \
    tntnet/tntnet.conf.in \
    init.d/tntnet.in

nobase_sysconf_DATA = \
    tntnet/tntnet.conf \
    init.d/tntnet

nobase_dist_sysconf_DATA = \
    tntnet/mime.conf

nobase_nodist_sysconf_DATA = \
    tntnet/tntnet.properties

tntnet/tntnet.conf: tntnet/tntnet.conf.in Makefile
	sed -e 's,@libdir\@,@libdir@,g;' \
	    -e 's,@pkglibdir\@,@pkglibdir@,g;' \
	    -e 's,@sysconfdir\@,@sysconfdir@,g;' \
	    -e 's,@localstatedir\@,@localstatedir@,g;' \
		-e 's,@datadir\@,@datadir@,g;' <tntnet/tntnet.conf.in >tntnet/tntnet.conf

init.d/tntnet: init.d/tntnet.in Makefile
	sed -e 's,@libdir\@,@libdir@,g;' \
	    -e 's,@pkglibdir\@,@pkglibdir@,g;' \
	    -e 's,@sysconfdir\@,@sysconfdir@,g;' \
	    -e 's,@localstatedir\@,@localstatedir@,g;' \
		-e 's,@datadir\@,@datadir@,g;' <init.d/tntnet.in >init.d/tntnet

tntnet/tntnet.properties: Makefile
	cxxtools-config --logfile @localstatedir@/log/tntnet/tntnet.log --properties tntnet >$@
