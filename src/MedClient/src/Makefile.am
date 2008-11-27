#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  MED MedClient : tool to transfer MED CORBA from server producer of MED object to a client using those MED object
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

salomeinclude_HEADERS= \
	COORDINATEClient.hxx \
	CONNECTIVITYClient.hxx \
	MESHClient.hxx \
	SUPPORTClient.hxx \
	FAMILYClient.hxx \
	GROUPClient.hxx \
	FIELDClient.hxx \
	FIELDClient.cxx \
	libMEDClient.i

if MED_SWIG_1_3_21
SWIG_DEF = libMEDClient.i dummy.i

dummy_wrap.cxx: libMEDClient_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<FAMILY \* > >/vector<FAMILY \* >/g' libMEDClient_wrap.cxx > toto
	mv toto libMEDClient_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<SUPPORT \* > const >/vector<SUPPORT \* >/g' libMEDClient_wrap.cxx > toto
	mv toto libMEDClient_wrap.cxx
	touch dummy_wrap.cxx
else !MED_SWIG_1_3_21
SWIG_DEF = libMEDClient.i
endif

dist_salomescript_DATA= \
	medClient_test.py \
	testMeshAlliances.py
nodist_salomescript_DATA= libMEDClient.py

lib_LTLIBRARIES= libMEDClientcmodule.la

dist_libMEDClientcmodule_la_SOURCES= \
	COORDINATEClient.cxx \
	CONNECTIVITYClient.cxx \
	MESHClient.cxx \
	SUPPORTClient.cxx \
	FAMILYClient.cxx \
	GROUPClient.cxx

libMEDClientcmodule_la_CPPFLAGS= $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) \
	$(MED2_INCLUDES) $(HDF5_INCLUDES) $(PYTHON_INCLUDES) ${KERNEL_CXXFLAGS} \
	-I$(srcdir)/../../MEDMEM_I -I$(srcdir)/../../MEDMEM -I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl \
	-I$(srcdir)/../../MEDWrapper/V2_1/Core -I$(srcdir)/../../MEDMEM_SWIG -I$(srcdir)/../../MED
libMEDClientcmodule_la_LDFLAGS=${KERNEL_LDFLAGS} -lOpUtil -lSalomeCommunication ../../MedCorba_Swig/libMedCorba_Swigcmodule.la ../../MEDMEM/libmedmem.la

SWIG_FLAGS= @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../../MedCorba_Swig -I$(srcdir)/../../MEDMEM_SWIG
SWIG_DEP=$(srcdir)/../../MedCorba_Swig/libMedCorba_Swig.i $(srcdir)/../../MEDMEM_SWIG/libMEDMEM_Swig.i
nodist_libMEDClientcmodule_la_SOURCES= libMEDClient_wrap.cxx

libMEDClient.py: libMEDClient_wrap.cxx

libMEDClient_wrap.cxx: $(SWIG_DEF) $(SWIG_DEP)
	$(SWIG) $(SWIG_FLAGS) -o $@ $(SWIG_DEF)

CLEANFILES= libMEDClient.py libMEDClient_wrap.cxx

install-exec-hook: $(libdir)/_libMEDClient.so

$(libdir)/_libMEDClient.so:
	( cd $(libdir); ln -sf libMEDClientcmodule.so _libMEDClient.so; )
