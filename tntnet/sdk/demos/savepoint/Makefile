topdir=../../..
CXXFLAGS=-fPIC -I$(topdir)/framework/common
LDFLAGS=-lecpp -L$(topdir)/framework/common
ECPPC=../../tools/ecppc/ecppc
CC=$(CXX)

%.cpp %.h: %.ecpp
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

savepoint.so: savepoint.o
	$(CXX) -shared $(LDFLAGS) -o $@ $^
