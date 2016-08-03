dnl Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

# Check availability of Med binary distribution
#
# Author : Nicolas REJNERI (OPEN CASCADE, 2003)
#

AC_DEFUN([CHECK_MED],[
AC_REQUIRE([AC_LINKER_OPTIONS])dnl
AC_REQUIRE([CHECK_MED3])

AC_CHECKING(for Med)

Med_ok=no

MED_LDFLAGS=""
MED_CXXFLAGS=""

AC_ARG_WITH(med,
	    [  --with-med=DIR root directory path of MED installation ],
	    MED_DIR="$withval",MED_DIR="")

if test "x${MED_DIR}" == "x" ; then
  AC_MSG_RESULT(for \${MED_ROOT_DIR}: ${MED_ROOT_DIR})
   # --with-med option is not used
   if test "x${MED_ROOT_DIR}" != "x" ; then

    # MED_ROOT_DIR environment variable defined
      MED_DIR=${MED_ROOT_DIR}

   fi

fi

if test -f ${MED_DIR}/idl/salome/MEDCouplingCorbaServant.idl ; then
   AC_MSG_RESULT(Using Med module distribution in ${MED_DIR})
   Med_ok=yes

   if test "x$MED_ROOT_DIR" == "x" ; then
      MED_ROOT_DIR=${MED_DIR}
   fi

   AC_SUBST(MED_ROOT_DIR)

   MED_LDFLAGS=-L${MED_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
   MED_CXXFLAGS=-I${MED_DIR}/include/salome

   AC_SUBST(MED_LDFLAGS)
   AC_SUBST(MED_CXXFLAGS)

else
   AC_MSG_WARN("Cannot find Med module sources")
fi

AC_MSG_RESULT(for Med: $Med_ok)

])dnl
