// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#include "MEDCouplingMemArray.txx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"

#include <set>
#include <cmath>
#include <limits>
#include <numeric>
#include <algorithm>
#include <functional>

using namespace ParaMEDMEM;

/*!
 * Returns a new instance of DataArrayChar. The caller is to delete this array
 * using decrRef() as it is no more needed. 
 */
DataArrayChar *DataArrayChar::New()
{
  return new DataArrayChar;
}

/*!
 * Checks if raw data is allocated. Read more on the raw data
 * in \ref MEDCouplingArrayBasicsTuplesAndCompo "DataArrays infos" for more information.
 *  \return bool - \a true if the raw data is allocated, \a false else.
 */
bool DataArrayChar::isAllocated() const
{
  return getConstPointer()!=0;
}

/*!
 * Checks if raw data is allocated and throws an exception if it is not the case.
 *  \throw If the raw data is not allocated.
 */
void DataArrayChar::checkAllocated() const throw(INTERP_KERNEL::Exception)
{
  if(!isAllocated())
    throw INTERP_KERNEL::Exception("DataArrayChar::checkAllocated : Array is defined but not allocated ! Call alloc or setValues method first !");
}

std::size_t DataArrayChar::getHeapMemorySize() const
{
  std::size_t sz=(std::size_t)_mem.getNbOfElemAllocated();
  return DataArray::getHeapMemorySize()+sz;
}

/*!
 * Sets information on all components. This method can change number of components
 * at certain conditions; if the conditions are not respected, an exception is thrown.
 * The number of components can be changed provided that \a this is not allocated.
 *
 * To know more on format of the component information see
 * \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] info - a vector of component infos.
 *  \throw If \a this->getNumberOfComponents() != \a info.size() && \a this->isAllocated()
 */
void DataArrayChar::setInfoAndChangeNbOfCompo(const std::vector<std::string>& info) throw(INTERP_KERNEL::Exception)
{
  if(getNumberOfComponents()!=(int)info.size())
    {
      if(!isAllocated())
        _info_on_compo=info;
      else
        {
          std::ostringstream oss; oss << "DataArrayChar::setInfoAndChangeNbOfCompo : input is of size " << info.size() << " whereas number of components is equal to " << getNumberOfComponents() << "  and this is already allocated !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  else
    _info_on_compo=info;
}

/*!
 * Returns the only one value in \a this, if and only if number of elements
 * (nb of tuples * nb of components) is equal to 1, and that \a this is allocated.
 *  \return double - the sole value stored in \a this array.
 *  \throw If at least one of conditions stated above is not fulfilled.
 */
char DataArrayChar::charValue() const throw(INTERP_KERNEL::Exception)
{
  if(isAllocated())
    {
      if(getNbOfElems()==1)
        {
          return *getConstPointer();
        }
      else
        throw INTERP_KERNEL::Exception("DataArrayChar::charValue : DataArrayChar instance is allocated but number of elements is not equal to 1 !");
    }
  else
    throw INTERP_KERNEL::Exception("DataArrayChar::charValue : DataArrayChar instance is not allocated !");
}

/*!
 * Returns an integer value characterizing \a this array, which is useful for a quick
 * comparison of many instances of DataArrayInt.
 *  \return int - the hash value.
 *  \throw If \a this is not allocated.
 */
int DataArrayChar::getHashCode() const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  int nbOfElems=getNbOfElems();
  int ret=nbOfElems*65536;
  int delta=3;
  if(nbOfElems>48)
    delta=nbOfElems/8;
  int ret0=0;
  const char *pt=begin();
  for(int i=0;i<nbOfElems;i+=delta)
    ret0+=pt[i];
  return ret+ret0;
}

/*!
 * Checks the number of tuples.
 *  \return bool - \a true if getNumberOfTuples() == 0, \a false else.
 *  \throw If \a this is not allocated.
 */
bool DataArrayChar::empty() const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  return getNumberOfTuples()==0;
}

/*!
 * Returns a full copy of \a this. For more info on copying data arrays see
 * \ref MEDCouplingArrayBasicsCopyDeep.
 *  \return DataArrayChar * - a new instance of DataArrayChar.
 */
DataArrayChar *DataArrayChar::deepCpy() const
{
  return new DataArrayChar(*this);
}

/*!
 * Returns either a \a deep or \a shallow copy of this array. For more info see
 * \ref MEDCouplingArrayBasicsCopyDeep and \ref MEDCouplingArrayBasicsCopyShallow.
 *  \param [in] dCpy - if \a true, a deep copy is returned, else, a shallow one.
 *  \return DataArrayChar * - either a new instance of DataArrayChar (if \a dCpy
 *          == \a true) or \a this instance (if \a dCpy == \a false).
 */
DataArrayChar *DataArrayChar::performCpy(bool dCpy) const
{
  if(dCpy)
    return deepCpy();
  else
    {
      incrRef();
      return const_cast<DataArrayChar *>(this);
    }
}

/*!
 * Copies all the data from another DataArrayChar. For more info see
 * \ref MEDCouplingArrayBasicsCopyDeepAssign.
 *  \param [in] other - another instance of DataArrayChar to copy data from.
 *  \throw If the \a other is not allocated.
 */
void DataArrayChar::cpyFrom(const DataArrayChar& other) throw(INTERP_KERNEL::Exception)
{
  other.checkAllocated();
  int nbOfTuples=other.getNumberOfTuples();
  int nbOfComp=other.getNumberOfComponents();
  allocIfNecessary(nbOfTuples,nbOfComp);
  int nbOfElems=nbOfTuples*nbOfComp;
  char *pt=getPointer();
  const char *ptI=other.getConstPointer();
  for(int i=0;i<nbOfElems;i++)
    pt[i]=ptI[i];
  copyStringInfoFrom(other);
}

/*!
 * This method reserve nbOfElems elements in memory ( nbOfElems*4 bytes ) \b without impacting the number of tuples in \a this.
 * If \a this has already been allocated, this method checks that \a this has only one component. If not an INTERP_KERNEL::Exception will be thrown.
 * If \a this has not already been allocated, number of components is set to one.
 * This method allows to reduce number of reallocations on invokation of DataArrayChar::pushBackSilent and DataArrayChar::pushBackValsSilent on \a this.
 * 
 * \sa DataArrayChar::pack, DataArrayChar::pushBackSilent, DataArrayChar::pushBackValsSilent
 */
void DataArrayChar::reserve(int nbOfElems) throw(INTERP_KERNEL::Exception)
{
  int nbCompo=getNumberOfComponents();
  if(nbCompo==1)
    {
      _mem.reserve(nbOfElems);
    }
  else if(nbCompo==0)
    {
      _mem.reserve(nbOfElems);
      _info_on_compo.resize(1);
    }
  else
    throw INTERP_KERNEL::Exception("DataArrayChar::reserve : not available for DataArrayChar with number of components different than 1 !");
}

/*!
 * This method adds at the end of \a this the single value \a val. This method do \b not update its time label to avoid useless incrementation
 * of counter. So the caller is expected to call TimeLabel::declareAsNew on \a this at the end of the push session.
 *
 * \param [in] val the value to be added in \a this
 * \throw If \a this has already been allocated with number of components different from one.
 * \sa DataArrayChar::pushBackValsSilent
 */
void DataArrayChar::pushBackSilent(char val) throw(INTERP_KERNEL::Exception)
{
  int nbCompo=getNumberOfComponents();
  if(nbCompo==1)
    _mem.pushBack(val);
  else if(nbCompo==0)
    {
      _info_on_compo.resize(1);
      _mem.pushBack(val);
    }
  else
    throw INTERP_KERNEL::Exception("DataArrayChar::pushBackSilent : not available for DataArrayChar with number of components different than 1 !");
}

/*!
 * This method adds at the end of \a this a serie of values [\c valsBg,\c valsEnd). This method do \b not update its time label to avoid useless incrementation
 * of counter. So the caller is expected to call TimeLabel::declareAsNew on \a this at the end of the push session.
 *
 *  \param [in] valsBg - an array of values to push at the end of \this.
 *  \param [in] valsEnd - specifies the end of the array \a valsBg, so that
 *              the last value of \a valsBg is \a valsEnd[ -1 ].
 * \throw If \a this has already been allocated with number of components different from one.
 * \sa DataArrayChar::pushBackSilent
 */
void DataArrayChar::pushBackValsSilent(const char *valsBg, const char *valsEnd) throw(INTERP_KERNEL::Exception)
{
  int nbCompo=getNumberOfComponents();
  if(nbCompo==1)
    _mem.insertAtTheEnd(valsBg,valsEnd);
  else if(nbCompo==0)
    {
      _info_on_compo.resize(1);
      _mem.insertAtTheEnd(valsBg,valsEnd);
    }
  else
    throw INTERP_KERNEL::Exception("DataArrayChar::pushBackValsSilent : not available for DataArrayChar with number of components different than 1 !");
}

/*!
 * This method returns silently ( without updating time label in \a this ) the last value, if any and suppress it.
 * \throw If \a this is already empty.
 * \throw If \a this has number of components different from one.
 */
char DataArrayChar::popBackSilent() throw(INTERP_KERNEL::Exception)
{
  if(getNumberOfComponents()==1)
    return _mem.popBack();
  else
    throw INTERP_KERNEL::Exception("DataArrayChar::popBackSilent : not available for DataArrayChar with number of components different than 1 !");
}

/*!
 * This method \b do \b not modify content of \a this. It only modify its memory footprint if the allocated memory is to high regarding real data to store.
 *
 * \sa DataArrayChar::getHeapMemorySize, DataArrayChar::reserve
 */
void DataArrayChar::pack() const throw(INTERP_KERNEL::Exception)
{
  _mem.pack();
}

/*!
 * Allocates the raw data in memory. If exactly as same memory as needed already
 * allocated, it is not re-allocated.
 *  \param [in] nbOfTuple - number of tuples of data to allocate.
 *  \param [in] nbOfCompo - number of components of data to allocate.
 *  \throw If \a nbOfTuple < 0 or \a nbOfCompo < 0.
 */
