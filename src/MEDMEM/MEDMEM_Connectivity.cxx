#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_CellModel.hxx"

#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ModulusArray.hxx"

#include "MEDMEM_STRING.hxx"
#include <iomanip>

using namespace std;
using namespace MEDMEM;
/*!
   Default Constructor. /n
   Default for Entity is MED_CELL and type of Connectivity is MED_NODAL */
//--------------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY(medEntityMesh Entity /* =MED_CELL */) :
//--------------------------------------------------------------//
  				_entity(Entity),
  				_typeConnectivity(MED_NODAL),
  				_numberOfTypes(0),
  				_geometricTypes((medGeometryElement*)NULL),
  				_type((CELLMODEL*)NULL),
  				_entityDimension(0),
//CCRT  				_count((int*)NULL),
  				_count((med_int*)NULL),
  				_nodal((MEDSKYLINEARRAY*)NULL),
  				_descending((MEDSKYLINEARRAY*)NULL),
  				_reverseNodalConnectivity((MEDSKYLINEARRAY*)NULL),
  				_reverseDescendingConnectivity((MEDSKYLINEARRAY*)NULL),
  				_neighbourhood((MEDSKYLINEARRAY*)NULL),
  				_constituent((CONNECTIVITY*)NULL)
{
   MESSAGE("CONNECTIVITY(medEntityMesh Entity=MED_CELL)");
}

/*!
   Constructor. /n
   Default for Entity is MED_CELL */
//------------------------------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY(int numberOfTypes,medEntityMesh Entity /* =MED_CELL */):
//------------------------------------------------------------------------------//
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
  MESSAGE("CONNECTIVITY(int numberOfTypes,medEntityMesh Entity=MED_CELL)");
  _geometricTypes = new medGeometryElement[numberOfTypes];
  _type = new CELLMODEL[numberOfTypes];
//CCRT  _count = new int[numberOfTypes+1];
  _count = new med_int[numberOfTypes+1];
}

/*!
  Copy Constructor.
*/
//------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY (const CONNECTIVITY & m):
//----------------------------------------------------//
  				_entity           (m._entity),
  				_typeConnectivity (m._typeConnectivity),
  				_numberOfTypes    (m._numberOfTypes),
  				_entityDimension  (m._entityDimension),
  				_numberOfNodes    (m._numberOfNodes)
{
 if (m._geometricTypes != NULL)
 {
    _geometricTypes = new medGeometryElement[_numberOfTypes];
    memcpy(_geometricTypes,m._geometricTypes,_numberOfTypes*sizeof(medGeometryElement));
 }
 else
    _geometricTypes = (medGeometryElement *) NULL;

 if (m._type != NULL) 
 {
    _type = new CELLMODEL[_numberOfTypes];
    for (int i=0;i<_numberOfTypes;i++)
        _type[i] = CELLMODEL(m._type[i]);
 }
 else
    _type = (CELLMODEL *) NULL;

 if (m._count != NULL)
 {
      _count = new med_int[_numberOfTypes+1];
      memcpy(_count,m._count,(_numberOfTypes+1)*sizeof(med_int));
 }
 else
    _count = (med_int *) NULL;

 if (m._nodal != NULL)
    _nodal = new MEDSKYLINEARRAY(* m._nodal);
 else
    _nodal = (MEDSKYLINEARRAY *) NULL;

 if (m._descending != NULL)
    _descending = new MEDSKYLINEARRAY(* m._descending);
 else
    _descending = (MEDSKYLINEARRAY *) NULL;

 if (m._reverseNodalConnectivity != NULL)
    _reverseNodalConnectivity = new MEDSKYLINEARRAY(* m._reverseNodalConnectivity);
 else
    _reverseNodalConnectivity = (MEDSKYLINEARRAY *) NULL;

 if (m._reverseDescendingConnectivity != NULL)
    _reverseDescendingConnectivity = new MEDSKYLINEARRAY(* m._reverseDescendingConnectivity);
 else
    _reverseDescendingConnectivity = (MEDSKYLINEARRAY *) NULL;

 if (m._neighbourhood != NULL)
    _neighbourhood = new MEDSKYLINEARRAY(* m._neighbourhood);
 else
    _neighbourhood = (MEDSKYLINEARRAY *) NULL;

 if (m._constituent != NULL)
    _constituent = new CONNECTIVITY(* m._constituent);
 else
    _constituent = (CONNECTIVITY *) NULL;
}

/*!
   Destructor./n
   desallocates existing pointers */
//----------------------------//
CONNECTIVITY::~CONNECTIVITY()
//----------------------------//
{
  MESSAGE("Destructeur de CONNECTIVITY()");

  if (_geometricTypes != NULL)
     delete [] _geometricTypes;
  if (_type != NULL)
     delete [] _type;
  if (_count != NULL)
     delete [] _count;
  if (_nodal != NULL)
     delete _nodal;
  if (_descending != NULL)
     delete _descending;
  if (_reverseNodalConnectivity != NULL)
     delete _reverseNodalConnectivity;
  if (_reverseDescendingConnectivity != NULL)
     delete _reverseDescendingConnectivity;
  if (_constituent != NULL)
     delete _constituent;
}

/*!
   set _constituent to Constituent
   be aware desallocation of _constituent is done by CONNECTIVITY:~CONNECTIVITY
   throws an exception if Constituent = MED_CELL
   A DOCUMENTER
    */
//----------------------------------------------------------//
void CONNECTIVITY::setConstituent(CONNECTIVITY * Constituent)
  		    throw (MEDEXCEPTION)
//----------------------------------------------------------//
{
  medEntityMesh Entity = Constituent->getEntity();
  if (Entity == MED_CELL)
    throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setConstituent : could not set constituent on MED_CELL !"));

  if ((Entity == MED_EDGE)&(_entityDimension == 3)) 
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setConstituent : Entity not found !"));
    _constituent->setConstituent(Constituent);
  }
  else
    _constituent = Constituent;
}

/*! Duplicated Types array in CONNECTIVITY object. */
//--------------------------------------------------------------------//
void CONNECTIVITY::setGeometricTypes(const medGeometryElement * Types,
				     const medEntityMesh Entity)
  				     throw (MEDEXCEPTION)
//--------------------------------------------------------------------//
{
  if (Entity == _entity)
    for (int i=0; i<_numberOfTypes; i++) 
    {
      _geometricTypes[i] = Types[i];
      _type[i] = CELLMODEL(_geometricTypes[i]);
    }
  else 
    { 
    if (_constituent == NULL)
        throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setGeometricTypes : Entity not found !"));
    _constituent->setGeometricTypes(Types,Entity);
  }
}

/*! A DOCUMENTER */
//--------------------------------------------------------------------//
//CCRTvoid CONNECTIVITY::setCount(const int * Count, const medEntityMesh Entity)
void CONNECTIVITY::setCount(const med_int * Count, const medEntityMesh Entity)
  			    throw (MEDEXCEPTION)
//--------------------------------------------------------------------//
{
  if (Entity == _entity) 
  {
//CCRT    int * index = new int[Count[_numberOfTypes]];
    med_int * index = new med_int[Count[_numberOfTypes]];
    index[0]=1;
    _count[0]=1;
    for (int i=0; i<_numberOfTypes; i++) {
      _count[i+1] = Count[i+1];
      int NumberOfNodesPerElement = _type[i].getNumberOfNodes() ;
      for (int j=_count[i]; j<_count[i+1]; j++)
	index[j] = index[j-1]+NumberOfNodesPerElement;
    }
    // allocate _nodal
    if (_nodal != NULL) delete _nodal;
    _nodal = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,index[_count[_numberOfTypes]-1]-1);
    _nodal->setIndex(index);
    delete [] index;
  }
  else
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setCount : Entity not found !"));
    _constituent->setCount(Count,Entity);
  }
}

//--------------------------------------------------------------------//
//CCRTvoid CONNECTIVITY::setNodal(const int * Connectivity,
void CONNECTIVITY::setNodal(const med_int * Connectivity,
			    const medEntityMesh Entity,
			    const medGeometryElement Type)
  			    throw (MEDEXCEPTION)
//--------------------------------------------------------------------//
{
  if (_entity == Entity) 
  {
    // find geometric type
    bool find = false;
    for (int i=0; i<_numberOfTypes; i++)
      if (_geometricTypes[i] == Type) {
	find = true;
	int NumberOfNodePerElements = _type[i].getNumberOfNodes() ;
	//_nodal->setI(i+1,Connectivity);
	for( int j=_count[i];j<_count[i+1]; j++)
	  _nodal->setI(j,Connectivity+(j-_count[i])*NumberOfNodePerElements);
      }
    if (!find)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setNodal : geometric type not found"));
  } else 
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setNodal : Entity not found !"));
      _constituent->setNodal(Connectivity,Entity,Type);
  }
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
      calculateNodalConnectivity();
    else 
      if (Entity==MED_CELL)
	calculateDescendingConnectivity();
      else
	throw MEDEXCEPTION("CONNECTIVITY::calculateConnectivity : Could not build DESCENDING !");
  if (Entity!=_entity) {
    calculateDescendingConnectivity();
    if (_entityDimension == 2 || _entityDimension == 3)
      _constituent->calculateConnectivity(ConnectivityType,Entity);
  }
}

/*!  Give, in full or no interlace mode (for nodal connectivity),
     descending or nodal connectivity.

      You must give a %medEntityMesh (ie:MED_EDGE) 
      and a %medGeometryElement (ie:MED_SEG3).  */

//------------------------------------------------------------//
void CONNECTIVITY::updateFamily(vector<FAMILY*> myFamilies) 
//------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::updateFamily(vector<FAMILY*>) ";
  BEGIN_OF(LOC);

  int numberOfFamilies = myFamilies.size();
  if (numberOfFamilies == 0 ) {
    MESSAGE(LOC<<"No family");
    return;
  }
  // does we do an update ?
  if ((_constituent != NULL)&(_descending != NULL)) {
    MESSAGE(LOC<<"Constituent is already defined");
    return;
  }

  if ((_constituent != NULL)&(_descending == NULL)) {
    if (myFamilies[0]->getEntity() != _constituent->getEntity()) {
      MESSAGE(LOC<<"Family and constituent entity are different. We do nothing");
      return;
    }

    // well we could go !
    CONNECTIVITY * oldConstituent = _constituent;

//      for(int i=0; i<numberOfFamilies; i++) {
//        FAMILY * myFamily = myFamilies[i];
//        MESSAGE(LOC<<"updating the family (BEGIN) : " << *myFamily);
//      }

    _constituent = (CONNECTIVITY *)NULL;
    // for instance we must have nodal connectivity in constituent :
    if (oldConstituent->_nodal == NULL)
      {
	MESSAGE(LOC<<"We have no nodal connectivity of sub cell");
	throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have no nodal connectivity of sub cell"));
      }
    int oldNumberOfFace = oldConstituent->_nodal->getNumberOf();
//CCRT    const int * oldConstituentValue = oldConstituent->_nodal->getValue();
    const med_int * oldConstituentValue = oldConstituent->_nodal->getValue();
//CCRT    const int * oldConstituentIndex = oldConstituent->_nodal->getIndex();
    const med_int * oldConstituentIndex = oldConstituent->_nodal->getIndex();

    calculateDescendingConnectivity();

    MESSAGE(LOC << " Right after the call to calculateDescendingConnectivity");
    //    int newNumberOfFace = _constituent->_nodal->getNumberOf();
//CCRT    const int * newConstituentValue = _constituent->_nodal->getValue();
    const med_int * newConstituentValue = _constituent->_nodal->getValue();
//CCRT    const int * newConstituentIndex = _constituent->_nodal->getIndex();
    const med_int * newConstituentIndex = _constituent->_nodal->getIndex();
    
//CCRT    const int * newReverseDescendingIndex =
    const med_int * newReverseDescendingIndex =
      _reverseDescendingConnectivity->getIndex();
    
//CCRT    const int * newDescendingIndex = _descending->getIndex();
    const med_int * newDescendingIndex = _descending->getIndex();
    //    const int * newDescendingValue = _descending->getValue();

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

//CCRT    int * renumberingFromOldToNew = new int [oldNumberOfFace];
    med_int * renumberingFromOldToNew = new med_int [oldNumberOfFace];
    int index1 = 0;
    int indexm1 = 0;

    MESSAGE(LOC << " Right before the call to _constituent->calculateReverseNodalConnectivity");
    _constituent->calculateReverseNodalConnectivity();
    MESSAGE(LOC << " Right after the call to _constituent->calculateReverseNodalConnectivity");
    

    SCRUTE(oldNumberOfFace);


    for (int iOldFace=0;iOldFace<oldNumberOfFace;iOldFace++)
      {
	int index = 0;

	renumberingFromOldToNew[iOldFace] = iOldFace+1;
	//  	  renumberingFromOldToNew[iOldFace] = 999999;
	
	int face_it_beginOld = oldConstituentIndex[iOldFace];
	int face_it_endOld = oldConstituentIndex[iOldFace+1];
	int face_size_itOld = face_it_endOld - face_it_beginOld;

//CCRT	const int* NodesLists = oldConstituentValue + (face_it_beginOld-1);
	const med_int* NodesLists = oldConstituentValue + (face_it_beginOld-1);
	int face_size_itNew;
	
//CCRT	const int * reverseFaceNodal = _constituent->getReverseNodalConnectivity();
	const med_int * reverseFaceNodal = _constituent->getReverseNodalConnectivity();
//CCRT	const int * reverseFaceNodalIndex = _constituent->getReverseNodalConnectivityIndex();
	const med_int * reverseFaceNodalIndex = _constituent->getReverseNodalConnectivityIndex();

	// set an array wich contains faces numbers arround first node 
	int BeginIndexFaceArrayFirstNode=reverseFaceNodalIndex[NodesLists[0]-1];
	int EndIndexFaceArrayFirstNode=reverseFaceNodalIndex[NodesLists[0]];
	int NumberOfFacesInList=EndIndexFaceArrayFirstNode-BeginIndexFaceArrayFirstNode;

//CCRT	int * FacesList = new int[NumberOfFacesInList];
	med_int * FacesList = new med_int[NumberOfFacesInList];

	for (int l=BeginIndexFaceArrayFirstNode; l<EndIndexFaceArrayFirstNode; l++){
	  FacesList[l-BeginIndexFaceArrayFirstNode]=reverseFaceNodal[l-1];
	}
	// foreach node in sub cell, we search elements which are in common
	// at the end, we must have only one !

	for (int nodeFaceOld=1; nodeFaceOld<face_size_itOld; nodeFaceOld++)
	  {
	    int NewNumberOfFacesInList = 0;
//CCRT	    int * NewFacesList = new int[NumberOfFacesInList];
	    med_int * NewFacesList = new med_int[NumberOfFacesInList];

	    for (int l1=0; l1<NumberOfFacesInList; l1++) {
	      for (int l2=reverseFaceNodalIndex[NodesLists[nodeFaceOld]-1]; l2<reverseFaceNodalIndex[NodesLists[nodeFaceOld]]; l2++) {
		if (FacesList[l1]<reverseFaceNodal[l2-1])
		  // increasing order : FacesList[l1] are not in elements list of node l
		  break;
		if (FacesList[l1]==reverseFaceNodal[l2-1]) {
		  // we have found one
		  NewFacesList[NewNumberOfFacesInList]=FacesList[l1];
		  NewNumberOfFacesInList++;
		  break;
		}
	      }
	    }
	    NumberOfFacesInList = NewNumberOfFacesInList;
	    delete [] FacesList;
	    FacesList = NewFacesList;
	  }

	if (!NumberOfFacesInList==0)
	  {
	    if (NumberOfFacesInList>1)
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"More than one face found ("<<NumberOfFacesInList<<") ! " <<FacesList[0]<<" "<<FacesList[1] ));
	
	    MEDMODULUSARRAY modulusArrayOld(face_size_itOld,NodesLists);

	    int face_it_beginNew = newConstituentIndex[FacesList[0]-1];
	    int face_it_endNew = newConstituentIndex[FacesList[0]];
	    face_size_itNew = face_it_endNew - face_it_beginNew;

//CCRT	    const int * newNodesLists = newConstituentValue+newConstituentIndex[FacesList[0]-1]-1;
	    const med_int * newNodesLists = newConstituentValue+newConstituentIndex[FacesList[0]-1]-1;
	    MEDMODULUSARRAY modulusArrayNew(face_size_itNew,newNodesLists);
	
	    int retCompareNewOld = modulusArrayNew.compare(modulusArrayOld);

	    //SCRUTE(retCompareNewOld);

	    // Real new face found
	
	    if(retCompareNewOld == 1)
	      {
		renumberingFromOldToNew[iOldFace] = FacesList[0];
		index = 1;
		index1++;
	      }
	
	    // Reverse new face found
	
	    if(retCompareNewOld == -1)
	      {
		renumberingFromOldToNew[iOldFace] = FacesList[0];
		index = 1;
		indexm1++;
	    
		int face_it_begin = newReverseDescendingIndex[FacesList[0]-1];
		int face_it_end = newReverseDescendingIndex[FacesList[0]];
		int face_size_it = face_it_end - face_it_begin;

		if (face_size_it == 1)
		  throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This is a boundary face/edge, it should not be renumbered and it is wrongly oriented."));
	    
		if (face_size_it > 2)
		  throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This face/edge should not be a (d-1) cell because it has "<<face_size_it<<" neighbouring"));
	    
		// we have always 2 neighbourings
		int cell1 = _reverseDescendingConnectivity->getIJ(FacesList[0],1);
		int cell2 = _reverseDescendingConnectivity->getIJ(FacesList[0],2);
		// PROVISOIRE : en attendant que le SKYLINEARRAY de ReverseDescending soit correct (sans le zero)
		//  		    if (cell2 == 0)
		//  		      throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This is a boundary face/edge, it should not be renumbered and it is wrongly oriented."));
	
		if (cell2 != 0) { // we are not on border !!!!
	      
		  _reverseDescendingConnectivity->setIJ(FacesList[0],1,cell2);
		  // Updating _constituent->_nodal because of reversity
//CCRT		  const int * oldArray = oldConstituentValue+face_it_beginOld-1;
		  const med_int * oldArray = oldConstituentValue+face_it_beginOld-1;
		  for(int iarray=1;iarray<=face_size_itNew;iarray++){
		    _constituent->_nodal->setIJ(FacesList[0],iarray,oldArray[iarray-1]);
		  }
	      
		  // Updating _reverseDescendingConnectivity
	      
	      
		  _reverseDescendingConnectivity->setIJ(FacesList[0],2,cell1);
	      
		  // Updating _descending for cell1 and cell2
		  for(int iface=newDescendingIndex[cell1-1];iface<=newDescendingIndex[cell1];iface++)
		    if (_descending->getIndexValue(iface)==FacesList[0])
		      _descending->setIndexValue(iface,-FacesList[0]);
		    else if (_descending->getIndexValue(iface)==-FacesList[0])
		      _descending->setIndexValue(iface,FacesList[0]);
		  // else nothing to do
	      
		  for(int iface=newDescendingIndex[cell2-1];iface<newDescendingIndex[cell2];iface++)
		    if (_descending->getIndexValue(iface)==FacesList[0])
		      _descending->setIndexValue(iface,-FacesList[0]);
		    else if (_descending->getIndexValue(iface)==-FacesList[0])
		      _descending->setIndexValue(iface,FacesList[0]);
		  // else nothing to do

		} else {// else we are on border and we do nothing !!!!!!!!
		  MESSAGE("WARNING,WARNING,WARNING,WARNING,WARNING,WARNING");
		  MESSAGE(LOC<<" Boudary FACE "<<iOldFace+1<<" are wrong oriented !");
		  MESSAGE("WARNING,WARNING,WARNING,WARNING,WARNING,WARNING");
		}
	      }
	
	    if(index == 0)
	      {
		MESSAGE(LOC<<"Renumbering problem with the Face connectivity given by the User and the new Connectivity computed");
		throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have a Face connectivity problem"));
	      }
	  }
	delete [] FacesList;
      }
    
    MESSAGE(LOC<<"The Renumbering is finished and the status is");

    // Updating the Family
    
    SCRUTE(numberOfFamilies);

    for(int i=0; i<numberOfFamilies; i++) {
      FAMILY * myFamily = myFamilies[i];

      SCRUTE(myFamily);
      //    MESSAGE(LOC<<(*myFamily));

      if (myFamily->isOnAllElements()) {
	// we build number
	// we must have more constituent ?
	if (oldNumberOfFace==_constituent->getNumberOf(_constituent->getEntity(),MED_ALL_ELEMENTS))
	  throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have a family which is already in all constituent !"));
	myFamily->setAll(false);
	// values array :
//CCRT	int * values = new int[oldNumberOfFace] ;
	med_int * values = new med_int[oldNumberOfFace] ;
	for (int ind=0;ind<oldNumberOfFace;ind++)
	  values[ind]=ind+1;
	// index array
	int NumberOfTypes = myFamily->getNumberOfTypes();
//CCRT	const int * count = oldConstituent->getGlobalNumberingIndex(_constituent->getEntity());
	const med_int * count = oldConstituent->getGlobalNumberingIndex(_constituent->getEntity());
//CCRT	int * index = new int[NumberOfTypes+1] ;
	med_int * index = new med_int[NumberOfTypes+1] ;
//CCRT	memcpy(index,count,(NumberOfTypes+1)*sizeof(int));
	memcpy(index,count,(NumberOfTypes+1)*sizeof(med_int));
	// build new number attribut
	myFamily->setNumber(index,values);
      }

      //    MESSAGE(LOC<<(*myFamily));
      MEDSKYLINEARRAY * number = myFamily->getnumber();

      int numberOfLines_skyline = number->getNumberOf();


      SCRUTE(numberOfLines_skyline);

//CCRT      const int * index_skyline = number->getIndex();
      const med_int * index_skyline = number->getIndex();
      
      SCRUTE(index_skyline);

      for (int i=0;i<numberOfLines_skyline;i++) {
	for (int j=index_skyline[i]; j<index_skyline[i+1];j++) {
	  number->setIndexValue(j,renumberingFromOldToNew[number->getIndexValue(j)-1]);
	}
      }
      //    MESSAGE(LOC<<"updating the family (END) : " << *myFamily);
    }

    delete oldConstituent ;
    delete [] renumberingFromOldToNew;
  }
  

  END_OF(LOC);
  return;
}

