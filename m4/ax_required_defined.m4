# http://www.gnu.org/software/autoconf-archive/ax_require_defined.html

#serial 1

AC_DEFUN([AX_REQUIRE_DEFINED], [dnl
  m4_ifndef([$1], [m4_fatal([macro ]$1[ is not defined; is a m4 file missing?])])
])dnl AX_REQUIRE_DEFINED
