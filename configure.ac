AC_PREREQ([2.69])
AC_INIT([shaman],[0],[halosghost@archlinux.info],[https://github.com/HalosGhost/shaman])
AC_CONFIG_SRCDIR([src/weather.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign 1.11 -Wall -Wno-portability subdir-objects silent-rules color-tests dist-xz tar-ustar])

AM_SILENT_RULES([yes])

### checks ###

# CC

AC_PROG_CC
AC_PROG_CC_C99
AC_GNU_SOURCE

# pkg-config

PKG_PROG_PKG_CONFIG

#### compiler flags ####

AX_APPEND_COMPILE_FLAGS(
        [-Wall -W -Wextra -pipe -Wno-long-long -Wno-overlength-strings -Wunsafe-loop-optimizations -Wundef -Wlogical-op -Wsign-compare -Wmissing-include-dirs -Wold-style-definition -Wpointer-arith -Winit-self -Wdeclaration-after-statement -Wfloat-equal -Wmissing-prototypes -Wstrict-prototypes -Wredundant-decls -Wmissing-declarations -Wmissing-noreturn -Wshadow -Wendif-labels -Wcast-align -Wstrict-aliasing -Wwrite-strings -Wno-unused-parameter -Wno-error=unused-function -ffast-math -fno-common -fdiagnostics-show-option],
        [], [-pedantic -pedantic-errors -Werror])

#### libtool ####

LT_PREREQ(2.4)
LT_INIT([dlopen disable-static])

#### environment ####

AC_HEADER_STDC

PKG_CHECK_MODULES(CURL, [libcurl])
PKG_CHECK_MODULES(JANSSON, [jansson])

#

AC_CONFIG_FILES([
Makefile
src/Makefile
tests/Makefile
])

AC_OUTPUT
