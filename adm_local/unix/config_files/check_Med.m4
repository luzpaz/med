# Check availability of Med binary distribution
#
# Author : Nicolas REJNERI (OPEN CASCADE, 2003)
#

AC_DEFUN([CHECK_MED],[
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_CHECKING(for Med)

Med_ok=no

MED_LDFLAGS=""
MED_CXXFLAGS=""

AC_ARG_WITH(med,
	    [  --with-med=DIR root directory path of MED installation ],
	    MED_DIR="$withval",MED_DIR="")

if test "x${MED_DIR}" == "x" ; then
  AC_MSG_RESULT(for \${MED_ROOT_DIR}: ${MED_ROOT_DIR})
# no --with-med-dir option used
   if test "x${MED_ROOT_DIR}" != "x" ; then

    # MED_ROOT_DIR environment variable defined
      MED_DIR=${MED_ROOT_DIR}

   fi

fi

if test -f ${MED_DIR}/idl/salome/MED.idl ; then
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
 
