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

#  MED MedCorba_Swig : binding of MED CORBA objects woth Python
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

salomepython_PYTHON = libMedCorba_Swig.py
salomepyexec_LTLIBRARIES = _libMedCorba_Swig.la

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

SWIG_FLAGS = @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../MEDMEM_SWIG
SWIG_DEP   = ../MEDMEM_SWIG/libMEDMEM_Swig.i

dist__libMedCorba_Swig_la_SOURCES   = libMedCorba_Swig.i
nodist__libMedCorba_Swig_la_SOURCES = libMedCorba_Swig_wrap.cxx
salomeinclude_HEADERS = libMedCorba_Swig.i

libMedCorba_Swig.py: libMedCorba_Swig_wrap.cxx

libMedCorba_Swig_wrap.cxx: $(SWIG_DEF) $(SWIG_DEP)
	$(SWIG) $(SWIG_FLAGS) -o $@ $(SWIG_DEF)

#############################################################################

_libMedCorba_Swig_la_CPPFLAGS = $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) \
	$(PYTHON_INCLUDES) $(MED3_INCLUDES) $(HDF5_INCLUDES) ${KERNEL_CXXFLAGS} \
	-I$(srcdir)/../MEDMEM_I -I$(srcdir)/../MEDMEM -I$(srcdir)/../MEDMEM_SWIG -I$(srcdir)/../MED -I$(srcdir)/../MEDWrapper/V2_1/Core \
	-I$(srcdir)/../INTERP_KERNEL -I$(srcdir)/../INTERP_KERNEL/Bases -I$(srcdir)/../INTERP_KERNEL/GaussPoints -I$(top_builddir)/idl
_libMedCorba_Swig_la_LDFLAGS  = -module $(PYTHON_LIBS) $(MED3_LIBS_C_ONLY) $(HDF5_LIBS)  ${KERNEL_LDFLAGS} -lSALOMELocalTrace \
	../MEDMEM_I/libMEDMEMImpl.la ../MEDMEM/libmedmem.la ../INTERP_KERNEL/libinterpkernel.la ../MED/libMEDEngine.la
###  ../MEDMEM_SWIG/_libMEDMEM_Swig.la

dist_salomescript_DATA   = medcorba_test.py batchmode_medcorba_test.py batchmode_medcorba_test1.py

CLEANFILES = libMedCorba_Swig.py libMedCorba_Swig_wrap.cxx
