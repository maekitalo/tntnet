ifndef DESTDIR
	export DESTDIR=/
endif

.PHONY: lib debian

MODULES = \
	framework/common \
	framework/runtime \
	framework/ecpp \
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

$(DESTDIR)/usr/lib:
	mkdir -p $(DESTDIR)/usr/lib

$(DESTDIR)/usr/include:
	mkdir -p $(DESTDIR)/usr/include

$(DESTDIR)/usr/bin:
	mkdir -p $(DESTDIR)/usr/bin

install: $(DESTDIR)/usr/lib $(DESTDIR)/usr/include $(DESTDIR)/usr/bin
	@for m in $(MODULES); \
	do \
	  $(MAKE) -C $$m install || exit 1; \
	done

debian:
	dpkg-buildpackage -rfakeroot -b -uc

