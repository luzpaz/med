#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_CellModel.hxx"

#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ModulusArray.hxx"

#include "MEDMEM_STRING.hxx"

//------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY(medEntityMesh Entity=MED_CELL):
//------------------------------------------------------//
  _entity(Entity),
  _typeConnectivity(MED_NODAL),
  _numberOfTypes(0),
  _geometricTypes((medGeometryElement*)NULL),
  _type((CELLMODEL*)NULL),
  _entityDimension(0),
  _count((int*)NULL),
  _nodal((MEDSKYLINEARRAY*)NULL),
  _descending((MEDSKYLINEARRAY*)NULL),
  _reverseNodalConnectivity((MEDSKYLINEARRAY*)NULL),
  _reverseDescendingConnectivity((MEDSKYLINEARRAY*)NULL),
  _neighbourhood((MEDSKYLINEARRAY*)NULL),
  _constituent((CONNECTIVITY*)NULL)
{
  MESSAGE("CONNECTIVITY(medEntityMesh Entity=MED_CELL)") ;
}

//-------------------------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY(int numberOfTypes,medEntityMesh Entity=MED_CELL):
//-------------------------------------------------------------------------//
  _entity(Entity),
  _typeConnectivity(MED_NODAL),
  _numberOfTypes(numberOfTypes),
  _entityDimension(0),
  _nodal((MEDSKYLINEARRAY*)NULL),
  _descending((MEDSKYLINEARRAY*)NULL),
  _reverseNodalConnectivity((MEDSKYLINEARRAY*)NULL),
  _reverseDescendingConnectivity((MEDSKYLINEARRAY*)NULL),
  _neighbourhood((MEDSKYLINEARRAY*)NULL),
  _constituent((CONNECTIVITY*)NULL)
{
  MESSAGE("CONNECTIVITY(int numberOfTypes,medEntityMesh Entity=MED_CELL)") ;
  _geometricTypes = new medGeometryElement[numberOfTypes];
  _type = new CELLMODEL[numberOfTypes];
  _count = new int[numberOfTypes];
}

//----------------------------//
CONNECTIVITY::~CONNECTIVITY()
//----------------------------//
{
  MESSAGE("Destructeur de CONNECTIVITY()") ;
  if ( _geometricTypes != NULL )
    delete [] _geometricTypes ;
  if ( _count != NULL )
    delete[] _count ;
  if ( _nodal != NULL )
    delete _nodal ;
  if ( _descending != NULL )
    delete _descending ;
  if ( _reverseNodalConnectivity != NULL )
    delete _reverseNodalConnectivity ;
  if ( _reverseDescendingConnectivity != NULL )
    delete _reverseDescendingConnectivity ;
  if ( _constituent != NULL )
    delete _constituent ;
}

/*! A DOCUMENTER */
//------------------------------------------------------------------------------------------//
void CONNECTIVITY::calculateConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity)
//------------------------------------------------------------------------------------------//
{
  MESSAGE("CONNECTIVITY::calculateConnectivity");

  // a temporary limitation due to calculteDescendingConnectivity function !!!
  if ((_entityDimension==3) & (Entity==MED_EDGE))
    throw MEDEXCEPTION("CONNECTIVITY::calculateConnectivity : Could not build EDGE !");
  
  if (Entity==_entity)
    if (ConnectivityType==MED_NODAL)
      calculateNodalConnectivity() ;
    else 
      if (Entity==MED_CELL)
	calculateDescendingConnectivity() ;
      else
	throw MEDEXCEPTION("CONNECTIVITY::calculateConnectivity : Could not build DESCENDING !");
  if (Entity!=_entity) {
    calculateDescendingConnectivity() ;
    _constituent->calculateConnectivity(ConnectivityType,Entity) ;
  }
}

/*!  Give, in full or no interlace mode (for nodal connectivity),
     descending or nodal connectivity.

      You must give a <medEntityMesh> (ie:MED_EDGE) 
      and a <medGeometryElement> (ie:MED_SEG3).  */

//------------------------------------------------------------//
void CONNECTIVITY::updateFamily(vector<FAMILY*> myFamilies) 
//------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::updateFamily(vector<FAMILY*>) ";
  BEGIN_OF(LOC);

  int numberOfFamilies = myFamilies.size();
  if (numberOfFamilies == 0 ) {
    MESSAGE(LOC<<"No family") ;
    return ;
  }
  // does we do an update ?
  if ((_constituent != NULL)&(_descending != NULL)) {
    MESSAGE(LOC<<"Constituent is already defined") ;
    return ;
  }

  if ((_constituent != NULL)&(_descending == NULL)) {
    if (myFamilies[0]->getEntity() != _constituent->getEntity()) {
      MESSAGE(LOC<<"Family and constituent entity are different. We do nothing") ;
      return ;
    }

    // well we could go !
    CONNECTIVITY * oldConstituent = _constituent ;
    _constituent = (CONNECTIVITY *)NULL ;
    // for instance we must have nodal connectivity in constituent :
    if (oldConstituent->_nodal == NULL)
      {
	MESSAGE(LOC<<"We have no nodal connectivity of sub cell");
	throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have no nodal connectivity of sub cell")) ;
      }
    int oldNumberOfFace = oldConstituent->_nodal->getNumberOf() ;
    int * oldConstituentValue = oldConstituent->_nodal->getValue() ;
    int * oldConstituentIndex = oldConstituent->_nodal->getIndex() ;

    calculateDescendingConnectivity() ;

    //    if (oldConstituent->_nodal != NULL) {
    int newNumberOfFace = _constituent->_nodal->getNumberOf() ;
    int * newConstituentValue = _constituent->_nodal->getValue() ;
    int * newConstituentIndex = _constituent->_nodal->getIndex() ;
    
    int * newReverseDescendingIndex =
      _reverseDescendingConnectivity->getIndex();
    int * newReverseDescendingValue =
      _reverseDescendingConnectivity->getValue();
    
    int * newDescendingIndex = _descending->getIndex();
    int * newDescendingValue = _descending->getValue();

    // loop on all family,
    //   for all constituent in family, we get it's old connectivity 
    //   (with oldCconstituentValue and oldConstituentIndex)
    //   and search the constituent in newConstituentValue with class 
    //   ModulusArry
    //
    //   when a new face is found, replace old constituent 
    //   number in family with new one
    //   If face is not rigth oriented, we must change _descending attribute 
    //   and _reverseDescendingConnectivity (see calculateDescendingConnectivity()).

    // Voila a toi de jouer Nadir :-)

    // First we get the renumbering from the oldCconstituentValue and
    // oldConstituentIndex in the the new one, newConstituentValue and
    // newConstituentIndex with a negative sign if the face is not
    // right orented

    int * renumberingFromOldToNew = new int [oldNumberOfFace];
    int index1 = 0;
    int indexm1 = 0;

    for (int iOldFace=0;iOldFace<oldNumberOfFace;iOldFace++)
      {
	int index = 0;
	
//  	  renumberingFromOldToNew[iOldFace] = 999999;

	int face_it_beginOld = oldConstituentIndex[iOldFace];
	int face_it_endOld = oldConstituentIndex[iOldFace+1];
	int face_size_itOld = face_it_endOld - face_it_beginOld;
	int face_size_itNew;
	
	MEDMODULUSARRAY modulusArrayOld(face_size_itOld,oldConstituentValue+face_it_beginOld-1);
	
	for (int iNewFace=0;iNewFace<newNumberOfFace && index == 0;
	     iNewFace++)
	  {
	    int face_it_beginNew = newConstituentIndex[iNewFace];
	    int face_it_endNew = newConstituentIndex[iNewFace+1];
	    face_size_itNew = face_it_endNew - face_it_beginNew;
	    
	    if (face_size_itNew == face_size_itOld)
	      {
		MEDMODULUSARRAY modulusArrayNew(face_size_itNew,newConstituentValue+face_it_beginNew-1);
		
		int retCompareNewOld = modulusArrayNew.compare(modulusArrayOld);

		// Real new face found
		
		if(retCompareNewOld == 1)
		  {
		    renumberingFromOldToNew[iOldFace] = iNewFace+1;
		    index = 1;
		    index1++;
		  }
		
		// Reverse new face found
		
		if(retCompareNewOld == -1)
		  {
		    renumberingFromOldToNew[iOldFace] = iNewFace+1;
		    index = 1;
		    indexm1++;
		    
		    int face_it_begin = newReverseDescendingIndex[iNewFace];
		    int face_it_end = newReverseDescendingIndex[iNewFace+1];
		    int face_size_it = face_it_end - face_it_begin;
		    
		    if (face_size_it == 1)
		      throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This is a boundary face/edge, it should not be renumbered and it is wrongly oriented."));
		    
		    if (face_size_it > 2)
		      throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This face/edge should not be a (d-1) cell because it has "<<face_size_it<<" neighbouring"));
		    
		    // we have always 2 neighbourings
		    int cell1 = newReverseDescendingValue[face_it_begin-1];
		    int cell2 = newReverseDescendingValue[face_it_begin];
		    
		    // PROVISOIRE : en attendant que le SKYLINEARRAY de ReverseDescending soit correct (sans le zero)
//  		    if (cell2 == 0)
//  		      throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This is a boundary face/edge, it should not be renumbered and it is wrongly oriented."));

  		    if (cell2 != 0) { // we are not on border !!!!
		    
		      newReverseDescendingValue[face_it_begin-1] = cell2;
		      // Updating _constituent->_nodal because of reversity
		      int * oldArray = oldConstituentValue+face_it_beginOld-1;
		      int * newArray = newConstituentValue+face_it_beginNew-1;
		      for(int iarray=0;iarray<face_size_itNew;iarray++)
			newArray[iarray] = oldArray[iarray] ;
		      
		      
		      // Updating _reverseDescendingConnectivity
		    

		      newReverseDescendingValue[face_it_begin] = cell1;
		    
		      // Updating _descending for cell1 and cell2
		      for(int iface=newDescendingIndex[cell1-1];iface<newDescendingIndex[cell1];iface++)
			if (newDescendingValue[iface-1]==iNewFace+1)
			  newDescendingValue[iface-1]=-iNewFace-1 ;
			else if (newDescendingValue[iface-1]==-iNewFace-1)
			  newDescendingValue[iface-1]=iNewFace+1 ;
		    
		      for(int iface=newDescendingIndex[cell2-1];iface<newDescendingIndex[cell2];iface++)
			if (newDescendingValue[iface-1]==iNewFace+1)
			  newDescendingValue[iface-1]=-iNewFace-1 ;
			else if (newDescendingValue[iface-1]==-iNewFace-1)
			  newDescendingValue[iface-1]=iNewFace+1 ;
		    } else {// else we are on border and we do nothing !!!!!!!!
		      INFOS("WARNING,WARNING,WARNING,WARNING,WARNING,WARNING");
		      INFOS(LOC<<" Boudary FACE "<<iOldFace+1<<" are wrong oriented !") ;
		      INFOS("WARNING,WARNING,WARNING,WARNING,WARNING,WARNING");
		  }
		  }
	      }
	  }
	
	if(index == 0)
	  {
	    INFOS(LOC<<"Renumbering problem with the Face connectivity given by the User and the new Connectivity computed");
	    throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have a Face connectivity problem"));
	  }
      }
    
    MESSAGE(LOC<<"The Renumbering is finished and the status is");
    SCRUTE(index1);
    SCRUTE(indexm1);

    // Updating the Family
    
    for(int i=0; i<numberOfFamilies; i++) {
      FAMILY * myFamily = myFamilies[i] ;
      
      int length_skyline = myFamily->getnumber()->getLength();
      int * value_skyline = myFamily->getnumber()->getValue();
      
      for (int i=0;i<length_skyline;i++)
	value_skyline[i] = renumberingFromOldToNew[value_skyline[i]-1];
    }
  }
  
  END_OF(LOC);
  return ;
}

//------------------------------------------------------------------------------------------------------------------//
med_int * CONNECTIVITY::getConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type) 
//------------------------------------------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getConnectivity" ;
  BEGIN_OF(LOC);

  MEDSKYLINEARRAY * Connectivity ;
  if (Entity==_entity) {
    
    if (ConnectivityType==MED_NODAL)
      Connectivity=_nodal;
    else
      Connectivity=_descending;
    
    if (Connectivity!=NULL)
      if (Type==MED_ALL_ELEMENTS)
	return Connectivity->getValue() ;
      else {
	for (med_int i=0; i<_numberOfTypes; i++)
	  if (_geometricTypes[i]==Type)
	    return Connectivity->getI(_count[i]) ;
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Type not found !")) ;
      }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !")) ;
  } else 
    if (_constituent != NULL)
      return _constituent->getConnectivity(ConnectivityType,Entity,Type);
  
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !")) ;
}  

/*!  Give morse index array to use with
     getConnectivity(MED_FULL_INTERLACE,mode,entity,MED_ALL_ELEMENTS).

      Each value give start index for corresponding entity in connectivity array.

      Example : i-th element, j-th node of it :
      - In C mode : Connectivity[ConnectivityIndex[i]-1+j-1]
      - In fortran mode : Connectivity[ConnectivityIndex[i]+j] */
//-----------------------------------------------------------------------------------------------//
med_int * CONNECTIVITY::getConnectivityIndex(medConnectivity ConnectivityType, medEntityMesh Entity) 
//------------------------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getConnectivityIndex" ;
  BEGIN_OF(LOC);

  MEDSKYLINEARRAY * Connectivity ;
  if (Entity==_entity) {
    
    if (ConnectivityType==MED_NODAL)
      Connectivity=_nodal;
    else
      Connectivity=_descending;
    
    if (Connectivity!=NULL)
      return Connectivity->getIndex() ;
    else 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !")) ;

  } else 
    if (_constituent != NULL)
      return _constituent->getConnectivityIndex(ConnectivityType,Entity);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !")) ;
}

/*! A DOCUMENTER */
//--------------------------------------------------------------//
CELLMODEL & CONNECTIVITY::getType(medGeometryElement Type) const
//--------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getType" ;
  BEGIN_OF(LOC);

  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE !")) ;
  for (med_int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i] ;
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !")) ;
}

/*!  Returns the number of elements of type <med geometrie element>.
     Note : not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
med_int CONNECTIVITY::getNumberOfNodesInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getNumberOfNodesInType" ;
  BEGIN_OF(LOC);

  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!")) ;
  for (med_int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i].getNumberOfNodes() ;
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !")) ;
}

/*!  Returns the number of geometric sub cells of <med geometrie element> type.
not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
med_int CONNECTIVITY::getNumberOfSubCellInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION("CONNECTIVITY::getNumberOfSubCell : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!") ;
  for (med_int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i].getNumberOfConstituents(1) ;
  throw MEDEXCEPTION("CONNECTIVITY::getNumberOfSubCell : medGeometryElement not found !") ;
}

/*!  Returns the number of elements of type <med geometrie element>.

     Note :
      - Implemented for MED_ALL_ELEMENTS
      - Not implemented for MED_ALL_ENTITIES (A VERIFIER)
      - Not implemented for MED_NONE */
//-----------------------------------------------------------------------------------//
med_int CONNECTIVITY::getNumberOf(medEntityMesh Entity, medGeometryElement Type) const
//-----------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getNumberOf" ;
  BEGIN_OF(LOC);

  MESSAGE(LOC<<" Entity = "<< Entity << ", _entity = "<<_entity);

  if (Entity==_entity) {
    if (Type==MED_NONE)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_NONE"));
    if (Type==MED_ALL_ELEMENTS)
      return _count[_numberOfTypes]-1;
    for (med_int i=0; i<_numberOfTypes; i++)
      if (_geometricTypes[i]==Type)
	return (_count[i+1] - _count[i]);
    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
  } else 
    if (_constituent != NULL)
      return _constituent->getNumberOf(Entity,Type);

  return 0 ; // valid if they are nothing !
  //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !")) ;
}

/*! A DOCUMENTER */
//--------------------------------------------------------------//
med_int* CONNECTIVITY::getValue(medConnectivity TypeConnectivity, 
				medGeometryElement Type)
//--------------------------------------------------------------//
{
  if (TypeConnectivity == MED_NODAL) 
    {
	  calculateNodalConnectivity();
	  if (Type==MED_ALL_ELEMENTS)
	    return _nodal->getValue();
	  for (med_int i=0; i<_numberOfTypes; i++)
	    if (_geometricTypes[i]==Type)
	      return _nodal->getI(_count[i]);
    } 
  else 
    {
      calculateDescendingConnectivity();
      if (Type==MED_ALL_ELEMENTS)
	return _descending->getValue();
      for (med_int i=0; i<_numberOfTypes; i++)
	if (_geometricTypes[i]==Type)
	  return _descending->getI(Type);
    }
  throw MEDEXCEPTION("Not found");
}

/*! A DOCUMENTER */
//---------------------------------------------------------------------//
med_int* CONNECTIVITY:: getValueIndex(medConnectivity TypeConnectivity) 
//---------------------------------------------------------------------//
{
  if (TypeConnectivity == MED_NODAL) 
    {
      calculateNodalConnectivity();
      return _nodal->getIndex();
    }
  else 
    {
      calculateDescendingConnectivity();
      return _descending->getIndex();
    }
}

/*! Not yet implemented */
//----------------------------------------------//
med_int* CONNECTIVITY:: getNeighbourhood() const
//----------------------------------------------//
{
  throw MEDEXCEPTION("CONNECTIVITY::getNeighbourhood : Not implemented");
}

/*! Returns an array which contains, for each node, all cells
    arround it. */
//-------------------------------------------------//
med_int* CONNECTIVITY::getReverseNodalConnectivity() 
//-------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getValue();
}

/*!  Give index array to use with getReverseConnectivity(MED_NODAL).
     It is unusefull with MED_DESCENDING mode, because we have allways two cells.  */
//-------------------------------------------------------//
med_int* CONNECTIVITY::getReverseNodalConnectivityIndex() 
//-------------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getIndex();
}

/*! Returns an array which contains, for each face (or edge),
    the 2 cells of each side. First is cell which face normal is outgoing.
    arround it. */
//------------------------------------------------------//
med_int* CONNECTIVITY::getReverseDescendingConnectivity() 
//------------------------------------------------------//
{
  // it is in _constituent connectivity only if we are in MED_CELL
  if (_entity==MED_CELL) {
    // we want descending connectivity 
    calculateDescendingConnectivity();
    return _reverseDescendingConnectivity->getValue();
  }
  throw MEDEXCEPTION("CONNECTIVITY::getReverseDescendingConnectivity : Error Only in MED_CELL connectivity");
}

/*! calculate the reverse descending Connectivity 
    and returns the index  ( A DOCUMENTER MIEUX)*/
//-----------------------------------------------------------//
med_int* CONNECTIVITY::getReverseDescendingConnectivityIndex() 
//-----------------------------------------------------------//
{
  // it is in _constituent connectivity only if we are in MED_CELL
  if (_entity==MED_CELL) {
    // we want descending connectivity 
    calculateDescendingConnectivity();
    return _reverseDescendingConnectivity->getIndex();
  }
  throw MEDEXCEPTION("CONNECTIVITY::getReverseDescendingConnectivityIndex : Error Only in MED_CELL connectivity");
}

/*! A DOCUMENTER (et a finir ???) */
//--------------------------------------------//
void CONNECTIVITY::calculateNodalConnectivity()
//--------------------------------------------//
{
  if (_nodal==NULL) 
    {
      if (_descending==NULL)
	throw MEDEXCEPTION("CONNECTIVITY::calculateNodalConnectivity : No connectivity found !");
      // calculate _nodal from _descending
    }
}

/*! If not yet done, calculate the nodal Connectivity 
    and the reverse nodal Connectivity*/
//---------------------------------------------------//
void CONNECTIVITY::calculateReverseNodalConnectivity()
//---------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::calculateReverseNodalConnectivity : " ;
  BEGIN_OF(LOC) ;

  if (_nodal==NULL) 
    calculateNodalConnectivity() ;
  
  MESSAGE(LOC<<"Number of nodes = "<<_numberOfNodes);

  if(_reverseNodalConnectivity==NULL) {

    med_int node_number = 0;
    vector <vector <med_int> > reverse_connectivity; 
    reverse_connectivity.resize(_numberOfNodes+1);
  
  // Treat all cells types
  
    for (med_int j = 0; j < _numberOfTypes; j++)
      {
	// node number of the cell type
	node_number = _type[j].getNumberOfNodes();
	// treat all cells of a particular type
	for (med_int k = _count[j]; k < _count[j+1]; k++)
	  // treat all nodes of the cell type
	  for (med_int local_node_number = 1 ; local_node_number < node_number+1; local_node_number++)
	    {
	      med_int global_node = _nodal->getIJ(k,local_node_number) ;
	      reverse_connectivity[global_node].push_back(k);
	    }
      }
  
    // Full reverse_nodal_connectivity and reverse_nodal_connectivity_index

  //calculate size of reverse_nodal_connectivity array
    med_int size_reverse_nodal_connectivity  = 0;
    for (med_int i = 1; i < _numberOfNodes+1; i++)
      size_reverse_nodal_connectivity += reverse_connectivity[i].size();
  
    MEDSKYLINEARRAY * ReverseConnectivity = new MEDSKYLINEARRAY(_numberOfNodes,size_reverse_nodal_connectivity) ;
    //  reverse_nodal_connectivity_index = new (med_int)[_numberOfNodes+1];
    //  reverse_nodal_connectivity = new (med_int)[size_reverse_nodal_connectivity];
    med_int * reverse_nodal_connectivity = ReverseConnectivity->getValue() ;
    med_int * reverse_nodal_connectivity_index = ReverseConnectivity->getIndex() ;

    reverse_nodal_connectivity_index[0] = 1;
    for (med_int i = 1; i < _numberOfNodes+1; i++)
      {
	med_int size = reverse_connectivity[i].size(); 
	reverse_nodal_connectivity_index[i] = reverse_nodal_connectivity_index[i-1] + size ;
	for (med_int j = 0; j < size; j++)
	  reverse_nodal_connectivity[reverse_nodal_connectivity_index[i-1]-1+j]= reverse_connectivity[i][j];
      }
  
    _reverseNodalConnectivity = ReverseConnectivity ;

  }
}

/*! If not yet done, calculate the Descending Connectivity */
//-------------------------------------------------//
void CONNECTIVITY::calculateDescendingConnectivity()
//-------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::calculateDescendingConnectivity() : ";
  BEGIN_OF(LOC);
  
  if (_descending==NULL) {
    if (_nodal==NULL) {
      MESSAGE(LOC<<"No connectivity found !");
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity found !"));
    }
    // calcul _descending from _nodal
    // we need CONNECTIVITY for constituent

    if (_constituent != NULL) 
    //      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"ERROR : No descending connectivity defined, but constituent exist !"));
      MESSAGE(LOC<<": No descending connectivity defined, but constituent exist !");

    if (_entityDimension == 3)
      _constituent = new CONNECTIVITY(MED_FACE) ;
    else if (_entityDimension == 2)
      _constituent = new CONNECTIVITY(MED_EDGE) ;
    else {
      MESSAGE(LOC<<"We are in 1D");
      return;
    }
    _constituent->_typeConnectivity = MED_DESCENDING ;
    _constituent->_numberOfNodes = _numberOfNodes ;
    // foreach cells, we built array of constituent
    int DescendingSize = 0 ;
    for(int i=0; i<_numberOfTypes; i++) 
      DescendingSize+=(_count[i+1]-_count[i])*_type[i].getNumberOfConstituents(1) ;
    _descending = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,DescendingSize) ;
    int * descend_connectivity = _descending->getValue() ;
    for (int i=0; i<DescendingSize; i++)
      descend_connectivity[i]=0;
    int * descend_connectivity_index = _descending->getIndex() ;
    descend_connectivity_index[0]=1;
    medGeometryElement* ConstituentsTypes = new medGeometryElement[2];
    ConstituentsTypes[0]=MED_NONE ;
    ConstituentsTypes[1]=MED_NONE ;
    int * NumberOfConstituentsForeachType = new int[2];
    NumberOfConstituentsForeachType[0]=0;
    NumberOfConstituentsForeachType[1]=0;
    for(int i=0; i<_numberOfTypes; i++) {
      // initialize descend_connectivity_index array :
      int NumberOfConstituents = _type[i].getNumberOfConstituents(1) ;
      for (int j=_count[i];j<_count[i+1];j++){
	descend_connectivity_index[j]=descend_connectivity_index[j-1]+NumberOfConstituents ;
	// compute number of constituent of all cell for each type
	for(int k=1;k<NumberOfConstituents+1;k++) {
	  medGeometryElement MEDType = _type[i].getConstituentType(1,k) ;
	  if (ConstituentsTypes[0]==MED_NONE) {
	    ConstituentsTypes[0]=MEDType;
	    NumberOfConstituentsForeachType[0]++ ;
	  } else if (ConstituentsTypes[0]==MEDType)
	    NumberOfConstituentsForeachType[0]++ ;
	  else if (ConstituentsTypes[1]==MED_NONE) {
	    ConstituentsTypes[1]=MEDType;
	    NumberOfConstituentsForeachType[1]++ ;
	  } else if (ConstituentsTypes[1]==MEDType)
	    NumberOfConstituentsForeachType[1]++ ;
	  else
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<MEDType<<" is different of "<<ConstituentsTypes[0]<<" and "<<ConstituentsTypes[1]));
	}
      }
    }

    // we could built _constituent !
    int TotalNumberOfConstituents = NumberOfConstituentsForeachType[0]+NumberOfConstituentsForeachType[1] ;
    int TotalNumberOfNodes = NumberOfConstituentsForeachType[0]*(ConstituentsTypes[0]%100)+NumberOfConstituentsForeachType[1]*(ConstituentsTypes[1]%100) ;
    _constituent->_nodal = new MEDSKYLINEARRAY(TotalNumberOfConstituents,TotalNumberOfNodes) ;

    // we use _constituent->_nodal 
    int * ConstituentNodalConnectivity = _constituent->_nodal->getValue();
    int * ConstituentNodalConnectivityIndex = _constituent->_nodal->getIndex();
    ConstituentNodalConnectivityIndex[0]=1;

    _constituent->_entityDimension=ConstituentsTypes[0]/100;
    if (ConstituentsTypes[1]==MED_NONE)
      _constituent->_numberOfTypes = 1 ;
    else
      _constituent->_numberOfTypes = 2 ;
    _constituent->_geometricTypes = new medGeometryElement[_constituent->_numberOfTypes] ;
    _constituent->_type = new CELLMODEL[_constituent->_numberOfTypes] ;
    _constituent->_count = new med_int[_constituent->_numberOfTypes+1] ;
    _constituent->_count[0]=1;
    int* tmp_NumberOfConstituentsForeachType = new med_int[_constituent->_numberOfTypes+1] ;
    tmp_NumberOfConstituentsForeachType[0]=0; // to count constituent of each type
    for (int i=0; i<_constituent->_numberOfTypes;i++) {
      _constituent->_geometricTypes[i]=ConstituentsTypes[i] ;
      _constituent->_count[i+1]=_constituent->_count[i]+NumberOfConstituentsForeachType[i] ;
      CELLMODEL Type(ConstituentsTypes[i]);
      _constituent->_type[i]=Type;
      tmp_NumberOfConstituentsForeachType[i+1]=NumberOfConstituentsForeachType[i] ;
      for (int j=tmp_NumberOfConstituentsForeachType[i]; j<tmp_NumberOfConstituentsForeachType[i+1]+tmp_NumberOfConstituentsForeachType[i]; j++)
	ConstituentNodalConnectivityIndex[j+1]=ConstituentNodalConnectivityIndex[j]+(ConstituentsTypes[i]%100) ;
    }
    delete[] ConstituentsTypes;
    delete[] NumberOfConstituentsForeachType ;
    
    // we need reverse nodal connectivity
    if (! _reverseNodalConnectivity)
      calculateReverseNodalConnectivity() ;
    int * ReverseNodalConnectivityIndex = _reverseNodalConnectivity->getIndex();
    int * ReverseNodalConnectivityValue = _reverseNodalConnectivity->getValue();

    // array to keep reverse descending connectivity
    int * ReverseDescendingConnectivityValue = new int[TotalNumberOfConstituents*2];
      
    int TotalNumberOfSubCell = 0 ;
    for (int i=0; i<_numberOfTypes; i++) { // loop on all cell type
      CELLMODEL Type = _type[i] ;
      int NumberOfNodesPerCell = Type.getNumberOfNodes() ;
      int NumberOfConstituentPerCell = Type.getNumberOfConstituents(1);
      for (int j=_count[i]; j<_count[i+1]; j++) // we loop on all cell of this type
	for (int k=1 ; k<=NumberOfConstituentPerCell; k++) { // we loop on all sub cell of it
	  if (descend_connectivity[descend_connectivity_index[j-1]+k-2]==0) {
	    // it is a new sub cell !
	    //	    TotalNumberOfSubCell++;
	    // Which type ?
	    if (Type.getConstituentType(1,k)==_constituent->_geometricTypes[0]){
	      tmp_NumberOfConstituentsForeachType[0]++;
	      TotalNumberOfSubCell=tmp_NumberOfConstituentsForeachType[0];
	    } else {
	      tmp_NumberOfConstituentsForeachType[1]++;
	      TotalNumberOfSubCell=tmp_NumberOfConstituentsForeachType[1];
	    }
	    //we have maximum two types

	    descend_connectivity[descend_connectivity_index[j-1]+k-2]=TotalNumberOfSubCell;
	    ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2]=j ;
	    int NumberOfNodesPerConstituent = Type.getConstituentType(1,k)%100 ;
	    
	    int * NodesLists = new int[NumberOfNodesPerConstituent] ;
	    for (int l=0; l<NumberOfNodesPerConstituent; l++) {
	      NodesLists[l]=_nodal->getIJ(j,Type.getNodeConstituent(1,k,l+1));
	      ConstituentNodalConnectivity[ConstituentNodalConnectivityIndex[TotalNumberOfSubCell-1]-1+l]=NodesLists[l];
	    }
	    // we use reverse_nodal_connectivity to find the other element which contain this sub cell

	    // all elements which contains first node of sub cell :
	    int ReverseNodalConnectivityIndex_0 = ReverseNodalConnectivityIndex[NodesLists[0]-1] ;
	    int ReverseNodalConnectivityIndex_1 = ReverseNodalConnectivityIndex[NodesLists[0]] ;
	    int NumberOfCellsInList = ReverseNodalConnectivityIndex_1-ReverseNodalConnectivityIndex_0 ;
	    int * CellsList = new int[NumberOfCellsInList] ;
	    for (int l=ReverseNodalConnectivityIndex_0; l<ReverseNodalConnectivityIndex_1; l++)
	      CellsList[l-ReverseNodalConnectivityIndex_0]=ReverseNodalConnectivityValue[l-1] ;
	    
	    // foreach node in sub cell, we search elements which are in common
	    // at the end, we must have only one !

	    for (int l=1; l<NumberOfNodesPerConstituent; l++) {
	      int NewNumberOfCellsInList = 0 ;
	      int * NewCellsList = new int[NumberOfCellsInList] ;
	      for (int l1=0; l1<NumberOfCellsInList; l1++)
		for (int l2=ReverseNodalConnectivityIndex[NodesLists[l]-1]; l2<ReverseNodalConnectivityIndex[NodesLists[l]]; l2++) {
		  if (CellsList[l1]<ReverseNodalConnectivityValue[l2-1])
		    // increasing order : CellsList[l1] are not in elements list of node l
		    break ;
		  if ((CellsList[l1]==ReverseNodalConnectivityValue[l2-1])&(CellsList[l1]!=j)) {
		    // we have found one
		    NewCellsList[NewNumberOfCellsInList]=CellsList[l1];
		    NewNumberOfCellsInList++;
		    break;
		  }
		}
	      NumberOfCellsInList = NewNumberOfCellsInList;
	      delete [] CellsList ;
	      CellsList = NewCellsList;
	    }
	    
	    int CellNumber = CellsList[0] ;
	    delete [] CellsList ;
	    if (NumberOfCellsInList>1)
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"More than one other Cell ("<<NumberOfCellsInList<<") !"));
	    

//  	    int cell_number_1=-1 ;
//  	    int cell_number_2=-1 ;
//  	    int cell_number_3=-1 ;
//  	    bool find = false ;
//  	    for (int l=ReverseNodalConnectivityIndex[NodesLists[0]-1]; l<ReverseNodalConnectivityIndex[NodesLists[0]]; l++) { // first node
//  	      cell_number_1 = ReverseNodalConnectivityValue[l-1] ;
//  	      if (cell_number_1 != j)
//  		for (int m=ReverseNodalConnectivityIndex[NodesLists[1]-1]; m<ReverseNodalConnectivityIndex[NodesLists[1]]; m++) { //second node
//  		  cell_number_2 = ReverseNodalConnectivityValue[m-1] ;
//  		  if ((cell_number_2 != j) && (cell_number_2 == cell_number_1))
//  		    for (int n=ReverseNodalConnectivityIndex[NodesLists[2]-1]; n<ReverseNodalConnectivityIndex[NodesLists[2]]; n++) { //third node
//  		      cell_number_3 = ReverseNodalConnectivityValue[n-1] ;
//  		      if ((cell_number_3 != j) && (cell_number_3 == cell_number_1)) { // we found element which have three node in it
//  			find = true ;
//  			break ;
//  		      }
//  		      if (find)
//  			break ;
//  		    }
//  		  if (find)
//  		    break ;
//  		}
//  	      if (find)
//  		break ;
//  	    }


//  	    if (find) {
	    if (NumberOfCellsInList==1) {
	      ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2+1]=CellNumber ;
	      // we search sub cell number in this cell to not calculate it another time
	      // which type ?
	      CELLMODEL Type2 ;
	      for (int l=0; l<_numberOfTypes; l++)
		if (CellNumber < _count[l+1]) {
		  Type2=_type[l] ;
		  break ;
		}
	      //int sub_cell_count2 = Type2.get_entities_count(1) ;
	      //int nodes_cell_count2 = Type2.get_nodes_count() ;
	      bool find2 = false ;
	      for (int l=1; l<=Type2.getNumberOfConstituents(1) ;l++) { // on all sub cell
		int counter = 0 ;
		for (int m=1; m<=Type2.getConstituentType(1,l)%100; m++)
		  for (int n=1; n<=Type.getConstituentType(1,k)%100; n++) { 
		    if (_nodal->getIJ(CellNumber,Type2.getNodeConstituent(1,l,m)) == NodesLists[n-1])
		      counter++ ;
		  }
		if (counter==Type.getConstituentType(1,k)%100) {
		  descend_connectivity[descend_connectivity_index[CellNumber-1]+l-2]=-1*TotalNumberOfSubCell; // because, see it in other side !
		  find2 = true ;
		}
		if (find2)
		  break ;
	      }
	      if (!find2)
		INFOS(LOC<<"ERROR ERROR ERROR ERROR ERROR : we find any subcell !!!") ; // exception ?
  	    } else
	      ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2+1]=0 ;

	    delete[] NodesLists ;
	  }
	}
    }
    // we adjust _constituent
    int NumberOfConstituent=0 ;
    int SizeOfConstituentNodal=0 ;
    for (int i=0;i<_constituent->_numberOfTypes; i++) {
      NumberOfConstituent += tmp_NumberOfConstituentsForeachType[i]-_constituent->_count[i]+1;
      SizeOfConstituentNodal += (tmp_NumberOfConstituentsForeachType[i]-_constituent->_count[i]+1)*_constituent->_type[i].getNumberOfNodes();
    }
    // we built new _nodal attribute in _constituent
    MEDSKYLINEARRAY *ConstituentNodal = new MEDSKYLINEARRAY(NumberOfConstituent,SizeOfConstituentNodal);
    int *ConstituentNodalValue = ConstituentNodal->getValue();
    int *ConstituentNodalIndex = ConstituentNodal->getIndex();
    ConstituentNodalIndex[0]=1;
    // we build _reverseDescendingConnectivity 
    _reverseDescendingConnectivity = new MEDSKYLINEARRAY(NumberOfConstituent,2*NumberOfConstituent) ;
    int *reverseDescendingConnectivityValue = _reverseDescendingConnectivity->getValue();
    int *reverseDescendingConnectivityIndex = _reverseDescendingConnectivity->getIndex();
    reverseDescendingConnectivityIndex[0]=1;

    // first constituent type
    for(int j=0; j<tmp_NumberOfConstituentsForeachType[0]; j++) {
      ConstituentNodalIndex[j+1]=ConstituentNodalConnectivityIndex[j+1];
      for(int k=ConstituentNodalIndex[j]; k<ConstituentNodalIndex[j+1]; k++){
	ConstituentNodalValue[k-1]=ConstituentNodalConnectivity[k-1];
      }
      reverseDescendingConnectivityIndex[j+1]=reverseDescendingConnectivityIndex[j]+2;
      for(int k=reverseDescendingConnectivityIndex[j]; k<reverseDescendingConnectivityIndex[j+1]; k++){
	reverseDescendingConnectivityValue[k-1]=ReverseDescendingConnectivityValue[k-1];
      }
    }
    // second type if any
    if (_constituent->_numberOfTypes==2) {
      int offset = _constituent->_count[1]-tmp_NumberOfConstituentsForeachType[0]-1 ;
      int offset1=offset*_constituent->_type[0].getNumberOfNodes();
      int offset2=offset*2 ;
      int NumberOfNodesPerConstituent = _constituent->_type[1].getNumberOfNodes();
      for(int j=tmp_NumberOfConstituentsForeachType[0]; j<NumberOfConstituent; j++) {
	ConstituentNodalIndex[j+1]=ConstituentNodalIndex[j]+NumberOfNodesPerConstituent;
	for(int k=ConstituentNodalIndex[j]; k<ConstituentNodalIndex[j+1]; k++){
	  ConstituentNodalValue[k-1]=ConstituentNodalConnectivity[offset1+k-1];
	}
	reverseDescendingConnectivityIndex[j+1]=reverseDescendingConnectivityIndex[j]+2;
	for(int k=reverseDescendingConnectivityIndex[j]; k<reverseDescendingConnectivityIndex[j+1]; k++){
	  reverseDescendingConnectivityValue[k-1]=ReverseDescendingConnectivityValue[offset2+k-1];
	}
      }
      _constituent->_count[2]=NumberOfConstituent+1 ;
      // we correct _descending to adjust face number
      for(int j=0;j<DescendingSize;j++)
	if (descend_connectivity[j]>tmp_NumberOfConstituentsForeachType[0])
	  descend_connectivity[j]-=offset;
      
    }
    _constituent->_count[1]=tmp_NumberOfConstituentsForeachType[0]+1;
    delete _constituent->_nodal ;
    _constituent->_nodal = ConstituentNodal ;
    
    delete[] ReverseDescendingConnectivityValue ;
  }
  END_OF(LOC);
}

//-----------------------------------------------------------------------------------------//
//  void CONNECTIVITY::calculateReverseDescendingConnectivity(CONNECTIVITY *myConnectivity)
//-----------------------------------------------------------------------------------------//
//  {
//    if (_entity==MED_CELL)
//      throw MEDEXCEPTION("CONNECTIVITY::calculateReverseDescending
//        Connectivity : we could not do that with MED_CELL entity !");
//    if (myConnectivity->getEntity()!=MED_CELL)
//      throw MEDEXCEPTION("CONNECTIVITY::calculateReverseDescending
//  		Connectivity : we need MED_CELL connectivity !");
//    return ;
//  }

/*! Not implemented yet */
//--------------------------------------------------------------------//
void CONNECTIVITY::calculateNeighbourhood(CONNECTIVITY &myConnectivity)
//--------------------------------------------------------------------//
{
  // Mesh dimension !
  return ;
}


/*! 
  Give, for one element number of a specified entity the geometric type
  Of this element.

  Example : medGeometryElement myType = myConnectivity.getElementType(MED_CELL,35) ;
*/
//--------------------------------------------------------------------//
medGeometryElement CONNECTIVITY::getElementType(medEntityMesh Entity,int Number) const
//--------------------------------------------------------------------//
{
  if (_entity==Entity) {
    for(int i=1; i<=_numberOfTypes;i++)
      if (Number<_count[i])
	return _geometricTypes[i-1] ;
  }
  else if (_constituent!=NULL)
    return _constituent->getElementType(Entity,Number) ;
  else
    throw MEDEXCEPTION(LOCALIZED("getElementType : Entity not defined !")) ;
  throw MEDEXCEPTION(LOCALIZED("getElementType : Wrong Number !")) ;
}
