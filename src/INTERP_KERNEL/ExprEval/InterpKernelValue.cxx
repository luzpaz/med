#include "InterpKernelValue.hxx"
#include "InterpKernelFunction.hxx"

#include <cmath>
#include <limits>
#include <algorithm>

using namespace INTERP_KERNEL;

ValueDouble::ValueDouble():_data(std::numeric_limits<double>::max())
{
}

Value *ValueDouble::newInstance() const
{
  return new ValueDouble;
}

ValueDouble::ValueDouble(double val):_data(val)
{
}

void ValueDouble::setDouble(double val) throw(INTERP_KERNEL::Exception)
{
  _data=val;
}

void ValueDouble::setVarname(int fastPos, const std::string& var) throw(INTERP_KERNEL::Exception)
{
  std::string msg("Error var : "); msg+=var; msg+=" not numeric : use another expression evaluator !";
  throw INTERP_KERNEL::Exception(msg.c_str());
}

void ValueDouble::positive() throw(INTERP_KERNEL::Exception)
{
}

void ValueDouble::negate() throw(INTERP_KERNEL::Exception)
{
  _data=-_data;
}

void ValueDouble::sqrt() throw(INTERP_KERNEL::Exception)
{
  _data=std::sqrt(_data);
}

void ValueDouble::cos() throw(INTERP_KERNEL::Exception)
{
  _data=std::cos(_data);
}

void ValueDouble::sin() throw(INTERP_KERNEL::Exception)
{
  _data=std::sin(_data);
}

void ValueDouble::tan() throw(INTERP_KERNEL::Exception)
{
  _data=std::tan(_data);
}

void ValueDouble::abs() throw(INTERP_KERNEL::Exception)
{
  if(_data<0.)
    _data=-_data;
}

void ValueDouble::exp() throw(INTERP_KERNEL::Exception)
{
  _data=std::exp(_data);
}

void ValueDouble:: ln() throw(INTERP_KERNEL::Exception)
{
  _data=std::log(_data);
}

Value *ValueDouble::plus(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=checkSameType(other);
  return new ValueDouble(_data+valC->_data);
}

Value *ValueDouble::minus(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=checkSameType(other);
  return new ValueDouble(_data-valC->_data);
}

Value *ValueDouble::mult(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=checkSameType(other);
  return new ValueDouble(_data*valC->_data);
}

Value *ValueDouble::div(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=checkSameType(other);
  return new ValueDouble(_data/valC->_data);
}

Value *ValueDouble::pow(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=checkSameType(other);
  return new ValueDouble(std::pow(_data,valC->_data));
}

Value *ValueDouble::max(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=checkSameType(other);
  return new ValueDouble(std::max(_data,valC->_data));
}

Value *ValueDouble::min(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=checkSameType(other);
  return new ValueDouble(std::min(_data,valC->_data));
}

const ValueDouble *ValueDouble::checkSameType(const Value *val) throw(INTERP_KERNEL::Exception)
{
  const ValueDouble *valC=dynamic_cast<const ValueDouble *>(val);
  if(!valC)
    throw INTERP_KERNEL::Exception("Trying to operate on non homogeneous Values (double with other type) !");
  return valC;
}

ValueUnit::ValueUnit()
{
}

Value *ValueUnit::newInstance() const
{
  return new ValueUnit;
}

ValueUnit::ValueUnit(const DecompositionInUnitBase& unit):_data(unit)
{
}

void ValueUnit::setDouble(double val) throw(INTERP_KERNEL::Exception)
{
  _data.tryToConvertInUnit(val);
}

void ValueUnit::setVarname(int fastPos, const std::string& var) throw(INTERP_KERNEL::Exception)
{
  double add,mult;
  const short *projInBase=UnitDataBase::_uniqueMapForExpr.getInfoForUnit(var,add,mult);
  _data.setInfo(projInBase,add,mult);
}

void ValueUnit::positive() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(PositiveFunction::REPR);
}

void ValueUnit::negate() throw(INTERP_KERNEL::Exception)
{
  _data.negate();
}

void ValueUnit::sqrt() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(SqrtFunction::REPR);
}

void ValueUnit::cos() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(CosFunction::REPR);
}

