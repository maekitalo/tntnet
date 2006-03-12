nobase_dist_noinst_DATA = \
    tntnet/tntnet.conf.in

nobase_sysconf_DATA = \
    tntnet/tntnet.conf

nobase_dist_sysconf_DATA = \
    tntnet/mime.conf \
    init.d/tntnet

nobase_nodist_sysconf_DATA = \
    tntnet/tntnet.properties

tntnet/tntnet.conf: tntnet/tntnet.conf.in
	sed -e 's,@libdir\@,@libdir@,g;' \
	    -e 's,@sysconfdir\@,@sysconfdir@,g;' \
		-e 's,@datadir\@,@datadir@,g;' <tntnet/tntnet.conf.in >tntnet/tntnet.conf

tntnet/tntnet.properties:
	cxxtools-config --logfile /var/log/tntnet/tntnet.log --properties tntnet >$@
