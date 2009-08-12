#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#  SALOMELocalTrace : log on local machine
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libMEDSPLITTERTest.la

dist_libMEDSPLITTERTest_la_SOURCES= \
	MEDSPLITTERTest.cxx \
	MEDSPLITTERTest_MESHCollection.cxx \
	MEDSPLITTERTest_ParallelTopology.cxx \
	MEDSPLITTERTest_Utils.cxx

if MED_ENABLE_PARMETIS
  dist_libMEDSPLITTERTest_la_SOURCES+= \
	ParaMEDSPLITTERTest_MeshSendReceive.cxx \
	ParaMEDSPLITTERTest_ParaDomainSelector.cxx \
	ParaMEDSPLITTERTest_medsplitter_para.cxx	
endif

salomeinclude_HEADERS = \
	MEDSPLITTERTest.hxx \
	MEDSPLITTERTest_Utils.cxx

libMEDSPLITTERTest_la_CPPFLAGS= @CPPUNIT_INCLUDES@ $(MED2_INCLUDES) \
	$(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ $(BOOST_CPPFLAGS) $(LIBXML_CXXFLAGS) \
	-I$(srcdir)/.. -I$(srcdir)/../../MEDWrapper/V2_1/Core -I$(srcdir)/../../MEDMEM \
	-I$(srcdir)/../../MEDMEMCppTest
libMEDSPLITTERTest_la_LDFLAGS= $(MED2_LIBS) @CPPUNIT_LIBS@

if MED_ENABLE_KERNEL
    libMEDSPLITTERTest_la_CPPFLAGS+= ${KERNEL_CXXFLAGS}
    libMEDSPLITTERTest_la_LDFLAGS+= ${KERNEL_LDFLAGS} -lSALOMELocalTrace 
endif

if MED_ENABLE_PARMETIS
  libMEDSPLITTERTest_la_CPPFLAGS+= $(PARMETIS_CPPFLAGS)
  libMEDSPLITTERTest_la_LDFLAGS+= $(PARMETIS_LIBS)
endif
if MED_ENABLE_METIS
  libMEDSPLITTERTest_la_CPPFLAGS+= $(METIS_CPPFLAGS)
  libMEDSPLITTERTest_la_LDFLAGS+= $(METIS_LIBS)
endif
if MED_ENABLE_SCOTCH
  libMEDSPLITTERTest_la_CPPFLAGS+= $(SCOTCH_CPPFLAGS)
  libMEDSPLITTERTest_la_LDFLAGS+= $(SCOTCH_LIBS)
endif

# Executables targets
bin_PROGRAMS= TestMEDSPLITTER

TestMEDSPLITTER_SOURCES= TestMEDSPLITTER.cxx

TestMEDSPLITTER_CPPFLAGS= $(libMEDSPLITTERTest_la_CPPFLAGS) 
TestMEDSPLITTER_LDADD= $(MED2_LIBS) $(libMEDSPLITTERTest_la_LDFLAGS) -lm \
	libMEDSPLITTERTest.la ../../MEDWrapper/V2_1/Core/libmed_V2_1.la \
	../../MEDMEM/libmedmem.la ../libmedsplitter.la
if MED_ENABLE_KERNEL
     TestMEDSPLITTER_LDADD+= -lSALOMEBasics
endif

dist_salomescript_DATA = testMEDSPLITTER.py

UNIT_TEST_PROG = TestMEDSPLITTER

# parallel exe

if MED_ENABLE_PARMETIS
  bin_PROGRAMS+= TestParaMEDSPLITTER
  TestParaMEDSPLITTER_SOURCES= TestParaMEDSPLITTER.cxx
  TestParaMEDSPLITTER_CPPFLAGS = $(TestMEDSPLITTER_CPPFLAGS)
  TestParaMEDSPLITTER_LDADD = $(TestMEDSPLITTER_LDADD) $(HDF5_LIBS)
  dist_salomescript_DATA += testParaMEDSPLITTER.py
  TESTS = TestParaMEDSPLITTER
endif

check: tests

CLEANFILES =            \
        UnitTestsResult
