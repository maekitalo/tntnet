DESTDIR = $(ROOTDIR)/usr/local
LIBDIR = $(DESTDIR)/lib
INCDIR = $(DESTDIR)/include
BINDIR = $(DESTDIR)/bin
CONFDIR = $(ROOTDIR)/etc/tntnet
INITDIR = $(ROOTDIR)/etc/init.d

.PHONY: all lib debian install clean

MODULES = \
	framework/common \
	framework/runtime \
	framework/ecpp \
	framework/defcomp \
	sdk/tools/common \
	sdk/tools/ecppc \
	sdk/tools/ecppl \
	sdk/tools/ecppll

all:
	@for m in $(MODULES); \
	do \
	  $(MAKE) -C $$m || exit 1; \
	done

clean:
	@for m in $(MODULES); \
	do \
	  $(MAKE) -C $$m clean || exit 1; \
	done

$(LIBDIR):
	mkdir -p $@

$(INCDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

$(CONFDIR):
	mkdir -p $@

$(INITDIR):
	mkdir -p $@

install: $(LIBDIR) $(INCDIR) $(BINDIR) $(CONFDIR) $(INITDIR)
	@for m in $(MODULES); \
	do \
	  $(MAKE) -C $$m install || exit 1; \
	done
	cp -ra etc/tntnet/tntnet.conf $(CONFDIR)
	cp -ra etc/tntnet/tntnet.properties $(CONFDIR)
	cp -ra etc/init.d/tntnet $(INITDIR)

debian:
	dpkg-buildpackage -rfakeroot -b -uc