void ValueUnit::sin() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(SinFunction::REPR);
}

void ValueUnit::tan() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(TanFunction::REPR);
}

void ValueUnit::abs() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(AbsFunction::REPR);
}

void ValueUnit::exp() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(ExpFunction::REPR);
}

void ValueUnit::ln() throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(LnFunction::REPR);
}

Value *ValueUnit::plus(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(PlusFunction::REPR);
  return 0;
}

Value *ValueUnit::minus(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(MinusFunction::REPR);
  return 0;
}

Value *ValueUnit::mult(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueUnit *valC=checkSameType(other);
  DecompositionInUnitBase tmp=_data;
  tmp*valC->getData();
  return new ValueUnit(tmp);
}

Value *ValueUnit::div(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueUnit *valC=checkSameType(other);
  DecompositionInUnitBase tmp=_data;
  tmp/valC->getData();
  return new ValueUnit(tmp);
}

Value *ValueUnit::pow(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueUnit *valC=checkSameType(other);
  DecompositionInUnitBase tmp=_data;
  tmp^valC->getData();
  return new ValueUnit(tmp);
}

Value *ValueUnit::max(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(MaxFunction::REPR);
  return 0;
}

Value *ValueUnit::min(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  unsupportedOp(MinFunction::REPR);
  return 0;
}

const ValueUnit *ValueUnit::checkSameType(const Value *val) throw(INTERP_KERNEL::Exception)
{
  const ValueUnit *valC=dynamic_cast<const ValueUnit *>(val);
  if(!valC)
    throw INTERP_KERNEL::Exception("Trying to operate on non homogeneous Values (Units with other type) !");
  return valC;
}

void ValueUnit::unsupportedOp(const char *type) throw(INTERP_KERNEL::Exception)
{
  const char msg[]="Unsupported operation for units :";
  std::string msgStr(msg);
  msgStr+=type;
  throw INTERP_KERNEL::Exception(msgStr.c_str());
}

ValueDoubleExpr::ValueDoubleExpr(int szDestData, const double *srcData):_sz_dest_data(szDestData),_dest_data(new double[_sz_dest_data]),_src_data(srcData)
{
}

ValueDoubleExpr::~ValueDoubleExpr()
{
  delete [] _dest_data;
}

Value *ValueDoubleExpr::newInstance() const
{
  return new ValueDoubleExpr(_sz_dest_data,_src_data);
}

void ValueDoubleExpr::setDouble(double val) throw(INTERP_KERNEL::Exception)
{
  std::fill(_dest_data,_dest_data+_sz_dest_data,val);
}

void ValueDoubleExpr::setVarname(int fastPos, const std::string& var) throw(INTERP_KERNEL::Exception)
{
  if(fastPos==-2)
    std::copy(_src_data,_src_data+_sz_dest_data,_dest_data);
  else if(fastPos>-2)
    std::fill(_dest_data,_dest_data+_sz_dest_data,_src_data[fastPos]);
  else
    {
      std::fill(_dest_data,_dest_data+_sz_dest_data,0.);
      _dest_data[-7-fastPos]=1.;
    }
}

void ValueDoubleExpr::positive() throw(INTERP_KERNEL::Exception)
{
}

void ValueDoubleExpr::negate() throw(INTERP_KERNEL::Exception)
{
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::negate<double>());
}

void ValueDoubleExpr::sqrt() throw(INTERP_KERNEL::Exception)
{
  double *it=std::find_if(_dest_data,_dest_data+_sz_dest_data,std::bind2nd(std::less<double>(),0.));
  if(it!=_dest_data+_sz_dest_data)
    throw INTERP_KERNEL::Exception("Trying to apply sqrt on < 0. value !");
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::ptr_fun<double,double>(std::sqrt));
}

void ValueDoubleExpr::cos() throw(INTERP_KERNEL::Exception)
{
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::ptr_fun<double,double>(std::cos));
}

void ValueDoubleExpr::sin() throw(INTERP_KERNEL::Exception)
{
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::ptr_fun<double,double>(std::sin));
}

void ValueDoubleExpr::tan() throw(INTERP_KERNEL::Exception)
{
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::ptr_fun<double,double>(std::tan));
}