void DataArrayChar::allocIfNecessary(int nbOfTuple, int nbOfCompo)
{
  if(isAllocated())
    {
      if(nbOfTuple!=getNumberOfTuples() || nbOfCompo!=getNumberOfComponents())
        alloc(nbOfTuple,nbOfCompo);
    }
  else
    alloc(nbOfTuple,nbOfCompo);
}

/*!
 * Allocates the raw data in memory. If the memory was already allocated, then it is
 * freed and re-allocated. See an example of this method use
 * \ref MEDCouplingArraySteps1WC "here".
 *  \param [in] nbOfTuple - number of tuples of data to allocate.
 *  \param [in] nbOfCompo - number of components of data to allocate.
 *  \throw If \a nbOfTuple < 0 or \a nbOfCompo < 0.
 */
void DataArrayChar::alloc(int nbOfTuple, int nbOfCompo) throw(INTERP_KERNEL::Exception)
{
  if(nbOfTuple<0 || nbOfCompo<0)
    throw INTERP_KERNEL::Exception("DataArrayChar::alloc : request for negative length of data !");
  _info_on_compo.resize(nbOfCompo);
  _mem.alloc(nbOfCompo*nbOfTuple);
  declareAsNew();
}

/*!
 * Checks if \a this and another DataArrayChar are fully equal. For more info see
 * \ref MEDCouplingArrayBasicsCompare.
 *  \param [in] other - an instance of DataArrayChar to compare with \a this one.
 *  \return bool - \a true if the two arrays are equal, \a false else.
 */
bool DataArrayChar::isEqual(const DataArrayChar& other) const
{
  std::string tmp;
  return isEqualIfNotWhy(other,tmp);
}

/*!
 * Equivalent to DataArrayChar::isEqual except that if false the reason of
 * mismatch is given.
 * 
 * \param [in] other the instance to be compared with \a this
 * \param [out] reason In case of inequality returns the reason.
 * \sa DataArrayChar::isEqual
 */
bool DataArrayChar::isEqualIfNotWhy(const DataArrayChar& other, std::string& reason) const
{
  if(!areInfoEqualsIfNotWhy(other,reason))
    return false;
  return _mem.isEqual(other._mem,0,reason);
}

/*!
 * Checks if values of \a this and another DataArrayChar are equal. For more info see
 * \ref MEDCouplingArrayBasicsCompare.
 *  \param [in] other - an instance of DataArrayChar to compare with \a this one.
 *  \return bool - \a true if the values of two arrays are equal, \a false else.
 */
bool DataArrayChar::isEqualWithoutConsideringStr(const DataArrayChar& other) const
{
  std::string tmp;
  return _mem.isEqual(other._mem,0,tmp);
}

/*!
 * Assign zero to all values in \a this array. To know more on filling arrays see
 * \ref MEDCouplingArrayFill.
 * \throw If \a this is not allocated.
 */
void DataArrayChar::fillWithZero() throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  _mem.fillWithValue(0);
  declareAsNew();
}

/*!
 * Assign \a val to all values in \a this array. To know more on filling arrays see
 * \ref MEDCouplingArrayFill.
 *  \param [in] val - the value to fill with.
 *  \throw If \a this is not allocated.
 */
void DataArrayChar::fillWithValue(char val) throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  _mem.fillWithValue(val);
  declareAsNew();
}

/*!
 * Returns a textual and human readable representation of \a this instance of
 * DataArrayChar. This text is shown when a DataArrayChar is printed in Python.
 *  \return std::string - text describing \a this DataArrayChar.
 */
std::string DataArrayChar::repr() const
{
  std::ostringstream ret;
  reprStream(ret);
  return ret.str();
}

std::string DataArrayChar::reprZip() const
{
  std::ostringstream ret;
  reprZipStream(ret);
  return ret.str();
}

void DataArrayChar::reprStream(std::ostream& stream) const
{
  stream << "Name of char array : \"" << _name << "\"\n";
  reprWithoutNameStream(stream);
}

void DataArrayChar::reprZipStream(std::ostream& stream) const
{
  stream << "Name of char array : \"" << _name << "\"\n";
  reprZipWithoutNameStream(stream);
}

void DataArrayChar::reprWithoutNameStream(std::ostream& stream) const
{
  DataArray::reprWithoutNameStream(stream);
  _mem.repr(getNumberOfComponents(),stream);
}

void DataArrayChar::reprZipWithoutNameStream(std::ostream& stream) const
{
  DataArray::reprWithoutNameStream(stream);
  _mem.reprZip(getNumberOfComponents(),stream);
}

void DataArrayChar::reprCppStream(const char *varName, std::ostream& stream) const
{
  int nbTuples=getNumberOfTuples(),nbComp=getNumberOfComponents();
  const char *data=getConstPointer();
  stream << "DataArrayChar *" << varName << "=DataArrayChar::New();" << std::endl;
  if(nbTuples*nbComp>=1)
    {
      stream << "const char " << varName << "Data[" << nbTuples*nbComp << "]={";
      std::copy(data,data+nbTuples*nbComp-1,std::ostream_iterator<char>(stream,","));
      stream << data[nbTuples*nbComp-1] << "};" << std::endl;
      stream << varName << "->useArray(" << varName << "Data,false,CPP_DEALLOC," << nbTuples << "," << nbComp << ");" << std::endl;
    }
  else
    stream << varName << "->alloc(" << nbTuples << "," << nbComp << ");" << std::endl;
  stream << varName << "->setName(\"" << getName() << "\");" << std::endl;
}

/*!
 * Changes number of tuples in the array. If the new number of tuples is smaller
 * than the current number the array is truncated, otherwise the array is extended.
 *  \param [in] nbOfTuples - new number of tuples. 
 *  \throw If \a this is not allocated.
 */
void DataArrayChar::reAlloc(int nbOfTuples) throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  _mem.reAlloc(getNumberOfComponents()*nbOfTuples);
  declareAsNew();
}

/*!
 * Creates a new DataArrayInt and assigns all (textual and numerical) data of \a this
 * array to the new one.
 *  \return DataArrayInt * - the new instance of DataArrayChar.
 */
DataArrayInt *DataArrayChar::convertToIntArr() const
{
  checkAllocated();
  DataArrayInt *ret=DataArrayInt::New();
  ret->alloc(getNumberOfTuples(),getNumberOfComponents());
  int nbOfVals=getNbOfElems();
  const char *src=getConstPointer();
  int *dest=ret->getPointer();
  std::copy(src,src+nbOfVals,dest);
  ret->copyStringInfoFrom(*this);
  return ret;
}

/*!
 * Permutes values of \a this array as required by \a old2New array. The values are
 * permuted so that \c new[ \a old2New[ i ]] = \c old[ i ]. Number of tuples remains
 * the same as in \this one.
 * If a permutation reduction is needed, substr() or selectByTupleId() should be used.
 * For more info on renumbering see \ref MEDCouplingArrayRenumbering.
 *  \param [in] old2New - C array of length equal to \a this->getNumberOfTuples()
 *     giving a new position for i-th old value.
 */
void DataArrayChar::renumberInPlace(const int *old2New)
{
  checkAllocated();
  int nbTuples=getNumberOfTuples();
  int nbOfCompo=getNumberOfComponents();
  char *tmp=new char[nbTuples*nbOfCompo];
  const char *iptr=getConstPointer();
  for(int i=0;i<nbTuples;i++)
    std::copy(iptr+nbOfCompo*i,iptr+nbOfCompo*(i+1),tmp+nbOfCompo*old2New[i]);
  std::copy(tmp,tmp+nbTuples*nbOfCompo,getPointer());
  delete [] tmp;
  declareAsNew();
}

/*!
 * Permutes values of \a this array as required by \a new2Old array. The values are
 * permuted so that \c new[ i ] = \c old[ \a new2Old[ i ]]. Number of tuples remains
 * the same as in \this one.
 * For more info on renumbering see \ref MEDCouplingArrayRenumbering.
 *  \param [in] new2Old - C array of length equal to \a this->getNumberOfTuples()
 *     giving a previous position of i-th new value.
 */
void DataArrayChar::renumberInPlaceR(const int *new2Old)
{
  checkAllocated();
  int nbTuples=getNumberOfTuples();
  int nbOfCompo=getNumberOfComponents();
  char *tmp=new char[nbTuples*nbOfCompo];
  const char *iptr=getConstPointer();
  for(int i=0;i<nbTuples;i++)
    std::copy(iptr+nbOfCompo*new2Old[i],iptr+nbOfCompo*(new2Old[i]+1),tmp+nbOfCompo*i);
  std::copy(tmp,tmp+nbTuples*nbOfCompo,getPointer());
  delete [] tmp;
  declareAsNew();
}

/*!
 * Returns a copy of \a this array with values permuted as required by \a old2New array.
 * The values are permuted so that  \c new[ \a old2New[ i ]] = \c old[ i ].
 * Number of tuples in the result array remains the same as in \this one.
 * If a permutation reduction is needed, renumberAndReduce() should be used.
 * For more info on renumbering see \ref MEDCouplingArrayRenumbering.
 *  \param [in] old2New - C array of length equal to \a this->getNumberOfTuples()
 *          giving a new position for i-th old value.
 *  \return DataArrayChar * - the new instance of DataArrayChar that the caller
 *          is to delete using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 */
DataArrayChar *DataArrayChar::renumber(const int *old2New) const
{
  checkAllocated();
  int nbTuples=getNumberOfTuples();
  int nbOfCompo=getNumberOfComponents();
  DataArrayChar *ret=DataArrayChar::New();
  ret->alloc(nbTuples,nbOfCompo);
  ret->copyStringInfoFrom(*this);
  const char *iptr=getConstPointer();
  char *optr=ret->getPointer();
  for(int i=0;i<nbTuples;i++)
    std::copy(iptr+nbOfCompo*i,iptr+nbOfCompo*(i+1),optr+nbOfCompo*old2New[i]);
  ret->copyStringInfoFrom(*this);
  return ret;
}

