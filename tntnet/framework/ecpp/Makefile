VERSION=1.1.0
CXXFLAGS = -Wall -O2 -I. -I../common -fPIC $(LOCAL_CXXFLAGS)
LDFLAGS = -L../common -ltntnet -Wl,-E $(LOCAL_LDFLAGS)
DESTDIR = $(ROOTDIR)/usr/local
LIBDIR = $(DESTDIR)/lib
INCDIR = $(DESTDIR)/include

all : libecpp.so

SRCS = ecpp.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.deps) \

%.deps : %.cpp
	$(CXX) $(CXXFLAGS) -M $< >$@

libecpp.so.$(VERSION) : $(OBJS)
	$(CXX) -shared -o $@ $(LDFLAGS) $^

libecpp.so: libecpp.so.$(VERSION)
	ln -sf $< $@

install: libecpp.so.$(VERSION)
	cp $^ $(LIBDIR)
	cp -ra --parents tnt/*.h $(INCDIR)
	ln -sf libecpp.so.$(VERSION) $(LIBDIR)/libecpp.so

clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
