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

// File      : MEDMEMTest_Extractor.cxx
// Created   : Mon Dec 22 11:12:57 2008
// Author    : Edward AGAPOV (eap)
//
#include "MEDMEMTest.hxx"

#include "MEDMEM_Extractor.hxx"
#include "MEDMEM_Meshing.hxx"

#include <cppunit/TestAssert.h>

using namespace std;
using namespace MEDMEM;

//================================================================================
/*!
 * \brief test extractLine()
 */
//================================================================================

static void test_extractLine( Extractor* extractor,
                              const double* coords, const double* direction,
                              int nbSegments,
                              const char* name,
                              const string& result_file)
{
  FIELD<double>* resField =0;
  CPPUNIT_ASSERT_NO_THROW( resField = extractor->extractLine(coords,direction));
  CPPUNIT_ASSERT( bool( resField ) == bool( nbSegments > 0 ));

  // store extracted field
  if ( resField )
    {
      const GMESH* mesh = resField->getSupport()->getMesh();
      mesh->write( MED_DRIVER, result_file, name );
      resField->write(MED_DRIVER, result_file );

      CPPUNIT_ASSERT_EQUAL( nbSegments, resField->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS));
      CPPUNIT_ASSERT_EQUAL( nbSegments+1, resField->getSupport()->getMesh()->getNumberOfNodes());
      resField->removeReference();
    }
}