// meme methode que updateFamily, mais avec des groupes. Il n'est pas possible d'utiliser
// l'heritage car les pointeurs sont dans un conteneur.
void CONNECTIVITY::updateGroup(vector<GROUP*> myFamilies)
//------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::updateGroup(vector<GROUP*>) ";
  BEGIN_OF(LOC);

  int numberOfFamilies = myFamilies.size();
  if (numberOfFamilies == 0 ) {
    MESSAGE(LOC<<"No family");
    return;
  }
  // does we do an update ?
  if ((_constituent != NULL)&(_descending != NULL)) {
    MESSAGE(LOC<<"Constituent is already defined");
    return;
  }

  if ((_constituent != NULL)&(_descending == NULL)) {
    if (myFamilies[0]->getEntity() != _constituent->getEntity()) {
      MESSAGE(LOC<<"Family and constituent entity are different. We do nothing");
      return;
    }

    // well we could go !
    CONNECTIVITY * oldConstituent = _constituent;

//      for(int i=0; i<numberOfFamilies; i++) {
//        FAMILY * myFamily = myFamilies[i];
//        MESSAGE(LOC<<"updating the family (BEGIN) : " << *myFamily);
//      }

    _constituent = (CONNECTIVITY *)NULL;
    // for instance we must have nodal connectivity in constituent :
    if (oldConstituent->_nodal == NULL)
      {
	MESSAGE(LOC<<"We have no nodal connectivity of sub cell");
	throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have no nodal connectivity of sub cell"));
      }
    int oldNumberOfFace = oldConstituent->_nodal->getNumberOf();
//CCRT    const int * oldConstituentValue = oldConstituent->_nodal->getValue();
    const med_int * oldConstituentValue = oldConstituent->_nodal->getValue();
//CCRT    const int * oldConstituentIndex = oldConstituent->_nodal->getIndex();
    const med_int * oldConstituentIndex = oldConstituent->_nodal->getIndex();

    calculateDescendingConnectivity();

    //    int newNumberOfFace = _constituent->_nodal->getNumberOf();
//CCRT    const int * newConstituentValue = _constituent->_nodal->getValue();
    const med_int * newConstituentValue = _constituent->_nodal->getValue();
//CCRT    const int * newConstituentIndex = _constituent->_nodal->getIndex();
    const med_int * newConstituentIndex = _constituent->_nodal->getIndex();
    
//CCRT    const int * newReverseDescendingIndex =
    const med_int * newReverseDescendingIndex =
      _reverseDescendingConnectivity->getIndex();
    
//CCRT    const int * newDescendingIndex = _descending->getIndex();
    const med_int * newDescendingIndex = _descending->getIndex();
    //    const int * newDescendingValue = _descending->getValue();

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

//CCRT    int * renumberingFromOldToNew = new int [oldNumberOfFace];
    med_int * renumberingFromOldToNew = new med_int [oldNumberOfFace];
    int index1 = 0;
    int indexm1 = 0;

    _constituent->calculateReverseNodalConnectivity();
    
    for (int iOldFace=0;iOldFace<oldNumberOfFace;iOldFace++)
      {
	int index = 0;

	renumberingFromOldToNew[iOldFace] = iOldFace+1;
	//  	  renumberingFromOldToNew[iOldFace] = 999999;
	
	int face_it_beginOld = oldConstituentIndex[iOldFace];
	int face_it_endOld = oldConstituentIndex[iOldFace+1];
	int face_size_itOld = face_it_endOld - face_it_beginOld;

//CCRT	const int* NodesLists = oldConstituentValue + (face_it_beginOld-1);
	const med_int* NodesLists = oldConstituentValue + (face_it_beginOld-1);
	int face_size_itNew;
	
//CCRT	const int * reverseFaceNodal = _constituent->getReverseNodalConnectivity();
	const med_int * reverseFaceNodal = _constituent->getReverseNodalConnectivity();
//CCRT	const int * reverseFaceNodalIndex = _constituent->getReverseNodalConnectivityIndex();
	const med_int * reverseFaceNodalIndex = _constituent->getReverseNodalConnectivityIndex();

	// set an array wich contains faces numbers arround first node 
	int BeginIndexFaceArrayFirstNode=reverseFaceNodalIndex[NodesLists[0]-1];
	int EndIndexFaceArrayFirstNode=reverseFaceNodalIndex[NodesLists[0]];
	int NumberOfFacesInList=EndIndexFaceArrayFirstNode-BeginIndexFaceArrayFirstNode;

//CCRT	int * FacesList = new int[NumberOfFacesInList];
	med_int * FacesList = new med_int[NumberOfFacesInList];

	for (int l=BeginIndexFaceArrayFirstNode; l<EndIndexFaceArrayFirstNode; l++){
	  FacesList[l-BeginIndexFaceArrayFirstNode]=reverseFaceNodal[l-1];
	}
	// foreach node in sub cell, we search elements which are in common
	// at the end, we must have only one !

	for (int nodeFaceOld=1; nodeFaceOld<face_size_itOld; nodeFaceOld++)
	  {
	    int NewNumberOfFacesInList = 0;
//CCRT	    int * NewFacesList = new int[NumberOfFacesInList];
	    med_int * NewFacesList = new med_int[NumberOfFacesInList];

	    for (int l1=0; l1<NumberOfFacesInList; l1++) {
	      for (int l2=reverseFaceNodalIndex[NodesLists[nodeFaceOld]-1]; l2<reverseFaceNodalIndex[NodesLists[nodeFaceOld]]; l2++) {
		if (FacesList[l1]<reverseFaceNodal[l2-1])
		  // increasing order : FacesList[l1] are not in elements list of node l
		  break;
		if (FacesList[l1]==reverseFaceNodal[l2-1]) {
		  // we have found one
		  NewFacesList[NewNumberOfFacesInList]=FacesList[l1];
		  NewNumberOfFacesInList++;
		  break;
		}
	      }
	    }
	    NumberOfFacesInList = NewNumberOfFacesInList;
	    delete [] FacesList;
	    FacesList = NewFacesList;
	  }

	if (!NumberOfFacesInList==0)
	  {
	    if (NumberOfFacesInList>1)
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"More than one face found ("<<NumberOfFacesInList<<") ! " <<FacesList[0]<<" "<<FacesList[1] ));
	
	    MEDMODULUSARRAY modulusArrayOld(face_size_itOld,NodesLists);

	    int face_it_beginNew = newConstituentIndex[FacesList[0]-1];
	    int face_it_endNew = newConstituentIndex[FacesList[0]];
	    face_size_itNew = face_it_endNew - face_it_beginNew;

//CCRT	    const int * newNodesLists = newConstituentValue+newConstituentIndex[FacesList[0]-1]-1;
	    const med_int * newNodesLists = newConstituentValue+newConstituentIndex[FacesList[0]-1]-1;
	    MEDMODULUSARRAY modulusArrayNew(face_size_itNew,newNodesLists);
	
	    int retCompareNewOld = modulusArrayNew.compare(modulusArrayOld);

	    //SCRUTE(retCompareNewOld);

	    // Real new face found
	
	    if(retCompareNewOld == 1)
	      {
		renumberingFromOldToNew[iOldFace] = FacesList[0];
		index = 1;
		index1++;
	      }
	
	    // Reverse new face found
	
	    if(retCompareNewOld == -1)
	      {
		renumberingFromOldToNew[iOldFace] = FacesList[0];
		index = 1;
		indexm1++;
	    
		int face_it_begin = newReverseDescendingIndex[FacesList[0]-1];
		int face_it_end = newReverseDescendingIndex[FacesList[0]];
		int face_size_it = face_it_end - face_it_begin;

		if (face_size_it == 1)
		  throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This is a boundary face/edge, it should not be renumbered and it is wrongly oriented."));
	    
		if (face_size_it > 2)
		  throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This face/edge should not be a (d-1) cell because it has "<<face_size_it<<" neighbouring"));
	    
		// we have always 2 neighbourings
		int cell1 = _reverseDescendingConnectivity->getIJ(FacesList[0],1);
		int cell2 = _reverseDescendingConnectivity->getIJ(FacesList[0],2);
		// PROVISOIRE : en attendant que le SKYLINEARRAY de ReverseDescending soit correct (sans le zero)
		//  		    if (cell2 == 0)
		//  		      throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"This is a boundary face/edge, it should not be renumbered and it is wrongly oriented."));
	
		if (cell2 != 0) { // we are not on border !!!!
	      
		  _reverseDescendingConnectivity->setIJ(FacesList[0],1,cell2);
		  // Updating _constituent->_nodal because of reversity
//CCRT		  const int * oldArray = oldConstituentValue+face_it_beginOld-1;
		  const med_int * oldArray = oldConstituentValue+face_it_beginOld-1;
		  for(int iarray=1;iarray<=face_size_itNew;iarray++){
		    _constituent->_nodal->setIJ(FacesList[0],iarray,oldArray[iarray-1]);
		  }
	      
		  // Updating _reverseDescendingConnectivity
	      
	      
		  _reverseDescendingConnectivity->setIJ(FacesList[0],2,cell1);
	      
		  // Updating _descending for cell1 and cell2
		  for(int iface=newDescendingIndex[cell1-1];iface<=newDescendingIndex[cell1];iface++)
		    if (_descending->getIndexValue(iface)==FacesList[0])
		      _descending->setIndexValue(iface,-FacesList[0]);
		    else if (_descending->getIndexValue(iface)==-FacesList[0])
		      _descending->setIndexValue(iface,FacesList[0]);
		  // else nothing to do
	      
		  for(int iface=newDescendingIndex[cell2-1];iface<newDescendingIndex[cell2];iface++)
		    if (_descending->getIndexValue(iface)==FacesList[0])
		      _descending->setIndexValue(iface,-FacesList[0]);
		    else if (_descending->getIndexValue(iface)==-FacesList[0])
		      _descending->setIndexValue(iface,FacesList[0]);
		  // else nothing to do

		} else {// else we are on border and we do nothing !!!!!!!!
		  MESSAGE("WARNING,WARNING,WARNING,WARNING,WARNING,WARNING");
		  MESSAGE(LOC<<" Boudary FACE "<<iOldFace+1<<" are wrong oriented !");
		  MESSAGE("WARNING,WARNING,WARNING,WARNING,WARNING,WARNING");
		}
	      }
	
	    if(index == 0)
	      {
		MESSAGE(LOC<<"Renumbering problem with the Face connectivity given by the User and the new Connectivity computed");
		throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have a Face connectivity problem"));
	      }
	  }
	delete [] FacesList;
      }
    
    MESSAGE(LOC<<"The Renumbering is finished and the status is");

    // Updating the Family
    
    for(int i=0; i<numberOfFamilies; i++) {
      GROUP * myFamily = myFamilies[i];
      
      MEDSKYLINEARRAY * number = myFamily->getnumber();
      int numberOfLines_skyline = number->getNumberOf();
//CCRT      const int * index_skyline = number->getIndex();
      const med_int * index_skyline = number->getIndex();
      
      for (int i=0;i<numberOfLines_skyline;i++) {
	for (int j=index_skyline[i]; j<index_skyline[i+1];j++) {
	  number->setIndexValue(j,renumberingFromOldToNew[number->getIndexValue(j)-1]);
	}
      }
      MESSAGE(LOC<<"updating the family (END) : " << *myFamily);
    }

    delete oldConstituent ;
    delete [] renumberingFromOldToNew;
  }
  

  END_OF(LOC);
  return;
}

