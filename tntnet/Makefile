DESTDIR = $(ROOTDIR)/usr/local
LIBDIR = $(DESTDIR)/lib
INCDIR = $(DESTDIR)/include
BINDIR = $(DESTDIR)/bin

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
	mkdir -p $(LIBDIR)

$(INCDIR):
	mkdir -p $(INCDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

install: $(LIBDIR) $(INCDIR) $(BINDIR)
	@for m in $(MODULES); \
	do \
	  $(MAKE) -C $$m install || exit 1; \
	done

debian:
	dpkg-buildpackage -rfakeroot -b -uc

