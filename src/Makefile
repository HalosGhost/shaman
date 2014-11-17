CC = gcc
CFLAGS = -Wall -Wpedantic -std=c11
LDLIBS = -lcurl -ljansson -lm

prefix = /usr
bindir = $(prefix)/bin
mandir = $(prefix)/share/man
man1dir = $(mandir)/man1

shaman: shaman.o weather.o

install:
	install -m 0755 -Ds shaman $(DESTDIR)$(bindir)/shaman
	install -m 0655 -D ../docs/shaman.1  $(DESTDIR)$(man1dir)/shaman.1

clean:
	rm -f *.o
