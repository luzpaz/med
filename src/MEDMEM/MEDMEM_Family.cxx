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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*
 File MEDMEM_Family.cxx
 $Header$
*/

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*
  Those defines are from the med File V2.1 and for this class they are fixed.
*/

#define MED_TAILLE_DESC 200
#define MED_TAILLE_LNOM  80

FAMILY::FAMILY():_identifier(0), _numberOfAttribute(0), _numberOfGroup(0)
{
    MESSAGE("FAMILY::FAMILY()");
};

FAMILY::FAMILY(MESH* Mesh, int Identifier, string Name, int NumberOfAttribute,
               int *AttributeIdentifier, int *AttributeValue, string AttributeDescription,
               int NumberOfGroup, string GroupName,
	       int * MEDArrayNodeFamily,
	       int ** MEDArrayCellFamily,
	       int ** MEDArrayFaceFamily,
	       int ** MEDArrayEdgeFamily
	       ): SUPPORT(Mesh,Name),
		  _identifier(Identifier), 
		  _numberOfAttribute(NumberOfAttribute), 
		  _numberOfGroup(NumberOfGroup)
{
  MESSAGE("FAMILY(int Identifier, string Name, int NumberOfAttribute,int *AttributeIdentifier,int *AttributeValue,string AttributeDescription,int NumberOfGroup,string GroupName, int ** Number) : "<<Identifier);

  _isOnAllElts = false ;
  SCRUTE(_numberOfAttribute);
  if (_numberOfAttribute > 0)
    {
      _attributeIdentifier.set(_numberOfAttribute,AttributeIdentifier);
      _attributeValue.set(_numberOfAttribute,AttributeValue);

      //_attributeDescription.set(_numberOfAttribute);
      _attributeDescription.resize(_numberOfAttribute);
      for (int i=0;i<NumberOfAttribute;i++) {
	_attributeDescription[i].assign(AttributeDescription,i*MED_TAILLE_DESC,MED_TAILLE_DESC);
	_attributeDescription[i].erase(strlen(_attributeDescription[i].c_str()));
	//SCRUTE(_attributeDescription[i]);
      }
    }
  else
    {
      _attributeIdentifier.set(_numberOfAttribute);
      _attributeValue.set(_numberOfAttribute);
      //_attributeDescription.set(_numberOfAttribute);
      _attributeDescription.resize(_numberOfAttribute);
    }
 
  //_groupName.set(_numberOfGroup);
  _groupName.resize(_numberOfGroup);
  for (int i=0;i<NumberOfGroup;i++) {
    _groupName[i].assign(GroupName,i*MED_TAILLE_LNOM,MED_TAILLE_LNOM);
    _groupName[i].erase(strlen(_groupName[i].c_str()));
    //SCRUTE(_groupName[i]);
  }

  // well, we must set SUPPORT attribut
  _description = "FAMILY" ;
  // on which geometric type :
  bool Find = false ;
  

  // ************************ NODES RELATED PART *************************************************
  // Scan every node family number <NodeFamilyNumber[i]> in order to create 
  // the list <tmp_NodesList> of node numbers <i+1> belonging to the family <_identifier>
  int NumberOfNodes         = _mesh->getNumberOfNodes();
  int NumberOfNodesInFamily = 0 ;
  //int * NodeFamilyNumber    = _mesh->getMEDArrayNodeFamily() ;   // EF : TEMPORAIRE !!
  int * tmp_NodesList       = new int[NumberOfNodes] ;           // Un peu brutal...., oui mais ce n'est qu'un tableau de travail !
  for (int i=0; i<NumberOfNodes; i++)
    if ( _identifier == MEDArrayNodeFamily[i] ) {
      tmp_NodesList[NumberOfNodesInFamily]=i+1 ;
      NumberOfNodesInFamily++;
    }

  SCRUTE(NumberOfNodesInFamily);

  // If we found nodes set the family attributes adequatly
  if (NumberOfNodesInFamily>0) {
    
    Find = true ;
    
    _entity = MED_NODE ;
//      _numberOfGeometricType = 1 ;
//      _geometricType = new medGeometryElement[1] ;
//      _geometricType[0]=MED_NONE ;
    
    // family on all NODE
    if (NumberOfNodesInFamily==NumberOfNodes) {
      _isOnAllElts = true ;
      update();
    } else {
      _numberOfGeometricType = 1 ;

      _geometricType.set(1) ;

      _geometricType[0]=MED_NONE ;
      _isOnAllElts= false ;

      _numberOfElements.set(1) ;

      _numberOfElements[0]=NumberOfNodesInFamily ;
      _totalNumberOfElements=NumberOfNodesInFamily;
      
//        _number=new MEDSKYLINEARRAY(1,NumberOfNodesInFamily) ;
//        int * NumberIndex = _number->getIndex();
//        int * NumberValue = _number->getValue();
      int * NumberIndex = new int[2];
      int * NumberValue = new int[NumberOfNodesInFamily];

      NumberIndex[0]=1;                          //set the MEDSKYLINEARRAY Index table
      NumberIndex[1]=1+NumberOfNodesInFamily;    //set the MEDSKYLINEARRAY Index table
      for(int i=0; i<NumberOfNodesInFamily; i++) // OH LA LA... 
	NumberValue[i]=tmp_NodesList[i] ;        // RESIZE de tmp_NodesList serait plus efficace !!!!!!!!
      // PG : pas de probl�me, si ca marche. Il faudra d�placer le delete !
      if(_number!=NULL) delete _number ;
      _number=new MEDSKYLINEARRAY(1,NumberOfNodesInFamily,NumberIndex,NumberValue) ;
      delete[] NumberIndex ;
      delete[] NumberValue ;
    }
  }
  delete[] tmp_NodesList ;
  


  // ************************ CELLS RELATED PART *************************************************
  // If we previously found nodes in our family don't scan the CELLS because a
  // family contains different geometic types of only one entity type.
  // ?? Scan every cell family number <NodeFamilyNumber[i]> in order to create 
  // ?? the list <tmp_NodesList> of node numbers <i+1> belonging to the family <_identifier>
  if (!Find) {
    Find = build(MED_CELL,MEDArrayCellFamily) ;
  }
  // on face ?
  if (!Find) {
    if ((_mesh->existConnectivityWithPoly(MED_NODAL,MED_FACE)) |
        (_mesh->existConnectivityWithPoly(MED_DESCENDING,MED_FACE))) {
      Find = build(MED_FACE,MEDArrayFaceFamily) ;
    }
  }

  // on edge ?
  if (!Find) {
    if ((_mesh->existConnectivity(MED_NODAL,MED_EDGE))|(_mesh->existConnectivity(MED_DESCENDING,MED_EDGE))) {
      Find = build(MED_EDGE,MEDArrayEdgeFamily) ;
    }
  }
  // That's all !

  // if not find : no entity in familly !!!
  if (!Find) {
    _numberOfGeometricType = 0 ;
    _isOnAllElts = false ;
    MESSAGE ("FAMILY() : No entity found !") ;
  }

  MESSAGE("Well now ??? :::");

  MESSAGE("Name : "<< getName());
  MESSAGE("Description : "<< getDescription());
  MESSAGE("Mesh name : " << getMesh()->getName());
  MESSAGE("Entity : "<< getEntity());
  MESSAGE("Entity list :");
  if (!(isOnAllElements())) {
    int numberoftypes = getNumberOfTypes() ;
    MESSAGE("NumberOfTypes : "<<numberoftypes);
    const medGeometryElement * types = getTypes();
    for (int j=0;j<numberoftypes;j++) {
      int numberOfElements = getNumberOfElements(types[j]);
      MESSAGE("    * Type "<<types[j]<<" : there is(are) "<<numberOfElements<<" element(s) : ");
      const int * number = getNumber(types[j]);
      SCRUTE(number);
      //      for (int k=0; k<numberOfElements;k++)
        //	MESSAGE("________________ " << number[k]);
    }
  } else
    MESSAGE("Is on all entities !");



};

