using namespace std;
/*
 File MedException.cxx
 $Header$
*/

using namespace std;

#include "utilities.h"
#include "MEDMEM_Exception.hxx"
using namespace MEDMEM;

extern "C"
{
#include <math.h>
#include <stdio.h>
#include <string.h>
}


/*!
  \internal
  Function used to duplicate char * 
*/
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

/*!
  \internal Default Constructor (Should not be used)
*/
// ------------------------------------------------------ //
MEDEXCEPTION::MEDEXCEPTION( void ): exception() , _text(0)
// ------------------------------------------------------ //
{
  MESSAGE( "You must user the standard builder : MEDEXCEPTION::MEDEXCEPTION( const char *text )" ) ;
  INTERRUPTION(1) ;
}


/*!
  \internal
  Function used to elaborate the text of the MEDEXCEPTION
*/
// --------------------------------------------------------------------------------------- //
const char *makeText( const char *text, const char *fileName, const unsigned int lineNumber )
// --------------------------------------------------------------------------------------- //
{
  char *newText = 0 ;

  ASSERT(text) ;
  const size_t l1 = strlen(text) ;

  const char* prefix = "MED Exception" ;
  const size_t l0 = strlen(prefix) ;

  if ( fileName )
  {
      const size_t l2 = strlen(fileName) ;

      ASSERT(lineNumber>=1) ;
      const size_t l3 = 1+int(log10(float(lineNumber))) ;
	
      const size_t l4 =  l0+l1+l2+l3+10+1 ;
      newText = new char [ l4 ] ;
      sprintf( newText , "%s in %s [%u] : %s" , prefix, fileName, lineNumber, text ) ;
      ASSERT(newText[l4-1] == '\0' );

  }
  else
  {
      newText = new char [ l0+l1+3+1 ] ;
      sprintf( newText , "%s : %s" , prefix, text ) ;
   }
  ASSERT(newText) ;
  return newText ;
}

/*!
    Constructor : \n
    It will create the text of the MEDEXCEPTION from the different parameters.
    It will take the form : \n
    MEDEXCEPTION, fileName, lineNumber and text of the exception
*/
// ------------------------------------------------------------------------------------------------ //
MEDEXCEPTION::MEDEXCEPTION( const char *text, const char *fileName, const unsigned int lineNumber ) : 
	      exception(), _text( makeText( text , fileName , lineNumber ) )
// ------------------------------------------------------------------------------------------------ //
{
  MESSAGE(_text);
}

/*!
  Destructor : \n
  If necessary desallocates Memory
*/

// ------------------------------------//
MEDEXCEPTION::~MEDEXCEPTION() throw ()
// ------------------------------------//
{
  if ( _text )
    {
      delete [] _text ;
      char *& txt = (char*)_text ;
      txt = 0 ;
    }
  ASSERT(_text==NULL) ;
}


/*!
  Copy Constructor : \n
  Should not be used very often
*/
// ----------------------------------------------------------------------- //
MEDEXCEPTION::MEDEXCEPTION( const MEDEXCEPTION &ex ): _text(duplicate(ex._text))
// ----------------------------------------------------------------------- //
{
  ;
}
/*!
  Operator << : put the message to the given stream.
*/
// ------------------------------------------------------- //
ostream & MEDMEM::operator<<( ostream &os , const MEDEXCEPTION &ex )
// ------------------------------------------------------- //
{
  os << ex._text ;
  return os ;
}

/*!
  Return a char * which contain the message.
*/
// ------------------------------------------------- //
const char* MEDEXCEPTION::what( void ) const throw ()
// ------------------------------------------------- //
{
  return _text ;
}


// -------------------- class MED_DRIVER_NOT_FOUND_EXCEPTION


MED_DRIVER_NOT_FOUND_EXCEPTION::MED_DRIVER_NOT_FOUND_EXCEPTION(const MED_DRIVER_NOT_FOUND_EXCEPTION &ex ):MEDEXCEPTION ( ex ) {};


MED_DRIVER_NOT_FOUND_EXCEPTION::MED_DRIVER_NOT_FOUND_EXCEPTION
(
 const char *text, const char *fileName/*=0*/, 
 const unsigned int lineNumber/*=0*/ 
 ) : MEDEXCEPTION(text, fileName, lineNumber) {};

MED_DRIVER_NOT_FOUND_EXCEPTION::~MED_DRIVER_NOT_FOUND_EXCEPTION() throw (){};
