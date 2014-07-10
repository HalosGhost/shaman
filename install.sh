#!/usr/bin/env bash

PREFIX = ${PREFIX:-/usr}

install -Dm755 src/shaman "${DESTDIR}/${PREFIX}/bin/shaman"
install -Dm755 src/libweather.so "${DESTDIR}/${PREFIX}/lib/libweather.so"
install -Dm644 src/weather.h "${DESTDIR}/${PREFIX}/include/shaman/weather.h"
install -Dm644 docs/shaman.1 "${DESTDIR}/${PREFIX}/share/man/man1/shaman.1"
install -Dm644 docs/strfweather.2 "${DESTDIR}/${PREFIX}/share/man/man2/strfweather.2"
