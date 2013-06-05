dnl/* -*- mode: m4; tab-width: 2; c-basic-offset: 2; indent-tabs-mode: nil; -*-
dnl * vim:set ts=2 sw=2 expandtab: *********************************************
dnl *
dnl * acinclude.m4 - Common configure macros especially for Qt3/Qt4
dnl * Copyright (C) 2006-2012 by Jens Langner, www.hzdr.de
dnl *
dnl * This library is free software; you can redistribute it and/or
dnl * modify it under the terms of the GNU Lesser General Public
dnl * License as published by the Free Software Foundation; either
dnl * version 2.1 of the License, or (at your option) any later version.
dnl *
dnl * This library is distributed in the hope that it will be useful,
dnl * but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl * Lesser General Public License for more details.
dnl *
dnl * You should have received a copy of the GNU Lesser General Public
dnl * License along with this library; if not, write to the Free Software
dnl * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl *
dnl * $Id$
dnl *
dnl **************************************************************************/

dnl
dnl AC_CXX_NAMESPACES: checks for proper C++ namespaces compatibility
dnl
AC_DEFUN([AC_CXX_NAMESPACES],
  [AC_CACHE_CHECK(whether the compiler implements namespaces,
      ac_cv_cxx_namespaces,
      [AC_LANG_SAVE
       AC_LANG_CPLUSPLUS
       AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                      [using namespace Outer::Inner; return i;],
                      ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
       AC_LANG_RESTORE
  ])

  if test "$ac_cv_cxx_namespaces" = yes; then
    AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
  fi
])

dnl
dnl AC_CXX_STATIC_CAST: checks for static_cast<> C++ functionality.
dnl
AC_DEFUN([AC_CXX_STATIC_CAST],
[AC_CACHE_CHECK(whether the compiler supports static_cast<>,
ac_cv_cxx_static_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0; };
class Derived : public Base { public : Derived () {} virtual void f () {} };
int g (Derived&) { return 0; }],[
Derived d; Base& b = d; Derived& s = static_cast<Derived&> (b); return g (s);],
 ac_cv_cxx_static_cast=yes, ac_cv_cxx_static_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_static_cast" = yes; then
  AC_DEFINE(HAVE_STATIC_CAST,,
            [define if the compiler supports static_cast<>])
fi
])

dnl
dnl AC_ANSI_COLOR: provides a switch to enable/disable ANSI color
dnl output for compilation/debugging via rtdebug library
dnl
AC_DEFUN([AC_ANSI_COLOR],
[
  AC_MSG_CHECKING(whether ANSI color should be used for terminal output)
  AC_ARG_ENABLE(ansi-color,
                [AC_HELP_STRING([--enable-ansi-color], [ansi-color terminal output [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_ansi_color=yes ;;
                  no)  test_on_ansi_color=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --disable-ansi-color) ;;
                esac], 
                [test_on_ansi_color=yes])

  if test "$test_on_ansi_color" = "yes"; then
    ANSI_COLOR="ansi_color" 
    AC_DEFINE(WITH_ANSI_COLOR)    
    AC_MSG_RESULT(yes)
  else  
    AC_MSG_RESULT(no)
  fi
  dnl AC_DEFINE([WITH_ANSI_COLOR], [], [Use ANSI color scheme in terminal debug output])
  
  AC_SUBST(ANSI_COLOR) 
])

dnl
dnl AC_ENABLE_DEBUG: provides a switch to enable/disable debugging
dnl
AC_DEFUN([AC_ENABLE_DEBUG],
[
  AC_MSG_CHECKING(whether to enable debugging)
  AC_ARG_ENABLE(debug,
                [AC_HELP_STRING([--enable-debug], [turn on debugging mode [default=no]])],
                [case "${enableval}" in
                  yes) test_on_enable_debug=yes ;;
                  no)  test_on_enable_debug=no  ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
                esac],
                [test_on_enable_debug=no])

  if test "$test_on_enable_debug" = "yes"; then
    COMPILE_LEVEL="debug"
    AC_MSG_RESULT(yes)
  else
    COMPILE_LEVEL="release"
    AC_MSG_RESULT(no)
  fi

  AC_SUBST(COMPILE_LEVEL) 
])


dnl
dnl AC_ENABLE_DEBUG: provides a switch to enable/disable rinterface
dnl
AC_DEFUN([AC_ENABLE_RINTERFACE],
[
  AC_MSG_CHECKING(whether to build with rinterface)
  AC_ARG_ENABLE(rinterface,
                [AC_HELP_STRING([--enable-rinterface], [turn on the rinterface [default=no]])],
                [case "${enableval}" in
                  yes) test_on_enable_rinterface=yes ;;
                  no)  test_on_enable_rinterface=no  ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-rinterface) ;;
                esac],
                [test_on_enable_rinterface=no])

  if test "$test_on_enable_rinterface" = "yes"; then
    R_INTERFACE="enabled"
    AC_MSG_RESULT(yes)
  else
    R_INTERFACE="disabled"
    AC_MSG_RESULT(no)
  fi

  AC_SUBST(R_INTERFACE)
])

