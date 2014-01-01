CFLAGS  +=  -Os -Wall
PROG    =   shaman
PREFIX  ?=  /usr
VER     =   0.1
MANDIR  ?=  /share/man
LIBS    =   -lm -lcurl -lxml2
INCLUDE =   -I/usr/include/libxml2

${PROG}: ${PROG}.c
	@${CC} ${INCLUDE} ${CFLAGS} ${LIBS} -o ${PROG} ${PROG}.c
#	@strip ${PROG}

clean:
	@rm -f ${PROG}
	@rm -f ${PROG}.1

docs: man1.tex
	@latex2man man1.tex ${PROG}.1

install: ${PROG}
	@install -Dm755 ${PROG} ${DESTDIR}${PREFIX}/bin/${PROG}
	@install -Dm644 ${PROG}.1 ${DESTDIR}${PREFIX}${MANDIR}/man1/${PROG}.1
	@install -Dm644 _${PROG} ${DESTDIR}${PREFIX}/share/zsh/site-functions/_${PROG}

# vim: set tabstop=4 shiftwidth=4
