# Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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
	ParaMEDSPLITTERTest_medsplitter_para.cxx \
	ParaMEDSPLITTERTest.hxx
endif

salomeinclude_HEADERS = \
	MEDSPLITTERTest.hxx \
	MEDSPLITTERTest_Utils.hxx \
	MEDSPLITTERTest_defs.hxx

libMEDSPLITTERTest_la_CPPFLAGS= $(MPI_INCLUDES) @CPPUNIT_INCLUDES@ $(MED3_INCLUDES) \
	$(HDF5_INCLUDES) $(BOOST_CPPFLAGS) $(LIBXML_CXXFLAGS) \
	-I$(srcdir)/.. -I$(srcdir)/../../MEDWrapper/V2_1/Core -I$(srcdir)/../../MEDMEM \
	-I$(srcdir)/../../MEDMEMCppTest -I$(srcdir)/../../INTERP_KERNEL/Bases \
	-I$(srcdir)/../../INTERP_KERNEL/GaussPoints \
	-I$(srcdir)/../../INTERP_KERNEL
libMEDSPLITTERTest_la_LDFLAGS= $(MED3_LIBS_C_ONLY) @CPPUNIT_LIBS@ ../libmedsplitter.la

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
libMEDSPLITTERTest_la_CXXFLAGS = @CXXTMPDPTHFLAGS@

dist_salomescript_DATA = testMEDSPLITTER.py

# parallel exe
if MPI_IS_OK
if MED_ENABLE_PARMETIS
  PARA_BIN_TEST_PROGS = TestParaMEDSPLITTER
  TestParaMEDSPLITTER_SOURCES= TestParaMEDSPLITTER.cxx
  TestParaMEDSPLITTER_CPPFLAGS = $(TestMEDSPLITTER_CPPFLAGS) $(MPI_INCLUDES) $(CPPUNIT_INCLUDES)
  TestParaMEDSPLITTER_CXXFLAGS = @CXXTMPDPTHFLAGS@
  TestParaMEDSPLITTER_LDADD = $(MED3_LIBS_C_ONLY) $(libMEDSPLITTERTest_la_LDFLAGS) -lm \
	libMEDSPLITTERTest.la ../../MEDWrapper/V2_1/Core/libmed_V2_1.la \
	../../MEDMEM/libmedmem.la ../../INTERP_KERNEL/libinterpkernel.la \
	../libmedsplitter.la $(HDF5_LIBS) $(MPI_LIBS) @CPPUNIT_LIBS@
  dist_salomescript_DATA += testParaMEDSPLITTER.py
#  TESTS = TestParaMEDSPLITTER
  UNIT_TEST_PROG = TestParaMEDSPLITTER
endif
else !MPI_IS_OK
if MED_ENABLE_METIS
# Executables targets

BIN_TEST_PROGRS = TestMEDSPLITTER

TestMEDSPLITTER_SOURCES= TestMEDSPLITTER.cxx

TestMEDSPLITTER_CPPFLAGS= $(libMEDSPLITTERTest_la_CPPFLAGS)
TestMEDSPLITTER_CXXFLAGS = @CXXTMPDPTHFLAGS@
TestMEDSPLITTER_LDADD= $(MED3_LIBS_C_ONLY) $(libMEDSPLITTERTest_la_LDFLAGS) -lm \
	libMEDSPLITTERTest.la ../../MEDWrapper/V2_1/Core/libmed_V2_1.la \
	../../MEDMEM/libmedmem.la ../../INTERP_KERNEL/libinterpkernel.la \
	../libmedsplitter.la
# TESTS = TestMEDSPLITTER
 UNIT_TEST_PROG = TestMEDSPLITTER
if MED_ENABLE_KERNEL
     TestMEDSPLITTER_LDADD+= -lSALOMEBasics
endif
endif
endif

bin_PROGRAMS = $(PARA_BIN_TEST_PROGS) $(BIN_TEST_PROGRS)

check: tests

clean-local:
	rm -f UnitTestsResult*