dnl
dnl AC_ENABLE_STATIC_LIB: if the project is a library this macro can be used to control
dnl if the library should be build as a shared or static library
dnl
AC_DEFUN([AC_ENABLE_STATIC_LIB],
[
  AC_MSG_CHECKING(whether to link as a static library)
  AC_ARG_ENABLE(static-lib,
                [AC_HELP_STRING([--enable-static-lib], [turn on static linkage [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_lib=yes  ;;
                  no)  test_on_enable_static_lib=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-lib) ;;
                esac],
                [test_on_enable_static_lib=yes])

  if test "$test_on_enable_static_lib" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticlib"
    AC_MSG_RESULT(yes)
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_SHARED_LIB: if the project is a library this macro can be used to control
dnl if the library should be build as a shared or static library
dnl
AC_DEFUN([AC_ENABLE_SHARED_LIB],
[
  AC_MSG_CHECKING(whether to link as a shared library)
  AC_ARG_ENABLE(shared-lib,
                [AC_HELP_STRING([--enable-shared-lib], [turn on shared linkage [default=no]])],
                [case "${enableval}" in
                  yes) test_on_enable_shared_lib=yes  ;;
                  no)  test_on_enable_shared_lib=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-shared-lib) ;;
                esac],
                [test_on_enable_shared_lib=no])

  if test "$test_on_enable_shared_lib" = "yes" -a "$ac_static_build" = "no"; then
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(yes)
  else
    QTLINK_LEVEL="${QTLINK_LEVEL} staticlib"
    AC_MSG_RESULT(no)
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_BUILD: enables complete static linkage of all components
dnl
AC_DEFUN([AC_ENABLE_STATIC_BUILD],
[
  AC_MSG_CHECKING(whether to link all components statically)
  AC_ARG_ENABLE(static-build,
                [AC_HELP_STRING([--enable-static-build], [turn on static linkage of all components [default=no]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_build=yes  ;;
                  no)  test_on_enable_static_build=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-build) ;;
                esac],
                [test_on_enable_static_build=no])

  if test "$test_on_enable_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticbuild"

    dnl go and put a link into the "lib" dir for the various
    dnl libraries we have to force to be linked statically
    rm -f lib/libstdc++.a
    ln -s `$CXX -print-file-name=libstdc++.a` lib/libstdc++.a
    
    AC_MSG_RESULT(yes)
    ac_static_build="yes"
  else
    QTLINK_LEVELL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_static_build="no"
  fi

  AC_SUBST(QTLINK_LEVEL)
])

dnl
dnl AC_ENABLE_STATIC_QT: provides a switch to control the link level (static/shared)
dnl of a linked Qt library.
dnl
AC_DEFUN([AC_ENABLE_STATIC_QT],
[
  AC_MSG_CHECKING(whether to link the Qt library static)
  AC_ARG_ENABLE(static-qt,
                [AC_HELP_STRING([--enable-static-qt], [turn on static linkage of Qt libs [default=no]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_qt=yes ;;
                  no)  test_on_enable_static_qt=no   ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-qt) ;;
                esac],
                [test_on_enable_static_qt=no])

  if test "$test_on_enable_static_qt" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticconfig"
    AC_MSG_RESULT(yes)
    ac_qt_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_qt_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_RTDEBUG: provides a switch to control the link level (static/shared)
dnl of a linked rtdebug library
dnl
AC_DEFUN([AC_ENABLE_STATIC_RTDEBUG],
[
  AC_MSG_CHECKING(whether to link rtdebug library static)
  AC_ARG_ENABLE(static-rtdebug,
                [AC_HELP_STRING([--enable-static-rtdebug], [turn on static linkage of rtdebug lib [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_rtdebug=yes  ;;
                  no)  test_on_enable_static_rtdebug=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-rtdebug) ;;
                esac],
                [test_on_enable_static_rtdebug=yes])

  if test "$COMPILE_LEVEL" = "release"; then
    AC_MSG_RESULT([skipping, debug disabled]) 
  elif test "have_rtdebug_lib" = "no"; then
    AC_MSG_RESULT([skipping, no rtdebug library found])
  elif test "$test_on_enable_static_rtdebug" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticrtdebug"
    AC_MSG_RESULT(yes)
    ac_rtdebug_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_rtdebug_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_MEDIO: provides a switch to control the link level (static/shared)
dnl of a linked medio library
dnl
AC_DEFUN([AC_ENABLE_STATIC_MEDIO],
[
  AC_MSG_CHECKING(whether to link the medio library static)
  AC_ARG_ENABLE(static-medio,
                [AC_HELP_STRING([--enable-static-medio], [turn on static linkage of medio libs [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_medio=yes  ;;
                  no)  test_on_enable_static_medio=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-medio) ;;
                esac],
                [test_on_enable_static_medio=yes])

  if test "$test_on_enable_static_medio" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticmedio"
    AC_MSG_RESULT(yes)
    ac_medio_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_medio_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_GSL: provides a switch to control the link level (static/shared)
dnl of a linked gsl library
dnl
AC_DEFUN([AC_ENABLE_STATIC_GSL],
[
  AC_MSG_CHECKING(whether to link the gsl library static)
  AC_ARG_ENABLE(static-gsl,
                [AC_HELP_STRING([--enable-static-gsl], [turn on static linkage of gsl libs [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_gsl=yes  ;;
                  no)  test_on_enable_static_gsl=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-gsl) ;;
                esac],
                [test_on_enable_static_gsl=yes])

  if test "$test_on_enable_static_gsl" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticgsl"
    AC_MSG_RESULT(yes)
    ac_gsl_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_gsl_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_NCURSES: provides a switch to control the link level (static/shared)
dnl of a linked ncurses library
dnl
AC_DEFUN([AC_ENABLE_STATIC_NCURSES],
[
  AC_MSG_CHECKING(whether to link the ncurses library static)
  AC_ARG_ENABLE(static-ncurses,
                [AC_HELP_STRING([--enable-static-ncurses], [turn on static linkage of ncurses lib [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_ncurses=yes  ;;
                  no)  test_on_enable_static_ncurses=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-ncurses) ;;
                esac],
                [test_on_enable_static_ncurses=yes])

  if test "$test_on_enable_static_ncurses" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticncurses"
    AC_MSG_RESULT(yes)
    ac_ncurses_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_ncurses_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_LIBMEDLM: provides a switch to control the link level (static/shared)
dnl of a linked libmedlm library
dnl
AC_DEFUN([AC_ENABLE_STATIC_MEDLM],
[
  AC_MSG_CHECKING(whether to link the medlm library static)
  AC_ARG_ENABLE(static-medlm,
                [AC_HELP_STRING([--enable-static-medlm], [turn on static linkage of medlm lib [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_medlm=yes  ;;
                  no)  test_on_enable_static_medlm=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-medlm) ;;
                esac],
                [test_on_enable_static_medlm=yes])

  if test "$test_on_enable_static_medlm" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticmedlm"
    AC_MSG_RESULT(yes)
    ac_medlm_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_medlm_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_LIBMTRACK: provides a switch to control the link level (static/shared)
dnl of a linked libmtrack library
dnl
AC_DEFUN([AC_ENABLE_STATIC_MTRACK],
[
  AC_MSG_CHECKING(whether to link the mtrack library static)
  AC_ARG_ENABLE(static-mtrack,
                [AC_HELP_STRING([--enable-static-mtrack], [turn on static linkage of mtrack lib [default=yes]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_mtrack=yes ;;
                  no)  test_on_enable_static_mtrack=no  ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-mtrack) ;;
                esac],
                [test_on_enable_static_mtrack=yes])

  if test "$test_on_enable_static_mtrack" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticmtrack"
    AC_MSG_RESULT(yes)
    ac_mtrack_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_mtrack_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_RCPP: provides a switch to control the link level (static/shared)
dnl of a linked Rcpp library
dnl
AC_DEFUN([AC_ENABLE_STATIC_RCPP],
[
  AC_MSG_CHECKING(whether to link the rcpp library static)
  AC_ARG_ENABLE(static-rcpp,
                [AC_HELP_STRING([--enable-static-rcpp], [turn on static linkage of rcpp libs [default=no]])],
                [case "${enableval}" in
                  yes) test_on_enable_static_rcpp=yes  ;;
                  no)  test_on_enable_static_rcpp=no ;;
                  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-static-rcpp) ;;
                esac],
                [test_on_enable_static_rcpp=no])


  if test "$R_INTERFACE" = "disabled"; then
    AC_MSG_RESULT([skipping, rinterface disabled]) 
  elif test "$test_on_enable_static_rcpp" = "yes" -o "$ac_static_build" = "yes"; then
    QTLINK_LEVEL="${QTLINK_LEVEL} staticrcpp"
    AC_MSG_RESULT(yes)
    ac_rcpp_link_level="static"
  else
    QTLINK_LEVEL="${QTLINK_LEVEL}"
    AC_MSG_RESULT(no)
    ac_rcpp_link_level="shared"
  fi

  AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_PROG_GCC_VERSION: finds out if the used gcc version is a supported one
dnl or not
dnl
AC_DEFUN([AC_PROG_GCC_VERSION],
[
 AC_MSG_CHECKING([for gcc version])

 dnl check if $CXX exists or not
 if eval $CXX -v 2>/dev/null >/dev/null; then
   dnl Check if version of gcc is sufficient
   cc_name=`( $CXX -v ) 2>&1 | tail -n 1 | cut -d ' ' -f 1`
   cc_version=`( $CXX -dumpversion ) 2>&1`
   if test "$?" -gt 0; then
     cc_version="not found"
   fi
   changequote(,)dnl   
   case $cc_version in
     '')
       cc_version="v. ?.??, bad"
       cc_verc_fail=yes
       ;;
     2.95.[2-9]|2.95.[2-9][-.]*|3.[0-9]*|3.[0-9].[0-9]*|4.[0-9]*)
       _cc_major=`echo $cc_version | cut -d '.' -f 1`
       _cc_minor=`echo $cc_version | cut -d '.' -f 2`
       _cc_mini=`echo $cc_version | cut -d '.' -f 3`
       cc_version="$cc_version, ok"
       cc_verc_fail=no
       ;;
     'not found')
       cc_verc_fail=yes
       ;;
     *)
       cc_version="$cc_version, bad"
       cc_verc_fail=yes
       ;;
   esac
   changequote([, ])dnl

   if test "$cc_verc_fail" = yes ; then
     AC_MSG_RESULT([$cc_version])
     AC_MSG_ERROR([gcc version check failed])
   else
     AC_MSG_RESULT([$cc_version])
     GCC_VERSION=$_cc_major
   fi
 else
   AC_MSG_RESULT(FAILED)
   AC_MSG_ERROR([gcc was not found '$CXX'])
 fi

 AC_SUBST(GCC_VERSION)
])

dnl
dnl AC_PATH_QRTDEBUG: allows to override the default library search path for
dnl searching for the rtdebug library.
dnl
AC_DEFUN([AC_PATH_RTDEBUG],
[
  AC_ARG_WITH(rtdebug, [AC_HELP_STRING([--with-rtdebug], [where the rtdebug environment is located.])],
                       [RTDEBUGDIR="$withval" ])
])

dnl
dnl AC_PATH_RTDEBUG_LIB: checks for the existance of the rtdebug library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN([AC_PATH_RTDEBUG_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(rtdebug-lib,
              [AC_HELP_STRING([--with-rtdebug-lib], [where the rtdebug library is located.])],
              [ac_rtdebug_libraries="$withval"], ac_rtdebug_libraries="")

  AC_MSG_CHECKING(for runtime debugging library)

  AC_CACHE_VAL(ac_cv_lib_rtdebuglib, [

  rtdebug_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_rtdebug_libraries"; then
    rtdebug_library_dirs="$RTDEBUGDIR/lib \
                          $RTDEBUGDIR/rtdebug \
                          $RTDEBUGDIR \
                          /usr/local/petlib/lib \
                          /usr/local/petlib/lib/rtdebug \ 
                          /usr/local/lib \
                          /usr/local/lib/rtdebug \
                          /usr/lib \
                          /usr/lib/rtdebug \
                          /Developer/rtdebug/lib \
                          C:/petlib/lib"
  else
    rtdebug_library_dirs="$ac_rtdebug_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the rtdebug library in one of
  dnl our search pathes
  ac_rtdebug_libdir=""
  if test "$ac_rtdebug_link_level" = "static"; then
    ac_rtdebug_libname="librtdebug.a"
    LIB_RTDEBUG="$ac_rtdebug_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_rtdebug_libname="librtdebug.dylib"
    else
      ac_rtdebug_libname="librtdebug.so"
    fi
    LIB_RTDEBUG="-lrtdebug"
  fi

  for rtdebug_dir in $rtdebug_library_dirs; do
    if test -r "$rtdebug_dir/$ac_rtdebug_libname"; then
      ac_rtdebug_libdir="$rtdebug_dir"
      break;
    else
      echo "tried $rtdebug_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_rtdebuglib="ac_rtdebug_libname=$ac_rtdebug_libname ac_rtdebug_libdir=$ac_rtdebug_libdir"
  
  ])

  eval "$ac_cv_lib_rtdebuglib"

  dnl Define a shell variable for later checks
  if test "$COMPILE_LEVEL" = "release"; then
    have_rtdebug_lib="no"
    AC_MSG_RESULT([skipping, debug disabled]) 
  elif test -z "$ac_rtdebug_libdir"; then
    have_rtdebug_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_rtdebug_link_level rtdebug library in linker path.
Try --with-rtdebug-lib to specify the path, manually.])
  else
    have_rtdebug_lib="yes"
    AC_MSG_RESULT([yes, $ac_rtdebug_libname in $ac_rtdebug_libdir found.])
  fi

  RTDEBUG_LDFLAGS="-L$ac_rtdebug_libdir"
  RTDEBUG_LIBDIR="$ac_rtdebug_libdir"
  AC_SUBST(RTDEBUG_LDFLAGS)
  AC_SUBST(RTDEBUG_LIBDIR)
  AC_SUBST(LIB_RTDEBUG)
])

dnl
dnl AC_PATH_RTDEBUG_INC: checks the existance of the includes files for successfully
dnl compiling support for the rtdebug library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_RTDEBUG_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for rtdebug.h include)

  AC_ARG_WITH(rtdebug-inc,
              [AC_HELP_STRING([--with-rtdebug-inc], [where the rtdebug headers are located.])],
              [rtdebug_include_dirs="$withval"], rtdebug_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_rtdebuginc, [

    dnl Did the user give --with-rtdebug-includes?
    if test -z "$rtdebug_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      rtdebug_include_dirs="\
        $RTDEBUGDIR/include \
        $RTDEBUGDIR/include/rtdebug \
        $RTDEBUGDIR \
        /usr/local/petlib/include \
        /usr/local/petlib/include/rtdebug \   
        /usr/local/rtdebug/include \
        /usr/include/rtdebug \
        /usr/lib/rtdebug/include \
        /usr/local/include/rtdebug \
        C:/petlib/include/rtdebug"
    fi

    for rtdebug_dir in $rtdebug_include_dirs; do
      if test -r "$rtdebug_dir/rtdebug.h"; then
        ac_rtdebug_includes=$rtdebug_dir
        break;
      fi
    done

    ac_cv_header_rtdebuginc=$ac_rtdebug_includes

  ])

  if test -z "$ac_cv_header_rtdebuginc"; then
    have_rtdebug_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([rtdebug.h include not found, you may run into problems.
Try --with-rtdebug-inc to specify the path, manually.])
  else
    have_rtdebug_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_rtdebuginc])
  fi

  RTDEBUG_INCLUDES="-I$ac_cv_header_rtdebuginc"
  RTDEBUG_INCDIR="$ac_cv_header_rtdebuginc"
  AC_SUBST(RTDEBUG_INCLUDES)
  AC_SUBST(RTDEBUG_INCDIR)
])

dnl
dnl AC_PATH_MEDIO: allows to override the default library search path for
dnl searching for the medio library.
dnl
AC_DEFUN([AC_PATH_MEDIO],
[
  AC_ARG_WITH(medio, [AC_HELP_STRING([--with-medio], [where the medio environment is located.])],
                     [MEDIODIR="$withval" ])
])

dnl
dnl AC_PATH_MEDIO_LIB: checks for the existance of the medio library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN([AC_PATH_MEDIO_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(medio-lib,
              [AC_HELP_STRING([--with-medio-lib], [where the libmedio library is located.])],
              [ac_medio_libraries="$withval"], ac_medio_libraries="")

  AC_MSG_CHECKING(for medical IO library)

  AC_CACHE_VAL(ac_cv_lib_mediolib, [

  medio_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_medio_libraries"; then
    medio_library_dirs="$MEDIODIR/lib \
                        $MEDIODIR/medio \
                        $MEDIODIR \
                        /usr/local/petlib/lib \
                        /usr/local/petlib/lib/medio \ 
                        /usr/local/lib \
                        /usr/local/lib/medio \
                        /usr/lib \
                        /usr/lib/medio \
                        /Developer/medio/lib"
  else
    medio_library_dirs="$ac_medio_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the medio library in one of
  dnl our search pathes
  ac_medio_libdir=""
  if test "$ac_medio_link_level" = "static"; then
    ac_medio_libname="libmedio.a"
    LIB_MEDIO="$ac_medio_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_medio_libname="libmedio.dylib"
    else
      ac_medio_libname="libmedio.so"
    fi
    LIB_MEDIO="-lmedio"
  fi

  for medio_dir in $medio_library_dirs; do
    if test -r "$medio_dir/$ac_medio_libname"; then
      ac_medio_libdir="$medio_dir"
      break;
    else
      echo "tried $medio_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_mediolib="ac_medio_libname=$ac_medio_libname ac_medio_libdir=$ac_medio_libdir"
  ])

  eval "$ac_cv_lib_mediolib"

  dnl Define a shell variable for later checks
  if test -z "$ac_medio_libdir"; then
    have_medio_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_medio_link_level medio library in linker path.
Try --with-medio-lib to specify the path, manually.])
  else
    have_medio_lib="yes"
    AC_MSG_RESULT([yes, $ac_medio_libname in $ac_medio_libdir found.])
  fi

  MEDIO_LDFLAGS="-L$ac_medio_libdir"
  MEDIO_LIBDIR="$ac_medio_libdir"
  AC_SUBST(MEDIO_LDFLAGS)
  AC_SUBST(MEDIO_LIBDIR)
  AC_SUBST(LIB_MEDIO)
])

dnl
dnl AC_PATH_MEDIO_INC: checks the existance of the includes files for successfully
dnl compiling support for the medio library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_MEDIO_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for libmedio includes)

  AC_ARG_WITH(medio-inc,
              [AC_HELP_STRING([--with-medio-inc], [where the libmedio headers are located.])],
              [medio_include_dirs="$withval"], medio_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_medioinc, [

    dnl Did the user give --with-medio-includes?
    if test -z "$medio_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      medio_include_dirs="\
        $MEDIODIR/include \
        $MEDIODIR/include/rtdebug \
        $MEDIODIR \     
        /usr/local/petlib/include \
        /usr/local/petlib/include/medio \         
        /usr/local/include \
        /usr/local/include/medio \
        /usr/include/medio \
        /usr/lib/medio/include"
    fi

    for medio_dir in $medio_include_dirs; do
      if test -r "$medio_dir/CMedIO"; then
        if test -r "$medio_dir/CMedIOData"; then
          ac_medio_includes=$medio_dir
          break;
        fi
      fi
    done

    ac_cv_header_medioinc=$ac_medio_includes

  ])

  if test -z "$ac_cv_header_medioinc"; then
    have_medio_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([libmedio include directory not found, you may run into problems.
Try --with-medio-inc to specify the path, manually.])
  else
    have_medio_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_medioinc])
  fi

  MEDIO_INCLUDES="-I$ac_cv_header_medioinc"
  MEDIO_INCDIR="$ac_cv_header_medioinc"
  AC_SUBST(MEDIO_INCLUDES)
  AC_SUBST(MEDIO_INCDIR)
])

dnl
dnl AC_PATH_GSL: allows to override the default library search path for
dnl searching for the medio library.
dnl
AC_DEFUN([AC_PATH_GSL],
[
  AC_ARG_WITH(gsl, [AC_HELP_STRING([--with-gsl], [where the gsl environment is located.])],
                   [GSLDIR="$withval" ])
])

dnl
dnl AC_PATH_GSL_LIB: checks for the existance of the gsl library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN([AC_PATH_GSL_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(gsl-lib,
              [AC_HELP_STRING([--with-gsl-lib], [where the GSL library is located.])],
              [ac_gsl_libraries="$withval"], ac_gsl_libraries="")

  AC_MSG_CHECKING(for GNU Scientific library)

  AC_CACHE_VAL(ac_cv_lib_gsllib, [

  gsl_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_gsl_libraries"; then
    gsl_library_dirs="$GSLDIR/lib \
                      $GSLDIR/gsl \
                      $GSLDIR \
                      /usr/local/lib \
                      /usr/local/lib/gsl \
                      /usr/lib \
                      /usr/lib/gsl \
                      /Developer/gsl/lib"
  else
    gsl_library_dirs="$ac_gsl_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the gsl library in one of
  dnl our search pathes
  ac_gsl_libdir=""
  if test "$ac_gsl_link_level" = "static"; then
    ac_gsl_libname="libgsl.a"
    LIB_GSL="$ac_gsl_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_gsl_libname="libgsl.dylib"
    else
      ac_gsl_libname="libgsl.so"
    fi
    LIB_GSL="-lgsl -lgslcblas"
  fi

  for gsl_dir in $gsl_library_dirs; do
    if test -r "$gsl_dir/$ac_gsl_libname"; then
      ac_gsl_libdir="$gsl_dir"
      break;
    else
      echo "tried $gsl_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_gsllib="ac_gsl_libname=\"$ac_gsl_libname\" ac_gsl_libdir=\"$ac_gsl_libdir\""
  ])

  eval "$ac_cv_lib_gsllib"

  dnl Define a shell variable for later checks
  if test -z "$ac_gsl_libdir"; then
    have_gsl_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_gsl_link_level gsl library in linker path.
Try --with-gsl-lib to specify the path, manually.])
  else
    have_gsl_lib="yes"
    AC_MSG_RESULT([yes, $ac_gsl_libname in $ac_gsl_libdir found.])
  fi

  GSL_LDFLAGS="-L$ac_gsl_libdir"
  GSL_LIBDIR="$ac_gsl_libdir"
  AC_SUBST(GSL_LDFLAGS)
  AC_SUBST(GSL_LIBDIR)
  AC_SUBST(LIB_GSL)
])

