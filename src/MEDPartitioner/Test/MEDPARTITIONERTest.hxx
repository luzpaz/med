// Copyright (C) 2007-2011  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __MEDPARTITIONERTEST_HXX__
#define __MEDPARTITIONERTEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

#include <set>
#include <string>
#include <iostream>

#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingExtrudedMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"

class MEDPARTITIONERTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( MEDPARTITIONERTest );
  CPPUNIT_TEST( testMeshCollectionSingle );
  CPPUNIT_TEST( testMeshCollectionXml );
  CPPUNIT_TEST( testMeshCollectionSinglePartitionMetis );
  CPPUNIT_TEST( testMeshCollectionComplexPartitionMetis );
  CPPUNIT_TEST( testMetisSmallSize );
  
#if defined(HAVE_MPI2) && defined(MED_ENABLE_PARMETIS)
  //test with mpi on system
  CPPUNIT_TEST( testMpirunSmallSize );
  CPPUNIT_TEST( testMpirunMedianSize );
  CPPUNIT_TEST( testMpirunHugeSize );
#endif

  //CPPUNIT_TEST( deleteTestMeshes );
  CPPUNIT_TEST_SUITE_END();

public:

  //global use
  int _ni;  //nb of hexa9
  int _nj;
  int _nk;
  int _ntot;
  std::string _file_name; //initial test mesh file med CUBE3D
  std::string _file_name_with_faces; //initial test mesh file med CUBE3D plus a set of faces
  std::string _file_name2; //initial test mesh file med CARRE3D
  std::string _file_name_huge_xml;
  int _nb_target_huge;
  std::string _mesh_name; //initial test mesh file med
  int _verbose;
  
  //for utils
  void setSize(int ni, int nj, int nk);
  void setSmallSize();
  void setMedianSize();
  void setbigSize();
  ParaMEDMEM::MEDCouplingUMesh * buildCUBE3DMesh();
  ParaMEDMEM::MEDCouplingUMesh * buildFACE3DMesh();
  ParaMEDMEM::MEDCouplingUMesh * buildCARRE3DMesh();
  ParaMEDMEM::MEDCouplingFieldDouble * buildVecFieldOnCells(std::string myfileName);
  ParaMEDMEM::MEDCouplingFieldDouble * buildVecFieldOnNodes();
  void createTestMeshWithoutField();
  void createTestMeshWithVecFieldOnCells();
  void createTestMeshWithVecFieldOnNodes();
  void verifyTestMeshWithVecFieldOnNodes();
  void verifyMetisMedpartitionerOnSmallSizeForMesh();
  void verifyMetisMedpartitionerOnSmallSizeForFieldOnCells();
  void verifyMetisMedpartitionerOnSmallSizeForFieldOnGaussNe();
  void verifyMedpartitionerOnSmallSizeForMesh();
  void verifyMedpartitionerOnSmallSizeForFieldOnCells();
  void verifyMedpartitionerOnSmallSizeForFieldOnGaussNe();
  void createTestMeshes();
  void createHugeTestMesh(int ni, int nj, int nk, int nbx, int nby, int nbz, int nbTarget);
  void launchMetisMedpartitionerOnTestMeshes();
  void launchMedpartitionerOnTestMeshes();
  void launchMedpartitionerOnHugeTestMeshes();
  void deleteTestMeshes();
  
  //for CPPUNIT_TEST
  void setUp();
  void tearDown();
  void testMeshCollectionSingle();
  void testMeshCollectionXml();
  void testMeshCollectionSinglePartitionMetis();
  void testMeshCollectionComplexPartitionMetis();
  void testMetisSmallSize();
  
#if defined(HAVE_MPI2)
  void testMpirunSmallSize();
  void testMpirunMedianSize();
  void testMpirunHugeSize();
#endif
};

#endif