//------------------------------------------------------------------------------------------------------------------//
const med_int * CONNECTIVITY::getConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type) 
//------------------------------------------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getConnectivity";
  BEGIN_OF(LOC);

  MEDSKYLINEARRAY * Connectivity;
  if (Entity==_entity) {
    
    if (ConnectivityType==MED_NODAL)
      {
	calculateNodalConnectivity();
	Connectivity=_nodal;
      }
    else
      {
	calculateDescendingConnectivity();
	Connectivity=_descending;
      }
    
    if (Connectivity!=NULL)
      if (Type==MED_ALL_ELEMENTS) {
	return Connectivity->getValue();
      }
      else {
	for (med_int i=0; i<_numberOfTypes; i++)
	  if (_geometricTypes[i]==Type) {
	    //return Connectivity->getI(i+1);
	    return Connectivity->getI(_count[i]);
	  }
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Type not found !"));
      }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !"));
  } else 
    if (_constituent != NULL)
      return _constituent->getConnectivity(ConnectivityType,Entity,Type);
  
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}  

/*!  Give morse index array to use with
     getConnectivity(MED_FULL_INTERLACE,mode,entity,MED_ALL_ELEMENTS).

      Each value give start index for corresponding entity in connectivity array.

      Example : i-th element, j-th node of it :
      - In C mode : Connectivity[ConnectivityIndex[i]-1+j-1]
      - In fortran mode : Connectivity[ConnectivityIndex[i]+j] */
