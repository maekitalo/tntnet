.PHONY: all

all: calc.so

ECPPC = ecppc

%.cpp %.h: %.ecpp
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

LDFLAGS=-lecpp

calc.so: calc.o
	$(CXX) -shared -o $@ $(LDFLAGS) $^
