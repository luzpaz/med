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

#include "MEDMEM_FieldConvert.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_VtkMeshDriver.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

#include <cppunit/TestAssert.h>

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
 *  class FIELD_ 
{
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
 *  
}
 *
 *  template <class T, class INTERLACING_TAG> class FIELD : public FIELD_ 
{
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
 *  
}
 *
 *  Use code of test_operation_fieldint.cxx
 *              test_operation_fielddouble.cxx
 *              test_copie_field_.cxx
 *              test_copie_fieldT.cxx
 */
 static void compareField_(const FIELD_ * theField_1, const FIELD_ * theField_2, bool isFIELD, bool isValue)
{
  // name, description, support
  CPPUNIT_ASSERT_EQUAL(theField_1->getName(), theField_2->getName());
  CPPUNIT_ASSERT_EQUAL(theField_1->getDescription(), theField_2->getDescription());
  CPPUNIT_ASSERT_EQUAL(theField_1->getSupport(), theField_2->getSupport());

  // components information
  int aNbComps = theField_1->getNumberOfComponents();
  CPPUNIT_ASSERT_EQUAL(aNbComps, theField_2->getNumberOfComponents());

  for (int i = 1; i <= aNbComps; i++) 
    {
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

  if (isFIELD) 
    {
      // Value type and Interlacing type
      CPPUNIT_ASSERT_EQUAL(theField_1->getValueType(), theField_2->getValueType());
      CPPUNIT_ASSERT_EQUAL(theField_1->getInterlacingType(), theField_2->getInterlacingType());

      // Gauss Presence
      if (isValue) 
        {
          CPPUNIT_ASSERT_EQUAL(theField_1->getGaussPresence(), theField_2->getGaussPresence());
        }
      else
        {
          CPPUNIT_ASSERT_THROW(theField_1->getGaussPresence(), MEDEXCEPTION);
          CPPUNIT_ASSERT_THROW(theField_2->getGaussPresence(), MEDEXCEPTION);
        }
    }
  else
    {
      CPPUNIT_ASSERT_THROW(theField_1->getGaussPresence(), MEDEXCEPTION);
      CPPUNIT_ASSERT_THROW(theField_2->getGaussPresence(), MEDEXCEPTION);
    }
}

static void checkField_(FIELD_ * theField_, const SUPPORT * theSupport,
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

  string aCompsNames[3] = 
    {
      "Vx", "Vy", "Vz"
    };
  string aCompsDescs[3] = 
    {
      "vitesse selon x", "vitesse selon y", "vitesse selon z"
    };
  string aCompsUnits[3] = 
    {
      "m.s-1", "m.s-1", "m.s-1"
    };

  theField_->setNumberOfComponents(aNbComps);
  CPPUNIT_ASSERT_EQUAL(aNbComps, theField_->getNumberOfComponents());

  theField_->setComponentsNames(aCompsNames);

  //#ifdef ENABLE_FAULTS
  try
    {
      theField_->setNumberOfComponents(7);
      // Segmentation fault here because array of components names is not resized
      for (int i = 1; i <= 7; i++) 
        {
          theField_->setComponentName(i, "AnyComponent");
        }
    }
  catch (MEDEXCEPTION& ex) 
    {
      // Ok, it is good to have MEDEXCEPTION here
    }
  catch (...) 
    {
      CPPUNIT_FAIL("Unknown exception cought");
    }
  // restore components names
  theField_->setNumberOfComponents(aNbComps);
  theField_->setComponentsNames(aCompsNames);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //  CPPUNIT_FAIL("FIELD_::_componentsNames bad management");
  //#endif

  theField_->setComponentsDescriptions(aCompsDescs);
  theField_->setMEDComponentsUnits(aCompsUnits);

  const string * aCompsNamesBack = theField_->getComponentsNames();
  const string * aCompsDescsBack = theField_->getComponentsDescriptions();
  const string * aCompsUnitsBack = theField_->getMEDComponentsUnits();
  for (int i = 1; i <= aNbComps; i++) 
    {
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
  CPPUNIT_ASSERT_THROW(theField_->setComponentName(0, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setComponentName(aNbComps + 1, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setComponentDescription(0, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setComponentDescription(aNbComps + 1, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setMEDComponentUnit(0, "str"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(theField_->setMEDComponentUnit(aNbComps + 1, "str"), MEDEXCEPTION);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //  CPPUNIT_FAIL("FIELD::setComponentXXX() does not check component index");
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
    const GMESH* aMesh = theSupport->getMesh();
    int spaceDim = 3;
    if (aMesh) spaceDim = aMesh->getSpaceDimension();
    theField->deallocValue();
    theField->allocValue(/*NumberOfComponents = */spaceDim + 1);

    //  0020142: [CEA 315] Unused function in MEDMEM::FIELD
    // getVolume() etc. does nothing
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

    if (aMesh) 
      {
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
  theField->getGaussPresence();
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //  CPPUNIT_FAIL("FIELD::deallocValue() does not nullify _value pointer");
  //#endif

  // copy constructor
  FIELD<T, INTERLACING_TAG> aField_copy1 (*theField);
  //compareField(theField, &aField_copy1, /*isValue = */false);
  compareField(theField, &aField_copy1, /*isValue = */true);

  // operator=
  //#ifdef ENABLE_FAULTS
  // (BUG) This fails (Segmentation fault) if not set:
  // _componentsNames or _componentsDescriptions, or _componentsUnits, or _MEDComponentsUnits
  FIELD<T, INTERLACING_TAG> aField_copy2;
  aField_copy2 = *theField;
  //compareField(theField, &aField_copy2, /*isValue = */false);
  compareField(theField, &aField_copy2, /*isValue = */true);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //  CPPUNIT_FAIL("FIELD_::operator=() fails if _componentsUnits is not set");
  //#endif
}

template<class T>
FIELD<T> * createFieldOnGroup(MESH* theMesh, const GROUP* theGroup,
                              const string theName, const string theDescr)
{
  FIELD<T> * aFieldOnGroup = new FIELD<T> (theGroup, /*NumberOfComponents = */2);

  aFieldOnGroup->setName(theName);
  aFieldOnGroup->setDescription(theDescr);

  string aCompsNames[2] = 
    {
      "Pos", "Neg"
    };
  string aCompsDescs[2] = 
    {
      "+", "-"
    };
  string aCompsUnits[2] = 
    {
      "unit1", "unit2"
    };

  aFieldOnGroup->setComponentsNames(aCompsNames);
  aFieldOnGroup->setComponentsDescriptions(aCompsDescs);
  aFieldOnGroup->setMEDComponentsUnits(aCompsUnits);

  return aFieldOnGroup;
}

double plus13 (double val);
double plus13 (double val)
{
  return val + 13;
}

// function to calculate field values from coordinates of an element
// typedef void (*myFuncType)(const double * temp, T* output);
// size of temp array = space dim = 3
// size of output array = nb. comps = 2
static void proj2d (const double * temp, double* output)
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

static void testDrivers()
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

  //////////////////
  //TestRead Part//
  //////////////////
  FIELD<double> *aField_1 = NULL;
  CPPUNIT_ASSERT_NO_THROW(aField_1 = new FIELD<double>(MED_DRIVER, filename_rd, fieldname_celldouble_rd));

  //Test read(int index) method
  int IdDriver_rd = aField_1->addDriver(MED_DRIVER,filename_rd,fieldname_celldouble_rd);
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Cannot open file, but file exist
  CPPUNIT_ASSERT_NO_THROW(aField_1->read(IdDriver_rd));
  //#endif

  //Test read(GENDRIVER & genDriver) method
  //Creation a Driver
  MED_FIELD_RDONLY_DRIVER<int> *aMedRdFieldDriver_1 = new MED_FIELD_RDONLY_DRIVER<int>();
  //Creation a Field
  FIELD<int> *aField_2 = new FIELD<int>();
  aField_2->setName(fieldname_nodeint_rd);
  aField_2->addDriver(*aMedRdFieldDriver_1);
  aField_2->read(*aMedRdFieldDriver_1);

  ///////////////////
  //Test Write Part//
  ///////////////////
  int IdDriver;
  MESH *aMesh = new MESH(MED_DRIVER,filename_rd,meshname);
  const SUPPORT *aSupport = aMesh->getSupportOnAll( MED_CELL );
  FIELD<int> *aFieldSupport;
  //#ifdef ENABLE_FORCED_FAILURES  
  CPPUNIT_ASSERT_NO_THROW(aFieldSupport = 
                          new FIELD<int>(aSupport, MED_DRIVER,filename_support_wr,fieldname_nodeint_rd));
  //(BUG) Can not open file
  MED_FIELD_WRONLY_DRIVER<int> * aFieldWrDriver = 
    new MED_FIELD_WRONLY_DRIVER<int>(filename_support_wr,aFieldSupport);
  aFieldWrDriver->setFieldName(aFieldSupport->getName() + "_copy");
  CPPUNIT_ASSERT_NO_THROW(IdDriver= aFieldSupport->addDriver(*aFieldWrDriver));
  CPPUNIT_ASSERT_NO_THROW(aFieldSupport->write(IdDriver));
  aFieldSupport->removeReference();
  delete aFieldWrDriver;
  //#endif

  //Create fileds
  FIELD<double> * aField_3 = new FIELD<double>();
  MED_FIELD_RDONLY_DRIVER<double> *aMedRdFieldDriver_2 =
    new MED_FIELD_RDONLY_DRIVER<double>(filename_rd, aField_3);
  aMedRdFieldDriver_2->open();
  aMedRdFieldDriver_2->setFieldName(fieldname_celldouble_rd);
  aMedRdFieldDriver_2->read();
  aMedRdFieldDriver_2->close();

  //Test write(int index) method
  //Add drivers to FIELDs
  int IdDriver1 = -1;
  try
    {
      IdDriver1 = aField_3->addDriver(MED_DRIVER,filename_wr,fieldname_celldouble_wr);
    }
  catch(MEDEXCEPTION &e)
    {
      e.what();
    }
  catch( ... )
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  //Trying call write(int index) method with incorrect index
  //#ifdef ENABLE_FAULTS
  CPPUNIT_ASSERT_THROW(aField_3->write(IdDriver1+1),MEDEXCEPTION);
  // => Segmentation fault
  //#endif

  //Write field to file
  //#ifdef ENABLE_FAULTS
  try
    {
      aField_3->write(IdDriver1);
      // => Segmentation fault
    }
  catch(MEDEXCEPTION &e)
    {
      e.what();
    }
  catch( ... )
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  //#endif

  CPPUNIT_ASSERT_NO_THROW(aField_3->rmDriver(IdDriver1));

  //Test write(const GENDRIVER &);
  //Create a driver
  MED_FIELD_WRONLY_DRIVER<int> *aMedWrFieldDriver = new MED_FIELD_WRONLY_DRIVER<int>();
  aMedWrFieldDriver->setFileName(filename_wr);
  aField_2->setName(fieldname_nodeint_wr1);
  //Add driver to a field
  aField_2->addDriver(*aMedWrFieldDriver);

  try
    {
      aField_2->write(*aMedWrFieldDriver);
    }
  catch(MEDEXCEPTION &e)
    {
      e.what();
    }
  catch( ... )
    {
      CPPUNIT_FAIL("Unknown exception");
    }

  //Test writeAppend(int index) method
  //Create a vtk file
  MESH * aMesh_1 = new MESH;
  MED_MESH_RDONLY_DRIVER *aMedMeshRdDriver22 = new MED_MESH_RDONLY_DRIVER(filename22_rd, aMesh_1);
  aMedMeshRdDriver22->open();
  aMedMeshRdDriver22->setMeshName(meshname);
  aMedMeshRdDriver22->read();
  aMedMeshRdDriver22->close();
  VTK_MESH_DRIVER *aVtkDriver = new VTK_MESH_DRIVER(filenamevtk_wr, aMesh_1);
  aVtkDriver->open();
  aVtkDriver->write();
  aVtkDriver->close();

  //Create a field
  FIELD<int> * aField_4 = new FIELD<int>();
  MED_FIELD_RDONLY_DRIVER<int> *aMedRdFieldDriver22 =
    new MED_FIELD_RDONLY_DRIVER<int>(filename22_rd, aField_2);
  aMedRdFieldDriver22->open();
  aMedRdFieldDriver22->setFieldName(fieldname_nodeint_rd);
  aMedRdFieldDriver22->read();
  aMedRdFieldDriver22->close();

  //Add Driver to a field
  int IdDriver2;
  try
    {
      IdDriver2 = aField_4->addDriver(VTK_DRIVER, filenamevtk_wr ,fieldname_nodeint_wr);
    }
  catch(MEDEXCEPTION &e)
    {
      e.what();
    }
  catch( ... )
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  //#ifdef ENABLE_FAULTS
  //Trying call writeAppend() method with incorrect index
  CPPUNIT_ASSERT_THROW(aField_4->writeAppend(IdDriver2+1,fieldname_nodeint_wr),MEDEXCEPTION);
  // => Segmentation fault
  //#endif

  //#ifdef ENABLE_FAULTS
  // (BUG) => Segmentation fault
  CPPUNIT_ASSERT_NO_THROW(aField_4->writeAppend(IdDriver2, fieldname_nodeint_wr));
  //#endif

  //Test writeAppend(const GENDRIVER &) method
  aField_4->setName(fieldname_nodeint_wr1);

  //Add driver to a field
  //#ifdef ENABLE_FAULTS
  //Create a driver
  VTK_FIELD_DRIVER<int> *aVtkFieldDriver = new VTK_FIELD_DRIVER<int>(filenamevtk_wr, aField_4);
  CPPUNIT_ASSERT_NO_THROW(aField_4->addDriver(*aVtkFieldDriver));
  //(BUG) => Segmentation fault after addDriver(const GENDRIVER &)
  CPPUNIT_ASSERT_NO_THROW(aField_4->writeAppend(*aVtkFieldDriver));
  //#endif


  //Delete objects
  aField_1->removeReference();
  delete aMedRdFieldDriver_1;
  aField_2->removeReference();
  aField_3->removeReference();
  delete aMedRdFieldDriver_2;
  aField_4->removeReference();
  delete aMedMeshRdDriver22;
  delete aMedWrFieldDriver;
  delete aVtkDriver;
  aMesh->removeReference();
  aMesh_1->removeReference();
  delete aMedRdFieldDriver22;
}

static void MEDMEMTest_testField()
{
  SUPPORT *anEmptySupport=new SUPPORT;
  ////////////////////
  // TEST 1: FIELD_ //
  ////////////////////
  FIELD_ *aField_=new FIELD_ ;

  // check set/get methods
  MED_EN::med_type_champ aValueType = MED_EN::MED_UNDEFINED_TYPE;
  MED_EN::medModeSwitch  anInterlace = MED_EN::MED_UNDEFINED_INTERLACE;
  checkField_(aField_, anEmptySupport, aValueType, anInterlace);

  // copy constructor
  // This fails (Segmentation fault) if not set:
  // _componentsNames or _componentsDescriptions, or _MEDComponentsUnits
  FIELD_ *aField_copy1=new FIELD_(*aField_);
  compareField_(aField_, aField_copy1, /*isFIELD = */false, /*isValue = */false);

  // operator=
  //#ifdef ENABLE_FAULTS
  // (BUG) This fails (Segmentation fault) if not set:
  // _componentsNames or _componentsDescriptions, or _componentsUnits, or _MEDComponentsUnits
  // (BUG) Code duplication with copyGlobalInfo(), called from copy constructor
  FIELD_ *aField_copy2=new FIELD_;
  *aField_copy2 = *aField_;
  compareField_(aField_, aField_copy2, /*isFIELD = */false, /*isValue = */false);
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //  CPPUNIT_FAIL("FIELD_::operator=() fails if _componentsUnits is not set");
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
  FIELD_ aField_case2;
  aField_case2.setSupport(&anEmptySupport);
  aField_case2.setNumberOfComponents(10);

  #ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_EQUAL_MESSAGE("No correspondance between CASE1 and CASE2",
  aField_case1.getNumberOfValues(),
  aField_case2.getNumberOfValues());
  #endif
  }
  */

  ////////////////////////
  // TEST 2: FIELD<int> //
  ////////////////////////
  FIELD<int> *aFieldInt=new FIELD<int>();
  checkField(aFieldInt, anEmptySupport);

  ////////////////////////////////////////
  // TEST 3: FIELD<double, NoInterlace> //
  ////////////////////////////////////////
  MESH * aMesh = MEDMEMTest_createTestMesh();
  const GROUP* aGroup = aMesh->getGroup(MED_EN::MED_FACE, 1);

  FIELD<double, NoInterlace> aFieldDouble;
  checkField(&aFieldDouble, aGroup);

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

  for (int i = 1; i <= nbVals; i++) 
    {
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

  for (int i = 1; i <= nbVals; i++) 
    {
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
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(3. + 2.*i*i, aFieldOnGroup2->getValueIJ(anElems[i-1], 1), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(3. - 2.*i*i, aFieldOnGroup2->getValueIJ(anElems[i-1], 2), 0.000001);
    }

  // apply function plus13() to aFieldOnGroup1
  aFieldOnGroup1->applyFunc<plus13>();
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
    }

  // scalarProduct
  FIELD<double, FullInterlace> * aScalarProduct =
    FIELD<double, FullInterlace>::scalarProduct(*aFieldOnGroup1, *aFieldOnGroup2, /*deepCheck = */true);
  CPPUNIT_ASSERT_EQUAL(nbVals, aScalarProduct->getNumberOfValues());
  CPPUNIT_ASSERT_EQUAL(1, aScalarProduct->getNumberOfComponents());
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(78. + 4.*i*i*i, //(3. + 2.*i*i)*(13 + i) + (3. - 2.*i*i)*(13 - i)
                                   aScalarProduct->getValueIJ(anElems[i-1], 1), 0.000001);
    }

  // fillFromAnalytic
  aFieldOnGroup2->fillFromAnalytic(proj2d);

  double bary [3];
  double outp [2];
  const SUPPORT * aSupp = aFieldOnGroup2->getSupport();
  FIELD<double,FullInterlace> * barycenter = aMesh->getBarycenter(aSupp);
  for (int i = 1; i <= nbVals; i++) 
    {
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
  double cooRef[6] = 
    {
      1.,1., 2.,4., 3.,9.
    }; // xy xy xy
  double cooGauss[10] = 
    {
      7.,7., 6.,6., 5.,5., 4.,3., 2.,1.
    }; // x1,y1  x2,y2  x3,y3  x4,y4  x5,y5
  double wg[5] = 
    {
      1., 2., 3., 4., 5.
    };
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
  SUPPORT * aSubSupport1 = new SUPPORT;
  aSubSupport1->setMesh( aMesh );
  aSubSupport1->setName( "Sub-Support 1 of Group1" );
  aSubSupport1->setEntity( MED_EN::MED_FACE );

  int nbTypes1 = 1;
  int nbElemsInEachType1[1];
  nbElemsInEachType1[0] = nbElemsInEachType[0];
  int nbElems1 = nbElemsInEachType1[0];
  MED_EN::medGeometryElement aGeomTypes1[1];
  aGeomTypes1[0] = aGeomTypes[0];
  int * anElems1 = new int[nbElems1];
  for (int i = 0; i < nbElems1; i++) 
    {
      anElems1[i] = anElems[i];
    }

  aSubSupport1->setpartial("Support for sub-field 1 on one type of elements",
                           nbTypes1, nbElems1, aGeomTypes1, nbElemsInEachType1, anElems1);

  //cout << "aSubSupport1:" << endl;
  //cout << *aSubSupport1 << endl;

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
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) FIELD::normL2(int component, const FIELD * p_field_volume):
  //       component is not taken into account
  CPPUNIT_ASSERT_DOUBLES_EQUAL(132.5, aSubField1->normL2(2), 0.00001); // (12*12 + 11*11)/2
  //#endif
  CPPUNIT_ASSERT_DOUBLES_EQUAL(343.0, aSubField1->normL2() , 0.00001); // 210.5 + 132.5

  CPPUNIT_ASSERT_DOUBLES_EQUAL(14.5, aSubField1->normL1(1), 0.00001); // (14 + 15)/2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(11.5, aSubField1->normL1(2), 0.00001); // (12 + 11)/2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(26.0, aSubField1->normL1() , 0.00001); // 14.5 + 11.5

  double aNewArea [2] = 
    {
      1., 0.
    }; // only first element will be taken into account
  anAreaField->setValue(aNewArea);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(196.0, aSubField1->normL2(1, anAreaField), 0.00001); // 14*14/1
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) FIELD::normL2(int component, const FIELD * p_field_volume):
  //       component is not taken into account
  CPPUNIT_ASSERT_DOUBLES_EQUAL(144.0, aSubField1->normL2(2, anAreaField), 0.00001); // 12*12/1
  //#endif
  CPPUNIT_ASSERT_DOUBLES_EQUAL(340.0, aSubField1->normL2(anAreaField) , 0.00001); // 196 + 144

  CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0, aSubField1->normL1(1, anAreaField), 0.00001); // 14/1
  CPPUNIT_ASSERT_DOUBLES_EQUAL(12.0, aSubField1->normL1(2, anAreaField), 0.00001); // 12/1
  CPPUNIT_ASSERT_DOUBLES_EQUAL(26.0, aSubField1->normL1(anAreaField) , 0.00001); // 14 + 12

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
  double row[2] = 
    {
      -1., -3.
    };
  aSubField1->setRow(anElems1[0], row);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-1., aSubField1->getValueIJ(anElems1[0], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-3., aSubField1->getValueIJ(anElems1[0], 2), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 5., aSubField1->getValueIJ(anElems1[1], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., aSubField1->getValueIJ(anElems1[1], 2), 0.000001);
  // out of range
  CPPUNIT_ASSERT_THROW(aSubField1->setRow(3, row), MEDEXCEPTION);

  // setColumn
  double col[2] = 
    {
      -7., -9.
    };
  aSubField1->setColumn(1, col);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-7., aSubField1->getValueIJ(anElems1[0], 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-3., aSubField1->getValueIJ(anElems1[0], 2), 0.000001);
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) in MEDMEM_Array::setColumn()
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-9., aSubField1->getValueIJ(anElems1[1], 1), 0.000001);
  //#endif
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., aSubField1->getValueIJ(anElems1[1], 2), 0.000001);
  // out of range
  CPPUNIT_ASSERT_THROW(aSubField1->setColumn(3, col), MEDEXCEPTION);

  // setArray (Gauss)
  {
    int nbelgeoc[2] = 
      {
        1, 3
      }; // 3 - 1 = two elements for the first (and the only) type
    int nbgaussgeo[2] = 
      {
        -1, 1
      }; // one gauss point per each element
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
    //    CPPUNIT_FAIL("? Bug in FullInterlaceGaussPolicy::getIndex(int i,int j) ?");
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
    //    CPPUNIT_FAIL("? Bug in FullInterlaceGaussPolicy::getIndex(int i,int j) ?");
    //#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4., aSubField1->getValueIJK(anElems1[0], 1, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-2., aSubField1->getValueIJK(anElems1[0], 2, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-5., aSubField1->getValueIJK(anElems1[1], 1, 1), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1., aSubField1->getValueIJK(anElems1[1], 2, 1), 0.000001);
  }

  // alloc/dealloc; compatibility of new size with support
  try
    {
      aSubField1->deallocValue();
      aSubField1->allocValue(/*NumberOfComponents*/2, /*LengthValue*/5);
      //#ifdef ENABLE_FAULTS
      // (BUG) No compatibility between Support and allocated value
      aSubField1->normL1();
      //#endif
      //#ifdef ENABLE_FORCED_FAILURES
      //    CPPUNIT_FAIL("Error: no compatibility between Support and allocated value");
      //#endif
    }
  catch (MEDEXCEPTION & ex) 
    {
      // normal behaviour
    }
  catch (...) 
    {
      CPPUNIT_FAIL("Error: no compatibility between Support and allocated value");
    }

  // check that aFieldOnGroup1 is not changed after aSubField1 modifications
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
    }

  // reset aFieldOnGroup2 values for simple control of operators results
  for (int i = 1; i <= nbVals; i++) 
    {
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
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val2[i], val_res[i], 0.000001);
    }
  aSum->removeReference();

  // -
  FIELD<double> *aDifference = *aFieldOnGroup1 - *aFieldOnGroup2;
  aDifference->setName(aFieldOnGroup1->getName());
  aDifference->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aDifference, true, true);
  val_res = aDifference->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] - val2[i], val_res[i], 0.000001);
    }
  aDifference->removeReference();

  // - (unary)
  FIELD<double> *aNegative = - *aFieldOnGroup1;
  aNegative->setName(aFieldOnGroup1->getName());
  aNegative->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aNegative, true, true);
  val_res = aNegative->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(- val1[i], val_res[i], 0.000001);
    }
  aNegative->removeReference();

  // *
  FIELD<double> *aProduct = (*aFieldOnGroup1) * (*aFieldOnGroup2);
  aProduct->setName(aFieldOnGroup1->getName());
  aProduct->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aProduct, true, true);
  val_res = aProduct->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] * val2[i], val_res[i], 0.000001);
    }
  aProduct->removeReference();
  // /
  FIELD<double> *aQuotient = *aFieldOnGroup1 / *aFieldOnGroup2;
  aQuotient->setName(aFieldOnGroup1->getName());
  aQuotient->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aQuotient, true, true);
  val_res = aQuotient->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] / val2[i], val_res[i], 0.000001);
    }
  aQuotient->removeReference();

  double val22 = aFieldOnGroup2->getValueIJ(anElems[2], 2);
  aFieldOnGroup2->setValueIJ(anElems[2], 2, 0.);

  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 / *aFieldOnGroup2, MEDEXCEPTION);
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) is it up to user to control validity of data to avoid division on zero?
  CPPUNIT_ASSERT_THROW(*aFieldOnGroup1 /= *aFieldOnGroup2, MEDEXCEPTION);
  //#endif
  CPPUNIT_ASSERT_THROW(FIELD<double>::div(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup2), MEDEXCEPTION);

  // restore value
  aFieldOnGroup2->setValueIJ(anElems[2], 2, val22);

  // restore values
  for (int i = 1; i <= nbVals; i++) 
    {
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
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // sub
  aPtr = FIELD<double>::sub(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] - val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // mul
  aPtr = FIELD<double>::mul(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] * val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // div
  aPtr = FIELD<double>::div(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] / val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // addDeep
  aPtr = FIELD<double>::addDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // subDeep
  aPtr = FIELD<double>::subDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] - val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // mulDeep
  aPtr = FIELD<double>::mulDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] * val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // divDeep
  aPtr = FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup2);
  aPtr->setName(aFieldOnGroup1->getName());
  aPtr->setDescription(aFieldOnGroup1->getDescription());
  compareField_(aFieldOnGroup1, aPtr, true, true);
  val_res = aPtr->getValue();
  for (int i = 0; i < len; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] / val2[i], val_res[i], 0.000001);
    }
  delete aPtr;

  // +=
  *aFieldOnGroup1 += *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i + i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i - i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
    }

  // -=
  *aFieldOnGroup1 -= *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
    }

  // *=
  *aFieldOnGroup1 *= *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL( (13 + i)*i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(-(13 - i)*i*i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
    }

  // /=
  *aFieldOnGroup1 /= *aFieldOnGroup2;
  for (int i = 1; i <= nbVals; i++) 
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 + i, aFieldOnGroup1->getValueIJ(anElems[i-1], 1), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(13 - i, aFieldOnGroup1->getValueIJ(anElems[i-1], 2), 0.000001);
    }

  // check case of different operands: support
  MESH * aMeshOneMore = MEDMEMTest_createTestMesh();
  const GROUP* aGroupOneMore = aMeshOneMore->getGroup(MED_EN::MED_FACE, 1);
  FIELD<double> * aFieldOnGroup3 =
    createFieldOnGroup<double>(aMeshOneMore, aGroupOneMore, "Test_Diff_Mesh", "test");
  for (int i = 1; i <= nbVals; i++) 
    {
      aFieldOnGroup3->setValueIJ(anElems[i-1], 1, 2*i);
      aFieldOnGroup3->setValueIJ(anElems[i-1], 2, 3*i);
    }
  const double * val3 = aFieldOnGroup3->getValue();

  //CPPUNIT_ASSERT_NO_THROW();
  try
    {
      aPtr = FIELD<double>::addDeep(*aFieldOnGroup1, *aFieldOnGroup3);
      aPtr->setName(aFieldOnGroup1->getName());
      aPtr->setDescription(aFieldOnGroup1->getDescription());
      compareField_(aFieldOnGroup1, aPtr, true, true);
      val_res = aPtr->getValue();
      for (int i = 0; i < len; i++) 
        {
          CPPUNIT_ASSERT_DOUBLES_EQUAL(val1[i] + val3[i], val_res[i], 0.000001);
        }
      delete aPtr;

      aPtr = FIELD<double>::subDeep(*aFieldOnGroup1, *aFieldOnGroup3);
      delete aPtr;
      aPtr = FIELD<double>::mulDeep(*aFieldOnGroup1, *aFieldOnGroup3);
      delete aPtr;
      aPtr = FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup3);
      delete aPtr;
    }
  catch (MEDEXCEPTION & ex) 
    {
      CPPUNIT_FAIL(ex.what());
    }
  catch (...) 
    {
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
  try
    {
      aPtr = FIELD<double>::mul(*aFieldOnGroup1, *aFieldOnGroup2);
      delete aPtr;
      aPtr = FIELD<double>::div(*aFieldOnGroup1, *aFieldOnGroup2);
      delete aPtr;
      aPtr = FIELD<double>::mulDeep(*aFieldOnGroup1, *aFieldOnGroup2);
      delete aPtr;
      aPtr = FIELD<double>::divDeep(*aFieldOnGroup1, *aFieldOnGroup2);
      delete aPtr;

      *aFieldOnGroup1 *= *aFieldOnGroup2;
      *aFieldOnGroup1 /= *aFieldOnGroup2;

      FIELD<double> *aPr = *aFieldOnGroup1 * *aFieldOnGroup2;
      FIELD<double> *aQu = *aFieldOnGroup1 / *aFieldOnGroup2;
      aPr->removeReference();
      aQu->removeReference();
    }
  catch (MEDEXCEPTION & ex) 
    {
      CPPUNIT_FAIL(ex.what());
    }
  catch (...) 
    {
      CPPUNIT_FAIL("Unknown exception");
    }

  // restore MED units
  aFieldOnGroup1->setMEDComponentUnit(1, "unit1");

  // check case of different operands: numberOfComponents
  //#ifdef ENABLE_FAULTS
  // (BUG) Cannot allocate value of higher dimension because of _componentsTypes reinitialization
  // Must be MEDEXCEPTION instead. And on attempt to change nb.components must be the same behaviour.
  aFieldOnGroup1->deallocValue();
  CPPUNIT_ASSERT_THROW(aFieldOnGroup1->allocValue(/*dim*/5), MEDEXCEPTION);
  //#endif

  aSubSupport1->removeReference();
  delete [] anElems1;

  delete aScalarProduct;
  delete aSubField1;
  delete anAreaField;
  delete barycenter;
  delete aFieldOnGroup1;
  delete aFieldOnGroup2;
  delete aFieldOnGroup3;

  delete aMesh;
  delete aMeshOneMore;

  /////////////////////
  // TEST 5: Drivers //
  /////////////////////
  testDrivers();
}

int main (int argc, char** argv)
{
  MEDMEMTest_testField();
}
