all : ecppl

CXXFLAGS=-I../common -I../../../framework/common $(LOCAL_CXXFLAGS)
LDFLAGS=-L../common -ltntnet_sdk $(LOCAL_LDFLAGS)

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
	cp -ra $^ $(DESTDIR)/usr/bin

clean:
	rm -f ecppl $(OBJS) $(DEPS)

.PHONY: depend
depend: $(DEPS)

include $(DEPS)