FAMILY::FAMILY(const FAMILY & m):SUPPORT(m)
{
  MESSAGE("FAMILY::FAMILY(FAMILY & m)");
  _identifier = m._identifier;
  _numberOfAttribute = m._numberOfAttribute;

  if (_numberOfAttribute) {
    _attributeIdentifier.set(_numberOfAttribute,m._attributeIdentifier);
    _attributeValue.set(_numberOfAttribute,m._attributeValue);
  }

  //_attributeDescription.set(_numberOfAttribute);
  _attributeDescription.resize(_numberOfAttribute);
  for (int i=0;i<m._numberOfAttribute;i++)
    _attributeDescription[i] = m._attributeDescription[i];

  _numberOfGroup = m._numberOfGroup;

  //_groupName.set(_numberOfGroup) ;
  _groupName.resize(_numberOfGroup) ;
  for (int i=0;i<m._numberOfGroup;i++)
    _groupName[i]=m._groupName[i];
};

FAMILY::FAMILY(const SUPPORT & s):SUPPORT(s)
{
  MESSAGE("FAMILY::FAMILY(const SUPPORT & s)");

  _identifier = 0;
  _numberOfAttribute = 0;

  _numberOfGroup = 0;
};

FAMILY::~FAMILY() 
{
    MESSAGE("~FAMILY()");
};
  
FAMILY & FAMILY::operator=(const FAMILY &fam) 
{
    MESSAGE("FAMILY::operator=");
    if ( this == &fam ) return *this;

    //Etant donn� que l'op�rateur d'affectation de la classe SUPPORT effectuait
    //une recopie profonde j'ai mis en coh�rence l'op�rateur d'affectation
    // de la classe FAMILY
    SUPPORT::operator=(fam);
 

    _identifier = fam._identifier;
    _numberOfAttribute = fam._numberOfAttribute; 
    _attributeIdentifier.set(_numberOfAttribute, fam._attributeIdentifier) ;
    _attributeValue.set(_numberOfAttribute, fam._attributeValue) ;
    //_attributeDescription.set(_numberOfAttribute, fam._attributeDescription) ;
    _attributeDescription.clear();
    _attributeDescription = fam._attributeDescription;
    _numberOfGroup = fam._numberOfGroup;
    //_groupName.set(_numberOfGroup, fam._groupName) ;
    _groupName.clear();
    _groupName = fam._groupName;
    return *this;
};

ostream & MEDMEM::operator<<(ostream &os, FAMILY &myFamily)
{
  // how do cast without duplicate ?
  os << (SUPPORT) myFamily;

  os << "  - Identifier : "<<myFamily.getIdentifier()<<endl;
  int numberofattributes = myFamily.getNumberOfAttributes();
  os << "  - Attributes ("<<numberofattributes<<") :"<<endl;
  for (int j=1;j<numberofattributes+1;j++)
    os << "    * "<<myFamily.getAttributeIdentifier(j)<<" : "<<myFamily.getAttributeValue(j)<<", "<<myFamily.getAttributeDescription(j).c_str()<<endl;
  int numberofgroups = myFamily.getNumberOfGroups();
  os << "  - Groups ("<<numberofgroups<<") :"<<endl;
  for (int j=1;j<numberofgroups+1;j++)
    os << "    * "<<myFamily.getGroupName(j).c_str()<<endl ;

  return os;
};

ostream & MEDMEM::operator<<(ostream &os, const FAMILY &myFamily)
{
  // how do cast without duplicate ?
  os << (SUPPORT) myFamily;

  os << "  - Identifier : "<<myFamily.getIdentifier()<<endl;
  int numberofattributes = myFamily.getNumberOfAttributes();
  os << "  - Attributes ("<<numberofattributes<<") :"<<endl;
  for (int j=1;j<numberofattributes+1;j++)
    os << "    * "<<myFamily.getAttributeIdentifier(j)<<" : "<<myFamily.getAttributeValue(j)<<", "<<myFamily.getAttributeDescription(j).c_str()<<endl;
  int numberofgroups = myFamily.getNumberOfGroups();
  os << "  - Groups ("<<numberofgroups<<") :"<<endl;
  for (int j=1;j<numberofgroups+1;j++)
    os << "    * "<<myFamily.getGroupName(j).c_str()<<endl ;

  return os;
};