dnl
dnl AC_PATH_GSL_INC: checks the existance of the includes files for successfully
dnl compiling support for the gsl library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_GSL_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for libgsl includes)

  AC_ARG_WITH(gsl-inc,
              [AC_HELP_STRING([--with-gsl-inc], [where the libgsl headers are located.])],
              [gsl_include_dirs="$withval"], gsl_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_gslinc, [

    dnl Did the user give --with-gsl-includes?
    if test -z "$gsl_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      gsl_include_dirs="\
        $GSLDIR/include \
        $GSLDIR \           
        /usr/local/gsl/include \
        /usr/include/ \
        /usr/lib/gsl/include \
        /usr/local/include/"
    fi

    for gsl_dir in $gsl_include_dirs; do
      if test -r "$gsl_dir/gsl/gsl_version.h"; then
        if test -r "$gsl_dir/gsl/gsl_types.h"; then
          ac_gsl_includes=$gsl_dir
          break;
        fi
      fi
    done

    ac_cv_header_gslinc=$ac_gsl_includes

  ])

  if test -z "$ac_cv_header_gslinc"; then
    have_gsl_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([libgsl include directory not found, you may run into problems.
Try --with-gsl-inc to specify the path, manually.])
  else
    have_gsl_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_gslinc])
  fi

  GSL_INCLUDES="-I$ac_cv_header_gslinc"
  GSL_INCDIR="$ac_cv_header_gslinc"
  AC_SUBST(GSL_INCLUDES)
  AC_SUBST(GSL_INCDIR)
])

