// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEMTest.hxx"

#include "MEDMEM_Remapper.hxx"
#include "MEDMEM_Meshing.hxx"

#include "MEDNormalizedUnstructuredMesh.txx"
#include "Interpolation3D.txx"

#include <cppunit/TestAssert.h>
#include <iostream>
//#include <vector>

// namespace MEDMEMTest
// {
void MEDMEMTest::test_RemapperP0P0() 
{
  std::string sourcename=getResourceFile("square1.med");
  MEDMEM::MESH *source_mesh=new MEDMEM::MESH (MED_DRIVER,sourcename,"Mesh_2");

  std::string targetname=getResourceFile("square2.med");
  MEDMEM::MESH *target_mesh=new MEDMEM::MESH (MED_DRIVER,targetname,"Mesh_3");

  int nbcomp=3;

  const MEDMEM::SUPPORT *source_support=source_mesh->getSupportOnAll( MED_EN::MED_CELL );
  MEDMEM::FIELD<double> *source_field=new MEDMEM::FIELD<double>(source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field->getValue());
  for (int i=0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;

  const MEDMEM::SUPPORT *target_support=target_mesh->getSupportOnAll( MED_EN::MED_CELL );
  MEDMEM::FIELD<double> *target_field=new MEDMEM::FIELD<double>(target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field->getValue());
  for (int i=0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=0.0;


  MEDMEM_REMAPPER remapper;
  const std::string intersectiontype = "IntersectionType";
  std::string convex = "Convex";
  remapper.setOptionDouble("Precision",1.e-8);
  remapper.setOptionString(intersectiontype,convex);
  remapper.setOptionInt("PrintLevel",1);
  remapper.prepare(*source_mesh,*target_mesh,"P0P0");
  remapper.transfer(*source_field,*target_field);

  //MN: Old tests
  MEDMEM::FIELD<double> *source_areas=source_mesh->getArea(source_support);
  MEDMEM::FIELD<double> *target_areas=target_mesh->getArea(target_support);
  //MEDMEMTest::absField(*source_areas); //absolute value
  //MEDMEMTest::absField(*target_areas); //absolute value

  //target square is in reverse order as compared to initial square
  double source_integral=source_field->normL2(nbcomp,source_areas);
  double target_integral=target_field->normL2(nbcomp,target_areas);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(source_integral,target_integral,1e-10);

  //MN: Transfer test
  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);

  //MN: Reverse transfer test
  remapper.reverseTransfer(*source_field,*target_field);

  max = -2;
  min =  2;
  for(int i = 0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);

  //MN: Hxx2salome transfer test
  MEDMEM::FIELD<double> *newTargetField =remapper.transferField(*source_field);
  MEDMEM::FIELD<double> *newSourceField =remapper.reverseTransferField(*target_field);
  sourcevalue=const_cast<double*>((*newSourceField).getValue());
  targetvalue=const_cast<double*>((*newTargetField).getValue());
  max = -2;
  min =  2;
  for(int i = 0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
  for(int i = 0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);
  source_field->removeReference();
  newSourceField->removeReference();
  newTargetField->removeReference();
  target_mesh->removeReference();
  source_areas->removeReference();
  target_areas->removeReference();
  target_field->removeReference();
  source_mesh->removeReference();
}

void  MEDMEMTest::test_RemapperP1P1() 
{
  std::string sourcename=getResourceFile("square1.med");
  MEDMEM::MESH *source_mesh=new MEDMEM::MESH (MED_DRIVER,sourcename,"Mesh_2");

  std::string targetname=getResourceFile("square2.med");
  MEDMEM::MESH *target_mesh=new MEDMEM::MESH (MED_DRIVER,targetname,"Mesh_3");

  int nbcomp=2;

  const MEDMEM::SUPPORT *source_support=source_mesh->getSupportOnAll( MED_EN::MED_NODE );
  MEDMEM::FIELD<double> *source_field=new MEDMEM::FIELD<double>(source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field->getValue());
  for (int i=0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;

  const MEDMEM::SUPPORT *target_support=target_mesh->getSupportOnAll( MED_EN::MED_NODE );
  MEDMEM::FIELD<double> *target_field=new MEDMEM::FIELD<double>(target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field->getValue());
  for (int i=0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=-0.0001;


  MEDMEM_REMAPPER remapper;
  remapper.prepare(*source_mesh,*target_mesh,"P1P1");
  remapper.transfer(*source_field,*target_field);

  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);

  remapper.reverseTransfer(*source_field,*target_field);

  max = -2;
  min =  2;
  for(int i = 0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);
  source_field->removeReference();
  source_mesh->removeReference();
  target_field->removeReference();
  target_mesh->removeReference();
}


void  MEDMEMTest::test_RemapperP1P0() 
{
  std::string sourcename=getResourceFile("square1.med");
  MEDMEM::MESH *source_mesh=new MEDMEM::MESH (MED_DRIVER,sourcename,"Mesh_2");

  std::string targetname=getResourceFile("square2.med");
  MEDMEM::MESH *target_mesh=new MEDMEM::MESH (MED_DRIVER,targetname,"Mesh_3");

  int nbcomp=3;
  const MEDMEM::SUPPORT *source_support=source_mesh->getSupportOnAll( MED_EN::MED_NODE );
  MEDMEM::FIELD<double> *source_field=new MEDMEM::FIELD<double>(source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field->getValue());
  for (int i=0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;

  const MEDMEM::SUPPORT *target_support=target_mesh->getSupportOnAll( MED_EN::MED_CELL );
  MEDMEM::FIELD<double> *target_field=new MEDMEM::FIELD<double>(target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field->getValue());
  for (int i=0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=0.0;


  MEDMEM_REMAPPER remapper;
  remapper.prepare(*source_mesh,*target_mesh,"P1P0");
  target_mesh->removeReference();
  source_mesh->removeReference();
  remapper.transfer(*source_field,*target_field);

  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);

  remapper.reverseTransfer(*source_field,*target_field);
  max = -2;
  min =  2;
  for(int i = 0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);
  source_field->removeReference();
  target_field->removeReference();
}

void  MEDMEMTest::test_RemapperP0P1() 
{
  std::string sourcename=getResourceFile("square1.med");
  MEDMEM::MESH *source_mesh=new MEDMEM::MESH (MED_DRIVER,sourcename,"Mesh_2");

  std::string targetname=getResourceFile("square2.med");
  MEDMEM::MESH *target_mesh=new MEDMEM::MESH (MED_DRIVER,targetname,"Mesh_3");

  int nbcomp=4;
  const MEDMEM::SUPPORT *source_support=source_mesh->getSupportOnAll( MED_EN::MED_CELL );
  MEDMEM::FIELD<double> *source_field=new MEDMEM::FIELD<double>(source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field->getValue());
  for (int i=0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;

  const MEDMEM::SUPPORT *target_support=target_mesh->getSupportOnAll( MED_EN::MED_NODE );
  MEDMEM::FIELD<double> *target_field=new MEDMEM::FIELD<double>(target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field->getValue());
  for (int i=0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=0.0;


  MEDMEM_REMAPPER remapper;
  remapper.prepare(*source_mesh,*target_mesh,"P0P1");
  remapper.transfer(*source_field,*target_field);

  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);

  remapper.reverseTransfer(*source_field,*target_field);

  max = -2;
  min =  2;
  for(int i = 0; i<source_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }

  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);
  source_field->removeReference();
  target_field->removeReference();
  target_mesh->removeReference();
  source_mesh->removeReference();
}

namespace
{
  MESH * build3DSourceMesh1()
  {
    const double coords[84]=
      {
        100.0, 100.0, 0.0, 100.0, 100.0, 100.0, 100.0, 0.0, 100.0, 100.0, 0.0, 0.0, 0.0, 100.0, 0.0,
        0.0, 100.0, 100.0, 0.0, 0.0, 100.0, 0.0, 0.0, 0.0, 100.0, 100.0, 200.0, 100.0, 0.0, 200.0,
        0.0, 100.0, 200.0, 0.0, 0.0, 200.0, 100.0, 200.0, 0.0, 100.0, 200.0, 100.0, 0.0, 200.0, 0.0,
        0.0, 200.0, 100.0, 100.0, 200.0, 200.0, 0.0, 200.0, 200.0, 200.0, 100.0, 0.0, 200.0, 100.00000000833332,
        100.00000000833332, 200.0, 0.0, 100.0, 200.0, 0.0, 0.0, 200.0, 100.0, 200.0, 200.0, 0.0, 200.0, 200.0,
        200.0, 0.0, 200.0, 200.0, 100.0, 200.0, 200.0, 200.0, 149.999999970343, 149.9999999874621, 49.999999881628682
      };

    const int conn[212]=
      {
        26, 28, 14, 20, 19, 4, 21, 22, 6, 11, 18, 2, 2, 4, 1, 8, 19, 2, 1, 28, 13, 28, 14, 25,
        26, 20, 17, 27, 2, 3, 7, 9, 16, 14, 13, 6, 25, 14, 26, 28, 11, 12, 10, 7, 20, 9, 24, 2,
        23, 9, 24, 20, 17, 14, 18, 2, 7, 10, 11, 9, 14, 18, 6, 16, 6, 5, 2, 13, 19, 1, 25, 28,
        20, 21, 19, 2, 8, 7, 6, 2, 5, 13, 16, 15, 26, 28, 20, 19, 2, 20, 17, 14, 21, 20, 24, 2,
        28, 13, 2, 1, 7, 6, 2, 11, 5, 6, 2, 8, 13, 28, 2, 14, 6, 16, 5, 13, 20, 17, 27, 23, 14,
        6, 18, 2, 2, 4, 8, 3, 14, 6, 2, 13, 19, 2, 4, 1, 9, 24, 3, 10, 4, 2, 19, 21, 2, 28, 20,
        14, 25, 26, 19, 28, 26, 17, 20, 14, 8, 2, 3, 7, 4, 2, 21, 3, 9, 17, 18, 2, 8, 5, 1, 2, 19,
        20, 2, 28, 28, 13, 1, 25, 10, 7, 3, 9, 2, 5, 1, 13, 20, 17, 23, 9, 9, 3, 24, 2, 2, 17, 20,
        9, 21, 3, 2, 24, 11, 2, 7, 9, 11, 9, 18, 2
      };
    MESHING* meshing = new MESHING;
    meshing->setName( "TESTMESH" );
    const int nNodes=28;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN", MED_EN::MED_FULL_INTERLACE);
    std::string coordname[3] = { "x", "y", "z" };
    meshing->setCoordinatesNames(coordname);
    std::string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement classicalTypesCell[1]={MED_EN::MED_TETRA4};
    const int nbOfCellElts[1]={53};
    meshing->setNumberOfTypes(1,MED_EN::MED_CELL);
    meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_TETRA4,conn);
    return meshing;
  }

  MESH * build3DTargetMesh1()
  {
    const double coords[24]=
      {
        200.0, 200.0, 0.0, 200.0, 200.0, 200.0, 200.0, 0.0, 0.0, 200.0, 0.0, 200.0,
        0.0, 200.0, 0.0, 0.0, 200.0, 200.0, 0.0, 0.0, 0.0, 0.0, 0.0, 200.0
      };

    const int conn[20]=
      {
        6, 7, 4, 1, 2, 4, 1, 6, 4, 7, 6, 8, 7, 5, 1, 6, 7, 4, 1, 3
      };

    MESHING* meshing = new MESHING;
    meshing->setName( "TESTMESH" );
    const int nNodes=8;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
    std::string coordname[3] = { "x", "y", "z" };
    meshing->setCoordinatesNames(coordname);
    std::string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement classicalTypesCell[1]={MED_EN::MED_TETRA4};
    const int nbOfCellElts[1]={5};
    meshing->setNumberOfTypes(1,MED_EN::MED_CELL);
    meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_TETRA4,conn);
    return meshing;
  }

  MESH * build1DTargetMesh1()
  {
    double coords[36]=
      {
        25.,25.,0., 25.,25.,50., 25.,25.,200., 75.,25.,0., 75.,25.,50., 75.,25.,200.,
        25.,125.,0., 25.,125.,50., 25.,125.,200., 125.,125.,0., 125.,125.,50., 125.,125.,200.
      };
    const int conn[16]=
      {
        1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 11, 12
      };
    MESHING* meshing = new MESHING;
    meshing->setName( "TESTMESH" );
    const int nNodes=12;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
    string coordname[3] = { "x", "y", "z" };
    meshing->setCoordinatesNames(coordname);
    string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement classicalTypesCell[1]={MED_EN::MED_SEG2};
    const int nbOfCellElts[1]={8};
    meshing->setNumberOfTypes(1,MED_EN::MED_CELL);
    meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_SEG2,conn);
    return meshing;
  }

  MESH * build3DSourceMesh2()
  {
    double coords[27]=
      {
        0.0, 0.0, 200.0, 0.0, 0.0, 0.0, 0.0, 200.0, 200.0, 0.0, 200.0, 0.0, 200.0, 0.0, 200.0,
        200.0, 0.0, 0.0, 200.0, 200.0, 200.0, 200.0, 200.0, 0.0, 100.0, 100.0, 100.0 
      };
    int conn[48]=
      {
        9, 2, 8, 4, 7, 1, 9, 3, 8, 5, 6, 9, 7, 9, 5, 8, 7, 9, 1, 5, 7, 9, 8, 4, 9, 2, 4, 1, 5, 2, 6, 9, 2, 8, 6, 9, 1, 4, 9, 3, 9, 2, 1, 5, 4, 7, 9, 3
      };
    MESHING* meshing = new MESHING;
    meshing->setName( "TESTMESH" );
    const int nNodes=9;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
    string coordname[3] = { "x", "y", "z" };
    meshing->setCoordinatesNames(coordname);
    string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement classicalTypesCell[1]={MED_EN::MED_TETRA4};
    const int nbOfCellElts[1]={12};
    meshing->setNumberOfTypes(1,MED_EN::MED_CELL);
    meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //
    const MED_EN::medGeometryElement classicalTypesFace[1]={MED_EN::MED_TRIA3};
    const int nbOfFaceElts[1]={1};
    meshing->setNumberOfTypes(1,MED_EN::MED_FACE);
    meshing->setTypes(classicalTypesFace,MED_EN::MED_FACE);
    meshing->setNumberOfElements(nbOfFaceElts,MED_EN::MED_FACE);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_TETRA4,conn);
    meshing->setConnectivity(MED_EN::MED_FACE,MED_EN::MED_TRIA3,conn);
    return meshing;
  }

  MESH * build3DTargetMesh2()
  {
    double coords[81]=
      {
        0., 0., 0., 50., 0., 0. , 200., 0., 0.  , 0., 50., 0., 50., 50., 0. , 200., 50., 0.,   0., 200., 0., 50., 200., 0. , 200., 200., 0. ,
        0., 0., 50., 50., 0., 50. , 200., 0., 50.  , 0., 50., 50., 50., 50., 50. , 200., 50., 50.,   0., 200., 50., 50., 200., 50. , 200., 200., 50. ,
        0., 0., 200., 50., 0., 200. , 200., 0., 200.  , 0., 50., 200., 50., 50., 200. , 200., 50., 200.,   0., 200., 200., 50., 200., 200. , 200., 200., 200. 
      };
    int conn[64]=
      {
        1, 2, 5, 4, 10, 11, 14, 13, 2, 3, 6, 5, 11, 12, 15, 14, 4, 5, 8, 7, 13, 14, 17, 16, 5, 6, 9, 8, 14, 15, 18,
        17, 10, 11, 14, 13, 19, 20, 23, 22, 11, 12, 15, 14, 20, 21, 24, 23, 13, 14, 17, 16, 22, 23, 26, 25, 14, 15, 18, 17, 23, 24, 27, 26
      };
    MESHING* meshing = new MESHING;
    meshing->setName( "TESTMESH" );
    const int nNodes=27;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
    string coordname[3] = { "x", "y", "z" };
    meshing->setCoordinatesNames(coordname);
    string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement classicalTypesCell[1]={MED_EN::MED_HEXA8};
    const int nbOfCellElts[1]={8};
    meshing->setNumberOfTypes(1,MED_EN::MED_CELL);
    meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //
    const MED_EN::medGeometryElement classicalTypesFace[1]={MED_EN::MED_QUAD4};
    const int nbOfFaceElts[1]={1};
    meshing->setNumberOfTypes(1,MED_EN::MED_FACE);
    meshing->setTypes(classicalTypesFace,MED_EN::MED_FACE);
    meshing->setNumberOfElements(nbOfFaceElts,MED_EN::MED_FACE);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_HEXA8,conn);
    meshing->setConnectivity(MED_EN::MED_FACE,MED_EN::MED_QUAD4,conn);
    return meshing;
  }

  MESH * build3DSourceMesh2Poly()
  {
    double coords[27]={ 0.0, 0.0, 200.0, 0.0, 0.0, 0.0, 0.0, 200.0, 200.0, 0.0, 200.0, 0.0, 200.0, 0.0, 200.0,
                            200.0, 0.0, 0.0, 200.0, 200.0, 200.0, 200.0, 200.0, 0.0, 100.0, 100.0, 100.0 };
    int conn[40]={9, 2, 8, 4, 7, 1, 9, 3, 8, 5, 6, 9, 7, 9, 5, 8, 7, 9, 1, 5, 7, 9, 8, 4, 9, 2, 4, 1, 5, 2, 6, 9, 2, 8, 6, 9, 1, 4, 9, 3};
    int connPoly1[3]={1,16,31};
    int connPoly2[30]={9, 2, 1, -1, 9, 5, 2, -1, 2, 5, 1, -1, 1, 5, 9,
                       4, 7, 9, -1, 4, 3, 7, -1, 7, 3, 9, -1, 9, 3, 4};
    MESHING* meshing = new MESHING;
    meshing->setName( "TESTMESH" );
    const int nNodes=9;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
    string coordname[3] = { "x", "y", "z" };
    meshing->setCoordinatesNames(coordname);
    string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement typesCell[2]={MED_EN::MED_TETRA4,MED_EN::MED_POLYHEDRA};
    const int nbOfCellElts[2]={10,2};
    meshing->setNumberOfTypes(2,MED_EN::MED_CELL);
    meshing->setTypes(typesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_TETRA4,conn);
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_POLYHEDRA,connPoly2,connPoly1);
    return meshing;
  }

  MESH * build3DTargetMesh2Poly()
  {
    double coords[81]={ 0., 0., 0., 50., 0., 0. , 200., 0., 0.  , 0., 50., 0., 50., 50., 0. , 200., 50., 0.,   0., 200., 0., 50., 200., 0. , 200., 200., 0. ,
                        0., 0., 50., 50., 0., 50. , 200., 0., 50.  , 0., 50., 50., 50., 50., 50. , 200., 50., 50.,   0., 200., 50., 50., 200., 50. , 200., 200., 50. ,
                        0., 0., 200., 50., 0., 200. , 200., 0., 200.  , 0., 50., 200., 50., 50., 200. , 200., 50., 200.,   0., 200., 200., 50., 200., 200. , 200., 200., 200. };
    int conn[56]={1, 2, 5, 4, 10, 11, 14, 13, 2, 3, 6, 5, 11, 12, 15, 14, 4, 5, 8, 7, 13, 14, 17, 16, 5, 6, 9, 8, 14, 15, 18,
                  17, 10, 11, 14, 13, 19, 20, 23, 22, 11, 12, 15, 14, 20, 21, 24, 23, 13, 14, 17, 16, 22, 23, 26, 25};
    int connPoly1[2]={1,30};
    int connPoly3[29]={ 14, 15, 18, 17, -1, 23, 26, 27, 24, -1, 14, 23, 24, 15, -1, 15, 24, 27, 18, -1, 18, 27, 26, 17, -1, 17, 26, 23, 14};
    MESHING* meshing = new MESHING;
    meshing->setName( "TESTMESH" );
    const int nNodes=27;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
    string coordname[3] = { "x", "y", "z" };
    meshing->setCoordinatesNames(coordname);
    string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement typesCell[2]={MED_EN::MED_HEXA8,MED_EN::MED_POLYHEDRA};
    const int nbOfCellElts[2]={7,1};
    meshing->setNumberOfTypes(2,MED_EN::MED_CELL);
    meshing->setTypes(typesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_HEXA8,conn);
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_POLYHEDRA,connPoly3,connPoly1);
    return meshing;
  }
} // noname namespace

