#ifndef FAMILY_HXX
#define FAMILY_HXX

#include <string>
#include "MEDMEM_Support.hxx"

class FAMILY : public SUPPORT
{
protected :
			/*! Identifier of the family in the mesh
			    Note : There is precisely one for each family. */
  int      _identifier ;
 			/*! Number of attribute of the family */ 
  int      _numberOfAttribute ;
			/*! Array of all attributes' identifiers.
        		    There is one for each attribute.  */ 
  int *    _attributeIdentifier ;
			/*! Array of all attributes' values.
        		    There is one for each attribute.  */ 
  int *    _attributeValue ;
			/*! Array of all attributes' descriptions.
        		    There is one for each attribute.  */ 
  string * _attributeDescription ;

			/*! Number of the group the family belongs to */
  int      _numberOfGroup ;
			/*! Name of the group the family belongs to */
  string * _groupName ;

public:
  FAMILY();

  // constructor to use with med driver
  FAMILY(MESH* Mesh, int Identifier, string Name, int NumberOfAttribute,int *AttributeIdentifier,int *AttributeValue,string AttributeDescription,int NumberOfGroup,string GroupName) ;

  FAMILY(FAMILY & m);

  ~FAMILY();
  FAMILY & operator=(const FAMILY &fam);

  inline void setIdentifier             (int Identifier);        
  inline void setNumberOfAttributes     (int NumberOfAttribute);
  inline void setAttributesIdentifiers  (int * AttributeIdentifier);
  inline void setAttributesValues       (int * AttributeValue);
  inline void setAttributesDescriptions (string * AttributeDescription); 
  inline void setNumberOfGroups         (int NumberOfGroups);
  inline void setGroupsNames            (string * GroupName);

  inline int      getIdentifier()              const;
  inline int      getNumberOfAttributes()      const;
  inline int *    getAttributesIdentifiers()   const;
  inline int *    getAttributesValues()        const;
  inline string * getAttributesDescriptions()  const;
  inline int      getNumberOfGroups()          const;
  inline string * getGroupsNames()             const;

  // A FAIRE : VERIFIER LA VALIDITE DES PARAMETRES !
  inline int      getAttributeIdentifier(int i)  const;
  inline int      getAttributeValue(int i)       const;
  inline string   getAttributeDescription(int i) const;
  inline string   getGroupName(int i)            const;
};

// inline methods :

/* Set the attribute _identifier to Identifier. */
//----------------------------------------------
inline void FAMILY::setIdentifier(int Identifier)         
//----------------------------------------------
{ 
    _identifier = Identifier; 
}

/* Set the attribute _numberOfAttribute to NumberOfAttribute. */
//--------------------------------------------------------------
inline void FAMILY::setNumberOfAttributes(int NumberOfAttribute) 
//--------------------------------------------------------------
{ 
    _numberOfAttribute = NumberOfAttribute; 
}

/* Set the attribute _attributeIdentifier to AttributeIdentifier. */
//---------------------------------------------------------------------
inline void FAMILY::setAttributesIdentifiers(int * AttributeIdentifier) 
//---------------------------------------------------------------------
{ 
    _attributeIdentifier = AttributeIdentifier ; 
}

/* Set the attribute _attributeValue to AttributeValue. */
//-----------------------------------------------------------
inline void FAMILY::setAttributesValues(int * AttributeValue) 
//-----------------------------------------------------------
{ 
    _attributeValue = AttributeValue ; 
}

/* Set the attribute _identifier to Identifier. */
//--------------------------------------------------------------------------
inline void FAMILY::setAttributesDescriptions(string * AttributeDescription) 
//--------------------------------------------------------------------------
{ 
    _attributeDescription = AttributeDescription ; 
}

/* Set the attribute _identifier to Identifier. */
//-------------------------------------------------------
inline void FAMILY::setNumberOfGroups(int NumberOfGroups) 
//-------------------------------------------------------
{ 
    _numberOfGroup = NumberOfGroups ; 
}

/* Set the attribute _identifier to Identifier. */
//----------------------------------------------------
inline void FAMILY::setGroupsNames(string * GroupName) 
//----------------------------------------------------
{ 
    _groupName = GroupName ; 
}
//--------------------------------------
inline int FAMILY::getIdentifier() const
//--------------------------------------
{ 
    return _identifier ; 
}
//----------------------------------------------
inline int FAMILY::getNumberOfAttributes() const
//----------------------------------------------
{ 
    return _numberOfAttribute ; 
}
//---------------------------------------------------
inline int * FAMILY::getAttributesIdentifiers() const
//---------------------------------------------------
{ 
    return _attributeIdentifier ; 
}
//----------------------------------------------------
inline int FAMILY::getAttributeIdentifier(int i) const     
//----------------------------------------------------
{ 
    return _attributeIdentifier[i-1] ; 
}
//----------------------------------------------
inline int * FAMILY::getAttributesValues() const             
//----------------------------------------------
{ 
    return _attributeValue ; 
}
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
//--------------------------------------------------------
inline string FAMILY::getAttributeDescription(int i) const 
//--------------------------------------------------------
{ 
    return _attributeDescription[i-1] ; 
}
//------------------------------------------
inline int FAMILY::getNumberOfGroups() const                   
//------------------------------------------
{ 
    return _numberOfGroup; 
}
//--------------------------------------------
inline string * FAMILY::getGroupsNames() const               
//--------------------------------------------
{ 
    return _groupName ; 
}
//---------------------------------------------
inline string FAMILY::getGroupName(int i) const            
//---------------------------------------------
{ 
    return _groupName[i-1] ; 
}
  

#endif /* FAMILY_HXX */
