VERSION=1.0.0
CXXFLAGS = -Wall -O2 -I. -I../common -fPIC $(LOCAL_CXXFLAGS)
LDFLAGS = -L../common -ltntnet $(LOCAL_LDFLAGS)

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
	cp $^ $(DESTDIR)/usr/lib
	cp -ra --parents tnt/*.h $(DESTDIR)/usr/include
	ln -sf libecpp.so.$(VERSION) $(DESTDIR)/usr/lib/libecpp.so

clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
