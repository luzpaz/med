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
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include "MEDMEM_Array.hxx"
#include "MEDMEM_Coordinate.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #8: MEDMEM_Coordinate.hxx  }  MEDMEMTest_Coordinate.cxx

/*!
 *  Check methods (23), defined in MEDMEM_Coordinate.hxx:
 *  class COORDINATE {
 *   (+)     COORDINATE();
 *   (+)     COORDINATE(int SpaceDimension, int NumberOfNodes, MED_EN::medModeSwitch Mode);
 *   (+)     COORDINATE(int SpaceDimension,const string * CoordinateName, const string * CoordinateUnit);
 *   (+)     COORDINATE(const COORDINATE & m);
 *   (+)     virtual ~COORDINATE();
 *   (+)     void setCoordinates(MEDARRAY<double> *Coordinate,bool shallowCopy=false);
 *   (+)     void setCoordinates(const MED_EN::medModeSwitch Mode, const double *Coordinate);
 *   (+)     void setCoordinatesNames(const string * CoordinateName);
 *   (+)     void setCoordinateName(const string CoordinateName, const int i);
 *   (+)     void setCoordinatesUnits(const string * CoordinateUnit);
 *   (+)     void setCoordinateUnit(const string CoordinateUnit, const int i);
 *   (+)     void setCoordinatesSystem(const string CoordinateSystem);
 *   (+)     void setNodesNumbers(const int * NodeNumber);
 *   (+)     int  getSpaceDimension() const;
 *   (+)     int  getNumberOfNodes() const;
 *   (+)     virtual const int* getNodesNumbers() const;
 *   (+)     string getCoordinatesSystem() const;
 *   (+)     virtual const double * getCoordinates(MED_EN::medModeSwitch Mode);
 *   (+)     virtual double         getCoordinate(int Number,int Axis);
 *   (+)     virtual const double * getCoordinateAxis(int Axis);
 *   (+)     const string * getCoordinatesNames() const;
 *   (+)     string getCoordinateName(int Axis) const;
 *   (+)     const string * getCoordinatesUnits() const;
 *   (+)     string getCoordinateUnit(int Axis) const;
 *  }
 *
 *  Use code of MEDMEM/tests/testUCoordinate.cxx
 *              MEDMEM/test_copie_coordinate.cxx
 */
