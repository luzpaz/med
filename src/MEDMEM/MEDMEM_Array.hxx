#ifndef __MEDARRAY_H__
#define __MEDARRAY_H__

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_PointerOf.hxx"
#include "utilities.h"

using namespace MED_EN;

/*!
  A template class to generate an array of any particular type (int, long,
  float, double) for our purpose in the MED++ library.
*/

template <class T> class MEDARRAY 
// Class Template MEDARRAY
// template to generate an array of any
// particular type (int, long, float, double)
// for our purpose in the MED++ library
{
private :
  med_int _ldValues; 	          // leading dimension of value (example : space dimension with coordinates)
  med_int _lengthValues;          // length of values (example : number of nodes with coordinates) 
  medModeSwitch _mode;            // data access mode:
        		          // MED_FULL_INTERLACE (default mode) 
			          // or MED_NO_INTERLACE
  PointerOf <T> _valuesDefault;   // in _mode representation
  PointerOf <T> _valuesOther;     // in other _mode representation

public :
  MEDARRAY(): _ldValues(0), _lengthValues(0), _mode(MED_FULL_INTERLACE),
	      _valuesDefault(), _valuesOther()
  {
  };

  ~MEDARRAY()
  {
  };
  

  // the following constructor are for the generation
  // with the default or not _mode

  MEDARRAY(const med_int ld_values, const med_int length_values, 
	   const medModeSwitch mode=MED_FULL_INTERLACE) ;

  MEDARRAY( T*values, const med_int ld_values,
	   const med_int length_values, const medModeSwitch mode=MED_FULL_INTERLACE) ;

  MEDARRAY(MEDARRAY const &m );

  inline med_int getLeadingValue() const; // as Space Dimension with coordinates
  inline med_int getLengthValue()  const; // as Number of Nodes with coordinates
 
  T * const get   (const medModeSwitch mode) ;
  T * const getI  (const medModeSwitch mode, const med_int i) ;
  T   const getIJ (const med_int i,          const med_int j) const;

  // need by write, to get default mode !
  inline medModeSwitch getMode() const ;
  
  void set   (medModeSwitch mode, T* value);
  void setI  (medModeSwitch mode, med_int i, T* value);
  void setIJ (med_int i,          med_int j, T  value);

private:
  void calculateOther() ;
  void allocateOther();
  void updateDefault();
};


// implemented code

template <class T> MEDARRAY<T>::MEDARRAY(const med_int ld_values,
					 const med_int length_values,
					 const medModeSwitch mode):
						_ldValues(ld_values),
						_lengthValues(length_values),
						_mode(mode),
						_valuesOther(),
  						_valuesDefault(length_values*ld_values)
{
  BEGIN_OF("constructor MEDARRAY<T>::MEDARRAY(const med_int, const med_int, const medModeSwitch(= MED_FULL_INTERLACE))");

  // if could not allocate 

  if ((ld_values<1)|(length_values<1))
  {
    throw MEDEXCEPTION(LOCALIZED("MEDARRAY<T>::MEDARRAY(const med_int, const med_int, const medModeSwitch) : dimension < 1 !"));
  }

  SCRUTE((T*)_valuesDefault);
  SCRUTE((T*)_valuesOther);
  SCRUTE(_ldValues);
  SCRUTE(_lengthValues);
  SCRUTE(_mode);

  END_OF("constructor MEDARRAY<T>::MEDARRAY(const med_int, const med_int, const medModeSwitch (= MED_FULL_INTERLACE))");
}

template <class T> MEDARRAY<T>::MEDARRAY( T*values,
					  const med_int ld_values,
					  const med_int length_values,
					  const medModeSwitch mode): 
						_ldValues(ld_values),
						_lengthValues(length_values),
						_mode(mode),
						_valuesOther()
{
	_valuesDefault.set(values);
}

template <class T> MEDARRAY<T>::MEDARRAY(MEDARRAY<T> const & m ):
  			_ldValues(m._ldValues), 
			_lengthValues(m._lengthValues),
  			_mode(m._mode), 
			_valuesDefault((int)m._ldValues*m._lengthValues)
{
  if ( m._ldValues*m._lengthValues >= 0 )
  {
  	memcpy((T*)_valuesDefault,(const T* const)m._valuesDefault,m._ldValues*m._lengthValues*sizeof(T));
  }
  if ((const T* const)(m._valuesOther) != NULL) 
  {
      _valuesOther.set(m._ldValues*m._lengthValues);
      memcpy((T*)_valuesOther,(const T* const)m._valuesOther,m._ldValues*m._lengthValues*sizeof(T));
  }
  else
  {
	_valuesOther.set(0);
  }
}

template <class T> inline med_int MEDARRAY<T>::getLeadingValue() const 
{
  return _ldValues ;
};

template <class T> inline med_int MEDARRAY<T>::getLengthValue() const
{
  return _lengthValues ;
};

template <class T> T* const MEDARRAY<T>::get(const medModeSwitch mode) 
{
  if ((T*)_valuesDefault != NULL)
    if (mode == _mode) 
      {
    	return _valuesDefault;
      }
    else 
      {
  	calculateOther() ;
      	return _valuesOther ;
      }
  throw MEDEXCEPTION("MEDARRAY::get(mode) : No values defined !");
}

template <class T> T* const MEDARRAY<T>::getI(const medModeSwitch mode,const med_int i) 
{
  // 1<=i<=_lengthValues and return an array of _ldValues length
  // if MED_FULL_INTERLACE
  // 1<=i<=_ldValues and return an array of _lengthValues length
  // if MED_NO_INTERLACE

  if (i<=0) 
      throw MEDEXCEPTION("MEDARRAY::getI(mode,i) : argument i must be > 0");

  if (( T* )_valuesDefault != NULL) 
  {
    // if mode = MED_FULL_INTERLACE :
    int first = _ldValues ;
    int second = _lengthValues;

    // else :
    if (mode == MED_NO_INTERLACE) 
    {
      first = _lengthValues ;
      second = _ldValues ;
    }

    if (i>second) 
      throw MEDEXCEPTION("MEDARRAY::getI(mode,i) : argument i must be <= second");

    if (mode == _mode)
      return _valuesDefault + (i-1)*first ;
    else 
    {
      calculateOther() ;
      return _valuesOther + (i-1)*first ;
    }
  }
  throw MEDEXCEPTION("MEDARRAY::getI(mode,i) : No values defined !");
}


template <class T> T const MEDARRAY<T>::getIJ(const med_int i,const  med_int j) const
{
  // 1<=i<=_lengthValues and 1<j<_ldValues

  if (i<1) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument i must be >= 1");
  if (i>_lengthValues) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument i must be <= _lengthValues");

  if (j<1) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument j must be >= 1");
  if (j>_ldValues) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument j must be <= _ldValues");

  if ( (const T* const)_valuesDefault !=  NULL) 
  {
    switch (_mode)
    {
        case MED_FULL_INTERLACE :
	{
  	     return _valuesDefault[(i-1)*_ldValues+j-1];
	}
        case MED_NO_INTERLACE : 
	{
	     return _valuesDefault[(j-1)*_lengthValues+i-1];
	}
    }
  } 
  else
        throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : No value in array !");
}

template <class T> inline medModeSwitch MEDARRAY<T>::getMode() const 
{
  return _mode ;
}

template <class T> void MEDARRAY<T>::set(medModeSwitch mode, T* value)
{

  BEGIN_OF("MEDARRAY<T>::set(mode,i,value)");

  SCRUTE(_ldValues);
  SCRUTE(_lengthValues);
  SCRUTE((T*)_valuesDefault);
  SCRUTE((T*)_valuesOther);
  SCRUTE(_mode);
  SCRUTE(mode);
  SCRUTE(value);

  if ((T*)_valuesDefault != NULL) 
  {
      if (mode == _mode)
      {
	_valuesDefault = value ;
	if ((T*)_valuesOther != NULL) calculateOther() ;
      }
      else
      {
	allocateOther() ;
	_valuesOther = value ;
	updateDefault() ;
      }
   }
   else 
   {
	throw MEDEXCEPTION("MEDARRAY::set(mode,value) : No values defined !");
   }

   END_OF("MEDARRAY<T>::set(mode,i,value)");
}

