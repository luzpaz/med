dnl  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
AC_DEFUN([CHECK_LIBXML],[

AC_CHECKING(for libXML Library)

AC_LANG_SAVE
AC_LANG_C

LIBXML_CPPFLAGS=""
LIBXML_LIBSUFFIX=""
LIBXML_LIBS=""

AC_CHECKING(for LIBXML location)
AC_ARG_WITH(libxml,
            [  --with-libxml=DIR      root directory path to LIBXML library installation ],
            [LIBXML_DIR="$withval"
             AC_MSG_RESULT("select $withval as path to LIBXML library")])

AC_MSG_RESULT(\$LIBXML_DIR = ${LIBXML_DIR})

CPPFLAGS_old="${CPPFLAGS}"
LIBS_old=$LIBS

if test "x${LIBXML_DIR}" != "x" ; then
  LIBXML_CPPFLAGS="-I${LIBXML_DIR}/include/libxml2"
  if test "x$LIBXML_DIR" = "x/usr"
  then
    LIBXML_LIBS="-lxml2"
  else
    LIBXML_LIBS="-L$LIBXML_DIR/lib -lxml2"
  fi
else
  if test -f /usr/include/libxml2/libxml/tree.h ; then
    LIBXML_DIR="/usr"
    LIBXML_CPPFLAGS="-I${LIBXML_DIR}/include/libxml2"
    LIBXML_LIBS="-lxml2"
  fi
fi

libxml_ok=no
libxml_headers_ok=no
libxml_binaries_ok=no

dnl LIBXML headers
AC_CHECKING(for LIBXML headers)
CPPFLAGS="${CPPFLAGS_old} ${LIBXML_CPPFLAGS}"

libxml_include_dir_ok=yes
if test "x${LIBXML_DIR}" != "x" ; then
  AC_CHECK_FILE(${LIBXML_DIR}/include/libxml2/libxml/tree.h,
                libxml_include_dir_ok=yes,
                libxml_include_dir_ok=no)
fi

if test "x${libxml_include_dir_ok}" = "xyes" ; then
  AC_TRY_COMPILE([#include <libxml/tree.h>],
                 [xmlNewDoc("1.0")],
                 libxml_headers_ok=yes,
                 libxml_headers_ok=no)
fi

if test "x${libxml_headers_ok}" = "xno" ; then
  LIBXML_CPPFLAGS="LIBXML_CPPFLAGS_NOT_DEFINED"
else
  AC_MSG_RESULT(\$LIBXML_CPPFLAGS = ${LIBXML_CPPFLAGS})
fi
AC_MSG_RESULT(for libxml headers: $libxml_headers_ok)

if test "x${libxml_headers_ok}" = "xyes" ; then
dnl LIBXML binaries
  AC_CHECKING(for LIBXML binaries)
  libxml_lib_dir_ok=yes
  if test "x${LIBXML_DIR}" != "x" ; then
    AC_CHECK_FILE(${LIBXML_DIR}/lib/libxml2.a,
                  libxml_lib_dir_ok=yes,
                  libxml_lib_dir_ok=no)
    if test "x${libxml_lib_dir_ok}" = "xno" ; then
      LIBXML_LIBSUFFIX=""
      AC_CHECK_FILE(${LIBXML_DIR}/lib/libxml2.a,
                    libxml_lib_dir_ok=yes,
                    libxml_lib_dir_ok=no)
    fi
  fi
  if test "x${libxml_lib_dir_ok}" = "xyes" ; then
    LIBS="${LIBS_old} ${LIBXML_LIBS}"
    AC_TRY_LINK([#include <libxml/tree.h>],
                [xmlNewDoc("1.0")],
                libxml_binaries_ok=yes,
                libxml_binaries_ok=no)
    if test "x${libxml_binaries_ok}" = "xno" ; then
      LIBXML_LIBSUFFIX=""
      LIBS="${LIBS_old} ${LIBXML_LIBS} "
      AC_TRY_LINK([#include <libxml/tree.h>],
                  [xmlNewDoc("1.0")],
                  libxml_binaries_ok=yes,
                  libxml_binaries_ok=no)
    fi
  fi
fi

if test "x${libxml_binaries_ok}" = "xno" ; then
  LIBXML_LIBS="LIBXML_LIBS_NOT_FOUND"
  LIBXML_LIBSUFFIX="-not-defined"
else
  AC_MSG_RESULT(\$LIBXML_LIBSUFFIX = ${LIBXML_LIBSUFFIX})
  AC_MSG_RESULT(\$LIBXML_LIBS = ${LIBXML_LIBS})
fi
AC_MSG_RESULT(for libxml binaries: $libxml_binaries_ok)

CPPFLAGS="${CPPFLAGS_old}"
LIBS="${LIBS_old}"

if test "x${libxml_headers_ok}" = "xyes" ; then
  if test "x${libxml_binaries_ok}" = "xyes" ; then
    libxml_ok=yes
  fi
fi

AC_MSG_RESULT(for libxml: $libxml_ok)

AC_SUBST(LIBXML_CPPFLAGS)
AC_SUBST(LIBXML_LIBSUFFIX)
AC_SUBST(LIBXML_LIBS)

AC_LANG_RESTORE

])dnl


