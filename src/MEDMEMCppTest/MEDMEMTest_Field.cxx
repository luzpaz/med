//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_FieldConvert.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Support.hxx"
#include <MEDMEM_VtkMeshDriver.hxx>


#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #14,15: MEDMEMTest_Field.cxx
// Check methods from MEDMEM_Field.hxx, MEDMEM_FieldConvert.hxx

/*!
 *  Check methods (48), defined in MEDMEM_Field.hxx:
 *  class FIELD_ {
 *   (+)     FIELD_();
 *   (+)     FIELD_(const SUPPORT * Support, const int NumberOfComponents);
 *   (+)     FIELD_(const FIELD_ &m);
 *   (+)     virtual ~FIELD_();
 *   (+)     FIELD_& operator=(const FIELD_ &m);
 *
 *   (-)     virtual  void     rmDriver(int index=0);
 *   (-)     virtual   int     addDriver(driverTypes driverType,
 *                             const string & fileName="Default File Name.med",
 *                              const string & driverFieldName="Default Field Nam",
 *                              MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (-)     virtual  int      addDriver(GENDRIVER & driver);
 *
 *   (-)     virtual  void     read (const GENDRIVER &);
 *   (-)     virtual  void     read(int index=0);
 *   (-)     virtual  void     openAppend(void);
 *   (-)     virtual  void     write(const GENDRIVER &);
 *   (-)     virtual  void     write(int index=0, const string & driverName="");
 *   (-)     virtual  void     writeAppend(const GENDRIVER &);
 *   (-)     virtual  void     writeAppend(int index=0, const string & driverName="");
 *
 *   (+)     inline void     setName(const string Name);
 *   (+)     inline string   getName() const;
 *   (+)     inline void     setDescription(const string Description);
 *   (+)     inline string   getDescription() const;
 *   (+)     inline const SUPPORT * getSupport() const;
 *   (+)     inline void     setSupport(const SUPPORT * support);
 *   (+)     inline void     setNumberOfComponents(const int NumberOfComponents);
 *   (+)     inline int      getNumberOfComponents() const;
 *   (+)     inline void     setNumberOfValues(const int NumberOfValues);
 *   (+)     inline int      getNumberOfValues() const;
 *   (+)     inline void     setComponentsNames(const string * ComponentsNames);
 *   (+)     inline void     setComponentName(int i, const string ComponentName);
 *   (+)     inline const string * getComponentsNames() const;
 *   (+)     inline string   getComponentName(int i) const;
 *   (+)     inline void     setComponentsDescriptions(const string * ComponentsDescriptions);
 *   (+)     inline void     setComponentDescription(int i, const string ComponentDescription);
 *   (+)     inline const string * getComponentsDescriptions() const;
 *   (+)     inline string   getComponentDescription(int i) const;
 *   (+)     inline void     setComponentsUnits(const UNIT * ComponentsUnits);
 *   (+)     inline const UNIT *   getComponentsUnits() const;
 *   (+)     inline const UNIT *   getComponentUnit(int i) const;
 *   (+)     inline void     setMEDComponentsUnits(const string * MEDComponentsUnits);
 *   (+)     inline void     setMEDComponentUnit(int i, const string MEDComponentUnit);
 *   (+)     inline const string * getMEDComponentsUnits() const;
 *   (+)     inline string   getMEDComponentUnit(int i) const;
 *
 *   (+)     inline void     setIterationNumber(int IterationNumber);
 *   (+)     inline int      getIterationNumber() const;
 *   (+)     inline void     setTime(double Time);
 *   (+)     inline double   getTime() const;
 *   (+)     inline void     setOrderNumber(int OrderNumber);
 *   (+)     inline int      getOrderNumber() const;
 *
 *   (+)     inline MED_EN::med_type_champ getValueType () const;
 *   (+)     inline MED_EN::medModeSwitch  getInterlacingType() const;
 *   (-)     virtual inline bool getGaussPresence() const throw (MEDEXCEPTION);
 *  }
 *
 *  template <class T, class INTERLACING_TAG> class FIELD : public FIELD_ {
 *   (+)     FIELD();
 *   (+)     FIELD(const FIELD &m);
 *   (+)     FIELD(const SUPPORT * Support, const int NumberOfComponents) throw (MEDEXCEPTION);
 *   (+)     FIELD(driverTypes driverType,
 *                 const string & fileName, const string & fieldDriverName,
 *                 const int iterationNumber=-1, const int orderNumber=-1) throw (MEDEXCEPTION);
 *   (+) FIELD(const SUPPORT * Support, driverTypes driverType,
 *                 const string & fileName="", const string & fieldName="",
 *                 const int iterationNumber = -1, const int orderNumber = -1) throw (MEDEXCEPTION);
 *   (+)     ~FIELD();
 *   (+)     FIELD & operator=(const FIELD &m);
 *
 *   (+) const FIELD operator+(const FIELD& m) const;
 *   (+) const FIELD operator-(const FIELD& m) const;
 *   (+) const FIELD operator*(const FIELD& m) const;
 *   (+) const FIELD operator/(const FIELD& m) const;
 *   (+) const FIELD operator-() const;
 *   (+) FIELD& operator+=(const FIELD& m);
 *   (+) FIELD& operator-=(const FIELD& m);
 *   (+) FIELD& operator*=(const FIELD& m);
 *   (+) FIELD& operator/=(const FIELD& m);
 *
 *   (+) static FIELD* add(const FIELD& m, const FIELD& n);
 *   (+) static FIELD* addDeep(const FIELD& m, const FIELD& n);
 *   (+) static FIELD* sub(const FIELD& m, const FIELD& n);
 *   (+) static FIELD* subDeep(const FIELD& m, const FIELD& n);
 *   (+) static FIELD* mul(const FIELD& m, const FIELD& n);
 *   (+) static FIELD* mulDeep(const FIELD& m, const FIELD& n);
 *   (+) static FIELD* div(const FIELD& m, const FIELD& n);
 *   (+) static FIELD* divDeep(const FIELD& m, const FIELD& n);
 *
 *   (+)     double normMax() const throw (MEDEXCEPTION);
 *   (+)     double norm2() const throw (MEDEXCEPTION);
 *
 *   (+)     void   applyLin(T a, T b);
 *   (+)     template <T T_function(T)> void applyFunc();
 *   (+)     void applyPow(T scalar);
 *
 *   (+)     static FIELD* scalarProduct(const FIELD& m, const FIELD& n, bool deepCheck=false);
 *
 *   (+)     double normL2(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *   (+)     double normL2(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *   (+)     double normL1(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *   (+)     double normL1(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *
 *   (+)     FIELD* extract(const SUPPORT *subSupport) const throw (MEDEXCEPTION);
 *
 *   (EMPTY COMMENT, EMPTY IMPLEMENTATION!!!) void init ();
 *
 *   (+)     void rmDriver(int index=0);
 *   (+)     int  addDriver(driverTypes driverType,
 *                          const string & fileName="Default File Name.med",
 *                          const string & driverFieldName="Default Field Name",
 *                          MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (+)     int  addDriver(GENDRIVER & driver);
 *
 *   (+)     void allocValue(const int NumberOfComponents);
 *   (+)     void allocValue(const int NumberOfComponents, const int LengthValue);
 *   (+)     void deallocValue();
 *
 *   (+)     inline void read(int index=0);
 *   (+)     inline void read(const GENDRIVER & genDriver);
 *   (+)     inline void write(int index=0, const string & driverName = "");
 *   (+)     inline void write(const GENDRIVER &);
 *   (+)     inline void writeAppend(int index=0, const string & driverName = "");
 *   (+) inline void writeAppend(const GENDRIVER &);
 *
 *   (+)     inline MEDMEM_Array_  * getArray()        const throw (MEDEXCEPTION);
 *   (+)     inline ArrayGauss     * getArrayGauss()   const throw (MEDEXCEPTION);
 *   (+)     inline ArrayNoGauss   * getArrayNoGauss() const throw (MEDEXCEPTION);
 *   (+)     inline bool             getGaussPresence() const throw (MEDEXCEPTION);
 *
 *   (+)     inline int      getValueLength() const throw (MEDEXCEPTION);
 *   (+)     inline const T* getValue()       const throw (MEDEXCEPTION);
 *   (+)     inline const T* getRow(int i)    const throw (MEDEXCEPTION);
 *   (+)     inline const T* getColumn(int j) const throw (MEDEXCEPTION);
 *   (+)     inline T        getValueIJ(int i,int j) const throw (MEDEXCEPTION);
 *   (+)     inline T        getValueIJK(int i,int j,int k) const throw (MEDEXCEPTION);
 *   (+)     bool            getValueOnElement(int eltIdInSup,T* retValues) const throw (MEDEXCEPTION);
 *
 *   (+)     const int getNumberOfGeometricTypes() const throw (MEDEXCEPTION);
 *
 *   (+)     const GAUSS_LOCALIZATION<INTERLACING_TAG> & getGaussLocalization
 *                          (MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
 *   (+)     const GAUSS_LOCALIZATION<INTERLACING_TAG> * getGaussLocalizationPtr
 *                          (MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
 *   (+)     void setGaussLocalization(MED_EN::medGeometryElement geomElement,
 *                                     const GAUSS_LOCALIZATION<INTERLACING_TAG> & gaussloc);
 *   (+)     const int * getNumberOfGaussPoints() const throw (MEDEXCEPTION);
 *   (+)     const int   getNumberOfGaussPoints
 *                          (MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
 *   (+)     const int   getNbGaussI(int i) const throw (MEDEXCEPTION);
 *
 *   (+)     const int * getNumberOfElements() const throw (MEDEXCEPTION);
 *   (+)     const MED_EN::medGeometryElement * getGeometricTypes() const throw (MEDEXCEPTION);
 *   (+)     bool        isOnAllElements() const throw (MEDEXCEPTION);
 *
 *   (+)     inline void setArray(MEDMEM_Array_ *value) throw (MEDEXCEPTION);
 *   (+)     inline void setValue(T* value) throw (MEDEXCEPTION);
 *   (+)     inline void setRow(int i, T* value) throw (MEDEXCEPTION);
 *   (+)     inline void setColumn(int i, T* value) throw (MEDEXCEPTION);
 *   (+)     inline void setValueIJ(int i, int j, T value) throw (MEDEXCEPTION);
 *
 *   (NOT IMPLEMENTED!!!) void getVolume() const throw (MEDEXCEPTION);
 *   (NOT IMPLEMENTED!!!) void getArea() const throw (MEDEXCEPTION);
 *   (NOT IMPLEMENTED!!!) void getLength() const throw (MEDEXCEPTION);
 *   (NOT IMPLEMENTED!!!) void getNormal() const throw (MEDEXCEPTION);
 *   (NOT IMPLEMENTED!!!) void getBarycenter() const throw (MEDEXCEPTION);
 *
 *   (+)     void fillFromAnalytic(myFuncType f) throw (MEDEXCEPTION);
 *  }
 *
 *  Use code of test_operation_fieldint.cxx
 *              test_operation_fielddouble.cxx
 *              test_copie_field_.cxx
 *              test_copie_fieldT.cxx
 */
