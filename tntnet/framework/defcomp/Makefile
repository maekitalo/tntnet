VERSION=1.1.0
CXXFLAGS = -Wall -O2 -I../common -Izipcomp -fPIC $(LOCAL_CXXFLAGS)
CFLAGS = -Wall -O2 -fPIC
LDFLAGS = -L../common -ltntnet -lz $(LOCAL_LDFLAGS)
DESTDIR = $(ROOTDIR)/usr/local
LIBDIR = $(DESTDIR)/lib
COMPDIR = $(LIBDIR)

.PHONY: install all clean depend

all : tntnet.so

SRCS = \
	error.cpp \
	mime.cpp \
	redirect.cpp \
	static.cpp \
	unzipcomp/unzip_pp.cpp \
	unzipcomp/unzipcomp.cpp

C_SRCS = \
	unzipcomp/unzip.c \
	unzipcomp/ioapi.c

OBJS = $(SRCS:.cpp=.o) $(C_SRCS:.c=.o)
DEPS = $(SRCS:.cpp=.deps) $(C_SRCS:.c=.deps)

%.deps : %.cpp
	( echo -n $(dir $<) && $(CXX) $(CXXFLAGS) -M $< || ( rm -f $@; exit 1 )) >$@

%.deps : %.c
	( echo -n $(dir $<) && $(CC) $(CFLAGS) -M $< || ( rm -f $@; exit 1 )) >$@

tntnet.so : $(OBJS)
	$(CXX) -shared -o $@ $(LDFLAGS) $^

install: tntnet.so
	cp $^ $(COMPDIR)
	strip $(COMPDIR)/$^

clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