dnl
dnl AC_PATH_NCURSES: allows to override the default library search path for
dnl searching for the ncurses library.
dnl
AC_DEFUN([AC_PATH_NCURSES],
[
  AC_ARG_WITH(ncurses, [AC_HELP_STRING([--with-ncurses], [where the ncurses environment is located.])],
                       [NCURSESDIR="$withval" ])
])

dnl
dnl AC_PATH_NCURSES_LIB: checks for the existance of the ncurses library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN([AC_PATH_NCURSES_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(ncurses-lib,
              [AC_HELP_STRING([--with-ncurses-lib], [where the ncurses library is located.])],
              [ac_ncurses_libraries="$withval"], ac_ncurses_libraries="")

  AC_MSG_CHECKING(for ncurses library)

  AC_CACHE_VAL(ac_cv_lib_ncurseslib, [

  ncurses_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_ncurses_libraries"; then
    ncurses_library_dirs="$NCURSESDIR/lib \
                      $NCURSESDIR/ncurses \
                      $NCURSESDIR \
                      /usr/local/lib \
                      /usr/lib \
                      /Developer/ncurses/lib"
  else
    ncurses_library_dirs="$ac_ncurses_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the ncurses library in one of
  dnl our search pathes
  ac_ncurses_libdir=""
  if test "$ac_ncurses_link_level" = "static"; then
    ac_ncurses_libname="libncurses.a"
    LIB_NCURSES="$ac_ncurses_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_ncurses_libname="libncurses.dylib"
    else
      ac_ncurses_libname="libncurses.so"
    fi
    LIB_NCURSES="-lncurses"
  fi

  for ncurses_dir in $ncurses_library_dirs; do
    if test -r "$ncurses_dir/$ac_ncurses_libname"; then
      ac_ncurses_libdir="$ncurses_dir"
      break;
    else
      echo "tried $ncurses_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_ncurseslib="ac_ncurses_libname=\"$ac_ncurses_libname\" ac_ncurses_libdir=\"$ac_ncurses_libdir\""
  ])

  eval "$ac_cv_lib_ncurseslib"

  dnl Define a shell variable for later checks
  if test -z "$ac_ncurses_libdir"; then
    have_ncurses_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_ncurses_link_level ncurses library in linker path.
Try --with-ncurses-lib to specify the path, manually.])
  else
    have_ncurses_lib="yes"
    AC_MSG_RESULT([yes, $ac_ncurses_libname in $ac_ncurses_libdir found.])
  fi

  NCURSES_LDFLAGS="-L$ac_ncurses_libdir"
  NCURSES_LIBDIR="$ac_ncurses_libdir"
  AC_SUBST(NCURSES_LDFLAGS)
  AC_SUBST(NCURSES_LIBDIR)
  AC_SUBST(LIB_NCURSES)
])