void MEDMEMTest::testCoordinate()
{
  //////////////////////////////////////////////
  // TEST 1: MEDMEM/tests/testUCoordinate.cxx //
  //////////////////////////////////////////////
  {
    const double pouieme = 0.009;

    int SpaceDim = 3;
    int NbOfNodes = 5;

    string * noms = new string[3];
    noms[0] = " X ";
    noms[1] = " Y ";
    noms[2] = " Z ";

    string  units[3];
    units[0] = " m ";
    units[1] = " m ";
    units[2] = " m ";

    int * numbers = new int[5];
    for (int i = 0; i < 5; i++) numbers[i] = 10 + i;

    double coor[15] = {0,0,0,0,0,1,0,1,0,1,0,0,0.5,0.5,0.5};
    CPPUNIT_ASSERT(SpaceDim * NbOfNodes == 15);

    double * coor1 = new double [SpaceDim*NbOfNodes];
    for (int k = 0; k < SpaceDim*NbOfNodes; k++)
      coor1[k] = coor[k];

    MEDARRAY<double>* CoordinateArray =
      new MEDARRAY<double>(coor1, SpaceDim, NbOfNodes, MED_EN::MED_FULL_INTERLACE);
    COORDINATE mycoo;
    CPPUNIT_ASSERT_NO_THROW(mycoo.setCoordinates(CoordinateArray));

    //--------------------------------------------------------------------//
    //                        Tests des methodes                          //
    //                                                                    //
    // - setCoordinatesNames                                              //
    // - setCoordinatesUnits                                              //
    // - setCoordinatesSystem                                             //
    // - setNodesNumbers                                                  //
    //                                                                    //
    //--------------------------------------------------------------------//

    CPPUNIT_ASSERT_NO_THROW(mycoo.setCoordinatesNames(noms));
    CPPUNIT_ASSERT_NO_THROW(mycoo.setCoordinatesUnits(units));
    CPPUNIT_ASSERT_NO_THROW(mycoo.setCoordinatesSystem("cartesien"));
    CPPUNIT_ASSERT_NO_THROW(mycoo.setNodesNumbers(numbers));

    //--------------------------------------------------------------------//
    //                        Tests des methodes                          //
    //                                                                    //
    // - getCoordinatesNames                                              //
    // - getCoordinatesUnits                                              //
    // - getCoordinatesUnit                                               //
    // - getCoordinatesSystem                                             //
    // - getNodesNumbers                                                  //
    //                                                                    //
    //--------------------------------------------------------------------//
    CPPUNIT_ASSERT_NO_THROW(CPPUNIT_ASSERT(mycoo.getCoordinatesSystem() == "cartesien"));

    const string * units2;
    try
    {
      units2 = mycoo.getCoordinatesUnits();
      for (int axe = 0; axe < SpaceDim; axe++) {
        string verif = mycoo.getCoordinateUnit(axe+1);
        CPPUNIT_ASSERT(verif == units2[axe]);
      }
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    const string * noms2;
    try
    {
      noms2 = mycoo.getCoordinatesNames();
      for (int axe = 0; axe < SpaceDim; axe++) {
        string verif = mycoo.getCoordinateName(axe+1);
        CPPUNIT_ASSERT(verif == noms2[axe]);
      }
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    try
    {
      const double * coor2 = mycoo.getCoordinates(MED_EN::MED_FULL_INTERLACE);

      for (int axe = 0; axe < SpaceDim; axe++) {
        const double * coor3 = mycoo.getCoordinateAxis(axe+1);
        for (int num = 0; num < NbOfNodes; num++) {
          const double d = mycoo.getCoordinate(num + 1, axe + 1);
          CPPUNIT_ASSERT(fabs(d - coor3[num                 ]) < pouieme);
          CPPUNIT_ASSERT(fabs(d - coor2[(num * SpaceDim)+axe]) < pouieme);
          CPPUNIT_ASSERT(fabs(d - coor [(num * SpaceDim)+axe]) < pouieme);
        }
      }
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    delete [] noms;
    delete [] numbers;
    delete [] coor1;

    delete CoordinateArray;
  }

  //////////////////////////////////////////////
  // TEST 2: MEDMEM/test_copie_coordinate.cxx //
  //////////////////////////////////////////////
  {
    const int numberofNodes = 5;
    const int spaceDimension = 3;
    const MED_EN::medModeSwitch mode = MED_EN::MED_FULL_INTERLACE;

    //construction tableau MEDARRAY des coordonnées
    MEDARRAY<double> * myMedArray = new MEDARRAY<double>(spaceDimension, numberofNodes, mode);
    for (int i = 1; i <= myMedArray->getLengthValue(); i++) {
      for (int j = 1; j <= myMedArray->getLeadingValue(); j++)
        myMedArray->setIJ(i, j, (double) i*j);
    }

    //construction noms des coordonnées
    string * myCoordinatesNames = new string[spaceDimension];
    if (spaceDimension >= 1) myCoordinatesNames[0] = "x";
    if (spaceDimension >= 2) myCoordinatesNames[1] = "y";
    if (spaceDimension >= 3) myCoordinatesNames[2] = "z";

    //construction unités des coordonnées
    string * myCoordinatesUnits = new string[spaceDimension];
    if (spaceDimension >= 1) myCoordinatesUnits[0] = "m";
    if (spaceDimension >= 2) myCoordinatesUnits[1] = "m";
    if (spaceDimension >= 3) myCoordinatesUnits[2] = "m";

    //construction des indices des noeuds
    int * myNodeNumber = new int[numberofNodes];
    for (int i = 0; i < numberofNodes; i++)
      myNodeNumber[i] = numberofNodes - i - 1;

    //construction de l'objet COORDINATE
    COORDINATE * myCoordinate = new COORDINATE();
    myCoordinate->setCoordinates(myMedArray);
    myCoordinate->setCoordinatesNames(myCoordinatesNames);
    myCoordinate->setCoordinatesUnits(myCoordinatesUnits);
    myCoordinate->setNodesNumbers(myNodeNumber);

    delete myMedArray;
    delete[] myCoordinatesNames;
    delete[] myCoordinatesUnits;
    delete[] myNodeNumber;

    COORDINATE * myCoordinate2 = new COORDINATE(* myCoordinate);
    delete myCoordinate;
    myCoordinate = NULL;

    // check copied coordinate
    int _spaceDimension = myCoordinate2->getSpaceDimension();
    int _numberofNodes  = myCoordinate2->getNumberOfNodes();
    CPPUNIT_ASSERT(_spaceDimension == spaceDimension);
    CPPUNIT_ASSERT(_numberofNodes  == numberofNodes);

    for (int i = 1; i <= _numberofNodes; i++) {
      for (int j = 1; j <= _spaceDimension; j++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(i*j, myCoordinate2->getCoordinate(i, j), 0.000001);
      }
    }

    CPPUNIT_ASSERT(myCoordinate2->getCoordinateName(1) == "x");
    CPPUNIT_ASSERT(myCoordinate2->getCoordinateName(2) == "y");
    CPPUNIT_ASSERT(myCoordinate2->getCoordinateName(3) == "z");

    CPPUNIT_ASSERT(myCoordinate2->getCoordinateUnit(1) == "m");
    CPPUNIT_ASSERT(myCoordinate2->getCoordinateUnit(2) == "m");
    CPPUNIT_ASSERT(myCoordinate2->getCoordinateUnit(3) == "m");

    for (int i = 0; i < _numberofNodes; i++)
      CPPUNIT_ASSERT(myCoordinate2->getNodesNumbers()[i] == _numberofNodes - i - 1);

    delete myCoordinate2;
  }

  ////////////
  // TEST 3 //
  ////////////
  {
    // COORDINATE(int SpaceDimension, int NumberOfNodes, MED_EN::medModeSwitch Mode);
    COORDINATE anEmptyC (2, 10, MED_EN::MED_FULL_INTERLACE);
    CPPUNIT_ASSERT(anEmptyC.getSpaceDimension() == 2);
    CPPUNIT_ASSERT(anEmptyC.getNumberOfNodes()  == 10);
    CPPUNIT_ASSERT(anEmptyC.getNodesNumbers() == NULL);
    // ?: how to fill it with coordinates?
    // 1. void setCoordinates(MEDARRAY<double> *Coordinate,bool shallowCopy=false);
    // but this way we can override all three constructor parameters
    // 2. void setCoordinates(const MED_EN::medModeSwitch Mode, const double *Coordinate);
    // in this case we can override Mode

//#ifdef ENABLE_FAULTS
    // (BUG) Incoherence between setCoordinateName() and getCoordinateName()
    //anEmptyC.setCoordinateName("alpha", 1);
    //anEmptyC.setCoordinateName("betta", 2);
    // (BUG) Incoherence between setCoordinateUnit() and getCoordinateUnit()
    //anEmptyC.setCoordinateUnit("ttt", 1);
    //anEmptyC.setCoordinateUnit("sss", 2);
//#else
    anEmptyC.setCoordinateName("alpha", 0);
    anEmptyC.setCoordinateName("betta", 1);

    anEmptyC.setCoordinateUnit("ttt", 0);
    anEmptyC.setCoordinateUnit("sss", 1);
//#endif
//#ifdef ENABLE_FORCED_FAILURES
    //CPPUNIT_FAIL("Incoherence between COORDINATE::setCoordinateName() and COORDINATE::getCoordinateName()");
    //CPPUNIT_FAIL("Incoherence between COORDINATE::setCoordinateUnit() and COORDINATE::getCoordinateUnit()");
//#endif

    int len = 10 * 2;
    double * cc = new double[len];
    for (int i = 0; i < len; i++) {
      cc[i] = (double)(i + 1);
    }
    anEmptyC.setCoordinates(MED_EN::MED_NO_INTERLACE, cc);

    CPPUNIT_ASSERT(anEmptyC.getCoordinateName(1) == "alpha");
    CPPUNIT_ASSERT(anEmptyC.getCoordinateName(2) == "betta");
    CPPUNIT_ASSERT(anEmptyC.getCoordinateUnit(1) == "ttt");
    CPPUNIT_ASSERT(anEmptyC.getCoordinateUnit(2) == "sss");
    for (int nn = 1; nn <= 10; nn++) {
      for (int aa = 1; aa <= 2; aa++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(nn + (aa - 1) * 10, anEmptyC.getCoordinate(nn, aa), 0.000001);
      }
    }

    CPPUNIT_ASSERT_THROW(anEmptyC.getCoordinate(0, 0), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(anEmptyC.getCoordinate(10, 10), MEDEXCEPTION);

    MEDARRAY<double> mcc (cc, 2, 10, MED_EN::MED_FULL_INTERLACE, false, false);
    anEmptyC.setCoordinates(&mcc, false);

    // coordinates names and units are not changed
    CPPUNIT_ASSERT(anEmptyC.getCoordinateName(1) == "alpha");
    CPPUNIT_ASSERT(anEmptyC.getCoordinateName(2) == "betta");
    CPPUNIT_ASSERT(anEmptyC.getCoordinateUnit(1) == "ttt");
    CPPUNIT_ASSERT(anEmptyC.getCoordinateUnit(2) == "sss");
    for (int nn = 1; nn <= 10; nn++) {
      for (int aa = 1; aa <= 2; aa++) {
        // coordinates changed
        CPPUNIT_ASSERT_DOUBLES_EQUAL((nn - 1) * 2 + aa, anEmptyC.getCoordinate(nn, aa), 0.000001);
      }
    }

    delete [] cc;

//#ifdef ENABLE_FAULTS
    // (BUG) Segmentation Fault or Hang up after anEmptyC and mcc destruction,
    // because array will be owned by two pointers (in mcc and in anEmptyC) after this call
    //???skl anEmptyC.setCoordinates(&mcc, true);
    // In other case (if we dynamically allocate mcc and do not free it) we will have memory leak.
//#endif
//#ifdef ENABLE_FORCED_FAILURES
    //CPPUNIT_FAIL("Bug in COORDINATE::setCoordinates() in shallow copy mode");
//#endif
  }

  ////////////
  // TEST 4 //
  ////////////
  {
    // COORDINATE(int SpaceDimension,const string * CoordinateName, const string * CoordinateUnit);
    string cnames [3] = {"al", "be", "ga"};
    string cunits [3] = {"kg", "mm", "s2"};
    COORDINATE anEmptyA (3, cnames, cunits);

    CPPUNIT_ASSERT(anEmptyA.getCoordinateName(1) == "al");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateName(2) == "be");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateName(3) == "ga");

    CPPUNIT_ASSERT(anEmptyA.getCoordinateUnit(1) == "kg");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateUnit(2) == "mm");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateUnit(3) == "s2");

    CPPUNIT_ASSERT_EQUAL(anEmptyA.getSpaceDimension(), 0);
    CPPUNIT_ASSERT_EQUAL(anEmptyA.getNumberOfNodes(),  0);

    MEDARRAY<double> mcc (3, 7, MED_EN::MED_NO_INTERLACE);
    { vector<double> val(3*7,0); // avoid usage of not initialized memory
      mcc.set(MED_EN::MED_NO_INTERLACE, &val[0]); }
    anEmptyA.setCoordinates(&mcc, false);

    CPPUNIT_ASSERT_EQUAL(anEmptyA.getSpaceDimension(), 3);
    CPPUNIT_ASSERT_EQUAL(anEmptyA.getNumberOfNodes(),  7);

    CPPUNIT_ASSERT(anEmptyA.getCoordinateName(1) == "al");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateName(2) == "be");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateName(3) == "ga");

    CPPUNIT_ASSERT(anEmptyA.getCoordinateUnit(1) == "kg");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateUnit(2) == "mm");
    CPPUNIT_ASSERT(anEmptyA.getCoordinateUnit(3) == "s2");

    CPPUNIT_ASSERT_THROW(anEmptyA.getCoordinate(-1, 0), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(anEmptyA.getCoordinate(10, 10), MEDEXCEPTION);

    // No COORDINATE::operator=, but this is compilable
    // good
    //COORDINATE anEmptyB;
    //COORDINATE anEmptyD (3, cnames, cunits);
    //anEmptyB = anEmptyD;
    //CPPUNIT_ASSERT(anEmptyB.getCoordinateName(1) == "al");

    // bad (assert fails)
    //COORDINATE anEmptyB;
    // Object, created in this line, is destructed right after it.
    //anEmptyB = COORDINATE(3, cnames, cunits);
    // Now a pointer _coordinateName inside anEmptyB points to a desallocated memory zone
    //CPPUNIT_ASSERT(anEmptyB.getCoordinateName(1) == "al");
  }
}
