all : ecppl

CXXFLAGS=-Wall -O2 -I../common -I../../../framework/common $(LOCAL_CXXFLAGS)
LDFLAGS=-L../common -ltntnet_sdk $(LOCAL_LDFLAGS)
DESTDIR = $(ROOTDIR)/usr/local
BINDIR = $(DESTDIR)/bin

SRCS = \
	ecppl.cpp \
	ecpplang.cpp

OBJS = $(SRCS:.cpp=.o)
CC = $(CXX)

%.deps : %.cpp
	$(CXX) $(CXXFLAGS) -M $< >$@

ecppl : $(OBJS)

DEPS = $(SRCS:.cpp=.deps)

install: ecppl
	cp -ra $^ $(BINDIR)
	strip $(BINDIR)/$^

clean:
	rm -f ecppl $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

include $(DEPS)

