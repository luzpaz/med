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
  MEDSKYLINEARRAY( const med_int count , const med_int length );
  
  void setMEDSKYLINEARRAY( const med_int count, const med_int length, med_int* index , med_int* value ) ;

  inline med_int  getNumberOf()       const;
  inline med_int  getLength()         const;
  inline med_int* getIndex()  	      ; 
  inline med_int* getValue()	      ; 
  inline med_int  getNumberOfI(int i) const throw (MEDEXCEPTION) ;
  inline med_int* getI(int i) 	       throw (MEDEXCEPTION) ;
  inline med_int  getIJ(int i, int j) const throw (MEDEXCEPTION) ;
  
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
inline med_int* MEDSKYLINEARRAY::getIndex()  
{ 
	return (med_int*)_index ; 
} ;
inline med_int* MEDSKYLINEARRAY::getValue()  
{ 
	return (med_int*)_value ; 
} ;
inline med_int MEDSKYLINEARRAY::getNumberOfI(int i) const throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument must be >= 1");
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument is out of range");
  return _index[i]-_index[i-1] ;
} ;
inline med_int* MEDSKYLINEARRAY::getI(int i)  throw (MEDEXCEPTION)
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


# endif