bool FAMILY::build(medEntityMesh Entity,int **FamilyNumber /* from MED file */)
{
  MESSAGE("FAMILY::build(medEntityMesh Entity,int **FamilyNumber /* from MED file */)");
  bool Find = false ;
  // Get types information from <_mesh>
  int    numberOfTypes             = _mesh->getNumberOfTypesWithPoly(Entity) ;
  medGeometryElement * types       = _mesh->getTypesWithPoly(Entity) ;
  
  int *  numberOfElementsInFamily         = new int[numberOfTypes] ;
  int    numberOfElementTypesInFamily     = 0 ;
  
  medGeometryElement * tmp_Types  = new medGeometryElement[numberOfTypes];
  int ** tmp_ElementsLists                = new int*[numberOfTypes] ;
  //  int *  GeometricTypeNumber           = new int[numberOfTypes] ;
  const int *  GlobalNumberingIndex          = _mesh->getGlobalNumberingIndex(Entity);
  

  SCRUTE(numberOfTypes);

  // we search for all elements in this family
  for (int TypeNumber=0; TypeNumber < numberOfTypes; TypeNumber++) {
    
    int NumberOfElements             = _mesh->getNumberOfElementsWithPoly(Entity,types[TypeNumber]) ;
    int NumberOfElementsInThisFamily = 0 ;
    int * ElementsOfThisFamilyNumber = FamilyNumber[TypeNumber];
    int * tmp_ElementsList           = new int[NumberOfElements];
      
    for (int i=0; i<NumberOfElements; i++)
      {
        //	SCRUTE(ElementsOfThisFamilyNumber[i]);
	if (_identifier == ElementsOfThisFamilyNumber[i]) {
	  tmp_ElementsList[NumberOfElementsInThisFamily]=i+GlobalNumberingIndex[TypeNumber] ;
	  NumberOfElementsInThisFamily++;
	}
      }
    
    if (NumberOfElementsInThisFamily>0) {// we have found some elements
      numberOfElementsInFamily[numberOfElementTypesInFamily]=NumberOfElementsInThisFamily;

      //int * ElementsList = tmp_ElementsList.resize(NumberOfElementsInThisFamily);
      int * ElementsList = new int[NumberOfElementsInThisFamily] ;
      memcpy(ElementsList,tmp_ElementsList,sizeof(int)*NumberOfElementsInThisFamily); // RESIZE de tmp_NodesList serait plus efficace !!!!!!!!
	
      tmp_ElementsLists[numberOfElementTypesInFamily]=ElementsList ;
      tmp_Types[numberOfElementTypesInFamily]=types[TypeNumber];
      //      GeometricTypeNumber[numberOfElementTypesInFamily]=TypeNumber+1;
      numberOfElementTypesInFamily++;
      //      delete [] ElementsList;
    }
    delete[] tmp_ElementsList;
  }
  
  // we define all attribut in SUPPORT :
  if (numberOfElementTypesInFamily>0) { // we have found elements
    Find = true ;
    _entity = Entity ;
    _numberOfGeometricType = numberOfElementTypesInFamily ;
    _geometricType.set(numberOfElementTypesInFamily) ;

    _isOnAllElts = false ;

    _numberOfElements.set(numberOfElementTypesInFamily) ;
    _totalNumberOfElements=0;

    //_numberOfGaussPoint = new int[numberOfElementTypesInFamily] ;

    //int * numberIndex = new int[numberOfElementTypesInFamily+1];
    //numberIndex[0]=1;
    for (int i=0; i<numberOfElementTypesInFamily; i++) {
      _geometricType[i]=tmp_Types[i] ;
      //numberIndex[i+1]=numberIndex[i]+numberOfElementsInFamily[i];
      _numberOfElements[i]=numberOfElementsInFamily[i]; // plutot un resize !!
      _totalNumberOfElements+=_numberOfElements[i];
      //_numberOfGaussPoint[i]=1;
    }
    //    delete[] numberOfElementsInFamily;
    //    delete[] tmp_Types;
    //    delete[] GeometricTypeNumber;
      
    // family on all ELEMENT ?
    if (_totalNumberOfElements == 
        _mesh->getNumberOfElementsWithPoly(Entity,MED_ALL_ELEMENTS)/* && Entity==MED_EN::MED_CELL*/) {
      _isOnAllElts = true ;
      // all others attributs are rights !
      for (int i=0; i<_numberOfGeometricType; i++)
	delete[] tmp_ElementsLists[i];
    } else {
      int *NumberValue = new int[_totalNumberOfElements];
      int *NumberIndex = new int[_numberOfGeometricType+1];
      NumberIndex[0]=1;
      for (int i=0; i<_numberOfGeometricType; i++) {
	NumberIndex[i+1]=NumberIndex[i]+_numberOfElements[i];
	for (int j=NumberIndex[i]; j<NumberIndex[i+1]; j++)
	  NumberValue[j-1]=tmp_ElementsLists[i][j-NumberIndex[i]];
	delete[] tmp_ElementsLists[i];
      }
      if(_number!=NULL) delete _number ;
      _number = new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfElements,NumberIndex,NumberValue);
      delete[] NumberIndex ;
      delete[] NumberValue ;
    }
  }
  delete[] tmp_Types;
  delete[] types;
  delete[] numberOfElementsInFamily;

  delete[] tmp_ElementsLists;

  return Find ;
}