void compareField_(const FIELD_ * theField_1, const FIELD_ * theField_2, bool isFIELD, bool isValue)
{
  // name, description, support
  CPPUNIT_ASSERT_EQUAL(theField_1->getName(), theField_2->getName());
  CPPUNIT_ASSERT_EQUAL(theField_1->getDescription(), theField_2->getDescription());
  CPPUNIT_ASSERT_EQUAL(theField_1->getSupport(), theField_2->getSupport());

  // components information
  int aNbComps = theField_1->getNumberOfComponents();
  CPPUNIT_ASSERT_EQUAL(aNbComps, theField_2->getNumberOfComponents());

  for (int i = 1; i <= aNbComps; i++) {
    CPPUNIT_ASSERT_EQUAL(theField_1->getComponentName(i), theField_2->getComponentName(i));
    CPPUNIT_ASSERT_EQUAL(theField_1->getComponentDescription(i), theField_2->getComponentDescription(i));
    CPPUNIT_ASSERT_EQUAL(theField_1->getMEDComponentUnit(i), theField_2->getMEDComponentUnit(i));
  }

  // iteration information
  CPPUNIT_ASSERT_EQUAL(theField_1->getIterationNumber(), theField_2->getIterationNumber());
  CPPUNIT_ASSERT_EQUAL(theField_1->getOrderNumber(), theField_2->getOrderNumber());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(theField_1->getTime(), theField_2->getTime(), 0.0000001);

  // Value
  int nbOfValues = theField_1->getNumberOfValues();
  CPPUNIT_ASSERT_EQUAL(nbOfValues, theField_2->getNumberOfValues());

  if (isFIELD) {
    // Value type and Interlacing type
    CPPUNIT_ASSERT_EQUAL(theField_1->getValueType(), theField_2->getValueType());
    CPPUNIT_ASSERT_EQUAL(theField_1->getInterlacingType(), theField_2->getInterlacingType());

    // Gauss Presence
    if (isValue) {
      CPPUNIT_ASSERT_EQUAL(theField_1->getGaussPresence(), theField_2->getGaussPresence());
    }
    else {
      CPPUNIT_ASSERT_THROW(theField_1->getGaussPresence(), MEDEXCEPTION);
      CPPUNIT_ASSERT_THROW(theField_2->getGaussPresence(), MEDEXCEPTION);
    }
  }
  else {
    CPPUNIT_ASSERT_THROW(theField_1->getGaussPresence(), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theField_2->getGaussPresence(), MEDEXCEPTION);
  }
}

