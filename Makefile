CFLAGS		+=  -Wall -g # -Os
PROG		=   shaman
PREFIX		?=  /usr
VER			=   2.0
MANDIR		?=  /share/man
LIBS		=   `pkg-config --libs-only-l libcurl jansson`
INCLUDE		=   `pkg-config --cflags-only-I libcurl jansson`

.PHONY: all check clean clobber docs install shared test

$(PROG): shared $(PROG).c
	@cd src && $(CC) -L`pwd` $(INCLUDE) $(CFLAGS) $(LIBS) -lweather -o $(PROG) $(PROG).c
#	@strip $(PROG)

all: $(PROG)

check: test
	@LD_LIBRARY_PATH=`pwd` ./test_suite

clean:
	@rm -f weather.o

clobber:
	@rm -f $(PROG)
	@rm -f test_suite
	@rm -f libweather.so
	@rm -f src/weather.o

install: libweather.so $(PROG)
	@install -Dm755 libweather.so $(DESTDIR)$(PREFIX)/lib/libweather.so
	@install -Dm755 $(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)

shared: 
	@cd src && $(CC) -c $(CFLAGS) -fPIC weather.c
	@cd src && $(CC) -shared -o ../libweather.so weather.o
#	@strip libweather.so

test: shared
	@cd src && $(CC) -L`pwd`/.. $(INCLUDE) $(CFLAGS) $(LIBS) -lweather -o ../test_suite suite.c

# vim:set tabstop=4 shiftwidth=4:
