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

class GROUP : public SUPPORT
{
protected :
				  /*! Number of families in the group */
  int             _numberOfFamilies ;
				  /*! Vector of families in the group */
  vector<FAMILY*> _family ;
  
public:
  GROUP();
  GROUP(const string & name, const list<FAMILY*> & family) throw (MEDEXCEPTION) ;
  GROUP(GROUP & m);
  ~GROUP();
  GROUP & operator=(const GROUP &group);
  
  inline void setNumberOfFamilies(int numberOfFamilies);
  inline void setFamilies(vector<FAMILY*> Family);
  
  inline int 	         getNumberOfFamilies() const ;
  inline vector<FAMILY*> getFamilies() const ;
  inline FAMILY * 	 getFamily(int i) const ;

//   void init(const list<FAMILY*> & family);
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
