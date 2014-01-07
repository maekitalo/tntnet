all: gd.so

test: all
	${TNTNET} tntnet.xml

gd.so: gd.o gd++.o imgstr.o
	${CXX} -o $@ $^ ${LDFLAGS}

.SUFFIXES: .ecpp .gif .jpg .css .js .cpp
ECPPC=/usr/local/bin/ecppc
TNTNET=/usr/local/bin/tntnet
CXXFLAGS+=-I/usr/local/include -I. -fPIC -O2
LDFLAGS+=-shared -L/usr/local/lib -ltntnet -lcxxtools -lgd

.ecpp.cpp:
	${ECPPC} ${ECPPFLAGS} ${ECPPFLAGS_CPP} -o $@ $<
.gif.cpp:
	${ECPPC} ${ECPPFLAGS} -m image/gif ${ECPPFLAGS_GIF} -b -o $@ $<
.jpg.cpp:
	${ECPPC} ${ECPPFLAGS} -m image/jpg ${ECPPFLAGS_JPG} -b -o $@ $<
.png.cpp:
	${ECPPC} ${ECPPFLAGS} -m image/png ${ECPPFLAGS_PNG} -b -o $@ $<
.ico.cpp:
	${ECPPC} ${ECPPFLAGS} -m image/x-icon ${ECPPFLAGS_ICO} -b -o $@ $<
.css.cpp:
	${ECPPC} ${ECPPFLAGS} -m text/css ${ECPPFLAGS_CSS} -b -o $@ $<
.js.cpp:
	${ECPPC} ${ECPPFLAGS} -m application/javascript ${ECPPFLAGS_JS} -b -o $@ $<
