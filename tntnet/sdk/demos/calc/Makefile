.PHONY: all

all: calc.so

BASEDIR=$(PWD)/../../..
TOOLSDIR=$(PWD)/../../tools
LIBPATH=$(BASEDIR)/framework/common:$(TOOLSDIR)/common
ECPPC = LD_LIBRARY_PATH=$(LIBPATH) $(TOOLSDIR)/ecppc/ecppc
ECPPFLAGS=-x
CXXFLAGS = -fPIC -Wall -pedantic -I$(BASEDIR)/framework/common -I$(BASEDIR)/framework/ecpp

%.cpp %.h: %.ecpp
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

LDFLAGS= -L$(BASEDIR)/framework/ecpp -lecpp

calc.so: calc.o
	$(CXX) -shared -o $@ $(LDFLAGS) $^

run: calc.so
	LD_LIBRARY_PATH=$(LIBPATH):$(BASEDIR)/framework/ecpp $(BASEDIR)/framework/runtime/tntnet -c tntnet.conf
