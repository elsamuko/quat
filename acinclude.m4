AC_DEFUN(ACX_FLAG,
[
	AC_LANG_CASE(C, cur_lang=C;, C++, cur_lang=C++;)dnl undocumented !
	dnl if test x$ac_ext = xcc; then
	if test x$cur_lang = xC++; then
		AC_CACHE_CHECK(whether the C++ compiler supports $1, ac_cv_flag_cc_$2,
			OLDCXXFLAGS="$CXXFLAGS"
			CXXFLAGS="$CXXFLAGS $1"
			[AC_TRY_LINK([],[int i=0],
				ac_cv_flag_cc_$2=yes
				$3,
				ac_cv_flag_cc_$2=no
				CXXFLAGS="$OLDCXXFLAGS"
				$4
			)]
		)
	else
		AC_CACHE_CHECK(whether the C compiler supports $1, ac_cv_flag_c_$2,
			OLDCFLAGS="$CFLAGS"
			CFLAGS="$CFLAGS $1"
			[AC_TRY_LINK([],[int i=0],
				ac_cv_flag_c_$2=yes
				$3,
				ac_cv_flag_c_$2=no
				CFLAGS="$OLDCFLAGS"
				$4
			)]
		)
	fi
])

AC_DEFUN(ACX_M_PI,
[
	AC_CACHE_CHECK(for M_PI, ac_cv_m_pi,
		[AC_TRY_COMPILE([
			#include <math.h>],[
				double d = M_PI
			],ac_cv_m_pi=yes,ac_cv_m_pi=no
		)]
	)
	if test $ac_cv_m_pi = no; then
		AC_DEFINE(M_PI, 3.14159265358979323846, [Define to the value of pi if you don't already have it])
	fi
])

AC_DEFUN(ACX_NAMESPACE,
[
	AC_LANG_PUSH(C++)
	AC_CACHE_CHECK(whether the C++ compiler needs std::, ac_cv_needstd,
		[AC_TRY_COMPILE([#include <string>],
			[string s],
			ac_cv_needstd=no,
			ac_cv_needstd=yes
		)]
	)
	if test $ac_cv_needstd = no; then
		AC_DEFINE(NO_NAMESPACE,, [Define if your compiler doesn't support namespaces])
	fi
	AC_LANG_POP(C++)
])

AC_DEFUN(ACX_FL_PIXMAP,
[
	AC_MSG_CHECKING([[whether Fl_Pixmap needs const char*const*]])
	AC_TRY_COMPILE([
		#include <FL/Enumerations.H>
		#include <FL/Fl_Pixmap.H>],[
		char *const abuf[] = { "a", "b" };
		Fl_Pixmap pix(abuf)
	],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes)]
	[AC_DEFINE_UNQUOTED(XPMCONST, const, [Define to const if your compiler complains about the constness of Fl_Pixmap])])
])

AC_DEFUN(ACX_FIND_FILE,
[
	AC_CACHE_CHECK(for $1, ac_cv_path_$4,
		for i in $$3 $2;
		do
			if test -r "$i/$1"; then
				$3=$i
				ac_cv_path_$4=$i
				break
			fi
		done
		if test "$$3" = ""; then ac_cv_path_$4=no; fi
	)
])

dnl parameters:
dnl 1: header file, 2: name of lib, 3: function in lib,
dnl 4: action if found, 5: action if not found
AC_DEFUN(ACX_CHECK_LIB,
[
	success=no
	AC_CHECK_HEADER($1,[AC_CHECK_LIB($2,$3,success=yes)])
	if test x$success = xyes; then
		LIBS="-l$2 $LIBS"
		$4
	else
		:
		$5
	fi
])

AC_DEFUN(ACX_X_OR_WIN,
[
	AC_REQUIRE([AC_CANONICAL_HOST])dnl
	case "$host_os" in
		*cygwin* ) x_or_win=win;;
		 *mingw* ) x_or_win=win;;
		       * ) x_or_win=neither;
	esac

	if [test x$x_or_win = xwin]; then
		CFLAGS="-mwindows $CFLAGS"
		CXXFLAGS="-mwindows $CXXFLAGS"
		LDFLAGS="-mwindows $LDFLAGS"
		LIBS="-lgdi32 -luser32 -lglu32 -lmsvcrt -lwsock32 $LIBS"
	else
		AC_PATH_XTRA
		if [test a$no_x = a]; then
			x_or_win=x
			dnl there's no direct reference to X11 in the code, so no CFLAGS required
			LDFLAGS="$X_LIBS $LDFLAGS"
			LIBS="-lX11 $X_EXTRA_LIBS $X_PRE_LIBS $LIBS"
		fi
	fi
])

