lib_LTLIBRARIES = libweather.la

libweather_la_SOURCES = weather.c weather.h
libweather_la_CFLAGS = $(AM_CFLAGS) $(CURL_CFLAGS) $(JANSSON_CFLAGS)
libweather_la_LIBADD = $(AM_CFLAGS) $(CURL_LIBS) $(JANSSON_LIBS)

bin_PROGRAMS = shaman

shaman_SOURCES = shaman.c
shaman_LDADD = libweather.la
