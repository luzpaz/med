# if ! defined( __PointerOf_HXX__ )
# define __PointerOf_HXX__

#include <cstdlib>
#include "utilities.h"

/*!
	The template class PointerOf embedding a standard pointer (_pointer) is in charge of
	managing a the pointed memory.
*/

template <typename T> class PointerOf
{
protected :
	T* _pointer ;
	bool _done ;
public :
	PointerOf() ; // 
	PointerOf( const int &size ) ;
	PointerOf( const T *pointer ) ;
	~PointerOf() ;
  operator T*() ;
	operator const T*() const ;

  //operator T*() const ;

	void set( const int &size ) ;
	void set( const T *pointer ) ;
	PointerOf<T>& operator=( const PointerOf<T> &pointer ) ;
} ;

template <typename T> PointerOf<T>& PointerOf<T>::operator=( const PointerOf<T> &pointer )
{
	BEGIN_OF("PointerOf<T>::operator=( const PointerOf<T> &pointer )") ;
	if ( &pointer != this )
	{
		this->set( pointer._pointer ) ;
	}
	END_OF("PointerOf<T>::operator=( const PointerOf<T> &pointer )") ;
	return *this ;
}

template <typename T> PointerOf<T>::PointerOf() : _pointer(0), _done(false)
{
}

template <typename T> PointerOf<T>::PointerOf( const int &size ) : _pointer( new T[ size ]), _done(true)
{
}

template <typename T> PointerOf<T>::PointerOf( const T* pointer ) : _pointer( (T*)pointer ), _done(false)
{
}

template <typename T> PointerOf<T>::~PointerOf()
{
	if ( _pointer )
	{
		if( _done )
		{
			MESSAGE("PointerOf<T>::~PointerOf() --> deleting _pointer") ;
			delete [] _pointer ;
			_done = false ;
		}
		else
		{
			MESSAGE("_pointer is only nullified") ;
		}
		_pointer = 0 ;
	}
}

template <typename T> PointerOf<T>::operator T*()
{
	return _pointer ;
}

template <typename T> PointerOf<T>::operator const T*() const
{
	return _pointer ;
}

//  template <typename T> PointerOf<T>::operator T*() const
//  {
//  	return _pointer ;
//  }

template <typename T> void PointerOf<T>::set( const int &size )
{
	ASSERT(size>0) ;
	if ( _pointer && _done )
	{
		delete [] _pointer ; 
		_pointer=0 ;
	}
	_pointer = new T[ size ] ;
	_done = true ;
	return ;
}
template <typename T> void PointerOf<T>::set( const T *pointer )
{
	MESSAGE( "PointerOf<T>::set( const T *pointer )" ) ;
	SCRUTE(pointer) ;
	SCRUTE(_done) ;
	if ( _pointer && _done )
	{
		MESSAGE("PointerOf<T>::set --> deleting _pointer") ;
		delete [] _pointer ; 
		_pointer=0 ;
		_done=false ;
	}
	_pointer=(T*)pointer ;
	_done=false ;
	MESSAGE( "PointerOf<T>::set( const T *pointer )" ) ;
	return ;
}

# endif		/* # if ! defined( __PointerOf_HXX__ ) */
