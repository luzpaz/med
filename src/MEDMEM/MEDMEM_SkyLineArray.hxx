//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MEDMEM_SkyLineArray.hxx
//  Module : MED

# ifndef __MEDSKYLINEARRAY_H__
# define __MEDSKYLINEARRAY_H__

#include "MEDMEM_Exception.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

using  MED_EN::med_int ;

class MEDSKYLINEARRAY
{
private :
  med_int   _count ;
  med_int   _length ;
  PointerOf <med_int> _index ; // array of size _count+1 : _index[0]=1 and
		               // _index[_count]=length+1
  PointerOf <med_int> _value ; // array of size _length

public :
  MEDSKYLINEARRAY();
  ~MEDSKYLINEARRAY();
  MEDSKYLINEARRAY( const MEDSKYLINEARRAY &myArray );
  MEDSKYLINEARRAY( const med_int count, const med_int length );
  MEDSKYLINEARRAY( const med_int count, const med_int length,
		   const med_int* index, const med_int* value );

  //void setMEDSKYLINEARRAY( const med_int count, const med_int length, med_int* index , med_int* value ) ;

  inline med_int  getNumberOf()       const;
  inline med_int  getLength()         const;
  inline const med_int*  getIndex()   const;
  inline const med_int*  getValue()   const;
  inline med_int  getNumberOfI(int i) const throw (MEDEXCEPTION) ;
  inline const med_int*  getI(int i)  const throw (MEDEXCEPTION) ;
  inline med_int  getIJ(int i, int j) const throw (MEDEXCEPTION) ;
  inline med_int  getIndexValue(int i) const throw (MEDEXCEPTION) ;

  inline void setIndex(const med_int* index) ;
  inline void setI(const med_int i, const med_int* values) throw (MEDEXCEPTION) ;
  inline void setIJ(med_int i, med_int j, med_int value) throw (MEDEXCEPTION) ;
  inline void setIndexValue(med_int i, med_int value) throw (MEDEXCEPTION) ;

} ;

// ---------------------------------------
//              Methodes Inline
// ---------------------------------------
inline med_int MEDSKYLINEARRAY::getNumberOf() const
{
  return _count ;
};
inline med_int MEDSKYLINEARRAY::getLength() const
{
  return _length ;
};
inline const med_int*  MEDSKYLINEARRAY::getIndex() const
{
  return (const med_int*)_index ;
} ;
inline const med_int*  MEDSKYLINEARRAY::getValue() const
{
  return (const med_int*)_value ;
} ;
inline med_int MEDSKYLINEARRAY::getNumberOfI(int i) const throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument must be >= 1");
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument is out of range");
  return _index[i]-_index[i-1] ;
} ;
inline const med_int* MEDSKYLINEARRAY::getI(int i) const throw (MEDEXCEPTION)
{
    if (i<1)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getI : argument must be >= 1");
    if (i>_count)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getI : argument is out of range");
    return _value+_index[i-1]-1 ;
}
inline med_int MEDSKYLINEARRAY::getIJ(int i, int j) const throw (MEDEXCEPTION)
{
    if (i<1)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : first argument must be >= 1");
    if (j<1)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : second argument must be >= 1");
    if (i>_count)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : first argument is out of range") ;
    if (j>_index[i])
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : second argument is out of range") ;
    return _value[_index[i-1]+j-2] ;
}

inline med_int  MEDSKYLINEARRAY::getIndexValue(int i) const throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getIndexValue : argument must be >= 1");
  if (i>_index[_count])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getIndexValue : argument is out of range") ;
  return _value[i-1] ;
}

inline void MEDSKYLINEARRAY::setIndex(const med_int* index)
{
  memcpy((med_int*)_index,index,(_count+1)*sizeof(med_int));
}


inline void MEDSKYLINEARRAY::setIJ(med_int i, med_int j, med_int value) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : first argument must be >= 1");
  if (j<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : second argument must be >= 1");
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : first argument is out of range") ;
  if (j>_index[i])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : second argument is out of range") ;
  
  _value[_index[i-1]+j-2]=value ;

}

inline void MEDSKYLINEARRAY::setI(const med_int i, const med_int * values) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setI : index must be >= 1");
;
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setI : index is out of range") ;

  memcpy(_value+_index[i-1]-1,values,(_index[i]-_index[i-1])*sizeof(med_int)) ;
}

inline void MEDSKYLINEARRAY::setIndexValue(med_int i, med_int value) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIndexValue : argument must be >= 1");
  if (i>_index[_count])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIndexValue : argument is out of range") ;
  _value[i-1]=value ;
}

# endif