//-----------------------------------------------------------------------------------------------//
const med_int * CONNECTIVITY::getConnectivityIndex(medConnectivity ConnectivityType, medEntityMesh Entity) 
//----0000000--------------------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getConnectivityIndex";
  BEGIN_OF(LOC);

  MEDSKYLINEARRAY * Connectivity;
  if (Entity==_entity) {
    
    if (ConnectivityType==MED_NODAL)
      Connectivity=_nodal;
    else
      Connectivity=_descending;
    
    if (Connectivity!=NULL)
      return Connectivity->getIndex();
    else 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !"));

  } else 
    if (_constituent != NULL)
      return _constituent->getConnectivityIndex(ConnectivityType,Entity);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}

/*! A DOCUMENTER */
//--------------------------------------------------------------//
const CELLMODEL & CONNECTIVITY::getType(medGeometryElement Type) const
//--------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getType";
  BEGIN_OF(LOC);

  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE !"));
  for (med_int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i];
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
}

/*!  Returns the number of elements of type %medGeometryElement.
     Note : not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
med_int CONNECTIVITY::getNumberOfNodesInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getNumberOfNodesInType";
  BEGIN_OF(LOC);

  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!"));
  for (med_int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i].getNumberOfNodes();
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
}

/*!  Returns the number of geometric sub cells of %medGeometryElement type.
not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
med_int CONNECTIVITY::getNumberOfSubCellInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION("CONNECTIVITY::getNumberOfSubCell : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!");
  for (med_int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i].getNumberOfConstituents(1);
  throw MEDEXCEPTION("CONNECTIVITY::getNumberOfSubCell : medGeometryElement not found !");
}

/*!  Returns the number of elements of type %medGeometryElement.

     Note :
      - Implemented for MED_ALL_ELEMENTS
      - Not implemented for MED_ALL_ENTITIES (A VERIFIER)
      - Not implemented for MED_NONE */
//-----------------------------------------------------------------------------------//
med_int CONNECTIVITY::getNumberOf(medEntityMesh Entity, medGeometryElement Type) const
//-----------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getNumberOf";
  BEGIN_OF(LOC);

  MESSAGE(LOC<<" Entity = "<< Entity << ", _entity = "<<_entity);

  if (Entity==_entity) {
    if (Type==MED_NONE)
      return 0; // not defined !
    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_NONE"));
    if (Type==MED_ALL_ELEMENTS)
      return _count[_numberOfTypes]-1;
    for (med_int i=0; i<_numberOfTypes; i++)
      if (_geometricTypes[i]==Type)
	return (_count[i+1] - _count[i]);
    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
  } else 
    if (_constituent != NULL)
      return _constituent->getNumberOf(Entity,Type);

  return 0; // valid if they are nothing else !
  //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}

/*! A DOCUMENTER */
//--------------------------------------------------------------//
const med_int* CONNECTIVITY::getValue(medConnectivity TypeConnectivity, 
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
const med_int* CONNECTIVITY:: getValueIndex(medConnectivity TypeConnectivity) 
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
const med_int* CONNECTIVITY:: getNeighbourhood() const
//----------------------------------------------//
{
  throw MEDEXCEPTION("CONNECTIVITY::getNeighbourhood : Not implemented");
}

/*! Returns an array which contains, for each node, all cells
    arround it. */
//-------------------------------------------------//
const med_int* CONNECTIVITY::getReverseNodalConnectivity() 
//-------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getValue();
}

/*!  Give index array to use with getReverseConnectivity(MED_NODAL).
     It is unusefull with MED_DESCENDING mode, because we have allways two cells.  */
//-------------------------------------------------------//
const med_int* CONNECTIVITY::getReverseNodalConnectivityIndex() 
//-------------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getIndex();
}

/*! Returns an array which contains, for each face (or edge),
    the 2 cells of each side. First is cell which face normal is outgoing.
    arround it. */
//------------------------------------------------------//
const med_int* CONNECTIVITY::getReverseDescendingConnectivity() 
//------------------------------------------------------//
{
  // it is in _constituent connectivity only if we are in MED_CELL 
  // (we could not for instance calculate face-edge connectivity !)
  if (_entity!=MED_CELL)
    throw MEDEXCEPTION("CONNECTIVITY::getReverseDescendingConnectivity : Error Only in MED_CELL connectivity");

  // we need descending connectivity 
  calculateDescendingConnectivity();
  return _reverseDescendingConnectivity->getValue();
}

/*! calculate the reverse descending Connectivity 
    and returns the index  ( A DOCUMENTER MIEUX)*/
//-----------------------------------------------------------//
const med_int* CONNECTIVITY::getReverseDescendingConnectivityIndex() 
//-----------------------------------------------------------//
{
  // it is in _constituent connectivity only if we are in MED_CELL
  if (_entity!=MED_CELL) 
    throw MEDEXCEPTION("CONNECTIVITY::getReverseDescendingConnectivityIndex : Error Only in MED_CELL connectivity");

  // we need descending connectivity 
  calculateDescendingConnectivity();
  return _reverseDescendingConnectivity->getIndex();
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
  const char * LOC = "CONNECTIVITY::calculateReverseNodalConnectivity : ";
  BEGIN_OF(LOC);

  SCRUTE(_nodal);
  SCRUTE(_reverseNodalConnectivity);


  if (_nodal==NULL) 
    calculateNodalConnectivity();
  
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
	  for (med_int local_node_number = 1; local_node_number < node_number+1; local_node_number++)
	    {
	      med_int global_node = _nodal->getIJ(k,local_node_number);
	      reverse_connectivity[global_node].push_back(k);
	    }
      }
  
    // Full reverse_nodal_connectivity and reverse_nodal_connectivity_index

  //calculate size of reverse_nodal_connectivity array
    med_int size_reverse_nodal_connectivity  = 0;
    for (med_int i = 1; i < _numberOfNodes+1; i++)
      size_reverse_nodal_connectivity += reverse_connectivity[i].size();
  
    //MEDSKYLINEARRAY * ReverseConnectivity = new MEDSKYLINEARRAY(_numberOfNodes,size_reverse_nodal_connectivity);
//CCRT    med_int * reverse_nodal_connectivity_index = new (med_int)[_numberOfNodes+1];
    med_int * reverse_nodal_connectivity_index = new med_int[_numberOfNodes+1];
//CCRT    med_int * reverse_nodal_connectivity = new (med_int)[size_reverse_nodal_connectivity];
    med_int * reverse_nodal_connectivity = new med_int[size_reverse_nodal_connectivity];
    //const med_int * reverse_nodal_connectivity = ReverseConnectivity->getValue();
    //const med_int * reverse_nodal_connectivity_index = ReverseConnectivity->getIndex();

    reverse_nodal_connectivity_index[0] = 1;
    for (med_int i = 1; i < _numberOfNodes+1; i++)
      {
	med_int size = reverse_connectivity[i].size(); 
	reverse_nodal_connectivity_index[i] = reverse_nodal_connectivity_index[i-1] + size;
	for (med_int j = 0; j < size; j++)
	  reverse_nodal_connectivity[reverse_nodal_connectivity_index[i-1]-1+j]= reverse_connectivity[i][j];
      }
  
    //_reverseNodalConnectivity = ReverseConnectivity;
    _reverseNodalConnectivity = new MEDSKYLINEARRAY(_numberOfNodes,size_reverse_nodal_connectivity,
						    reverse_nodal_connectivity_index,
						    reverse_nodal_connectivity);
    delete [] reverse_nodal_connectivity_index;
    delete [] reverse_nodal_connectivity;
  }
  END_OF(LOC);
}

/*! If not yet done, calculate the Descending Connectivity */
//-------------------------------------------------//
void CONNECTIVITY::calculateDescendingConnectivity()
  //-------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::calculateDescendingConnectivity() : ";
  BEGIN_OF(LOC);
  
  if (_descending==NULL)
    {
      if (_nodal==NULL)
	{
	  MESSAGE(LOC<<"No connectivity found !");
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity found !"));
	}
      // calcul _descending from _nodal
      // we need CONNECTIVITY for constituent
      
      if (_constituent != NULL) 
	//      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"ERROR : No descending connectivity defined, but constituent exist !"));
	MESSAGE(LOC<<": No descending connectivity defined, but constituent exist !");

      if (_entityDimension == 3)
	_constituent = new CONNECTIVITY(MED_FACE);
      else if (_entityDimension == 2)
	_constituent = new CONNECTIVITY(MED_EDGE);
      else
	{
	  MESSAGE(LOC<<"We are in 1D");
	  return;
	}

      _constituent->_typeConnectivity = MED_NODAL;
      _constituent->_numberOfNodes = _numberOfNodes;
      // foreach cells, we built array of constituent
      int DescendingSize = 0;
      for(int i=0; i<_numberOfTypes; i++) 
	DescendingSize+=(_count[i+1]-_count[i])*_type[i].getNumberOfConstituents(1);
      //_descending = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,DescendingSize);
      //const int * descend_connectivity = _descending->getValue();
//CCRT      int * descend_connectivity = new int[DescendingSize];
      med_int * descend_connectivity = new med_int[DescendingSize];
      for (int i=0; i<DescendingSize; i++)
	descend_connectivity[i]=0;
      //const int * descend_connectivity_index = _descending->getIndex();
//CCRT      int * descend_connectivity_index = new int[_count[_numberOfTypes]];
      med_int * descend_connectivity_index = new med_int[_count[_numberOfTypes]];
      descend_connectivity_index[0]=1;
      medGeometryElement* ConstituentsTypes = new medGeometryElement[2];
      ConstituentsTypes[0]=MED_NONE;
      ConstituentsTypes[1]=MED_NONE;
//CCRT      int * NumberOfConstituentsForeachType = new int[2];
      med_int * NumberOfConstituentsForeachType = new med_int[2];
      NumberOfConstituentsForeachType[0]=0;
      NumberOfConstituentsForeachType[1]=0;
      for(int i=0; i<_numberOfTypes; i++)
	{
	  // initialize descend_connectivity_index array :
	  int NumberOfConstituents = _type[i].getNumberOfConstituents(1);
	  for (int j=_count[i];j<_count[i+1];j++)
	    {
	      descend_connectivity_index[j]=descend_connectivity_index[j-1]+NumberOfConstituents;
	      // compute number of constituent of all cell for each type
	      for(int k=1;k<NumberOfConstituents+1;k++)
		{
		  medGeometryElement MEDType = _type[i].getConstituentType(1,k);
		  if (ConstituentsTypes[0]==MED_NONE)
		    {
		      ConstituentsTypes[0]=MEDType;
		      NumberOfConstituentsForeachType[0]++;
		    } else if (ConstituentsTypes[0]==MEDType)
		      NumberOfConstituentsForeachType[0]++;
		  else if (ConstituentsTypes[1]==MED_NONE)
		    {
		      ConstituentsTypes[1]=MEDType;
		      NumberOfConstituentsForeachType[1]++;
		    } else if (ConstituentsTypes[1]==MEDType)
		      NumberOfConstituentsForeachType[1]++;
		  else
		    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<MEDType<<" is different of "<<ConstituentsTypes[0]<<" and "<<ConstituentsTypes[1]));
		}
	    }
	}

      // we could built _constituent !
      int TotalNumberOfConstituents = NumberOfConstituentsForeachType[0]+NumberOfConstituentsForeachType[1];
      int TotalNumberOfNodes = NumberOfConstituentsForeachType[0]*(ConstituentsTypes[0]%100)+NumberOfConstituentsForeachType[1]*(ConstituentsTypes[1]%100);
      
      //_constituent->_nodal = new MEDSKYLINEARRAY(TotalNumberOfConstituents,TotalNumberOfNodes);

      // we use _constituent->_nodal 
      //const int * ConstituentNodalConnectivity = _constituent->_nodal->getValue();
      //const int * ConstituentNodalConnectivityIndex = _constituent->_nodal->getIndex();
//CCRT      int * ConstituentNodalConnectivity = new int[TotalNumberOfNodes];
      med_int * ConstituentNodalConnectivity = new med_int[TotalNumberOfNodes];
//CCRT      int * ConstituentNodalConnectivityIndex = new int[TotalNumberOfConstituents+1];
      med_int * ConstituentNodalConnectivityIndex = new med_int[TotalNumberOfConstituents+1];
      ConstituentNodalConnectivityIndex[0]=1;

      _constituent->_entityDimension=ConstituentsTypes[0]/100;
      if (ConstituentsTypes[1]==MED_NONE)
	_constituent->_numberOfTypes = 1;
      else
	_constituent->_numberOfTypes = 2;
      _constituent->_geometricTypes = new medGeometryElement[_constituent->_numberOfTypes];
      _constituent->_type = new CELLMODEL[_constituent->_numberOfTypes];
      _constituent->_count = new med_int[_constituent->_numberOfTypes+1];
      _constituent->_count[0]=1;
//CCRT      int* tmp_NumberOfConstituentsForeachType = new med_int[_constituent->_numberOfTypes+1];
      med_int* tmp_NumberOfConstituentsForeachType = new med_int[_constituent->_numberOfTypes+1];
      tmp_NumberOfConstituentsForeachType[0]=0; // to count constituent of each type
      for (int i=0; i<_constituent->_numberOfTypes;i++) {
	_constituent->_geometricTypes[i]=ConstituentsTypes[i];
	_constituent->_count[i+1]=_constituent->_count[i]+NumberOfConstituentsForeachType[i];
	CELLMODEL Type(ConstituentsTypes[i]);
	_constituent->_type[i]=Type;
	tmp_NumberOfConstituentsForeachType[i+1]=NumberOfConstituentsForeachType[i];
	for (int j=tmp_NumberOfConstituentsForeachType[i]; j<tmp_NumberOfConstituentsForeachType[i+1]+tmp_NumberOfConstituentsForeachType[i]; j++)
	  ConstituentNodalConnectivityIndex[j+1]=ConstituentNodalConnectivityIndex[j]+(ConstituentsTypes[i]%100);
      }
      delete [] ConstituentsTypes;
      delete [] NumberOfConstituentsForeachType;
    
      // we need reverse nodal connectivity
      if (! _reverseNodalConnectivity)
	calculateReverseNodalConnectivity();
//CCRT      const int * ReverseNodalConnectivityIndex = _reverseNodalConnectivity->getIndex();
      const med_int * ReverseNodalConnectivityIndex = _reverseNodalConnectivity->getIndex();
//CCRT      const int * ReverseNodalConnectivityValue = _reverseNodalConnectivity->getValue();
      const med_int * ReverseNodalConnectivityValue = _reverseNodalConnectivity->getValue();

      // array to keep reverse descending connectivity
//CCRT      int * ReverseDescendingConnectivityValue = new int[TotalNumberOfConstituents*2];
      med_int * ReverseDescendingConnectivityValue = new med_int[TotalNumberOfConstituents*2];
      
      int TotalNumberOfSubCell = 0;
      for (int i=0; i<_numberOfTypes; i++)
	{ // loop on all cell type
	  CELLMODEL Type = _type[i];
	  //      int NumberOfNodesPerCell = Type.getNumberOfNodes();
	  int NumberOfConstituentPerCell = Type.getNumberOfConstituents(1);
	  for (int j=_count[i]; j<_count[i+1]; j++) // we loop on all cell of this type
	    for (int k=1; k<=NumberOfConstituentPerCell; k++)
	      { // we loop on all sub cell of it
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
		  ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2]=j;
		  int NumberOfNodesPerConstituent = Type.getConstituentType(1,k)%100;
	    
//CCRT		  int * NodesLists = new int[NumberOfNodesPerConstituent];
		  med_int * NodesLists = new med_int[NumberOfNodesPerConstituent];
		  for (int l=0; l<NumberOfNodesPerConstituent; l++) {
		    NodesLists[l]=_nodal->getIJ(j,Type.getNodeConstituent(1,k,l+1));
		    ConstituentNodalConnectivity[ConstituentNodalConnectivityIndex[TotalNumberOfSubCell-1]-1+l]=NodesLists[l];
		  }
		  // we use reverse_nodal_connectivity to find the other element which contain this sub cell

		  // all elements which contains first node of sub cell :
		  int ReverseNodalConnectivityIndex_0 = ReverseNodalConnectivityIndex[NodesLists[0]-1];
		  int ReverseNodalConnectivityIndex_1 = ReverseNodalConnectivityIndex[NodesLists[0]];
		  int NumberOfCellsInList = ReverseNodalConnectivityIndex_1-ReverseNodalConnectivityIndex_0;

		  if (NumberOfCellsInList > 0)
		    { // we could have no element !
//CCRT		      int * CellsList = new int[NumberOfCellsInList];
		      med_int * CellsList = new med_int[NumberOfCellsInList];
		      for (int l=ReverseNodalConnectivityIndex_0; l<ReverseNodalConnectivityIndex_1; l++)
			CellsList[l-ReverseNodalConnectivityIndex_0]=ReverseNodalConnectivityValue[l-1];
		  
		      // foreach node in sub cell, we search elements which are in common
		      // at the end, we must have only one !

		      for (int l=1; l<NumberOfNodesPerConstituent; l++) {
			int NewNumberOfCellsInList = 0;
//CCRT			int * NewCellsList = new int[NumberOfCellsInList];
			med_int * NewCellsList = new med_int[NumberOfCellsInList];
			for (int l1=0; l1<NumberOfCellsInList; l1++)
			  for (int l2=ReverseNodalConnectivityIndex[NodesLists[l]-1]; l2<ReverseNodalConnectivityIndex[NodesLists[l]]; l2++)
			    {
			      if (CellsList[l1]<ReverseNodalConnectivityValue[l2-1])
				// increasing order : CellsList[l1] are not in elements list of node l
				break;
			      if ((CellsList[l1]==ReverseNodalConnectivityValue[l2-1])&(CellsList[l1]!=j)) {
				// we have found one
				NewCellsList[NewNumberOfCellsInList]=CellsList[l1];
				NewNumberOfCellsInList++;
				break;
			      }
			    }
			NumberOfCellsInList = NewNumberOfCellsInList;

			delete [] CellsList;
			CellsList = NewCellsList;
		      }
	    
		      if (NumberOfCellsInList > 0) { // We have found some elements !
			int CellNumber = CellsList[0];
			//delete [] CellsList;
			if (NumberOfCellsInList>1)
			  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"More than one other Cell ("<<NumberOfCellsInList<<") !"));

			if (NumberOfCellsInList==1)
			  {
			    ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2+1]=CellNumber;
		      
			    // we search sub cell number in this cell to not calculate it another time
			    // which type ?
			    CELLMODEL Type2;
			    for (int l=0; l<_numberOfTypes; l++)
			      if (CellNumber < _count[l+1]) {
				Type2=_type[l];
				break;
			      }
			    //int sub_cell_count2 = Type2.get_entities_count(1);
			    //int nodes_cell_count2 = Type2.get_nodes_count();
			    bool find2 = false;
			    for (int l=1; l<=Type2.getNumberOfConstituents(1);l++) { // on all sub cell
			      int counter = 0;
			      for (int m=1; m<=Type2.getConstituentType(1,l)%100; m++)
				for (int n=1; n<=Type.getConstituentType(1,k)%100; n++)
				  { 
				    if (_nodal->getIJ(CellNumber,Type2.getNodeConstituent(1,l,m)) == NodesLists[n-1])
				      counter++;
				  }
			      if (counter==Type.getConstituentType(1,k)%100)
				{
				  descend_connectivity[descend_connectivity_index[CellNumber-1]+l-2]=-1*TotalNumberOfSubCell; // because, see it in other side !
				  find2 = true;
				}
			      if (find2)
				break;
			    }
			    if (!find2)
			      MESSAGE(LOC<<"ERROR ERROR ERROR ERROR ERROR : we find any subcell !!!"); // exception ?
			  } 
		      } else {
			ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2+1]=0;
		      }
		      delete [] CellsList;
		    }

		  delete [] NodesLists;
		}
	      }
	}
      // we adjust _constituent
      int NumberOfConstituent=0;
      int SizeOfConstituentNodal=0;
      for (int i=0;i<_constituent->_numberOfTypes; i++) {
	NumberOfConstituent += tmp_NumberOfConstituentsForeachType[i]-_constituent->_count[i]+1;
	SizeOfConstituentNodal += (tmp_NumberOfConstituentsForeachType[i]-_constituent->_count[i]+1)*_constituent->_type[i].getNumberOfNodes();
      }
      // we built new _nodal attribute in _constituent
      //MEDSKYLINEARRAY *ConstituentNodal = new MEDSKYLINEARRAY(NumberOfConstituent,SizeOfConstituentNodal);
      //const int *ConstituentNodalValue = ConstituentNodal->getValue();
      //const int *ConstituentNodalIndex = ConstituentNodal->getIndex();
