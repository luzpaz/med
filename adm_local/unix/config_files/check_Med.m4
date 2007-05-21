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

MED_ENABLE_MULTIPR=no

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

   # MULTIPR
   AC_CHECK_FILE(${MED_DIR}/include/salome/MULTIPR_Obj.hxx,
                 ENABLE_MULTIPR=yes,
                 ENABLE_MULTIPR=no)
   AC_SUBST(ENABLE_MULTIPR)
   MULTIPR_CPPFLAGS=""
   MULTIPR_LIBS=""
   if test "x${ENABLE_MULTIPR}" = "xyes" ; then
      MULTIPR_CPPFLAGS="-DENABLE_MULTIPR"
      MULTIPR_LIBS="-lMULTIPR_API"
   fi
   AC_SUBST(MULTIPR_CPPFLAGS)
   AC_SUBST(MULTIPR_LIBS)
else
   AC_MSG_WARN("Cannot find Med module sources")
fi

AC_MSG_RESULT(for Med: $Med_ok)
AC_MSG_RESULT(for MULTIPR Med package: $ENABLE_MULTIPR)

])dnl