dnl
dnl AC_PATH_NCURSES_INC: checks the existance of the includes files for successfully
dnl compiling support for the ncurses library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_NCURSES_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for libncurses includes)

  AC_ARG_WITH(ncurses-inc,
              [AC_HELP_STRING([--with-ncurses-inc], [where the libncurses headers are located.])],
              [ncurses_include_dirs="$withval"], ncurses_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_ncursesinc, [

    dnl Did the user give --with-ncurses-includes?
    if test -z "$ncurses_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      ncurses_include_dirs="\
        $NCURSESDIR/include/ncurses \
        $NCURSESDIR/ncurses \           
        /usr/local/include/ncurses \
        /usr/local/include \
        /usr/include/ncurses \
        /usr/include \
        /usr/lib/ncurses/include"
    fi

    for ncurses_dir in $ncurses_include_dirs; do
      if test -r "$ncurses_dir/ncurses.h"; then
        ac_ncurses_includes=$ncurses_dir
        break;
      fi
    done

    ac_cv_header_ncursesinc=$ac_ncurses_includes

  ])

  if test -z "$ac_cv_header_ncursesinc"; then
    have_ncurses_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([libncurses include directory not found, you may run into problems.
Try --with-ncurses-inc to specify the path, manually.])
  else
    have_ncurses_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_ncursesinc])
  fi

  NCURSES_INCLUDES="-I$ac_cv_header_ncursesinc"
  NCURSES_INCDIR="$ac_cv_header_ncursesinc"
  AC_SUBST(NCURSES_INCLUDES)
  AC_SUBST(NCURSES_INCDIR)
])


dnl
dnl AC_PATH_MEDLM: allows to override the default library search path for
dnl searching for the libmedlm library.
dnl
AC_DEFUN([AC_PATH_MEDLM],
[
  AC_ARG_WITH(medlm, [AC_HELP_STRING([--with-medlm], [where the libmedlm environment is located.])],
                     [MEDLMDIR="$withval" ])
])

dnl
dnl AC_PATH_MEDLM_LIB: checks for the existance of the libmedlm library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN([AC_PATH_MEDLM_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(medlm-lib,
              [AC_HELP_STRING([--with-medlm-lib], [where the medlm library is located.])],
              [ac_medlm_libraries="$withval"], ac_medlm_libraries="")

  AC_MSG_CHECKING(for listmode library)

  AC_CACHE_VAL(ac_cv_lib_medlmlib, [

  medlm_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_medlm_libraries"; then
    medlm_library_dirs="$MEDLMDIR/lib \
                        $MEDLMDIR/lm \
                        $MEDLMDIR \
                        /usr/local/petlib/lib \
                        /usr/local/petlib/lib/lm \  
                        /usr/local/lib \
                        /usr/local/lib/lm \
                        /usr/lib \
                        /usr/lib/lm \
                        /Developer/lm/lib"
  else
    medlm_library_dirs="$ac_medlm_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the libmedlm library in one of
  dnl our search pathes
  ac_medlm_libdir=""
  if test "$ac_medlm_link_level" = "static"; then
    ac_medlm_libname="libmedlm.a"
    LIB_MEDLM="$ac_medlm_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_medlm_libname="libmedlm.dylib"
    else
      ac_medlm_libname="libmedlm.so"
    fi
    LIB_MEDLM="-lmedlm"
  fi

  for medlm_dir in $medlm_library_dirs; do
    if test -r "$medlm_dir/$ac_medlm_libname"; then
      ac_medlm_libdir="$medlm_dir"
      break;
    else
      echo "tried $medlm_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_medlmlib="ac_medlm_libname=$ac_medlm_libname ac_medlm_libdir=$ac_medlm_libdir"
  ])

  eval "$ac_cv_lib_medlmlib"

  dnl Define a shell variable for later checks
  if test -z "$ac_medlm_libdir"; then
    have_medlm_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_medlm_link_level listmode library (libmedlm) in linker path.
Try --with-medlm-lib to specify the path, manually.])
  else
    have_medlm_lib="yes"
    AC_MSG_RESULT([yes, $ac_medlm_libname in $ac_medlm_libdir found.])
  fi

  MEDLM_LDFLAGS="-L$ac_medlm_libdir"
  MEDLM_LIBDIR="$ac_medlm_libdir"
  AC_SUBST(MEDLM_LDFLAGS)
  AC_SUBST(MEDLM_LIBDIR)
  AC_SUBST(LIB_MEDLM)
])

dnl
dnl AC_PATH_MEDLM_INC: checks the existance of the includes files for successfully
dnl compiling support for the libmedlm library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_MEDLM_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for libmedlm includes)

  AC_ARG_WITH(medlm-inc,
              [AC_HELP_STRING([--with-medlm-inc], [where the libmedlm headers are located.])],
              [medlm_include_dirs="$withval"], medlm_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_medlminc, [

    dnl Did the user give --with-medlm-includes?
    if test -z "$medlm_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      medlm_include_dirs="\
        $MEDLMDIR/include \
        $MEDLMDIR/include/medlm \
        $MEDLMDIR \     
        /usr/local/petlib/include \
        /usr/local/petlib/include/medlm \         
        /usr/local/include \
        /usr/local/include/medlm \
        /usr/include/medlm \
        /usr/lib/medlm/include"
    fi

    for medlm_dir in $medlm_include_dirs; do
      if test -r "$medlm_dir/CListModeFile.h"; then
        if test -r "$medlm_dir/CSinglesFile.h"; then
          ac_medlm_includes=$medlm_dir
          break;
        fi
      fi
    done

    ac_cv_header_medlminc=$ac_medlm_includes

  ])

  if test -z "$ac_cv_header_medlminc"; then
    have_medlm_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([libmedlm include directory not found, you may run into problems.
Try --with-medlm-inc to specify the path, manually.])
  else
    have_medlm_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_medlminc])
  fi

  MEDLM_INCLUDES="-I$ac_cv_header_medlminc"
  MEDLM_INCDIR="$ac_cv_header_medlminc"
  AC_SUBST(MEDLM_INCLUDES)
  AC_SUBST(MEDLM_INCDIR)
])

dnl
dnl AC_PATH_MTRACK: allows to override the default library search path for
dnl searching for the libmtrack library.
dnl
AC_DEFUN([AC_PATH_MTRACK],
[
  AC_ARG_WITH(mtrack, [AC_HELP_STRING([--with-mtrack], [where the libmtrack environment is located.])],
                      [MTRACKDIR="$withval" ])
])

dnl
dnl AC_PATH_MTRACK_LIB: checks for the existance of the libmtrack library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN([AC_PATH_MTRACK_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(mtrack-lib,
              [AC_HELP_STRING([--with-mtrack-lib], [where the mtrack library is located.])],
              [ac_mtrack_libraries="$withval"], ac_mtrack_libraries="")

  AC_MSG_CHECKING(for motion-tracking library)

  AC_CACHE_VAL(ac_cv_lib_mtracklib, [

  mtrack_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_mtrack_libraries"; then
    mtrack_library_dirs="$MTRACKDIR/lib \
                         $MTRACKDIR/mtrack \
                         $MTRACKDIR \
                         /usr/local/petlib/lib \
                         /usr/local/petlib/lib/mtrack \ 
                         /usr/local/lib \
                         /usr/local/lib/mtrack \
                         /usr/lib \
                         /usr/lib/mtrack \
                         /Developer/mtrack/lib"
  else
    mtrack_library_dirs="$ac_mtrack_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the libmtrack library in one of
  dnl our search pathes
  ac_mtrack_libdir=""
  if test "$ac_mtrack_link_level" = "static"; then
    ac_mtrack_libname="libmtrack.a"
    LIB_MTRACK="$ac_mtrack_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_mtrack_libname="libmtrack.dylib"
    else
      ac_mtrack_libname="libmtrack.so"
    fi
    LIB_MTRACK="-lmtrack"
  fi

  for mtrack_dir in $mtrack_library_dirs; do
    if test -r "$mtrack_dir/$ac_mtrack_libname"; then
      ac_mtrack_libdir="$mtrack_dir"
      break;
    else
      echo "tried $mtrack_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_mtracklib="ac_mtrack_libname=$ac_mtrack_libname ac_mtrack_libdir=$ac_mtrack_libdir"
  ])

  eval "$ac_cv_lib_mtracklib"

  dnl Define a shell variable for later checks
  if test -z "$ac_mtrack_libdir"; then
    have_mtrack_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_mtrack_link_level motion-tracking library (libmtrack) in linker path.
Try --with-mtrack-lib to specify the path, manually.])
  else
    have_mtrack_lib="yes"
    AC_MSG_RESULT([yes, $ac_mtrack_libname in $ac_mtrack_libdir found.])
  fi

  MTRACK_LDFLAGS="-L$ac_mtrack_libdir"
  MTRACK_LIBDIR="$ac_mtrack_libdir"
  AC_SUBST(MTRACK_LDFLAGS)
  AC_SUBST(MTRACK_LIBDIR)
  AC_SUBST(LIB_MTRACK)
])

