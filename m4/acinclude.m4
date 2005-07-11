dnl/***************************************************************************
dnl
dnl acsacq - Direct, transparent ACS2 based Acquisition
dnl Copyright (C) 2003-2005 by Jens Langner <Jens.Langner@light-speed.de>
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
dnl
dnl $Id$
dnl
dnl***************************************************************************
dnl# -*- mode: m4 -*-

dnl Autoconf Macros from gnu.org

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

dnl Copyright (c) 2003 Jens Langner
AC_DEFUN(AC_ANSI_COLOR,
[
	AC_MSG_CHECKING(whether ANSI color should be used for terminal output)
	AC_ARG_ENABLE(ansi-color,
								[AC_HELP_STRING([--disable-ansi-color], [ansi-color terminal output should be disabled [default=no]])],
								[case "${enableval}" in
									yes) test_on_ansi_color=yes ;;
									no)  test_on_ansi_color=no ;;
									*)   AC_MSG_ERROR(bad value ${enableval} for --disable-ansi-color) ;;
								esac], 
								[test_on_ansi_color=no])

	if test "$test_on_ansi_color" = "yes"; then
		AC_MSG_RESULT(no)
	else
		ANSI_COLOR="ansi_color" 
		AC_DEFINE(WITH_ANSI_COLOR) 		
		AC_MSG_RESULT(yes)
	fi
	AC_DEFINE([WITH_ANSI_COLOR], [], [Use ANSI color scheme in terminal output])
	
	AC_SUBST(ANSI_COLOR) 
])

# this macro is used to get the arguments supplied
# to the configure script (./configure --enable-debug)
AC_DEFUN(AC_ENABLE_DEBUG,
[
	AC_MSG_CHECKING(whether to enable debugging)
	AC_ARG_ENABLE(debug,
								[AC_HELP_STRING([--enable-debug], [turn on debugging mode [default=no]])],
								[case "${enableval}" in
									yes) test_on_enable_debug=yes	;;
									no)	 test_on_enable_debug=no	;;
									*)	 AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
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

# this macro is used to get the arguments supplied
# to the configure script (./configure --enable-static-qt)
AC_DEFUN(AC_ENABLE_STATIC_QT,
[
	AC_MSG_CHECKING(whether to link the QT library static)
	AC_ARG_ENABLE(static-qt,
								[AC_HELP_STRING([--enable-static-qt], [turn on static linking of QT libs [default=no]])],
								[case "${enableval}" in
									yes) test_on_enable_static_qt=yes	;;
									no)	 test_on_enable_static_qt=no	;;
									*)	 AC_MSG_ERROR(bad value ${enableval} for --enable-static-qt) ;;
								esac],
								[test_on_enable_static_qt=no])

	if test "$test_on_enable_static_qt" = "yes"; then
		QTLINK_LEVEL="${QTLINK_LEVEL} staticqt"
		AC_MSG_RESULT(yes)
	else
		QTLINK_LEVEL="${QTLINK_LEVEL}"
		AC_MSG_RESULT(no)
	fi

	AC_SUBST(QTLINK_LEVEL) 
])

# this macro is used to get the arguments supplied
# to the configure script (./configure --enable-static-rtdebug)
AC_DEFUN(AC_ENABLE_STATIC_RTDEBUG,
[
	AC_MSG_CHECKING(whether to link the rtdebug library static)
	AC_ARG_ENABLE(static-rtdebug,
								[AC_HELP_STRING([--enable-static-rtdebug], [turn on static linking of rtdebug lib [default=no]])],
								[case "${enableval}" in
									yes) test_on_enable_static_rtdebug=yes	;;
									no)	 test_on_enable_static_rtdebug=no	;;
									*)	 AC_MSG_ERROR(bad value ${enableval} for --enable-static-rtdebug) ;;
								esac],
								[test_on_enable_static_rtdebug=no])

	if test "$test_on_enable_static_rtdebug" = "yes"; then
		QTLINK_LEVEL="${QTLINK_LEVEL} staticrtdebug"
		AC_MSG_RESULT(yes)
	else
		QTLINK_LEVEL="${QTLINK_LEVEL}"
		AC_MSG_RESULT(no)
	fi

	AC_SUBST(QTLINK_LEVEL) 
])

AC_DEFUN(AC_PATH_RTDEBUG_LIB,
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(rtdebug-lib,
              [AC_HELP_STRING([--with-rtdebug-lib], [where the librtdebug library is located.])],
							[ac_rtdebug_libraries="$withval"], ac_rtdebug_libraries="")

  AC_MSG_CHECKING(for runtime debugging library)

  AC_CACHE_VAL(ac_cv_lib_rtdebuglib, [

  rtdebug_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_rtdebug_libraries"; then
    rtdebug_library_dirs="$rtdebug_library_dirs \
		                    /usr/local/lib \
												/usr/local/lib/rtdebug \
		                    /usr/lib \
		                    /usr/lib/rtdebug \
		                    /Developer/rtdebug/lib"
  else
    rtdebug_library_dirs="$ac_rtdebug_libraries"
  fi

  dnl Save some global vars
  save_LDFLAGS="$LDFLAGS"
  save_LIBS="$LIBS"

  rtdebug_found="0"
  ac_rtdebug_libdir=
  ac_rtdebug_libname="-lrtdebug"
  
  LIBS="$ac_rtdebug_libname $save_LIBS"
  for rtdebug_dir in $rtdebug_library_dirs; do
    LDFLAGS="-L$rtdebug_dir $save_LDFLAGS"
    AC_TRY_LINK_FUNC(main, [rtdebug_found="1"], [rtdebug_found="0"])
    if test $rtdebug_found = 1; then
      ac_rtdebug_libdir="$rtdebug_dir"
      break;
    else
      echo "tried $rtdebug_dir" >&AC_FD_CC 
    fi
  done

  dnl Restore the saved vars
  LDFLAGS="$save_LDFLAGS"
  LIBS="$save_LIBS"

  ac_cv_lib_rtdebuglib="ac_rtdebug_libname=$ac_rtdebug_libname ac_rtdebug_libdir=$ac_rtdebug_libdir"
  ])

  eval "$ac_cv_lib_rtdebuglib"

  dnl Define a shell variable for later checks
  if test -z "$ac_rtdebug_libdir"; then
    have_rtdebug_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required runtime debugging (librtdebug) library in linker path.
Try --with-rtdebug-lib to specify the path, manually.])
  else
    have_rtdebug_lib="yes"
    AC_MSG_RESULT([yes, $ac_rtdebug_libname in $ac_rtdebug_libdir found.])
  fi

  RTDEBUG_LDFLAGS="-L$ac_rtdebug_libdir"
  RTDEBUG_LIBDIR="$ac_rtdebug_libdir"
  LIB_RTDEBUG="$ac_rtdebug_libname"
  AC_SUBST(RTDEBUG_LDFLAGS)
  AC_SUBST(RTDEBUG_LIBDIR)
  AC_SUBST(LIB_RTDEBUG)
])

AC_DEFUN(AC_PATH_RTDEBUG_INC,
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for librtdebug includes)

  AC_ARG_WITH(rtdebug-inc,
              [AC_HELP_STRING([--with-rtdebug-inc], [where the librtdebug headers are located.])],
              [rtdebug_include_dirs="$withval"], rtdebug_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_rtdebuginc, [

    dnl Did the user give --with-rtdebug-includes?
    if test -z "$rtdebug_include_dirs"; then

      dnl No they didn't, so lets look for them...
      dnl If you need to add extra directories to check, add them here.
      rtdebug_include_dirs="\
        /usr/local/rtdebug/include \
        /usr/include/rtdebug \
        /usr/lib/rtdebug/include \
        /usr/local/include/rtdebug"
    fi

    for rtdebug_dir in $rtdebug_include_dirs; do
      if test -r "$rtdebug_dir/CRTDebug.h"; then
        if test -r "$rtdebug_dir/rtdebug.h"; then
          ac_rtdebug_includes=$rtdebug_dir
          break;
        fi
      fi
    done

    ac_cv_header_rtdebuginc=$ac_rtdebug_includes

  ])

  if test -z "$ac_cv_header_rtdebuginc"; then
    have_rtdebug_inc="no"
    AC_MSG_RESULT([no])
    AC_MSG_WARN([librtdebug include directory not found, you may run into problems.
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


dnl - find out the used gcc version and validate that this one is
dnl - a working one for lmmc
AC_DEFUN(AC_PROG_GCC_VERSION,[

 AC_MSG_CHECKING([for gcc version])

 dnl check if $CC exists or not
 if eval $CC -v 2>/dev/null >/dev/null; then
   dnl Check if version of gcc is sufficient
	 cc_name=`( $CC -v ) 2>&1 | tail -n 1 | cut -d ' ' -f 1`
	 cc_version=`( $CC -dumpversion ) 2>&1`
	 if test "$?" -gt 0; then
		 cc_version="not found"
	 fi
	 changequote(,)dnl	 
	 case $cc_version in
	   '')
		   cc_version="v. ?.??, bad"
			 cc_verc_fail=yes
			 ;;
		 2.95.[2-9]|2.95.[2-9][-.]*|3.[0-9]|3.[0-9].[0-9]|4.[0-9].[0-9])
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
   AC_MSG_ERROR([gcc was not found '$CC'])
 fi

 AC_SUBST(GCC_VERSION)
])

dnl - AutoConf check for QT4 library path. Please note that with QT4 there is no $QTDIR
dnl - anymore, so we have to check through some predefined library pathes.
AC_DEFUN(AC_PATH_QT4_LIB,
[
  AC_REQUIRE_CPP()
  AC_ARG_WITH(qt-lib, [AC_HELP_STRING([--with-qt-lib], [where the Qt4 libraries are located.])],
                      [ac_qt_libraries="$withval"], ac_qt_libraries="")

  AC_MSG_CHECKING(for Qt4 libraries)

  AC_CACHE_VAL(ac_cv_lib_qtlib, [

  qt_libdir=

  dnl No they didnt, so lets look for them...
  dnl If you need to add extra directories to check, add them here.
  if test -z "$ac_qt_libraries"; then
    qt_library_dirs="/usr/local/qt4/lib \
										 /usr/lib/qt4 \
										 /usr/local/qt/lib \
                     /usr/local/lib/qt4 \
                     /usr/local/lib/qt \
                     /usr/lib \
                     /usr/local/lib \
                     /usr/lib/qt \
                     /usr/lib/qt/lib \
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

  dnl Save some global vars
  save_LDFLAGS="$LDFLAGS"
  save_LIBS="$LIBS"

  qt_found="0"
  ac_qt_libdir=
  ac_qt_libname="-lQtCore"
  
  LIBS="$ac_qt_libname $save_LIBS"
  for qt_dir in $qt_library_dirs; do
    LDFLAGS="-L$qt_dir $save_LDFLAGS"
    AC_TRY_LINK_FUNC(main, [qt_found="1"], [qt_found="0"])
    if test $qt_found = 1; then
      ac_qt_libdir="$qt_dir"
      break;
    else
      echo "tried $qt_dir" >&AC_FD_CC 
    fi
  done

  dnl Restore the saved vars
  LDFLAGS="$save_LDFLAGS"
  LIBS="$save_LIBS"

  ac_cv_lib_qtlib="ac_qt_libname=$ac_qt_libname ac_qt_libdir=$ac_qt_libdir"
  ])

  eval "$ac_cv_lib_qtlib"

  dnl Define a shell variable for later checks
  if test -z "$ac_qt_libdir"; then
    have_qt_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required Qt4 libraries in linker path. Try --with-qt-lib to specify the path, manualy.])
  else
    have_qt_lib="yes"
    AC_MSG_RESULT([yes, $ac_qt_libname in $ac_qt_libdir found.])
  fi

  QT_LDFLAGS="-L$ac_qt_libdir"
  QT_LIBDIR="$ac_qt_libdir"
  LIB_QT="$ac_qt_libname"
  AC_SUBST(QT_LDFLAGS)
  AC_SUBST(QT_LIBDIR)
  AC_SUBST(LIB_QT)
])

AC_DEFUN(AC_PATH_QT4_INC,
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for Qt4 includes)

  AC_ARG_WITH(qt-inc, [AC_HELP_STRING([--with-qt-inc], [where the Qt4 headers are located.])],
                      [qt_include_dirs="$withval"], qt_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_qtinc, [

    dnl Did the user give --with-qt-includes?
    if test -z "$qt_include_dirs"; then
      qt_include_dirs="\
				/usr/local/qt4/include \
			  /usr/include/qt4/ \
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
    AC_MSG_WARN([Qt4 include directory not found, you may run into problems. Try --with-qt-inc to specify the path, manualy.])
  else
    have_qt_inc="yes"
    AC_MSG_RESULT([yes, in $ac_cv_header_qtinc])
  fi

  QT_INCLUDES="-I$ac_cv_header_qtinc"
  QT_INCDIR="$ac_cv_header_qtinc"
  AC_SUBST(QT_INCLUDES)
  AC_SUBST(QT_INCDIR)
])
