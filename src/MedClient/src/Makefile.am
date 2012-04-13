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

#  MED MedClient : tool to transfer MED CORBA from server producer of MED object to a client using those MED object
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

salomeinclude_HEADERS = \
	MEDClient.hxx \
	COORDINATEClient.hxx \
	CONNECTIVITYClient.hxx \
	GMESHClient.hxx \
	MESHClient.hxx \
	SUPPORTClient.hxx \
	FAMILYClient.hxx \
	GROUPClient.hxx \
	FIELDClient.hxx \
	FIELDClient.cxx \
	UtilClient.hxx \
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

dist_salomescript_DATA =  \
	medClient_test.py \
	testMeshAlliances.py
nodist_salomescript_DATA = libMEDClient.py

lib_LTLIBRARIES = libMEDClientcmodule.la _libMEDClient.la

dist_libMEDClientcmodule_la_SOURCES = \
	COORDINATEClient.cxx \
	CONNECTIVITYClient.cxx \
	GMESHClient.cxx \
	MESHClient.cxx \
	SUPPORTClient.cxx \
	FAMILYClient.cxx \
	GROUPClient.cxx

dist__libMEDClient_la_SOURCES = libMEDClient.i
nodist__libMEDClient_la_SOURCES = libMEDClient_wrap.cxx

libMEDClientcmodule_la_CPPFLAGS = $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) \
	$(MED3_INCLUDES) $(HDF5_INCLUDES) $(PYTHON_INCLUDES) ${KERNEL_CXXFLAGS} \
	-I$(srcdir)/../../MEDMEM_I -I$(srcdir)/../../MEDMEM -I$(top_builddir)/idl \
	-I$(srcdir)/../../MEDMEM_SWIG \
	-I$(srcdir)/../../MED \
	-I$(srcdir)/../../INTERP_KERNEL \
	-I$(srcdir)/../../INTERP_KERNEL/Bases \
	-I$(srcdir)/../../INTERP_KERNEL/GaussPoints

libMEDClientcmodule_la_LDFLAGS  = ${KERNEL_LDFLAGS} -lOpUtil -lSalomeCommunication \
	../../MEDMEM/libmedmem.la \
	../../MEDMEM_I/libMEDMEMImpl.la \
	../../INTERP_KERNEL/libinterpkernel.la \
	../../MED/libMEDEngine.la

_libMEDClient_la_CPPFLAGS = $(libMEDClientcmodule_la_CPPFLAGS)
_libMEDClient_la_LDFLAGS  = $(PYTHON_LIBS) -module $(libMEDClientcmodule_la_LDFLAGS)
_libMEDClient_la_LIBADD   = libMEDClientcmodule.la

SWIG_FLAGS = @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../../MedCorba_Swig -I$(srcdir)/../../MEDMEM_SWIG
SWIG_DEP   = $(srcdir)/../../MedCorba_Swig/libMedCorba_Swig.i $(srcdir)/../../MEDMEM_SWIG/libMEDMEM_Swig.i

libMEDClient.py: libMEDClient_wrap.cxx

libMEDClient_wrap.cxx: $(SWIG_DEF) $(SWIG_DEP)
	$(SWIG) $(SWIG_FLAGS) -o $@ $(SWIG_DEF)

CLEANFILES = libMEDClient.py libMEDClient_wrap.cxx

OBSOLETE_FILES = \
	FIELDDOUBLEClient.cxx \
	FIELDDOUBLEClient.hxx \
	FIELDINTClient.cxx \
	FIELDINTClient.hxx \
	MakefileForTest \
	MemorySpy.cxx \
	MemorySpy.hxx \
	README_medclient_test \
	TESTMEDCLIENT_Gen.idl \
	TESTMEDCLIENT_Gen_i.cxx \
	TESTMEDCLIENT_Gen_i.hxx \
	create_mesh_c2q4.c \
	create_mesh_c2t3.c \
	create_mesh_c3h8.c \
	create_mesh_c3t4.c \
	test_medclient.py

EXTRA_DIST += $(OBSOLETE_FILES)