void ValueDoubleExpr::abs() throw(INTERP_KERNEL::Exception)
{
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::ptr_fun(fabs));
}

void ValueDoubleExpr::exp() throw(INTERP_KERNEL::Exception)
{
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::ptr_fun<double,double>(std::exp));
}

void ValueDoubleExpr::ln() throw(INTERP_KERNEL::Exception)
{
  double *it=std::find_if(_dest_data,_dest_data+_sz_dest_data,std::bind2nd(std::less_equal<double>(),0.));
  if(it!=_dest_data+_sz_dest_data)
    throw INTERP_KERNEL::Exception("Trying to apply sqrt on < 0. value !");
  std::transform(_dest_data,_dest_data+_sz_dest_data,_dest_data,std::ptr_fun<double,double>(std::log));
}

Value *ValueDoubleExpr::plus(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDoubleExpr *otherC=static_cast<const ValueDoubleExpr *>(other);
  ValueDoubleExpr *ret=new ValueDoubleExpr(_sz_dest_data,_src_data);
  std::transform(_dest_data,_dest_data+_sz_dest_data,otherC->getData(),ret->getData(),std::plus<double>());
  return ret;
}

Value *ValueDoubleExpr::minus(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDoubleExpr *otherC=static_cast<const ValueDoubleExpr *>(other);
  ValueDoubleExpr *ret=new ValueDoubleExpr(_sz_dest_data,_src_data);
  std::transform(_dest_data,_dest_data+_sz_dest_data,otherC->getData(),ret->getData(),std::minus<double>());
  return ret;
}

Value *ValueDoubleExpr::mult(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDoubleExpr *otherC=static_cast<const ValueDoubleExpr *>(other);
  ValueDoubleExpr *ret=new ValueDoubleExpr(_sz_dest_data,_src_data);
  std::transform(_dest_data,_dest_data+_sz_dest_data,otherC->getData(),ret->getData(),std::multiplies<double>());
  return ret;
}

Value *ValueDoubleExpr::div(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDoubleExpr *otherC=static_cast<const ValueDoubleExpr *>(other);
  double *it=std::find(otherC->getData(),otherC->getData()+_sz_dest_data,0.);
  if(it!=otherC->getData()+_sz_dest_data)
    throw INTERP_KERNEL::Exception("Trying to operate division by 0. !");
  ValueDoubleExpr *ret=new ValueDoubleExpr(_sz_dest_data,_src_data);
  std::transform(_dest_data,_dest_data+_sz_dest_data,otherC->getData(),ret->getData(),std::divides<double>());
  return ret;
}

Value *ValueDoubleExpr::pow(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDoubleExpr *otherC=static_cast<const ValueDoubleExpr *>(other);
  double p=otherC->getData()[0];
  double *it=std::find_if(_dest_data,_dest_data+_sz_dest_data,std::bind2nd(std::less<double>(),0.));
  if(it!=_dest_data+_sz_dest_data)
    throw INTERP_KERNEL::Exception("Trying to operate pow(a,b) with a<0. !");
  ValueDoubleExpr *ret=new ValueDoubleExpr(_sz_dest_data,_src_data);
  std::transform(_dest_data,_dest_data+_sz_dest_data,ret->getData(),std::bind2nd(std::ptr_fun(powl),p));
  return ret;
}

Value *ValueDoubleExpr::max(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDoubleExpr *otherC=static_cast<const ValueDoubleExpr *>(other);
  ValueDoubleExpr *ret=new ValueDoubleExpr(_sz_dest_data,_src_data);
  std::transform(_dest_data,_dest_data+_sz_dest_data,otherC->getData(),ret->getData(),std::ptr_fun(fmax));
  return ret;
}

Value *ValueDoubleExpr::min(const Value *other) const throw(INTERP_KERNEL::Exception)
{
  const ValueDoubleExpr *otherC=static_cast<const ValueDoubleExpr *>(other);
  ValueDoubleExpr *ret=new ValueDoubleExpr(_sz_dest_data,_src_data);
  std::transform(_dest_data,_dest_data+_sz_dest_data,otherC->getData(),ret->getData(),std::ptr_fun(fmin));
  return ret;
}