/*!
 * Returns a copy of \a this array with values permuted as required by \a new2Old array.
 * The values are permuted so that  \c new[ i ] = \c old[ \a new2Old[ i ]]. Number of
 * tuples in the result array remains the same as in \this one.
 * If a permutation reduction is needed, substr() or selectByTupleId() should be used.
 * For more info on renumbering see \ref MEDCouplingArrayRenumbering.
 *  \param [in] new2Old - C array of length equal to \a this->getNumberOfTuples()
 *     giving a previous position of i-th new value.
 *  \return DataArrayChar * - the new instance of DataArrayChar that the caller
 *          is to delete using decrRef() as it is no more needed.
 */
DataArrayChar *DataArrayChar::renumberR(const int *new2Old) const
{
  checkAllocated();
  int nbTuples=getNumberOfTuples();
  int nbOfCompo=getNumberOfComponents();
  DataArrayChar *ret=DataArrayChar::New();
  ret->alloc(nbTuples,nbOfCompo);
  ret->copyStringInfoFrom(*this);
  const char *iptr=getConstPointer();
  char *optr=ret->getPointer();
  for(int i=0;i<nbTuples;i++)
    std::copy(iptr+nbOfCompo*new2Old[i],iptr+nbOfCompo*(new2Old[i]+1),optr+nbOfCompo*i);
  ret->copyStringInfoFrom(*this);
  return ret;
}

/*!
 * Returns a shorten and permuted copy of \a this array. The new DataArrayChar is
 * of size \a newNbOfTuple and it's values are permuted as required by \a old2New array.
 * The values are permuted so that  \c new[ \a old2New[ i ]] = \c old[ i ] for all
 * \a old2New[ i ] >= 0. In other words every i-th tuple in \a this array, for which 
 * \a old2New[ i ] is negative, is missing from the result array.
 * For more info on renumbering see \ref MEDCouplingArrayRenumbering.
 *  \param [in] old2New - C array of length equal to \a this->getNumberOfTuples()
 *     giving a new position for i-th old tuple and giving negative position for
 *     for i-th old tuple that should be omitted.
 *  \return DataArrayChar * - the new instance of DataArrayChar that the caller
 *          is to delete using decrRef() as it is no more needed.
 */
DataArrayChar *DataArrayChar::renumberAndReduce(const int *old2New, int newNbOfTuple) const
{
  checkAllocated();
  int nbTuples=getNumberOfTuples();
  int nbOfCompo=getNumberOfComponents();
  DataArrayChar *ret=DataArrayChar::New();
  ret->alloc(newNbOfTuple,nbOfCompo);
  const char *iptr=getConstPointer();
  char *optr=ret->getPointer();
  for(int i=0;i<nbTuples;i++)
    {
      int w=old2New[i];
      if(w>=0)
        std::copy(iptr+i*nbOfCompo,iptr+(i+1)*nbOfCompo,optr+w*nbOfCompo);
    }
  ret->copyStringInfoFrom(*this);
  return ret;
}

/*!
 * Returns a shorten and permuted copy of \a this array. The new DataArrayChar is
 * of size \a new2OldEnd - \a new2OldBg and it's values are permuted as required by
 * \a new2OldBg array.
 * The values are permuted so that  \c new[ i ] = \c old[ \a new2OldBg[ i ]].
 * This method is equivalent to renumberAndReduce() except that convention in input is
 * \c new2old and \b not \c old2new.
 * This method is equivalent to selectByTupleId() except that it prevents coping data
 * from behind the end of \a this array.
 * For more info on renumbering see \ref MEDCouplingArrayRenumbering.
 *  \param [in] new2OldBg - pointer to the beginning of a permutation array that gives a
 *              tuple index in \a this array to fill the i-th tuple in the new array.
 *  \param [in] new2OldEnd - specifies the end of the permutation array that starts at
 *              \a new2OldBg, so that pointer to a tuple index (\a pi) varies as this:
 *              \a new2OldBg <= \a pi < \a new2OldEnd.
 *  \return DataArrayChar * - the new instance of DataArrayChar that the caller
 *          is to delete using decrRef() as it is no more needed.
 *  \throw If \a new2OldEnd - \a new2OldBg > \a this->getNumberOfTuples().
 */
DataArrayChar *DataArrayChar::selectByTupleIdSafe(const int *new2OldBg, const int *new2OldEnd) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  MEDCouplingAutoRefCountObjectPtr<DataArrayChar> ret=DataArrayChar::New();
  int nbComp=getNumberOfComponents();
  int oldNbOfTuples=getNumberOfTuples();
  ret->alloc((int)std::distance(new2OldBg,new2OldEnd),nbComp);
  ret->copyStringInfoFrom(*this);
  char *pt=ret->getPointer();
  const char *srcPt=getConstPointer();
  int i=0;
  for(const int *w=new2OldBg;w!=new2OldEnd;w++,i++)
    if(*w>=0 && *w<oldNbOfTuples)
      std::copy(srcPt+(*w)*nbComp,srcPt+((*w)+1)*nbComp,pt+i*nbComp);
    else
      throw INTERP_KERNEL::Exception("DataArrayChar::selectByTupleIdSafe : some ids has been detected to be out of [0,this->getNumberOfTuples) !");
  ret->copyStringInfoFrom(*this);
  return ret.retn();
}

/*!
 * Returns a shorten copy of \a this array. The new DataArrayChar contains every
 * (\a bg + \c i * \a step)-th tuple of \a this array located before the \a end2-th
 * tuple. Indices of the selected tuples are the same as ones returned by the Python
 * command \c range( \a bg, \a end2, \a step ).
 * This method is equivalent to selectByTupleIdSafe() except that the input array is
 * not constructed explicitly.
 * For more info on renumbering see \ref MEDCouplingArrayRenumbering.
 *  \param [in] bg - index of the first tuple to copy from \a this array.
 *  \param [in] end2 - index of the tuple before which the tuples to copy are located.
 *  \param [in] step - index increment to get index of the next tuple to copy.
 *  \return DataArrayChar * - the new instance of DataArrayChar that the caller
 *          is to delete using decrRef() as it is no more needed.
 *  \throw If (\a end2 < \a bg) or (\a step <= 0).
 *  \sa DataArrayChar::substr.
 */
DataArrayChar *DataArrayChar::selectByTupleId2(int bg, int end2, int step) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  MEDCouplingAutoRefCountObjectPtr<DataArrayChar> ret=DataArrayChar::New();
  int nbComp=getNumberOfComponents();
  int newNbOfTuples=GetNumberOfItemGivenBES(bg,end2,step,"DataArrayInt::selectByTupleId2 : ");
  ret->alloc(newNbOfTuples,nbComp);
  char *pt=ret->getPointer();
  const char *srcPt=getConstPointer()+bg*nbComp;
  for(int i=0;i<newNbOfTuples;i++,srcPt+=step*nbComp)
    std::copy(srcPt,srcPt+nbComp,pt+i*nbComp);
  ret->copyStringInfoFrom(*this);
  return ret.retn();
}

/*!
 * Checks if all values in \a this array are equal to \a val.
 *  \param [in] val - value to check equality of array values to.
 *  \return bool - \a true if all values are \a val.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1
 */
bool DataArrayChar::isUniform(char val) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::isUniform : must be applied on DataArrayChar with only one component, you can call 'rearrange' method before !");
  int nbOfTuples=getNumberOfTuples();
  const char *w=getConstPointer();
  const char *end2=w+nbOfTuples;
  for(;w!=end2;w++)
    if(*w!=val)
      return false;
  return true;
}

/*!
 * Changes the number of components within \a this array so that its raw data **does
 * not** change, instead splitting this data into tuples changes.
 *  \param [in] newNbOfComp - number of components for \a this array to have.
 *  \throw If \a this is not allocated
 *  \throw If getNbOfElems() % \a newNbOfCompo != 0.
 *  \warning This method erases all (name and unit) component info set before!
 */
void DataArrayChar::rearrange(int newNbOfCompo) throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  int nbOfElems=getNbOfElems();
  if(nbOfElems%newNbOfCompo!=0)
    throw INTERP_KERNEL::Exception("DataArrayChar::rearrange : nbOfElems%newNbOfCompo!=0 !");
  _info_on_compo.clear();
  _info_on_compo.resize(newNbOfCompo);
  declareAsNew();
}

/*!
 * Returns a shorten or extended copy of \a this array. If \a newNbOfComp is less
 * than \a this->getNumberOfComponents() then the result array is shorten as each tuple
 * is truncated to have \a newNbOfComp components, keeping first components. If \a
 * newNbOfComp is more than \a this->getNumberOfComponents() then the result array is
 * expanded as each tuple is populated with \a dftValue to have \a newNbOfComp
 * components.  
 *  \param [in] newNbOfComp - number of components for the new array to have.
 *  \param [in] dftValue - value assigned to new values added to the new array.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble that the caller
 *          is to delete using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 */
DataArrayChar *DataArrayChar::changeNbOfComponents(int newNbOfComp, char dftValue) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  DataArrayChar *ret=DataArrayChar::New();
  ret->alloc(getNumberOfTuples(),newNbOfComp);
  const char *oldc=getConstPointer();
  char *nc=ret->getPointer();
  int nbOfTuples=getNumberOfTuples();
  int oldNbOfComp=getNumberOfComponents();
  int dim=std::min(oldNbOfComp,newNbOfComp);
  for(int i=0;i<nbOfTuples;i++)
    {
      int j=0;
      for(;j<dim;j++)
        nc[newNbOfComp*i+j]=oldc[i*oldNbOfComp+j];
      for(;j<newNbOfComp;j++)
        nc[newNbOfComp*i+j]=dftValue;
    }
  ret->setName(getName().c_str());
  for(int i=0;i<dim;i++)
    ret->setInfoOnComponent(i,getInfoOnComponent(i).c_str());
  ret->setName(getName().c_str());
  return ret;
}

