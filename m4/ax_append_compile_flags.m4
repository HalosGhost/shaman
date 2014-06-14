# http://www.gnu.org/software/autoconf-archive/ax_append_compile_flags.html

#serial 4

AC_DEFUN([AX_APPEND_COMPILE_FLAGS],
[AX_REQUIRE_DEFINED([AX_CHECK_COMPILE_FLAG])
AX_REQUIRE_DEFINED([AX_APPEND_FLAG])
for flag in $1; do
  AX_CHECK_COMPILE_FLAG([$flag], [AX_APPEND_FLAG([$flag], [$2])], [], [$3])
done
])dnl AX_APPEND_COMPILE_FLAGS
