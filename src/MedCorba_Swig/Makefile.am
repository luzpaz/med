#  MED MedCorba_Swig : binding of MED CORBA objects woth Python
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES= libMedCorba_Swigcmodule.la

if MED_SWIG_1_3_21
SWIG_DEF = libMedCorba_Swig.i dummy.i

dummy_wrap.cxx: libMedCorba_Swig_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<FAMILY \* > >/vector<FAMILY \* >/g' libMedCorba_Swig_wrap.cxx > toto
	mv toto libMedCorba_Swig_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<SUPPORT \* > const >/vector<SUPPORT \* >/g' libMedCorba_Swig_wrap.cxx > toto
	mv toto libMedCorba_Swig_wrap.cxx
	touch dummy_wrap.cxx
else !MED_SWIG_1_3_21
SWIG_DEF = libMedCorba_Swig.i
endif

SWIG_FLAGS= @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../MEDMEM_SWIG
SWIG_DEP= ../MEDMEM_SWIG/libMEDMEM_Swig.i

nodist_libMedCorba_Swigcmodule_la_SOURCES= libMedCorba_Swig_wrap.cxx
salomeinclude_HEADERS= libMedCorba_Swig.i

libMedCorba_Swig.py: libMedCorba_Swig_wrap.cxx

libMedCorba_Swig_wrap.cxx: $(SWIG_DEF) $(SWIG_DEP)
	$(SWIG) $(SWIG_FLAGS) -o $@ $(SWIG_DEF)

#############################################################################

libMedCorba_Swigcmodule_la_CPPFLAGS= $(PYTHON_INCLUDES) $(MED2_INCLUDES) $(HDF5_INCLUDES) ${KERNEL_CXXFLAGS} -ftemplate-depth-42 \
	-I$(srcdir)/../MEDMEM_I -I$(srcdir)/../MEDMEM -I$(srcdir)/../MEDMEM_SWIG -I$(srcdir)/../MED -I$(srcdir)/../MEDWrapper/V2_1/Core \
	-I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl
libMedCorba_Swigcmodule_la_LDFLAGS=$(PYTHON_LIBS) $(MED2_LIBS) $(HDF5_LIBS)  ${KERNEL_LDFLAGS} -lSALOMELocalTrace \
	../MEDMEM_I/libMEDMEMImpl.la ../MEDMEM/libmedmem.la ../MEDMEM_SWIG/libMEDMEM_Swigcmodule.la ../MED/libMEDEngine.la

dist_salomescript_DATA = medcorba_test.py batchmode_medcorba_test.py batchmode_medcorba_test1.py
nodist_salomescript_DATA = libMedCorba_Swig.py

CLEANFILES= libMedCorba_Swig.py libMedCorba_Swig_wrap.cxx

install-exec-hook: $(libdir)/_libMedCorba_Swig.so

$(libdir)/_libMedCorba_Swig.so:
	ln -sf $(libdir)/libMedCorba_Swigcmodule.la $@