dnl
dnl AC_PATH_MTRACK_INC: checks the existance of the includes files for successfully
dnl compiling support for the libmtrack library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_MTRACK_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for libmtrack includes)

  AC_ARG_WITH(mtrack-inc,
              [AC_HELP_STRING([--with-mtrack-inc], [where the libmtrack headers are located.])],
              [mtrack_include_dirs="$withval"], mtrack_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_mtrackinc, [

    dnl Did the user give --with-mtrack-includes?
    if test -z "$mtrack_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      mtrack_include_dirs="\
        $MTRACKDIR/include \
        $MTRACKDIR/include/mtrack \
        $MTRACKDIR \      
        /usr/local/petlib/include \
        /usr/local/petlib/include/mtrack \          
        /usr/local/include \
        /usr/local/include/mtrack \
        /usr/include/mtrack \
        /usr/lib/mtrack/include"
    fi

    for mtrack_dir in $mtrack_include_dirs; do
      if test -r "$mtrack_dir/CMotionData.h"; then
        if test -r "$mtrack_dir/CTransMatrix.h"; then
          ac_mtrack_includes=$mtrack_dir
          break;
        fi
      fi
    done

    ac_cv_header_mtrackinc=$ac_mtrack_includes

  ])

  if test -z "$ac_cv_header_mtrackinc"; then
    have_mtrack_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([libmtrack include directory not found, you may run into problems.
Try --with-mtrack-inc to specify the path, manually.])
  else
    have_mtrack_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_mtrackinc])
  fi

  MTRACK_INCLUDES="-I$ac_cv_header_mtrackinc"
  MTRACK_INCDIR="$ac_cv_header_mtrackinc"
  AC_SUBST(MTRACK_INCLUDES)
  AC_SUBST(MTRACK_INCDIR)
])

dnl
dnl AC_QTSHAREDBUILD: tries to find out if the used Qt3 library was build
dnl as a shared or static library
dnl
AC_DEFUN([AC_QTSHAREDBUILD],
[
  AC_REQUIRE_CPP()
  AC_REQUIRE([AC_PATH_QT3_INC])
  AC_REQUIRE([AC_PATH_QT3_LIB])

  AC_MSG_CHECKING(for qSharedBuild in Qt3 lib qt-mt)
  
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
 
  ac_cv_lib_qtlib="ac_qt_libname=$ac_qt_libname ac_qt_libdir=$ac_qt_libdir"
  dnl Save some global vars
  save_CXXFLAGS="$CXXFLAGS"
  save_LDFLAGS="$LDFLAGS"
  save_LIBS="$LIBS"

  LIBS="$ac_qt_libname $save_LIBS"
  LDFLAGS="-L$ac_qt_libdir $save_LDFLAGS"
  CXXFLAGS="-I$ac_cv_header_qtinc $save_CXXFLAGS"

  AC_TRY_LINK([#include <qglobal.h>], qSharedBuild();, ac_have_qsharedbuild="yes", ac_have_qsharedbuild="no")

  dnl Define a shell variable for later checks
  if test "$ac_have_qsharedbuild" = "no"; then
    AC_MSG_RESULT([no])
  else
    AC_MSG_RESULT([yes])
    AC_DEFINE(HAVE_QSHAREDBUILD)
  fi

  dnl Restore the saved vars
  CXXFLAGS="$save_CXXFLAGS"
  LDFLAGS="$save_LDFLAGS"
  LIBS="$save_LIBS"

  AC_LANG_RESTORE
])

dnl
dnl AC_PATH_QT3DIR: allows to override the Qt3 specific QTDIR variable for
dnl specifying the main directory where Qt3 is installed
dnl
AC_DEFUN([AC_PATH_QT3DIR],
[
  AC_ARG_WITH(qt3, [AC_HELP_STRING([--with-qt3], [where the Qt3 multithreaded library is located.])],
                   [QTDIR="$withval" ])

  if test -z "$QTDIR"; then
    AC_MSG_WARN([environment variable QTDIR is not set, you may run into problems])
  fi

])

dnl
dnl AC_PATH_QT3_LIB: tries to check for the existenance of the Qt3 libraries and
dnl also provides means of overriding the default directory in which we are going
dnl to search for the Qt3 libs
dnl
AC_DEFUN([AC_PATH_QT3_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(qt3-lib,[AC_HELP_STRING([--with-qt3-lib], [where the Qt3 multithreaded library is located.])],
                      [ac_qt_libraries="$withval"], ac_qt_libraries="")

  AC_MSG_CHECKING(for Qt3 multithreaded library)

  AC_CACHE_VAL(ac_cv_lib_qtlib, [

  qt_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_qt_libraries"; then
    if test -n "$QTDIR"; then
      qt_library_dirs="$QTDIR/lib $qt_library_dirs"
    fi

    if test -n "$QTLIB"; then
      qt_library_dirs="$QTLIB $qt_library_dirs"
    fi

    qt_library_dirs="$qt_library_dirs \
                     /usr/qt/lib \
                     /usr/local/lib/qt \
                     /usr/lib/qt3/lib \
                     /usr/lib \
                     /usr/local/lib \
                     /usr/lib/qt \
                     /usr/lib/qt/lib \
                     /usr/local/lib/qt \
                     /usr/X11/lib \
                     /usr/X11/lib/qt \
                     /usr/X11R6/lib \
                     /usr/X11R6/lib/qt \
                     /Developer/qt/lib"
  else
    qt_library_dirs="$ac_qt_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the Qt3 library in one of
  dnl our search pathes
  ac_qt_libdir=""
  if test "$ac_qt_link_level" = "static"; then
    ac_qt_libname="libqt-mt.a"
    LIB_QT="$ac_qt_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_qt_libname="libqt-mt.dylib"
    else
      ac_qt_libname="libqt-mt.so"
    fi
    LIB_QT="-lqt-mt"
  fi

  for qt_dir in $qt_library_dirs; do
    if test -r "$qt_dir/$ac_qt_libname"; then
      ac_qt_libdir="$qt_dir"
      break;
    else
      echo "tried $qt_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_qtlib="ac_qt_libname=$ac_qt_libname ac_qt_libdir=$ac_qt_libdir"
  ])

  eval "$ac_cv_lib_qtlib"

  dnl Define a shell variable for later checks
  if test -z "$ac_qt_libdir"; then
    have_qt_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_qt_link_level Qt3 multithreaded library in linker path.
Try --with-qt-lib to specify the path, manually.])
  else
    have_qt_lib="yes"
    AC_MSG_RESULT([yes, lib: $ac_qt_libname in $ac_qt_libdir])
  fi

  QT_LDFLAGS="-L$ac_qt_libdir"
  QT_LIBDIR="$ac_qt_libdir"
  AC_SUBST(QT_LDFLAGS)
  AC_SUBST(QT_LIBDIR)
  AC_SUBST(LIB_QT)
])

dnl
dnl AC_PATH_QT3_INC: tries to find out if the Qt3 headers are reachable and provides
dnl means of overriding the default search pathes.
dnl
AC_DEFUN([AC_PATH_QT3_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for Qt3 includes)

  AC_ARG_WITH(qt3-inc,[AC_HELP_STRING([--with-qt3-inc], [where the Qt3 includes are located.])],
                      [qt_include_dirs="$withval"], qt_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_qtinc, [

    dnl Did the user give --with-qt-includes?
    if test -z "$qt_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      qt_include_dirs="\
        /usr/lib/qt3/include \
        /usr/lib/qt/include \
        /usr/include/qt \
        /usr/local/qt/include \
        /usr/local/include/qt \
        /usr/X11/include/qt \
        /usr/X11/include/X11/qt \
        /usr/X11R6/include \
        /usr/X11R6/include/qt \
        /usr/X11R6/include/X11/qt \
        /usr/X11/lib/qt/include"

      if test -n "$QTDIR"; then
        qt_include_dirs="$QTDIR/include $qt_include_dirs"
      fi

      if test -n "$QTINC"; then
        qt_include_dirs="$QTINC $qt_include_dirs"
      fi
    fi

    for qt_dir in $qt_include_dirs; do
      if test -r "$qt_dir/qt.h"; then
        if test -r "$qt_dir/qglobal.h"; then
          if test -r "$qt_dir/qbig5codec.h"; then
            if test -r "$qt_dir/qtranslatordialog.h"; then
              AC_MSG_ERROR([
                This is not Qt 2.1.0 or later. Somebody cheated you.

                Most likely this is because you've installed a crappy
                outdated Redhat 6.2 RPM. Go to ftp://people.redhat.com/bero/qt
                and update to the correct one.
              ])
            fi
          fi
          ac_qt_includes=$qt_dir
          break;
        fi
      fi
    done

    ac_cv_header_qtinc=$ac_qt_includes

  ])

  if test -z "$ac_cv_header_qtinc"; then
    have_qt_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([Qt3 include directory not found, you may run into problems.
Try --with-qt-inc to specify the path, manualy.])
  else
    have_qt_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_qtinc])
  fi

  QT_INCLUDES="-I$ac_cv_header_qtinc"
  QT_INCDIR="$ac_cv_header_qtinc"
  AC_SUBST(QT_INCLUDES)
  AC_SUBST(QT_INCDIR)
])

