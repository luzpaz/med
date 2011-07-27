dnl  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl  This library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public
dnl  License as published by the Free Software Foundation; either
dnl  version 2.1 of the License.
dnl
dnl  This library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this library; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

AC_DEFUN([CHECK_SCOTCH],[

AC_CHECKING(for SCOTCH Library)

AC_LANG_SAVE
AC_LANG_C

SCOTCH_CPPFLAGS=""
SCOTCH_LIBSUFFIX=""
SCOTCH_LIBS=""
ENABLE_SCOTCH="no"

AC_CHECKING(for SCOTCH location)
AC_ARG_WITH(scotch,
            [  --with-scotch=DIR      root directory path to SCOTCH library installation ],
            [SCOTCHDIR="$withval"
             AC_MSG_RESULT("select $withval as path to SCOTCH library")])

AC_MSG_RESULT(\$SCOTCHDIR = ${SCOTCHDIR})

CPPFLAGS_old="${CPPFLAGS}"
LIBS_old=$LIBS

if test "x${SCOTCHDIR}" != "x" ; then
  SCOTCH_CPPFLAGS="-DENABLE_SCOTCH -I${SCOTCHDIR}"
  SCOTCH_LIBS="-L${SCOTCHDIR}/bin -lscotch -lscotcherr"
fi

scotch_ok=no
scotch_headers_ok=no
scotch_binaries_ok=no

dnl SCOTCH headers
AC_CHECKING(for SCOTCH headers)
CPPFLAGS="${CPPFLAGS_old} ${SCOTCH_CPPFLAGS} -std=c99"

scotch_include_dir_ok=yes
if test "x${SCOTCHDIR}" != "x" ; then
  AC_CHECK_FILE(${SCOTCHDIR}/bin/scotch.h,
                scotch_include_dir_ok=yes,
                scotch_include_dir_ok=no)
fi

if test "x${scotch_include_dir_ok}" = "xyes" ; then
  AC_TRY_COMPILE([#include <stdio.h>
		  #include <bin/scotch.h>],
                 [SCOTCH_Graph* graph;
		  SCOTCH_graphInit(graph)],
                 scotch_headers_ok=yes,
                 scotch_headers_ok=no)
fi

if test "x${scotch_headers_ok}" = "xno" ; then
  SCOTCH_CPPFLAGS="SCOTCH_CPPFLAGS_NOT_DEFINED"
else
  AC_MSG_RESULT(\$SCOTCH_CPPFLAGS = ${SCOTCH_CPPFLAGS})
fi
AC_MSG_RESULT(for scotch headers: $scotch_headers_ok)

if test "x${scotch_headers_ok}" = "xyes" ; then
  dnl SCOTCH binaries
  AC_CHECKING(for SCOTCH binaries)
  scotch_lib_dir_ok=yes
  if test "x${SCOTCHDIR}" != "x" ; then
    AC_CHECK_FILE(${SCOTCHDIR}/bin/libscotch.a,
                  scotch_lib_dir_ok=yes,
                  scotch_lib_dir_ok=no)
    if test "x${scotch_lib_dir_ok}" = "xno" ; then
      SCOTCH_LIBSUFFIX=""
      AC_CHECK_FILE(${SCOTCHDIR}/bin/libscotch.a,
                    scotch_lib_dir_ok=yes,
                    scotch_lib_dir_ok=no)
    fi
  fi
  if test "x${scotch_lib_dir_ok}" = "xyes" ; then
    LIBS="${LIBS_old} ${SCOTCH_LIBS}"
    AC_TRY_LINK([#include <stdio.h>
		#include <bin/scotch.h>],
                  [SCOTCH_Graph* graph;
		  SCOTCH_graphInit(graph)],
                scotch_binaries_ok=yes,
                scotch_binaries_ok=no)
    if test "x${scotch_binaries_ok}" = "xno" ; then
      SCOTCH_LIBSUFFIX=""
      LIBS="${LIBS_old} ${SCOTCH_LIBS} "
      AC_TRY_LINK([#include <stdio.h>
		   #include <bin/scotch.h>],
                  [SCOTCH_Graph* graph;
		  SCOTCH_graphInit(graph)],
                  scotch_binaries_ok=yes,
                  scotch_binaries_ok=no)
    fi
  fi
fi

if test "x${scotch_binaries_ok}" = "xno" ; then
  SCOTCH_LIBS="SCOTCH_LIBS_NOT_FOUND"
  SCOTCH_LIBSUFFIX="-not-defined"
else
  AC_MSG_RESULT(\$SCOTCH_LIBSUFFIX = ${SCOTCH_LIBSUFFIX})
  AC_MSG_RESULT(\$SCOTCH_LIBS = ${SCOTCH_LIBS})
fi
AC_MSG_RESULT(for scotch binaries: $scotch_binaries_ok)

CPPFLAGS="${CPPFLAGS_old}"
LIBS="${LIBS_old}"

if test "x${scotch_headers_ok}" = "xyes" ; then
  if test "x${scotch_binaries_ok}" = "xyes" ; then
    scotch_ok=yes
    ENABLE_SCOTCH="yes"
  fi
fi

AC_MSG_RESULT(for scotch: $scotch_ok)

AC_SUBST(SCOTCH_CPPFLAGS)
AC_SUBST(SCOTCH_LIBSUFFIX)
AC_SUBST(SCOTCH_LIBS)
AC_SUBST(ENABLE_SCOTCH)

AC_LANG_RESTORE

])dnl


