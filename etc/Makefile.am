nobase_dist_noinst_DATA = \
    tntnet/tntnet.xml.in 

nobase_sysconf_DATA = \
    tntnet/tntnet.xml

CLEANFILES = tntnet.xml

tntnet/tntnet.xml: tntnet/tntnet.xml.in
	$(AM_V_GEN) \
	$(MKDIR_P) tntnet && \
	$(SED) -e 's,@libdir\@,@libdir@,g;' \
	    -e 's,@pkglibdir\@,@pkglibdir@,g;' \
	    -e 's,@sysconfdir\@,@sysconfdir@,g;' \
	    -e 's,@localstatedir\@,@localstatedir@,g;' \
	    -e 's,@datadir\@,@datadir@,g;' < $(srcdir)/tntnet/tntnet.xml.in > tntnet/tntnet.xml
