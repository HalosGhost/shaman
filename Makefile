PROGNM =  shaman
PREFIX ?= /usr/local
DOCDIR ?= $(DESTDIR)$(PREFIX)/share/man
BINDIR ?= $(DESTDIR)$(PREFIX)/bin
INCDIR ?= $(DESTDIR)$(PREFIX)/include
LIBDIR ?= $(DESTDIR)$(PREFIX)/lib
ZSHDIR ?= $(DESTDIR)$(PREFIX)/share/zsh

.PHONY: all clean gen clang-analyze cov-build simple install uninstall

all: dist
	@tup upd

clean:
	@rm -rf -- dist cov-int $(PROGNM).tgz make.sh ./src/*.plist

dist:
	@mkdir -p ./dist/locale

gen: clean
	@tup generate make.sh

cov-build: gen dist
	@cov-build --dir cov-int ./make.sh
	@tar czvf $(PROGNM).tgz cov-int

clang-analyze:
	@(pushd ./src; clang-check -analyze ./*.c)

simple: gen dist
	@./make.sh

install:
	@install -Dm755 dist/shaman        $(BINDIR)/$(PROGNM)
	@install -Dm755 dist/libweather.so $(LIBDIR)/libweather.so
	@install -Dm644 dist/weather.h     $(INCDIR)/shaman/weather.h
	@install -Dm644 dist/shaman.1      $(DOCDIR)/man1/$(PROGNM).1
	@install -Dm644 dist/strfweather.3 $(DOCDIR)/man3/strfweather.3
	@install -Dm644 dist/zsh           $(ZSHDIR)/site-functions/_$(PROGNM)

uninstall:
	@rm -f $(BINDIR)/$(PROGNM)
	@rm -f $(LIBDIR)/libweather.so
	@rm -f $(INCDIR)/shaman/weather.h
	@rm -f $(DOCDIR)/man1/$(PROGNM).1
	@rm -f $(DOCDIR)/man3/strfweather.3
	@rm -f $(ZSHDIR)/site-functions/_shaman
