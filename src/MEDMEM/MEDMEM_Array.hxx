#ifndef __MEDARRAY_H__
#define __MEDARRAY_H__

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
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
  med_int _ld_values ; 	    // leading dimension of value (example : space dimension with coordinates)
  med_int _length_values ;  // length of values (example : number of nodes with coordinates) 
  medModeSwitch _mode;    // data access mode:
        		    // MED_FULL_INTERLACE (default mode) 
			    // or MED_NO_INTERLACE
  T * _valuesDefault ;       // in _mode representation
  T * _valuesOther   ;       // in other _mode

public :
  MEDARRAY():_ld_values(0),_length_values(0),_mode(MED_FULL_INTERLACE),_valuesDefault((T*)NULL),_valuesOther((T*)NULL)
  {
  };

  ~MEDARRAY()
  {
    if (_valuesDefault)
      delete [] _valuesDefault ;
    if (_valuesOther)
      delete [] _valuesOther ;
  };
  

  // the following constructor are for the generation
  // with the default or not _mode

  MEDARRAY(const med_int ld_values, const med_int length_values, 
	   const medModeSwitch mode=MED_FULL_INTERLACE) ;

  MEDARRAY( T*values, const med_int ld_values,
	   const med_int length_values, const medModeSwitch mode=MED_FULL_INTERLACE) ;

  inline med_int getLeadingValue() ; // as Space Dimension with coordinates
  inline med_int getLengthValue() ; // as Number of Nodes with coordinates
 
  T * const get(medModeSwitch mode) ;
  T* const getI(medModeSwitch mode,med_int i) ;
  T  getIJ(med_int i, med_int j) const ;

  // need by write, to get default mode !
  inline medModeSwitch getMode() const ;
  
  void set(medModeSwitch mode, T* value);
  void setI(medModeSwitch mode, med_int i, T* value);
  void setIJ(med_int i, med_int j, T value);

private:
  void calculateOther() ;
  void allocateOther();
  void updateDefault();
};


// implemented code

template <class T> MEDARRAY<T>::MEDARRAY(const med_int ld_values,
					 const med_int length_values,
					 const medModeSwitch mode = MED_FULL_INTERLACE): _ld_values(ld_values),_length_values(length_values),_mode(mode),_valuesOther((T*)NULL)
{
  BEGIN_OF("constructor MEDARRAY<T>::MEDARRAY(const med_int , const med_int, const medModeSwitch (= MED_FULL_INTERLACE))");

  // if could not allocate ?
  if ((ld_values<1)|(length_values<1))
    throw MEDEXCEPTION( LOCALIZED("MEDARRAY<T>::MEDARRAY(const med_int, const med_int, const medModeSwitch) : dimension < 1 !"));
  _valuesDefault = new T[ld_values*length_values] ;

  SCRUTE(_valuesDefault);

  SCRUTE(_valuesOther);

  SCRUTE(_ld_values);

  SCRUTE(_length_values);

  SCRUTE(_mode);

  END_OF("constructor MEDARRAY<T>::MEDARRAY(const med_int , const med_int, const medModeSwitch (= MED_FULL_INTERLACE))");
}

template <class T> MEDARRAY<T>::MEDARRAY( T*values,
					  const med_int ld_values,
					  const med_int length_values,
					  const medModeSwitch mode=MED_FULL_INTERLACE): _ld_values(ld_values),_length_values(length_values),_mode(mode),_valuesOther((T*)NULL)
{
  _valuesDefault = values;
}

template <class T> inline med_int MEDARRAY<T>::getLeadingValue() {
  return _ld_values ;
};
template <class T> inline med_int MEDARRAY<T>::getLengthValue() {
  return _length_values ;
};

template <class T> T* const MEDARRAY<T>::get(medModeSwitch mode) 
{
  if (_valuesDefault != NULL)
    if (mode == _mode) {
      return _valuesDefault;
    }
    else {
      calculateOther() ;
      return _valuesOther ;
    }
  throw MEDEXCEPTION("MEDARRAY::get(mode) : No values defined !");
}

template <class T> T* const MEDARRAY<T>::getI(medModeSwitch mode,med_int i)
{
  // 1<=i<=_length_values and return an array of _ld_values length
  // if MED_FULL_INTERLACE
  // 1<=i<=_ld_values and return an array of _length_values length
  // if MED_NO_INTERLACE

  if (_valuesDefault != NULL) {
    // if mode = MED_FULL_INTERLACE :
    int first = _ld_values ;
    int second = _length_values;
    // else :
    if (mode == MED_NO_INTERLACE) {
      first = _length_values ;
      second = _ld_values ;
    }
    if (i<=0) 
      throw MEDEXCEPTION("MEDARRAY::getI(mode,i) : argument i must be > 0");
    if (i>second) 
      throw MEDEXCEPTION("MEDARRAY::getI(mode,i) : argument i must be <= second");
    if (mode == _mode)
      return _valuesDefault + (i-1)*first ;
    else {
      calculateOther() ;
      return _valuesOther + (i-1)*first ;
      //      return _valuesOther + (i-1)*second ;
    }
  }
  throw MEDEXCEPTION("MEDARRAY::getI(mode,i) : No values defined !");
}


template <class T> T MEDARRAY<T>::getIJ(med_int i, med_int j) const
{
  // 1<=i<=_length_values and 1<j<_ld_values

  if (i<1) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument i must be >= 1");
  if (i>_length_values) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument i must be <= _length_values");

  if (j<1) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument j must be >= 1");
  if (j>_ld_values) 
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument j must be <= _ld_values");

  if (_valuesDefault != NULL) {
    switch (_mode)
      {
      case MED_FULL_INTERLACE :
	{
	  return _valuesDefault[(i-1)*_ld_values+j-1];
	}
      case MED_NO_INTERLACE : 
	{
	  return _valuesDefault[(j-1)*_length_values+i-1];
	}
      }
  } else
    throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : No value in array !");
}

template <class T> inline medModeSwitch MEDARRAY<T>::getMode() const 
{
  return _mode ;
}

template <class T> void MEDARRAY<T>::set(medModeSwitch mode, T* value)
{

  BEGIN_OF("MEDARRAY<T>::set(mode,i,value)");

  _mode = mode ;

  SCRUTE(_ld_values);
  SCRUTE(_length_values);
  SCRUTE(_valuesDefault);
  SCRUTE(_valuesOther);
  SCRUTE(_mode);
  SCRUTE(mode);
  SCRUTE(value);

  if (_valuesDefault != NULL)
    if (mode == _mode) {
      _valuesDefault = value ;
      if (_valuesOther != NULL) calculateOther() ;
      //      if (_valuesOther != NULL) _valuesOther = value;
    }
    else {
      allocateOther() ;
      _valuesOther = value ;
      updateDefault() ;
    }
  else throw MEDEXCEPTION("MEDARRAY::set(mode,value) : No values defined !");

  END_OF("MEDARRAY<T>::set(mode,i,value)");
}

template <class T> void MEDARRAY<T>::setI(medModeSwitch mode, med_int i, T* value)
{
  BEGIN_OF("MEDARRAY<T>::setI(mode,i,value)");

  _mode = mode ;

  SCRUTE(i);
  SCRUTE(_ld_values);
  SCRUTE(_length_values);
  SCRUTE(_valuesDefault);
  SCRUTE(_valuesOther);
  SCRUTE(_mode);
  SCRUTE(mode);

  // 1<=i<=_length_values and return an array of _ld_values length
  // if MED_FULL_INTERLACE
  // 1<=i<=_ld_values and return an array of _length_values length
  // if MED_NO_INTERLACE

  if (_valuesDefault != NULL) {
    // if mode = MED_FULL_INTERLACE :
    int first = _ld_values ;
    int second = _length_values;
    // else :
    if (mode == MED_NO_INTERLACE) {
      first = _length_values ;
      second = _ld_values ;
    }
    SCRUTE(second);
    if (i<=0) 
      throw MEDEXCEPTION("MEDARRAY::setI(mode,i,value) : argument i must be > 0");
    if (i>second) {
      SCRUTE(i);
      SCRUTE(second);
      throw MEDEXCEPTION("MEDARRAY::setI(mode,i,value) : argument i must be <= second");}
    if (mode == _mode)
      {
	//	_valuesDefault + (i-1)*first = value ;
	for (int k =0;k<first;k++)
	  //	  _valuesDefault[(i-1)*second+k] = value[k];
	  (_valuesDefault + (i-1)*first)[k] = value[k];
	if (_valuesOther != NULL)
	  for (int k =0;k<first;k++) {
	    _valuesOther[k*second+i-1] = value[k];
	  }
      }
    else
      {
	allocateOther() ;
	// 	return _valuesOther + (i-1)*first = &value ;
	for (int k =0;k<second;k++) {
	  _valuesOther[(i-1)*first+k-1] = value[k];
	  _valuesDefault[k*second+i-1] = value[k];
	}
      }
  }
  else throw MEDEXCEPTION("MEDARRAY::setI(mode,i,value) : No values defined !");

  SCRUTE(_valuesDefault);
  SCRUTE( _valuesOther);

  END_OF("MEDARRAY::setI(mode,i,value)");
}

template <class T> void MEDARRAY<T>::setIJ(med_int i, med_int j, T value)
{
  // 1<=i<=_length_values and 1<j<_ld_values

  if (i<1) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument i must be >= 1");
  if (i>_length_values) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument i must be <= _length_values");

  if (j<1) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument j must be >= 1");
  if (j>_ld_values) 
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument j must be <= _ld_values");

  if (_valuesDefault != NULL) {
    switch (_mode)
      {
      case MED_FULL_INTERLACE :
	{
	  _valuesDefault[(i-1)*_ld_values+j-1] = value;
	  break;
	}
      case MED_NO_INTERLACE : 
	{
	  _valuesDefault[(j-1)*_length_values+i-1] = value;
	  break;
	}
      }
  } else
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : No value in array !");
}

template <class T> void MEDARRAY<T>::calculateOther()
{
  if (_valuesDefault != NULL) {
    //we allocate _valuesOther if needed
    if (_valuesOther == NULL) _valuesOther = new T[_ld_values*_length_values] ;
    // we set _valuesOther
    // if mode = MED_FULL_INTERLACE :
    int first = _ld_values ;
    int second = _length_values;
    // else :
    if (_mode == MED_NO_INTERLACE) {
      first = _length_values ;
      second = _ld_values;
    }

    for (int i=0; i<first;i++)
      for (int j=0; j<second; j++)
	_valuesOther[i*second+j] = _valuesDefault[j*first+i];
  }
  else
    throw MEDEXCEPTION("MEDARRAY::calculateOther() : No values defined !");
}

template <class T> void MEDARRAY<T>::allocateOther()
{
  if (_valuesDefault != NULL) {
    if (_valuesOther == NULL) {
      // we set _valuesOther
      _valuesOther = new T[_ld_values*_length_values] ;
    }
  }
  else
    throw MEDEXCEPTION("MEDARRAY::allocateOther() : No values defined !");
}

template <class T> void MEDARRAY<T>::updateDefault()
{
  BEGIN_OF("MEDARRAY<T>::updateDefault()");

  if (_valuesDefault != NULL) {
    if (_valuesOther != NULL) {
      // we update _valuesDefault with _valuesOther
      // if mode = MED_FULL_INTERLACE :
      int first = _ld_values ;
      int second = _length_values;
      // else :
      if (_mode == MED_NO_INTERLACE) {
	first = _length_values ;
	second = _ld_values;
      }
      for (int i=0; i<first;i++)
	for (int j=0; j<second; j++)
	  _valuesDefault[j*first+i] = _valuesOther[i*second+j];
    }
    else
      throw MEDEXCEPTION("MEDARRAY<T>::updateDefault() : No valuesOther defined !");
  }
  else
    throw MEDEXCEPTION("MEDARRAY<T>::updateDefault() : No values defined !");

  END_OF("MEDARRAY<T>::updateDefault()");
}

# endif 	/* # ifndef __MEDARRAY_H__ */
