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
//  File   : MEDMEM_Family.hxx
//  Module : MED

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

class FAMILY : public SUPPORT
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
  int *    _attributeIdentifier ;
  /*!
    \if developper
    Array of all attributes' values.
    There is one for each attribute.
    \endif
  */
  int *    _attributeValue ;
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
  */
  FAMILY( MESH* Mesh, int Identifier, string Name, 
	  int NumberOfAttribute, int *AttributeIdentifier,
          int *AttributeValue, string AttributeDescription,
          int NumberOfGroup,   string GroupName,
	  int * MEDArrayNodeFamily,
	  int ** MEDArrayCellFamily,
	  int ** MEDArrayFaceFamily,
	  int ** MEDArrayEdgeFamily
	  ) ;

			/*! Copy Constructor. */
  FAMILY(const FAMILY & m);

			/*! Constructor with SUPPORT entry. */
  FAMILY(const SUPPORT & s);

			/*! Destructor. */
  ~FAMILY();
  FAMILY & operator=(const FAMILY &fam);
  friend ostream & operator<<(ostream &os, FAMILY &my) ;

  bool build(medEntityMesh Entity,int **FamilyNumber);

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
inline void FAMILY::setAttributesIdentifiers(int * AttributeIdentifier) 
//---------------------------------------------------------------------
{ 
    _attributeIdentifier = AttributeIdentifier ; 
}

/*! Sets the attribute _attributeValue to AttributeValue. */
//-----------------------------------------------------------
inline void FAMILY::setAttributesValues(int * AttributeValue) 
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
inline int * FAMILY::getAttributesIdentifiers() const
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
inline int * FAMILY::getAttributesValues() const             
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
