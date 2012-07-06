AC_INIT([elemines], [0.1], [ngc891@gmail.com])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([configure.ac])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AC_CONFIG_HEADERS([config.h])

# this will speed up libtool checks
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
define([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_PROG_LIBTOOL

AC_PROG_CC
EFL_ATTRIBUTE_UNUSED
EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-W])

# Checks for header files.
EFL_CHECK_PATH_MAX

# Check for EFL
PKG_CHECK_MODULES([EINA], [eina >= 1.2.0])
PKG_CHECK_MODULES([EVAS], [evas >= 1.2.0])
PKG_CHECK_MODULES([EDJE], [edje  >= 1.2.0])
PKG_CHECK_MODULES([ELEMENTARY], [elementary >= 1.0.0])

# Option to enable sound
multisense=`pkg-config edje --variable=multisense`
AC_ARG_ENABLE([sound], [AC_HELP_STRING([--enable-sound], [Enable sound support by multisense. @<:@default=disable@:>@])],
  [if test "x${enableval}" = "xyes" ; then
    if test "x${multisense}" = "xyes"; then
      enable_sound="yes"
      AC_DEFINE([SOUND], [0], [Sound support])
    else
      AC_MSG_ERROR([sound support requested, but multisense not found by pkg-config.])
    fi
  else
    enable_sound="no"
    AC_DEFINE([Sound], [1], [No sound])
  fi],
  [enable_sound="no"])
AM_CONDITIONAL([enable_sound], [test "x${enable_sound}" = "xyes"])
AC_MSG_CHECKING([for MULTISENSE])
AC_MSG_RESULT([${enable_sound}])

AC_CONFIG_FILES([
Makefile
data/Makefile
data/desktop/Makefile
data/themes/Makefile
data/themes/fonts/Makefile
data/themes/images/Makefile
data/themes/sounds/Makefile
packaging/Makefile
packaging/slackware/Makefile
src/Makefile
])
AC_OUTPUT

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Compilation..............: make (or gmake)"
echo "  CFLAGS.................: $CFLAGS"
echo "  Sound..................: ${enable_sound}"
echo
echo "Installation.............: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix.................: $prefix"
echo