void checkField_(FIELD_ * theField_, const SUPPORT * theSupport,
                 MED_EN::med_type_champ theValueType,
                 MED_EN::medModeSwitch theInterlace)
{
  // name
  const string aFieldName = "a_name_of_a_field";
  theField_->setName(aFieldName);
  CPPUNIT_ASSERT_EQUAL(aFieldName, theField_->getName());

  // description
  const string aFieldDescr = "a_description_of_a_field";
  theField_->setDescription(aFieldDescr);
  CPPUNIT_ASSERT_EQUAL(aFieldDescr, theField_->getDescription());

  // support
  theField_->setSupport(theSupport);
  CPPUNIT_ASSERT(theField_->getSupport() == theSupport);

  // components information
  int aNbComps = 3;

  string aCompsNames[3] = {"Vx", "Vy", "Vz"};
  string aCompsDescs[3] = {"vitesse selon x", "vitesse selon y", "vitesse selon z"};
  string aCompsUnits[3] = {"m.s-1", "m.s-1", "m.s-1"};

  theField_->setNumberOfComponents(aNbComps);
  CPPUNIT_ASSERT_EQUAL(aNbComps, theField_->getNumberOfComponents());

  theField_->setComponentsNames(aCompsNames);

  //#ifdef ENABLE_FAULTS
  try {
    theField_->setNumberOfComponents(7);
    // Segmentation fault here because array of components names is not resized
    for (int i = 1; i <= 7; i++) {
      theField_->setComponentName(i, "AnyComponent");
    }
  }
  catch (MEDEXCEPTION& ex) {
    // Ok, it is good to have MEDEXCEPTION here
  }
  catch (...) {
    CPPUNIT_FAIL("Unknown exception cought");
  }
  // restore components names
  theField_->setNumberOfComponents(aNbComps);
  theField_->setComponentsNames(aCompsNames);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("FIELD_::_componentsNames bad management");
  //#endif

  theField_->setComponentsDescriptions(aCompsDescs);
  theField_->setMEDComponentsUnits(aCompsUnits);

  const string * aCompsNamesBack = theField_->getComponentsNames();
  const string * aCompsDescsBack = theField_->getComponentsDescriptions();
  const string * aCompsUnitsBack = theField_->getMEDComponentsUnits();
  for (int i = 1; i <= aNbComps; i++) {
    CPPUNIT_ASSERT_EQUAL(aCompsNamesBack[i-1], theField_->getComponentName(i));
    CPPUNIT_ASSERT_EQUAL(aCompsNamesBack[i-1], aCompsNames[i-1]);

    CPPUNIT_ASSERT_EQUAL(aCompsDescsBack[i-1], theField_->getComponentDescription(i));
    CPPUNIT_ASSERT_EQUAL(aCompsDescsBack[i-1], aCompsDescs[i-1]);

    CPPUNIT_ASSERT_EQUAL(aCompsUnitsBack[i-1], theField_->getMEDComponentUnit(i));
    CPPUNIT_ASSERT_EQUAL(aCompsUnitsBack[i-1], aCompsUnits[i-1]);
  }

  const string aCompName2 ("Name of second component");
  const string aCompDesc2 ("Description of second component");
  const string aCompUnit2 ("Unit of second MED component");

  theField_->setComponentName(2, aCompName2);
  theField_->setComponentDescription(2, aCompDesc2);
  theField_->setMEDComponentUnit(2, aCompUnit2);

  const string * aCompsNamesBack2 = theField_->getComponentsNames();
  const string * aCompsDescsBack2 = theField_->getComponentsDescriptions();
  const string * aCompsUnitsBack2 = theField_->getMEDComponentsUnits();

  CPPUNIT_ASSERT_EQUAL(aCompsNamesBack2[1], theField_->getComponentName(2));
  CPPUNIT_ASSERT_EQUAL(aCompsNamesBack2[1], aCompName2);

  CPPUNIT_ASSERT_EQUAL(aCompsDescsBack2[1], theField_->getComponentDescription(2));
  CPPUNIT_ASSERT_EQUAL(aCompsDescsBack2[1], aCompDesc2);

  CPPUNIT_ASSERT_EQUAL(aCompsUnitsBack2[1], theField_->getMEDComponentUnit(2));
  CPPUNIT_ASSERT_EQUAL(aCompsUnitsBack2[1], aCompUnit2);

  //#ifdef ENABLE_FAULTS
  // (BUG) No index checking
  // It's normal: performance reason
  CPPUNIT_ASSERT_THROW(theField_->setComponentName(0, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setComponentName(aNbComps + 1, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setComponentDescription(0, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setComponentDescription(aNbComps + 1, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setMEDComponentUnit(0, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setMEDComponentUnit(aNbComps + 1, "str"), MEDEXCEPTION);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("FIELD::setComponentXXX() does not check component index");
  //#endif

  // iteration information
  int anIterNumber = 10; // set value to MED_NOPDT if undefined (default)
  theField_->setIterationNumber(anIterNumber);
  CPPUNIT_ASSERT_EQUAL(anIterNumber, theField_->getIterationNumber());

  int anOrderNumber = 1; // set value to MED_NONOR if undefined (default)
  theField_->setOrderNumber(anOrderNumber);
  CPPUNIT_ASSERT_EQUAL(anOrderNumber, theField_->getOrderNumber());

  double aTime = 3.435678; // in second
  theField_->setTime(aTime);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(aTime, theField_->getTime(), 0.0000001);

  // Value
  int nbOfValues = 10;
  // dangerous method, because it does not reallocate values array
  theField_->setNumberOfValues(nbOfValues);
  CPPUNIT_ASSERT_EQUAL(nbOfValues, theField_->getNumberOfValues());

  // Value type and Interlacing type
  CPPUNIT_ASSERT_EQUAL(theValueType, theField_->getValueType());
  CPPUNIT_ASSERT_EQUAL(theInterlace, theField_->getInterlacingType());
}

template<class T, class INTERLACING_TAG>
void compareField(const FIELD<T, INTERLACING_TAG> * theField_1,
                  const FIELD<T, INTERLACING_TAG> * theField_2, bool isValue)
{
  // compare FIELD_ part
  compareField_(theField_1, theField_2, /*isFIELD = */true, isValue);

  // compare FIELD part
  // TO DO
}

template<class T, class INTERLACING_TAG>
void checkField (FIELD<T, INTERLACING_TAG> * theField, const SUPPORT * theSupport)
{
  // check FIELD_ part
  MED_EN::med_type_champ aValueType = SET_VALUE_TYPE<T>::_valueType;
  MED_EN::medModeSwitch  anInterlace = SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;
  checkField_(theField, theSupport, aValueType, anInterlace);

  // check FIELD part

  // filling by support charackteristics (NOT IMPLEMENTED METHODS!!!):
  // value type must be MED_REEL64 (i.e. a FIELD<double>) for these methods,
  // nb. of components must be equal 1 (for Volume, Area, Length) or
  // space dimension (for Normal, Barycenter, )
  {
    GMESH* aMesh = theSupport->getMesh();
    int spaceDim = 3;
    if (aMesh) spaceDim = aMesh->getSpaceDimension();
    theField->deallocValue();
    theField->allocValue(/*NumberOfComponents = */spaceDim + 1);

    //  0020142: [CEA 315] Unused function in MEDMEM::FIELD
    // getVolume() etc. does nothing
    //
//     CPPUNIT_ASSERT_THROW(theField->getVolume(), MEDEXCEPTION);
//     CPPUNIT_ASSERT_THROW(theField->getArea(), MEDEXCEPTION);
//     CPPUNIT_ASSERT_THROW(theField->getLength(), MEDEXCEPTION);
//     if (aMesh) {
//       CPPUNIT_ASSERT_THROW(theField->getNormal(), MEDEXCEPTION);
//       CPPUNIT_ASSERT_THROW(theField->getBarycenter(), MEDEXCEPTION);
//     }

    theField->deallocValue();
    theField->allocValue(/*NumberOfComponents = */1);
    //  0020142: [CEA 315] Unused function in MEDMEM::FIELD
    // getVolume() etc. does nothing
//     if (aValueType == MED_EN::MED_REEL64) {
//       CPPUNIT_ASSERT_NO_THROW(theField->getVolume());
//       CPPUNIT_ASSERT_NO_THROW(theField->getArea());
//       CPPUNIT_ASSERT_NO_THROW(theField->getLength());
//     }
//     else {
//       CPPUNIT_ASSERT_THROW(theField->getVolume(), MEDEXCEPTION);
//       CPPUNIT_ASSERT_THROW(theField->getArea(), MEDEXCEPTION);
//       CPPUNIT_ASSERT_THROW(theField->getLength(), MEDEXCEPTION);
//     }

    if (aMesh) {
      theField->deallocValue();
      theField->allocValue(/*NumberOfComponents = */spaceDim);
    //  0020142: [CEA 315] Unused function in MEDMEM::FIELD
    // getVolume() etc. does nothing
//       if (aValueType == MED_EN::MED_REEL64) {
//         CPPUNIT_ASSERT_NO_THROW(theField->getNormal());
//         CPPUNIT_ASSERT_NO_THROW(theField->getBarycenter());
//       }
//       else {
//         CPPUNIT_ASSERT_THROW(theField->getNormal(), MEDEXCEPTION);
//         CPPUNIT_ASSERT_THROW(theField->getBarycenter(), MEDEXCEPTION);
//       }
    }
  }

  // values
  theField->deallocValue();
  theField->allocValue(/*NumberOfComponents = */2);
  int nbElemSupport = theSupport->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  CPPUNIT_ASSERT_EQUAL(nbElemSupport, theField->getNumberOfValues());

  //#ifdef ENABLE_FAULTS
  // (BUG) FIELD::deallocValue() does not nullify _value pointer,
  // that is why there can be failures in other methods
  // (even if simply call deallocValue() two times)
  theField->deallocValue();
  CPPUNIT_ASSERT_THROW(theField->getGaussPresence(), MEDEXCEPTION);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("FIELD::deallocValue() does not nullify _value pointer");
  //#endif

  // copy constructor
  FIELD<T, INTERLACING_TAG> *aField_copy1= new FIELD<T, INTERLACING_TAG>(*theField);
  compareField(theField, aField_copy1, /*isValue = */false);
  //compareField(theField, &aField_copy1, /*isValue = */true);
  aField_copy1->removeReference();

  // operator=
  //#ifdef ENABLE_FAULTS
  // (BUG) This fails (Segmentation fault) if not set:
  // _componentsNames or _componentsDescriptions, or _componentsUnits, or _MEDComponentsUnits
  FIELD<T, INTERLACING_TAG> *aField_copy2=new FIELD<T, INTERLACING_TAG>();
  *aField_copy2 = *theField;
  compareField(theField, aField_copy2, /*isValue = */false);
  aField_copy2->removeReference();
  //compareField(theField, &aField_copy2, /*isValue = */true);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("FIELD_::operator=() fails if _componentsUnits is not set");
  //#endif
}

template<class T>
FIELD<T> * createFieldOnGroup(MESH* theMesh, const GROUP* theGroup,
                              const string theName, const string theDescr)
{
  FIELD<T> * aFieldOnGroup = new FIELD<T>(theGroup, /*NumberOfComponents = */2);

  aFieldOnGroup->setName(theName);
  aFieldOnGroup->setDescription(theDescr);

  string aCompsNames[2] = {"Pos", "Neg"};
  string aCompsDescs[2] = {"+", "-"};
  string aCompsUnits[2] = {"unit1", "unit2"};

  aFieldOnGroup->setComponentsNames(aCompsNames);
  aFieldOnGroup->setComponentsDescriptions(aCompsDescs);
  aFieldOnGroup->setMEDComponentsUnits(aCompsUnits);

  return aFieldOnGroup;
}

double plus13 (double val)
{
  return val + 13;
}

// function to calculate field values from coordinates of an element
// typedef void (*myFuncType)(const double * temp, T* output);
// size of temp array = space dim = 3
// size of output array = nb. comps = 2
void proj2d (const double * temp, double* output)
{
  // dimetric projection with coefficients:
  // 1.0 along Oy and Oz, 0.5 along Ox
  //
  //    ^ z (y_)
  //    |
  //    |
  //    .----> y (x_)
  //   /
  //  L x
  //
  // x_ = y - x * sqrt(2.) / 4.
  // y_ = z - x * sqrt(2.) / 4.

  double dx = temp[0] * std::sqrt(2.) / 4.;
  output[0] = temp[1] - dx;
  output[1] = temp[2] - dx;
}

void testDrivers()
{
  string filename_rd                  = getResourceFile("pointe.med");
  string filename_wr                  = makeTmpFile("myMedFieldfile.med", filename_rd);
  string filename_support_wr          = makeTmpFile("myMedSupportFiledfile.med");
  string filename22_rd                = getResourceFile("pointe.med");
  string filenamevtk_wr               = makeTmpFile("myMedFieldfile22.vtk");

  string fieldname_celldouble_rd      = "fieldcelldoublescalar";
  string fieldname_celldouble_wr      = fieldname_celldouble_rd + "_cpy";
  string fieldname_nodeint_rd         = "fieldnodeint";
  string fieldname_nodeint_wr         = fieldname_nodeint_rd + "_cpy";
  string fieldname_nodeint_wr1        = fieldname_nodeint_rd + "_cpy1";
  string meshname                     = "maa1";

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filenamevtk_wr);
  aRemover.Register(filename_support_wr);

  FIELD<int> *aInvalidField=new FIELD<int>();
  //must throw becase only VTK_DRIVER or MED_DRIVER may be specified as driverType for FIELD
  CPPUNIT_ASSERT_THROW(*aInvalidField = *(new FIELD<int>(NO_DRIVER, filename_rd, fieldname_nodeint_rd)),
                       MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aInvalidField = *(new FIELD<int>(GIBI_DRIVER, filename_rd, fieldname_nodeint_rd)),
                       MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aInvalidField = *(new FIELD<int>(PORFLOW_DRIVER, filename_rd, fieldname_nodeint_rd)),
                       MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aInvalidField = *(new FIELD<int>(ASCII_DRIVER, filename_rd, fieldname_nodeint_rd)),
                       MEDEXCEPTION);
  aInvalidField->removeReference();
  //////////////////
  //TestRead Part//
  //////////////////
  FIELD<double> *aField_1 = NULL;
  CPPUNIT_ASSERT_NO_THROW(aField_1 = new FIELD<double>(MED_DRIVER, filename_rd,
                                                       fieldname_celldouble_rd));

  //Test read(int index) method
  int IdDriver_rd = aField_1->addDriver(MED_DRIVER,filename_rd,fieldname_celldouble_rd);
  // TODO: throw if read for the second time
  // (BUG) Cannot open file, but file exist
  // EAP: no more pb with opening the file for the second time with "weaker" mode,
  // but why to re-read the field?
  //CPPUNIT_ASSERT_THROW(aField_1->read(IdDriver_rd),MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aField_1->read(IdDriver_rd));

  //Test read(GENDRIVER & genDriver) method
  FIELD<int> *aField_2 = new FIELD<int>();
  aField_2->setName(fieldname_nodeint_rd);
  {
    MED_FIELD_RDONLY_DRIVER<int> aMedRdFieldDriver;
    aMedRdFieldDriver.setFileName(filename_rd);
    aField_2->read( aMedRdFieldDriver );
  }
  //Test read(driverTypes driverType, const std::string & fileName);
  FIELD<double> * aField_3 = new FIELD<double>();
  aField_3->setName(fieldname_celldouble_rd);
  aField_3->read( MED_DRIVER, filename_rd);

  ///////////////////
  //Test Write Part//
  ///////////////////
  //int IdDriver;
  MESH *aMesh = new MESH(MED_DRIVER,filename_rd,meshname);
  SUPPORT *aSupport = new SUPPORT(aMesh, "aSupport",MED_CELL);
  FIELD<int> *aFieldSupport;
  //#ifdef ENABLE_FORCED_FAILURES  
  CPPUNIT_ASSERT_THROW(aFieldSupport = 
                       new FIELD<int>(aSupport, MED_DRIVER,filename_rd,
                                       fieldname_nodeint_rd), MEDMEM::MEDEXCEPTION);
  aSupport->removeReference();
  aSupport = new SUPPORT(aMesh, "aSupport",MED_NODE);
  CPPUNIT_ASSERT_NO_THROW(aFieldSupport = 
                          new FIELD<int>(aSupport, MED_DRIVER, filename_rd,
                                         fieldname_nodeint_rd));
  aFieldSupport->removeReference();

  //Test write(int index) method
  // Add drivers to FIELDs
  int IdDriver1 = aField_3->addDriver(MED_DRIVER,filename_wr,fieldname_celldouble_wr);

  //Trying call write(int index) method with incorrect index
  CPPUNIT_ASSERT_THROW(aField_3->write(IdDriver1+1),MEDEXCEPTION);

  //Write field to file
  aField_3->write(IdDriver1);

  CPPUNIT_ASSERT_NO_THROW(aField_3->rmDriver(IdDriver1));

  //Test write(const GENDRIVER &);
  {
    MED_FIELD_WRONLY_DRIVER<int> aMedWrFieldDriver;
    aMedWrFieldDriver.setFileName(filename_wr);

    aField_3->setName(fieldname_nodeint_wr);
    aField_3->write(aMedWrFieldDriver);
    FIELD<double> aField_3_RD;
    aField_3_RD.setName(fieldname_nodeint_wr);
    aField_3_RD.read(MED_DRIVER,filename_wr);
  }

  // Test write(driverTypes driverType, const std::string& filename)
  aField_3->setName(fieldname_nodeint_wr1);
  aField_3->write(MED_DRIVER,filename_wr);
  {
    FIELD<double> aField_3_RD;
    aField_3_RD.setName(fieldname_nodeint_wr1);
    aField_3_RD.read(MED_DRIVER,filename_wr);
  }

  //Test writeAppend(int index) method
  //Create a vtk file
  MESH * aMesh_1 = new MESH(MED_DRIVER,filename22_rd, meshname);
  aMesh_1->write(VTK_DRIVER, filenamevtk_wr);
  //Create a field
  FIELD<int> * aField_4 =
    new FIELD<int>(MED_DRIVER,filename22_rd,fieldname_nodeint_rd,-1,-1,aMesh_1);
  //Add Driver to a field
  int IdDriver2 = aField_4->addDriver(VTK_DRIVER, filenamevtk_wr ,fieldname_nodeint_wr);
  //Trying call writeAppend() method with incorrect index
  CPPUNIT_ASSERT_THROW(aField_4->writeAppend(IdDriver2+1,fieldname_nodeint_wr),MEDEXCEPTION);

  CPPUNIT_ASSERT_NO_THROW(aField_4->writeAppend(IdDriver2, fieldname_nodeint_wr));
  
  //Test writeAppend(const GENDRIVER &) method
  aField_4->setName(fieldname_nodeint_wr1);
  {
    VTK_FIELD_DRIVER<int> aVtkFieldDriver(filenamevtk_wr, aField_4);
    CPPUNIT_ASSERT_NO_THROW(aField_4->writeAppend(aVtkFieldDriver));
  }

  //Delete objects
  aField_1->removeReference();
  aField_2->removeReference();
  aField_3->removeReference();
  aField_4->removeReference();
  aMesh->removeReference();
  aMesh_1->removeReference();
  aSupport->removeReference();
}

void MEDMEMTest::testField()
{
  SUPPORT *anEmptySupport=new SUPPORT;
  ////////////////////
  // TEST 1: FIELD_ //
  ////////////////////
  FIELD_ *aField_=new FIELD_;

  // check set/get methods
  MED_EN::med_type_champ aValueType = MED_EN::MED_UNDEFINED_TYPE;
  MED_EN::medModeSwitch  anInterlace = MED_EN::MED_UNDEFINED_INTERLACE;
  checkField_(aField_, anEmptySupport, aValueType, anInterlace);

  // copy constructor
  // This fails (Segmentation fault) if not set:
  // _componentsNames or _componentsDescriptions, or _MEDComponentsUnits
  FIELD_ *aField_copy1=new FIELD_(*aField_);
  compareField_(aField_, aField_copy1, /*isFIELD = */false, /*isValue = */false);
  aField_copy1->removeReference();
  // operator=
  //#ifdef ENABLE_FAULTS
  // (BUG) This fails (Segmentation fault) if not set:
  // _componentsNames or _componentsDescriptions, or _componentsUnits, or _MEDComponentsUnits
  // (BUG) Code duplication with copyGlobalInfo(), called from copy constructor
  FIELD_ *aField_copy2=new FIELD_;
  *aField_copy2 = *aField_;
  compareField_(aField_, aField_copy2,/*isFIELD = */false, /*isValue = */false);
  aField_copy2->removeReference();
  aField_->removeReference();
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("FIELD_::operator=() fails if _componentsUnits is not set");
  //#endif

  // following test is commented since method
  // setTotalNumberOfElements() is removed.
  /*
  // construction on a given support
  {
    anEmptySupport.setTotalNumberOfElements(11);
    // CASE1:
    FIELD_ aField_case1 (&anEmptySupport, 10);
    // CASE2:
    // Invalid API usage
//     FIELD_ aField_case2;
//     aField_case2.setSupport(&anEmptySupport);
//     aField_case2.setNumberOfComponents(10);

// #ifdef ENABLE_FORCED_FAILURES
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("No correspondance between CASE1 and CASE2",
//                                  aField_case1.getNumberOfValues(),
//                                  aField_case2.getNumberOfValues());
// #endif
  }
  */

  ////////////////////////
  // TEST 2: FIELD<int> //
  ////////////////////////
  FIELD<int> *aFieldInt=new FIELD<int>();
  checkField(aFieldInt, anEmptySupport);
  aFieldInt->removeReference();
  anEmptySupport->removeReference();
  ////////////////////////////////////////
  // TEST 3: FIELD<double, NoInterlace> //
  ////////////////////////////////////////
  MESH * aMesh = MEDMEMTest_createTestMesh();
  const GROUP* aGroup = aMesh->getGroup(MED_EN::MED_FACE, 1);

  FIELD<double, NoInterlace> *aFieldDouble=new FIELD<double, NoInterlace>();
  checkField(aFieldDouble, aGroup);
  aFieldDouble->removeReference();
  //////////////////////////////////////////
  // TEST 4: FIELD<double, FullInterlace> //
  //////////////////////////////////////////
  FIELD<double> * aFieldOnGroup1 = createFieldOnGroup<double>(aMesh, aGroup, "Linear", "N");
  FIELD<double> * aFieldOnGroup2 = createFieldOnGroup<double>(aMesh, aGroup, "Quadratic", "N**2");

  int nbVals = aFieldOnGroup1->getNumberOfValues();
  CPPUNIT_ASSERT(nbVals);

  // numbers of elements in group,
  // they are needed in method FIELD::setValueIJ()
  const int *anElems = aGroup->getnumber()->getValue();
  double eucl1 = 0., eucl2 = 0.;

  for (int i = 1; i <= nbVals; i++) {
    aFieldOnGroup1->setValueIJ(anElems[i-1], 1, (double)i);
    aFieldOnGroup1->setValueIJ(anElems[i-1], 2, (double)(-i));

    aFieldOnGroup2->setValueIJ(anElems[i-1], 1, (double)i*i);
    aFieldOnGroup2->setValueIJ(anElems[i-1], 2, (double)(-i*i));

    eucl1 += 2. * i * i;
    eucl2 += 2. * i * i * i * i;
  }

  // out of bound (inexisting 33-th component of last element)
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->setValueIJ(anElems[nbVals-1], 33, 10.), MEDEXCEPTION);

  // normMax
  CPPUNIT_ASSERT_DOUBLES_EQUAL(nbVals, aFieldOnGroup1->normMax(), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(nbVals*nbVals, aFieldOnGroup2->normMax(), 0.000001);

  // norm2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(std::sqrt(eucl1), aFieldOnGroup1->norm2(), 0.000001); // 10.4881
  CPPUNIT_ASSERT_DOUBLES_EQUAL(std::sqrt(eucl2), aFieldOnGroup2->norm2(), 0.000001); // 44.2493

  // check getXXX methods
  CPPUNIT_ASSERT(!aFieldOnGroup1->getGaussPresence());
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getArrayGauss(), MEDEXCEPTION);
  MEDMEM_ArrayInterface<double, FullInterlace,NoGauss>::Array * anArrayNoGauss =
    aFieldOnGroup1->getArrayNoGauss();

  MEDMEM_Array_ * aMEDMEM_Array_ = aFieldOnGroup1->getArray();
  MEDMEM_ArrayInterface<double, FullInterlace,NoGauss>::Array * aMEDMEM_Array_conv =
    static_cast<MEDMEM_ArrayInterface<double, FullInterlace,NoGauss>::Array *>(aMEDMEM_Array_);

  const double * aValues = aFieldOnGroup1->getValue();

  // out of range
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getColumn(3), MEDEXCEPTION);
  // cannot get column in FullInterlace
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getColumn(1), MEDEXCEPTION);

  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)i   , aFieldOnGroup1->getValueIJK(anElems[i-1], 1, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)(-i), aFieldOnGroup1->getValueIJK(anElems[i-1], 2, 1), 0.000001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)i   , aValues[(i-1)*2 + 0], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)(-i), aValues[(i-1)*2 + 1], 0.000001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)i   , anArrayNoGauss->getIJ(i, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)(-i), anArrayNoGauss->getIJ(i, 2), 0.000001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)i   , aMEDMEM_Array_conv->getIJ(i, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)(-i), aMEDMEM_Array_conv->getIJ(i, 2), 0.000001);

    const double* row_i = aFieldOnGroup1->getRow(anElems[i-1]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)i   , row_i[0], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)(-i), row_i[1], 0.000001);

    double vals_i [2];
    aFieldOnGroup1->getValueOnElement(anElems[i-1], vals_i);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)i   , vals_i[0], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)(-i), vals_i[1], 0.000001);
  }

  // modify all values of aFieldOnGroup2 by formula a*x + b (a = 2, b = 3)
  aFieldOnGroup2->applyLin(2., 3.);
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3. + 2.*i*i, aFieldOnGroup2->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3. - 2.*i*i, aFieldOnGroup2->getValueIJ(anElems[i-1], 2), 0.000001);
  }

  // apply function plus13() to aFieldOnGroup1
  aFieldOnGroup1->applyFunc<plus13>();
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
  }

  // scalarProduct
  FIELD<double, FullInterlace> * aScalarProduct =
    FIELD<double, FullInterlace>::scalarProduct(*aFieldOnGroup1, *aFieldOnGroup2, /*deepCheck = */true);
  CPPUNIT_ASSERT_EQUAL(nbVals, aScalarProduct->getNumberOfValues());
  CPPUNIT_ASSERT_EQUAL(1, aScalarProduct->getNumberOfComponents());
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(78. + 4.*i*i*i, //(3. + 2.*i*i)*(13 + i) + (3. - 2.*i*i)*(13 - i)
                                 aScalarProduct->getValueIJ(anElems[i-1], 1), 0.000001);
  }

  // fillFromAnalytic
  aFieldOnGroup2->fillFromAnalytic(proj2d);

  double bary [3];
  double outp [2];
  const SUPPORT * aSupp = aFieldOnGroup2->getSupport();
  FIELD<double,FullInterlace> * barycenter = aMesh->getBarycenter(aSupp);
  for (int i = 1; i <= nbVals; i++) {
    bary[0] = barycenter->getValueIJ(anElems[i-1], 1);
    bary[1] = barycenter->getValueIJ(anElems[i-1], 2);
    bary[2] = barycenter->getValueIJ(anElems[i-1], 3);

    proj2d(bary, outp);

    //cout << "barycenter (" << bary[0] << ", " << bary[1] << ", " << bary[2] << ")" << endl;
    //cout << "proj2d     (" << outp[0] << ", " << outp[1] << ")" << endl;

    //bary (-0.666667,  0.666667, 0.666667) -> outp ( 0.902369, 0.902369)
    //bary ( 0.666667, -0.666667, 0.666667) -> outp (-0.902369, 0.430964)
    //bary ( 0.      ,  0.      , 2.      ) -> outp ( 0.      , 2.      )
    //bary ( 0.      ,  0.      , 3.      ) -> outp ( 0.      , 3.      )
    //bary (-1.      ,  0.      , 2.5     ) -> outp ( 0.353553, 2.85355 )

    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) in FIELD::fillFromAnalytic() in case of support, different from nodes:
    //       barycenterField in FullInterlace, but values extracted like from NoInterlace
    // TODO: fix MEDMEM_Field:3483 - code should depend on interlace
    CPPUNIT_ASSERT_DOUBLES_EQUAL(outp[0], aFieldOnGroup2->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(outp[1], aFieldOnGroup2->getValueIJ(anElems[i-1], 2), 0.000001);
    //#endif

    // currently it gives values, that are wrong:
    //aFieldOnGroup2 row1 ( 0.902369,  0.235702)
    //aFieldOnGroup2 row2 (-0.235702,  2.7643  )
    //aFieldOnGroup2 row3 (-0.235702, -1.2357  )
    //aFieldOnGroup2 row4 ( 1.7643  , -0.235702)
    //aFieldOnGroup2 row5 ( 0.235702,  2.7357  )
  }

  // info about support (Group1)
  CPPUNIT_ASSERT(!aFieldOnGroup1->isOnAllElements()); // because we build Group1 so
  int nbTypes = aFieldOnGroup1->getNumberOfGeometricTypes();
  //CPPUNIT_ASSERT(nbTypes);
  CPPUNIT_ASSERT_EQUAL(2, nbTypes);
  const int * nbElemsInEachType = aFieldOnGroup1->getNumberOfElements();
  const MED_EN::medGeometryElement * aGeomTypes = aFieldOnGroup1->getGeometricTypes();

  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_TRIA3, aGeomTypes[0]);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_QUAD4, aGeomTypes[1]);

  // GAUSS

  // now we have no gauss localization in aFieldOnGroup1
  CPPUNIT_ASSERT_EQUAL(1, aFieldOnGroup1->getNumberOfGaussPoints(MED_EN::MED_TRIA3));
  CPPUNIT_ASSERT_EQUAL(1, aFieldOnGroup1->getNumberOfGaussPoints(MED_EN::MED_QUAD4));
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getNumberOfGaussPoints(MED_EN::MED_TRIA6), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getNumberOfGaussPoints(), MEDEXCEPTION);

  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getGaussLocalization(MED_EN::MED_TRIA3), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getGaussLocalizationPtr(MED_EN::MED_TRIA3), MEDEXCEPTION);

  CPPUNIT_ASSERT_EQUAL(1, aFieldOnGroup1->getNbGaussI(anElems[0]));

  // set a gauss localization into aFieldOnGroup1
  double cooRef[6] = {1.,1., 2.,4., 3.,9.}; // xy xy xy
  double cooGauss[10] = {7.,7., 6.,6., 5.,5., 4.,3., 2.,1.}; // x1,y1  x2,y2  x3,y3  x4,y4  x5,y5
  double wg[5] = {1., 2., 3., 4., 5.};
  GAUSS_LOCALIZATION<> gl1 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef, cooGauss, wg);

  aFieldOnGroup1->setGaussLocalization(MED_EN::MED_TRIA3, gl1);

  // now we have a gauss localization for MED_TRIA3 type
  CPPUNIT_ASSERT_EQUAL(5, aFieldOnGroup1->getNumberOfGaussPoints(MED_EN::MED_TRIA3));
  CPPUNIT_ASSERT_EQUAL(1, aFieldOnGroup1->getNumberOfGaussPoints(MED_EN::MED_QUAD4));
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getNumberOfGaussPoints(MED_EN::MED_TRIA6), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getNumberOfGaussPoints(), MEDEXCEPTION);

  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getGaussLocalization(MED_EN::MED_QUAD4), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->getGaussLocalizationPtr(MED_EN::MED_QUAD4), MEDEXCEPTION);

  GAUSS_LOCALIZATION<> gl1Back = aFieldOnGroup1->getGaussLocalization(MED_EN::MED_TRIA3);
  const GAUSS_LOCALIZATION<> * gl1BackPtr = aFieldOnGroup1->getGaussLocalizationPtr(MED_EN::MED_TRIA3);

  CPPUNIT_ASSERT(gl1 == gl1Back);
  CPPUNIT_ASSERT(gl1 == *gl1BackPtr);

  CPPUNIT_ASSERT_EQUAL(1, aFieldOnGroup1->getNbGaussI(anElems[0]));

  // sub-support of Group1 on one (first) geometric type
  SUPPORT * aSubSupport1 = new SUPPORT(aMesh, "Sub-Support 1 of Group1", MED_EN::MED_FACE);
  aSubSupport1->setAll(false);

  int nbTypes1 = 1;
  int nbElemsInEachType1[1];
  nbElemsInEachType1[0] = nbElemsInEachType[0];
  int nbElems1 = nbElemsInEachType1[0];
  MED_EN::medGeometryElement aGeomTypes1[1];
  aGeomTypes1[0] = aGeomTypes[0];
  int * anElems1 = new int[nbElems1];
  for (int i = 0; i < nbElems1; i++) {
    anElems1[i] = anElems[i];
  }

  aSubSupport1->setpartial("Support for sub-field 1 on one type of elements",
                           nbTypes1, nbElems1, aGeomTypes1, nbElemsInEachType1, anElems1);

  // extract sub-field on aSubSupport1
  FIELD<double, FullInterlace> * aSubField1 = aFieldOnGroup1->extract(aSubSupport1);
  CPPUNIT_ASSERT_EQUAL(nbElems1 * /*NumberOfComponents = */2, aSubField1->getValueLength());

  // aSubField1:
  // elt\comp |  1 |  2
  //--------------------
  //  1       | 14 | 12
  //  2       | 15 | 11

  // check normL2() and normL1()
  FIELD<double>* anAreaField = aMesh->getArea(aSubSupport1);
  double area1 = anAreaField->getValueIJ(anElems1[0], 1);
  double area2 = anAreaField->getValueIJ(anElems1[1], 1);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2.44949, area1, 0.00001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2.44949, area2, 0.00001);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(210.5, aSubField1->normL2(1), 0.00001); // (14*14 + 15*15)/2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(132.5, aSubField1->normL2(2), 0.00001); // (12*12 + 11*11)/2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(343.0, aSubField1->normL2() , 0.00001); // 210.5 + 132.5

  CPPUNIT_ASSERT_DOUBLES_EQUAL(14.5, aSubField1->normL1(1), 0.00001); // (14 + 15)/2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(11.5, aSubField1->normL1(2), 0.00001); // (12 + 11)/2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(26.0, aSubField1->normL1() , 0.00001); // 14.5 + 11.5

  double aNewArea [2] = {1., 0.}; // only first element will be taken into account
  anAreaField->setValue(aNewArea);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(196.0, aSubField1->normL2(1, anAreaField), 0.00001); // 14*14/1
  CPPUNIT_ASSERT_DOUBLES_EQUAL(144.0, aSubField1->normL2(2, anAreaField), 0.00001); // 12*12/1
  CPPUNIT_ASSERT_DOUBLES_EQUAL(340.0, aSubField1->normL2(anAreaField) , 0.00001); // 196 + 144

  CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0, aSubField1->normL1(1, anAreaField), 0.00001); // 14/1
  CPPUNIT_ASSERT_DOUBLES_EQUAL(12.0, aSubField1->normL1(2, anAreaField), 0.00001); // 12/1
  CPPUNIT_ASSERT_DOUBLES_EQUAL(26.0, aSubField1->normL1(anAreaField) , 0.00001); // 14 + 12

  // check normL2() on nodal field (issue 0020120)
  {
    // read nodal field from pointe_import22.med
    string filename  = getResourceFile("pointe.med");
    string fieldname = "fieldnodedouble";
    string meshname  = "maa1";
    FIELD<double> *nodalField=new FIELD<double>( MED_DRIVER, filename, fieldname);
    MESH *mesh=new MESH( MED_DRIVER, filename, meshname);
    nodalField->getSupport()->setMesh( mesh );

    // make a field on the nodes of first cell only
    SUPPORT *oneCellNodesSup=new SUPPORT(mesh, "Sub-Support of nodes of 1 cell", MED_NODE);
    int NumberOfElements[] = { mesh->getTypes(MED_CELL)[0]%100 };
    medGeometryElement GeometricType[] = { MED_POINT1 };
    oneCellNodesSup->setpartial("Support for sub-field of one cell nodes",
                               /*NumberOfGeometricType=*/1,
                               /*TotalNumberOfElements=*/ *NumberOfElements,
                               GeometricType,
                               NumberOfElements,
                               /*NumberValue=*/ mesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,
                                                                      MED_CELL,MED_ALL_ELEMENTS ));
    FIELD<double, FullInterlace> * oneCellNodesField = nodalField->extract( oneCellNodesSup );
    oneCellNodesSup->removeReference();
    // compute normL2 by avarage nodal value on the cell

    SUPPORT *allCellsSupport=new SUPPORT( mesh );
    FIELD<double>* volumeField = mesh->getVolume(allCellsSupport);
    allCellsSupport->removeReference();
    // mdump output:
    // - Mailles de type MED_TETRA4 : 
    //  [     1 ] :          1          2          3          6
    // - Valeurs :
    // | 1.000000 | 1.000000 | 1.000000 | 2.000000 | 2.000000 | 2.000000 | ...
    const double cellVal = ( 1.000000 + 1.000000 + 1.000000 + 2.000000 ) / 4;
    const double vol = abs( volumeField->getValueIJ( 1, 1 ));
    const double norm = cellVal*cellVal*vol/vol; // v*v*vol/totVol;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(norm, oneCellNodesField->normL2(), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(norm, oneCellNodesField->normL2(1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(norm, oneCellNodesField->normL2(volumeField), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(norm, oneCellNodesField->normL2(1, volumeField), 0.000001);

    CPPUNIT_ASSERT_THROW(oneCellNodesField->normL1(), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(oneCellNodesField->normL1(1), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(oneCellNodesField->normL2(nodalField), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(oneCellNodesField->normL2(anAreaField), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(oneCellNodesField->normL2(aSubField1), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(oneCellNodesField->normL2(aFieldOnGroup1), MEDEXCEPTION);
    nodalField->removeReference();
    volumeField->removeReference();
    oneCellNodesField->removeReference();
    mesh->removeReference();
  }

  // double integral(SUPPORT*) - mantis issue 0020460:
  // [CEA 352] Integral calculus on all field or on a field subarea (groupe or family)
  {
    // make 2D grid 2x2 with steps along axis 1.0, 2.0
    const int dim = 2;
    double coord[3] = { 0.0, 1.0, 3.0 };
    vector< vector<double> > xyz_array( dim, vector<double>( coord, coord + 3 ));
    vector<string> coord_name( dim, "coord_name");
    vector<string> coord_unit( dim, "m");
    MESH *mesh= const_cast<MESH*>( GRID( xyz_array, coord_name, coord_unit ).convertInMESH());

    // make supports on the grid
    SUPPORT *supOnAll=new SUPPORT(mesh,"supOnAll");
    SUPPORT *sup123=new SUPPORT(mesh,"sup123"); int nbEl123[] = {3}, elems123[] = { 1,2,3 };
    SUPPORT *sup12=new SUPPORT(mesh,"sup12");  int nbEl12 [] = {2}, elems12 [] = { 1,2 };
    SUPPORT *sup34=new SUPPORT(mesh,"sup34");  int nbEl34 [] = {2}, elems34 [] = { 3,4 };
    const int nbGeomTypes = 1;
    const medGeometryElement * geomType = mesh->getTypes(MED_EN::MED_CELL);
    mesh->removeReference();
    sup123->setpartial("test", nbGeomTypes, *nbEl123, geomType, nbEl123, elems123 );
    sup12->setpartial("test", nbGeomTypes, *nbEl12 , geomType, nbEl12 , elems12 );
    sup34->setpartial("test", nbGeomTypes, *nbEl34 , geomType, nbEl34 , elems34 );

    // make vectorial fields with values of i-th elem { i, i*10, i*100 }
    const int nbComp = 3, nbElems = 4;
    const int mult[nbComp] = { 1, 10, 100 };
    FIELD<int,NoInterlaceByType> *fAllNoTy=new FIELD<int,NoInterlaceByType>(supOnAll, nbComp), *f12NoTy=new FIELD<int,NoInterlaceByType>(sup12, nbComp);
    FIELD<int,NoInterlace>       *fAllNo=new FIELD<int,NoInterlace>(supOnAll, nbComp), *f12No=new FIELD<int,NoInterlace>(sup12, nbComp);
    FIELD<int,FullInterlace>     *fAllFull=new FIELD<int,FullInterlace>(supOnAll, nbComp), *f12Full=new FIELD<int,FullInterlace>(sup12, nbComp);
    supOnAll->removeReference();
    int i, j;
    for ( i = 1; i <= nbElems; ++i )
      for ( j = 1; j <= nbComp; ++j )
        {
          fAllFull->setValueIJ( i, j, i * mult[j-1]);
          fAllNoTy->setValueIJ( i, j, i * mult[j-1]);
          fAllNo  ->setValueIJ( i, j, i * mult[j-1]);
          if ( i < 3 )
            {
              f12Full->setValueIJ( i, j, i * mult[j-1]);
              f12NoTy->setValueIJ( i, j, i * mult[j-1]);
              f12No  ->setValueIJ( i, j, i * mult[j-1]);
            }
        }
    // Test
    double preci = 1e-18, integral;
    // Integral = SUM( area * (i*1 + i*10 + i*100)) == 111 * SUM( area * i )
    // elem area: { 1, 2, 2, 4 }
    integral = 111*( 1*1 + 2*2 + 2*3 + 4*4 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllNoTy->integral(), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllNo  ->integral(), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllFull->integral(), preci );
    integral = 111*( 1*1 + 2*2 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12NoTy->integral(), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12No  ->integral(), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12Full->integral(), preci );

    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllNoTy->integral(sup12), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllNo  ->integral(sup12), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllFull->integral(sup12), preci );

    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12NoTy->integral(sup12), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12No  ->integral(sup12), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12Full->integral(sup12), preci );
    sup12->removeReference();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12NoTy->integral(sup123), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12No  ->integral(sup123), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12Full->integral(sup123), preci );
    integral = 111*( 1*1 + 2*2 + 2*3 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllNoTy->integral(sup123), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllNo  ->integral(sup123), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, fAllFull->integral(sup123), preci );
    fAllNoTy->removeReference();
    fAllNo->removeReference();
    sup123->removeReference();
    fAllFull->removeReference();
    integral = 0;
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12NoTy->integral(sup34), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12No  ->integral(sup34), preci );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( integral, f12Full->integral(sup34), preci );
    sup34->removeReference();
    f12NoTy->removeReference();
    f12No->removeReference();
    f12Full->removeReference();
  }

  // applyPow
  aSubField1->applyPow(2.);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(196., aSubField1->getValueIJ(anElems1[0], 1), 0.000001); // 14*14
  CPPUNIT_ASSERT_DOUBLES_EQUAL(144., aSubField1->getValueIJ(anElems1[0], 2), 0.000001); // 12*12
  CPPUNIT_ASSERT_DOUBLES_EQUAL(225., aSubField1->getValueIJ(anElems1[1], 1), 0.000001); // 15*15
  CPPUNIT_ASSERT_DOUBLES_EQUAL(121., aSubField1->getValueIJ(anElems1[1], 2), 0.000001); // 11*11

  // setArray (NoGauss)
  MEDMEM_ArrayInterface<double,FullInterlace,NoGauss>::Array * aNewArrayNoGauss =
    new MEDMEM_ArrayInterface<double,FullInterlace,NoGauss>::Array(/*dim*/2, /*nbelem*/2);
  aNewArrayNoGauss->setIJ(1, 1, 4.);
  aNewArrayNoGauss->setIJ(1, 2, 2.);
  aNewArrayNoGauss->setIJ(2, 1, 5.);
  aNewArrayNoGauss->setIJ(2, 2, 1.);
  aSubField1->setArray(aNewArrayNoGauss);
  // no need to delete aNewArrayNoGauss, because it will be deleted
  // in destructor or in deallocValue() method of aSubField1

  CPPUNIT_ASSERT_DOUBLES_EQUAL(4., aSubField1->getValueIJ(anElems1[0], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2., aSubField1->getValueIJ(anElems1[0], 2), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(5., aSubField1->getValueIJ(anElems1[1], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1., aSubField1->getValueIJ(anElems1[1], 2), 0.000001);

  // setRow
  double row[2] = {-1., -3.};
  aSubField1->setRow(anElems1[0], row);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-1., aSubField1->getValueIJ(anElems1[0], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-3., aSubField1->getValueIJ(anElems1[0], 2), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 5., aSubField1->getValueIJ(anElems1[1], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., aSubField1->getValueIJ(anElems1[1], 2), 0.000001);
  // out of range
  CPPUNIT_ASSERT_THROW(aSubField1->setRow(3, row), MEDEXCEPTION);

  // setColumn
  double col[2] = {-7., -9.};
  aSubField1->setColumn(1, col);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-7., aSubField1->getValueIJ(anElems1[0], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-3., aSubField1->getValueIJ(anElems1[0], 2), 0.000001);
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) in MEDMEM_Array::setColumn()
  // WAIT FOR DECISION
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-9., aSubField1->getValueIJ(anElems1[1], 1), 0.000001);
  //#endif
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., aSubField1->getValueIJ(anElems1[1], 2), 0.000001);
  // out of range
  CPPUNIT_ASSERT_THROW(aSubField1->setColumn(3, col), MEDEXCEPTION);

  // setArray (Gauss)
  {
    int nbelgeoc[2] = {1, 3}; // 3 - 1 = two elements for the first (and the only) type
    int nbgaussgeo[2] = {0, 1}; // one gauss point per each element
    MEDMEM_ArrayInterface<double,FullInterlace,Gauss>::Array * aNewArrayGauss =
      new MEDMEM_ArrayInterface<double,FullInterlace,Gauss>::Array
      (/*dim*/2, /*nbelem*/2, /*nbtypegeo*/1, /*nbelgeoc*/nbelgeoc, /*nbgaussgeo*/nbgaussgeo);

    //#ifdef ENABLE_FAULTS
    aNewArrayGauss->setIJ(1, 1, -4.);
    aNewArrayGauss->setIJ(1, 2, -2.);
    aNewArrayGauss->setIJ(2, 1, -5.);
    aNewArrayGauss->setIJ(2, 2, -1.);
    //#endif
    //#ifdef ENABLE_FORCED_FAILURES
    // ? (BUG) in FullInterlaceGaussPolicy::getIndex(int i,int j)
    // FullInterlaceGaussPolicy::getIndex(2,2) returns 4!!!
    //CPPUNIT_FAIL("? Bug in FullInterlaceGaussPolicy::getIndex(int i,int j) ?");
    //#endif

    aNewArrayGauss->setIJK(1, 1, 1, -4.);
    aNewArrayGauss->setIJK(1, 2, 1, -2.);
    aNewArrayGauss->setIJK(2, 1, 1, -5.);
    aNewArrayGauss->setIJK(2, 2, 1, -1.);

    aSubField1->setArray(aNewArrayGauss);
    // no need to delete aNewArrayGauss, because it will be deleted
    // in destructor or in deallocValue() method of aSubField1

    //#ifdef ENABLE_FAULTS
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4., aSubField1->getValueIJ(anElems1[0], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-2., aSubField1->getValueIJ(anElems1[0], 2), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-5., aSubField1->getValueIJ(anElems1[1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1., aSubField1->getValueIJ(anElems1[1], 2), 0.000001);
    //#endif
    //#ifdef ENABLE_FORCED_FAILURES
    // ? (BUG) in FullInterlaceGaussPolicy::getIndex(int i,int j)
    // Must be   : return _G[i-1]-1 + (j-1);
    // Instead of: return _G[i-1]-1 + (j-1)*_dim;
    // TODO: THINK YOUR-SELF
    //CPPUNIT_FAIL("? Bug in FullInterlaceGaussPolicy::getIndex(int i,int j) ?");
    //#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4., aSubField1->getValueIJK(anElems1[0], 1, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-2., aSubField1->getValueIJK(anElems1[0], 2, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-5., aSubField1->getValueIJK(anElems1[1], 1, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1., aSubField1->getValueIJK(anElems1[1], 2, 1), 0.000001);
  }

  // alloc/dealloc; compatibility of new size with support
  try {
    aSubField1->deallocValue();
    aSubField1->allocValue(/*NumberOfComponents*/2, /*LengthValue*/5);
    //#ifdef ENABLE_FAULTS
    // (BUG) No compatibility between Support and allocated value
    //aSubField1->normL1();
    CPPUNIT_ASSERT_THROW(aSubField1->normL1(),MEDEXCEPTION);
    //#endif
    //#ifdef ENABLE_FORCED_FAILURES
    // TODO: FIX to throw an EXCEPTION
    //    CPPUNIT_FAIL("Error: no compatibility between Support and allocated value");
    //#endif
  }
  catch (MEDEXCEPTION & ex) {
    // normal behaviour
  }
  catch (...) {
    CPPUNIT_FAIL("Error: no compatibility between Support and allocated value");
  }

  // check that aFieldOnGroup1 is not changed after aSubField1 modifications
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
  }

  // reset aFieldOnGroup2 values for simple control of operators results
  for (int i = 1; i <= nbVals; i++) {
    aFieldOnGroup2->setValueIJ(anElems[i-1], 1, i*i);
    aFieldOnGroup2->setValueIJ(anElems[i-1], 2, -i*i);
  }

  int len = aFieldOnGroup1->getValueLength();
  const double * val1 = aFieldOnGroup1->getValue();
  const double * val2 = aFieldOnGroup2->getValue();
  const double * val_res;

  // operators and add, sub, mul, div

  // +
  FIELD<double> *aSum = *aFieldOnGroup1 + *aFieldOnGroup2;
  aSum->setName(aFieldOnGroup1->getName());
  aSum->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aSum, true, true);
  val_res = aSum->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val2[i], val_res[i], 0.000001);
  }
  aSum->removeReference();
  // -
  FIELD<double> *aDifference = *aFieldOnGroup1 - *aFieldOnGroup2;
  aDifference->setName(aFieldOnGroup1->getName());
  aDifference->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aDifference, true, true);
  val_res = aDifference->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] - val2[i], val_res[i], 0.000001);
  }
  aDifference->removeReference();
  // - (unary)
  FIELD<double> *aNegative = - *aFieldOnGroup1;
  aNegative->setName(aFieldOnGroup1->getName());
  aNegative->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aNegative, true, true);
  val_res = aNegative->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(- val1[i], val_res[i], 0.000001);
  }
  aNegative->removeReference();
  // *
  FIELD<double> *aProduct = (*aFieldOnGroup1) * (*aFieldOnGroup2);
  aProduct->setName(aFieldOnGroup1->getName());
  aProduct->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aProduct, true, true);
  val_res = aProduct->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] * val2[i], val_res[i], 0.000001);
  }
  aProduct->removeReference();
  // /
  FIELD<double> *aQuotient = *aFieldOnGroup1 / *aFieldOnGroup2;
  aQuotient->setName(aFieldOnGroup1->getName());
  aQuotient->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aQuotient, true, true);
  val_res = aQuotient->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] / val2[i], val_res[i], 0.000001);
  }
  aQuotient->removeReference();
  double val22 = aFieldOnGroup2->getValueIJ(anElems[2], 2);
  aFieldOnGroup2->setValueIJ(anElems[2], 2, 0.);

  CPPUNIT_ASSERT_THROW((*aFieldOnGroup1 / *aFieldOnGroup2), MEDEXCEPTION);
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) is it up to user to control validity of data to avoid division on zero?
  // YES: USER SHOULD CARE OF IT
  //CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 /= *aFieldOnGroup2, MEDEXCEPTION);
  //#endif
  CPPUNIT_ASSERT_THROW(FIELD<double>::div(*aFieldOnGroup1, *aFieldOnGroup2)->removeReference(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup2)->removeReference(), MEDEXCEPTION);

  // restore value
  aFieldOnGroup2->setValueIJ(anElems[2], 2, val22);

  // restore values
  for (int i = 1; i <= nbVals; i++) {
    aFieldOnGroup1->setValueIJ(anElems[i-1], 1, 13 + i);
    aFieldOnGroup1->setValueIJ(anElems[i-1], 2, 13 - i);
  }

  // static methods
  FIELD<double> * aPtr;

  // add
  aPtr = FIELD<double>::add(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // sub
  aPtr = FIELD<double>::sub(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] - val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // mul
  aPtr = FIELD<double>::mul(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] * val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // div
  aPtr = FIELD<double>::div(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] / val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // addDeep
  aPtr = FIELD<double>::addDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // subDeep
  aPtr = FIELD<double>::subDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] - val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // mulDeep
  aPtr = FIELD<double>::mulDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] * val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // divDeep
  aPtr = FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] / val2[i], val_res[i], 0.000001);
  }
  aPtr->removeReference();

  // +=
  *aFieldOnGroup1 += *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i + i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i - i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
  }

  // -=
  *aFieldOnGroup1 -= *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
  }

  // *=
  *aFieldOnGroup1 *= *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL( (13 + i)*i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-(13 - i)*i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
  }

  // /=
  *aFieldOnGroup1 /= *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
  }

  // check case of different operands: support
  MESH * aMeshOneMore = MEDMEMTest_createTestMesh();
  const GROUP* aGroupOneMore = aMeshOneMore->getGroup(MED_EN::MED_FACE, 1);
  FIELD<double> * aFieldOnGroup3 =
    createFieldOnGroup<double>(aMeshOneMore, aGroupOneMore, "Test_Diff_Mesh", "test");
  for (int i = 1; i <= nbVals; i++) {
    aFieldOnGroup3->setValueIJ(anElems[i-1], 1, 2*i);
    aFieldOnGroup3->setValueIJ(anElems[i-1], 2, 3*i);
  }
  const double * val3 = aFieldOnGroup3->getValue();

  //CPPUNIT_ASSERT_NO_THROW();
  try {
    aPtr = FIELD<double>::addDeep(*aFieldOnGroup1, *aFieldOnGroup3);
    aPtr->setName(aFieldOnGroup1->getName());
    aPtr->setDescription(aFieldOnGroup1->getDescription());
    compareField_(aFieldOnGroup1, aPtr, true, true);
    val_res = aPtr->getValue();
    for (int i = 0; i < len; i++) {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val3[i], val_res[i], 0.000001);
    }
    aPtr->removeReference();

    aPtr = FIELD<double>::subDeep(*aFieldOnGroup1, *aFieldOnGroup3);
    aPtr->removeReference();
    aPtr = FIELD<double>::mulDeep(*aFieldOnGroup1, *aFieldOnGroup3);
    aPtr->removeReference();
    aPtr = FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup3);
    aPtr->removeReference();
  }
  catch (MEDEXCEPTION & ex) {
    CPPUNIT_FAIL(ex.what());
  }
  catch (...) {
    CPPUNIT_FAIL("Unknown exception in FIELD::xxxDeep()");
  }

  CPPUNIT_ASSERT_THROW(FIELD<double>::add(*aFieldOnGroup1, *aFieldOnGroup3), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::sub(*aFieldOnGroup1, *aFieldOnGroup3), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::mul(*aFieldOnGroup1, *aFieldOnGroup3), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::div(*aFieldOnGroup1, *aFieldOnGroup3), MEDEXCEPTION);

  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 + *aFieldOnGroup3, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 - *aFieldOnGroup3, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 * *aFieldOnGroup3, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 / *aFieldOnGroup3, MEDEXCEPTION);

  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 += *aFieldOnGroup3, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 -= *aFieldOnGroup3, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 *= *aFieldOnGroup3, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 /= *aFieldOnGroup3, MEDEXCEPTION);

  // check case of different operands: MEDComponentsUnits
  aFieldOnGroup1->setMEDComponentUnit(1, "unit3");

  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 + *aFieldOnGroup2, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 - *aFieldOnGroup2, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 += *aFieldOnGroup2, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 -= *aFieldOnGroup2, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::add(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::sub(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::addDeep(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::subDeep(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);

  //CPPUNIT_ASSERT_NO_THROW();
  try {
    aPtr = FIELD<double>::mul(*aFieldOnGroup1, *aFieldOnGroup2);
    aPtr->removeReference();
    aPtr = FIELD<double>::div(*aFieldOnGroup1, *aFieldOnGroup2);
    aPtr->removeReference();
    aPtr = FIELD<double>::mulDeep(*aFieldOnGroup1, *aFieldOnGroup2);
    aPtr->removeReference();
    aPtr = FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup2);
    aPtr->removeReference();

    *aFieldOnGroup1 *= *aFieldOnGroup2;
    *aFieldOnGroup1 /= *aFieldOnGroup2;

    FIELD<double> *aPr = *aFieldOnGroup1 * *aFieldOnGroup2;
    FIELD<double> *aQu = *aFieldOnGroup1 / *aFieldOnGroup2;
    aPr->removeReference();
    aQu->removeReference();
  }
  catch (MEDEXCEPTION & ex) {
    CPPUNIT_FAIL(ex.what());
  }
  catch (...) {
    CPPUNIT_FAIL("Unknown exception");
  }

  // restore MED units
  aFieldOnGroup1->setMEDComponentUnit(1, "unit1");

  // check case of different operands: valueType
  //FIELD<int> * aFieldOnGroup4 =
  //  createFieldOnGroup<int>(aMeshOneMore, aGroupOneMore, "Test_Diff_Mesh", "test");
  //
  //CPPUNIT_ASSERT_THROW(FIELD<double>::add(*aFieldOnGroup4, *aFieldOnGroup3), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(*aFieldOnGroup4 - *aFieldOnGroup3, MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(*aFieldOnGroup4 *= *aFieldOnGroup3, MEDEXCEPTION);
  //delete aFieldOnGroup4;

  // check case of different operands: numberOfComponents
  //#ifdef ENABLE_FAULTS
  // (BUG) Cannot allocate value of higher dimension because of _componentsTypes reinitialization
  aFieldOnGroup1->deallocValue();
  //CPPUNIT_ASSERT_THROW(aFieldOnGroup1->allocValue(/*dim*/5), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aFieldOnGroup1->allocValue(/*dim*/5));
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  // YES THERE MUST BE AN EXCEPTION
  //   CPPUNIT_FAIL("Segmentation fault on attempt to allocate value of higher dimension."
  //                " Must be MEDEXCEPTION instead. And on attempt to change nb.components"
  //                " must be the same behaviour.");
  //#endif
  aFieldOnGroup1->setNumberOfComponents(5);

  CPPUNIT_ASSERT_THROW(FIELD<double>::sub(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 * *aFieldOnGroup2, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 /= *aFieldOnGroup2, MEDEXCEPTION);

  // check case of different operands: numberOfValues
  aFieldOnGroup1->deallocValue();
  aFieldOnGroup1->allocValue(2, nbVals + 1);
  // be carefull: aFieldOnGroup1 reallocated and contains random values

  CPPUNIT_ASSERT_THROW(FIELD<double>::mul(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 / *aFieldOnGroup2, MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 += *aFieldOnGroup2, MEDEXCEPTION);

  // restore aFieldOnGroup1
  aFieldOnGroup1->deallocValue();
  aFieldOnGroup1->allocValue(2, nbVals);
  // be carefull: aFieldOnGroup1 reallocated and contains random values

  aSubSupport1->removeReference();
  delete [] anElems1;

  aScalarProduct->removeReference();
  aSubField1->removeReference();
  anAreaField->removeReference();
  barycenter->removeReference();
  aFieldOnGroup1->removeReference();
  aFieldOnGroup2->removeReference();
  aFieldOnGroup3->removeReference();

  aMesh->removeReference();
  aMeshOneMore->removeReference();

  /////////////////////
  // TEST 5: Drivers //
  /////////////////////
  testDrivers();
}

/*!
 *  Check methods (2), defined in MEDMEM_FieldConvert.hxx:
 *  (+) template <class T> FIELD<T,FullInterlace> * FieldConvert(const FIELD<T,NoInterlace> & field);
 *  (+) template <class T> FIELD<T,NoInterlace> * FieldConvert(const FIELD<T,FullInterlace> & field);
 */
void MEDMEMTest::testFieldConvert()
{
  // create an empty integer field 2x10
  FIELD<int, FullInterlace> * aField_FING = new FIELD<int, FullInterlace>();

  aField_FING->setName("Field_FING");
  aField_FING->setDescription("Field full interlace no gauss");

  aField_FING->setNumberOfComponents(2);
  aField_FING->setNumberOfValues(10);

  string aCompsNames[2] = {"Pos", "Neg"};
  string aCompsDescs[2] = {"+", "-"};
  string aMEDCompsUnits[2] = {"unit1", "unit2"};
  UNIT   aCompsUnits[2];

  aCompsUnits[0] = UNIT("u1", "descr1");
  aCompsUnits[1] = UNIT("u2", "descr2");

  aField_FING->setComponentsNames(aCompsNames);
  aField_FING->setComponentsDescriptions(aCompsDescs);
  aField_FING->setMEDComponentsUnits(aMEDCompsUnits);
  aField_FING->setComponentsUnits(aCompsUnits);

  // check UNITs (for testField())
  const UNIT * aCompsUnitsBack = aField_FING->getComponentsUnits();
  CPPUNIT_ASSERT(aCompsUnits[0].getName() == aCompsUnitsBack[0].getName());
  CPPUNIT_ASSERT(aCompsUnits[1].getName() == aCompsUnitsBack[1].getName());

  const UNIT * aCompUnitBack1 = aField_FING->getComponentUnit(1);
  const UNIT * aCompUnitBack2 = aField_FING->getComponentUnit(2);
  CPPUNIT_ASSERT(aCompsUnits[0].getName() == aCompUnitBack1->getName());
  CPPUNIT_ASSERT(aCompsUnits[1].getName() == aCompUnitBack2->getName());

  // create one more field by copy
  FIELD<int, FullInterlace> * aField_FIGG = new FIELD<int, FullInterlace>(*aField_FING);

  // values
  int values_FING[20] = { 7,- 7, 14,-14, 21,-21, 28,-28, 35,-35,
                          42,-42, 49,-49, 56,-56, 63,-63, 70,-70};

  /////////////////////
  // TEST 1: NoGauss //
  /////////////////////

  MEDMEM_ArrayInterface<int,FullInterlace,NoGauss>::Array * anArray_FING =
    new MEDMEM_ArrayInterface<int,FullInterlace,NoGauss>::Array
    (values_FING, /*dim*/2, /*nbelem*/10, /*shallowCopy*/false, /*ownershipOfValues*/false);
  aField_FING->setArray(anArray_FING);
  // no need to delete anArray_FING, because it will be deleted in destructor of aField_FING

  // 1. FullInterlace -> NoInterlace
  FIELD<int, NoInterlace> * aField_NING = FieldConvert(*aField_FING);
  const int * values_NING = aField_NING->getValue();

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 2; j++) {
      CPPUNIT_ASSERT_EQUAL(values_FING[2*i + j], values_NING[10*j + i]);
    }
  }

  // 2. NoInterlace -> FullInterlace
  FIELD<int, FullInterlace> * aField_FING_conv = FieldConvert(*aField_NING);
  const int * values_FING_conv = aField_FING_conv->getValue();

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 2; j++) {
      CPPUNIT_ASSERT_EQUAL(values_FING_conv[2*i + j], values_FING[2*i + j]);
      CPPUNIT_ASSERT_EQUAL(values_FING_conv[2*i + j], values_NING[10*j + i]);
    }
  }

  aField_FING->removeReference();
  aField_NING->removeReference();
  aField_FING_conv->removeReference();

  ///////////////////
  // TEST 2: Gauss //
  ///////////////////
  int nbelgeoc[2] = {1, 11};
  int nbgaussgeo[2] = {-1, 1};
  MEDMEM_ArrayInterface<int,FullInterlace,Gauss>::Array * anArray_FIGG =
    new MEDMEM_ArrayInterface<int,FullInterlace,Gauss>::Array
    (values_FING, /*dim*/2, /*nbelem*/10, /*nbtypegeo*/1, /*nbelgeoc*/nbelgeoc,
     /*nbgaussgeo*/nbgaussgeo, /*shallowCopy*/false, /*ownershipOfValues*/false);
  aField_FIGG->setArray(anArray_FIGG);
  // no need to delete anArray_FIGG, because it will be deleted in destructor of aField_FIGG

  // 1. FullInterlace -> NoInterlace
  FIELD<int, NoInterlace> * aField_NIGG = FieldConvert(*aField_FIGG);
  const int * values_NIGG = aField_NIGG->getValue();

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 2; j++) {
      CPPUNIT_ASSERT_EQUAL(values_FING[2*i + j], values_NIGG[10*j + i]);
    }
  }

  // 2. NoInterlace -> FullInterlace
  FIELD<int, FullInterlace> * aField_FIGG_conv = FieldConvert(*aField_NIGG);
  const int * values_FIGG_conv = aField_FIGG_conv->getValue();

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 2; j++) {
      CPPUNIT_ASSERT_EQUAL(values_FIGG_conv[2*i + j], values_FING[2*i + j]);
      CPPUNIT_ASSERT_EQUAL(values_FIGG_conv[2*i + j], values_NIGG[10*j + i]);
    }
  }

  aField_FIGG->removeReference();
  aField_NIGG->removeReference();
  aField_FIGG_conv->removeReference();

  //#ifdef ENABLE_FAULTS
  // (BUG) in FieldConvert(), concerning FIELD_::operator=
  {
    // create an empty integer field 2x10
    FIELD<int, FullInterlace> * aField = new FIELD<int, FullInterlace>();

    aField->setName("aField");
    aField->setDescription("Field full interlace no gauss");

    aField->setNumberOfComponents(2);
    aField->setNumberOfValues(10);

    aField->setComponentsNames(aCompsNames);
    aField->setComponentsDescriptions(aCompsDescs);
    aField->setMEDComponentsUnits(aMEDCompsUnits);

    MEDMEM_ArrayInterface<int,FullInterlace,NoGauss>::Array * anArray =
      new MEDMEM_ArrayInterface<int,FullInterlace,NoGauss>::Array
      (values_FING, /*dim*/2, /*nbelem*/10, /*shallowCopy*/false, /*ownershipOfValues*/false);
    aField->setArray(anArray);
    // no need to delete anArray, because it will be deleted in destructor of aField

    FIELD<int, NoInterlace> * aField_conv = FieldConvert(*aField);
    aField->removeReference();
    CPPUNIT_ASSERT(aField_conv);
    aField_conv->removeReference();
  }
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  // STD::vector
  //CPPUNIT_FAIL("FieldConvert() fails if _componentsUnits is not set, because it calls FIELD_::operator=");
  //#endif
}

