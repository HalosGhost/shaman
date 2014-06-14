# http://www.gnu.org/software/autoconf-archive/ax_append_flag.html

#serial 2

AC_DEFUN([AX_APPEND_FLAG],
[AC_PREREQ(2.59)dnl for _AC_LANG_PREFIX
AS_VAR_PUSHDEF([FLAGS], [m4_default($2,_AC_LANG_PREFIX[FLAGS])])dnl
AS_VAR_SET_IF(FLAGS,
  [case " AS_VAR_GET(FLAGS) " in
    *" $1 "*)
      AC_RUN_LOG([: FLAGS already contains $1])
      ;;
    *)
      AC_RUN_LOG([: FLAGS="$FLAGS $1"])
      AS_VAR_SET(FLAGS, ["AS_VAR_GET(FLAGS) $1"])
      ;;
   esac],
  [AS_VAR_SET(FLAGS,["$1"])])
AS_VAR_POPDEF([FLAGS])dnl
])dnl AX_APPEND_FLAG