/*!
 * Returns a copy of \a this array composed of selected components.
 * The new DataArrayChar has the same number of tuples but includes components
 * specified by \a compoIds parameter. So that getNbOfElems() of the result array
 * can be either less, same or more than \a this->getNbOfElems().
 *  \param [in] compoIds - sequence of zero based indices of components to include
 *              into the new array.
 *  \return DataArrayChar * - the new instance of DataArrayChar that the caller
 *          is to delete using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If a component index (\a i) is not valid: 
 *         \a i < 0 || \a i >= \a this->getNumberOfComponents().
 *
 *  \ref cpp_mcdataarrayint_keepselectedcomponents "Here is a Python example".
 */
DataArrayChar *DataArrayChar::keepSelectedComponents(const std::vector<int>& compoIds) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  MEDCouplingAutoRefCountObjectPtr<DataArrayChar> ret(DataArrayChar::New());
  int newNbOfCompo=(int)compoIds.size();
  int oldNbOfCompo=getNumberOfComponents();
  for(std::vector<int>::const_iterator it=compoIds.begin();it!=compoIds.end();it++)
    DataArray::CheckValueInRange(oldNbOfCompo,(*it),"keepSelectedComponents invalid requested component");
  int nbOfTuples=getNumberOfTuples();
  ret->alloc(nbOfTuples,newNbOfCompo);
  ret->copyPartOfStringInfoFrom(*this,compoIds);
  const char *oldc=getConstPointer();
  char *nc=ret->getPointer();
  for(int i=0;i<nbOfTuples;i++)
    for(int j=0;j<newNbOfCompo;j++,nc++)
      *nc=oldc[i*oldNbOfCompo+compoIds[j]];
  return ret.retn();
}

/*!
 * Appends components of another array to components of \a this one, tuple by tuple.
 * So that the number of tuples of \a this array remains the same and the number of 
 * components increases.
 *  \param [in] other - the DataArrayChar to append to \a this one.
 *  \throw If \a this is not allocated.
 *  \throw If \a this and \a other arrays have different number of tuples.
 *
 *  \ref cpp_mcdataarrayint_meldwith "Here is a C++ example".
 *
 *  \ref py_mcdataarrayint_meldwith "Here is a Python example".
 */
void DataArrayChar::meldWith(const DataArrayChar *other) throw(INTERP_KERNEL::Exception)
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayChar::meldWith : DataArrayChar pointer in input is NULL !");
  checkAllocated();
  other->checkAllocated();
  int nbOfTuples=getNumberOfTuples();
  if(nbOfTuples!=other->getNumberOfTuples())
    throw INTERP_KERNEL::Exception("DataArrayChar::meldWith : mismatch of number of tuples !");
  int nbOfComp1=getNumberOfComponents();
  int nbOfComp2=other->getNumberOfComponents();
  char *newArr=new char[nbOfTuples*(nbOfComp1+nbOfComp2)];
  char *w=newArr;
  const char *inp1=getConstPointer();
  const char *inp2=other->getConstPointer();
  for(int i=0;i<nbOfTuples;i++,inp1+=nbOfComp1,inp2+=nbOfComp2)
    {
      w=std::copy(inp1,inp1+nbOfComp1,w);
      w=std::copy(inp2,inp2+nbOfComp2,w);
    }
  useArray(newArr,true,CPP_DEALLOC,nbOfTuples,nbOfComp1+nbOfComp2);
  std::vector<int> compIds(nbOfComp2);
  for(int i=0;i<nbOfComp2;i++)
    compIds[i]=nbOfComp1+i;
  copyPartOfStringInfoFrom2(compIds,*other);
}

/*!
 * Copy all values from another DataArrayChar into specified tuples and components
 * of \a this array. Textual data is not copied.
 * The tree parameters defining set of indices of tuples and components are similar to
 * the tree parameters of the Python function \c range(\c start,\c stop,\c step).
 *  \param [in] a - the array to copy values from.
 *  \param [in] bgTuples - index of the first tuple of \a this array to assign values to.
 *  \param [in] endTuples - index of the tuple before which the tuples to assign to
 *              are located.
 *  \param [in] stepTuples - index increment to get index of the next tuple to assign to.
 *  \param [in] bgComp - index of the first component of \a this array to assign values to.
 *  \param [in] endComp - index of the component before which the components to assign
 *              to are located.
 *  \param [in] stepComp - index increment to get index of the next component to assign to.
 *  \param [in] strictCompoCompare - if \a true (by default), then \a a->getNumberOfComponents() 
 *              must be equal to the number of columns to assign to, else an
 *              exception is thrown; if \a false, then it is only required that \a
 *              a->getNbOfElems() equals to number of values to assign to (this condition
 *              must be respected even if \a strictCompoCompare is \a true). The number of 
 *              values to assign to is given by following Python expression:
 *              \a nbTargetValues = 
 *              \c len(\c range(\a bgTuples,\a endTuples,\a stepTuples)) *
 *              \c len(\c range(\a bgComp,\a endComp,\a stepComp)).
 *  \throw If \a a is NULL.
 *  \throw If \a a is not allocated.
 *  \throw If \a this is not allocated.
 *  \throw If parameters specifying tuples and components to assign to do not give a
 *            non-empty range of increasing indices.
 *  \throw If \a a->getNbOfElems() != \a nbTargetValues.
 *  \throw If \a strictCompoCompare == \a true && \a a->getNumberOfComponents() !=
 *            \c len(\c range(\a bgComp,\a endComp,\a stepComp)).
 *
 *  \ref cpp_mcdataarrayint_setpartofvalues1 "Here is a Python example".
 */
void DataArrayChar::setPartOfValues1(const DataArrayChar *a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare) throw(INTERP_KERNEL::Exception)
{
  if(!a)
    throw INTERP_KERNEL::Exception("DataArrayChar::setPartOfValues1 : DataArrayChar pointer in input is NULL !");
  const char msg[]="DataArrayChar::setPartOfValues1";
  checkAllocated();
  a->checkAllocated();
  int newNbOfTuples=DataArray::GetNumberOfItemGivenBES(bgTuples,endTuples,stepTuples,msg);
  int newNbOfComp=DataArray::GetNumberOfItemGivenBES(bgComp,endComp,stepComp,msg);
  int nbComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  DataArray::CheckValueInRangeEx(nbOfTuples,bgTuples,endTuples,"invalid tuple value");
  DataArray::CheckValueInRangeEx(nbComp,bgComp,endComp,"invalid component value");
  bool assignTech=true;
  if(a->getNbOfElems()==newNbOfTuples*newNbOfComp)
    {
      if(strictCompoCompare)
        a->checkNbOfTuplesAndComp(newNbOfTuples,newNbOfComp,msg);
    }
  else
    {
      a->checkNbOfTuplesAndComp(1,newNbOfComp,msg);
      assignTech=false;
    }
  char *pt=getPointer()+bgTuples*nbComp+bgComp;
  const char *srcPt=a->getConstPointer();
  if(assignTech)
    {
      for(int i=0;i<newNbOfTuples;i++,pt+=stepTuples*nbComp)
        for(int j=0;j<newNbOfComp;j++,srcPt++)
          pt[j*stepComp]=*srcPt;
    }
  else
    {
      for(int i=0;i<newNbOfTuples;i++,pt+=stepTuples*nbComp)
        {
          const char *srcPt2=srcPt;
          for(int j=0;j<newNbOfComp;j++,srcPt2++)
            pt[j*stepComp]=*srcPt2;
        }
    }
}

/*!
 * Assign a given value to values at specified tuples and components of \a this array.
 * The tree parameters defining set of indices of tuples and components are similar to
 * the tree parameters of the Python function \c range(\c start,\c stop,\c step)..
 *  \param [in] a - the value to assign.
 *  \param [in] bgTuples - index of the first tuple of \a this array to assign to.
 *  \param [in] endTuples - index of the tuple before which the tuples to assign to
 *              are located.
 *  \param [in] stepTuples - index increment to get index of the next tuple to assign to.
 *  \param [in] bgComp - index of the first component of \a this array to assign to.
 *  \param [in] endComp - index of the component before which the components to assign
 *              to are located.
 *  \param [in] stepComp - index increment to get index of the next component to assign to.
 *  \throw If \a this is not allocated.
 *  \throw If parameters specifying tuples and components to assign to, do not give a
 *            non-empty range of increasing indices or indices are out of a valid range
 *            for \this array.
 *
 *  \ref cpp_mcdataarrayint_setpartofvaluessimple1 "Here is a Python example".
 */
void DataArrayChar::setPartOfValuesSimple1(char a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp) throw(INTERP_KERNEL::Exception)
{
  const char msg[]="DataArrayChar::setPartOfValuesSimple1";
  checkAllocated();
  int newNbOfTuples=DataArray::GetNumberOfItemGivenBES(bgTuples,endTuples,stepTuples,msg);
  int newNbOfComp=DataArray::GetNumberOfItemGivenBES(bgComp,endComp,stepComp,msg);
  int nbComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  DataArray::CheckValueInRangeEx(nbOfTuples,bgTuples,endTuples,"invalid tuple value");
  DataArray::CheckValueInRangeEx(nbComp,bgComp,endComp,"invalid component value");
  char *pt=getPointer()+bgTuples*nbComp+bgComp;
  for(int i=0;i<newNbOfTuples;i++,pt+=stepTuples*nbComp)
    for(int j=0;j<newNbOfComp;j++)
      pt[j*stepComp]=a;
}


