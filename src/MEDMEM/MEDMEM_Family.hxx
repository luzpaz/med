#ifndef FAMILY_HXX
#define FAMILY_HXX

#include <string>
#include "MEDMEM_Support.hxx"

/*!

  This class describe a family of elements on an entity./n
  It inherits from support. /n
  It contains a list of elements (by SUPPORT class inheritance)
  and a description of some attributs./n

  All families on one entity represent a mesh partition for this entity.

*/
namespace MEDMEM {
class FAMILY : virtual public SUPPORT
{
protected :
  /*!
    \if developper
    Identifier of the family in the mesh
    Note : There is precisely one for each family.
    \endif
  */
  int      _identifier ;
  /*!
    \if developper
    Number of attribute of the family ./n
    Note that attributes are numbered from 1 to N.
    \endif
  */
  int      _numberOfAttribute ;
  /*!
    \if developper
    Array of all attributes' identifiers.
    There is one for each attribute.
    \endif
  */ 
//CCRT  int *    _attributeIdentifier ;
  med_int *    _attributeIdentifier ;
  /*!
    \if developper
    Array of all attributes' values.
    There is one for each attribute.
    \endif
  */
//CCRT  int *    _attributeValue ;
  med_int *    _attributeValue ;
  /*!
    \if developper
    Array of all attributes' descriptions.
    There is one for each attribute.
    \endif
  */
  string * _attributeDescription ;
  /*!
    \if developper
    Number of the group the family belongs to.
    \endif
  */
  int      _numberOfGroup ;
  /*!
    \if developper
    Name of the group the family belongs to.
    \endif
  */
  string * _groupName ;

public:
			/*! Constructor. */
  FAMILY();
  /*!
    \if developper
    Constructor to use with med driver.
    \endif
  */
//CCRT  FAMILY( MESH* Mesh, int Identifier, string Name, 
//CCRT	  int NumberOfAttribute, int *AttributeIdentifier,
//CCRT          int *AttributeValue, string AttributeDescription,
//CCRT          int NumberOfGroup,   string GroupName,
//CCRT	  int * MEDArrayNodeFamily,
//CCRT	  int ** MEDArrayCellFamily,
//CCRT	  int ** MEDArrayFaceFamily,
//CCRT	  int ** MEDArrayEdgeFamily
  FAMILY( MESH* Mesh, int Identifier, string Name, 
	  int NumberOfAttribute, med_int *AttributeIdentifier,
          med_int *AttributeValue, string AttributeDescription,
          int NumberOfGroup,   string GroupName,
	  med_int * MEDArrayNodeFamily,
	  med_int ** MEDArrayCellFamily,
	  med_int ** MEDArrayFaceFamily,
	  med_int ** MEDArrayEdgeFamily
	  ) ;

			/*! Copy Constructor. */
  FAMILY(const FAMILY & m);

			/*! Constructor with SUPPORT entry. */
  FAMILY(const SUPPORT & s);

			/*! Destructor. */
  virtual ~FAMILY();
  FAMILY & operator=(const FAMILY &fam);
  friend ostream & operator<<(ostream &os, FAMILY &my) ;

  friend ostream & operator<<(ostream &os, const FAMILY &my) ;

//CCRT  bool build(medEntityMesh Entity,int **FamilyNumber);
  bool build(medEntityMesh Entity,med_int **FamilyNumber);

  inline void setIdentifier             (int Identifier);        
  inline void setNumberOfAttributes     (int NumberOfAttribute);
//CCRT  inline void setAttributesIdentifiers  (int * AttributeIdentifier);
  inline void setAttributesIdentifiers  (med_int * AttributeIdentifier);
//CCRT  inline void setAttributesValues       (int * AttributeValue);
  inline void setAttributesValues       (med_int * AttributeValue);
  inline void setAttributesDescriptions (string * AttributeDescription); 
  inline void setNumberOfGroups         (int NumberOfGroups);
  inline void setGroupsNames            (string * GroupName);

  inline int      getIdentifier()              const;
  inline int      getNumberOfAttributes()      const;
//CCRT  inline int *    getAttributesIdentifiers()   const;
  inline med_int *    getAttributesIdentifiers()   const;
//CCRT  inline int *    getAttributesValues()        const;
  inline med_int *    getAttributesValues()        const;
  inline string * getAttributesDescriptions()  const;
  inline int      getNumberOfGroups()          const;
  inline string * getGroupsNames()             const;

  // A FAIRE : VERIFIER LA VALIDITE DES PARAMETRES !
  inline int      getAttributeIdentifier(int i)  const;
  inline int      getAttributeValue(int i)       const;
  inline string   getAttributeDescription(int i) const;
  inline string   getGroupName(int i)            const;
};
};

