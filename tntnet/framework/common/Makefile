VERSION=1.0.0
CXXFLAGS = -Wall -O2 -I. -fPIC $(LOCAL_CXXFLAGS)
LDFLAGS = -lcxxtools -llog4cplus -lz $(LOCAL_LDFLAGS)

all : libtntnet.so

SRCS = \
	comploader.cpp \
	component.cpp \
	contentdisposition.cpp \
	contenttype.cpp \
	http.cpp \
	log.cpp \
	messageattribute.cpp \
	messageheader.cpp \
	multipart.cpp \
	sodata.cpp \
	tntconfig.cpp \
	zdata.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.deps) \

%.deps : %.cpp
	( echo -n $(dir $<) && $(CXX) $(CXXFLAGS) -M $< || ( rm -f $@; exit 1 )) >$@

libtntnet.so.$(VERSION) : $(OBJS)
	$(CXX) -shared -o $@ $(LDFLAGS) $^

libtntnet.so: libtntnet.so.$(VERSION)
	ln -sf $< $@

install: libtntnet.so.$(VERSION)
	cp $^ $(DESTDIR)/usr/lib
	cp -ra --parents tnt/*.h $(DESTDIR)/usr/include
	ln -sf libtntnet.so.$(VERSION) $(DESTDIR)/usr/lib/libtntnet.so

clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