void MEDMEMTest::testExtractor()
{
  // =======================
  // TEST 3D->2D extraction
  // =======================

  // read a mesh from a MED file
  string filename  = getResourceFile("pointe.med");
  string meshname  = "maa1";
  string fieldname = "fieldcelldoublescalar";

  string result_file = makeTmpFile("extracted_2D.med", filename);

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(result_file);

  // field with no mesh
  FIELD<double> * aField1 = new FIELD<double> (MED_DRIVER, filename, fieldname);
  CPPUNIT_ASSERT_THROW( new Extractor(*aField1), MEDEXCEPTION);

  const SUPPORT * aSupport = aField1->getSupport();
  MESH * aMesh = new MESH(MED_DRIVER, filename, aSupport->getMeshName());
  aSupport->setMesh(aMesh);

  Extractor* extractor = 0;
  CPPUNIT_ASSERT_NO_THROW( extractor = new Extractor(*aField1));

  FIELD<double>* resField = 0;
  double coords [3] = 
    {
      0,0,0
    };
  {
    // bad normal
    double normal [3] = 
      {
        0,0,0
      };
    CPPUNIT_ASSERT_THROW( extractor->extractPlane(coords,normal), MEDEXCEPTION);
  }
  double normal [3] = 
    {
      10,0,0
    };
  {
    // no intersection
    double coords [3] = 
      {
        10,0,0
      };
    CPPUNIT_ASSERT_NO_THROW( resField = extractor->extractPlane(coords,normal));
    CPPUNIT_ASSERT( !bool( resField ));
  }
  CPPUNIT_ASSERT_NO_THROW( resField = extractor->extractPlane(coords,normal ));

  // store extracted mesh
  const GMESH* mesh = resField->getSupport()->getMesh();
  mesh->write( MED_DRIVER, result_file );
  CPPUNIT_ASSERT_EQUAL( 2, mesh->getNumberOfTypes(MED_CELL));
  CPPUNIT_ASSERT_EQUAL( 6, mesh->getNumberOfElements(MED_CELL, MED_TRIA3));
  CPPUNIT_ASSERT_EQUAL( 2, mesh->getNumberOfElements(MED_CELL, MED_QUAD4));

  // store extracted field
  resField->write(MED_DRIVER, result_file);

  delete extractor; extractor=0;
  aMesh->removeReference(); aMesh=0;
  resField->removeReference(); resField=0;
  aField1->removeReference(); aField1=0;

  // ===================================================================================
  // TEST 2D->1D extraction in 3d space
  // ===================================

  // create 3d shell
  int SpaceDimension = 3 ;
  int NumberOfNodes = 8 ;
  double Coordinates[24] = 
    {
      0.0, 0.0, 200,
      0.0, 0.0, 0.0,
      0.0, 200, 200,
      0.0, 200, 0.0,
      200, 0.0, 200,
      200, 0.0, 0.0,
      200, 200, 200,
      200, 200, 0.0
    };
  MESHING* myMeshing  = new MESHING();
  myMeshing->setName("shell") ;

  myMeshing->setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,
                            "CARTESIAN",MED_FULL_INTERLACE);
  string Names[3] = 
    {
      "X","Y","Z"
    } ;
  myMeshing->setCoordinatesNames(Names);
  string Units[3] = 
    {
      "cm","cm","cm"
    } ;
  myMeshing->setCoordinatesUnits(Units) ;

  // define conectivities

  const int NumberOfTypes = 1;
  medGeometryElement Types[NumberOfTypes] = 
    {
      MED_TRIA3
    };
  const int NumberOfElements[NumberOfTypes] = 
    {
      12
    };

  myMeshing->setNumberOfTypes(NumberOfTypes,MED_CELL);
  myMeshing->setTypes(Types,MED_CELL);
  myMeshing->setNumberOfElements(NumberOfElements,MED_CELL);

  const int sizeTria = 12*3 ;
  int ConnectivityTria[sizeTria]= 
    {
      2, 5, 1, 
      4, 1, 3, 
      3, 1, 7, 
      4, 8, 2, 
      1, 5, 7, 
      6, 5, 2, 
      2, 8, 6, 
      8, 7, 5, 
      6, 8, 5, 
      2, 1, 4, 
      8, 4, 7, 
      4, 3, 7 
    };
  myMeshing->setConnectivity(MED_CELL,MED_TRIA3,ConnectivityTria);

  // store input mesh

  fieldname   = "doubleOnTria";
  result_file = makeTmpFile("extracted_1D.med");

  // To remove tmp files from disk
  aRemover.Register(result_file);

  CPPUNIT_ASSERT_NO_THROW( myMeshing->write( MED_DRIVER, result_file ));

  // Make input field

  aSupport = myMeshing->getSupportOnAll( MED_CELL );
  FIELD<double>* inField = new FIELD<double>( aSupport, 1 );
  inField->setName(fieldname);
  string str = "";
  inField->setComponentsNames( &str );
  inField->setDescription( str );
  inField->setComponentsDescriptions( &str );
  inField->setMEDComponentsUnits( &str );

  vector<double> value( NumberOfElements[0] );
  for ( unsigned i = 0; i < value.size(); ++i )
    value[i] = double ( i % 10 );
  inField->setValue( &value[0] );

  // store input field
  int drv = inField->addDriver(MED_DRIVER, result_file, fieldname);
  inField->write(drv);

  // Extraction
  {
    double coords [3] = 
      {
        0,0,0
      };
    double normal [3] = 
      {
        -2, -2, 1 
      };
    CPPUNIT_ASSERT_NO_THROW( extractor = new Extractor(*inField));
    CPPUNIT_ASSERT_NO_THROW( resField = extractor->extractPlane(coords,normal));
    CPPUNIT_ASSERT( bool( resField ));
  }

  // store extracted mesh
  mesh = resField->getSupport()->getMesh();
  mesh->write( MED_DRIVER, result_file );
  //   CPPUNIT_ASSERT_EQUAL( 2, mesh->getNumberOfTypes(MED_CELL));
  //   CPPUNIT_ASSERT_EQUAL( 6, mesh->getNumberOfElements(MED_CELL, MED_TRIA3));
  //   CPPUNIT_ASSERT_EQUAL( 2, mesh->getNumberOfElements(MED_CELL, MED_QUAD4));

  // store extracted field
  drv = resField->addDriver(MED_DRIVER, result_file, resField->getName());
  resField->write( drv );

  aSupport=0;
  myMeshing->removeReference(); myMeshing=0;
  inField->removeReference(); inField=0;
  delete extractor; extractor=0;
  resField->removeReference(); resField=0;

  // ======================================================================================
  // TEST 3D->1D extraction
  // =======================

  // create 3d 2x2x2 hexa box
  myMeshing  = new MESHING();
  {
    SpaceDimension = 3 ;
    NumberOfNodes = 27 ;
    double Coordinates[27*3] = 
      {
        0.0, 0.0, 200,
        0.0, 0.0, 0.0,
        0.0, 200, 200,
        0.0, 200, 0.0,
        200, 0.0, 200,
        200, 0.0, 0.0,
        200, 200, 200,
        200, 200, 0.0,
        0.0, 0.0, 100,
        0.0, 100, 200,
        0.0, 200, 100,
        0.0, 100, 0.0,
        200, 0.0, 100,
        200, 100, 200,
        200, 200, 100,
        200, 100, 0.0,
        100, 0.0, 0.0,
        100, 0.0, 200,
        100, 200, 0.0,
        100, 200, 200,
        0.0, 100, 100,
        200, 100, 100,
        100, 0.0, 100,
        100, 200, 100,
        100, 100, 0.0,
        100, 100, 200,
        100, 100, 100,
      };
    myMeshing->setName("box") ;

    myMeshing->setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,
                              "CARTESIAN",MED_FULL_INTERLACE);
    string Names[3] = 
      {
        "X","Y","Z"
      } ;
    myMeshing->setCoordinatesNames(Names);
    string Units[3] = 
      {
        "cm","cm","cm"
      } ;
    myMeshing->setCoordinatesUnits(Units) ;

    // define conectivities

    const int NumberOfTypes = 1;
    medGeometryElement Types[NumberOfTypes] = 
      {
        MED_HEXA8
      };
    const int NumberOfElements[NumberOfTypes] = 
      {
        8
      };

    myMeshing->setNumberOfTypes(NumberOfTypes,MED_CELL);
    myMeshing->setTypes(Types,MED_CELL);
    myMeshing->setNumberOfElements(NumberOfElements,MED_CELL);

    int ConnectivityHex[8*8]= 
      {
        9, 23, 18, 1, 21, 27, 26, 10, 
        25, 16, 22, 27, 19, 8, 15, 24, 
        27, 22, 14, 26, 24, 15, 7, 20, 
        17, 6, 13, 23, 25, 16, 22, 27, 
        21, 27, 26, 10, 11, 24, 20, 3, 
        2, 17, 23, 9, 12, 25, 27, 21, 
        23, 13, 5, 18, 27, 22, 14, 26, 
        12, 25, 27, 21, 4, 19, 24, 11 
      };
    myMeshing->setConnectivity(MED_CELL,MED_HEXA8,ConnectivityHex);
  }
  // store input mesh

  fieldname   = "doubleOnHex";
  result_file = makeTmpFile("extracted3D_1D.med");

  // To remove tmp files from disk
  aRemover.Register(result_file);

  drv = myMeshing->addDriver( MED_DRIVER, result_file, myMeshing->getName() );
  CPPUNIT_ASSERT_NO_THROW( myMeshing->write(drv) );

  // Make input field

  aSupport = myMeshing->getSupportOnAll( MED_CELL );
  inField = new FIELD<double>( aSupport, 1 );
  inField->setName(fieldname);
  inField->setComponentsNames( &str );
  inField->setDescription( str );
  inField->setComponentsDescriptions( &str );
  inField->setMEDComponentsUnits( &str );

  value.resize( NumberOfElements[0] );
  for ( unsigned i = 0; i < value.size(); ++i )
    value[i] = double (i+1);
  inField->setValue( &value[0] );
  // store input field
  drv = inField->addDriver(MED_DRIVER, result_file, fieldname);
  inField->write(drv);

  // Extraction
  CPPUNIT_ASSERT_NO_THROW( extractor = new Extractor(*inField));
  {
    // Test corner-corner intersection
    double coords [3] = 
      {
        1, 1, 1 
      };
    double direct [3] = 
      {
        2, 2, 2 
      };
    test_extractLine( extractor, coords, direct, 2, "corner-corner", result_file );
  }
  {
    // Test corner-face intersection
    double coords [3] = 
      {
        0,0,0
      };
    double direct [3] = 
      {
        2, 1, 1 
      };
    test_extractLine( extractor, coords, direct, 2, "corner-face", result_file );
  }
  {
    // Test on-face intersection
    double coords [3] = 
      {
        -2, 0,-1 
      };
    double direct [3] = 
      {
        2, 0, 1 
      };
    test_extractLine( extractor, coords, direct, 2, "on-face", result_file );
  }
  {
    // Test between-cells intersection
    double coords [3] = 
      {
        100,0,0
      };
    double direct [3] = 
      {
        0, 2, 2 
      };
    test_extractLine( extractor, coords, direct, 2, "between-cells", result_file );
  }
  {
    // Test between-cells-entrance intersection
    double coords [3] = 
      {
        100,0,0
      };
    double direct [3] = 
      {
        1, 2, 2 
      };
    test_extractLine( extractor, coords, direct, 2, "between-cells-entrance", result_file );
  }
  {
    // Test edge-entrance intersection
    double coords [3] = 
      {
        100,0,50
      };
    double direct [3] = 
      {
        1, 2, 2 
      };
    test_extractLine( extractor, coords, direct, 3, "edge-entrance", result_file );
  }
  {
    // Test touch intersection - expect no result
    double coords [3] = 
      {
        0,0,0
      };
    double direct [3] = 
      {
        0, 2, -2 
      };
    test_extractLine( extractor, coords, direct, 0, "touch", result_file );
  }
  {
    // Test face-face intersection
    double coords [3] = 
      {
        50,50,0
      };
    double direct [3] = 
      {
        2, 2, 0 
      };
    test_extractLine( extractor, coords, direct, 2, "corner-corner-on", result_file );
  }
  {
    // Test face-face intersection
    double coords [3] = 
      {
        50,50,0
      };
    double direct [3] = 
      {
        2, 2, 2 
      };
    test_extractLine( extractor, coords, direct, 3, "face-face", result_file );
  }
  {
    // Test external edge intersection
    double coords [3] = 
      {
        0, 0,200 
      };
    double direct [3] = 
      {
        -1, 0, 0 
      };
    test_extractLine( extractor, coords, direct, 2, "external edge", result_file );
  }
  {
    // Test internal edge intersection
    double coords [3] = 
      {
        100,0,100
      };
    double direct [3] = 
      {
        0, -2, 0 
      };
    test_extractLine( extractor, coords, direct, 2, "internal edge", result_file );
  }

  inField->setSupport(0);
  aSupport=0;
  delete extractor; extractor=0;
  myMeshing->removeReference(); myMeshing=0;

  // ======================================================================================
  // TEST 3D->1D extraction on a large model
  // =======================================

  // read a mesh
  filename = getResourceFile("geomMesh22.med");
  meshname = "GeomMesh";
  aMesh = new MESH(MED_DRIVER, filename, meshname);
  aSupport = aMesh->getSupportOnAll( MED_CELL );

  // make a field
  int nbValues = aSupport->getNumberOfElements(MED_ALL_ELEMENTS);
  inField->setSupport( aSupport );
  inField->allocValue( 1, nbValues );
  {
    double* value = const_cast<double*>( inField->getValue() );
    for ( int i = 0; i < nbValues; ++i )
      value[i] = double ( i % 7 );
  }
  // extract a field
  CPPUNIT_ASSERT_NO_THROW( extractor = new Extractor(*inField));
  {
    double coords [3] = 
      {
        20,0,10
      };
    double direct [3] = 
      {
        1, 1,1.5 
      };
    CPPUNIT_ASSERT_NO_THROW( resField = extractor->extractLine(coords,direct));
  }
  CPPUNIT_ASSERT( resField );
  CPPUNIT_ASSERT_EQUAL( 31, resField->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_EQUAL( 33, resField->getSupport()->getMesh()->getNumberOfNodes());
  delete extractor;
  resField->removeReference(); resField=0;
  aMesh->removeReference(); aMesh=0;
  aSupport=0;
  inField->removeReference(); inField=0;
}