using namespace MEDMEM;
// inline methods :
/*! Sets the attribute _identifier to Identifier. */
//----------------------------------------------
inline void FAMILY::setIdentifier(int Identifier)         
//----------------------------------------------
{ 
    _identifier = Identifier; 
}

/*! Sets the attribute _numberOfAttribute to NumberOfAttribute. */
//--------------------------------------------------------------
inline void FAMILY::setNumberOfAttributes(int NumberOfAttribute) 
//--------------------------------------------------------------
{ 
    _numberOfAttribute = NumberOfAttribute; 
}

/*! Sets the attribute _attributeIdentifier to AttributeIdentifier. */
//---------------------------------------------------------------------
//CCRTinline void FAMILY::setAttributesIdentifiers(int * AttributeIdentifier) 
inline void FAMILY::setAttributesIdentifiers(med_int * AttributeIdentifier) 
//---------------------------------------------------------------------
{ 
    _attributeIdentifier = AttributeIdentifier ; 
}

/*! Sets the attribute _attributeValue to AttributeValue. */
//-----------------------------------------------------------
//CCRTinline void FAMILY::setAttributesValues(int * AttributeValue) 
inline void FAMILY::setAttributesValues(med_int * AttributeValue) 
//-----------------------------------------------------------
{ 
    _attributeValue = AttributeValue ; 
}

/*! Sets the attribute _attributeDescription to  AttributeDescription. */
//--------------------------------------------------------------------------
inline void FAMILY::setAttributesDescriptions(string * AttributeDescription) 
//--------------------------------------------------------------------------
{ 
    _attributeDescription = AttributeDescription ; 
}

/*! Sets the attribute _numberOfGroup to NumberOfGroups. */
//-------------------------------------------------------
inline void FAMILY::setNumberOfGroups(int NumberOfGroups) 
//-------------------------------------------------------
{ 
    _numberOfGroup = NumberOfGroups ; 
}

/*! Sets the attribute _groupName to GroupName. */
//----------------------------------------------------
inline void FAMILY::setGroupsNames(string * GroupName) 
//----------------------------------------------------
{ 
    _groupName = GroupName ; 
}
/*! Returns the attribute _identifier./n
   Note that there is one identifier precisely for each family. */
//--------------------------------------
inline int FAMILY::getIdentifier() const
//--------------------------------------
{ 
    return _identifier ; 
}

/*! Returns the number of attributes of the family. */
//----------------------------------------------
inline int FAMILY::getNumberOfAttributes() const
//----------------------------------------------
{ 
    return _numberOfAttribute ; 
}
/*! Returns a pointer to attributes identifiers .
    (There are _numberOfAttribute attributes) */
//---------------------------------------------------
//CCRTinline int * FAMILY::getAttributesIdentifiers() const
inline med_int * FAMILY::getAttributesIdentifiers() const
//---------------------------------------------------
{ 
    return _attributeIdentifier ; 
}
/*! Returns identifer of the Ith attribute of the family./n
   Note that they are numbered from 1 to N */
//----------------------------------------------------
inline int FAMILY::getAttributeIdentifier(int i) const     
//----------------------------------------------------
{ 
    return _attributeIdentifier[i-1] ; 
}
/*! Returns a pointer to attributes values.
    (There are _numberOfAttribute attributes)*/
//----------------------------------------------
//CCRTinline int * FAMILY::getAttributesValues() const             
inline med_int * FAMILY::getAttributesValues() const             
//----------------------------------------------
{ 
    return _attributeValue ; 
}
/*! Returns value of the Ith attribute of the family./n
   Note that they are numbered from 1 to N */
//-----------------------------------------------
inline int FAMILY::getAttributeValue(int i) const          
//-----------------------------------------------
{ 
    return _attributeValue[i-1] ; 
}
//-------------------------------------------------------
inline string * FAMILY::getAttributesDescriptions() const    
//-------------------------------------------------------
{ 
    return _attributeDescription ; 
}
/*! Returns description of the Ith attribute of the family/n
   Note that they are numbered from 1 to N */
//--------------------------------------------------------
inline string FAMILY::getAttributeDescription(int i) const 
//--------------------------------------------------------
{ 
    return _attributeDescription[i-1] ; 
}
/*! Returns the number of groups the family belongs to.*/
//------------------------------------------
inline int FAMILY::getNumberOfGroups() const                   
//------------------------------------------
{ 
    return _numberOfGroup; 
}
/*! Returns a pointer to the names of the groups the family belongs to */
//--------------------------------------------
inline string * FAMILY::getGroupsNames() const               
//--------------------------------------------
{ 
    return _groupName ; 
}
/*! Returns the name of the Ith group the family belongs to./n
    Note that they are numbered from 1 to N*/
//---------------------------------------------
inline string FAMILY::getGroupName(int i) const            
//---------------------------------------------
{ 
    return _groupName[i-1] ; 
}
#endif /* FAMILY_HXX */
