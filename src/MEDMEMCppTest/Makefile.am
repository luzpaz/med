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

#  SALOMELocalTrace : log on local machine
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libMEDMEMCppTest.la

salomeinclude_HEADERS = MEDMEMTest.hxx MEDMEMCppTestExport.hxx

EXTRA_DIST += BasicMainTest.hxx

dist_libMEDMEMCppTest_la_SOURCES= \
	MEDMEMTest.cxx \
	MEDMEMTest_Array.cxx \
	MEDMEMTest_AsciiFieldDriver.cxx \
	MEDMEMTest_Connectivity.cxx \
	MEDMEMTest_Coordinate.cxx \
	MEDMEMTest_DesactivateFacesComputation.cxx \
	MEDMEMTest_GetVolumeAbs.cxx \
	MEDMEMTest_DriverFactory.cxx \
	MEDMEMTest_Exception.cxx \
	MEDMEMTest_Extractor.cxx \
	MEDMEMTest_Family.cxx \
	MEDMEMTest_Field.cxx \
	MEDMEMTest_Formulae.cxx \
	MEDMEMTest_GaussLocalization.cxx \
	MEDMEMTest_GibiMeshDriver.cxx \
	MEDMEMTest_Grid.cxx \
	MEDMEMTest_Group.cxx \
	MEDMEMTest_MedFileBrowser.cxx \
	MEDMEMTest_MedFieldDriver.cxx \
	MEDMEMTest_MedMeshDriver.cxx \
	MEDMEMTest_MedVersion.cxx \
	MEDMEMTest_MeshAndMeshing.cxx \
	MEDMEMTest_ModulusArray.cxx \
	MEDMEMTest_PartialDescendingConnectivity.cxx \
	MEDMEMTest_PointerOf.cxx \
	MEDMEMTest_PorflowMeshDriver.cxx \
	MEDMEMTest_Remapper.cxx \
	MEDMEMTest_SkyLineArray.cxx \
	MEDMEMTest_Support.cxx \
	MEDMEMTest_TopLevel.cxx \
	MEDMEMTest_Unit.cxx \
	MEDMEMTest_VtkFieldDriver.cxx \
	MEDMEMTest_VtkMedDriver.cxx \
	MEDMEMTest_VtkMeshDriver.cxx \
	MEDMEMTest_nArray.cxx \
	MEDMEMTest_Utils.hxx \
	MEDMEMTest_Utils.cxx \
	MEDMEMTest_MeshFuse.cxx

libMEDMEMCppTest_la_CPPFLAGS= @CPPUNIT_INCLUDES@ $(MED3_INCLUDES) $(HDF5_INCLUDES) \
	-I$(srcdir)/../MEDMEM \
	-I$(srcdir)/../INTERP_KERNEL \
	-I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../INTERP_KERNEL/GaussPoints \
	-I$(srcdir)/../INTERP_KERNEL/Geometric2D

libMEDMEMCppTest_la_LDFLAGS= $(MED3_LIBS_C_ONLY) @CPPUNIT_LIBS@ \
	../MEDMEM/libmedmem.la

if MED_ENABLE_KERNEL
    libMEDMEMCppTest_la_CPPFLAGS+= ${KERNEL_CXXFLAGS}
    libMEDMEMCppTest_la_LDFLAGS+= ${KERNEL_LDFLAGS} -lSALOMELocalTrace
endif

# Executables targets
bin_PROGRAMS = TestMEDMEM \
	MEDMEMTest_Array_fault \
	MEDMEMTest_Connectivity_fault \
	MEDMEMTest_Coordinate_fault \
	MEDMEMTest_Family_fault \
	MEDMEMTest_Field_fault \
	MEDMEMTest_Grid_fault \
	MEDMEMTest_MeshAndMeshing_fault \
	MEDMEMTest_Support_fault \
	MEDMEMTest_VtkFieldDriver_fault \
	MEDMEMTest_VtkMeshDriver_fault

dist_TestMEDMEM_SOURCES= TestMEDMEM.cxx
dist_MEDMEMTest_Array_fault_SOURCES= MEDMEMTest_Array_fault.cxx
dist_MEDMEMTest_Connectivity_fault_SOURCES= MEDMEMTest_Connectivity_fault.cxx
dist_MEDMEMTest_Coordinate_fault_SOURCES= MEDMEMTest_Coordinate_fault.cxx
dist_MEDMEMTest_Family_fault_SOURCES= MEDMEMTest_Family_fault.cxx
dist_MEDMEMTest_Field_fault_SOURCES= MEDMEMTest_Field_fault.cxx
dist_MEDMEMTest_Grid_fault_SOURCES= MEDMEMTest_Grid_fault.cxx
dist_MEDMEMTest_MeshAndMeshing_fault_SOURCES= MEDMEMTest_MeshAndMeshing_fault.cxx
dist_MEDMEMTest_Support_fault_SOURCES= MEDMEMTest_Support_fault.cxx
dist_MEDMEMTest_VtkFieldDriver_fault_SOURCES= MEDMEMTest_VtkFieldDriver_fault.cxx
dist_MEDMEMTest_VtkMeshDriver_fault_SOURCES= MEDMEMTest_VtkMeshDriver_fault.cxx

LDADD= $(MED3_LIBS_C_ONLY) $(libMEDMEMCppTest_la_LDFLAGS) -lm \
	 ../MEDMEM/libmedmem.la \
	../INTERP_KERNEL/libinterpkernel.la libMEDMEMCppTest.la
if MED_ENABLE_KERNEL
     LDADD+= -lSALOMEBasics
endif
AM_CPPFLAGS += $(libMEDMEMCppTest_la_CPPFLAGS)

dist_salomescript_DATA = testMEDMEM.py

UNIT_TEST_PROG = TestMEDMEM

OBSOLETE_FILES = \
	TestOpenMED.cxx

EXTRA_DIST += $(OBSOLETE_FILES)

check: tests

CLEANFILES =            \
        UnitTestsResult

clean-local:
	rm -rf *.med


