CXXFLAGS=-fPIC
LDFLAGS=-lecpp
CC=$(CXX)

%.cpp %.h: %.ecpp
	ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

cookie.so: cookie.o
	$(CXX) -shared $(LDFLAGS) -o $@ $^
