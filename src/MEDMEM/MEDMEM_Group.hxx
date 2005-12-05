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
/*
 File Group.hxx
 $Header$
*/

#ifndef GROUP_HXX
#define GROUP_HXX

#include <vector>
#include <list>
#include "MEDMEM_Family.hxx"


/*!

  This class describe a group of elements on an entity.\n
  It inherits from SUPPORT.\n
  It is a blending of some FAMILY class.\n\n

*/

namespace MEDMEM {
class FAMILY;
class GROUP : virtual public SUPPORT
{
protected :
  /*!
    \if developper
    Number of families in the group.
    \endif
  */
  int             _numberOfFamilies ;
  /*!
    \if developper
    Vector of families in the group.
    Note that Families are numbered from 1 to N.
    \endif
  */
  vector<FAMILY*> _family ;
  
public:
  /*! Constructor. */
  GROUP();
  /*! Constructor. */
  GROUP(const string & name, const list<FAMILY*> & family) throw (MEDEXCEPTION) ;
  /*! Copy Constructor. */
  GROUP(const GROUP & m);
  /*! Destructor. */
  virtual ~GROUP();

  /*! Copy operator. */
  // PN ATTention il n y a pas de copie du vecteur Family ????
  GROUP & operator=(const GROUP &group);

  /*! Operator << */
  friend ostream & operator<<(ostream &os, GROUP &my) ;

  inline void setNumberOfFamilies(int numberOfFamilies);
  inline void setFamilies(vector<FAMILY*> Family);
  
  inline int 	         getNumberOfFamilies() const ;
  inline vector<FAMILY*> getFamilies() const ;
  inline FAMILY * 	 getFamily(int i) const ;

};

// inline method :

/*! set the attribut _numberOfFamilies to numberOfFamilies */
//----------------------------------------------------------
inline void GROUP::setNumberOfFamilies(int numberOfFamilies) 
//----------------------------------------------------------
{
  _numberOfFamilies = numberOfFamilies; 
};


/*! set the attribut _family to Family              */
//----------------------------------------------------
inline void GROUP::setFamilies(vector<FAMILY*> Family) 
//----------------------------------------------------
{ 
  _family = Family; 
};

/*! returns number of families in the group */
//--------------------------------------------
inline int GROUP::getNumberOfFamilies() const        
//-------------------------------------------
{ 
  return _numberOfFamilies; 
};

/*! returns the vector of families in the group */
//------------------------------------------------
inline vector<FAMILY*> GROUP::getFamilies() const 
//------------------------------------------------
{ 
  return _family; 
};

/*! returns  a reference on family I of the group */
//--------------------------------------------------
FAMILY * GROUP::getFamily(int i) const   
//--------------------------------------------------
{ 
  return _family[i-1]; 
};
}//End namespace MEDMEM

#endif /* GROUP_HXX */