//================================================================================
/*!
 * \brief 0020582: [CEA 368] MEDMEM don't work with a same field on NODES and CELLS
 * Before fixing the issue there was the error:
 *   RuntimeError: MED Exception in .../MED_SRC/src/MEDMEM/MEDMEM_MedFieldDriver22.hxx [503] : MED_FIELD_DRIVER<T>::createFieldSupportPart1(...) Field |field on NODEs and CELLs| with (ndt,or) = (-1,-1) must not be defined on different entity types
 */
//================================================================================

void MEDMEMTest::testReadFieldOnNodesAndCells()
{
  const string outfile = makeTmpFile("field_on_nodes_and_cells.med");
  const string fieldName = "field on NODEs and CELLs";

  MEDMEMTest_TmpFilesRemover aTmpFilesRemover;
  aTmpFilesRemover.Register( outfile );

  // write file with a field on NODEs and CELLs

  using namespace MED_EN;

  MEDMEM::MESH *mesh=MEDMEMTest_createTestMesh();
  int drv = mesh->addDriver( MED_DRIVER, outfile, mesh->getName());
  mesh->write(drv);

  SUPPORT *supportOnCells=new SUPPORT(mesh,"On_All_Cells",MED_CELL);
  SUPPORT *supportOnNodes=new SUPPORT(mesh,"On_All_Nodes",MED_NODE);
  mesh->removeReference();
  int numberOfCells = supportOnCells->getNumberOfElements(MED_ALL_ELEMENTS);
  int numberOfNodes = supportOnNodes->getNumberOfElements(MED_ALL_ELEMENTS);

  PointerOf<double> cellValues( numberOfCells ), nodeValues( numberOfNodes );
  for ( int i = 0; i < numberOfCells; ++i ) cellValues[i] = i;
  for ( int i = 0; i < numberOfNodes; ++i ) nodeValues[i] = -i;

  FIELD<double> *wrFieldOnCells=new FIELD<double>(supportOnCells,1);
  wrFieldOnCells->setName(fieldName);
  wrFieldOnCells->setComponentName(1,"Vx");
  wrFieldOnCells->setComponentDescription(1,"comp1");
  wrFieldOnCells->setMEDComponentUnit(1,"unit1");
  wrFieldOnCells->setValue( cellValues );
  drv = wrFieldOnCells->addDriver(MED_DRIVER, outfile, fieldName);
  wrFieldOnCells->write( drv );
  wrFieldOnCells->removeReference();

  FIELD<double> *wrFieldOnNodes=new FIELD<double>(supportOnNodes,1);
  wrFieldOnNodes->setName(fieldName);
  wrFieldOnNodes->setComponentName(1,"Vx");
  wrFieldOnNodes->setComponentDescription(1,"comp1");
  wrFieldOnNodes->setMEDComponentUnit(1,"unit1");
  wrFieldOnNodes->setValue( nodeValues );
  drv = wrFieldOnNodes->addDriver(MED_DRIVER, outfile, fieldName);
  wrFieldOnNodes->write( drv );
  wrFieldOnNodes->removeReference();

  //  READ FIELDS BACK

  //  field on CELLs
  FIELD<double> *cellField=new FIELD<double>(supportOnCells, MED_DRIVER, outfile, fieldName, -1, -1 );
  CPPUNIT_ASSERT_EQUAL( MED_CELL, cellField->getSupport()->getEntity());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( numberOfCells-1, cellField->getValueIJ( numberOfCells, 1 ),1e-20);
  cellField->removeReference();

  //  field on NODEs
  FIELD<double> *nodeField=new FIELD<double>(supportOnNodes, MED_DRIVER, outfile, fieldName, -1, -1 );
  CPPUNIT_ASSERT_EQUAL( MED_NODE, nodeField->getSupport()->getEntity());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( -(numberOfNodes-1), nodeField->getValueIJ( numberOfNodes, 1 ),1e-20);
  nodeField->removeReference();
  supportOnCells->removeReference();
  supportOnNodes->removeReference();
}
