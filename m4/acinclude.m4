dnl/* vim:set ts=2 nowrap: ****************************************************
dnl
dnl acinclude.m4 - Common configure macros especially for Qt3/Qt4
dnl Copyright (C) 2003-2005 by Jens Langner <Jens.Langner@light-speed.de>
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
dnl
dnl $Id$
dnl
dnl****************************************************************************
dnl# -*- mode: m4 -*-

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

dnl
dnl AC_ENABLE_DEBUG: provides a switch to enable/disable debugging
dnl
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

dnl
dnl AC_ENABLE_STATIC_QT: provides a switch to control the link level (static/shared)
dnl of a linked Qt library.
dnl
AC_DEFUN(AC_ENABLE_STATIC_QT,
[
	AC_MSG_CHECKING(whether to link the Qt library static)
	AC_ARG_ENABLE(static-qt,
								[AC_HELP_STRING([--enable-static-qt], [turn on static linking of Qt libs [default=no]])],
								[case "${enableval}" in
									yes) test_on_enable_static_qt=yes ;;
									no)  test_on_enable_static_qt=no	 ;;
									*)	 AC_MSG_ERROR(bad value ${enableval} for --enable-static-qt) ;;
								esac],
								[test_on_enable_static_qt=no])

	if test "$test_on_enable_static_qt" = "yes"; then
		QTLINK_LEVEL="${QTLINK_LEVEL} staticconfig"
		AC_MSG_RESULT(yes)
	else
		QTLINK_LEVEL="${QTLINK_LEVEL}"
		AC_MSG_RESULT(no)
	fi

	AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_RTDEBUG: provides a switch to control the link level (static/shared)
dnl of a linked rtdebug library
dnl
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

dnl
dnl AC_ENABLE_STATIC_MEDIO: provides a switch to control the link level (static/shared)
dnl of a linked medio library
dnl
AC_DEFUN(AC_ENABLE_STATIC_MEDIO,
[
	AC_MSG_CHECKING(whether to link the medio library static)
	AC_ARG_ENABLE(static-medio,
								[AC_HELP_STRING([--enable-static-medio], [turn on static linking of medio libs [default=no]])],
								[case "${enableval}" in
									yes) test_on_enable_static_medio=yes	;;
									no)	 test_on_enable_static_medio=no	;;
									*)	 AC_MSG_ERROR(bad value ${enableval} for --enable-static-medio) ;;
								esac],
								[test_on_enable_static_medio=no])

	if test "$test_on_enable_static_medio" = "yes"; then
		QTLINK_LEVEL="${QTLINK_LEVEL} staticmedio"
		AC_MSG_RESULT(yes)
	else
		QTLINK_LEVEL="${QTLINK_LEVEL}"
		AC_MSG_RESULT(no)
	fi

	AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_GSL: provides a switch to control the link level (static/shared)
dnl of a linked gsl library
dnl
AC_DEFUN(AC_ENABLE_STATIC_GSL,
[
	AC_MSG_CHECKING(whether to link the gsl library static)
	AC_ARG_ENABLE(static-gsl,
								[AC_HELP_STRING([--enable-static-gsl], [turn on static linking of gsl libs [default=no]])],
								[case "${enableval}" in
									yes) test_on_enable_static_gsl=yes	;;
									no)	 test_on_enable_static_gsl=no	;;
									*)	 AC_MSG_ERROR(bad value ${enableval} for --enable-static-gsl) ;;
								esac],
								[test_on_enable_static_gsl=no])

	if test "$test_on_enable_static_gsl" = "yes"; then
		QTLINK_LEVEL="${QTLINK_LEVEL} staticgsl"
		AC_MSG_RESULT(yes)
	else
		QTLINK_LEVEL="${QTLINK_LEVEL}"
		AC_MSG_RESULT(no)
	fi

	AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_ENABLE_STATIC_LIB: if the project is a library this macro can be used to control
dnl if the library should be build as a shared or static library
dnl
AC_DEFUN(AC_ENABLE_STATIC_LIB,
[
	AC_MSG_CHECKING(whether to link as a static library)
	AC_ARG_ENABLE(static-lib,
								[AC_HELP_STRING([--enable-static-lib], [turn on static linking [default=no]])],
								[case "${enableval}" in
									yes) test_on_enable_static_lib=yes	;;
									no)  test_on_enable_static_lib=no	;;
									*)	 AC_MSG_ERROR(bad value ${enableval} for --enable-static-lib) ;;
								esac],
								[test_on_enable_static_lib=no])

	if test "$test_on_enable_static_lib" = "yes"; then
		QTLINK_LEVEL="${QTLINK_LEVEL} staticlib"
		AC_MSG_RESULT(yes)
	else
		QTLINK_LEVEL="${QTLINK_LEVEL}"
		AC_MSG_RESULT(no)
	fi

	AC_SUBST(QTLINK_LEVEL) 
])

dnl
dnl AC_PROG_GCC_VERSION: finds out if the used gcc version is a supported one
dnl or not
dnl
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
		 2.95.[2-9]|2.95.[2-9][-.]*|3.[0-9]*|3.[0-9].[0-9]*|4.[0-9].[0-9]*)
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

dnl
dnl AC_PATH_RTDEBUG_LIB: checks for the existance of the rtdebug library in the
dnl default pathes and allows to override them as well
dnl
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
												/usr/local/petlib/lib \
												/usr/local/petlib/lib/rtdebug \	
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
    AC_MSG_ERROR([Cannot find required runtime debugging library in linker path.
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

dnl
dnl AC_PATH_RTDEBUG_INC: checks the existance of the includes files for successfully
dnl compiling support for the rtdebug library and also allows to override the default
dnl path to that includes.
dnl
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
			  /usr/local/petlib/include \
				/usr/local/petlib/include/rtdebug \		
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

dnl
dnl AC_PATH_MEDIO_LIB: checks for the existance of the medio library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN(AC_PATH_MEDIO_LIB,
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
    medio_library_dirs="$medio_library_dirs \
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

  dnl Save some global vars
  save_LDFLAGS="$LDFLAGS"
  save_LIBS="$LIBS"

  medio_found="0"
  ac_medio_libdir=
  ac_medio_libname="-lmedio"
  
  LIBS="$ac_medio_libname $save_LIBS"
  for medio_dir in $medio_library_dirs; do
    LDFLAGS="-L$medio_dir $save_LDFLAGS"
    AC_TRY_LINK_FUNC(main, [medio_found="1"], [medio_found="0"])
    if test $medio_found = 1; then
      ac_medio_libdir="$medio_dir"
      break;
    else
      echo "tried $medio_dir" >&AC_FD_CC 
    fi
  done

  dnl Restore the saved vars
  LDFLAGS="$save_LDFLAGS"
  LIBS="$save_LIBS"

  ac_cv_lib_mediolib="ac_medio_libname=$ac_medio_libname ac_medio_libdir=$ac_medio_libdir"
  ])

  eval "$ac_cv_lib_mediolib"

  dnl Define a shell variable for later checks
  if test -z "$ac_medio_libdir"; then
    have_medio_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required medical IO (libmedio) library in linker path.
Try --with-medio-lib to specify the path, manually.])
  else
    have_medio_lib="yes"
    AC_MSG_RESULT([yes, $ac_medio_libname in $ac_medio_libdir found.])
  fi

  MEDIO_LDFLAGS="-L$ac_medio_libdir"
  MEDIO_LIBDIR="$ac_medio_libdir"
  LIB_MEDIO="$ac_medio_libname"
  AC_SUBST(MEDIO_LDFLAGS)
  AC_SUBST(MEDIO_LIBDIR)
  AC_SUBST(LIB_MEDIO)
])

dnl
dnl AC_PATH_MEDIO_INC: checks the existance of the includes files for successfully
dnl compiling support for the medio library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN(AC_PATH_MEDIO_INC,
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
			  /usr/local/petlib/include \
				/usr/local/petlib/include/medio \					
        /usr/local/include \
        /usr/local/include/medio \
        /usr/include/medio \
        /usr/lib/medio/include"
    fi

    for medio_dir in $medio_include_dirs; do
      if test -r "$medio_dir/CECATFile.h"; then
        if test -r "$medio_dir/CECATDirectory.h"; then
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
    AC_MSG_WARN([libmedio include directory not found, you may run into problems. Try --with-medio-inc to specify the path, manually.])
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
dnl AC_PATH_GSL_LIB: checks for the existance of the gsl library in the
dnl default pathes and allows to override them as well
dnl
AC_DEFUN(AC_PATH_GSL_LIB,
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
    gsl_library_dirs="$gsl_library_dirs \
		                  /usr/local/lib \
											/usr/local/lib/gsl \
		                  /usr/lib \
		                  /usr/lib/gsl \
		                  /Developer/gsl/lib"
  else
    gsl_library_dirs="$ac_gsl_libraries"
  fi

  dnl Save some global vars
  save_LDFLAGS="$LDFLAGS"
  save_LIBS="$LIBS"

  gsl_found="0"
  ac_gsl_libdir=
  ac_gsl_libname="-lgsl -lgslcblas"
  
  LIBS="$ac_gsl_libname $save_LIBS"
  for gsl_dir in $gsl_library_dirs; do
    LDFLAGS="-L$gsl_dir $save_LDFLAGS"
    AC_TRY_LINK_FUNC(main, [gsl_found="1"], [gsl_found="0"])
    if test $gsl_found = 1; then
      ac_gsl_libdir="$gsl_dir"
      break;
    else
      echo "tried $gsl_dir" >&AC_FD_CC 
    fi
  done

  dnl Restore the saved vars
  LDFLAGS="$save_LDFLAGS"
  LIBS="$save_LIBS"

  ac_cv_lib_gsllib="ac_gsl_libname=\"$ac_gsl_libname\" ac_gsl_libdir=\"$ac_gsl_libdir\""
  ])

  eval "$ac_cv_lib_gsllib"

  dnl Define a shell variable for later checks
  if test -z "$ac_gsl_libdir"; then
    have_gsl_lib="no"
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find required GNU Scientific 'libgsl' library in linker path. Try --with-gsl-lib to specify the path, manually.])
  else
    have_gsl_lib="yes"
    AC_MSG_RESULT([yes, $ac_gsl_libname in $ac_gsl_libdir found.])
  fi

  GSL_LDFLAGS="-L$ac_gsl_libdir"
  GSL_LIBDIR="$ac_gsl_libdir"
  LIB_GSL="$ac_gsl_libname"
  AC_SUBST(GSL_LDFLAGS)
  AC_SUBST(GSL_LIBDIR)
  AC_SUBST(LIB_GSL)
])

dnl
dnl AC_PATH_GSL_INC: checks the existance of the includes files for successfully
dnl compiling support for the gsl library and also allows to override the default
dnl path to that includes.
dnl
AC_DEFUN(AC_PATH_GSL_INC,
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
        /usr/local/gsl/include \
        /usr/include/gsl \
        /usr/lib/gsl/include \
        /usr/local/include/gsl"
    fi

    for gsl_dir in $gsl_include_dirs; do
      if test -r "$gsl_dir/gsl_version.h"; then
        if test -r "$gsl_dir/gsl_types.h"; then
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
    AC_MSG_WARN([libgsl include directory not found, you may run into problems. Try --with-gsl-inc to specify the path, manually.])
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
dnl AC_QTSHAREDBUILD: tries to find out if the used Qt3 library was build
dnl as a shared or static library
dnl
AC_DEFUN(AC_QTSHAREDBUILD,
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
AC_DEFUN(AC_PATH_QT3DIR,
[
  AC_ARG_WITH(qtdir, [AC_HELP_STRING([--with-qtdir], [set the main Qt3 'QTDIR' search variable manually.])],
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
AC_DEFUN(AC_PATH_QT3_LIB,
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

  dnl Save some global vars
  save_LDFLAGS="$LDFLAGS"
  save_LIBS="$LIBS"

  qt_found="0"
  ac_qt_libdir=
  ac_qt_libname="-lqt-mt"
  
  LIBS="$ac_qt_libname $save_LIBS"
  for qt_dir in $qt_library_dirs; do
    dnl echo $qt_dir;
    LDFLAGS="-L$qt_dir $save_LDFLAGS"
    AC_TRY_LINK_FUNC(main, [qt_found="1"], [qt_found="0"])
    dnl AC_TRY_RUN([#include <qglobal.h>
    dnl int 
    dnl main()
    dnl {
    dnl  main();
    dnl  ;
    dnl  return QT_VERSION;
    dnl }])
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
    AC_MSG_ERROR([Cannot find required Qt3 multithreaded library in linker path.
Try --with-qt-lib to specify the path, manualy.])
  else
    have_qt_lib="yes"
    AC_MSG_RESULT([yes, lib: $ac_qt_libname in $ac_qt_libdir])
  fi

  QT_LDFLAGS="-L$ac_qt_libdir"
  QT_LIBDIR="$ac_qt_libdir"
  LIB_QT="$ac_qt_libname"
  AC_SUBST(QT_LDFLAGS)
  AC_SUBST(QT_LIBDIR)
  AC_SUBST(LIB_QT)
])

dnl
dnl AC_PATH_QT3_INC: tries to find out if the Qt3 headers are reachable and provides
dnl means of overriding the default search pathes.
dnl
AC_DEFUN(AC_PATH_QT3_INC,
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
AC_DEFUN(AC_PATH_QT3_QMAKE,
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
dnl AC_PATH_QT4_LIB: checks if the Qt4 libraries are reachable and provides means
dnl of overriding the default search path
dnl
AC_DEFUN(AC_PATH_QT4_LIB,
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
    qt_library_dirs="/usr/lib/qt4 \
										 /usr/local/qt4/lib \
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
    AC_MSG_ERROR([Cannot find required Qt4 libraries in linker path. Try --with-qt4-lib to specify the path, manualy.])
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

dnl
dnl AC_PATH_QT4_INC: checks for the existance of the Qt4 includes and provides means
dnl to override the default search pathes to it
dnl
AC_DEFUN(AC_PATH_QT4_INC,
[
  AC_REQUIRE_CPP()
  AC_MSG_CHECKING(for Qt4 includes)

  AC_ARG_WITH(qt4-inc, [AC_HELP_STRING([--with-qt4-inc], [where the Qt4 includes are located.])],
                      [qt_include_dirs="$withval"], qt_include_dirs="")

  AC_CACHE_VAL(ac_cv_header_qtinc, [

    dnl Did the user give --with-qt-includes?
    if test -z "$qt_include_dirs"; then
      qt_include_dirs="\
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
    AC_MSG_WARN([Qt4 include directory not found, you may run into problems. Try --with-qt4-inc to specify the path, manualy.])
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
AC_DEFUN(AC_PATH_QT4_QMAKE,
[
  AC_ARG_WITH(qt4-qmake,[AC_HELP_STRING([--with-qt4-qmake], [where the Qt4 qmake binary is located.])],
                        [ac_qt_qmake="$withval"], ac_qt_qmake="")

  if test -z "$ac_qt_qmake"; then
    AC_PATH_PROG(
      QMAKE_PATH,
      qmake,
      qmake,
      /usr/lib/qt4/bin:/usr/local/qt4/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:/Developer/qt4/bin:$PATH
    )
  else
    AC_MSG_CHECKING(for qmake)

    if test -f $ac_qt_qmake && test -x $ac_qt_qmake; then
      QMAKE_PATH=$ac_qt_qmake
    else
      AC_MSG_ERROR(
        --with-qt4-qmake expects path and name of the qmake tool
      )
    fi

    AC_MSG_RESULT($QMAKE_PATH)
  fi

  if test -z "$QMAKE_PATH"; then
    AC_MSG_ERROR(couldn't find Qt4 qmake. Please use --with-qt4-qmake)
  fi

  dnl Check if we have the right qmake by outputing the version
	dnl information
	qmake_vers=`"$QMAKE_PATH" -v 2>&1 | grep "Qt version 4"`
  if test -z "$qmake_vers"; then
    AC_MSG_ERROR([didn't find the correct Qt4 version of qmake, Please use --with-qt4-qmake])
  fi

  AC_SUBST(QMAKE_PATH)
])
