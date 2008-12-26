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
#  MED MEDMEM : MED files in memory
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# first '.' says that this folder must be compiled before Test (MEDMEM/Test uses MEDMEM)
#if CPPUNIT_IS_OK
# SUBDIRS=. Test
#endif

#DIST_SUBDIRS= Test

#lib_LTLIBRARIES= libMEDMEMBinTest.la


#libmedmem_la_LDFLAGS= -L$(top_builddir)/lib@LIB_LOCATION_SUFFIX@/salome 
#libMEDMEMBinTest_la_CPPFLAGS= $(MED2_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ \
#	-I$(srcdir)/../MEDWrapper/Base \
#	-I$(srcdir)/../MEDWrapper/V2_1/Core -I$(srcdir)/../INTERPOLATION

# change motivated by the bug KERNEL4778.
#libMEDMEMBinTest_la_LDFLAGS=$(MED2_LIBS) $(HDF5_LIBS) ../MEDWrapper/V2_1/Core/libmed_V2_1.la \
#	../MEDWrapper/Base/libMEDWrapperBase.la $(STDLIB)
#if MED_ENABLE_KERNEL
#    libMEDMEMBinTest_la_CPPFLAGS+= ${KERNEL_CXXFLAGS}
#    libMEDMEMBinTest_la_LDFLAGS+= ${KERNEL_LDFLAGS} -lSALOMELocalTrace 
#endif

