CFLAGS  +=  -Os -Wall
PROG    =   shaman
PREFIX  ?=  /usr
VER     =   2.0
MANDIR  ?=  /share/man
LIBS    =   `pkg-config --libs-only-l libcurl libxml-2.0` #-lweather
INCLUDE =   `pkg-config --cflags-only-I libxml-2.0` #-L${PWD}/src
HEADERS =   ${PROG}.h fetch.h format.h memsafety.h parse.h usage.h

.PHONY: all clean clobber docs install shared

${PROG}: shared src/${PROG}.c
	@cd src && ${CC} ${INCLUDE} ${CFLAGS} ${LIBS} -o ../${PROG} ${PROG}.c
	@strip ${PROG}

all: ${PROG} docs

clean:
	@rm -f src/weather.o

clobber:
	@rm -f ${PROG}
	@rm -f ${PROG}.1
	@rm -f strfwthr.3
	@rm -f src/libweather.so
	@rm -f src/weather.o

docs: docs/man1.tex docs/man3.tex
	@cd docs && latex2man man1.tex ../${PROG}.1 && latex2man man3.tex ../strfwthr.3

install: src/libstrfwthr.so ${PROG} ${PROG}.1 _${PROG} 
	@install -Dm755 src/libweather.so ${DESTDIR}${PREFIX}/lib/libweather.so
	@install -Dm755 ${PROG} ${DESTDIR}${PREFIX}/bin/${PROG}
	@install -Dm644 ${PROG}.1 ${DESTDIR}${PREFIX}${MANDIR}/man1/${PROG}.1
	@install -Dm644 _${PROG} ${DESTDIR}${PREFIX}/share/zsh/site-functions/_${PROG}

shared: src/weather.c
	@cd src && ${CC} -c ${INCLUDE} ${CFLAGS} ${LIBS} -fPIC weather.c
	@cd src && ${CC} -shared -o libweather.so weather.o

# vim:set tabstop=4 shiftwidth=4:
