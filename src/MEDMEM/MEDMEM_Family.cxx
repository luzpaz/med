using namespace std;
/*
 File MEDMEM_Family.cxx
 $Header$
*/

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"

FAMILY::FAMILY():_identifier(0), _numberOfAttribute(0), 
                 _attributeIdentifier((int*)NULL),_attributeValue((int*)NULL), _attributeDescription((string*)NULL),
		 _numberOfGroup(0), _groupName((string*)NULL) 
{
    MESSAGE("FAMILY::FAMILY()");
};

FAMILY::FAMILY(MESH* Mesh, int Identifier, string Name, int NumberOfAttribute,
               int *AttributeIdentifier, int *AttributeValue, string AttributeDescription,
               int NumberOfGroup, string GroupName
	       ): SUPPORT(Mesh,Name),
		  _identifier(Identifier), 
		  _numberOfAttribute(NumberOfAttribute), 
		  _attributeIdentifier(AttributeIdentifier),
		  _attributeValue(AttributeValue),
		  _numberOfGroup(NumberOfGroup)
{
  MESSAGE("FAMILY(int Identifier, string Name, int NumberOfAttribute,int *AttributeIdentifier,int *AttributeValue,string AttributeDescription,int NumberOfGroup,string GroupName, int ** Number) : "<<Identifier);

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
  int * NodeFamilyNumber    = _mesh->getMEDArrayNodeFamily() ;   // EF : TEMPORAIRE !!
  int * tmp_NodesList       = new int[NumberOfNodes] ;           // Un peu brutal...., oui mais ce n'est qu'un tableau de travail !
  for (int i=0; i<NumberOfNodes; i++)
    if ( _identifier == NodeFamilyNumber[i] ) {
      tmp_NodesList[NumberOfNodesInFamily]=i+1 ;
      NumberOfNodesInFamily++;
    }
  
  // If we found nodes set the family attributes adequatly
  if (NumberOfNodesInFamily>0) {
    
    Find = true ;
    
    _entity = MED_NODE ;
    _numberOfGeometricType = 1 ;
    _geometricType = new medGeometryElement[1] ;
    _geometricType[0]=MED_NONE ;
    
    // family on all NODE
    if (NumberOfNodesInFamily==NumberOfNodes)
      _isOnAllElts = true ;
    else {
      _isOnAllElts= false ;
      _numberOfEntities = new int[1] ;
      _numberOfEntities[0]=NumberOfNodesInFamily ;
      _totalNumberOfEntities=NumberOfNodesInFamily;
      
      _number=new MEDSKYLINEARRAY(1,NumberOfNodesInFamily) ;
      int * NumberIndex = _number->getIndex();
      int * NumberValue = _number->getValue();
      NumberIndex[0]=1;                          //set the MEDSKYLINEARRAY Index table
      NumberIndex[1]=1+NumberOfNodesInFamily;    //set the MEDSKYLINEARRAY Index table
      for(int i=0; i<NumberOfNodesInFamily; i++) // OH LA LA... 
	NumberValue[i]=tmp_NodesList[i] ;        // RESIZE de tmp_NodesList serait plus efficace !!!!!!!!
      // PG : pas de problème, si ca marche. Il faudra déplacer le delete !
    }
  }
  delete[] tmp_NodesList ;
  


  // ************************ CELLS RELATED PART *************************************************
  // If we previously found nodes in our family don't scan the CELLS because a
  // family contains different geometic types of only one entity type.
  // ?? Scan every cell family number <NodeFamilyNumber[i]> in order to create 
  // ?? the list <tmp_NodesList> of node numbers <i+1> belonging to the family <_identifier>
  if (!Find) {

    // Get cell types information from <_mesh>
    int    numberOfCellTypes             = _mesh->getNumberOfTypes(MED_CELL) ;
    medGeometryElement * cellTypes       = _mesh->getTypes(MED_CELL) ;
  
    int *  numberOfCellsInFamily         = new int[numberOfCellTypes] ;
    int    numberOfCellTypesInFamily     = 0 ;
    
    medGeometryElement * tmp_CellsTypes  = new medGeometryElement[numberOfCellTypes];
    int ** tmp_CellsLists                = new int*[numberOfCellTypes] ;
    int *  GeometricTypeNumber           = new int[numberOfCellTypes] ;
    int ** CellFamilyNumber              = _mesh->getMEDArrayCellFamily();             // EF:  TEMPORAIRE
    int *  GlobalNumberingIndex          = _mesh->getGlobalNumberingIndex(MED_CELL);
    
    // we search for all cell in this family  
    for (int cellTypeNumber=0; cellTypeNumber < numberOfCellTypes; cellTypeNumber++) {
      
      int NumberOfCells             = _mesh->getNumberOfElements(MED_CELL,cellTypes[cellTypeNumber]) ;
      int NumberOfCellsInThisFamily = 0 ;
      int * CellsOfThisFamilyNumber = CellFamilyNumber[cellTypeNumber];
      int * tmp_CellsList           = new int[NumberOfCells];
      
      for (int i=0; i<NumberOfCells; i++)
	if (_identifier == CellsOfThisFamilyNumber[i]) {
	  tmp_CellsList[NumberOfCellsInThisFamily]=i+GlobalNumberingIndex[cellTypeNumber] ;
	  NumberOfCellsInThisFamily++;
	}
      
      if (NumberOfCellsInThisFamily>0) {// we have found some cells
	numberOfCellsInFamily[numberOfCellTypesInFamily]=NumberOfCellsInThisFamily;
	int * CellsList=new int[NumberOfCellsInThisFamily] ;
	
        for (int i=0;i<NumberOfCellsInThisFamily;i++)
	  CellsList[i]=tmp_CellsList[i] ;                         // RESIZE de tmp_NodesList serait plus efficace !!!!!!!!
	
        tmp_CellsLists[numberOfCellTypesInFamily]=CellsList ;
	tmp_CellsTypes[numberOfCellTypesInFamily]=cellTypes[cellTypeNumber];
	GeometricTypeNumber[numberOfCellTypesInFamily]=cellTypeNumber+1;
	numberOfCellTypesInFamily++;
      }
     
      delete[] tmp_CellsList ;
    }
    
    // we define all attribut in SUPPORT :
    if (numberOfCellTypesInFamily>0) { // we have found cells
      Find = true ;
      _entity = MED_CELL ;
      _numberOfGeometricType = numberOfCellTypesInFamily ;
      _geometricType = new medGeometryElement[numberOfCellTypesInFamily] ;
      _geometricTypeNumber = new int[numberOfCellTypesInFamily] ;
      _isOnAllElts = false ;
      _numberOfEntities = new int[numberOfCellTypesInFamily] ;
      _totalNumberOfEntities=0;
      for (int i=0; i<numberOfCellTypesInFamily; i++) {
	_geometricType[i]=tmp_CellsTypes[i] ;
	_geometricTypeNumber[i]=GeometricTypeNumber[i];
	_numberOfEntities[i]=numberOfCellsInFamily[i] ;
	_totalNumberOfEntities+=numberOfCellsInFamily[i] ;
      }
      delete[] numberOfCellsInFamily;
      delete[] tmp_CellsTypes;
      delete[] GeometricTypeNumber;
      
      // family on all CELL ?
      if (_totalNumberOfEntities==Mesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS)) {
	_isOnAllElts = true ;
	delete[] _numberOfEntities ;
	_numberOfEntities=(int*)NULL;
      } else {
	_number=new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfEntities);
	int *NumberValue=_number->getValue();
	int *NumberIndex=_number->getIndex();
	NumberIndex[0]=1;
	for (int i=0; i<_numberOfGeometricType; i++) {
	  NumberIndex[i+1]=NumberIndex[i]+_numberOfEntities[i];
	  for (int j=NumberIndex[i]; j<NumberIndex[i+1]; j++)
	    NumberValue[j-1]=tmp_CellsLists[i][j-NumberIndex[i]];
	  delete[] tmp_CellsLists[i];
	}
	delete[] tmp_CellsLists;
      }
    }
  }
  // on face ?
  if (!Find) 
    if ((_mesh->existConnectivity(MED_NODAL,MED_FACE))|(_mesh->existConnectivity(MED_DESCENDING,MED_FACE))) {
      int NumberOfFacesType = _mesh->getNumberOfTypes(MED_FACE) ;
      medGeometryElement * FacesType = _mesh->getTypes(MED_FACE) ;
      int * NumberOfFacesInFamily = new int[NumberOfFacesType] ;
      int NumberOfFacesTypeInFamily = 0 ;
      medGeometryElement * tmp_FacesTypes = new medGeometryElement[NumberOfFacesType];
      int ** tmp_FacesLists = new int*[NumberOfFacesType] ;
      int * GeometricTypeNumber = new int[NumberOfFacesType] ;
      int ** FaceFamilyNumber = _mesh->getMEDArrayFaceFamily();
      int * GlobalNumberingIndex = _mesh->getGlobalNumberingIndex(MED_FACE);
      // we search all face in this family
      for (int FaceTypeNumber=0; FaceTypeNumber<NumberOfFacesType; FaceTypeNumber++) {
	int NumberOfThisFaces = _mesh->getNumberOfElements(MED_FACE,FacesType[FaceTypeNumber]) ;
	int NumberOfFacesInThisFamily = 0 ;
	int * FaceOfThisFamilyNumber = FaceFamilyNumber[FaceTypeNumber];
	int * tmp_FacesList = new int[NumberOfThisFaces];
	for (int i=0; i<NumberOfThisFaces; i++)
	  if (_identifier == FaceOfThisFamilyNumber[i]) {
	    tmp_FacesList[NumberOfFacesInThisFamily]=i+GlobalNumberingIndex[FaceTypeNumber] ;
	    NumberOfFacesInThisFamily++;
	    
	    SCRUTE(i);
	    SCRUTE(NumberOfFacesInThisFamily);

	  }
	if (NumberOfFacesInThisFamily>0) {// we have found some faces
	  NumberOfFacesInFamily[NumberOfFacesTypeInFamily]=NumberOfFacesInThisFamily;
	  int * FacesList=new int[NumberOfFacesInThisFamily] ;
	  for (int i=0;i<NumberOfFacesInThisFamily;i++)
	    FacesList[i]=tmp_FacesList[i] ;
	  tmp_FacesLists[NumberOfFacesTypeInFamily]=FacesList ;
	  tmp_FacesTypes[NumberOfFacesTypeInFamily]=FacesType[FaceTypeNumber];
	  GeometricTypeNumber[NumberOfFacesTypeInFamily]=FaceTypeNumber+1;
	  NumberOfFacesTypeInFamily++;
	}
	delete[] tmp_FacesList ;
      }
      // we define all attribut in SUPPORT :
      if (NumberOfFacesTypeInFamily>0) { // we have found faces
	Find = true ;
	_entity = MED_FACE ;
	_numberOfGeometricType = NumberOfFacesTypeInFamily ;
	_geometricType = new medGeometryElement[NumberOfFacesTypeInFamily] ;
	_geometricTypeNumber = new int[NumberOfFacesTypeInFamily] ;
	_isOnAllElts = false ;
	_numberOfEntities = new int[NumberOfFacesTypeInFamily] ;
	_totalNumberOfEntities=0;
	for (int i=0; i<NumberOfFacesTypeInFamily; i++) {
	  _geometricType[i]=tmp_FacesTypes[i] ;
	  _geometricTypeNumber[i]=GeometricTypeNumber[i];
	  _numberOfEntities[i]=NumberOfFacesInFamily[i] ;
	  _totalNumberOfEntities+=NumberOfFacesInFamily[i] ;
	}
	delete[] NumberOfFacesInFamily;
	delete[] tmp_FacesTypes;
	delete[] GeometricTypeNumber;
      
	// family on all FACE ?

	// we suppose family is never on all face !!!!!!!!!
	// in all case, family is only on boundary and connectivity is partial

//  	if (_totalNumberOfEntities==Mesh->getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS)) {
//  	  _isOnAllElts = true ;
//  	  delete[] _numberOfEntities ;
//  	  _numberOfEntities=(int*)NULL;
//  	} else {
	  _number=new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfEntities);
	  int *NumberValue=_number->getValue();
	  int *NumberIndex=_number->getIndex();
	  NumberIndex[0]=1;
	  for (int i=0; i<_numberOfGeometricType; i++) {
	    NumberIndex[i+1]=NumberIndex[i]+_numberOfEntities[i];
	    for (int j=NumberIndex[i]; j<NumberIndex[i+1]; j++)
	      NumberValue[j-1]=tmp_FacesLists[i][j-NumberIndex[i]];
	    delete[] tmp_FacesLists[i];
	  }
	  delete[] tmp_FacesLists;
//  	}
      }
    }
  
  // on edge ?
  if (!Find) 
    if ((_mesh->existConnectivity(MED_NODAL,MED_EDGE))|(_mesh->existConnectivity(MED_DESCENDING,MED_EDGE))) {
      int NumberOfEdgesType = _mesh->getNumberOfTypes(MED_EDGE) ;
      medGeometryElement * EdgesType = _mesh->getTypes(MED_EDGE) ;
      int * NumberOfEdgesInFamily = new int[NumberOfEdgesType] ;
      int NumberOfEdgesTypeInFamily = 0 ;
      medGeometryElement * tmp_EdgesTypes = new medGeometryElement[NumberOfEdgesType];
      int ** tmp_EdgesLists = new int*[NumberOfEdgesType] ;
      int * GeometricTypeNumber = new int[NumberOfEdgesType] ;
      int ** EdgeFamilyNumber = _mesh->getMEDArrayEdgeFamily();
      int * GlobalNumberingIndex = _mesh->getGlobalNumberingIndex(MED_EDGE);
      // we search all edge in this family
      for (int EdgeTypeNumber=0; EdgeTypeNumber<NumberOfEdgesType; EdgeTypeNumber++) {
	int NumberOfThisEdges = _mesh->getNumberOfElements(MED_EDGE,EdgesType[EdgeTypeNumber]) ;
	int NumberOfEdgesInThisFamily = 0 ;
	int * EdgeOfThisFamilyNumber = EdgeFamilyNumber[EdgeTypeNumber];
	int * tmp_EdgesList = new int[NumberOfThisEdges];
	for (int i=0; i<NumberOfThisEdges; i++)
	  if (_identifier == EdgeOfThisFamilyNumber[i]) {
	    tmp_EdgesList[NumberOfEdgesInThisFamily]=i+GlobalNumberingIndex[EdgeTypeNumber] ;
	    NumberOfEdgesInThisFamily++;
	  }
	if (NumberOfEdgesInThisFamily>0) {// we have found some edges
	  NumberOfEdgesInFamily[NumberOfEdgesTypeInFamily]=NumberOfEdgesInThisFamily;
	  int * EdgesList=new int[NumberOfEdgesInThisFamily] ;
	  for (int i=0;i<NumberOfEdgesInThisFamily;i++)
	    EdgesList[i]=tmp_EdgesList[i] ;
	  tmp_EdgesLists[NumberOfEdgesTypeInFamily]=EdgesList ;
	  tmp_EdgesTypes[NumberOfEdgesTypeInFamily]=EdgesType[EdgeTypeNumber];
	  GeometricTypeNumber[NumberOfEdgesTypeInFamily]=EdgeTypeNumber+1;
	  NumberOfEdgesTypeInFamily++;
	}
	delete[] tmp_EdgesList ;
      }
      // we define all attribut in SUPPORT :
      if (NumberOfEdgesTypeInFamily>0) { // we have found edges
	Find = true ;
	_entity = MED_EDGE ;
	_numberOfGeometricType = NumberOfEdgesTypeInFamily ;
	_geometricType = new medGeometryElement[NumberOfEdgesTypeInFamily] ;
	_geometricTypeNumber = new int[NumberOfEdgesTypeInFamily] ;
	_isOnAllElts = false ;
	_numberOfEntities = new int[NumberOfEdgesTypeInFamily] ;
	_totalNumberOfEntities=0;
	for (int i=0; i<NumberOfEdgesTypeInFamily; i++) {
	  _geometricType[i]=tmp_EdgesTypes[i] ;
	  _geometricTypeNumber[i]=GeometricTypeNumber[i];
	  _numberOfEntities[i]=NumberOfEdgesInFamily[i] ;
	  _totalNumberOfEntities+=NumberOfEdgesInFamily[i] ;
	}
	delete[] NumberOfEdgesInFamily;
	delete[] tmp_EdgesTypes;
	delete[] GeometricTypeNumber;
      
	// family on all EDGE ?

	// we suppose family is never on all edge !!!!!!!!!
	// in all case, family is only on boundary and connectivity is partial

//  	if (_totalNumberOfEntities==Mesh->getNumberOfElements(MED_EDGE,MED_ALL_ELEMENTS)) {
//  	  _isOnAllElts = true ;
//  	  delete[] _numberOfEntities ;
//  	  _numberOfEntities=(int*)NULL;
//  	} else {
	  _number=new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfEntities);
	  int *NumberValue=_number->getValue();
	  int *NumberIndex=_number->getIndex();
	  NumberIndex[0]=1;
	  for (int i=0; i<_numberOfGeometricType; i++) {
	    NumberIndex[i+1]=NumberIndex[i]+_numberOfEntities[i];
	    for (int j=NumberIndex[i]; j<NumberIndex[i+1]; j++)
	      NumberValue[j-1]=tmp_EdgesLists[i][j-NumberIndex[i]];
	    delete[] tmp_EdgesLists[i];
	  }
	  delete[] tmp_EdgesLists;
//  	}
      }
    }
  // That's all !

  // if not find : no entity in familly !!!
  if (!Find) {
    _numberOfGeometricType = 0 ;
    _isOnAllElts = false ;
    MESSAGE ("FAMILY() : No entity found !") ;
  } else { // set gauss point number to be equal one !
    _numberOfGaussPoint = new int[_numberOfGeometricType] ;
    for (int i=0; i<_numberOfGeometricType; i++)
      _numberOfGaussPoint[i]=1 ;
  }
};

FAMILY::FAMILY(FAMILY & m):SUPPORT(m)
{
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
