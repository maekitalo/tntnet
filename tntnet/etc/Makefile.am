nobase_dist_noinst_DATA = \
    tntnet/tntnet.conf.in

nobase_sysconf_DATA = \
    tntnet/tntnet.conf

nobase_dist_sysconf_DATA = \
    tntnet/mime.conf \
    tntnet/tntnet.properties \
    init.d/tntnet

tntnet/tntnet.conf: tntnet/tntnet.conf.in
	sed -e 's,@libdir\@,@libdir@,g;' \
	    -e 's,@sysconfdir\@,@sysconfdir@,g;' \
		-e 's,@datadir\@,@datadir@,g;' <$< >$@