# Add a variable $2 (e.g. "-L...") to $1, if it's not already contained
AC_DEFUN(ACX_ADD_BEFORE,
[
	contained=no
	for addvar in $$1; do
		if test x$addvar = x$$2; then
			contained=yes
		fi
	done
	if test x$contained = xno; then
		$1="$$2 $$1"
	fi
])

# Add a variable $2 (e.g. "-L...") to $1, if it's not already contained
AC_DEFUN(ACX_ADD_AFTER,
[
	contained=no
	for addvar in $$1; do
		if test x$addvar = x$$2; then
			contained=yes
		fi
	done
	if test x$contained = xno; then
		$1="$$1 $$2"
	fi
])

# $1: required api-version (1.1),
# $2: action (--use-images --use-gl)
# $3: action if success, $4: action if failure
AC_DEFUN(ACX_FLTK_CONFIG,
[
	AC_PATH_PROG(FLTK,fltk-config)
	if test a$FLTK != a; then

		# look for fluid
		if test x$FLUID = x; then
			FLUID=`$FLTK --exec-prefix`/bin/fluid$EXEEXT
		fi
		AC_SUBST(FLUID)

		# check api-version
		if test x$1 != x; then
			FLTK_API_VERSION=`$FLTK --api-version`
			if test x$1 != x$FLTK_API_VERSION; then
				AC_MSG_WARN([FLTK: api version $1 required, but $FLTK_API_VERSION found.])
			fi
		fi

		# add requested flags to LDFLAGS
		FLTK_LDFLAGS=`$FLTK $2 --ldflags`
		FLTK_LDFLAGS_REVERSE=""
		for acx_var in $FLTK_LDFLAGS; do
			FLTK_LDFLAGS_REVERSE="$acx_var $FLTK_LDFLAGS_REVERSE"
		done
		for acx_var in $FLTK_LDFLAGS_REVERSE; do
			case "$acx_var" in
				-l*) ACX_ADD_BEFORE(LIBS,acx_var);;
				*) ACX_ADD_BEFORE(LDFLAGS,acx_var);;
			esac
		done

		# add requested flags to CPPFLAGS or CXXFLAGS
		FLTK_INCLUDE=`$FLTK $2 --cxxflags`
		for acx_var in $FLTK_INCLUDE; do
			case "$acx_var" in
				-I*) ACX_ADD_AFTER(CPPFLAGS,acx_var);;
				*) ACX_ADD_AFTER(CXXFLAGS,acx_var);;
			esac
		done
		$3
	else
		:
		$4
	fi
])

AC_DEFUN(ACX_FLTK,
[
	AC_REQUIRE([ACX_X_OR_WIN])dnl
#$EXEEXT instead!!!
	if test a$x_or_win = ax; then
		AC_PATH_PROG(FLUID,fluid,,$PATH:/usr/local/bin:/usr/bin)
	elif test x$x_or_win = xwin; then
		AC_PATH_PROG(FLUID,fluid.exe,,$PATH)
	fi
	AC_SUBST(FLUID)

	AC_LANG_PUSH(C++)
	ACX_CHECK_LIB(FL/Fl.H,fltk,fl_window,fltk=yes,fltk=no)
	AC_LANG_POP(C++)
])
