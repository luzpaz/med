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

#  MED MEDMEM : MED files in memory
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# first '.' says that this folder must be compiled before Test (MEDMEM/Test uses MEDMEM)
#if CPPUNIT_IS_OK
# SUBDIRS=. Test
#endif

#DIST_SUBDIRS= Test

lib_LTLIBRARIES= libmedmem.la

salomeinclude_HEADERS= \
MEDMEM_PointerOf.hxx \
MEDMEM_CellModel.hxx \
MEDMEM_Connectivity.hxx \
MEDMEM_Coordinate.hxx \
MEDMEM_define.hxx \
MEDMEM_DriversDef.hxx \
MEDMEM_DriverFactory.hxx \
MEDMEM_DriverFactory.ixx \
MEDMEM_Family.hxx \
MEDMEM_Field.hxx \
MEDMEM_GenDriver.hxx \
MEDMEM_Group.hxx \
MEDMEM_Array.hxx \
MEDMEM_Exception.hxx \
MEDMEM_MedFieldDriver.hxx \
MEDMEM_MedFieldDriver.txx \
MEDMEM_AsciiFieldDriver.hxx \
MEDMEM_MedMeshDriver.hxx \
MEDMEM_GibiMeshDriver.hxx \
MEDMEM_PorflowMeshDriver.hxx \
MEDMEM_DriverTools.hxx \
MEDMEM_ModulusArray.hxx \
MEDMEM_SkyLineArray.hxx \
MEDMEM_VtkMedDriver.hxx \
MEDMEM_VtkMeshDriver.hxx \
MEDMEM_VtkFieldDriver.hxx \
MEDMEM_Mesh.hxx \
MEDMEM_Meshing.hxx \
MEDMEM_STRING.hxx \
MEDMEM_Support.hxx \
MEDMEM_Unit.hxx \
MEDMEM_Grid.hxx \
MEDMEM_RCBase.hxx \
MEDMEM_MedVersion.hxx \
MEDMEM_TopLevel.hxx \
MEDMEM_Utilities.hxx \
MEDMEM_ArrayInterface.hxx \
MEDMEM_IndexCheckingPolicy.hxx \
MEDMEM_InterlacingPolicy.hxx \
MEDMEM_InterlacingTraits.hxx \
MEDMEM_nArray.hxx \
MEDMEM_Tags.hxx \
MEDMEM_FieldConvert.hxx \
MEDMEM_ArrayConvert.hxx \
MEDMEM_FieldForward.hxx \
MEDMEM_GaussLocalization.hxx \
MEDMEM_SetInterlacingType.hxx\
MEDMEM_ConnectZone.hxx \
MEDMEM_EnsightMedDriver.hxx \
MEDMEM_EnsightMeshDriver.hxx \
MEDMEM_EnsightFieldDriver.hxx \
MEDMEM.hxx \
MEDMEM_OptionManager.hxx \
MEDMEM_EnsightUtils.hxx \
MEDMEM_Extractor.hxx \
MEDMEM_Remapper.hxx \
PointLocator.hxx \
MEDNormalizedUnstructuredMesh.hxx \
MEDNormalizedUnstructuredMesh.txx \
MEDMEM_GMesh.hxx \
MEDMEM_MeshFuse.hxx \
MEDMEM_MedFileBrowser.hxx

dist_libmedmem_la_SOURCES= \
MEDMEM_Init.cxx \
MEDMEM_RCBase.cxx \
MEDMEM_CellModel.cxx \
MEDMEM_Connectivity.cxx \
MEDMEM_Coordinate.cxx \
MEDMEM_DriversDef.cxx \
MEDMEM_DriverFactory.cxx \
MEDMEM_Family.cxx \
MEDMEM_Field.cxx \
MEDMEM_GenDriver.cxx \
MEDMEM_Group.cxx \
MEDMEM_Exception.cxx \
MEDMEM_MedMeshDriver.cxx \
MEDMEM_GibiMeshDriver.cxx \
MEDMEM_PorflowMeshDriver.cxx \
MEDMEM_SkyLineArray.cxx \
MEDMEM_Mesh.cxx \
MEDMEM_Meshing.cxx \
MEDMEM_Support.cxx \
MEDMEM_Unit.cxx \
MEDMEM_VtkMedDriver.cxx \
MEDMEM_VtkMeshDriver.cxx \
MEDMEM_Grid.cxx \
MEDMEM_MedVersion.cxx \
MEDMEM_TopLevel.cxx \
MEDMEM_ConnectZone.cxx \
MEDMEM_EnsightMedDriver.cxx \
MEDMEM_EnsightFieldDriver.cxx \
MEDMEM_EnsightUtils.cxx \
MEDMEM_EnsightMeshDriver.cxx \
MEDMEM_DriverTools.cxx \
MEDMEM_GaussLocalization.cxx \
MEDMEM_Extractor.cxx \
PointLocator.cxx \
MEDMEM_Remapper.cxx \
MEDMEM_GMesh.cxx \
MEDMEM_MeshFuse.cxx \
MEDMEM_MedFileBrowser.cxx

#libmedmem_la_LDFLAGS= -L$(top_builddir)/lib@LIB_LOCATION_SUFFIX@/salome 
# libmedmem_la_CPPFLAGS= $(MED3_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@
libmedmem_la_CPPFLAGS= $(MED3_INCLUDES) $(HDF5_INCLUDES) $(XDR_CPPFLAGS) \
	-I$(srcdir)/../INTERP_KERNEL \
	-I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../INTERP_KERNEL/Geometric2D \
	-I$(srcdir)/../INTERP_KERNEL/GaussPoints
libmedmem_la_CXXFLAGS= @CXXTMPDPTHFLAGS@

# change motivated by the bug KERNEL4778.
libmedmem_la_LDFLAGS=$(MED3_LIBS_C_ONLY) $(HDF5_LIBS) $(XDR_LIBS) \
	$(STDLIB) ../INTERP_KERNEL/libinterpkernel.la

AM_CPPFLAGS+= $(libmedmem_la_CPPFLAGS)
AM_CXXFLAGS+= $(libmedmem_la_CXXFLAGS)
LDADD= $(libmedmem_la_LDFLAGS) -lm libmedmem.la

OBSOLETE_FILES = \
	MEDMEM_MEDMEMchampLire.hxx \
	MEDMEM_MEDMEMchampLire.cxx \
	MEDMEM_MEDMEMgaussEcr.hxx \
	MEDMEM_MEDMEMgaussEcr.cxx \
	MEDMEM_MEDMEMprofilEcr.hxx \
	MEDMEM_MEDMEMprofilEcr.cxx \
	MEDMEM_TypeMeshDriver.cxx \
	MEDMEM_TypeMeshDriver.hxx \
	checkScript \
	DataTest

EXTRA_DIST += $(OBSOLETE_FILES)
