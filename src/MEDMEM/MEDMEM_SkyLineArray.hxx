# ifndef __MEDSKYLINEARRAY_H__
# define __MEDSKYLINEARRAY_H__

#include "MEDMEM_Exception.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {
class MEDSKYLINEARRAY
{
private :
  int   _count ;
  int   _length ;
  PointerOf <int> _index ; // array of size _count+1 : _index[0]=1 and
		               // _index[_count]=length+1
  PointerOf <int> _value ; // array of size _length

public :
  MEDSKYLINEARRAY();
  ~MEDSKYLINEARRAY();
  MEDSKYLINEARRAY( const MEDSKYLINEARRAY &myArray );
  MEDSKYLINEARRAY( const int count, const int length );
  MEDSKYLINEARRAY( const int count, const int length,
		   const int* index, const int* value, bool shallowCopy=false );

  //void setMEDSKYLINEARRAY( const int count, const int length, int* index , int* value ) ;

  inline int  getNumberOf()       const;
  inline int  getLength()         const;
  inline const int*  getIndex()   const;
  inline const int*  getValue()   const;
  inline int  getNumberOfI(int i) const throw (MEDEXCEPTION) ;
  inline const int*  getI(int i)  const throw (MEDEXCEPTION) ;
  inline int  getIJ(int i, int j) const throw (MEDEXCEPTION) ;
  inline int  getIndexValue(int i) const throw (MEDEXCEPTION) ;

  inline void setIndex(const int* index) ;
  inline void setI(const int i, const int* values) throw (MEDEXCEPTION) ;
  inline void setIJ(int i, int j, int value) throw (MEDEXCEPTION) ;
  inline void setIndexValue(int i, int value) throw (MEDEXCEPTION) ;

};

// ---------------------------------------
//              Methodes Inline
// ---------------------------------------
inline int MEDSKYLINEARRAY::getNumberOf() const
{
  return _count ;
};
inline int MEDSKYLINEARRAY::getLength() const
{
  return _length ;
};
inline const int*  MEDSKYLINEARRAY::getIndex() const
{
  return (const int*)_index ;
} ;
inline const int*  MEDSKYLINEARRAY::getValue() const
{
  return (const int*)_value ;
} ;
inline int MEDSKYLINEARRAY::getNumberOfI(int i) const throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument must be >= 1");
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument is out of range");
  return _index[i]-_index[i-1] ;
} ;
inline const int* MEDSKYLINEARRAY::getI(int i) const throw (MEDEXCEPTION)
{
    if (i<1)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getI : argument must be >= 1");
    if (i>_count)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getI : argument is out of range");
    return _value+_index[i-1]-1 ;
}
inline int MEDSKYLINEARRAY::getIJ(int i, int j) const throw (MEDEXCEPTION)
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

inline int  MEDSKYLINEARRAY::getIndexValue(int i) const throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getIndexValue : argument must be >= 1");
  if (i>_index[_count])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getIndexValue : argument is out of range") ;
  return _value[i-1] ;
}

inline void MEDSKYLINEARRAY::setIndex(const int* index)
{
  memcpy((int*)_index,index,(_count+1)*sizeof(int));
}


inline void MEDSKYLINEARRAY::setIJ(int i, int j, int value) throw (MEDEXCEPTION)
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

inline void MEDSKYLINEARRAY::setI(const int i, const int * values) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setI : index must be >= 1");
;
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setI : index is out of range") ;

  memcpy(_value+_index[i-1]-1,values,(_index[i]-_index[i-1])*sizeof(int)) ;
}

inline void MEDSKYLINEARRAY::setIndexValue(int i, int value) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIndexValue : argument must be >= 1");
  if (i>_index[_count])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIndexValue : argument is out of range") ;
  _value[i-1]=value ;
}
}
# endif