AM_CPPFLAGS= $(MED2_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ \
	-I$(srcdir)/../MEDWrapper/Base \
	-I$(srcdir)/../MEDWrapper/V2_1/Core -I$(srcdir)/../INTERPOLATION \
	-I$(srcdir)/../MEDMEM

LDADD= $(MED2_LIBS) $(HDF5_LIBS) ../MEDWrapper/V2_1/Core/libmed_V2_1.la \
	../MEDWrapper/Base/libMEDWrapperBase.la $(STDLIB) ../MEDMEM/libmedmem.la

if MED_ENABLE_KERNEL
    AM_CPPFLAGS+= ${KERNEL_CXXFLAGS}
    LDADD+= ${KERNEL_LDFLAGS} -lSALOMELocalTrace -lSALOMEBasics
endif


bin_PROGRAMS= \
	ensight2med med2ensight \
	med2_1_To_med2_2 med2vtk duplicateMED med_test duplicateMEDMESH test_write_groups_poly\
	test_MEDMEM_ModulusArray test_MEDMEM_Array test_MEDMEM_SkyLineArray \
	test_MEDMEM_CellModel testUPointerOf testUCoordinate testUUnit \
	testUGeoNameMeshEntities testUMedException testUModulusArray \
	testUSkyLineArray testUArray testUCellModel readEntete readCoordinate \
	test_copie_field_ test_copie_fieldT test_copie_coordinate \
	test_copie_medarray test_copie_connectivity test_copie_support \
	test_copie_family test_copie_group test_copie_mesh test_affect_medarray \
	test_MEDMEM_Meshing test_MEDMEM_MeshingPoly test_operation_fielddouble \
	test_operation_fieldint test_gibi_driver test_porflow_driver \
	test_grid test_MEDMEM_PolyhedronArray test_MEDMEM_PolyConnectivity \
	test_MEDMEM_PolyDriverMedMeshRead test_MEDMEM_PolyDriverMedMeshWrite \
	test_MEDMEM_poly3D test_MEDMEM_nArray test_MEDMEM_Meshing_poly test_profil_MedFieldDriver \
	test_profil_gauss_MedFieldDriver test_GaussLocalization testAnalFile test_MEDMEM_MeshingFlica testConvertPolygon \
	create_mesh create_mesh_c2q4s2 create_mesh_c3h8q4 create_mesh_c3h8q4_wrong \
	create_mesh_c2q4s2_wrong create_grid create_mesh_c2q4 create_mesh_c3h8 create_poly3D

#AM_CPPFLAGS= $(libMEDMEMBinTest_la_CPPFLAGS)
#LDADD= $(libMEDMEMBinTest_la_LDFLAGS) -lm libMEDMEMBinTest.la

TESTS = $(bin_PROGRAMS)

#if MED_ENABLE_KERNEL
#     LDADD+=-lSALOMEBasics
#endif

dist_ensight2med_SOURCES= ensight2med.cxx
dist_med2ensight_SOURCES= med2ensight.cxx
dist_med2_1_To_med2_2_SOURCES= med2_1_To_med2_2.cxx
dist_med2vtk_SOURCES= med2vtk.cxx
dist_duplicateMED_SOURCES= duplicateMED.cxx
dist_med_test_SOURCES= med_test.cxx
dist_duplicateMEDMESH_SOURCES= duplicateMEDMESH.cxx
dist_test_write_groups_poly_SOURCES= test_write_groups_poly.cxx
dist_test_MEDMEM_ModulusArray_SOURCES= test_MEDMEM_ModulusArray.cxx
dist_test_MEDMEM_Array_SOURCES= test_MEDMEM_Array.cxx
dist_test_MEDMEM_SkyLineArray_SOURCES= test_MEDMEM_SkyLineArray.cxx
dist_test_MEDMEM_CellModel_SOURCES= test_MEDMEM_CellModel.cxx
dist_testUPointerOf_SOURCES= tests/testUPointerOf.cxx
dist_testUCoordinate_SOURCES= tests/testUCoordinate.cxx
dist_testUUnit_SOURCES= tests/testUUnit.cxx
dist_testUGeoNameMeshEntities_SOURCES= tests/testUGeoNameMeshEntities.cxx
dist_testUMedException_SOURCES= tests/testUMedException.cxx
dist_testUModulusArray_SOURCES= tests/testUModulusArray.cxx
dist_testUSkyLineArray_SOURCES= tests/testUSkyLineArray.cxx
dist_testUArray_SOURCES= tests/testUArray.cxx
dist_testUCellModel_SOURCES= tests/testUCellModel.cxx
dist_readEntete_SOURCES= tests/readEntete.cxx
dist_readCoordinate_SOURCES= tests/readCoordinate.cxx
dist_test_copie_field__SOURCES= test_copie_field_.cxx
dist_test_copie_fieldT_SOURCES= test_copie_fieldT.cxx
dist_test_copie_coordinate_SOURCES= test_copie_coordinate.cxx
dist_test_copie_medarray_SOURCES= test_copie_medarray.cxx
dist_test_copie_connectivity_SOURCES= test_copie_connectivity.cxx
dist_test_copie_support_SOURCES= test_copie_support.cxx
dist_test_copie_family_SOURCES= test_copie_family.cxx
dist_test_copie_group_SOURCES= test_copie_group.cxx
dist_test_copie_mesh_SOURCES= test_copie_mesh.cxx
dist_test_affect_medarray_SOURCES= test_affect_medarray.cxx
dist_test_MEDMEM_Meshing_SOURCES= test_MEDMEM_Meshing.cxx
dist_test_MEDMEM_MeshingPoly_SOURCES= test_MEDMEM_MeshingPoly.cxx
dist_test_operation_fielddouble_SOURCES= test_operation_fielddouble.cxx
dist_test_operation_fieldint_SOURCES= test_operation_fieldint.cxx
dist_test_gibi_driver_SOURCES= test_gibi_driver.cxx
dist_test_porflow_driver_SOURCES= test_porflow_driver.cxx
dist_test_grid_SOURCES= test_grid.cxx
dist_test_MEDMEM_PolyhedronArray_SOURCES= test_MEDMEM_PolyhedronArray.cxx
dist_test_MEDMEM_PolyConnectivity_SOURCES= test_MEDMEM_PolyConnectivity.cxx
dist_test_MEDMEM_PolyDriverMedMeshRead_SOURCES= test_MEDMEM_PolyDriverMedMeshRead.cxx
dist_test_MEDMEM_PolyDriverMedMeshWrite_SOURCES= test_MEDMEM_PolyDriverMedMeshWrite.cxx
dist_test_MEDMEM_poly3D_SOURCES= test_MEDMEM_poly3D.cxx
dist_test_MEDMEM_nArray_SOURCES= test_MEDMEM_nArray.cxx
dist_test_MEDMEM_Meshing_poly_SOURCES= test_MEDMEM_Meshing_poly.cxx
dist_test_profil_MedFieldDriver_SOURCES= test_profil_MedFieldDriver.cxx
dist_test_profil_gauss_MedFieldDriver_SOURCES= test_profil_gauss_MedFieldDriver.cxx
dist_test_GaussLocalization_SOURCES= test_GaussLocalization.cxx
dist_testAnalFile_SOURCES= testAnalFile.cxx
dist_test_MEDMEM_MeshingFlica_SOURCES= test_MEDMEM_MeshingFlica.cxx
dist_testConvertPolygon_SOURCES=testConvertPolygon.cxx

OBSOLETE_FILES = \
	create_poly2D.c \
	testAG.cxx

EXTRA_DIST += $(OBSOLETE_FILES)
