all : tntnet

BINDIR = $(DESTDIR)/bin
CXXFLAGS=-I. -I../common $(LOCAL_CXXFLAGS)
LDFLAGS=-L../common -lpthread -ltntnet -lcxxtools -lboost_regex-gcc-mt -lssl $(LOCAL_LDFLAGS)
SRCS = \
	dispatcher.cpp \
	job.cpp \
	server.cpp \
	tntnet.cpp \
	ssl.cpp \
	listener.cpp

OBJS = $(SRCS:.cpp=.o)
CC = $(CXX)

%.deps : %.cpp
	$(CXX) $(CXXFLAGS) -M $< >$@

tntnet : $(OBJS)

DEPS = $(SRCS:.cpp=.deps)

install: tntnet
	cp -ra $^ $(BINDIR)

clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

include $(DEPS)
