dnl Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
AC_DEFUN([CHECK_PARMETIS],[
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_REQUIRE([CHECK_MPI])dnl
AC_REQUIRE([CHECK_METIS])dnl

AC_CHECKING(for ParMETIS Library)

AC_LANG_SAVE
AC_LANG_C

PARMETIS_CPPFLAGS=""
PARMETIS_LIBS=""
ENABLE_PARMETIS="no"

AC_CHECKING(for ParMETIS location)
AC_ARG_WITH(parmetis,
            [  --with-parmetis=DIR      root directory path to ParMETIS library installation ],
            [PARMETISDIR="$withval"
             AC_MSG_RESULT("select $withval as path to ParMETIS library")])

AC_MSG_RESULT(\$PARMETISDIR = ${PARMETISDIR})

CPPFLAGS_old="${CPPFLAGS}"
LIBS_old=$LIBS

if test "x${PARMETISDIR}" != "x" ; then
  PARMETIS_CPPFLAGS="-DMED_ENABLE_PARMETIS -I${PARMETISDIR}/include ${MPI_INCLUDES}"
  PARMETIS_LIBS="-L${PARMETISDIR}/lib -lparmetis -lmetis ${MPI_LIBS}"
fi

parmetis_ok=no
parmetis_headers_ok=no
parmetis_binaries_ok=no

dnl ParMETIS headers
AC_CHECKING(for ParMETIS headers)
CPPFLAGS="${CPPFLAGS_old} ${PARMETIS_CPPFLAGS}"

parmetis_include_dir_ok=yes
if test "x${PARMETISDIR}" != "x" ; then
  AC_CHECK_FILE(${PARMETISDIR}/include/parmetis.h,
                parmetis_headers_ok=yes,
                parmetis_headers_ok=no)
fi

if test "x${parmetis_headers_ok}" = "xyes" ; then
  AC_MSG_RESULT(\$PARMETIS_CPPFLAGS = ${PARMETIS_CPPFLAGS})
fi
AC_MSG_RESULT(for ParMETIS headers: $parmetis_headers_ok)

if test "x${parmetis_headers_ok}" = "xyes" ; then
  dnl ParMETIS binaries
  AC_CHECKING(for ParMETIS binaries)
  AC_CHECK_FILE(${PARMETISDIR}/lib/libparmetis.a,
                parmetis_binaries_ok=yes,
                parmetis_binaries_ok=no)
fi

if test "x${parmetis_binaries_ok}" = "xyes" ; then
  AC_MSG_RESULT(\$PARMETIS_LIBS = ${PARMETIS_LIBS})
fi
AC_MSG_RESULT(for ParMETIS binaries: $parmetis_binaries_ok)

CPPFLAGS="${CPPFLAGS_old}"
LIBS="${LIBS_old}"

if test "x${parmetis_headers_ok}" = "xyes" ; then
  if test "x${parmetis_binaries_ok}" = "xyes" ; then
    parmetis_ok=yes
    ENABLE_PARMETIS="yes"
    # ParMETIS includes METIS, so we redefine METIS cppflags and libs
    # And metis.h #include parmetis.h + mpi.h
    #metis_ok=yes
    #ENABLE_METIS="yes"
    #METIS_CPPFLAGS="-DENABLE_METIS -I${METISDIR}/METISLib ${PARMETIS_CPPFLAGS}"
    #METIS_LIBS="-L${METISDIR} -lmetis ${MPI_LIBS}"
  fi
fi

AC_MSG_RESULT(for ParMETIS: $parmetis_ok)

AC_SUBST(ENABLE_PARMETIS)
AC_SUBST(PARMETIS_CPPFLAGS)
AC_SUBST(PARMETIS_LIBS)

AC_LANG_RESTORE

])dnl
