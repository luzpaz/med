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
//  File   : MEDMEM_Exception.hxx
//  Module : MED

/*
 File MedException.hxx
 $Header$
*/

#ifndef MEDEXCEPTION_HXX
#define MEDEXCEPTION_HXX

using namespace std;

#include <exception>
#include <iostream>

#ifndef LOCALIZED
#define LOCALIZED(message) message , __FILE__ , __LINE__
#endif

#define MED_EXCEPTION MEDEXCEPTION

/*!
  Class used to throws exception.\n
  Inherits from public exception.
*/
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

#endif	/* MEDEXCEPTION_HXX */