dnl
dnl AC_PATH_QT3_QMAKE: tries to find out if the "qmake" binary of Qt3 is reachable or not and
dnl allows to override the default path to it
dnl
AC_DEFUN([AC_PATH_QT3_QMAKE],
[
  AC_ARG_WITH(qt3-qmake,[AC_HELP_STRING([--with-qt3-qmake], [where the Qt3 qmake binary is located.])],
                        [ac_qt_qmake="$withval"], ac_qt_qmake="")

  if test -z "$ac_qt_qmake"; then
    dnl search on our own

    if test -z "$QTDIR"; then
      AC_MSG_WARN(environment variable QTDIR is not set, qmake might not be found)
    fi

    AC_PATH_PROG(
      QMAKE_PATH,
      qmake,
      $QTDIR/bin/qmake,
      $QTDIR/bin:/usr/lib/qt2/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:/Developer/qt/bin:$PATH
    )
  else
    AC_MSG_CHECKING(for qmake)

    if test -f $ac_qt_qmake && test -x $ac_qt_qmake; then
      QMAKE_PATH=$ac_qt_qmake
    else
      AC_MSG_ERROR(
        --with-qt3-qmake expects path and name of the qmake tool
      )
    fi

    AC_MSG_RESULT($QMAKE_PATH)
  fi

  if test -z "$QMAKE_PATH"; then
    AC_MSG_ERROR(couldn't find Qt3 qmake. Please use --with-qt3-qmake)
  fi

  dnl Check if we have the right qmake by outputing the version
  dnl information
  qmake_vers=`"$QMAKE_PATH" -v 2>&1 | grep "Qt 3"`
  if test -z "$qmake_vers"; then
    AC_MSG_ERROR([didn't find the correct Qt3 version of qmake, Please use --with-qt3-qmake])
  fi

  AC_SUBST(QMAKE_PATH)
])

dnl
dnl AC_PATH_QT4: allows to override the default library search path for
dnl searching for the Qt4 libraries/binaries and stuff.
dnl
AC_DEFUN([AC_PATH_QT4],
[
  AC_ARG_WITH(qt4, [AC_HELP_STRING([--with-qt4], [where the Qt4 environment is located.])],
                   [QT4DIR="$withval" ])
])


dnl
dnl AC_PATH_QT4_LIB: checks if the Qt4 libraries are reachable and provides means
dnl of overriding the default search path
dnl
AC_DEFUN([AC_PATH_QT4_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(qt4-lib,[AC_HELP_STRING([--with-qt4-lib], [where the Qt4 libraries are located.])],
                      [ac_qt_libraries="$withval"], ac_qt_libraries="")

  AC_MSG_CHECKING(for Qt4 libraries)

  AC_CACHE_VAL(ac_cv_lib_qtlib, [

  qt_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_qt_libraries"; then
    qt_library_dirs="$QT4DIR/lib \
                     /usr/lib/qt4 \
                     /usr/local/qt4/lib \
                     /usr/local/qt/lib \
                     /usr/local/lib/qt4 \
                     /usr/local/lib/qt \
                     /usr/lib \
                     /usr/local/lib \
                     /usr/lib/qt \
                     /usr/lib/qt/lib \
                     /usr/lib/x86_64-linux-gnu \
                     /usr/local/lib/qt \
                     /usr/X11/lib \
                     /usr/X11/lib/qt \
                     /usr/X11R6/lib \
                     /usr/X11R6/lib/qt \
                     /Developer/qt4/lib
                     /Developer/qt/lib"
  else
    qt_library_dirs="$ac_qt_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the QtCore library in one of
  dnl our search pathes
  ac_qt_libdir=""
  if test "$ac_qt_link_level" = "static"; then
    ac_qt_libname="libQtCore.a"
    LIB_QT="$ac_qt_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_qt_libname="libQtCore.dylib"
    else
      ac_qt_libname="libQtCore.so"
    fi
    LIB_QT="-lQtCore"
  fi

  for qt_dir in $qt_library_dirs; do
    if test -r "$qt_dir/$ac_qt_libname"; then
      ac_qt_libdir="$qt_dir"
      break;
    else
      echo "tried $qt_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_qtlib="ac_qt_libname=$ac_qt_libname ac_qt_libdir=$ac_qt_libdir"
  ])

  eval "$ac_cv_lib_qtlib"

  dnl Define a shell variable for later checks
  if test -z "$ac_qt_libdir"; then
    have_qt_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_qt_link_level Qt4 libraries in linker path.
Try --with-qt4-lib to specify the path, manually.])
  else
    have_qt_lib="yes"
    AC_MSG_RESULT([yes, $ac_qt_libname in $ac_qt_libdir found.])
  fi

  QT_LDFLAGS="-L$ac_qt_libdir"
  QT_LIBDIR="$ac_qt_libdir"
  AC_SUBST(QT_LDFLAGS)
  AC_SUBST(QT_LIBDIR)
  AC_SUBST(LIB_QT)
])

dnl
dnl AC_PATH_QT4_INC: checks for the existance of the Qt4 includes and provides means
dnl to override the default search pathes to it
dnl
AC_DEFUN([AC_PATH_QT4_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for Qt4 includes)

  AC_ARG_WITH(qt4-inc, [AC_HELP_STRING([--with-qt4-inc], [where the Qt4 includes are located.])],
                      [qt_include_dirs="$withval"], qt_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_qtinc, [

    dnl Did the user give --with-qt-includes?
    if test -z "$qt_include_dirs"; then
      qt_include_dirs="\
        $QT4DIR/include \
        /usr/include/qt4/ \
        /usr/local/qt4/include \
        /usr/lib/qt/include \
        /usr/include/qt \
        /usr/local/qt/include \
        /usr/local/include/qt \
        /usr/X11/include/qt \
        /usr/X11/include/X11/qt \
        /usr/X11R6/include \
        /usr/X11R6/include/qt \
        /usr/X11R6/include/X11/qt \
        /usr/X11/lib/qt/include"
    fi

    dnl now we do check for the QtCore subdir and the Qt include
    for qt_dir in $qt_include_dirs; do
      if test -r "$qt_dir/QtCore"; then
        if test -r "$qt_dir/QtCore/Qt"; then
          ac_qt_includes=$qt_dir
          break;
        fi
      fi
    done

    ac_cv_header_qtinc=$ac_qt_includes

  ])

  if test -z "$ac_cv_header_qtinc"; then
    have_qt_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([Qt4 include directory not found, you may run into problems.
Try --with-qt4-inc to specify the path, manually.])
  else
    have_qt_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_qtinc])
  fi

  QT_INCLUDES="-I$ac_cv_header_qtinc"
  QT_INCDIR="$ac_cv_header_qtinc"
  AC_SUBST(QT_INCLUDES)
  AC_SUBST(QT_INCDIR)
])

