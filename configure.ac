# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# Author : Marc Tajchman (CEA)
# Date : 28/06/2001
# Modified by : Patrick GOLDBRONN (CEA)
# Modified by : Marc Tajchman (CEA)
# Modified by : Mikhail PONIKAROV (OCN)
#
AC_INIT([Salome2 Project MED module], [6.6.0], [webmaster.salome@opencascade.com], [SalomeMED])
AC_CONFIG_AUX_DIR(adm_local/unix/config_files)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([tar-pax -Wno-portability])

XVERSION=`echo $VERSION | awk -F. '{printf("0x%02x%02x%02x",$1,$2,$3)}'`
AC_SUBST(XVERSION)
VERSION_DEV=0
AC_SUBST(VERSION_DEV)

# set up MODULE_NAME variable for dynamic construction of directories (resources, etc.)
MODULE_NAME=med
AC_SUBST(MODULE_NAME)

dnl
dnl Initialize source and build root directories
dnl
ROOT_BUILDDIR=`pwd`
ROOT_SRCDIR=`echo $0 | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
cd $ROOT_SRCDIR
ROOT_SRCDIR=`pwd`
cd $ROOT_BUILDDIR

AC_SUBST(ROOT_SRCDIR)
AC_SUBST(ROOT_BUILDDIR)

echo
echo Source root directory : $ROOT_SRCDIR
echo Build  root directory : $ROOT_BUILDDIR
echo

dnl remember MED_WITH_KERNEL set by build configure
MED_WITH_KERNEL=yes
old_with_kernel=${MED_WITH_KERNEL}

dnl
dnl Check --with-kernel option
dnl
dnl  ---------------------------------------------
                 WITH_KERNEL
dnl  ---------------------------------------------
dnl

dnl   Update and re-run configure if there was  build_configure --without-kernel
dnl   but configure --with-kernel=DIR is being called

if test "x${old_with_kernel}" != "x${MED_WITH_KERNEL}"; then
    if test "x${old_with_kernel}" = "xno" ; then
         if test ! -d "${KERNEL_ROOT_DIR}"; then
             echo "failed : KERNEL_ROOT_DIR variable is not correct !"
             exit
         fi
         kernel_check_in_aclocal=`grep KERNEL_ROOT_DIR ${ROOT_SRCDIR}/aclocal.m4`
         if test "x${kernel_check_in_aclocal}" = "x"; then
             echo "Configuration changed: without KERNEL -> with KERNEL"
             echo -n "Updating 'configure' script ...  "
             cd $ROOT_SRCDIR
           if build_configure --with-kernel={MED_WITH_KERNEL}; then
             echo "... done"
           else
             echo "... failed"
             cd $ROOT_BUILDDIR
             exit 1
           fi
             cd $ROOT_BUILDDIR
             $0 $*
             exit
         fi
    fi
fi

echo
echo ---------------------------------------------
echo "MED_WITH_KERNEL: ${MED_WITH_KERNEL}"
echo ---------------------------------------------
echo

###
# --with-interp-kernel-only option: available only in --with-kernel=no mode (see build_configure)
###

AC_ARG_WITH([interp-kernel-only],
          [AC_HELP_STRING([--with-interp-kernel-only],[Build MicroMED packages only, default=no])],
          [],[with_interp_kernel_only=no])

if test "$MED_WITH_KERNEL" == "yes" ; then
  if test "$with_interp_kernel_only" != "no" ; then
    AC_MSG_WARN([Option --with-interp-kernel-only is supported in --without-kernel mode only])
  fi
  with_interp_kernel_only="no"
fi

###
# Common tests
###

dnl Modification B. Secher portage sur osf CCRT
AC_CHECK_PROG(SHELL,sh,,)
AC_SUBST(SHELL)

if test -z "$AR"; then
   AC_CHECK_PROGS(AR,ar xar,:,$PATH)
fi
AC_SUBST(AR)

dnl Export the AR macro so that it will be placed in the libtool file
dnl correctly.
export AR

echo
echo ---------------------------------------------
echo testing make
echo ---------------------------------------------
echo

AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_LOCAL_INSTALL
dnl 
dnl libtool macro check for CC, LD, NM, LN_S, RANLIB, STRIP + pour les librairies dynamiques !

AC_ENABLE_DEBUG(yes)
AC_DISABLE_PRODUCTION

echo
echo ---------------------------------------------
echo testing libtool
echo ---------------------------------------------
echo

dnl first, we set static to no!
dnl if we want it, use --enable-static
AC_ENABLE_STATIC(no)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

dnl Fix up the INSTALL macro if it s a relative path. We want the
dnl full-path to the binary instead.
case "$INSTALL" in
   *install-sh*)
      INSTALL='\${ROOT_BUILDDIR}'/adm_local/unix/config_files/install-sh
      ;;
esac

echo
echo ---------------------------------------------
echo testing C/C++
echo ---------------------------------------------
echo

cc_ok=no
dnl inutil car libtool
dnl AC_PROG_CC
AC_PROG_CXX
AC_CXX_WARNINGS
AC_CXX_TEMPLATE_OPTIONS
AC_DEPEND_FLAG
# AC_CC_WARNINGS([ansi])
cc_ok=yes

dnl Library libdl :
AC_CHECK_LIB(dl,dlopen)

dnl Library librt : for alpha/osf
AC_CHECK_LIB(rt,nanosleep)

dnl add library libm :
AC_CHECK_LIB(m,ceil)

dnl 
dnl Check if we use std iostream by default or if we must add
dnl a compiler directive for that
dnl

AC_CXX_USE_STD_IOSTREAM

dnl 
dnl Well we use sstream which is not in gcc pre-2.95.3
dnl We must test if it exists. If not, add it in include !
dnl

AC_CXX_HAVE_SSTREAM

dnl
dnl ---------------------------------------------
dnl testing linker
dnl ---------------------------------------------
dnl

AC_LINKER_OPTIONS

echo
echo ---------------------------------------------
echo testing threads
echo ---------------------------------------------
echo

ENABLE_PTHREADS

echo
echo ---------------------------------------------
echo testing MPI
echo ---------------------------------------------
echo

CHECK_MPI

echo
echo ---------------------------------------------
echo testing HDF5
echo ---------------------------------------------
echo

CHECK_HDF5

echo
echo ---------------------------------------------
echo testing MED3
echo ---------------------------------------------
echo

CHECK_MED3

echo
echo ---------------------------------------------------
echo "testing CPPUNIT (only required for unitary testing)"
echo --------------------------------------------------
echo

CHECK_CPPUNIT

echo
echo ---------------------------------------------
echo testing FVM library generators
echo ---------------------------------------------
echo

CHECK_FVM

echo
echo ---------------------------------------------
echo testing html generators
echo ---------------------------------------------
echo

CHECK_SPHINX
CHECK_HTML_GENERATORS

###
# Defaults
###

gui_ok=no
# CHECK_GUI ONLY if MED_WITH_KERNEL is yes

WITHIHM="no"

# This flags should be used to switch between the MEDMEM GUI (defined
# in the MEDGUI package) and the MEDOP GUI (defined in the MEDOP
# package). In any case the module factory function is provided by the
# MEDOP GUI library (package MEDOP/gui) and this factory function
# creates weither a MedGUI instance or a MEDOPModule instance,
# depending on the value of this flags. The default is to use the new
# MEDOP GUI. 
AM_CONDITIONAL(WITH_MEDMEMGUI,test "1" = "0")

###
# Other tests should be performed only in --with-interp-kernel-only=no mode
###

if test "$with_interp_kernel_only" != "yes" ; then
    
    echo
    echo ---------------------------------------------
    echo testing python
    echo ---------------------------------------------
    echo
    
    CHECK_PYTHON
    
    echo
    echo ---------------------------------------------
    echo testing swig
    echo ---------------------------------------------
    echo
    
    AM_PATH_PYTHON(2.3)
    CHECK_SWIG
    
    echo
    echo ---------------------------------------------
    echo testing BOOST Library
    echo ---------------------------------------------
    echo
    
    CHECK_BOOST 

    
    echo
    echo ---------------------------------------------
    echo checking SPLITTER
    echo ---------------------------------------------
    echo
    
    AC_ENABLE_SPLITTER(yes)
    
    if test "x$ENABLE_SPLITTER" = "xyes"; then

        echo  
        echo --------------------------------------------- 
        echo testing ParMETIS 
        echo ---------------------------------------------
        echo 
        parmetis_ok=no 
        CHECK_PARMETIS dnl parmetis_ok is set to yes by CHECK_PARMETIS

        echo  
        echo --------------------------------------------- 
        echo testing METIS 
        echo ---------------------------------------------
        echo 
        metis_ok=no 
        CHECK_METIS dnl metis_ok is set to yes by CHECK_METIS
        
        echo  
        echo --------------------------------------------- 
        echo testing SCOTCH 
        echo ---------------------------------------------
        scotch_ok=no 
        CHECK_SCOTCH 
        dnl scotch_ok is set to yes by CHECK_SCOTCH    
        
        echo        
        echo --------------------------------------------- 
        echo testing LIBXML2
        echo ---------------------------------------------
        libxml_ok=no 
        CHECK_LIBXML 
        dnl libxml_ok is set to yes by CHECK_LIBXML         
        
    fi
    
    AC_ENABLE_RENUMBER(yes)

    if test "x$ENABLE_RENUMBER" = "xyes"; then
        if test "x$metis_ok" = "x"; then
            echo  
            echo --------------------------------------------- 
            echo testing METIS 
            echo ---------------------------------------------
            echo 
            metis_ok=no 
            CHECK_METIS dnl metis_ok is set to yes by CHECK_METIS
        fi
    fi

    if test "${MED_WITH_KERNEL}" = "yes"; then
        
      dnl
      dnl ---------------------------------------------
      dnl testing sockets
      dnl ---------------------------------------------
      dnl
        
      CHECK_SOCKETS
        
      echo
      echo ---------------------------------------------
      echo testing omniORB
      echo ---------------------------------------------
      echo
        
      CHECK_OMNIORB
        
      DEFAULT_ORB=omniORB
      CHECK_CORBA
        
      AC_SUBST_FILE(CORBA)
      corba=make_$ORB
      CORBA=adm_local/unix/$corba
        
      dnl echo
      dnl echo ---------------------------------------------
      dnl echo testing mico
      dnl echo ---------------------------------------------
      dnl echo
        
      dnl CHECK_MICO
        
      echo
      echo ---------------------------------------------
      echo testing SALOME Kernel
      echo ---------------------------------------------
      echo
        
      CHECK_KERNEL
        
        echo
        echo ---------------------------------------------
        echo testing SALOME GUI
        echo ---------------------------------------------
        echo
        
        CHECK_GUI_MODULE
        
        if test "${SalomeGUI_need}" != "no" -a "${FullGUI_ok}" = "yes" ; then 
            gui_ok=yes
        fi
        
        if test "${SalomeGUI_need}" == "yes"; then
            if test "${FullGUI_ok}" != "yes"; then
                AC_MSG_WARN(For configure MED module necessary full GUI!)
            fi
        elif test "${SalomeGUI_need}" == "auto"; then
            if test "${FullGUI_ok}" != "yes"; then
                AC_MSG_WARN(Full GUI not found. Build will be done without GUI!)
            fi
        elif test "${SalomeGUI_need}" == "no"; then
            echo Build without GUI option has been chosen
        fi
        
      if test "${gui_ok}" = "yes" ; then
            
            echo
            echo ---------------------------------------------
            echo testing LEX \& YACC
            echo ---------------------------------------------
            echo
            
            lex_yacc_ok=no
            AC_PROG_YACC
            AC_PROG_LEX
            lex_yacc_ok=yes
            
            echo
            echo ---------------------------------------------
            echo testing openGL
            echo ---------------------------------------------
            echo
            
            CHECK_OPENGL
            
            echo
            echo ---------------------------------------------
            echo testing QT
            echo ---------------------------------------------
            echo
            
            CHECK_QT
            
            echo
            echo ---------------------------------------------
            echo testing VTK
            echo ---------------------------------------------
            echo
            
            CHECK_VTK
            
            echo
            echo ---------------------------------------------
            echo testing OpenCascade
            echo ---------------------------------------------
            echo
            
            CHECK_CAS
            
        fi # if "${gui_ok}" = "yes" ...
        
    fi # MED_WITH_KERNEL
    
fi # "$with_interp_kernel_only" != "yes" ...


###
# Additional substitutions
###

AC_SUBST(WITHIHM)

###
# Conditionals
###

AM_CONDITIONAL(MED_ENABLE_KERNEL,   [test "$MED_WITH_KERNEL" = yes])
AM_CONDITIONAL(MED_ENABLE_MICROMED, [test "$with_interp_kernel_only" = yes])
AM_CONDITIONAL(MED_ENABLE_GUI,      [test "${gui_ok}" = "yes"])
AM_CONDITIONAL(MED_ENABLE_SSTREAM,  [test "$HAVE_SSTREAM" = yes])
AM_CONDITIONAL(MED_SWIG_1_3_21,     [test "$SWIG_VERSION" = 1.3.21])
AM_CONDITIONAL(MED_ENABLE_SPLITTER, [test "$ENABLE_SPLITTER" = yes])
AM_CONDITIONAL(MED_ENABLE_PARMETIS, [test "$ENABLE_PARMETIS" = yes])
AM_CONDITIONAL(MED_ENABLE_METIS,    [test "$ENABLE_METIS" = yes])
AM_CONDITIONAL(MED_ENABLE_SCOTCH,   [test "$ENABLE_SCOTCH" = yes])
AM_CONDITIONAL(MED_WITHMPI,         [test "$WITHMPI" = yes])
AM_CONDITIONAL(MED_ENABLE_FVM,      [test "$ENABLE_FVM" = yes])
AM_CONDITIONAL(MED_ENABLE_MED3,     [test "$med3_ok" = yes])
AM_CONDITIONAL(MED_ENABLE_RENUMBER, [test "$ENABLE_RENUMBER" = yes])
AM_CONDITIONAL(BOOST_IS_OK,         [test "$boost_ok" = yes])
###
# Print summary
###

echo
echo ---------------------------------------------
echo Summary
echo ---------------------------------------------
echo

if test "$with_interp_kernel_only" = "yes"; then 
    echo "Configure (Micro MED)"
elif test "$MED_WITH_KERNEL" = "no"; then 
    echo "Configure (without Kernel)"
elif test "${SalomeGUI_need}" == "no"; then
    echo "Configure (without GUI)"
else
    echo Configure
fi

obligatory_vars="cc_ok threads_ok doxygen_ok"
optional_vars="mpi_ok fvm_ok cppunit_ok"
gui_vars=""

if test "x$ENABLE_SPLITTER" = "xyes" ; then
  splitter_obligatory_vars="libxml_ok"
  splitter_optional_vars="metis_ok parmetis_ok scotch_ok"
fi

if test "x$ENABLE_RENUMBER" = "xyes" ; then
  if test "x$splitter_optional_vars" = "x" ; then
    renumber_optional_vars="metis_ok"
  fi
fi
if test "$MED_WITH_KERNEL" = "yes" ; then 
    # --with-kernel=yes
    obligatory_vars="$obligatory_vars python_ok boost_ok swig_ok hdf5_ok med3_ok omniORB_ok $splitter_obligatory_vars"
    optional_vars="$optional_vars $splitter_optional_vars $renumber_optional_vars graphviz_ok Kernel_ok"
    if test "${gui_ok}" = "yes"; then
        # GUI is enabled
        gui_vars="lex_yacc_ok OpenGL_ok qt_ok vtk_ok occ_ok gui_ok"
    elif test "${SalomeGUI_need}" != "no"; then
        gui_vars="gui_ok"
    fi
else
    # --with-kernel=no
    if test "$with_interp_kernel_only" == "yes" ; then
        # --with-interp-kernel-only=yes mode
        optional_vars="$optional_vars $splitter_optional_vars $renumber_optional_vars hdf5_ok med3_ok"
    else
        # --with-interp-kernel-only=no mode
        obligatory_vars="$obligatory_vars python_ok boost_ok swig_ok hdf5_ok med3_ok $splitter_obligatory_vars"
        optional_vars="$optional_vars $splitter_optional_vars $renumber_optional_vars"
    fi
fi

###
# obligatory products
###

echo "--- Obligatory products:"

missing_obligatory=no
for var in $obligatory_vars
do
   eval toto=\$$var
   if test x$toto == "x"; then toto="no" ; fi
   if test x$toto == "xno"; then missing_obligatory=yes ; fi
     printf "   %10s : " `echo \$var | sed -e "s,_ok,,"`
     eval echo \$$var
   #fi
done
if test x$missing_obligatory == "xyes"; then 
AC_MSG_ERROR([one or more obligatory product is missing])
fi

###
# GUI products (if enabled)
###

if test "$gui_vars" != "" ; then

    echo "--- GUI products (optional):"

    for var in $gui_vars
    do
        eval toto=\$$var
        if test x$toto == "x"; then toto="no"; fi
        printf "   %10s : " `echo \$var | sed -e "s,_ok,,"`
        eval echo \$toto
    done
fi

###
# optional products
###

echo "--- Other (optional) products:"

for var in $optional_vars
do
   eval toto=\$$var
   if test x$toto != "x"; then
     printf "   %10s : " `echo \$var | sed -e "s,_ok,,"`
     eval echo \$$var
   fi
done

###
# default ORB (if enabled)
###

if test "$DEFAULT_ORB" != "" ; then
   echo
   echo "Default ORB   : $DEFAULT_ORB"
   echo
fi

dnl We don t need to say when we re entering directories if we re using
dnl GNU make becuase make does it for us.
if test "X$GMAKE" = "Xyes"; then
   AC_SUBST(SETX) SETX=":"
else
   AC_SUBST(SETX) SETX="set -x"
fi

if test -n "${NO_MED_TRACE+X}"; then
    CPPFLAGS="${CPPFLAGS} -D_NO_MED_TRACE_"
fi

echo
echo ---------------------------------------------
echo generating Makefiles and configure files
echo ---------------------------------------------
echo

AC_OUTPUT_COMMANDS([ \
      chmod +x ./bin/* \
])

AC_HACK_LIBTOOL
AC_CONFIG_COMMANDS([hack_libtool],[
sed -i "s%^CC=\"\(.*\)\"%hack_libtool (){ \n\
  $(pwd)/hack_libtool \1 \"\$[@]\" \n\
}\n\
CC=\"hack_libtool\"%g" libtool
sed -i "s%\(\s*\)for searchdir in \$newlib_search_path \$lib_search_path \$sys_lib_search_path \$shlib_search_path; do%\1searchdirs=\"\$newlib_search_path \$lib_search_path \$sys_lib_search_path \$shlib_search_path\"\n\1for searchdir in \$searchdirs; do%g" libtool
sed -i "s%\(\s*\)searchdirs=\"\$newlib_search_path \$lib_search_path \(.*\)\"%\1searchdirs=\"\$newlib_search_path \$lib_search_path\"\n\1sss_beg=\"\"\n\1sss_end=\"\2\"%g" libtool
sed -i "s%\(\s*\)\(for searchdir in \$searchdirs; do\)%\1for sss in \$searchdirs; do\n\1  if ! test -d \$sss; then continue; fi\n\1  ssss=\$(cd \$sss; pwd)\n\1  if test \"\$ssss\" != \"\" \&\& test -d \$ssss; then\n\1    case \$ssss in\n\1      /usr/lib | /usr/lib64 ) ;;\n\1      * ) sss_beg=\"\$sss_beg \$ssss\" ;;\n\1    esac\n\1  fi\n\1done\n\1searchdirs=\"\$sss_beg \$sss_end\"\n\1\2%g" libtool
],[])

AC_OUTPUT([ \
        adm_local_without_kernel/SALOMEconfig.h \
        Makefile \
        MED_version.h \
        adm_local/unix/config_files/Makefile \
        adm_local/unix/Makefile \
        adm_local/cmake_files/Makefile \
        adm_local/Makefile \
        adm_local_without_kernel/unix/config_files/Makefile \
        adm_local_without_kernel/unix/Makefile \
        adm_local_without_kernel/Makefile \
        resources/Makefile
        resources/SalomeApp.xml \
        resources/MEDCatalog.xml \
        bin/Makefile \
        bin/VERSION \
        idl/Makefile \
        src/Makefile \
        src/MEDWrapper/Makefile \
        src/MEDWrapper/Base/Makefile \
        src/MEDWrapper/Factory/Makefile \
        src/MEDWrapper/V2_1/Makefile \
        src/MEDWrapper/V2_1/Core/Makefile \
        src/MEDWrapper/V2_1/Wrapper/Makefile \
        src/MEDWrapper/V2_2/Makefile \
        src/MEDMEM/Doxyfile_med_devel \
        src/MEDMEM/Doxyfile_med_user \
        src/MEDMEM/Makefile \
        src/MEDMEM_SWIG/Makefile \
        src/MEDMEMBinTest/Makefile \
        src/MEDMEMCppTest/Makefile \
        src/INTERP_KERNEL/Makefile \
        src/INTERP_KERNELTest/Makefile \
        src/MEDCoupling/Makefile \
        src/MEDCoupling/Test/Makefile \
        src/MEDLoader/Makefile \
        src/MEDLoader/Swig/Makefile \
        src/MEDLoader/Test/Makefile \
        src/MEDCoupling_Swig/Makefile \
        src/MEDCouplingCorba/Makefile \
        src/MEDCouplingCorba/Client/Makefile \
        src/MEDCouplingCorba/Test/Makefile \
        src/MEDCouplingCorba_Swig/Makefile \
        src/MEDCouplingCorba_Swig/Client/Makefile \
        src/MEDCalculator/Makefile \
        src/MEDCalculator/Swig/Makefile \
        src/MEDCalculator/Test/Makefile \
        src/MEDSPLITTER/Makefile \
        src/MEDSPLITTER/Test/Makefile \
        src/MEDSPLITTER_Swig/Makefile \
        src/MEDPartitioner/Makefile \
        src/MEDPartitioner/Test/Makefile \
        src/RENUMBER/Makefile \
        src/ParaMEDCouplingCorba/Makefile \
        src/ParaMEDMEM/Makefile \
        src/ParaMEDMEMComponent/Makefile \
        src/ParaMEDLoader/Makefile \
        src/ParaMEDMEMTest/Makefile \
        src/ParaMEDMEM_Swig/Makefile \
        src/MEDMEM_I/Makefile \
        src/MED/Makefile \
        src/MedCorba_Swig/Makefile \
        src/MED_SWIG/Makefile \
        src/MedClient/Makefile \
        src/MedClient/src/Makefile \
        src/MedClient/test/Makefile \
        src/MedClient/test/environ/Makefile \
        src/MedClient/test/environ/runContainer \
        src/MedClient/test/environ/runEnvironTests \
        src/MedClient/test/environ/runTestMedCorba \
        src/MedClient/test/environ/stopContainer \
        src/MedClient/test/environ/csh/init1 \
        src/MedClient/test/environ/csh/init2 \
        src/MedClient/test/environ/csh/init3 \
        src/MedClient/test/environ/csh/Makefile \
        src/MedClient/test/environ/csh/runContainer \
        src/MedClient/test/environ/csh/runEnvironTests \
        src/MedClient/test/environ/csh/stopContainer \
        src/MedClient/test/test1/Makefile \
        src/MedClient/test/test2/Makefile \
        src/MEDGUI/Makefile \
	src/MEDOP/Makefile \
	src/MEDOP/cmp/Makefile \
	src/MEDOP/tui/Makefile \
	src/MEDOP/tui/xmedpy/Makefile \
	src/MEDOP/res/Makefile \
	src/MEDOP/res/testfiles/Makefile \
	src/MEDOP/gui/dialogs/Makefile \
	src/MEDOP/gui/Makefile \
	src/MEDOP/exe/Makefile \
	src/MEDOP/exe/appligen/Makefile \
	src/MEDOP/exe/appligen/config_appli_model.xml \
	src/MEDOP/exe/appligen/appligen.sh \
	src/MEDOP/exe/image2med/Makefile \
	src/MEDOP/doc/Makefile \
	src/MEDOP/doc/sphinx/conf.py \
	src/MEDOP/doc/sphinx/Makefile \
        doc/Makefile \
        doc/MEDMEM/Makefile \
        doc/MEDMEM/MEDMEM_Content.tex \
        doc/MEDMEM/MEDMEM_UML_light.png \
        doc/MEDMEM/MEDMEM_UML.png \
        doc/MEDMEM/MEDMEM_UsersGuide.tex \
        doc/salome/Makefile \
        doc/salome/tui/Makefile \
        doc/salome/tui/doxyfile \
        doc/salome/tui/static/header.html \
        doc/doxygen/Makefile \
        doc/doxygen/Doxyfile_med_user \
        doc/doxygen/static/header.html \
])
