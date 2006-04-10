newsdb.so: index.o news.o newscss.o
	$(CXX) -o $@ $(LDFLAGS) $^

SUFFIXES=.ecpp .gif .jpg
ECPPC=/usr/local/bin/ecppc
CXXFLAGS+=`tntnet-config --cxxflags` -fPIC -O2
LDFLAGS+=-shared `tntnet-config --libs` -ltntdb

%.cpp: %.ecpp
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<
%.cpp: %.gif
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_GIF) -b $<
%.cpp: %.jpg
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_JPG) -b $<
%.cpp: %.css
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_CSS) -b $<

index.cpp: index.ecpp \
  global.ecpp
news.cpp: news.ecpp \
  global.ecpp
