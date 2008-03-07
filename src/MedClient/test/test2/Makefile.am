#  MED MedClient : tool to transfer MED CORBA from server producer of MED object to a client using those MED object
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
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

dist_salomescript_DATA= \
	TestMedCorba6.py \
	TestMedCorba7.py \
	TestMedCorba8.py
nodist_salomescript_DATA= \
	libCompo2.py

# Libraries targets
lib_LTLIBRARIES= libCompo2cmodule.la
dist_libCompo2cmodule_la_SOURCES= Compo2.cxx
nodist_libCompo2cmodule_la_SOURCES= libCompo2_wrap.cxx

EXTRA_DIST+= Compo2.hxx

libCompo2cmodule_la_CPPFLAGS= $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) \
	${MED2_INCLUDES} ${HDF5_INCLUDES} ${PYTHON_INCLUDES} ${KERNEL_CXXFLAGS} \
	-I$(srcdir)/../../../MEDMEM_I -I$(srcdir)/../../../MEDMEM -I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl \
	-I$(srcdir)/../../../MEDWrapper/V2_1/Core -I$(srcdir)/../../../MEDMEM_SWIG -I$(srcdir)/../../../MED -I$(srcdir)/../../src
libCompo2cmodule_la_LDFLAGS= ${KERNEL_LDFLAGS}  ${MED2_LIBS} ${HDF5_LIBS} \
	../../src/libMEDClientcmodule.la ../../../MEDMEM_SWIG/libMEDMEM_Swigcmodule.la \
	../../../MEDMEM/libmedmem.la ../../../MEDMEM_I/libMEDMEMImpl.la

if MED_SWIG_1_3_21
SWIG_DEF = libCompo2.i dummy.i

dummy_wrap.cxx: libCompo2_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<FAMILY \* > >/vector<FAMILY \* >/g' libCompo2_wrap.cxx > toto
	mv toto libCompo2_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<SUPPORT \* > const >/vector<SUPPORT \* >/g' libCompo2_wrap.cxx > toto
	mv toto libCompo2_wrap.cxx
	touch dummy_wrap.cxx
else !MED_SWIG_1_3_21
SWIG_DEF = libCompo2.i
endif

SWIG_FLAGS= @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../../src -I$(srcdir)/../../../MEDMEM_SWIG -I$(srcdir)/../../../MedCorba_Swig
SWIG_DEP=../../src/libMEDClient.i ../../../MEDMEM_SWIG/libMEDMEM_Swig.i ../../../MedCorba_Swig/libMedCorba_Swig.i

libCompo2.py: libCompo2_wrap.cxx

libCompo2_wrap.cxx: $(SWIG_DEF)
	$(SWIG) $(SWIG_FLAGS) -o $@ $<

CLEANFILES= libCompo2_wrap.cxx libCompo2.py

install-exec-hook: $(libdir)/_libCompo2.so

$(libdir)/_libCompo2.so:
	( cd $(libdir); ln -sf libCompo2cmodule.so _libCompo2.so; )
