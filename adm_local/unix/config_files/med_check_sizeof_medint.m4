AC_DEFUN([MED_CHECK_SIZEOF_MEDINT], [

  ## Guess the fortran int size
  AC_CHECK_SIZEOF_FORTRAN(integer)
  if test "x$ac_cv_sizeof_fortran_integer" = "x8" ; then
     AC_CHECK_SIZEOF(long)
     test "x$ac_cv_sizeof_long" = "x8" || AC_MSG_ERROR([Size of C type long expected to be eight bytes])
     DEFINED_F77INT64="-DHAVE_F77INT64"
  elif test "x$ac_cv_sizeof_fortran_integer" = "x4" ; then
     AC_CHECK_SIZEOF(int)
     test "x$ac_cv_sizeof_int" = "x4" || AC_MSG_ERROR([Size of C type int expected to be four bytes])
     DEFINED_F77INT64=""
  else
     AC_MSG_ERROR([Size of Fortran type integer is neither four nor eigth bytes])
  fi
  AC_SUBST(DEFINED_F77INT64)
])