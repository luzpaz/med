//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>
#include "MEDMEM_Remapper.hxx"

#include <iostream>
//#include <vector>

// namespace MEDMEMTest
// {

void MEDMEMTest::test_RemapperP0P0() {
  string sourcename=getenv("MED_ROOT_DIR");
  sourcename +="/share/salome/resources/med/square1.med";
  MEDMEM::MESH source_mesh (MED_DRIVER,sourcename,"Mesh_2");
  
  string targetname=getenv("MED_ROOT_DIR");
  targetname +="/share/salome/resources/med/square2.med";
  MEDMEM::MESH target_mesh (MED_DRIVER,targetname,"Mesh_3");
  
  int nbcomp=3;
  
  MEDMEM::SUPPORT source_support(&source_mesh,"on All support",MED_EN::MED_CELL);
  MEDMEM::FIELD<double> source_field(&source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field.getValue());
  for (int i=0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;
  
  MEDMEM::SUPPORT target_support(&target_mesh,"on All support",MED_EN::MED_CELL);
  MEDMEM::FIELD<double> target_field(&target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field.getValue());
  for (int i=0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=0.0;
  
  
  MEDMEM_REMAPPER remapper;
  const string intersectiontype = "IntersectionType";
  string convex = "Convex";
  remapper.setOptionDouble("Precision",1.e-8);
  remapper.setOptionString(intersectiontype,convex);
  remapper.setOptionInt("PrintLevel",1);
  remapper.prepare(source_mesh,target_mesh,"P0P0");
  remapper.transfer(source_field,target_field);
  
  //MN: Old tests
  MEDMEM::FIELD<double> *source_areas=source_mesh.getArea(&source_support);
  MEDMEM::FIELD<double> *target_areas=target_mesh.getArea(&target_support);
  //MEDMEMTest::absField(*source_areas); //absolute value
  //MEDMEMTest::absField(*target_areas); //absolute value
  
  //target square is in reverse order as compared to initial square
  double source_integral=source_field.normL2(nbcomp,source_areas);
  double target_integral=target_field.normL2(nbcomp,target_areas);
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(source_integral,target_integral,1e-10);
  
  //MN: Transfer test
  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
  
  //MN: Reverse transfer test
  remapper.reverseTransfer(source_field,target_field);
  
  max = -2;
  min =  2;
  for(int i = 0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
  
  //MN: Hxx2salome transfer test
  MEDMEM::FIELD<double> *newTargetField =remapper.transferField(source_field,target_mesh);
  MEDMEM::FIELD<double> *newSourceField =remapper.reverseTransferField(target_field,source_mesh);
  sourcevalue=const_cast<double*>((*newSourceField).getValue());
  targetvalue=const_cast<double*>((*newTargetField).getValue());
  max = -2;
  min =  2;
  for(int i = 0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
  for(int i = 0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    

  delete newSourceField;
  delete newTargetField;
  
  delete source_areas;
  delete target_areas;
}

void  MEDMEMTest::test_RemapperP1P1() {
  string sourcename=getenv("MED_ROOT_DIR");
  sourcename +="/share/salome/resources/med/square1.med";
  MEDMEM::MESH source_mesh (MED_DRIVER,sourcename,"Mesh_2");
  
  string targetname=getenv("MED_ROOT_DIR");
  targetname +="/share/salome/resources/med/square2.med";
  MEDMEM::MESH target_mesh (MED_DRIVER,targetname,"Mesh_3");
  
  int nbcomp=2;
  
  MEDMEM::SUPPORT source_support(&source_mesh,"on All support",MED_EN::MED_NODE);
  MEDMEM::FIELD<double> source_field(&source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field.getValue());
  for (int i=0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;
  
  MEDMEM::SUPPORT target_support(&target_mesh,"on All support",MED_EN::MED_NODE);
  MEDMEM::FIELD<double> target_field(&target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field.getValue());
  for (int i=0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=-0.0001;
  
  
  MEDMEM_REMAPPER remapper;
  remapper.prepare(source_mesh,target_mesh,"P1P1");
  remapper.transfer(source_field,target_field);
  
  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
  
  remapper.reverseTransfer(source_field,target_field);
  
  max = -2;
  min =  2;
  for(int i = 0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
}


void  MEDMEMTest::test_RemapperP1P0() {
  string sourcename=getenv("MED_ROOT_DIR");
  sourcename +="/share/salome/resources/med/square1.med";
  MEDMEM::MESH source_mesh (MED_DRIVER,sourcename,"Mesh_2");
  
  string targetname=getenv("MED_ROOT_DIR");
  targetname +="/share/salome/resources/med/square2.med";
  MEDMEM::MESH target_mesh (MED_DRIVER,targetname,"Mesh_3");
  
  int nbcomp=3;
  MEDMEM::SUPPORT source_support(&source_mesh,"on All support",MED_EN::MED_NODE);
  MEDMEM::FIELD<double> source_field(&source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field.getValue());
  for (int i=0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;
  
  MEDMEM::SUPPORT target_support(&target_mesh,"on All support",MED_EN::MED_CELL);
  MEDMEM::FIELD<double> target_field(&target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field.getValue());
  for (int i=0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=0.0;
  
  
  MEDMEM_REMAPPER remapper;
  remapper.prepare(source_mesh,target_mesh,"P1P0");
  remapper.transfer(source_field,target_field);
  
  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
  
  remapper.reverseTransfer(source_field,target_field);
  
  max = -2;
  min =  2;
  for(int i = 0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);
}

void  MEDMEMTest::test_RemapperP0P1() {
  string sourcename=getenv("MED_ROOT_DIR");
  sourcename +="/share/salome/resources/med/square1.med";
  MEDMEM::MESH source_mesh (MED_DRIVER,sourcename,"Mesh_2");

  string targetname=getenv("MED_ROOT_DIR");
  targetname +="/share/salome/resources/med/square2.med";
  MEDMEM::MESH target_mesh (MED_DRIVER,targetname,"Mesh_3");

  int nbcomp=4;
  MEDMEM::SUPPORT source_support(&source_mesh,"on All support",MED_EN::MED_CELL);
  MEDMEM::FIELD<double> source_field(&source_support,nbcomp);
  double* sourcevalue=const_cast<double*>(source_field.getValue());
  for (int i=0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    sourcevalue[i]=1.0;
  
  MEDMEM::SUPPORT target_support(&target_mesh,"on All support",MED_EN::MED_NODE);
  MEDMEM::FIELD<double> target_field(&target_support,nbcomp);
  double* targetvalue=const_cast<double*>(target_field.getValue());
  for (int i=0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    targetvalue[i]=0.0;
  
  
  MEDMEM_REMAPPER remapper;
  remapper.prepare(source_mesh,target_mesh,"P0P1");
  remapper.transfer(source_field,target_field);
  
  double max = -2;
  double min =  2;
  for(int i = 0; i<target_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( targetvalue[i] >max) max = targetvalue[i];
      if( targetvalue[i] <min) min = targetvalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
  
  remapper.reverseTransfer(source_field,target_field);
  
  max = -2;
  min =  2;
  for(int i = 0; i<source_support.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)*nbcomp; i++)
    {
      if( sourcevalue[i] >max) max = sourcevalue[i];
      if( sourcevalue[i] <min) min = sourcevalue[i];
    }
  
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,max,1e-10);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1,min,1e-10);    
}

void absField(MEDMEM::FIELD<double>& field)
{
  double* areas=const_cast<double*>(field.getValue());
  for (int i=0; i< field.getNumberOfValues();i++)
    {
      areas[i]=fabs(areas[i]);
    }
}

// }
