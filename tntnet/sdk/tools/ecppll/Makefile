all : ecppll

CXXFLAGS=-I../common -I../../../framework/common $(LOCAL_CXXFLAGS)
LDFLAGS=-L../common -ltntnet_sdk $(LOCAL_LDFLAGS)

SRCS = \
	ecppll.cpp

OBJS = $(SRCS:.cpp=.o)
CC = $(CXX)

%.deps : %.cpp
	$(CXX) $(CXXFLAGS) -M $< >$@

ecppll : $(OBJS)

DEPS = $(SRCS:.cpp=.deps)

install: ecppll
	cp -ra $^ $(DESTDIR)/usr/bin

clean:
	rm -f ecppll $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

include $(DEPS)

