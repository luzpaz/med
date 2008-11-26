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
#  File   : Makefile.in
#  Module : MED
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

bin_PROGRAMS = \
	test_MEDMEM_InterpolationFlipBack \
	test_MEDMEM_InterpolationFromMesh_toMesh \
	test_MEDMEM_InterpolationRecopieMaillage \
	test_MEDMEM_InterpolationSansRecopieMaillage \
	test_MEDMEM_InterpolationTimeStep \
	UseCasedTree \
	UseCaseInterpolationts \
	UseCaseInterpolationwots \
	UseCaseMapping \
	UseCaseWrapper_Maillage \
	create_mesh_interpolation

salomeinclude_HEADERS = \
MEDMEM_dTree.hxx\
MEDMEM_dTreeSommet.hxx\
MEDMEM_InterpolationHighLevelObjects.hxx\
MEDMEM_Interpolation.hxx\
MEDMEM_InterpolationTools.hxx\
MEDMEM_Mapping.hxx\
MEDMEM_MappingTools.hxx\
MEDMEM_WrapperCells.hxx\
MEDMEM_WrapperConnectivity.hxx\
MEDMEM_WrapperField.hxx\
MEDMEM_WrapperMesh.hxx\
MEDMEM_WrapperNodes.hxx

dist_test_MEDMEM_InterpolationFlipBack_SOURCES= test_MEDMEM_InterpolationFlipBack.cxx
dist_test_MEDMEM_InterpolationFromMesh_toMesh_SOURCES= test_MEDMEM_InterpolationFromMesh_toMesh.cxx
dist_test_MEDMEM_InterpolationRecopieMaillage_SOURCES= test_MEDMEM_InterpolationRecopieMaillage.cxx
dist_test_MEDMEM_InterpolationSansRecopieMaillage_SOURCES= test_MEDMEM_InterpolationSansRecopieMaillage.cxx
dist_test_MEDMEM_InterpolationTimeStep_SOURCES= test_MEDMEM_InterpolationTimeStep.cxx
dist_UseCasedTree_SOURCES= UseCasedTree.cxx
dist_UseCaseInterpolationts_SOURCES= UseCaseInterpolationts.cxx
dist_UseCaseInterpolationwots_SOURCES= UseCaseInterpolationwots.cxx
dist_UseCaseMapping_SOURCES= UseCaseMapping.cxx
dist_UseCaseWrapper_Maillage_SOURCES= UseCaseWrapper_Maillage.cxx

AM_CPPFLAGS=-U_DEBUG_ $(MED2_INCLUDES) $(HDF5_INCLUDES) -I$(srcdir)/../MEDMEM -I$(srcdir)/../MEDWrapper/V2_1/Core
LDADD=-lm $(MED2_LIBS) $(HDF5_LIBS) ../MEDMEM/libmedmem.la ../MEDWrapper/V2_1/Core/libmed_V2_1.la

AM_CXXFLAGS= @CXXTMPDPTHFLAGS@

if MED_ENABLE_KERNEL
  AM_CPPFLAGS+= ${KERNEL_CXXFLAGS}
  LDADD+= -lSALOMELocalTrace -lSALOMEBasics ${KERNEL_LDFLAGS}
endif