/*!
 * Copy all values from another DataArrayChar (\a a) into specified tuples and 
 * components of \a this array. Textual data is not copied.
 * The tuples and components to assign to are defined by C arrays of indices.
 * There are two *modes of usage*:
 * - If \a a->getNbOfElems() equals to number of values to assign to, then every value
 *   of \a a is assigned to its own location within \a this array. 
 * - If \a a includes one tuple, then all values of \a a are assigned to the specified
 *   components of every specified tuple of \a this array. In this mode it is required
 *   that \a a->getNumberOfComponents() equals to the number of specified components.
 * 
 *  \param [in] a - the array to copy values from.
 *  \param [in] bgTuples - pointer to an array of tuple indices of \a this array to
 *              assign values of \a a to.
 *  \param [in] endTuples - specifies the end of the array \a bgTuples, so that
 *              pointer to a tuple index <em>(pi)</em> varies as this: 
 *              \a bgTuples <= \a pi < \a endTuples.
 *  \param [in] bgComp - pointer to an array of component indices of \a this array to
 *              assign values of \a a to.
 *  \param [in] endComp - specifies the end of the array \a bgTuples, so that
 *              pointer to a component index <em>(pi)</em> varies as this: 
 *              \a bgComp <= \a pi < \a endComp.
 *  \param [in] strictCompoCompare - this parameter is checked only if the
 *               *mode of usage* is the first; if it is \a true (default), 
 *               then \a a->getNumberOfComponents() must be equal 
 *               to the number of specified columns, else this is not required.
 *  \throw If \a a is NULL.
 *  \throw If \a a is not allocated.
 *  \throw If \a this is not allocated.
 *  \throw If any index of tuple/component given by <em>bgTuples / bgComp</em> is
 *         out of a valid range for \a this array.
 *  \throw In the first *mode of usage*, if <em>strictCompoCompare == true </em> and
 *         if <em> a->getNumberOfComponents() != (endComp - bgComp) </em>.
 *  \throw In the second *mode of usage*, if \a a->getNumberOfTuples() != 1 or
 *         <em> a->getNumberOfComponents() != (endComp - bgComp)</em>.
 *
 *  \ref cpp_mcdataarrayint_setpartofvalues2 "Here is a Python example".
 */
void DataArrayChar::setPartOfValues2(const DataArrayChar *a, const int *bgTuples, const int *endTuples, const int *bgComp, const int *endComp, bool strictCompoCompare) throw(INTERP_KERNEL::Exception)
{
  if(!a)
    throw INTERP_KERNEL::Exception("DataArrayChar::setPartOfValues2 : DataArrayChar pointer in input is NULL !");
  const char msg[]="DataArrayChar::setPartOfValues2";
  checkAllocated();
  a->checkAllocated();
  int nbComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  for(const int *z=bgComp;z!=endComp;z++)
    DataArray::CheckValueInRange(nbComp,*z,"invalid component id");
  int newNbOfTuples=(int)std::distance(bgTuples,endTuples);
  int newNbOfComp=(int)std::distance(bgComp,endComp);
  bool assignTech=true;
  if(a->getNbOfElems()==newNbOfTuples*newNbOfComp)
    {
      if(strictCompoCompare)
        a->checkNbOfTuplesAndComp(newNbOfTuples,newNbOfComp,msg);
    }
  else
    {
      a->checkNbOfTuplesAndComp(1,newNbOfComp,msg);
      assignTech=false;
    }
  char *pt=getPointer();
  const char *srcPt=a->getConstPointer();
  if(assignTech)
    {    
      for(const int *w=bgTuples;w!=endTuples;w++)
        {
          DataArray::CheckValueInRange(nbOfTuples,*w,"invalid tuple id");
          for(const int *z=bgComp;z!=endComp;z++,srcPt++)
            {    
              pt[(*w)*nbComp+(*z)]=*srcPt;
            }
        }
    }
  else
    {
      for(const int *w=bgTuples;w!=endTuples;w++)
        {
          const char *srcPt2=srcPt;
          DataArray::CheckValueInRange(nbOfTuples,*w,"invalid tuple id");
          for(const int *z=bgComp;z!=endComp;z++,srcPt2++)
            {    
              pt[(*w)*nbComp+(*z)]=*srcPt2;
            }
        }
    }
}

/*!
 * Assign a given value to values at specified tuples and components of \a this array.
 * The tuples and components to assign to are defined by C arrays of indices.
 *  \param [in] a - the value to assign.
 *  \param [in] bgTuples - pointer to an array of tuple indices of \a this array to
 *              assign \a a to.
 *  \param [in] endTuples - specifies the end of the array \a bgTuples, so that
 *              pointer to a tuple index (\a pi) varies as this: 
 *              \a bgTuples <= \a pi < \a endTuples.
 *  \param [in] bgComp - pointer to an array of component indices of \a this array to
 *              assign \a a to.
 *  \param [in] endComp - specifies the end of the array \a bgTuples, so that
 *              pointer to a component index (\a pi) varies as this: 
 *              \a bgComp <= \a pi < \a endComp.
 *  \throw If \a this is not allocated.
 *  \throw If any index of tuple/component given by <em>bgTuples / bgComp</em> is
 *         out of a valid range for \a this array.
 *
 *  \ref cpp_mcdataarrayint_setpartofvaluessimple2 "Here is a Python example".
 */
void DataArrayChar::setPartOfValuesSimple2(char a, const int *bgTuples, const int *endTuples, const int *bgComp, const int *endComp) throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  int nbComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  for(const int *z=bgComp;z!=endComp;z++)
    DataArray::CheckValueInRange(nbComp,*z,"invalid component id");
  char *pt=getPointer();
  for(const int *w=bgTuples;w!=endTuples;w++)
    for(const int *z=bgComp;z!=endComp;z++)
      {
        DataArray::CheckValueInRange(nbOfTuples,*w,"invalid tuple id");
        pt[(*w)*nbComp+(*z)]=a;
      }
}

/*!
 * Copy all values from another DataArrayChar (\a a) into specified tuples and 
 * components of \a this array. Textual data is not copied.
 * The tuples to assign to are defined by a C array of indices.
 * The components to assign to are defined by three values similar to parameters of
 * the Python function \c range(\c start,\c stop,\c step).
 * There are two *modes of usage*:
 * - If \a a->getNbOfElems() equals to number of values to assign to, then every value
 *   of \a a is assigned to its own location within \a this array. 
 * - If \a a includes one tuple, then all values of \a a are assigned to the specified
 *   components of every specified tuple of \a this array. In this mode it is required
 *   that \a a->getNumberOfComponents() equals to the number of specified components.
 *
 *  \param [in] a - the array to copy values from.
 *  \param [in] bgTuples - pointer to an array of tuple indices of \a this array to
 *              assign values of \a a to.
 *  \param [in] endTuples - specifies the end of the array \a bgTuples, so that
 *              pointer to a tuple index <em>(pi)</em> varies as this: 
 *              \a bgTuples <= \a pi < \a endTuples.
 *  \param [in] bgComp - index of the first component of \a this array to assign to.
 *  \param [in] endComp - index of the component before which the components to assign
 *              to are located.
 *  \param [in] stepComp - index increment to get index of the next component to assign to.
 *  \param [in] strictCompoCompare - this parameter is checked only in the first
 *               *mode of usage*; if \a strictCompoCompare is \a true (default), 
 *               then \a a->getNumberOfComponents() must be equal 
 *               to the number of specified columns, else this is not required.
 *  \throw If \a a is NULL.
 *  \throw If \a a is not allocated.
 *  \throw If \a this is not allocated.
 *  \throw If any index of tuple given by \a bgTuples is out of a valid range for 
 *         \a this array.
 *  \throw In the first *mode of usage*, if <em>strictCompoCompare == true </em> and
 *         if <em> a->getNumberOfComponents()</em> is unequal to the number of components
 *         defined by <em>(bgComp,endComp,stepComp)</em>.
 *  \throw In the second *mode of usage*, if \a a->getNumberOfTuples() != 1 or
 *         <em> a->getNumberOfComponents()</em> is unequal to the number of components
 *         defined by <em>(bgComp,endComp,stepComp)</em>.
 *  \throw If parameters specifying components to assign to, do not give a
 *            non-empty range of increasing indices or indices are out of a valid range
 *            for \this array.
 *
 *  \ref cpp_mcdataarrayint_setpartofvalues3 "Here is a Python example".
 */
void DataArrayChar::setPartOfValues3(const DataArrayChar *a, const int *bgTuples, const int *endTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare) throw(INTERP_KERNEL::Exception)
{
  if(!a)
    throw INTERP_KERNEL::Exception("DataArrayChar::setPartOfValues3 : DataArrayChar pointer in input is NULL !");
  const char msg[]="DataArrayChar::setPartOfValues3";
  checkAllocated();
  a->checkAllocated();
  int newNbOfComp=DataArray::GetNumberOfItemGivenBES(bgComp,endComp,stepComp,msg);
  int nbComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  DataArray::CheckValueInRangeEx(nbComp,bgComp,endComp,"invalid component value");
  int newNbOfTuples=(int)std::distance(bgTuples,endTuples);
  bool assignTech=true;
  if(a->getNbOfElems()==newNbOfTuples*newNbOfComp)
    {
      if(strictCompoCompare)
        a->checkNbOfTuplesAndComp(newNbOfTuples,newNbOfComp,msg);
    }
  else
    {
      a->checkNbOfTuplesAndComp(1,newNbOfComp,msg);
      assignTech=false;
    }
  char *pt=getPointer()+bgComp;
  const char *srcPt=a->getConstPointer();
  if(assignTech)
    {
      for(const int *w=bgTuples;w!=endTuples;w++)
        for(int j=0;j<newNbOfComp;j++,srcPt++)
          {
            DataArray::CheckValueInRange(nbOfTuples,*w,"invalid tuple id");
            pt[(*w)*nbComp+j*stepComp]=*srcPt;
          }
    }
  else
    {
      for(const int *w=bgTuples;w!=endTuples;w++)
        {
          const char *srcPt2=srcPt;
          for(int j=0;j<newNbOfComp;j++,srcPt2++)
            {
              DataArray::CheckValueInRange(nbOfTuples,*w,"invalid tuple id");
              pt[(*w)*nbComp+j*stepComp]=*srcPt2;
            }
        }
    }
}