void MEDMEMTest::test_remapper4()
{
  const double valsSrc[28]=
    {
      1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,16.,17.,18.,19.,20.,21.,22.,23.,24.,25.,26.,27.,28.
    };
  const double targetExpected[8]= 
    {
      16.2061111122415724, 21.8916666665293072, 15.5833333333333321, 13.1613888888184309, 11.8583333333333361, 10.6969444444233712, 4.48388888888888815, 9.42500000000000071
    };

  MESH *source=build3DSourceMesh1();
  MESH *target=build3DTargetMesh1();
  const SUPPORT *supSrc=source->getSupportOnAll( MED_EN::MED_NODE );
  FIELD<double> *f1=new MEDMEM::FIELD<double>(supSrc,1);
  double *val=(double *)f1->getValue();
  std::copy(valsSrc,valsSrc+28,val);
  const SUPPORT *supTrg=target->getSupportOnAll( MED_EN::MED_NODE );
  FIELD<double> *f2=new FIELD<double>(supTrg,1);
  //
  MEDMEM_REMAPPER remap;
  remap.prepare(*source,*target,"P1P1");
  remap.transfer(*f1,*f2);
  const double *tmp=f2->getValue();
  for(int i=0;i<8;i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL(targetExpected[i],tmp[i],1e-12);
  //
  source->removeReference();
  target->removeReference();
  f1->removeReference();
  f2->removeReference();
}

void MEDMEMTest::test_remapper5()
{
  MESH *sourceMesh=build3DSourceMesh2();
  MESH *targetMesh=build3DTargetMesh2();
  MEDNormalizedUnstructuredMesh<3,3> sourceWrapper(sourceMesh);
  MEDNormalizedUnstructuredMesh<3,3> targetWrapper(targetMesh);
  INTERP_KERNEL::Interpolation3D myInterpolator;
  vector<map<int,double> > res;
  myInterpolator.setPrecision(1e-12);
  myInterpolator.setIntersectionType(INTERP_KERNEL::PointLocator);
  myInterpolator.interpolateMeshes(sourceWrapper,targetWrapper,res,"P0P0");
  CPPUNIT_ASSERT_EQUAL(8,(int)res.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[6][10],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][4],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][5],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][6],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][12],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(21.,sumAll(res),1e-12);
  res.clear();
  //
  MEDNormalizedUnstructuredMesh<3,3> sourceWrapper2(targetMesh);
  MEDNormalizedUnstructuredMesh<3,3> targetWrapper2(sourceMesh);
  INTERP_KERNEL::Interpolation3D myInterpolator2;
  myInterpolator2.setPrecision(1e-12);
  myInterpolator2.setIntersectionType(INTERP_KERNEL::PointLocator);
  myInterpolator2.interpolateMeshes(sourceWrapper2,targetWrapper2,res,"P0P1");
  CPPUNIT_ASSERT_EQUAL(9,(int)res.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][5],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][3],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][6],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[6][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][4],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[8][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(9.,sumAll(res),1e-12);
  res.clear();
  //
  myInterpolator.setPrecision(1e-12);
  myInterpolator.setIntersectionType(INTERP_KERNEL::PointLocator);
  myInterpolator.interpolateMeshes(sourceWrapper,targetWrapper,res,"P1P0");
  CPPUNIT_ASSERT_EQUAL(8,(int)res.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.75,res[0][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.25,res[0][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[1][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][6],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[1][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[2][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][4],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[2][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[3][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[3][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[4][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[4][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[5][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][5],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,res[5][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.25,res[6][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.25,res[6][3],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.25,res[6][4],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.25,res[6][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.25,res[7][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.75,res[7][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(21.,sumAll(res),1e-12);
  //
  targetMesh->removeReference();
  sourceMesh->removeReference();
}

void MEDMEMTest::test_remapper6()
{
  MESH *sourceMesh=build3DSourceMesh2Poly();
  MESH *targetMesh=build3DTargetMesh2Poly();
  MEDNormalizedUnstructuredMesh<3,3> sourceWrapper(sourceMesh);
  MEDNormalizedUnstructuredMesh<3,3> targetWrapper(targetMesh);
  INTERP_KERNEL::Interpolation3D myInterpolator;
  vector<map<int,double> > res;
  myInterpolator.setPrecision(1e-12);
  myInterpolator.setIntersectionType(INTERP_KERNEL::PointLocator);
  myInterpolator.interpolateMeshes(sourceWrapper,targetWrapper,res,"P0P0");
  CPPUNIT_ASSERT_EQUAL(8,(int)res.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[6][10],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][4],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][5],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][6],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][12],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(21.,sumAll(res),1e-12);
  targetMesh->removeReference();
  sourceMesh->removeReference();
}

void MEDMEMTest::test_remapper7()
{
  MESH *sourceMesh=build3DSourceMesh2();
  MESH *targetMesh=build3DTargetMesh2();
  sourceMesh->convertToPoly();
  targetMesh->convertToPoly();
  MEDNormalizedUnstructuredMesh<3,3> sourceWrapper(sourceMesh);
  MEDNormalizedUnstructuredMesh<3,3> targetWrapper(targetMesh);
  INTERP_KERNEL::Interpolation3D myInterpolator;
  vector<map<int,double> > res;
  myInterpolator.setPrecision(1e-12);
  myInterpolator.setIntersectionType(INTERP_KERNEL::PointLocator);
  myInterpolator.interpolateMeshes(sourceWrapper,targetWrapper,res,"P0P0");
  CPPUNIT_ASSERT_EQUAL(8,(int)res.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][9],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][11],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[6][10],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][4],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][5],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][6],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][12],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(21.,sumAll(res),1e-12);
  targetMesh->removeReference();
  sourceMesh->removeReference();
}

void MEDMEMTest::test_remapper3DTo1D()
{
  MESH *sourceMesh=build3DTargetMesh2();
  MESH *targetMesh=build1DTargetMesh1();
  MEDNormalizedUnstructuredMesh<3,3> sourceWrapper(sourceMesh);
  MEDNormalizedUnstructuredMesh<3,3> targetWrapper(targetMesh);
  INTERP_KERNEL::Interpolation3D myInterpolator;
  vector<map<int,double> > res;
  myInterpolator.setPrecision(1e-12);
  myInterpolator.setIntersectionType(INTERP_KERNEL::PointLocator);
  myInterpolator.interpolateMeshes(sourceWrapper,targetWrapper,res,"P0P0");
  CPPUNIT_ASSERT_EQUAL(8,(int)res.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[0][1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[1][5],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[2][2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[3][6],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[4][3],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[5][7],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[6][4],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,res[7][8],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8.,sumAll(res),1e-12);
  targetMesh->removeReference();
  sourceMesh->removeReference();
}

double MEDMEMTest::sumAll(const std::vector< std::map<int,double> >& matrix)
{
  double ret=0.;
  for(std::vector< std::map<int,double> >::const_iterator iter=matrix.begin();iter!=matrix.end();iter++)
    for(std::map<int,double>::const_iterator iter2=(*iter).begin();iter2!=(*iter).end();iter2++)
      ret+=(*iter2).second;
  return ret;
}

void MEDMEMTest::absField(MEDMEM::FIELD<double>& field)
{
  double* areas=const_cast<double*>(field.getValue());
  for (int i=0; i< field.getNumberOfValues();i++)
    {
      areas[i]=fabs(areas[i]);
    }
}

// }
