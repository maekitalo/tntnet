VERSION=1.0.0
CXXFLAGS = -Wall -O2 -I. -fPIC $(LOCAL_CXXFLAGS)
LDFLAGS = $(LOCAL_LDFLAGS)

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
	cp $^ $(DESTDIR)/usr/lib
	cp -ra --parents tnt/*.h $(DESTDIR)/usr/include
	ln -s libtntnet_sdk.so.$(VERSION) $(DESTDIR)/usr/lib/libtntnet_sdk.so

clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
