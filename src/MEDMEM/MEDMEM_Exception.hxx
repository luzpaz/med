/*
 File MedException.hxx
 $Header$
*/

#ifndef MEDEXCEPTION_HXX
#define MEDEXCEPTION_HXX

#include <exception>
#include <iostream>

using namespace std;

#ifndef LOCALIZED
#define LOCALIZED(message) message , __FILE__ , __LINE__
#endif

#define MED_EXCEPTION MEDEXCEPTION

/*!
  Class used to throws exception.\n
  Inherits from public exception.
*/
namespace MEDMEM {
//--------------------------------------//
  class MEDEXCEPTION : public exception
//--------------------------------------//
{
private :
  MEDEXCEPTION(void);

protected :
  const char* _text ;

public :
  MEDEXCEPTION(const char *text, const char *fileName=0, 
	       const unsigned int lineNumber=0 );
  MEDEXCEPTION(const MEDEXCEPTION &ex );
  ~MEDEXCEPTION() throw ();
  friend ostream & operator<< (ostream &os, const MEDEXCEPTION &ex );
  virtual const char *what(void) const throw () ;
} ;


//---------------------------------------------------------//
class MED_DRIVER_NOT_FOUND_EXCEPTION : public MEDEXCEPTION
//---------------------------------------------------------//
{
private :
  MED_DRIVER_NOT_FOUND_EXCEPTION(void);

public :
  MED_DRIVER_NOT_FOUND_EXCEPTION(const char *text, const char *fileName=0, 
                                 const unsigned int lineNumber=0 );
  MED_DRIVER_NOT_FOUND_EXCEPTION(const MED_DRIVER_NOT_FOUND_EXCEPTION &ex );
  ~MED_DRIVER_NOT_FOUND_EXCEPTION() throw();

};
};

#endif	/* MEDEXCEPTION_HXX */
