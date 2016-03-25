// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCALCULATORDBFIELD_HXX__
#define __MEDCALCULATORDBFIELD_HXX__

#include "MedCalculatorDefines.hxx"
#include "MEDCalculatorDBSliceField.hxx"
#include "MEDCalculatorDBRangeSelection.hxx"

#include "MEDCouplingRefCountObject.hxx"
#include "MCAuto.hxx"

#include "InterpKernelException.hxx"

#include <string>
#include <vector>

namespace MEDCoupling
{
  class MEDCalculatorBrowserField;
  class MEDCalculatorDBFieldReal;
  class MEDCalculatorDBFieldCst;


  class MEDCALCULATOR_EXPORT MEDCalculatorDBField : public RefCountObject
  {
  public:
    static MEDCalculatorDBFieldReal *New(const MEDCalculatorBrowserField& ls);
    static MEDCalculatorDBFieldCst *New(double val);
    std::size_t getHeapMemorySizeWithoutChildren() const;
    std::vector<const BigMemoryObject *> getDirectChildrenWithNull() const;
    virtual MEDCalculatorDBField *operator+(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception) = 0;
    MEDCalculatorDBField *operator+(double val) const throw(INTERP_KERNEL::Exception);
    virtual MEDCalculatorDBField *operator-(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception) = 0;
    MEDCalculatorDBField *operator-(double val) const throw(INTERP_KERNEL::Exception);
    virtual MEDCalculatorDBField *operator*(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception) = 0;
    MEDCalculatorDBField *operator*(double val) const throw(INTERP_KERNEL::Exception);
    virtual MEDCalculatorDBField *operator/(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception) = 0;
    MEDCalculatorDBField *operator/(double val) const throw(INTERP_KERNEL::Exception);
    virtual bool isEqual(const MEDCalculatorDBField& other, double precM, double precF) const = 0;
  };

  class MEDCALCULATOR_EXPORT MEDCalculatorDBFieldReal : public MEDCalculatorDBField
  {
  public:
    MEDCalculatorDBFieldReal(const MEDCalculatorBrowserField& ls);
    ~MEDCalculatorDBFieldReal();
    void setName(const char *name);
    void setDescription(const char *descr);
    void write(const char *fName, bool writeFromScratch) const throw(INTERP_KERNEL::Exception);
    void display() const throw(INTERP_KERNEL::Exception);
    std::string simpleRepr() const;
    MEDCalculatorDBFieldReal *operator()(const MEDCalculatorDBRangeSelection& t, const MEDCalculatorDBRangeSelection& p, const MEDCalculatorDBRangeSelection& c) throw(INTERP_KERNEL::Exception);
    const MEDCalculatorDBFieldReal& operator=(const MEDCalculatorDBFieldReal& other) throw(INTERP_KERNEL::Exception);
    const MEDCalculatorDBFieldReal& operator=(double val) throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator+(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *add(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator-(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *substract(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator*(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *multiply(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator/(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *divide(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator^(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *dot(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *crossProduct(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *doublyContractedProduct() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *determinant() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *eigenValues() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *eigenVectors() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *inverse() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *trace() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *deviator() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *magnitude() const throw(INTERP_KERNEL::Exception);
    void applyFunc(const char *func) throw(INTERP_KERNEL::Exception);
    bool isEqual(const MEDCalculatorDBField& other, double precM, double precF) const;
    bool isEqualSameType(const MEDCalculatorDBFieldReal& other, double precM, double precF) const;
    MEDCalculatorDBFieldReal *buildCstFieldFromThis(double val) const;
    void checkConsistencyLight(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    void fetchData() const throw(INTERP_KERNEL::Exception);
    TypeOfField getType() const { return _type; }
    int getNumberOfSteps() const;
    int getNumberOfFetchedSteps() const;
    int getNumberOfComponents() const;
    std::vector<MEDCouplingFieldDouble *> getFields() const throw(INTERP_KERNEL::Exception);
    std::string getInfoOnComponent(int i) const throw(INTERP_KERNEL::Exception);
    void setInfoOnComponent(int i, const char *info) throw(INTERP_KERNEL::Exception);
  private:
    MEDCalculatorDBFieldReal(TypeOfField type);
  private:
    std::string _name;
    std::string _description;
    //
    std::string _file_name;
    std::string _mesh_name;
    std::string _field_name;
    TypeOfField _type;
    MEDCalculatorDBRangeSelection _t;
    MEDCalculatorDBRangeSelection _p;
    std::vector<std::string> _c_labels;
    MEDCalculatorDBRangeSelection _c;
    std::vector< MCAuto<MEDCalculatorDBSliceField> > _time_steps;
  };

  class MEDCALCULATOR_EXPORT MEDCalculatorDBFieldCst : public MEDCalculatorDBField
  {
  public:
    MEDCalculatorDBFieldCst(double val);
    MEDCalculatorDBField *operator+(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator-(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator*(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator/(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
    bool isEqual(const MEDCalculatorDBField& other, double precM, double precF) const;
    MEDCalculatorDBFieldReal *convertIntoReal(const MEDCalculatorDBFieldReal& ref) const;
    double getValue() const { return _val; }
  private:
    double _val;
  };
}

#endif
