#  SALOMELocalTrace : log on local machine
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libMEDMEMTest.la

salomeinclude_HEADERS = MEDMEMTest.hxx

dist_libMEDMEMTest_la_SOURCES= \
	  MEDMEMTest.cxx \
	  MEDMEMTest_Array.cxx \
	  MEDMEMTest_AsciiFieldDriver.cxx \
	  MEDMEMTest_Connectivity.cxx \
	  MEDMEMTest_Coordinate.cxx \
	  MEDMEMTest_DriverFactory.cxx \
	  MEDMEMTest_Exception.cxx \
	  MEDMEMTest_Family.cxx \
	  MEDMEMTest_Field.cxx \
	  MEDMEMTest_Formulae.cxx \
	  MEDMEMTest_GaussLocalization.cxx \
          MEDMEMTest_GibiMeshDriver.cxx \
	  MEDMEMTest_Grid.cxx \
	  MEDMEMTest_Group.cxx \
          MEDMEMTest_Med.cxx \
	  MEDMEMTest_MedFieldDriver21.cxx \
	  MEDMEMTest_MedFieldDriver22.cxx \
	  MEDMEMTest_MedMedDriver21.cxx \
	  MEDMEMTest_MedMedDriver22.cxx \
	  MEDMEMTest_MedMeshDriver21.cxx \
	  MEDMEMTest_MedMeshDriver22.cxx \
	  MEDMEMTest_MedVersion.cxx \
          MEDMEMTest_MeshAndMeshing.cxx \
	  MEDMEMTest_ModulusArray.cxx \
	  MEDMEMTest_nArray.cxx \
          MEDMEMTest_PointerOf.cxx \
	  MEDMEMTest_PolyhedronArray.cxx \
	  MEDMEMTest_PorflowMeshDriver.cxx \
          MEDMEMTest_SkyLineArray.cxx \
          MEDMEMTest_Support.cxx \
          MEDMEMTest_TopLevel.cxx \
	  MEDMEMTest_Unit.cxx \
	  MEDMEMTest_VtkFieldDriver.cxx \
          MEDMEMTest_VtkMedDriver.cxx \
          MEDMEMTest_VtkMeshDriver.cxx

libMEDMEMTest_la_CPPFLAGS= -I/usr/include/cppunit $(KERNEL_CXXFLAGS) $(MED2_INCLUDES) $(HDF5_INCLUDES) \
	-I$(srcdir)/../../MEDMEM -I$(srcdir)/../../MEDWrapper/V2_1/Core
libMEDMEMTest_la_LDFLAGS= -lcppunit

# Executables targets
bin_PROGRAMS = TestMEDMEM \
	MEDMEMTest_Array_fault \
	MEDMEMTest_Connectivity_fault \
	MEDMEMTest_Coordinate_fault \
	MEDMEMTest_Family_fault \
	MEDMEMTest_Field_fault \
	MEDMEMTest_Grid_fault \
	MEDMEMTest_MedFieldDriver21_fault \
	MEDMEMTest_MedMedDriver21_fault \
	MEDMEMTest_MedMedDriver22_fault \
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
dist_MEDMEMTest_MedFieldDriver21_fault_SOURCES= MEDMEMTest_MedFieldDriver21_fault.cxx
dist_MEDMEMTest_MedMedDriver21_fault_SOURCES= MEDMEMTest_MedMedDriver21_fault.cxx
dist_MEDMEMTest_MedMedDriver22_fault_SOURCES= MEDMEMTest_MedMedDriver22_fault.cxx
dist_MEDMEMTest_MeshAndMeshing_fault_SOURCES= MEDMEMTest_MeshAndMeshing_fault.cxx
dist_MEDMEMTest_Support_fault_SOURCES= MEDMEMTest_Support_fault.cxx
dist_MEDMEMTest_VtkFieldDriver_fault_SOURCES= MEDMEMTest_VtkFieldDriver_fault.cxx
dist_MEDMEMTest_VtkMeshDriver_fault_SOURCES= MEDMEMTest_VtkMeshDriver_fault.cxx

LDADD= $(MED2_LIBS) $(libMEDMEMTest_la_LDFLAGS) -lm  ${KERNEL_LDFLAGS}  \
	../../MEDWrapper/V2_1/Core/libmed_V2_1.la ../libmedmem.la libMEDMEMTest.la
AM_CPPFLAGS= $(libMEDMEMTest_la_CPPFLAGS)

salomescript_DATA = testMEDMEM.py
UNIT_TEST_PROG = TestMEDMEM

