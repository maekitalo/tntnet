all : ecppll

CXXFLAGS=-Wall -O2 -I../common -I../../../framework/common $(LOCAL_CXXFLAGS)
LDFLAGS=-L../common -ltntnet_sdk $(LOCAL_LDFLAGS)
DESTDIR = $(ROOTDIR)/usr/local
BINDIR = $(DESTDIR)/bin

SRCS = \
	ecppll.cpp

OBJS = $(SRCS:.cpp=.o)
CC = $(CXX)

%.deps : %.cpp
	$(CXX) $(CXXFLAGS) -M $< >$@

ecppll : $(OBJS)

DEPS = $(SRCS:.cpp=.deps)

install: ecppll
	cp -ra $^ $(BINDIR)
	strip $(BINDIR)/$^

clean:
	rm -f ecppll $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

include $(DEPS)

