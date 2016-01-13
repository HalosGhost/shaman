PREFIX ?= /usr/local
DOCDIR ?= $(DESTDIR)$(PREFIX)/share/man
BINDIR ?= $(DESTDIR)$(PREFIX)/bin
INCDIR ?= $(DESTDIR)$(PREFIX)/include
LIBDIR ?= $(DESTDIR)$(PREFIX)/lib

.PHONY: all clean test install uninstall

all:
	@mkdir -p ./dist
	@tup upd

clean:
	@rm -rf ./dist

test:
	@./dist/suite

install:
	@install -Dm755 dist/shaman        $(BINDIR)/shaman
	@install -Dm755 dist/libweather.so $(LIBDIR)/libweather.so
	@install -Dm644 dist/weather.h     $(INCDIR)/shaman/weather.h
	@install -Dm644 dist/shaman.1      $(DOCDIR)/man1/shaman.1
	@install -Dm644 dist/strfweather.3 $(DOCDIR)/man3/strfweather.3

uninstall:
	@rm -f $(BINDIR)/shaman
	@rm -f $(DOCDIR)/man1/shaman.1
	@rm -f $(DOCDIR)/man3/strfweather.3
	@rm -f $(ZSHDIR)/site-functions/_shaman