//CCRT      int *ConstituentNodalValue = new int[SizeOfConstituentNodal];
      med_int *ConstituentNodalValue = new med_int[SizeOfConstituentNodal];
//CCRT      int *ConstituentNodalIndex = new int[NumberOfConstituent+1];
      med_int *ConstituentNodalIndex = new med_int[NumberOfConstituent+1];
      ConstituentNodalIndex[0]=1;
      // we build _reverseDescendingConnectivity 
      //_reverseDescendingConnectivity = new MEDSKYLINEARRAY(NumberOfConstituent,2*NumberOfConstituent);
      //const int *reverseDescendingConnectivityValue = _reverseDescendingConnectivity->getValue();
      //const int *reverseDescendingConnectivityIndex = _reverseDescendingConnectivity->getIndex();
//CCRT      int *reverseDescendingConnectivityValue = new int[2*NumberOfConstituent];
      med_int *reverseDescendingConnectivityValue = new med_int[2*NumberOfConstituent];
//CCRT      int *reverseDescendingConnectivityIndex = new int[NumberOfConstituent+1];
      med_int *reverseDescendingConnectivityIndex = new med_int[NumberOfConstituent+1];
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
	int offset = _constituent->_count[1]-tmp_NumberOfConstituentsForeachType[0]-1;
	int offset1=offset*_constituent->_type[0].getNumberOfNodes();
	int offset2=offset*2;
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
	_constituent->_count[2]=NumberOfConstituent+1;
	// we correct _descending to adjust face number
	for(int j=0;j<DescendingSize;j++)
	  if (descend_connectivity[j]>tmp_NumberOfConstituentsForeachType[0])
	    descend_connectivity[j]-=offset;
      }

      delete [] ConstituentNodalConnectivityIndex;
      delete [] ConstituentNodalConnectivity;

      _descending = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,
					DescendingSize,
					descend_connectivity_index,
					descend_connectivity);
      delete [] descend_connectivity_index;
      delete [] descend_connectivity;
      _reverseDescendingConnectivity = new MEDSKYLINEARRAY(NumberOfConstituent,
							   2*NumberOfConstituent,
							   reverseDescendingConnectivityIndex,
							   reverseDescendingConnectivityValue);
      delete [] reverseDescendingConnectivityIndex;
      delete [] reverseDescendingConnectivityValue;

      _constituent->_count[1]=tmp_NumberOfConstituentsForeachType[0]+1;
      delete [] tmp_NumberOfConstituentsForeachType;
      
      //delete _constituent->_nodal;
      _constituent->_nodal = new MEDSKYLINEARRAY(NumberOfConstituent,
						 SizeOfConstituentNodal,
						 ConstituentNodalIndex,
						 ConstituentNodalValue);
    
      delete [] ConstituentNodalIndex;
      delete [] ConstituentNodalValue;

      delete [] ReverseDescendingConnectivityValue;
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
//    return;
//  }

/*! Not implemented yet */
//--------------------------------------------------------------------//
void CONNECTIVITY::calculateNeighbourhood(CONNECTIVITY &myConnectivity)
//--------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::calculateNeighbourhood(CONNECTIVITY &myConnectivity) : ";
  BEGIN_OF(LOC);

  MESSAGE(LOC<<"method not yet implemented " << myConnectivity._entity);
  // Mesh dimension !

  END_OF(LOC);
  return;
}


/*! 
  Returns the geometry of an element given by its entity type & its global number.

  Example : medGeometryElement myType = myConnectivity.getElementType(MED_CELL,35);
*/
//--------------------------------------------------------------------//
medGeometryElement CONNECTIVITY::getElementType(medEntityMesh Entity,int globalNumber) const
//--------------------------------------------------------------------//
{
  const char * LOC = "medGeometryElement CONNECTIVITY::getElementType(medEntityMesh Entity, int globalNumber) const : ";
  BEGIN_OF(LOC);
  int globalNumberMin = 1;
  int globalNumberMax ;

  if (_entity==Entity) globalNumberMax = _count[_numberOfTypes];
  else if (_constituent!=NULL) globalNumberMax = _constituent->_count[_constituent->_numberOfTypes];
  else
    throw MEDEXCEPTION(LOCALIZED("getElementType : Entity not defined !"));

  // The globalNumber must verify  : 1 <=  globalNumber < _count[_numberOfTypes] (== totalNumberOfElement+1)

  if ( (globalNumber < globalNumberMin) || (globalNumber >  globalNumberMax-1 )  )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "globalNumber must be between >= |" 
                                 << globalNumberMin <<"| and <= |" << globalNumberMax-1 ));

  if (_entity==Entity) {
    for(int i=1; i<=_numberOfTypes;i++)
      if (globalNumber<_count[i])
	return _geometricTypes[i-1];
  }
  else if (_constituent!=NULL)
    return _constituent->getElementType(Entity,globalNumber);
  else
    throw MEDEXCEPTION(LOCALIZED("getElementType : Entity not defined !"));
  throw MEDEXCEPTION(LOCALIZED("getElementType : Wrong Number !"));

  END_OF(LOC);
}


ostream & MEDMEM::operator<<(ostream &os, CONNECTIVITY &co)
{
    os << endl << "------------- Entity = ";
    switch (co._entity)
    {
	case MED_CELL:
	    os << "MED_CELL";
	    break;
	case MED_FACE:
	    os << "MED_FACE";
	    break;
	case MED_EDGE:
	    os << "MED_EDGE";
	    break;
	case MED_NODE:
	    os << "MED_NODE";
	    break;
	case MED_ALL_ENTITIES:
	    os << "MED_ALL_ENTITIES";
	    break;
	default:
	    os << "Unknown";
	    break;
    } 
    os  << " -------------\n\nMedConnectivity : ";
    switch (co._typeConnectivity)
    {
	case MED_NODAL:
	    os << "MED_NODAL\n";
	    break;
	case MED_DESCENDING:
	    os << "MED_DESCENDING\n";
	    break;
	default:
	    break;
    } 
    os << "Entity dimension : " << co._entityDimension << endl;
    os << "Number of nodes : " << co._numberOfNodes << endl;
    os << "Number of types : " << co._numberOfTypes << endl;
    for (int i=0; i!=co._numberOfTypes ; ++i)
	os << "  -> Type " << co._geometricTypes[i] << " (" << co._type[i].getName() << ") : " 
	   << co._count[i+1]-co._count[i] << " elements" << endl;

    if (co._typeConnectivity == MED_NODAL )
    {
	for (int i=0; i<co._numberOfTypes; i++) 
	{
	    os << endl << co._type[i].getName() << " : " << endl;
	    int numberofelements = co._count[i+1]-co._count[i];
	    const med_int * connectivity = co.getConnectivity(co._typeConnectivity, co._entity, co._geometricTypes[i]);
	    int numberofnodespercell = co._geometricTypes[i]%100;
	    for (int j=0;j<numberofelements;j++)
	    {
		os << setw(6) << j+1 << " : ";
		for (int k=0;k<numberofnodespercell;k++)
		    os << connectivity[j*numberofnodespercell+k]<<" ";
		os << endl;
	    }
	}
    }
    else if (co._typeConnectivity == MED_DESCENDING)
    {
	int numberofelements = co.getNumberOf( co._entity , MED_ALL_ELEMENTS);
	const med_int *connectivity =  co.getConnectivity( co._typeConnectivity, co._entity, MED_ALL_ELEMENTS);
//CCRT	const int *connectivity_index =  co.getConnectivityIndex( co._typeConnectivity, co._entity );
	const med_int *connectivity_index =  co.getConnectivityIndex( co._typeConnectivity, co._entity );
	
	for ( int j=0; j!=numberofelements; j++ ) 
	{
	    os << "Element " << j+1 << " : ";
	    for ( int k=connectivity_index[j]; k!=connectivity_index[j+1]; k++ )
		os << connectivity[k-1] << " ";
	    os << endl;
	}
    }

    if (co._constituent)
	os << endl << *co._constituent << endl;

    return os;
}
