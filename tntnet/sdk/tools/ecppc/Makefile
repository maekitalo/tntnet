all : ecppc

CXXFLAGS=-I../common -I../../../framework/common $(LOCAL_CXXFLAGS)
LDFLAGS=-L../common -L../../../framework/common -ltntnet_sdk -ltntnet -lcxxtools $(LOCAL_LDFLAGS)

SRCS = \
	ecppc.cpp \
	ecppGenerator.cpp

OBJS = $(SRCS:.cpp=.o)
CC = $(CXX)

%.deps : %.cpp
	$(CXX) $(CXXFLAGS) -M $< >$@

ecppc : $(OBJS)

DEPS = $(SRCS:.cpp=.deps)

install: ecppc
	cp -ra $^ $(DESTDIR)/usr/bin

clean:
	rm -f ecppc $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

