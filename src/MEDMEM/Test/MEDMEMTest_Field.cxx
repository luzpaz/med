// Copyright (C) 2006  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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
#include <cppunit/TestAssert.h>

////#include "MEDMEM_nArray.hxx"
////#include "MEDMEM_ArrayConvert.hxx"
////#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
//#include "MEDMEM_AsciiFieldDriver.hxx"
//#include "MEDMEM_CellModel.hxx"
//#include "MEDMEM_Compatibility21_22.hxx"
//#include "MEDMEM_Connectivity.hxx"
////#include "MEDMEM_Coordinate.hxx"
//#include "MEDMEM_define.hxx"
//#include "MEDMEM_DriverFactory.hxx"
//#include "MEDMEM_DriversDef.hxx"
//#include "MEDMEM_DriverTools.hxx"
//#include "MEDMEM_Exception.hxx"
//#include "MEDMEM_Family.hxx"
//#include "MEDMEM_FieldConvert.hxx"
//#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_Field.hxx"
//#include "MEDMEM_Formulae.hxx"
//#include "MEDMEM_GaussLocalization.hxx"
//#include "MEDMEM_GenDriver.hxx"
//#include "MEDMEM_GibiMeshDriver.hxx"
//#include "MEDMEM_Grid.hxx"
//#include "MEDMEM_Group.hxx"
//#include "MEDMEM_IndexCheckingPolicy.hxx"
//#include "MEDMEM_InterlacingPolicy.hxx"
//#include "MEDMEM_InterlacingTraits.hxx"
//#include "MEDMEM_MedFieldDriver21.hxx"
//#include "MEDMEM_MedFieldDriver22.hxx"
//#include "MEDMEM_MedFieldDriver.hxx"
//#include "MEDMEM_Med.hxx"
//#include "MEDMEM_medimport_src.hxx"
//#include "MEDMEM_MedMedDriver21.hxx"
//#include "MEDMEM_MedMedDriver22.hxx"
//#include "MEDMEM_MedMedDriver.hxx"
//#include "MEDMEM_MEDMEMchampLire.hxx"
//#include "MEDMEM_MEDMEMgaussEcr.hxx"
//#include "MEDMEM_MEDMEMprofilEcr.hxx"
//#include "MEDMEM_MedMeshDriver21.hxx"
//#include "MEDMEM_MedMeshDriver22.hxx"
//#include "MEDMEM_MedMeshDriver.hxx"
//#include "MEDMEM_MedVersion.hxx"
//#include "MEDMEM_Mesh.hxx"
//#include "MEDMEM_Meshing.hxx"
//#include "MEDMEM_ModulusArray.hxx"
//#include "MEDMEM_PointerOf.hxx"
//#include "MEDMEM_PolyhedronArray.hxx"
//#include "MEDMEM_PorflowMeshDriver.hxx"
//#include "MEDMEM_RCBase.hxx"
//#include "MEDMEM_SetInterlacingType.hxx"
//#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_STRING.hxx"
//#include "MEDMEM_Support.hxx"
//#include "MEDMEM_Tags.hxx"
//#include "MEDMEM_TopLevel.hxx"
//#include "MEDMEM_TypeMeshDriver.hxx"
//#include "MEDMEM_Unit.hxx"
//#include "MEDMEM_Utilities.hxx"
//#include "MEDMEM_VtkFieldDriver.hxx"
//#include "MEDMEM_VtkMedDriver.hxx"
//#include "MEDMEM_VtkMeshDriver.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;

// #14,15,16: MEDMEMTest_Field.cxx
// Check methods from MEDMEM_Field.hxx, MEDMEM_FieldConvert.hxx, MEDMEM_FieldForward.hxx

// #14: MEDMEM_Field.hxx  }  MEDMEMTest_Field.cxx

