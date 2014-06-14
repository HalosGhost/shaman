AC_PREREQ([2.69])
AC_INIT([shaman],[0],[halosghost@archlinux.info],[https://github.com/HalosGhost/shaman])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign 1.11 -Wall -Wno-portability subdir-objects silent-rules color-tests dist-xz tar-ustar])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CC_C99
AC_GNU_SOURCE
AC_HEADER_STDC

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(CURL, [libcurl])
PKG_CHECK_MODULES(JANSSON, [jansson])

LT_PREREQ(2.4)
LT_INIT([dlopen disable-static])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT
