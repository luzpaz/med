/*
 File MedException.cxx
 $Header$
*/

using namespace std;

#include "utilities.h"
#include "MEDMEM_Exception.hxx"

extern "C"
{
#include <math.h>
#include <stdio.h>
#include <string.h>
}


const char* duplicate( const char *const str ) ;

const char* duplicate( const char *const str )
{
	ASSERT(str!=NULL) ;
	const size_t length = strlen( str ) ;
	ASSERT(length>0) ;
	char *new_str = new char[ 1+length ] ;
	ASSERT(new_str) ;
	strcpy( new_str , str ) ;
	return new_str ;
}

MEDEXCEPTION::MEDEXCEPTION( void ): exception() , _text(0)
{
  MESSAGE( "You must user the standard builder : MEDEXCEPTION::MEDEXCEPTION( const char *text )" ) ;
  INTERRUPTION(1) ;
}


const char *makeText( const char *text, const char *fileName, const unsigned int lineNumber )
{
  char *newText = 0 ;

  ASSERT(text) ;
  const size_t l1 = 1+strlen(text) ;
  ASSERT(l1>1) ;

  const char* prefix = "MED Exception" ;
  const size_t l0 = 2+strlen(prefix) ;

  if ( fileName )
    {
      const size_t l2 = 4+strlen(fileName) ;
      ASSERT(l2>4) ;

      ASSERT(lineNumber>=1) ;
      const size_t l3 = 4+int(log10(float(lineNumber))) ;
		
      newText = new char [ 1+l0+l1+l2+l3 ] ;
      sprintf( newText , "%s in %s [%u] : %s" , prefix, fileName, lineNumber, text ) ;
    }
  else
    {
      newText = new char [ 1+l0+l1 ] ;
      sprintf( newText , "%s : %s" , prefix, text ) ;
    }
  ASSERT(newText) ;
  return newText ;
}


MEDEXCEPTION::MEDEXCEPTION( const char *text, const char *fileName, const unsigned int lineNumber ) : exception(), _text( makeText( text , fileName , lineNumber ) )
{
  MESSAGE(_text);
}


MEDEXCEPTION::~MEDEXCEPTION() throw ()
{
  if ( _text )
    {
      delete [] _text ;
      char *& txt = (char*)_text ;
      txt = 0 ;
    }
  ASSERT(_text==NULL) ;
}



MEDEXCEPTION::MEDEXCEPTION( const MEDEXCEPTION &ex ): _text(duplicate(ex._text))
{
  ;
}


ostream & operator<<( ostream &os , const MEDEXCEPTION &ex )
{
  os << ex._text ;
  return os ;
}



const char* MEDEXCEPTION::what( void ) const throw ()
{
  return _text ;
}


// -------------------- class MED_DRIVER_NOT_FOUND_EXCEPTION


MED_DRIVER_NOT_FOUND_EXCEPTION::MED_DRIVER_NOT_FOUND_EXCEPTION(const MED_DRIVER_NOT_FOUND_EXCEPTION &ex ):MEDEXCEPTION ( ex ) {};


MED_DRIVER_NOT_FOUND_EXCEPTION::MED_DRIVER_NOT_FOUND_EXCEPTION
(
 const char *text, const char *fileName=0, 
 const unsigned int lineNumber=0 
 ) : MEDEXCEPTION(text, fileName, lineNumber) {};

MED_DRIVER_NOT_FOUND_EXCEPTION::~MED_DRIVER_NOT_FOUND_EXCEPTION() throw (){};