/*!
 * Assign a given value to values at specified tuples and components of \a this array.
 * The tuples to assign to are defined by a C array of indices.
 * The components to assign to are defined by three values similar to parameters of
 * the Python function \c range(\c start,\c stop,\c step).
 *  \param [in] a - the value to assign.
 *  \param [in] bgTuples - pointer to an array of tuple indices of \a this array to
 *              assign \a a to.
 *  \param [in] endTuples - specifies the end of the array \a bgTuples, so that
 *              pointer to a tuple index <em>(pi)</em> varies as this: 
 *              \a bgTuples <= \a pi < \a endTuples.
 *  \param [in] bgComp - index of the first component of \a this array to assign to.
 *  \param [in] endComp - index of the component before which the components to assign
 *              to are located.
 *  \param [in] stepComp - index increment to get index of the next component to assign to.
 *  \throw If \a this is not allocated.
 *  \throw If any index of tuple given by \a bgTuples is out of a valid range for 
 *         \a this array.
 *  \throw If parameters specifying components to assign to, do not give a
 *            non-empty range of increasing indices or indices are out of a valid range
 *            for \this array.
 *
 *  \ref cpp_mcdataarrayint_setpartofvaluessimple3 "Here is a Python example".
 */
void DataArrayChar::setPartOfValuesSimple3(char a, const int *bgTuples, const int *endTuples, int bgComp, int endComp, int stepComp) throw(INTERP_KERNEL::Exception)
{
  const char msg[]="DataArrayChar::setPartOfValuesSimple3";
  checkAllocated();
  int newNbOfComp=DataArray::GetNumberOfItemGivenBES(bgComp,endComp,stepComp,msg);
  int nbComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  DataArray::CheckValueInRangeEx(nbComp,bgComp,endComp,"invalid component value");
  char *pt=getPointer()+bgComp;
  for(const int *w=bgTuples;w!=endTuples;w++)
    for(int j=0;j<newNbOfComp;j++)
      {
        DataArray::CheckValueInRange(nbOfTuples,*w,"invalid tuple id");
        pt[(*w)*nbComp+j*stepComp]=a;
      }
}

void DataArrayChar::setPartOfValues4(const DataArrayChar *a, int bgTuples, int endTuples, int stepTuples, const int *bgComp, const int *endComp, bool strictCompoCompare) throw(INTERP_KERNEL::Exception)
{
  if(!a)
    throw INTERP_KERNEL::Exception("DataArrayInt::setPartOfValues4 : input DataArrayInt is NULL !");
  const char msg[]="DataArrayInt::setPartOfValues4";
  checkAllocated();
  a->checkAllocated();
  int newNbOfTuples=DataArray::GetNumberOfItemGivenBES(bgTuples,endTuples,stepTuples,msg);
  int newNbOfComp=(int)std::distance(bgComp,endComp);
  int nbComp=getNumberOfComponents();
  for(const int *z=bgComp;z!=endComp;z++)
    DataArray::CheckValueInRange(nbComp,*z,"invalid component id");
  int nbOfTuples=getNumberOfTuples();
  DataArray::CheckValueInRangeEx(nbOfTuples,bgTuples,endTuples,"invalid tuple value");
  bool assignTech=true;
  if(a->getNbOfElems()==newNbOfTuples*newNbOfComp)
    {
      if(strictCompoCompare)
        a->checkNbOfTuplesAndComp(newNbOfTuples,newNbOfComp,msg);
    }
  else
    {
      a->checkNbOfTuplesAndComp(1,newNbOfComp,msg);
      assignTech=false;
    }
  const char *srcPt=a->getConstPointer();
  char *pt=getPointer()+bgTuples*nbComp;
  if(assignTech)
    {
      for(int i=0;i<newNbOfTuples;i++,pt+=stepTuples*nbComp)
        for(const int *z=bgComp;z!=endComp;z++,srcPt++)
          pt[*z]=*srcPt;
    }
  else
    {
      for(int i=0;i<newNbOfTuples;i++,pt+=stepTuples*nbComp)
        {
          const char *srcPt2=srcPt;
          for(const int *z=bgComp;z!=endComp;z++,srcPt2++)
            pt[*z]=*srcPt2;
        }
    }
}

void DataArrayChar::setPartOfValuesSimple4(char a, int bgTuples, int endTuples, int stepTuples, const int *bgComp, const int *endComp) throw(INTERP_KERNEL::Exception)
{
  const char msg[]="DataArrayInt::setPartOfValuesSimple4";
  checkAllocated();
  int newNbOfTuples=DataArray::GetNumberOfItemGivenBES(bgTuples,endTuples,stepTuples,msg);
  int nbComp=getNumberOfComponents();
  for(const int *z=bgComp;z!=endComp;z++)
    DataArray::CheckValueInRange(nbComp,*z,"invalid component id");
  int nbOfTuples=getNumberOfTuples();
  DataArray::CheckValueInRangeEx(nbOfTuples,bgTuples,endTuples,"invalid tuple value");
  char *pt=getPointer()+bgTuples*nbComp;
  for(int i=0;i<newNbOfTuples;i++,pt+=stepTuples*nbComp)
    for(const int *z=bgComp;z!=endComp;z++)
      pt[*z]=a;
}

/*!
 * Copy some tuples from another DataArrayChar into specified tuples
 * of \a this array. Textual data is not copied. Both arrays must have equal number of
 * components.
 * Both the tuples to assign and the tuples to assign to are defined by a DataArrayChar.
 * All components of selected tuples are copied.
 *  \param [in] a - the array to copy values from.
 *  \param [in] tuplesSelec - the array specifying both source tuples of \a a and
 *              target tuples of \a this. \a tuplesSelec has two components, and the
 *              first component specifies index of the source tuple and the second
 *              one specifies index of the target tuple.
 *  \throw If \a this is not allocated.
 *  \throw If \a a is NULL.
 *  \throw If \a a is not allocated.
 *  \throw If \a tuplesSelec is NULL.
 *  \throw If \a tuplesSelec is not allocated.
 *  \throw If <em>this->getNumberOfComponents() != a->getNumberOfComponents()</em>.
 *  \throw If \a tuplesSelec->getNumberOfComponents() != 2.
 *  \throw If any tuple index given by \a tuplesSelec is out of a valid range for 
 *         the corresponding (\a this or \a a) array.
 */
