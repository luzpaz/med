/*
 File Group.hxx
 $Header$
*/

#ifndef GROUP_HXX
#define GROUP_HXX

#include <vector>
#include <list>
//#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"

class FAMILY;

/*!

  This class describe a group of elements on an entity./n
  It inherits from SUPPORT./n
  It is a blending of some FAMILY class./n/n

*/

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


#endif /* GROUP_HXX */
