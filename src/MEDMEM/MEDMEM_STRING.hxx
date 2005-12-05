// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
# ifndef MEDMEM_STRING_HXX
# define MEDMEM_STRING_HXX

using namespace std;

# include <string>
# include <strstream>

/*!
 A class to generate string from any type:

 here is a simple use case
      STRING msgErr;
      msgErr << "This Object cannot be instanciated by now ! Try "<< 11 << "times just to see what happens ";
      throw SALOME_EXCEPTION (LOCALIZED(msgErr)) ;
								*/
namespace MEDMEM {
class STRING : public string
{

private :
  ostrstream _s ;

public :
  
  STRING() :string(), _s() 
   {
   }

  ~STRING()
   {
	_s.freeze(false);
   }

  operator const char * () const 
   {     
        return const_cast <const char *> (this->c_str()) ;
   }

   template <class T> STRING( const T &valeur ) : string(), _s() 
   {
    	_s.freeze(false);

    	_s << valeur ;
        _s << ends;

    	this->string::operator =( _s.str());  // freeze is true by now
   }

   template <class T> STRING & operator<<( const T &valeur )
   {
  
        if ( _s.pcount() )
        {
                _s.seekp(-1, ios::cur); // Back up before NULL
                _s.rdbuf()->freeze(0); // Unfreeze it
        }

        _s << valeur ;
        _s << ends;

        this->string::operator = ( _s.str() ) ;  // freeze is true by now
        _s.freeze( false ) ;
        return *this ;


   }
} ;
} ;

# endif