void DataArrayChar::setPartOfValuesAdv(const DataArrayChar *a, const DataArrayChar *tuplesSelec) throw(INTERP_KERNEL::Exception)
{
  if(!a || !tuplesSelec)
    throw INTERP_KERNEL::Exception("DataArrayChar::setPartOfValuesAdv : DataArrayChar pointer in input is NULL !");
  checkAllocated();
  a->checkAllocated();
  tuplesSelec->checkAllocated();
  int nbOfComp=getNumberOfComponents();
  if(nbOfComp!=a->getNumberOfComponents())
    throw INTERP_KERNEL::Exception("DataArrayChar::setPartOfValuesAdv : This and a do not have the same number of components !");
  if(tuplesSelec->getNumberOfComponents()!=2)
    throw INTERP_KERNEL::Exception("DataArrayChar::setPartOfValuesAdv : Expecting to have a tuple selector DataArrayChar instance with exactly 2 components !");
  int thisNt=getNumberOfTuples();
  int aNt=a->getNumberOfTuples();
  char *valsToSet=getPointer();
  const char *valsSrc=a->getConstPointer();
  for(const char *tuple=tuplesSelec->begin();tuple!=tuplesSelec->end();tuple+=2)
    {
      if(tuple[1]>=0 && tuple[1]<aNt)
        {
          if(tuple[0]>=0 && tuple[0]<thisNt)
            std::copy(valsSrc+nbOfComp*tuple[1],valsSrc+nbOfComp*(tuple[1]+1),valsToSet+nbOfComp*tuple[0]);
          else
            {
              std::ostringstream oss; oss << "DataArrayChar::setPartOfValuesAdv : Tuple #" << std::distance(tuplesSelec->begin(),tuple)/2;
              oss << " of 'tuplesSelec' request of tuple id #" << tuple[0] << " in 'this' ! It should be in [0," << thisNt << ") !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayChar::setPartOfValuesAdv : Tuple #" << std::distance(tuplesSelec->begin(),tuple)/2;
          oss << " of 'tuplesSelec' request of tuple id #" << tuple[1] << " in 'a' ! It should be in [0," << aNt << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
}

/*!
 * Returns a value located at specified tuple and component.
 * This method is equivalent to DataArrayChar::getIJ() except that validity of
 * parameters is checked. So this method is safe but expensive if used to go through
 * all values of \a this.
 *  \param [in] tupleId - index of tuple of interest.
 *  \param [in] compoId - index of component of interest.
 *  \return double - value located by \a tupleId and \a compoId.
 *  \throw If \a this is not allocated.
 *  \throw If condition <em>( 0 <= tupleId < this->getNumberOfTuples() )</em> is violated.
 *  \throw If condition <em>( 0 <= compoId < this->getNumberOfComponents() )</em> is violated.
 */
char DataArrayChar::getIJSafe(int tupleId, int compoId) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(tupleId<0 || tupleId>=getNumberOfTuples())
    {
      std::ostringstream oss; oss << "DataArrayChar::getIJSafe : request for tupleId " << tupleId << " should be in [0," << getNumberOfTuples() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(compoId<0 || compoId>=getNumberOfComponents())
    {
      std::ostringstream oss; oss << "DataArrayChar::getIJSafe : request for compoId " << compoId << " should be in [0," << getNumberOfComponents() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  return _mem[tupleId*((int)_info_on_compo.size())+compoId];
}

/*!
 * Returns the last value of \a this. 
 *  \return double - the last value of \a this array.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a this->getNumberOfTuples() < 1.
 */
char DataArrayChar::back() const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::back : number of components not equal to one !");
  int nbOfTuples=getNumberOfTuples();
  if(nbOfTuples<1)
    throw INTERP_KERNEL::Exception("DataArrayChar::back : number of tuples must be >= 1 !");
  return *(getConstPointer()+nbOfTuples-1);
}

DataArrayCharIterator *DataArrayChar::iterator()
{
  return new DataArrayCharIterator(this);
}

/*!
 * Creates a new DataArrayChar containing IDs (indices) of tuples holding value equal to a
 * given one.
 *  \param [in] val - the value to find within \a this.
 *  \return DataArrayChar * - a new instance of DataArrayChar. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayChar::getIdsEqual(char val) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::getIdsEqual : the array must have only one component, you can call 'rearrange' method before !");
  const char *cptr=getConstPointer();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(*cptr==val)
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * Creates a new DataArrayChar containing IDs (indices) of tuples holding value \b not
 * equal to a given one. 
 *  \param [in] val - the value to ignore within \a this.
 *  \return DataArrayChar * - a new instance of DataArrayChar. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayChar::getIdsNotEqual(char val) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::getIdsNotEqual : the array must have only one component, you can call 'rearrange' method before !");
  const char *cptr=getConstPointer();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(*cptr!=val)
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * This method searches the sequence specified in input parameter \b vals in \b this.
 * This works only for DataArrayChar having number of components equal to one (if not an INTERP_KERNEL::Exception will be thrown).
 * This method differs from DataArrayChar::locateTuple in that the position is internal raw data is not considered here contrary to DataArrayChar::locateTuple.
 * \sa DataArrayChar::locateTuple
 */
int DataArrayChar::search(const std::vector<char>& vals) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  int nbOfCompo=getNumberOfComponents();
  if(nbOfCompo!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::search : works only for DataArrayChar instance with one component !");
  const char *cptr=getConstPointer();
  int nbOfVals=getNbOfElems();
  const char *loc=std::search(cptr,cptr+nbOfVals,vals.begin(),vals.end());
  if(loc!=cptr+nbOfVals)
    return std::distance(cptr,loc);
  return -1;
}

/*!
 * This method is an extension of DataArrayChar::locateValue method because this method works for DataArrayChar with
 * any number of components excepted 0 (an INTERP_KERNEL::Exception is thrown in this case).
 * This method searches in \b this is there is a tuple that matched the input parameter \b tupl.
 * If any the tuple id is returned. If not -1 is returned.
 * 
 * This method throws an INTERP_KERNEL::Exception if the number of components in \b this mismatches with the size of
 * the input vector. An INTERP_KERNEL::Exception is thrown too if \b this is not allocated.
 *
 * \return tuple id where \b tupl is. -1 if no such tuple exists in \b this.
 * \sa DataArrayChar::search.
 */
int DataArrayChar::locateTuple(const std::vector<char>& tupl) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  int nbOfCompo=getNumberOfComponents();
  if(nbOfCompo==0)
    throw INTERP_KERNEL::Exception("DataArrayChar::locateTuple : 0 components in 'this' !");
  if(nbOfCompo!=(int)tupl.size())
    {
      std::ostringstream oss; oss << "DataArrayChar::locateTuple : 'this' contains " << nbOfCompo << " components and searching for a tuple of length " << tupl.size() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  const char *cptr=getConstPointer();
  int nbOfVals=getNbOfElems();
  for(const char *work=cptr;work!=cptr+nbOfVals;)
    {
      work=std::search(work,cptr+nbOfVals,tupl.begin(),tupl.end());
      if(work!=cptr+nbOfVals)
        {
          if(std::distance(cptr,work)%nbOfCompo!=0)
            work++;
          else
            return std::distance(cptr,work)/nbOfCompo;
        }
    }
  return -1;
}

/*!
 * Returns \a true if a given value is present within \a this one-dimensional array.
 *  \param [in] value - the value to find within \a this array.
 *  \return bool - \a true in case if \a value is present within \a this array.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \sa locateValue()
 */
bool DataArrayChar::presenceOfValue(char value) const throw(INTERP_KERNEL::Exception)
{
  return locateValue(value)!=-1;
}

/*!
 * This method expects to be called when number of components of this is equal to one.
 * This method returns true if it exists a tuple so that the value is contained in \b vals.
 * If not any tuple contains one of the values contained in 'vals' false is returned.
 * \sa DataArrayChar::locateValue
 */
bool DataArrayChar::presenceOfValue(const std::vector<char>& vals) const throw(INTERP_KERNEL::Exception)
{
  return locateValue(vals)!=-1;
}

/*!
 * This method expects to be called when number of components of this is equal to one.
 * This method returns the tuple id, if it exists, of the first tuple equal to \b value.
 * If not any tuple contains \b value -1 is returned.
 * \sa DataArrayChar::presenceOfValue
 */
int DataArrayChar::locateValue(char value) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::presenceOfValue : the array must have only one component, you can call 'rearrange' method before !");
  const char *cptr=getConstPointer();
  int nbOfTuples=getNumberOfTuples();
  const char *ret=std::find(cptr,cptr+nbOfTuples,value);
  if(ret!=cptr+nbOfTuples)
    return std::distance(cptr,ret);
  return -1;
}

/*!
 * This method expects to be called when number of components of this is equal to one.
 * This method returns the tuple id, if it exists, of the first tuple so that the value is contained in \b vals.
 * If not any tuple contains one of the values contained in 'vals' false is returned.
 * \sa DataArrayChar::presenceOfValue
 */
int DataArrayChar::locateValue(const std::vector<char>& vals) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::presenceOfValue : the array must have only one component, you can call 'rearrange' method before !");
  std::set<char> vals2(vals.begin(),vals.end());
  const char *cptr=getConstPointer();
  int nbOfTuples=getNumberOfTuples();
  for(const char *w=cptr;w!=cptr+nbOfTuples;w++)
    if(vals2.find(*w)!=vals2.end())
      return std::distance(cptr,w);
  return -1;
}

/*!
 * Returns the maximal value and its location within \a this one-dimensional array.
 *  \param [out] tupleId - index of the tuple holding the maximal value.
 *  \return double - the maximal value among all values of \a this array.
 *  \throw If \a this->getNumberOfComponents() != 1
 *  \throw If \a this->getNumberOfTuples() < 1
 */
char DataArrayChar::getMaxValue(int& tupleId) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::getMaxValue : must be applied on DataArrayChar with only one component !");
  int nbOfTuples=getNumberOfTuples();
  if(nbOfTuples<=0)
    throw INTERP_KERNEL::Exception("DataArrayChar::getMaxValue : array exists but number of tuples must be > 0 !");
  const char *vals=getConstPointer();
  const char *loc=std::max_element(vals,vals+nbOfTuples);
  tupleId=(int)std::distance(vals,loc);
  return *loc;
}

/*!
 * Returns the maximal value within \a this array that is allowed to have more than
 *  one component.
 *  \return char - the maximal value among all values of \a this array.
 *  \throw If \a this is not allocated.
 */
char DataArrayChar::getMaxValueInArray() const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  const char *loc=std::max_element(begin(),end());
  return *loc;
}

/*!
 * Returns the minimal value and its location within \a this one-dimensional array.
 *  \param [out] tupleId - index of the tuple holding the minimal value.
 *  \return char - the minimal value among all values of \a this array.
 *  \throw If \a this->getNumberOfComponents() != 1
 *  \throw If \a this->getNumberOfTuples() < 1
 */
char DataArrayChar::getMinValue(int& tupleId) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::getMaxValue : must be applied on DataArrayChar with only one component !");
  int nbOfTuples=getNumberOfTuples();
  if(nbOfTuples<=0)
    throw INTERP_KERNEL::Exception("DataArrayChar::getMaxValue : array exists but number of tuples must be > 0 !");
  const char *vals=getConstPointer();
  const char *loc=std::min_element(vals,vals+nbOfTuples);
  tupleId=(int)std::distance(vals,loc);
  return *loc;
}

/*!
 * Returns the minimal value within \a this array that is allowed to have more than
 *  one component.
 *  \return char - the minimal value among all values of \a this array.
 *  \throw If \a this is not allocated.
 */
char DataArrayChar::getMinValueInArray() const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  const char *loc=std::min_element(begin(),end());
  return *loc;
}

/*!
 * This method works only on data array with one component.
 * This method returns a newly allocated array storing stored ascendantly tuple ids in \b this so that
 * this[*id] in [\b vmin,\b vmax)
 * 
 * \param [in] vmin begin of range. This value is included in range.
 * \param [out] vmax end of range. This value is \b not included in range.
 * \return a newly allocated data array that the caller should deal with.
 */
DataArrayInt *DataArrayChar::getIdsInRange(char vmin, char vmax) const throw(INTERP_KERNEL::Exception)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayChar::getIdsInRange : this must have exactly one component !");
  const char *cptr=getConstPointer();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(0,1);
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(*cptr>=vmin && *cptr<vmax)
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * Returns a new DataArrayChar by concatenating two given arrays, so that (1) the number
 * of tuples in the result array is <em> a1->getNumberOfTuples() + a2->getNumberOfTuples() -
 * offsetA2</em> and (2)
 * the number of component in the result array is same as that of each of given arrays.
 * First \a offsetA2 tuples of \a a2 are skipped and thus are missing from the result array.
 * Info on components is copied from the first of the given arrays. Number of components
 * in the given arrays must be the same.
 *  \param [in] a1 - an array to include in the result array.
 *  \param [in] a2 - another array to include in the result array.
 *  \param [in] offsetA2 - number of tuples of \a a2 to skip.
 *  \return DataArrayChar * - the new instance of DataArrayChar.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfComponents() != \a a2->getNumberOfComponents().
 */
DataArrayChar *DataArrayChar::Aggregate(const DataArrayChar *a1, const DataArrayChar *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayChar::Aggregate : input DataArrayChar instance is NULL !");
  std::vector<const DataArrayChar *> v(2); v[0]=a1; v[1]=a2;
  return Aggregate(v);
}

