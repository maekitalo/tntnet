VERSION=1.0.0
CXXFLAGS = -Wall -O2 -I. -fPIC $(LOCAL_CXXFLAGS)
LDFLAGS = $(LOCAL_LDFLAGS)
DESTDIR = $(ROOTDIR)/usr/local
LIBDIR = $(DESTDIR)/lib
INCDIR = $(DESTDIR)/include

all : libtntnet_sdk.so

SRCS = \
	datachunks_creator.cpp \
	ecppParser.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.deps) \

%.deps : %.cpp
	( echo -n $(dir $<) && $(CXX) $(CXXFLAGS) -M $< || ( rm -f $@; exit 1 )) >$@

libtntnet_sdk.so.$(VERSION) : $(OBJS)
	$(CXX) -shared -o $@ $(LDFLAGS) $^

libtntnet_sdk.so: libtntnet_sdk.so.$(VERSION)
	ln -sf $< $@

install: libtntnet_sdk.so.$(VERSION)
	cp $^ $(LIBDIR)
	cp -ra --parents tnt/*.h $(INCDIR)
	ln -s libtntnet_sdk.so.$(VERSION) $(LIBDIR)/libtntnet_sdk.so

clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