template <class T> void MEDARRAY<T>::setI(medModeSwitch mode, med_int i, T* value)
{
  BEGIN_OF("MEDARRAY<T>::setI(mode,i,value)");

  SCRUTE(i);
  SCRUTE(_ldValues);
  SCRUTE(_lengthValues);
  SCRUTE((T*)_valuesDefault);
  SCRUTE((T*)_valuesOther);
  SCRUTE(_mode);
  SCRUTE(mode);

  // 1<=i<=_lengthValues and return an array of _ldValues length
  // if MED_FULL_INTERLACE
  // 1<=i<=_ldValues and return an array of _lengthValues length
  // if MED_NO_INTERLACE

  if (i<=0) 
  {
      throw MEDEXCEPTION("MEDARRAY::setI(mode,i,value) : argument i must be > 0");
  }
  if ((T*)_valuesDefault != NULL) 
  {
    int first  = _ldValues;
    int second = _lengthValues;

    if (mode == MED_NO_INTERLACE) 
    {
        first  = _lengthValues;
        second = _ldValues;
    }
    SCRUTE(second);

    if ( i > second) 
    {
      SCRUTE(i);
      SCRUTE(second);
      throw MEDEXCEPTION("MEDARRAY::setI(mode,i,value) : argument i must be <= second");
    }
    if (mode == _mode)
    {
	for (int k =0;k<first;k++)
	  {
	  	(_valuesDefault + (i-1)*first)[k] = value[k];
	  }
	if ((T*)_valuesOther != NULL)
	  for (int k =0;k<first;k++) 
	  {
	    	_valuesOther[k*second+i-1] = value[k];
	  }
    }
    else
    {
	allocateOther() ;
	// 	return _valuesOther + (i-1)*first = &value ;
	for (int k =0;k<second;k++) 
	{
	  _valuesOther[(i-1)*first+k-1] = value[k];
	  _valuesDefault[k*second+i-1]  = value[k];
	}
    }
  }
  else 
  {
	throw MEDEXCEPTION("MEDARRAY::setI(mode,i,value) : No values defined !");
  }

  SCRUTE((T*) _valuesDefault);
  SCRUTE((T*) _valuesOther);

  END_OF("MEDARRAY::setI(mode,i,value)");
}

template <class T> void MEDARRAY<T>::setIJ(med_int i, med_int j, T value)
{
  // 1<=i<=_lengthValues and 1<=j<=_ldValues

  if (i<1) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument i must be >= 1");
  if (i>_lengthValues) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument i must be <= _lengthValues");

  if (j<1) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument j must be >= 1");
  if (j>_ldValues) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument j must be <= _ldValues");

  if ((T*)_valuesDefault != NULL) 
  {
    switch (_mode)
    {
      case MED_FULL_INTERLACE :
      {
	  _valuesDefault[(i-1)*_ldValues+j-1] = value;
	  break;
      }
      case MED_NO_INTERLACE : 
      {
	  _valuesDefault[(j-1)*_lengthValues+i-1] = value;
	  break;
      }
    }
  } 
  else
  {
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : No value in array !");
   }
}

template <class T> void MEDARRAY<T>::calculateOther()
{
  BEGIN_OF("MEDARRAY<T>::calculateOther()") ;
  SCRUTE((T*)_valuesDefault);
  SCRUTE((T*)_valuesOther);
  if ((T*)_valuesDefault != NULL) 
  {
	//we allocate _valuesOther if needed
	if ((T*)_valuesOther == NULL) 
	{
		_valuesOther.set(_ldValues*_lengthValues);
        }

	// we set _valuesOther
	int first = _ldValues ;
	int second = _lengthValues;
	if (_mode == MED_NO_INTERLACE) 
	{
		first = _lengthValues ;
		second = _ldValues;
	}
    	for (int i=0; i<first;i++)
      		for (int j=0; j<second; j++)
			_valuesOther[i*second+j] = _valuesDefault[j*first+i];
  }
  else
  {
    	throw MEDEXCEPTION("MEDARRAY::calculateOther() : No values defined !");
  }
  END_OF("MEDARRAY<T>::calculateOther()") ;
}

template <class T> void MEDARRAY<T>::allocateOther()
{
  if ((T*)_valuesDefault != NULL) 
  {
    if ((T*)_valuesOther == NULL) 
    {
	// we set _valuesOther
	_valuesOther.set(_ldValues*_lengthValues);
    }
  }
  else
    throw MEDEXCEPTION("MEDARRAY::allocateOther() : No values defined !");
}

template <class T> void MEDARRAY<T>::updateDefault()
{
  BEGIN_OF("MEDARRAY<T>::updateDefault()");

  if ((T*)_valuesDefault != NULL) 
  {
    if ((T*)_valuesOther != NULL) 
    {
      // we update _valuesDefault with _valuesOther
      // if mode = MED_FULL_INTERLACE :
      int first = _ldValues ;
      int second = _lengthValues;
      // else :
      if (_mode == MED_NO_INTERLACE) 
      {
	first = _lengthValues ;
	second = _ldValues;
      }
      for (int i=0; i<first;i++)
	for (int j=0; j<second; j++)
	  _valuesDefault[j*first+i] = _valuesOther[i*second+j];
    }
    else
    {
      throw MEDEXCEPTION("MEDARRAY<T>::updateDefault() : No valuesOther defined !");
    }
  }
  else
  {
    throw MEDEXCEPTION("MEDARRAY<T>::updateDefault() : No values defined !");
  }

  END_OF("MEDARRAY<T>::updateDefault()");
}

# endif 	/* # ifndef __MEDARRAY_H__ */
