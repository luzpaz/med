/*
 File MEDMEM_Family.cxx
 $Header$
*/

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"

using namespace std;
using namespace MEDMEM;

FAMILY::FAMILY():_identifier(0), _numberOfAttribute(0), 
                 _attributeIdentifier((int*)NULL),_attributeValue((int*)NULL), _attributeDescription((string*)NULL),
		 _numberOfGroup(0), _groupName((string*)NULL) 
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
  // replace them by pointerOf ?
  _attributeIdentifier = new int[_numberOfAttribute] ;
  memcpy(_attributeIdentifier,AttributeIdentifier,_numberOfAttribute*sizeof(int));
  _attributeValue = new int[_numberOfAttribute] ;
  memcpy(_attributeValue,AttributeValue,_numberOfAttribute*sizeof(int));

  _attributeDescription=new string[_numberOfAttribute];
  for (int i=0;i<NumberOfAttribute;i++) {
    _attributeDescription[i].assign(AttributeDescription,i*MED_TAILLE_DESC,MED_TAILLE_DESC);
    _attributeDescription[i].erase(strlen(_attributeDescription[i].c_str()));
    //SCRUTE(_attributeDescription[i]);
  }
 
  _groupName=new string[_numberOfGroup];
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
      if (_geometricType!=NULL) delete[] _geometricType ;
      _geometricType = new medGeometryElement[1] ;
      _geometricType[0]=MED_NONE ;
      _isOnAllElts= false ;
      if (_numberOfElements!=NULL) delete[] _numberOfElements ;
      _numberOfElements = new int[1] ;
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
      // PG : pas de problème, si ca marche. Il faudra déplacer le delete !
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
    if ((_mesh->existConnectivity(MED_NODAL,MED_FACE))|(_mesh->existConnectivity(MED_DESCENDING,MED_FACE))) {
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
  // already done by support !!!!
//    else { // set gauss point number to be equal one !
//      _numberOfGaussPoint = new int[_numberOfGeometricType] ;
//      for (int i=0; i<_numberOfGeometricType; i++)
//        _numberOfGaussPoint[i]=1 ;
//    }


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
      for (int k=0; k<numberOfElements;k++)
	MESSAGE("________________ " << number[k]);
    }
  } else
    MESSAGE("Is on all entities !");
  


};

FAMILY::FAMILY(const FAMILY & m):SUPPORT(m)
{
  MESSAGE("FAMILY::FAMILY(FAMILY & m)");
  _identifier = m._identifier;
  _numberOfAttribute = m._numberOfAttribute;
  if (m._attributeIdentifier != NULL)
    {
      _attributeIdentifier = new int[m._numberOfAttribute];
      memcpy(_attributeIdentifier,m._attributeIdentifier,m._numberOfAttribute*sizeof(int));
    }
  else
    _attributeIdentifier = (int *) NULL;
  if (m._attributeValue != NULL)
    {
      _attributeValue = new int[m._numberOfAttribute];
      memcpy(_attributeValue,m._attributeValue,m._numberOfAttribute*sizeof(int));
    }
  else
    _attributeValue = (int *) NULL;
  if (m._attributeDescription != NULL)
    {
      _attributeDescription = new string[m._numberOfAttribute];
      for (int i=0;i<m._numberOfAttribute;i++)
	_attributeDescription[i] = m._attributeDescription[i];
    }
  else
    _attributeDescription = (string *) NULL;
  _numberOfGroup = m._numberOfGroup;
  if (m._groupName != NULL)
    {
      _groupName = new string[m._numberOfGroup];
      for (int i=0;i<m._numberOfGroup;i++)
	_groupName[i]=m._groupName[i];
    }
  else
    _groupName = (string *) NULL;
};

FAMILY::FAMILY(const SUPPORT & s):SUPPORT(s)
{
  MESSAGE("FAMILY::FAMILY(const SUPPORT & s)");

  _identifier = 0;
  _numberOfAttribute = 0;
  _attributeIdentifier = (int*) NULL;
  _attributeValue = (int*) NULL;
  _attributeDescription = (string*) NULL;
  _numberOfGroup = 0;
  _groupName= (string*) NULL;
};

FAMILY::~FAMILY() 
{
    MESSAGE("~FAMILY()");
    if(_attributeIdentifier!=NULL)
       delete[] _attributeIdentifier;
    if(_attributeValue!=NULL)
       delete[] _attributeValue;
    if(_attributeDescription!=NULL)
       delete[] _attributeDescription;
    if(_groupName!=NULL)
       delete[] _groupName;
};
  
FAMILY & FAMILY::operator=(const FAMILY &fam) 
{
    MESSAGE("FAMILY::operator=");
    _identifier = fam._identifier;
    _numberOfAttribute = fam._numberOfAttribute; 
    _attributeIdentifier = fam._attributeIdentifier;
    _attributeValue = fam._attributeValue;
    _attributeDescription = fam._attributeDescription;
    _numberOfGroup = fam._numberOfGroup;
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
  int    numberOfTypes             = _mesh->getNumberOfTypes(Entity) ;
  const medGeometryElement * types       = _mesh->getTypes(Entity) ;
  
  int *  numberOfElementsInFamily         = new int[numberOfTypes] ;
  int    numberOfElementTypesInFamily     = 0 ;
  
  medGeometryElement * tmp_Types  = new medGeometryElement[numberOfTypes];
  int ** tmp_ElementsLists                = new int*[numberOfTypes] ;
  //  int *  GeometricTypeNumber           = new int[numberOfTypes] ;
  const int *  GlobalNumberingIndex          = _mesh->getGlobalNumberingIndex(Entity);
  

  SCRUTE(numberOfTypes);

  // we search for all elements in this family
  for (int TypeNumber=0; TypeNumber < numberOfTypes; TypeNumber++) {
    
    int NumberOfElements             = _mesh->getNumberOfElements(Entity,types[TypeNumber]) ;
    int NumberOfElementsInThisFamily = 0 ;
    int * ElementsOfThisFamilyNumber = FamilyNumber[TypeNumber];
    int * tmp_ElementsList           = new int[NumberOfElements];
      
    for (int i=0; i<NumberOfElements; i++)
      {
	SCRUTE(ElementsOfThisFamilyNumber[i]);
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
    if (_geometricType!=NULL) delete[] _geometricType ;
    _geometricType = new medGeometryElement[numberOfElementTypesInFamily] ;
    _isOnAllElts = false ;
    if (_numberOfElements!=NULL) delete[] _numberOfElements ;
    _numberOfElements = new int[numberOfElementTypesInFamily] ;
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
    if (_totalNumberOfElements == _mesh->getNumberOfElements(Entity,MED_ALL_ELEMENTS)) {
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
  delete[] numberOfElementsInFamily;

  delete[] tmp_ElementsLists;

  return Find ;
}

