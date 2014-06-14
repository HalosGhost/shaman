CFLAGS		+=  -Wall -gdwarf-2 -g3 # -Os
PROG		=   shaman
PREFIX		?=  /usr
VER			=   2.0
MANDIR		?=  /share/man
LIBS		=   `pkg-config --libs-only-l libcurl jansson`
INCLUDE		=   `pkg-config --cflags-only-I libcurl jansson`

.PHONY: all check clean clobber docs install shared test

$(PROG): shared src/$(PROG).c
	@cd src && $(CC) -L`pwd`/.. $(INCLUDE) $(CFLAGS) $(LIBS) -lweather -o ../$(PROG) $(PROG).c
#	@strip $(PROG)

all: check $(PROG)

check: test
	@LD_LIBRARY_PATH=`pwd` ./test_suite

clean:
	@rm -f weather.o

clobber:
	@rm -f $(PROG)
	@rm -f test_suite
	@rm -f libweather.so
	@rm -f src/weather.o

dist: $(PROG) docs
#    @tar -cz some such

docs: doc/man1.tex doc/man3.tex
#    @latex2man some such
#    @latex2man some other

install: libweather.so $(PROG)
	@install -Dm755 libweather.so $(DESTDIR)$(PREFIX)/lib/libweather.so
	@install -Dm755 $(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)

shared: 
	@cd src && $(CC) -c $(CFLAGS) -fPIC weather.c
	@cd src && $(CC) -shared -o ../libweather.so weather.o
#	@strip libweather.so

test: shared
	@cd test && $(CC) -I`pwd`/../src -L`pwd`/.. $(INCLUDE) $(CFLAGS) $(LIBS) -lweather -o ../test_suite suite.c

# vim:set tabstop=4 shiftwidth=4:
