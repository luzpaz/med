#
# Check configure option --with-kernel=[yes|no|DIR] taking into account that
# MED_WITH_KERNEL may be already set during build_configure,
# Set MED_WITH_KERNEL to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([WITH_KERNEL],[

AC_ARG_WITH(kernel,
	    [  --with-kernel default=yes ],
	    with_kernel="$withval",with_kernel="${MED_WITH_KERNEL}")

case $with_kernel in
  no)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*******   WITHOUT KERNEL configuration   *******)
#         AC_MSG_RESULT(************************************************)
        MED_WITH_KERNEL="no";;
  *)
        MED_WITH_KERNEL="yes"
esac

AC_SUBST(MED_WITH_KERNEL)
 
])dnl