/*!
 * Returns a new DataArrayChar by concatenating all given arrays, so that (1) the number
 * of tuples in the result array is a sum of the number of tuples of given arrays and (2)
 * the number of component in the result array is same as that of each of given arrays.
 * Info on components is copied from the first of the given arrays. Number of components
 * in the given arrays must be  the same.
 *  \param [in] arr - a sequence of arrays to include in the result array.
 *  \return DataArrayChar * - the new instance of DataArrayChar.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If all arrays within \a arr are NULL.
 *  \throw If getNumberOfComponents() of arrays within \a arr.
 */
DataArrayChar *DataArrayChar::Aggregate(const std::vector<const DataArrayChar *>& arr) throw(INTERP_KERNEL::Exception)
{
  std::vector<const DataArrayChar *> a;
  for(std::vector<const DataArrayChar *>::const_iterator it4=arr.begin();it4!=arr.end();it4++)
    if(*it4)
      a.push_back(*it4);
  if(a.empty())
    throw INTERP_KERNEL::Exception("DataArrayChar::Aggregate : input list must be NON EMPTY !");
  std::vector<const DataArrayChar *>::const_iterator it=a.begin();
  int nbOfComp=(*it)->getNumberOfComponents();
  int nbt=(*it++)->getNumberOfTuples();
  for(int i=1;it!=a.end();it++,i++)
    {
      if((*it)->getNumberOfComponents()!=nbOfComp)
        throw INTERP_KERNEL::Exception("DataArrayChar::Aggregate : Nb of components mismatch for array aggregation !");
      nbt+=(*it)->getNumberOfTuples();
    }
  DataArrayChar *ret=DataArrayChar::New();
  ret->alloc(nbt,nbOfComp);
  char *pt=ret->getPointer();
  for(it=a.begin();it!=a.end();it++)
    pt=std::copy((*it)->getConstPointer(),(*it)->getConstPointer()+(*it)->getNbOfElems(),pt);
  ret->copyStringInfoFrom(*(a[0]));
  return ret;
}

/*!
 * Returns a new DataArrayChar by aggregating two given arrays, so that (1) the number
 * of components in the result array is a sum of the number of components of given arrays
 * and (2) the number of tuples in the result array is same as that of each of given
 * arrays. In other words the i-th tuple of result array includes all components of
 * i-th tuples of all given arrays.
 * Number of tuples in the given arrays must be the same.
 *  \param [in] a1 - an array to include in the result array.
 *  \param [in] a2 - another array to include in the result array.
 *  \return DataArrayChar * - the new instance of DataArrayChar.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If both \a a1 and \a a2 are NULL.
 *  \throw If any given array is not allocated.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples()
 */
DataArrayChar *DataArrayChar::Meld(const DataArrayChar *a1, const DataArrayChar *a2) throw(INTERP_KERNEL::Exception)
{
  std::vector<const DataArrayChar *> arr(2);
  arr[0]=a1; arr[1]=a2;
  return Meld(arr);
}

/*!
 * Returns a new DataArrayChar by aggregating all given arrays, so that (1) the number
 * of components in the result array is a sum of the number of components of given arrays
 * and (2) the number of tuples in the result array is same as that of each of given
 * arrays. In other words the i-th tuple of result array includes all components of
 * i-th tuples of all given arrays.
 * Number of tuples in the given arrays must be  the same.
 *  \param [in] arr - a sequence of arrays to include in the result array.
 *  \return DataArrayChar * - the new instance of DataArrayChar.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If all arrays within \a arr are NULL.
 *  \throw If any given array is not allocated.
 *  \throw If getNumberOfTuples() of arrays within \a arr is different.
 */
DataArrayChar *DataArrayChar::Meld(const std::vector<const DataArrayChar *>& arr) throw(INTERP_KERNEL::Exception)
{
  std::vector<const DataArrayChar *> a;
  for(std::vector<const DataArrayChar *>::const_iterator it4=arr.begin();it4!=arr.end();it4++)
    if(*it4)
      a.push_back(*it4);
  if(a.empty())
    throw INTERP_KERNEL::Exception("DataArrayChar::Meld : array must be NON empty !");
  std::vector<const DataArrayChar *>::const_iterator it;
  for(it=a.begin();it!=a.end();it++)
    (*it)->checkAllocated();
  it=a.begin();
  int nbOfTuples=(*it)->getNumberOfTuples();
  std::vector<int> nbc(a.size());
  std::vector<const char *> pts(a.size());
  nbc[0]=(*it)->getNumberOfComponents();
  pts[0]=(*it++)->getConstPointer();
  for(int i=1;it!=a.end();it++,i++)
    {
      if(nbOfTuples!=(*it)->getNumberOfTuples())
        throw INTERP_KERNEL::Exception("DataArrayChar::meld : mismatch of number of tuples !");
      nbc[i]=(*it)->getNumberOfComponents();
      pts[i]=(*it)->getConstPointer();
    }
  int totalNbOfComp=std::accumulate(nbc.begin(),nbc.end(),0);
  DataArrayChar *ret=DataArrayChar::New();
  ret->alloc(nbOfTuples,totalNbOfComp);
  char *retPtr=ret->getPointer();
  for(int i=0;i<nbOfTuples;i++)
    for(int j=0;j<(int)a.size();j++)
      {
        retPtr=std::copy(pts[j],pts[j]+nbc[j],retPtr);
        pts[j]+=nbc[j];
      }
  int k=0;
  for(int i=0;i<(int)a.size();i++)
    for(int j=0;j<nbc[i];j++,k++)
      ret->setInfoOnComponent(k,a[i]->getInfoOnComponent(j).c_str());
  return ret;
}

/*!
 * Sets a C array to be used as raw data of \a this. The previously set info
 *  of components is retained and re-sized. 
 * For more info see \ref MEDCouplingArraySteps1.
 *  \param [in] array - the C array to be used as raw data of \a this.
 *  \param [in] ownership - if \a true, \a array will be deallocated at destruction of \a this.
 *  \param [in] type - specifies how to deallocate \a array. If \a type == ParaMEDMEM::CPP_DEALLOC,
 *                     \c delete [] \c array; will be called. If \a type == ParaMEDMEM::C_DEALLOC,
 *                     \c free(\c array ) will be called.
 *  \param [in] nbOfTuple - new number of tuples in \a this.
 *  \param [in] nbOfCompo - new number of components in \a this.
 */
void DataArrayChar::useArray(const char *array, bool ownership,  DeallocType type, int nbOfTuple, int nbOfCompo)
{
  _info_on_compo.resize(nbOfCompo);
  _mem.useArray(array,ownership,type,nbOfTuple*nbOfCompo);
  declareAsNew();
}

void DataArrayChar::useExternalArrayWithRWAccess(const char *array, int nbOfTuple, int nbOfCompo)
{
  _info_on_compo.resize(nbOfCompo);
  _mem.useExternalArrayWithRWAccess(array,nbOfTuple*nbOfCompo);
  declareAsNew();
}

DataArrayCharIterator::DataArrayCharIterator(DataArrayChar *da):_da(da),_pt(0),_tuple_id(0),_nb_comp(0),_nb_tuple(0)
{
  if(_da)
    {
      _da->incrRef();
      if(_da->isAllocated())
        {
          _nb_comp=da->getNumberOfComponents();
          _nb_tuple=da->getNumberOfTuples();
          _pt=da->getPointer();
        }
    }
}

DataArrayCharIterator::~DataArrayCharIterator()
{
  if(_da)
    _da->decrRef();
}

DataArrayCharTuple *DataArrayCharIterator::nextt()
{
  if(_tuple_id<_nb_tuple)
    {
      _tuple_id++;
      DataArrayCharTuple *ret=new DataArrayCharTuple(_pt,_nb_comp);
      _pt+=_nb_comp;
      return ret;
    }
  else
    return 0;
}

DataArrayCharTuple::DataArrayCharTuple(char *pt, int nbOfComp):_pt(pt),_nb_of_compo(nbOfComp)
{
}

std::string DataArrayCharTuple::repr() const
{
  std::ostringstream oss; oss << "(";
  for(int i=0;i<_nb_of_compo-1;i++)
    oss << _pt[i] << ", ";
  oss << _pt[_nb_of_compo-1] << ")";
  return oss.str();
}

char DataArrayCharTuple::charValue() const throw(INTERP_KERNEL::Exception)
{
  if(_nb_of_compo==1)
    return *_pt;
  throw INTERP_KERNEL::Exception("DataArrayCharTuple::charValue : DataArrayCharTuple instance has not exactly 1 component -> Not possible to convert it into an character !");
}

/*!
 * This method returns a newly allocated instance the caller should dealed with by a ParaMEDMEM::DataArrayChar::decrRef.
 * This method performs \b no copy of data. The content is only referenced using ParaMEDMEM::DataArrayChar::useArray with ownership set to \b false.
 * This method throws an INTERP_KERNEL::Exception is it is impossible to match sizes of \b this that is too say \b nbOfCompo=this->_nb_of_elem and \bnbOfTuples==1 or
 * \b nbOfCompo=1 and \bnbOfTuples==this->_nb_of_elem.
 */
DataArrayChar *DataArrayCharTuple::buildDAChar(int nbOfTuples, int nbOfCompo) const throw(INTERP_KERNEL::Exception)
{
  if((_nb_of_compo==nbOfCompo && nbOfTuples==1) || (_nb_of_compo==nbOfTuples && nbOfCompo==1))
    {
      DataArrayChar *ret=DataArrayChar::New();
      ret->useExternalArrayWithRWAccess(_pt,nbOfTuples,nbOfCompo);
      return ret;
    }
  else
    {
      std::ostringstream oss; oss << "DataArrayCharTuple::buildDAChar : unable to build a requested DataArrayChar instance with nbofTuple=" << nbOfTuples << " and nbOfCompo=" << nbOfCompo;
      oss << ".\nBecause the number of elements in this is " << _nb_of_compo << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}