dnl
dnl AC_PATH_QT4_QMAKE: tries to find out if the "qmake" binary of Qt4 is reachable or not and
dnl allows to override the default path to it
dnl
AC_DEFUN([AC_PATH_QT4_QMAKE],
[
  AC_ARG_WITH(qt4-qmake,[AC_HELP_STRING([--with-qt4-qmake], [where the Qt4 qmake binary is located.])],
                        [ac_qt_qmake="$withval"], ac_qt_qmake="")

  if test -z "$QT4DIR" && test -z "$ac_qt_make"; then
    AC_PATH_PROG(
      QMAKE_PATH,
      qmake,
      qmake,
      /usr/local/qt4/bin:/usr/lib/qt4/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:/Developer/qt4/bin:$PATH
    )
  fi

  if test -z "$QMAKE_PATH"; then
    AC_MSG_CHECKING(for qmake)

    if test -f "$ac_qt_qmake" && test -x "$ac_qt_qmake"; then
      QMAKE_PATH=$ac_qt_qmake
    else
      if test -f "$QT4DIR/bin/qmake" && test -x "$QT4DIR/bin/qmake"; then
        QMAKE_PATH="$QT4DIR/bin/qmake"
      else
        AC_MSG_ERROR(couldn't find Qt4 qmake. Please use --with-qt4-qmake)
      fi
    fi

    AC_MSG_RESULT($QMAKE_PATH)
  fi

  dnl Check if we have the right qmake by outputing the version
  dnl information
  qmake_vers=`"$QMAKE_PATH" -v 2>&1 | grep "Qt version 4"`
  if test -z "$qmake_vers"; then
    AC_MSG_ERROR([didn't find the correct Qt4 version of qmake, Please use --with-qt4-qmake])
  fi

  AC_SUBST(QMAKE_PATH)
])

dnl
dnl AC_CHECK_GETTICKCOUNT: checks for the existance of a GetTickCount() function which
dnl should only be available to Windows. We use that function as a fallback for gettimeofday().
dnl
AC_DEFUN([AC_CHECK_GETTICKCOUNT],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for GetTickCount)
  
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  save_LDFLAGS="$LDFLAGS"
  LDFLAGS="-lkernel32"
  
  AC_TRY_LINK([#include <windows.h>], GetTickCount();, ac_have_gettickcount="yes", ac_have_gettickcount="no")
  
  dnl Define a shell variable for later checks
  if test "$ac_have_gettickcount" = "no"; then
    AC_MSG_RESULT([no])
  else
    AC_MSG_RESULT([yes])
    AC_DEFINE(HAVE_GETTICKCOUNT)
  fi
  
  LDFLAGS="$save_LDFLAGS"
  AC_LANG_RESTORE
])

dnl
dnl AC_PATH_RCPP: allows to override the default library search path for
dnl searching for the Rcpp library.
dnl
AC_DEFUN([AC_PATH_RCPP],
[
  AC_ARG_WITH(rcpp,
              [AC_HELP_STRING([--with-rcpp], [where the rcpp environment is located.])],
              [RCPPDIR="$withval"], [RCPPDIR=""])
])

AC_DEFUN([AC_PATH_RCPP_LIB],
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(rcpp-lib,
               [AC_HELP_STRING([--with-rcpp-lib], [where the Rcpp library is located.])],
               [ac_rcpp_libraries="$withval"], ac_rcpp_libraries="")

  AC_MSG_CHECKING(for Rcpp library)

  
  AC_CACHE_VAL(ac_cv_lib_rcpplib, [

  rcpp_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_rcpp_libraries"; then
    rcpp_library_dirs="$RCPPDIR/lib \
                          $RCPPDIR/lib/Rcpp \
                          $RCPPDIR \
                          $HOME/R/library/Rcpp/lib \
                          /usr/lib/R/site-library/Rcpp/lib"
  else
    rcpp_library_dirs="$ac_rcpp_libraries"
  fi

  dnl for simplicity we simply go and check if
  dnl we can find the rcpp library in one of
  dnl our search pathes
  ac_rcpp_libdir=""
  if test "$ac_rcpp_link_level" = "static"; then
    ac_rcpp_libname="libRcpp.a"
    LIB_RCPP="$ac_rcpp_libname"
  else
    if test "$HOST_OS" = "Darwin"; then
      ac_rcpp_libname="libRcpp.dylib"
    else
      ac_rcpp_libname="libRcpp.so"
    fi
    LIB_RCPP="-lRcpp"
  fi

  for rcpp_dir in $rcpp_library_dirs; do
    if test -r "$rcpp_dir/$ac_rcpp_libname"; then
      ac_rcpp_libdir="$rcpp_dir"
      break;
    else
      echo "tried $rcpp_dir" >&AC_FD_CC 
    fi
  done

  ac_cv_lib_rcpplib="ac_rcpp_libname=$ac_rcpp_libname ac_rcpp_libdir=$ac_rcpp_libdir"
  
  ])

  eval "$ac_cv_lib_rcpplib"

  dnl Define a shell variable for later checks
  if test "$R_INTERFACE" = "disabled"; then
    have_rcpp_lib="no"
    AC_MSG_RESULT([skipping, R interface disabled]) 
  elif test -z "$ac_rcpp_libdir"; then
    have_rcpp_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required $ac_rcpp_link_level rcpp library in linker path.
Try --with-rcpp-lib to specify the path, manually.])
  else
    have_rcpp_lib="yes"
    AC_MSG_RESULT([yes, $ac_rcpp_libname in $ac_rcpp_libdir found.])
  fi

  RCPP_LDFLAGS="-L$ac_rcpp_libdir"
  RCPP_LIBDIR="$ac_rcpp_libdir"
  AC_SUBST(RCPP_LDFLAGS)
  AC_SUBST(RCPP_LIBDIR)
  AC_SUBST(LIB_RCPP)
])

dnl
dnl AC_PATH_RCPP_INC: checks the existance of the includes files for successfully
dnl compiling support for the rcpp library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_RCPP_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for Rcpp includes)

  AC_ARG_WITH(rcpp-inc,
              [AC_HELP_STRING([--with-rcpp-inc], [where the Rcpp includes are located.])],
              [rcpp_include_dirs="$withval"], ac_rcpp_includes="")

  AC_CACHE_VAL(ac_cv_header_rcppinc, [

    dnl Did the user give --with-rcpp-includes?
    if test -z "$rcpp_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directores to check, add them here.
      rcpp_include_dirs="\
        $RCPPDIR/include \
        $RCPPDIR/include/Rcpp \
        $RCPPDIR/include/Rcpp/include \
        $RCPPDIR \
        $HOME/R/library/Rcpp/include \
        /usr/lib/R/site-library/Rcpp/include"
    fi

    for rcpp_dir in $rcpp_include_dirs; do
      if test -r "$rcpp_dir/Rcpp.h"; then
        ac_rcpp_includes=$rcpp_dir
        break;
      fi
    done

    ac_cv_header_rcppinc=$ac_rcpp_includes
  ])

  if test -z "$ac_cv_header_rcppinc"; then
    have_rcpp_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([Rcpp.h include not found, you may run into problems.
Try --with-rcpp-inc to specify the path, manually.])
  else
    have_rcpp_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_rcppinc])
  fi

  RCPP_INCLUDES="-I$ac_cv_header_rcppinc"
  RCPP_INCDIR="$ac_cv_header_rcppinc"
  AC_SUBST(RCPP_INCLUDES)
  AC_SUBST(RCPP_INCDIR)
])

dnl
dnl AC_PATH_R_INC: checks the existance of the includes files for successfully
dnl compiling support for the r library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN([AC_PATH_R_INC],
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for R includes)

  AC_ARG_WITH(r-inc,
              [AC_HELP_STRING([--with-r-inc], [where the R includes are located.])],
              [r_include_dirs="$withval"], ac_r_includes="")

  AC_CACHE_VAL(ac_cv_header_rinc, [

    dnl Did the user give --with-r-includes?
    if test -z "$r_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directores to check, add them here.
      r_include_dirs="\
        $RDIR/include \
        $RDIR \
        /usr/share/R/include"
    fi

    for r_dir in $r_include_dirs; do
      if test -r "$r_dir/R.h"; then
        ac_r_includes=$r_dir
        break;
      fi
    done

    ac_cv_header_rinc=$ac_r_includes
  ])

  if test -z "$ac_cv_header_rinc"; then
    have_r_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([R.h include not found, you may run into problems.
Try --with-r-inc to specify the path, manually.])
  else
    have_r_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_rinc])
  fi

  R_INCLUDES="-I$ac_cv_header_rinc"
  R_INCDIR="$ac_cv_header_rinc"
  AC_SUBST(R_INCLUDES)
  AC_SUBST(R_INCDIR)
])