/*!
 *  Check methods (48), defined in MEDMEM_Field.hxx:
 *  class FIELD_ {
 *   (yetno) FIELD_ ();
 *   (yetno) FIELD_(const SUPPORT * Support, const int NumberOfComponents);
 *   (yetno) FIELD_(const FIELD_ &m);
 *   (yetno) virtual ~FIELD_();
 *   (yetno) FIELD_& operator=(const FIELD_ &m);
 *   (yetno) virtual  void     rmDriver(int index=0);
 *   (yetno) virtual   int     addDriver(driverTypes driverType,
 *                             const string & fileName="Default File Name.med",
 *                              const string & driverFieldName="Default Field Nam",
 *                              MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (yetno) virtual  int      addDriver(GENDRIVER & driver);
 *   (yetno) virtual  void     read (const GENDRIVER &);
 *   (yetno) virtual  void     read(int index=0);
 *   (yetno) virtual  void     openAppend(void);
 *   (yetno) virtual  void     write(const GENDRIVER &);
 *   (yetno) virtual  void     write(int index=0, const string & driverName="");
 *   (yetno) virtual  void     writeAppend(const GENDRIVER &);
 *   (yetno) virtual  void     writeAppend(int index=0, const string & driverName="");
 *   (yetno) inline void     setName(const string Name);
 *   (yetno) inline string   getName() const;
 *   (yetno) inline void     setDescription(const string Description);
 *   (yetno) inline string   getDescription() const;
 *   (yetno) inline const SUPPORT * getSupport() const;
 *   (yetno) inline void     setSupport(const SUPPORT * support);
 *   (yetno) inline void     setNumberOfComponents(const int NumberOfComponents);
 *   (yetno) inline int      getNumberOfComponents() const;
 *   (yetno) inline void     setNumberOfValues(const int NumberOfValues);
 *   (yetno) inline int      getNumberOfValues() const;
 *   (yetno) inline void     setComponentsNames(const string * ComponentsNames);
 *   (yetno) inline void     setComponentName(int i, const string ComponentName);
 *   (yetno) inline const string * getComponentsNames() const;
 *   (yetno) inline string   getComponentName(int i) const;
 *   (yetno) inline void     setComponentsDescriptions(const string * ComponentsDescriptions);
 *   (yetno) inline void     setComponentDescription(int i, const string ComponentDescription);
 *   (yetno) inline const string * getComponentsDescriptions() const;
 *   (yetno) inline string   getComponentDescription(int i) const;
 *   (yetno) inline void     setComponentsUnits(const UNIT * ComponentsUnits);
 *   (yetno) inline const UNIT *   getComponentsUnits() const;
 *   (yetno) inline const UNIT *   getComponentUnit(int i) const;
 *   (yetno) inline void     setMEDComponentsUnits(const string * MEDComponentsUnits);
 *   (yetno) inline void     setMEDComponentUnit(int i, const string MEDComponentUnit);
 *   (yetno) inline const string * getMEDComponentsUnits() const;
 *   (yetno) inline string   getMEDComponentUnit(int i) const;
 *   (yetno) inline void     setIterationNumber(int IterationNumber);
 *   (yetno) inline int      getIterationNumber() const;
 *   (yetno) inline void     setTime(double Time);
 *   (yetno) inline double   getTime() const;
 *   (yetno) inline void     setOrderNumber(int OrderNumber);
 *   (yetno) inline int      getOrderNumber() const;
 *   (yetno) inline MED_EN::med_type_champ getValueType () const;
 *   (yetno) inline MED_EN::medModeSwitch  getInterlacingType() const;
 *   (yetno) virtual inline bool getGaussPresence() const throw (MEDEXCEPTION);
 *  }
 *
 *  template <class T, class INTERLACING_TAG> class FIELD : public FIELD_ {
 *   (yetno) FIELD();
 *   (yetno) FIELD(const FIELD &m);
 *   (yetno) FIELD(const SUPPORT * Support, const int NumberOfComponents) throw (MEDEXCEPTION);
 *   (yetno) FIELD(driverTypes driverType,
 *         const string & fileName, const string & fieldDriverName,
 *         const int iterationNumber=-1, const int orderNumber=-1) throw (MEDEXCEPTION);
 *   (yetno) FIELD(const SUPPORT * Support, driverTypes driverType,
 *        const string & fileName="", const string & fieldName="",
 *        const int iterationNumber = -1, const int orderNumber = -1) throw (MEDEXCEPTION);
 *   (yetno) ~FIELD();
 *   (yetno) FIELD & operator=(const FIELD &m);
 *   (yetno) const FIELD operator+(const FIELD& m) const;
 *   (yetno) const FIELD operator-(const FIELD& m) const;
 *   (yetno) const FIELD operator*(const FIELD& m) const;
 *   (yetno) const FIELD operator/(const FIELD& m) const;
 *   (yetno) const FIELD operator-() const;
 *   (yetno) FIELD& operator+=(const FIELD& m);
 *   (yetno) FIELD& operator-=(const FIELD& m);
 *   (yetno) FIELD& operator*=(const FIELD& m);
 *   (yetno) FIELD& operator/=(const FIELD& m);
 *   (yetno) static FIELD* add(const FIELD& m, const FIELD& n);
 *   (yetno) static FIELD* addDeep(const FIELD& m, const FIELD& n);
 *   (yetno) static FIELD* sub(const FIELD& m, const FIELD& n);
 *   (yetno) static FIELD* subDeep(const FIELD& m, const FIELD& n);
 *   (yetno) static FIELD* mul(const FIELD& m, const FIELD& n);
 *   (yetno) static FIELD* mulDeep(const FIELD& m, const FIELD& n);
 *   (yetno) static FIELD* div(const FIELD& m, const FIELD& n);
 *   (yetno) static FIELD* divDeep(const FIELD& m, const FIELD& n);
 *   (yetno) double normMax() const throw (MEDEXCEPTION);
 *   (yetno) double norm2() const throw (MEDEXCEPTION);
 *   (yetno) void   applyLin(T a, T b);
 *   (yetno) template <T T_function(T)> void applyFunc();
 *   (yetno) void applyPow(T scalar);
 *   (yetno) static FIELD* scalarProduct(const FIELD& m, const FIELD& n, bool deepCheck=false);
 *   (yetno) double normL2(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *   (yetno) double normL2(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *   (yetno) double normL1(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *   (yetno) double normL1(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
 *   (yetno) FIELD* extract(const SUPPORT *subSupport) const throw (MEDEXCEPTION);
 *   (yetno) void init ();
 *   (yetno) void rmDriver(int index=0);
 *   (yetno) int  addDriver(driverTypes driverType,
 *                 const string & fileName="Default File Name.med",
 *                 const string & driverFieldName="Default Field Name",
 *                 MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (yetno) int  addDriver(GENDRIVER & driver);
 *   (yetno) void allocValue(const int NumberOfComponents);
 *   (yetno) void allocValue(const int NumberOfComponents, const int LengthValue);
 *   (yetno) void deallocValue();
 *   (yetno) inline void read(int index=0);
 *   (yetno) inline void read(const GENDRIVER & genDriver);
 *   (yetno) inline void write(int index=0, const string & driverName = "");
 *   (yetno) inline void write(const GENDRIVER &);
 *   (yetno) inline void writeAppend(int index=0, const string & driverName = "");
 *   (yetno) inline void writeAppend(const GENDRIVER &);
 *   (yetno) inline MEDMEM_Array_  * getArray()        const throw (MEDEXCEPTION);
 *   (yetno) inline ArrayGauss     * getArrayGauss()   const throw (MEDEXCEPTION);
 *   (yetno) inline ArrayNoGauss   * getArrayNoGauss() const throw (MEDEXCEPTION);
 *   (yetno) inline bool     getGaussPresence() const throw (MEDEXCEPTION);
 *   (yetno) inline int      getValueLength() const throw (MEDEXCEPTION);
 *   (yetno) inline const T* getValue()       const throw (MEDEXCEPTION);
 *   (yetno) inline const T* getRow(int i)    const throw (MEDEXCEPTION);
 *   (yetno) inline const T* getColumn(int j) const throw (MEDEXCEPTION);
 *   (yetno) inline T        getValueIJ(int i,int j) const throw (MEDEXCEPTION);
 *   (yetno) inline T        getValueIJK(int i,int j,int k) const throw (MEDEXCEPTION);
 *   (yetno) bool            getValueOnElement(int eltIdInSup,T* retValues) const throw (MEDEXCEPTION);
 *   (yetno) const int   getNumberOfGeometricTypes() const throw (MEDEXCEPTION);
 *   (yetno) const GAUSS_LOCALIZATION<INTERLACING_TAG> & getGaussLocalization
 *                          (MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
 *   (yetno) const GAUSS_LOCALIZATION<INTERLACING_TAG> * getGaussLocalizationPtr
 *                          (MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
 *   (yetno) const int * getNumberOfGaussPoints() const throw (MEDEXCEPTION);
 *   (yetno) const int   getNumberOfGaussPoints
 *                          (MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
 *   (yetno) const int   getNbGaussI(int i)          const throw (MEDEXCEPTION);
 *   (yetno) const int * getNumberOfElements()       const throw (MEDEXCEPTION);
 *   (yetno) const MED_EN::medGeometryElement  * getGeometricTypes()  const throw (MEDEXCEPTION);
 *   (yetno) bool        isOnAllElements()           const throw (MEDEXCEPTION);
 *   (yetno) inline void setArray(MEDMEM_Array_ *value) throw (MEDEXCEPTION);
 *   (yetno) inline void setValue(T* value) throw (MEDEXCEPTION);
 *   (yetno) inline void setRow(int i, T* value) throw (MEDEXCEPTION);
 *   (yetno) inline void setColumn(int i, T* value) throw (MEDEXCEPTION);
 *   (yetno) inline void setValueIJ(int i, int j, T value) throw (MEDEXCEPTION);
 *   (yetno) void getVolume() const throw (MEDEXCEPTION);
 *   (yetno) void getArea() const throw (MEDEXCEPTION);
 *   (yetno) void getLength() const throw (MEDEXCEPTION);
 *   (yetno) void getNormal() const throw (MEDEXCEPTION);
 *   (yetno) void getBarycenter() const throw (MEDEXCEPTION);
 *   (yetno) void fillFromAnalytic(myFuncType f) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testField()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #15: MEDMEM_FieldConvert.hxx  }  MEDMEMTest_Field.cxx

/*!
 *  Check methods (2), defined in MEDMEM_FieldConvert.hxx:
 *  (yetno) template <class T> FIELD<T,FullInterlace> * FieldConvert(const FIELD<T,NoInterlace> & field);
 *  (yetno) template <class T> FIELD<T,NoInterlace> * FieldConvert(const FIELD<T,FullInterlace> & field);
 */
void MEDMEMTest::testFieldConvert()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #16: MEDMEM_FieldForward.hxx  }  MEDMEMTest_Field.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_FieldForward.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testFieldForward()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}
