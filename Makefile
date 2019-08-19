.PHONY: all install

DESTDIR ?=
PREFIX ?= /usr
INCLUDEDIR ?= include
LIBDIR ?= lib
MAJ = 0
MIN = 0.1.0

all:
	$(CC) -c -Wall -fpic libco.c -DLIBCO_MP
	$(CC) -shared -o libco.so libco.o
	$(AR) rcs libco.a libco.o
	sed -e "s|@prefix@|$(PREFIX)|g" \
	    -e "s|@libdir@|$(PREFIX)/$(LIBDIR)|g" \
	    -e "s|@includedir@|$(PREFIX)/$(INCLUDEDIR)|g" \
	    libco.pc.in > libco.pc

install:
	install -d $(DESTDIR)$(PREFIX)/$(INCLUDEDIR)/
	install -d $(DESTDIR)$(PREFIX)/$(LIBDIR)/
	install -d $(DESTDIR)$(PREFIX)/$(LIBDIR)/pkgconfig/
	install -m 0664 libco.so $(DESTDIR)$(PREFIX)/$(LIBDIR)/libco.so.$(MIN)
	install -m 0664 libco.pc $(DESTDIR)$(PREFIX)/$(LIBDIR)/pkgconfig/
	ln -s libco.so.$(MIN) $(DESTDIR)$(PREFIX)/$(LIBDIR)/libco.so.$(MAJ)
	ln -s libco.so.$(MIN) $(DESTDIR)$(PREFIX)/$(LIBDIR)/libco.so
	install -m 0664 libco.a $(DESTDIR)$(PREFIX)/$(LIBDIR)/
	install -m 0664 libco.h $(DESTDIR)$(PREFIX)/$(INCLUDEDIR)/
