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
using namespace MED_EN;

// Enlarge the vector if necessary to insert the element
static inline void insert_vector(vector<int> &Vect, int Indice, int Element)
{
  if (Indice >= Vect.capacity())
    Vect.reserve(Indice + 1000);

  if (Indice >= Vect.size())
    Vect.resize(Indice+1);

  Vect[Indice] = Element;
}

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
				_numberOfNodes(0),
  				_count((int*)NULL),
  				_nodal((MEDSKYLINEARRAY*)NULL),
				_polygonsNodal((MEDSKYLINEARRAY*)NULL),
				_polyhedronNodal((POLYHEDRONARRAY*)NULL),
  				_descending((MEDSKYLINEARRAY*)NULL),
				_polygonsDescending((MEDSKYLINEARRAY*)NULL),
				_polyhedronDescending((MEDSKYLINEARRAY*)NULL),
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
				_numberOfNodes(0),
  				_nodal((MEDSKYLINEARRAY*)NULL),
				_polygonsNodal((MEDSKYLINEARRAY*)NULL),
				_polyhedronNodal((POLYHEDRONARRAY*)NULL),
  				_descending((MEDSKYLINEARRAY*)NULL),
				_polygonsDescending((MEDSKYLINEARRAY*)NULL),
				_polyhedronDescending((MEDSKYLINEARRAY*)NULL),
  				_reverseNodalConnectivity((MEDSKYLINEARRAY*)NULL),
  				_reverseDescendingConnectivity((MEDSKYLINEARRAY*)NULL),
  				_neighbourhood((MEDSKYLINEARRAY*)NULL),
  				_constituent((CONNECTIVITY*)NULL)
{
  MESSAGE("CONNECTIVITY(int numberOfTypes,medEntityMesh Entity=MED_CELL)");
  _geometricTypes = new medGeometryElement[numberOfTypes];
  _type = new CELLMODEL[numberOfTypes];
  _count = new int[numberOfTypes+1];
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
      _count = new int[_numberOfTypes+1];
      memcpy(_count,m._count,(_numberOfTypes+1)*sizeof(int));
 }
 else
    _count = (int *) NULL;

 if (m._nodal != NULL)
    _nodal = new MEDSKYLINEARRAY(* m._nodal);
 else
    _nodal = (MEDSKYLINEARRAY *) NULL;

 if (m._polygonsNodal != NULL)
   _polygonsNodal = new MEDSKYLINEARRAY(* m._polygonsNodal);
 else
   _polygonsNodal = (MEDSKYLINEARRAY *) NULL;

 if (m._polyhedronNodal != NULL)
   _polyhedronNodal = new POLYHEDRONARRAY(* m._polyhedronNodal);
 else
   _polyhedronNodal = (POLYHEDRONARRAY *) NULL;

 if (m._descending != NULL)
    _descending = new MEDSKYLINEARRAY(* m._descending);
 else
    _descending = (MEDSKYLINEARRAY *) NULL;

 if (m._polygonsDescending != NULL)
   _polygonsDescending = new MEDSKYLINEARRAY(* m._polygonsDescending);
 else
   _polygonsDescending = (MEDSKYLINEARRAY *) NULL;

 if (m._polyhedronDescending != NULL)
   _polyhedronDescending = new MEDSKYLINEARRAY(* m._polyhedronDescending);
 else
   _polyhedronDescending = (MEDSKYLINEARRAY *) NULL;

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
  if (_polygonsNodal != NULL)
     delete _polygonsNodal;
  if (_polyhedronNodal != NULL)
     delete _polyhedronNodal;
  if (_descending != NULL)
     delete _descending;
  if (_polygonsDescending != NULL)
     delete _polygonsDescending;
  if (_polyhedronDescending != NULL)
    delete _polyhedronDescending;
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
void CONNECTIVITY::setCount(const int * Count, const medEntityMesh Entity)
  			    throw (MEDEXCEPTION)
//--------------------------------------------------------------------//
{
  if (Entity == _entity)
  {
    int * index = new int[Count[_numberOfTypes]];
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
void CONNECTIVITY::setNodal(const int * Connectivity,
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


//--------------------------------------------------------------------//
void CONNECTIVITY::setPolygonsConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity, const int* PolygonsConnectivity, const int* PolygonsConnectivityIndex, int ConnectivitySize, int NumberOfPolygons)
//--------------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::setPolygonsConnectivity";
  BEGIN_OF(LOC);

  if (_entity == Entity)
    {
      MEDSKYLINEARRAY* Connectivity = new MEDSKYLINEARRAY(NumberOfPolygons,ConnectivitySize,PolygonsConnectivityIndex,PolygonsConnectivity);

      if (ConnectivityType == MED_NODAL)
	{
	  if (_polygonsNodal != (MEDSKYLINEARRAY*) NULL)
	    delete _polygonsNodal;
	  _polygonsNodal = Connectivity;
	}
      else
	{
	  if (_typeConnectivity != MED_DESCENDING)
	    _typeConnectivity = MED_DESCENDING; //by default it is set to MED_NODAL
	  if (_polygonsDescending != (MEDSKYLINEARRAY*) NULL)
	    delete _polygonsDescending;
	  _polygonsDescending = Connectivity;
	}
    }
  else
    {
      if (_constituent == (CONNECTIVITY*) NULL)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not found !"));
      _constituent->setPolygonsConnectivity(ConnectivityType, Entity, PolygonsConnectivity, PolygonsConnectivityIndex, ConnectivitySize, NumberOfPolygons);
    }
}


//--------------------------------------------------------------------//
void CONNECTIVITY::setPolyhedronConnectivity(medConnectivity ConnectivityType, const int* PolyhedronConnectivity, const int* PolyhedronIndex, int ConnectivitySize, int NumberOfPolyhedron, const int* PolyhedronFacesIndex /* =(int*)NULL */, int NumberOfFaces /* =0 */)
//--------------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::setPolyhedronConnectivity";
  BEGIN_OF(LOC);

  if (_entity == MED_CELL)
    {
      if (ConnectivityType == MED_NODAL)
	{
	  if (_polyhedronNodal != (POLYHEDRONARRAY*) NULL)
	    delete _polyhedronNodal;
	  _polyhedronNodal = new POLYHEDRONARRAY(NumberOfPolyhedron,NumberOfFaces,ConnectivitySize);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
          int i ;
          MED_EN::med_int * tmp_PolyhedronIndex = new med_int[NumberOfPolyhedron+1] ;
          for ( i = 0 ; i < NumberOfPolyhedron+1 ; i++ )
             tmp_PolyhedronIndex[i] = PolyhedronIndex[i] ;
	  _polyhedronNodal->setPolyhedronIndex(tmp_PolyhedronIndex);
          delete [] tmp_PolyhedronIndex ;
          MED_EN::med_int * tmp_PolyhedronFacesIndex = new med_int[NumberOfFaces+1] ;
          for ( i = 0 ; i < NumberOfFaces+1 ; i++ )
             tmp_PolyhedronFacesIndex[i] = PolyhedronFacesIndex[i] ;
	  _polyhedronNodal->setFacesIndex(tmp_PolyhedronFacesIndex);
          delete [] tmp_PolyhedronFacesIndex ;
          MED_EN::med_int * tmp_PolyhedronConnectivity = new med_int[ConnectivitySize] ;
          for ( i = 0 ; i < ConnectivitySize ; i++ )
             tmp_PolyhedronConnectivity[i] = PolyhedronConnectivity[i] ;
	  _polyhedronNodal->setNodes(tmp_PolyhedronConnectivity);
          delete [] tmp_PolyhedronConnectivity ;
#else
	  _polyhedronNodal->setPolyhedronIndex(PolyhedronIndex);
	  _polyhedronNodal->setFacesIndex(PolyhedronFacesIndex);
	  _polyhedronNodal->setNodes(PolyhedronConnectivity);
#endif
	}
      else
	{
	  if (_typeConnectivity != MED_DESCENDING)
	    _typeConnectivity = MED_DESCENDING; //by default it is set to MED_NODAL
	  if (_polyhedronDescending != (MEDSKYLINEARRAY*) NULL)
	    delete _polyhedronDescending;
	  _polyhedronDescending = new MEDSKYLINEARRAY(NumberOfPolyhedron,ConnectivitySize,PolyhedronIndex,PolyhedronConnectivity);
	}
    }
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : _entity must be MED_CELL to set polyhedron !"));
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
void CONNECTIVITY::updateFamily(const vector<FAMILY*>& myFamilies)
//------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::updateFamily(vector<FAMILY*>) ";
  int numberOfFamilies = myFamilies.size();
  if (numberOfFamilies == 0 || _constituent == NULL)
    return;
  // does we do an update ?
  if ((_constituent != NULL) && (_descending != NULL))
    return;
  if (myFamilies[0]->getEntity() != _constituent->getEntity())
    return;
  CONNECTIVITY * oldConstituent = _constituent;
  _constituent = (CONNECTIVITY *)NULL;
  if (oldConstituent->_nodal==NULL)
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have no nodal connectivity of sub cell"));

  //Loc vars defined to treat polygons exactly the same as classic types. Not nice but necessary.
  int oldNumberOfFaceTab[2];
  const int * oldConstituentValueTab[2];
  const int * oldConstituentIndexTab[2];
  int * renumberingFromOldToNewTab[2];//Final mapping array between old numbers and new numbers.;

  int oldNumberOfFace = oldConstituent->_nodal->getNumberOf(); oldNumberOfFaceTab[0]=oldNumberOfFace;
  const int * oldConstituentValue = oldConstituent->_nodal->getValue(); oldConstituentValueTab[0]=oldConstituentValue;
  const int * oldConstituentIndex = oldConstituent->_nodal->getIndex(); oldConstituentIndexTab[0]=oldConstituentIndex;
  int * renumberingFromOldToNew= new int [oldNumberOfFace]; renumberingFromOldToNewTab[0]=renumberingFromOldToNew;

  int oldNumberOfFacePoly = oldConstituent->getNumberOfPolygons();
  const int * oldConstituentValuePoly=0;
  const int * oldConstituentIndexPoly=0;
  int * renumberingFromOldToNewPoly=0;

  int nbOfTurnInGlobalLoop=1;//Defined to know if a second search on polygons is needed.
  if(oldNumberOfFacePoly>0)
    {
      oldNumberOfFaceTab[1]=oldNumberOfFacePoly;
      oldConstituentValuePoly = oldConstituent->_polygonsNodal->getValue(); oldConstituentValueTab[1]=oldConstituentValuePoly;
      oldConstituentIndexPoly = oldConstituent->_polygonsNodal->getIndex(); oldConstituentIndexTab[1]=oldConstituentIndexPoly;
      renumberingFromOldToNewPoly=new int[oldNumberOfFacePoly]; renumberingFromOldToNewTab[1]=renumberingFromOldToNewPoly;
      nbOfTurnInGlobalLoop++;
    }

  calculateDescendingConnectivity();//perform calculation of descending connectivity to update all connectivities after having taken into account of direction of faces...
  _constituent->calculateReverseNodalConnectivity();//getting d-1 nodal connectivity to find get new face numbers from nodes numbers...

  const int * reverseFaceNodal = _constituent->getReverseNodalConnectivity(); //Common to polygons and classic geometric types
  const int * reverseFaceNodalIndex = _constituent->getReverseNodalConnectivityIndex(); //Common to polygons and classic geometric types

  for(int loop=0;loop<nbOfTurnInGlobalLoop;loop++)
    {
      int oldNumberOfFaceLoop=oldNumberOfFaceTab[loop];
      const int * oldConstituentValueLoop=oldConstituentValueTab[loop];
      const int * oldConstituentIndexLoop= oldConstituentIndexTab[loop];
      int * renumberingFromOldToNewLoop=renumberingFromOldToNewTab[loop];
      CELLMODEL * aCELLMODEL = 0;
      if ( loop == 0 ) aCELLMODEL = & oldConstituent->_type[0];
      for(int iOldFace=0;iOldFace<oldNumberOfFaceLoop;iOldFace++)
	{
	  const int *nodesOfCurrentFaceOld=oldConstituentValueLoop+oldConstituentIndexLoop[iOldFace]-1;
	  int nbOfNodesOfCurrentFaceOld=oldConstituentIndexLoop[iOldFace+1]-oldConstituentIndexLoop[iOldFace];

	  //retrieving new number of polygon face...
	  int sizeOfNewFaceNb=reverseFaceNodalIndex[nodesOfCurrentFaceOld[0]]-reverseFaceNodalIndex[nodesOfCurrentFaceOld[0]-1];
	  int *newFaceNb=new int[sizeOfNewFaceNb];
	  memcpy(newFaceNb,reverseFaceNodal+reverseFaceNodalIndex[nodesOfCurrentFaceOld[0]-1]-1,sizeOfNewFaceNb*sizeof(int));//all faces candidates are in newFaceNb !!!
	  for(int curNode=1;curNode<nbOfNodesOfCurrentFaceOld && sizeOfNewFaceNb>1;curNode++)
	    {
	      const int *newFacesNbForCurNode=reverseFaceNodal+reverseFaceNodalIndex[nodesOfCurrentFaceOld[curNode]-1]-1;
	      int sizeOfNewFacesNbForCurNode=reverseFaceNodalIndex[nodesOfCurrentFaceOld[curNode]]-reverseFaceNodalIndex[nodesOfCurrentFaceOld[curNode]-1];
	      for(int i=0;i<sizeOfNewFaceNb;)
		{
		  bool found=false;
		  for(int j=0;j<sizeOfNewFacesNbForCurNode && !found;j++)
		    {
		      if(newFacesNbForCurNode[j]==newFaceNb[i])
			found=true;
		    }
		  if(found)
		    i++;
		  else
		    newFaceNb[i]=newFaceNb[--sizeOfNewFaceNb];
		}
	    }
	  if(sizeOfNewFaceNb!=1)
	    throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"unexisting face specified!!!"));
	  renumberingFromOldToNewLoop[iOldFace]=newFaceNb[0];
	  delete [] newFaceNb;
	  //end of retrieving new number of polygon face...
	  int nbOfNodesOfCurrentFaceNew;
	  const int *nodesOfCurrentFaceNew=_constituent->getConnectivityOfAnElementWithPoly(MED_NODAL,_constituent->getEntity(),
											    renumberingFromOldToNewLoop[iOldFace],nbOfNodesOfCurrentFaceNew);
          // compare nodes of a new face and those of an old one;
          // for the second order elements, only vertex nodes are compared
          int nbOfVertices = nbOfNodesOfCurrentFaceOld;
          if ( aCELLMODEL ) {
            if ( aCELLMODEL->getNumberOfNodes() != nbOfNodesOfCurrentFaceOld ) {
              // type changed, find a corresponding CELLMODEL
              int iType = 2; // 1-st type is already used at loop beginning
              while ( iOldFace + 1 >= oldConstituent->_count[ 1 + iType ]) // check next type
                ++iType;
              aCELLMODEL = & oldConstituent->_type[ iType - 1 ];
            }
            nbOfVertices = aCELLMODEL->getNumberOfVertexes();
          }
	  MEDMODULUSARRAY modulusArrayOld(nbOfVertices,nbOfNodesOfCurrentFaceOld,nodesOfCurrentFaceOld);
	  MEDMODULUSARRAY modulusArrayNew(nbOfVertices,nbOfNodesOfCurrentFaceNew,nodesOfCurrentFaceNew);
	  int retCompareNewOld=modulusArrayNew.compare(modulusArrayOld);
	  if(retCompareNewOld==0)
	    throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"Uncompatible given user face with calculated existing faces"));
	  if(retCompareNewOld==-1)
	    invertConnectivityForAFace(renumberingFromOldToNewLoop[iOldFace],nodesOfCurrentFaceOld,loop==1);
	}
    }
  // Updating the Family
  for(vector<FAMILY*>::const_iterator iter=myFamilies.begin();iter!=myFamilies.end();iter++)
    (*iter)->changeElementsNbs(_constituent->getEntity(),renumberingFromOldToNew,oldNumberOfFace,renumberingFromOldToNewPoly);
  delete oldConstituent ;
  delete [] renumberingFromOldToNew;
  if(oldNumberOfFacePoly>0)
    delete [] renumberingFromOldToNewPoly;
  return;
}

//------------------------------------------------------------------------------------------------------------------//
const int * MEDMEM::CONNECTIVITY::getConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type)
//------------------------------------------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getConnectivity";
  //  BEGIN_OF(LOC);

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
      if (Type==MED_ALL_ELEMENTS)
	return Connectivity->getValue();
      else {
	for (int i=0; i<_numberOfTypes; i++)
	  if (_geometricTypes[i]==Type)
	    //return Connectivity->getI(i+1);
	    return Connectivity->getI(_count[i]);
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Type not found !"));
      }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !"));
  } else
    if (_constituent != NULL)
      return _constituent->getConnectivity(ConnectivityType,Entity,Type);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}

//------------------------------------------------------------------------------------------------------------------//
int CONNECTIVITY::getConnectivityLength(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type)
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
      if (Type==MED_ALL_ELEMENTS)
	return Connectivity->getLength();
      else {
	for (int i=0; i<_numberOfTypes; i++)
	  if (_geometricTypes[i]==Type)
	    return Connectivity->getNumberOfI(_count[i]);
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Type not found !"));
      }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !"));
  }
  else
    if (_constituent != NULL)
      return _constituent->getConnectivityLength(ConnectivityType,Entity,Type);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}

/*!  Give morse index array to use with
     getConnectivity(MED_FULL_INTERLACE,mode,entity,MED_ALL_ELEMENTS).

      Each value give start index for corresponding entity in connectivity array.

      Example : i-th element, j-th node of it :
      - In C mode : Connectivity[ConnectivityIndex[i]-1+j-1]
      - In fortran mode : Connectivity[ConnectivityIndex[i]+j] */
//-----------------------------------------------------------------------------------------------//
const int * CONNECTIVITY::getConnectivityIndex(medConnectivity ConnectivityType, medEntityMesh Entity)
//-----------------------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getConnectivityIndex";

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


//-------------------------------------------------------------//
const int* CONNECTIVITY::getPolygonsConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity)
//-------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::getPolygonsConnectivity";
  BEGIN_OF(LOC);

  MEDSKYLINEARRAY* Connectivity;
  if (Entity == _entity)
    {
      if (ConnectivityType == MED_NODAL)
	{
	  calculateNodalConnectivity();
	  Connectivity = _polygonsNodal;
	}
      else
	{
	  calculateDescendingConnectivity();
	  Connectivity = _polygonsDescending;
	}
      if (Connectivity != NULL)
	return Connectivity->getValue();
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Polygons Connectivity not defined !"));
    }
  else
    if (_constituent != NULL)
      return _constituent->getPolygonsConnectivity(ConnectivityType, Entity);
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}


//-------------------------------------------------------------//
const int* CONNECTIVITY::getPolygonsConnectivityIndex(medConnectivity ConnectivityType, medEntityMesh Entity)
//-------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::getPolygonsConnectivityIndex";
  BEGIN_OF(LOC);

  MEDSKYLINEARRAY* Connectivity;
  if (Entity == _entity)
    {
      if (ConnectivityType == MED_NODAL)
	{
	  //	  calculateNodalConnectivity();
	  Connectivity = _polygonsNodal;
	}
      else
	{
	  //	  calculateDescendingConnectivity();
	  Connectivity = _polygonsDescending;
	}
      if (Connectivity != NULL)
	return Connectivity->getIndex();
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Polygons Connectivity not defined !"));
    }
  else
    if (_constituent != NULL)
      return _constituent->getPolygonsConnectivityIndex(ConnectivityType, Entity);
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}


/*! We suppose in this method that nodal and descending connectivities
  are coherent.*/
//-------------------------------------------------------------//
int CONNECTIVITY::getNumberOfPolygons(MED_EN::medEntityMesh Entity) const
//-------------------------------------------------------------//
{
  if(Entity==MED_ALL_ENTITIES || Entity==_entity )
    {
      if (_polygonsNodal != (MEDSKYLINEARRAY*) NULL)
	return _polygonsNodal->getNumberOf();
      else if (_polygonsDescending != (MEDSKYLINEARRAY*) NULL)
	return _polygonsDescending->getNumberOf();
      else
	return 0;
    }
  else
    {
      if (_constituent == (CONNECTIVITY*) NULL)
	throw MEDEXCEPTION("getNumberOfPolygons : Entity not found !");
      return _constituent->getNumberOfPolygons(Entity);
    }
}


//--------------------------------------------------------------//
const int* CONNECTIVITY::getPolyhedronConnectivity(medConnectivity ConnectivityType) const
//--------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::getPolyhedronConnectivity";
  BEGIN_OF(LOC);

  if (_entity == MED_CELL) //polyhedron can only be MED_CELL
    {
      if (ConnectivityType == MED_NODAL)
	{
	  ((CONNECTIVITY *)(this))->calculateNodalConnectivity();
	  if (_polyhedronNodal != (POLYHEDRONARRAY*) NULL) {
//CCRT
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
            int i ;
            const MED_EN::med_int * tmp_PolyhedronConnectivity = _polyhedronNodal->getNodes();
            int * PolyhedronConnectivity = new int[_polyhedronNodal->getNumberOfNodes()] ;
            for ( i = 0 ; i < _polyhedronNodal->getNumberOfNodes() ; i++ )
               PolyhedronConnectivity[i] = tmp_PolyhedronConnectivity[i] ;
//CCRT : return of a copy of PolyhedronConnectivity
            return PolyhedronConnectivity ;
#else
	    return _polyhedronNodal->getNodes();
#endif
          }
	  else
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Polyhedron Nodal Connectivity not defined !"));
	}
      else
	{
	  ((CONNECTIVITY *)(this))->calculateDescendingConnectivity();
	  if (_polyhedronDescending != (MEDSKYLINEARRAY*) NULL) {
	    return _polyhedronDescending->getValue();
          }
	  else
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Polyhedron Descending Connectivity not defined !"));
	}
    }
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : No polyhedron for _entity different from MED_CELL !"));
}


//---------------------------------------------------------------//
const int* CONNECTIVITY::getPolyhedronFacesIndex() const
//---------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::getPolyhedronFacesIndex";
  BEGIN_OF(LOC);

  if (_entity == MED_CELL) //polyhedron can only be MED_CELL
    {
      //      calculateNodalConnectivity();
      if (_polyhedronNodal != (POLYHEDRONARRAY*) NULL) {
//CCRT
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
            int i ;
            const MED_EN::med_int * tmp_PolyhedronFacesIndex = _polyhedronNodal->getFacesIndex();
            int * PolyhedronFacesIndex = new int[_polyhedronNodal->getNumberOfFaces()+1] ;
            for ( i = 0 ; i < _polyhedronNodal->getNumberOfFaces()+1 ; i++ )
               PolyhedronFacesIndex[i] = tmp_PolyhedronFacesIndex[i] ;
//CCRT : return of a copy of PolyhedronFacesIndex
            return PolyhedronFacesIndex ;
#else
	return _polyhedronNodal->getFacesIndex();
#endif
      }
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : No Polyhedron in that Connectivity !"));
    }
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : No polyhedron for _entity different from MED_CELL !"));
}


//--------------------------------------------------------------//
const int* CONNECTIVITY::getPolyhedronIndex(medConnectivity ConnectivityType) const
//--------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::getPolyhedronIndex";
  BEGIN_OF(LOC);

  if (_entity == MED_CELL) //polyhedron can only be MED_CELL
    {
      if (ConnectivityType == MED_NODAL)
	{
	  //	  calculateNodalConnectivity();
	  if (_polyhedronNodal != (POLYHEDRONARRAY*) NULL) {
//CCRT
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
            int i ;
            const MED_EN::med_int * tmp_PolyhedronIndex = _polyhedronNodal->getPolyhedronIndex();
            int * PolyhedronIndex = new int[_polyhedronNodal->getNumberOfPolyhedron()+1] ;
            for ( i = 0 ; i < _polyhedronNodal->getNumberOfPolyhedron()+1 ; i++ )
               PolyhedronIndex[i] = tmp_PolyhedronIndex[i] ;
//CCRT : return of a copy of PolyhedronIndex
            return PolyhedronIndex ;
#else
	    return _polyhedronNodal->getPolyhedronIndex();
#endif
          }
	  else
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Polyhedron Nodal Connectivity not defined !"));
	}
      else
	{
	  //	  calculateDescendingConnectivity();
	  if (_polyhedronDescending != (MEDSKYLINEARRAY*) NULL)
	    return _polyhedronDescending->getIndex();
	  else
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Polyhedron Descending Connectivity not defined !"));
	}
    }
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : No polyhedron for _entity different from MED_CELL !"));
}


/*! We suppose in this method that nodal and descending connectivities
  are coherent.*/
//-------------------------------------------------------------//
int CONNECTIVITY::getNumberOfPolyhedronFaces() const
//-------------------------------------------------------------//
{
  //  if (_polyhedronNodal == (POLYHEDRONARRAY*) NULL)
  //    calculateNodalConnectivity();
  if (_polyhedronNodal != (POLYHEDRONARRAY*) NULL)
    return _polyhedronNodal->getNumberOfFaces();
  else
    return 0;
}


/*! We suppose in this method that nodal and descending connectivities
  are coherent.*/
//--------------------------------------------------------------//
int CONNECTIVITY::getNumberOfPolyhedron() const
//--------------------------------------------------------------//
{
  if (_polyhedronNodal != (POLYHEDRONARRAY*) NULL)
    return _polyhedronNodal->getNumberOfPolyhedron();
  else if (_polyhedronDescending != (MEDSKYLINEARRAY*) NULL)
    return _polyhedronDescending->getNumberOf();
  else
    return 0;
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
  for (int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i];
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
}

/*!  Returns the number of elements of type %medGeometryElement.
     Note : not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
int CONNECTIVITY::getNumberOfNodesInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getNumberOfNodesInType";
  BEGIN_OF(LOC);

  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!"));
  for (int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i].getNumberOfNodes();
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
}

/*!  Returns the number of geometric sub cells of %medGeometryElement type.
not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
int CONNECTIVITY::getNumberOfSubCellInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_NONE))
    throw MEDEXCEPTION("CONNECTIVITY::getNumberOfSubCell : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!");
  for (int i=0; i<_numberOfTypes; i++)
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
int CONNECTIVITY::getNumberOf(medEntityMesh Entity, medGeometryElement Type) const
//-----------------------------------------------------------------------------------//
{
  //const char * LOC = "CONNECTIVITY::getNumberOf";
  if (Entity==_entity) {
    if (Type==MED_NONE)
      return 0; // not defined !
    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_NONE"));
    if (!existConnectivity(MED_NODAL,Entity) && !existConnectivity(MED_DESCENDING,Entity)) return 0; //case with only polygons for example
    if (Type==MED_ALL_ELEMENTS)
      return _count[_numberOfTypes]-1;
    for (int i=0; i<_numberOfTypes; i++)
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
const int* CONNECTIVITY::getValue(medConnectivity TypeConnectivity,
				medGeometryElement Type)
//--------------------------------------------------------------//
{
  if (TypeConnectivity == MED_NODAL)
    {
	  calculateNodalConnectivity();
	  if (Type==MED_ALL_ELEMENTS)
	    return _nodal->getValue();
	  for (int i=0; i<_numberOfTypes; i++)
	    if (_geometricTypes[i]==Type)
	      return _nodal->getI(_count[i]);
    }
  else
    {
      calculateDescendingConnectivity();
      if (Type==MED_ALL_ELEMENTS)
	return _descending->getValue();
      for (int i=0; i<_numberOfTypes; i++)
	if (_geometricTypes[i]==Type)
	  return _descending->getI(Type);
    }
  throw MEDEXCEPTION("Not found");
}

/*! A DOCUMENTER */
//---------------------------------------------------------------------//
const int* CONNECTIVITY:: getValueIndex(medConnectivity TypeConnectivity)
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
const int* CONNECTIVITY:: getNeighbourhood() const
//----------------------------------------------//
{
  throw MEDEXCEPTION("CONNECTIVITY::getNeighbourhood : Not implemented");
}

/*! Returns an array which contains, for each node, all cells
    arround it. */
//-------------------------------------------------//
const int* CONNECTIVITY::getReverseNodalConnectivity()
//-------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getValue();
}

/*!  Give index array to use with getReverseConnectivity(MED_NODAL).
     It is unusefull with MED_DESCENDING mode, because we have allways two cells.  */
//-------------------------------------------------------//
const int* CONNECTIVITY::getReverseNodalConnectivityIndex()
//-------------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getIndex();
}

/*! Returns an array which contains, for each face (or edge),
    the 2 cells of each side. First is cell which face normal is outgoing.
    arround it. */
//------------------------------------------------------//
const int* CONNECTIVITY::getReverseDescendingConnectivity()
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
const int* CONNECTIVITY::getReverseDescendingConnectivityIndex()
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
  if (_nodal==NULL && _polygonsNodal==NULL && _polyhedronNodal==NULL)
    {
      if (_descending==NULL && _polygonsDescending==NULL && _polyhedronDescending==NULL)
	throw MEDEXCEPTION("CONNECTIVITY::calculateNodalConnectivity : No connectivity found !");
      // calculate _nodal, _polygonsNodal and _polyhedronNodal from _descending, _polygonsDescending and _polyhedronDescending
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

    int node_number = 0;
    vector <vector <int> > reverse_connectivity;
    reverse_connectivity.resize(_numberOfNodes+1);

  // Treat all cells types

    for (int j = 0; j < _numberOfTypes; j++)
      {
	// node number of the cell type
	node_number = _type[j].getNumberOfNodes();
	// treat all cells of a particular type
	for (int k = _count[j]; k < _count[j+1]; k++)
	  // treat all nodes of the cell type
	  for (int local_node_number = 1; local_node_number < node_number+1; local_node_number++)
	    {
	      int global_node = _nodal->getIJ(k,local_node_number);
	      reverse_connectivity[global_node].push_back(k);
	    }
      }

    if(_polygonsNodal != NULL && _entityDimension==2)
      {
	int nbOfPolygons=_polygonsNodal->getNumberOf();
	int offset=getNumberOf(_entity,MED_ALL_ELEMENTS);
	const int *index=_polygonsNodal->getIndex();
	const int *value=_polygonsNodal->getValue();
	for (int local_polyg_number = 0; local_polyg_number < nbOfPolygons; local_polyg_number++)
	  {
	    for(int i=index[local_polyg_number];i<index[local_polyg_number+1];i++)
	      reverse_connectivity[value[i-1]].push_back(offset+local_polyg_number+1);
	  }
      }

    if(_polyhedronNodal != NULL && _entityDimension==3)
      {
	int nbOfPolyhedra=_polyhedronNodal->getNumberOfPolyhedron();
	int offset=getNumberOf(_entity,MED_ALL_ELEMENTS);
	for (int local_polyh_number = 0; local_polyh_number < nbOfPolyhedra; local_polyh_number++)
	    {
	      int nbOfNodes;
	      int global_polyh_number=offset+local_polyh_number+1;
	      int *nodes=getNodesOfPolyhedron(global_polyh_number,nbOfNodes);
	      for(int i=0;i<nbOfNodes;i++)
		reverse_connectivity[nodes[i]].push_back(global_polyh_number);
	      delete [] nodes;
	    }
      }

    // Full reverse_nodal_connectivity and reverse_nodal_connectivity_index

  //calculate size of reverse_nodal_connectivity array
    int size_reverse_nodal_connectivity  = 0;
    for (int i = 1; i < _numberOfNodes+1; i++)
      size_reverse_nodal_connectivity += reverse_connectivity[i].size();

    int * reverse_nodal_connectivity_index = new int[_numberOfNodes+1];
    int * reverse_nodal_connectivity = new int[size_reverse_nodal_connectivity];

    reverse_nodal_connectivity_index[0] = 1;
    for (int i = 1; i < _numberOfNodes+1; i++)
      {
	int size = reverse_connectivity[i].size();
	reverse_nodal_connectivity_index[i] = reverse_nodal_connectivity_index[i-1] + size;
	for (int j = 0; j < size; j++)
	  reverse_nodal_connectivity[reverse_nodal_connectivity_index[i-1]-1+j]= reverse_connectivity[i][j];
      }

    _reverseNodalConnectivity = new MEDSKYLINEARRAY(_numberOfNodes,size_reverse_nodal_connectivity,
						    reverse_nodal_connectivity_index,
						    reverse_nodal_connectivity,true);
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

  if (_descending==NULL && _polygonsDescending==NULL && _polyhedronDescending==NULL)
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
      int * descend_connectivity = new int[DescendingSize];
      for (int i=0; i<DescendingSize; i++)
	descend_connectivity[i]=0;
      //const int * descend_connectivity_index = _descending->getIndex();
      int * descend_connectivity_index = new int[_count[_numberOfTypes]];
      descend_connectivity_index[0]=1;
      medGeometryElement* ConstituentsTypes = new medGeometryElement[2];
      ConstituentsTypes[0]=MED_NONE;
      ConstituentsTypes[1]=MED_NONE;
      int * NumberOfConstituentsForeachType = new int[2];
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
      int * ConstituentNodalConnectivity = new int[TotalNumberOfNodes];
      int * ConstituentNodalConnectivityIndex = new int[TotalNumberOfConstituents+1];
      ConstituentNodalConnectivityIndex[0]=1;

      _constituent->_entityDimension = _entityDimension-1;
      if (ConstituentsTypes[1]==MED_NONE)
	_constituent->_numberOfTypes = 1;
      else
	_constituent->_numberOfTypes = 2;
      _constituent->_geometricTypes = new medGeometryElement[_constituent->_numberOfTypes];
      _constituent->_type = new CELLMODEL[_constituent->_numberOfTypes];
//CCRT      _constituent->_count = new med_int[_constituent->_numberOfTypes+1];
      _constituent->_count = new int[_constituent->_numberOfTypes+1];
      _constituent->_count[0]=1;
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
      const int * ReverseNodalConnectivityIndex = _reverseNodalConnectivity->getIndex();
      const int * ReverseNodalConnectivityValue = _reverseNodalConnectivity->getValue();

      // array to keep reverse descending connectivity
      int * ReverseDescendingConnectivityValue = new int[TotalNumberOfConstituents*2];

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

		  int * NodesLists = new int[NumberOfNodesPerConstituent];
		  for (int l=0; l<NumberOfNodesPerConstituent; l++) {
		    NodesLists[l]=_nodal->getIJ(j,Type.getNodeConstituent(1,k,l+1));
		    ConstituentNodalConnectivity[ConstituentNodalConnectivityIndex[TotalNumberOfSubCell-1]-1+l]=NodesLists[l];
		  }
		  // we use reverse_nodal_connectivity to find the other element which contain this sub cell

		  // all elements which contains first node of sub cell :
		  int ReverseNodalConnectivityIndex_0 = ReverseNodalConnectivityIndex[NodesLists[0]-1];
		  int ReverseNodalConnectivityIndex_1 = getIndexOfEndClassicElementInReverseNodal(ReverseNodalConnectivityValue,ReverseNodalConnectivityIndex,NodesLists[0]-1);
		  //ReverseNodalConnectivityIndex[NodesLists[0]];
		  int NumberOfCellsInList = ReverseNodalConnectivityIndex_1-ReverseNodalConnectivityIndex_0;

		  if (NumberOfCellsInList > 0)
		    { // we could have no element !
		      int * CellsList = new int[NumberOfCellsInList];
		      for (int l=ReverseNodalConnectivityIndex_0; l<ReverseNodalConnectivityIndex_1; l++)
			CellsList[l-ReverseNodalConnectivityIndex_0]=ReverseNodalConnectivityValue[l-1];

		      // foreach node in sub cell, we search elements which are in common
		      // at the end, we must have only one !

		      for (int l=1; l<NumberOfNodesPerConstituent; l++) {
			int NewNumberOfCellsInList = 0;
			int * NewCellsList = new int[NumberOfCellsInList];
			for (int l1=0; l1<NumberOfCellsInList; l1++)
			  for (int l2=ReverseNodalConnectivityIndex[NodesLists[l]-1]; l2<getIndexOfEndClassicElementInReverseNodal(ReverseNodalConnectivityValue,ReverseNodalConnectivityIndex,NodesLists[l]-1); l2++)
			  //for (int l2=ReverseNodalConnectivityIndex[NodesLists[l]-1]; l2<ReverseNodalConnectivityIndex[NodesLists[l]]; l2++)
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
      int *ConstituentNodalValue = new int[SizeOfConstituentNodal];
      int *ConstituentNodalIndex = new int[NumberOfConstituent+1];
      ConstituentNodalIndex[0]=1;
      // we build _reverseDescendingConnectivity
      //_reverseDescendingConnectivity = new MEDSKYLINEARRAY(NumberOfConstituent,2*NumberOfConstituent);
      //const int *reverseDescendingConnectivityValue = _reverseDescendingConnectivity->getValue();
      //const int *reverseDescendingConnectivityIndex = _reverseDescendingConnectivity->getIndex();
      int *reverseDescendingConnectivityValue = new int[2*NumberOfConstituent];
      int *reverseDescendingConnectivityIndex = new int[NumberOfConstituent+1];
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
	  if (abs(descend_connectivity[j])>tmp_NumberOfConstituentsForeachType[0]) {
            if ( descend_connectivity[j] > 0 )
              descend_connectivity[j]-=offset;
            else 
              descend_connectivity[j]+=offset;
          }
      }

      delete [] ConstituentNodalConnectivityIndex;
      delete [] ConstituentNodalConnectivity;
      delete [] ReverseDescendingConnectivityValue;
      _constituent->_count[1]=tmp_NumberOfConstituentsForeachType[0]+1;
      delete [] tmp_NumberOfConstituentsForeachType;

      _descending = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,
					DescendingSize,
					descend_connectivity_index,
					descend_connectivity);
      delete [] descend_connectivity_index;
      delete [] descend_connectivity;

      ////
      vector<int> PolyDescending;
      vector<int> Reversedescendingconnectivityvalue(reverseDescendingConnectivityValue,reverseDescendingConnectivityValue + 2*NumberOfConstituent);
      vector<int> Reversedescendingconnectivityindex(reverseDescendingConnectivityIndex,reverseDescendingConnectivityIndex + NumberOfConstituent);
      delete [] reverseDescendingConnectivityValue;
      delete [] reverseDescendingConnectivityIndex;


      // polygons (2D mesh)

      vector<int> Constituentnodalvalue(ConstituentNodalValue,ConstituentNodalValue + SizeOfConstituentNodal);
      vector<int> Constituentnodalindex(ConstituentNodalIndex,ConstituentNodalIndex + NumberOfConstituent+1);
      delete [] ConstituentNodalValue;
      delete [] ConstituentNodalIndex;
      int NumberOfNewSeg = 0;

      for (int i=0; i <getNumberOfPolygons(); i++) // for each polygon
	{
	  const int * vector_begin = &_polygonsNodal->getValue()[_polygonsNodal->getIndex()[i]-1];
	  int vector_size = _polygonsNodal->getIndex()[i+1]-_polygonsNodal->getIndex()[i]+1;
	  vector<int> myPolygon(vector_begin, vector_begin+vector_size);
	  myPolygon[myPolygon.size()-1] = myPolygon[0]; // because first and last point make a segment

	  for (int j=0; j<myPolygon.size()-1; j++) // for each segment of polygon
	    {
	      MEDMODULUSARRAY segment_poly(2,&myPolygon[j]);
	      int ret_compare = 0;

	      // we search it in existing segments

	      for (int k=0; k<Constituentnodalindex.size()-1; k++)
		{
		  MEDMODULUSARRAY segment(2,&Constituentnodalvalue[0] + Constituentnodalindex[k]-1);
		  ret_compare = segment_poly.compare(segment);
		  if (ret_compare) // segment_poly already exists
		    {
		      PolyDescending.push_back(ret_compare*(k+1)); // we had it to the connectivity
		      insert_vector(Reversedescendingconnectivityvalue, 2*k+1, i+1 + getNumberOf(MED_CELL,MED_ALL_ELEMENTS)); // add polygon i to reverse descending connectivity for segment_poly (in 2sd place)
		      break;
		    }
		}

	      // segment_poly must be created

	      if (!ret_compare)
		{
		  NumberOfNewSeg++;
		  PolyDescending.push_back(NumberOfConstituent+NumberOfNewSeg); // we had it to the connectivity
		  Constituentnodalvalue.push_back(segment_poly[0]);
		  Constituentnodalvalue.push_back(segment_poly[1]);
		  Constituentnodalindex.push_back(Constituentnodalindex[Constituentnodalindex.size()-1] + 2); // we have only segments
		  insert_vector(Reversedescendingconnectivityvalue, 2*(NumberOfConstituent+NumberOfNewSeg-1), i+1 + getNumberOf(MED_CELL,MED_ALL_ELEMENTS)); // add polygon i to reverse descending connectivity for segment_poly (in 1st place)
		  insert_vector(Reversedescendingconnectivityindex, NumberOfConstituent+NumberOfNewSeg-1, 2*(NumberOfConstituent+NumberOfNewSeg-1)+1); // idem with index
		}
	    }
	}

      if (getNumberOfPolygons() > 0)
	{
	  _polygonsDescending = new MEDSKYLINEARRAY(getNumberOfPolygons(),_polygonsNodal->getLength(),_polygonsNodal->getIndex(),&PolyDescending[0]); // index are the same for polygons nodal and descending connectivities

	  NumberOfConstituent += NumberOfNewSeg;
	  SizeOfConstituentNodal += 2*NumberOfNewSeg;
	  _constituent->_count[1] = NumberOfConstituent+1;
	}


      // polyhedron (3D mesh)

      vector<int> Constituentpolygonsnodalvalue;
      vector<int> Constituentpolygonsnodalindex(1,1);
      int NumberOfNewFaces = 0; // by convention new faces are polygons

      for (int i=0; i<getNumberOfPolyhedron(); i++) // for each polyhedron
	{
	  // we create a POLYHEDRONARRAY containing only this polyhedra
	  int myNumberOfFaces = _polyhedronNodal->getPolyhedronIndex()[i+1]-_polyhedronNodal->getPolyhedronIndex()[i];
	  int myNumberOfNodes = _polyhedronNodal->getFacesIndex()[_polyhedronNodal->getPolyhedronIndex()[i+1]-1]-_polyhedronNodal->getFacesIndex()[_polyhedronNodal->getPolyhedronIndex()[i]-1];
	  POLYHEDRONARRAY myPolyhedra(1,myNumberOfFaces,myNumberOfNodes);
//CCRT	  vector<int> myFacesIndex(_polyhedronNodal->getFacesIndex() + _polyhedronNodal->getPolyhedronIndex()[i]-1, _polyhedronNodal->getFacesIndex() + _polyhedronNodal->getPolyhedronIndex()[i]-1 + myNumberOfFaces+1);
	  vector<med_int> myFacesIndex(_polyhedronNodal->getFacesIndex() + _polyhedronNodal->getPolyhedronIndex()[i]-1, _polyhedronNodal->getFacesIndex() + _polyhedronNodal->getPolyhedronIndex()[i]-1 + myNumberOfFaces+1);
	  for (int j=myNumberOfFaces; j>=0; j--)
	    myFacesIndex[j] -= myFacesIndex[0]-1;
	  myPolyhedra.setFacesIndex(&myFacesIndex[0]);
	  myPolyhedra.setNodes(_polyhedronNodal->getNodes() + _polyhedronNodal->getFacesIndex()[_polyhedronNodal->getPolyhedronIndex()[i]-1]-1);

	  for (int j=0; j<myPolyhedra.getNumberOfFaces(); j++) // for each face of polyhedra
	    {
	      int myFaceNumberOfNodes = myPolyhedra.getFacesIndex()[j+1]-myPolyhedra.getFacesIndex()[j];
//CCRT
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
              int ii ;
              const med_int * Nodes = myPolyhedra.getNodes() ;
              int * tmp_Nodes = new int[myPolyhedra.getNumberOfNodes()] ;
              for ( ii = 0 ; ii < myPolyhedra.getNumberOfNodes() ; ii++ )
                 tmp_Nodes[ii] = Nodes[ii] ;
              const med_int * FacesIndex = myPolyhedra.getFacesIndex() ;
              int * tmp_FacesIndex = new int[myPolyhedra.getNumberOfFaces()+1] ;
              for ( ii = 0 ; ii < myPolyhedra.getNumberOfNodes() ; ii++ )
                 tmp_FacesIndex[ii] = FacesIndex[ii] ;
//CCRT : copy of Nodes
	      MEDMODULUSARRAY face_poly(myFaceNumberOfNodes,tmp_Nodes + tmp_FacesIndex[j]-1);
//CCRT              delete [] tmp_Nodes ;
              delete [] tmp_FacesIndex ;
#else
	      MEDMODULUSARRAY face_poly(myFaceNumberOfNodes,myPolyhedra.getNodes() + myPolyhedra.getFacesIndex()[j]-1);
#endif
	      int ret_compare = 0;

	      // we search it in existing faces

	      // we search first in TRIA3 and QUAD4
	      if (myFaceNumberOfNodes == 3 || myFaceNumberOfNodes == 4)
		{
		  int Begin = -1; // first TRIA3 or QUAD4
		  int Number = 0; // number of TRIA3 or QUAD4
		  for (int k=0; k<Constituentnodalindex.size()-1; k++)
		    if (Constituentnodalindex[k+1]-Constituentnodalindex[k] == myFaceNumberOfNodes)
		      {
			if (Begin == -1)
			  Begin = k;
			Number++;
		      }

		  for (int k=0; k<Number; k++)
		    {
		      MEDMODULUSARRAY face(myFaceNumberOfNodes,&Constituentnodalvalue[0] + Constituentnodalindex[Begin+k]-1);
		      ret_compare = face_poly.compare(face);
		      if (ret_compare)
			{
			  PolyDescending.push_back(ret_compare*(Begin+k+1)); // we had it to the connectivity
			  insert_vector(Reversedescendingconnectivityvalue, 2*(Begin+k)+1, i+1 + getNumberOf(MED_CELL,MED_ALL_ELEMENTS)); // add polyhedra i to reverse descending connectivity for face_poly (in 2sd place)
			  break;
			}
		    }
		}

	      // we search last in POLYGONS
	      if (!ret_compare)
		{
		  for (int k=0; k<static_cast<int>(Constituentpolygonsnodalindex.size())-1; k++) // we must cast the unsigned int into int before doing -1
		    {
		      if (Constituentpolygonsnodalindex[k+1]-Constituentpolygonsnodalindex[k] == myFaceNumberOfNodes)
			{
			  MEDMODULUSARRAY face(myFaceNumberOfNodes,&Constituentpolygonsnodalvalue[0] + Constituentpolygonsnodalindex[k]-1);
			  ret_compare = face_poly.compare(face);
			  if (ret_compare)
			    {
			      PolyDescending.push_back(ret_compare*(NumberOfConstituent+k+1)); // we had it to the connectivity
			      insert_vector(Reversedescendingconnectivityvalue, 2*(NumberOfConstituent+k)+1, i+1 + getNumberOf(MED_CELL,MED_ALL_ELEMENTS)); // add polyhedra i to reverse descending connectivity for face_poly (in 2sd place)
			      break;
			    }
			}
		    }
		}

	      // if not found, face_poly must be created

	      if (!ret_compare)
		{
		  NumberOfNewFaces++;
		  PolyDescending.push_back(NumberOfConstituent+NumberOfNewFaces); // we had it to the connectivity
		  for (int k=0; k<myFaceNumberOfNodes; k++)
		    Constituentpolygonsnodalvalue.push_back(face_poly[k]);
		  Constituentpolygonsnodalindex.push_back(Constituentpolygonsnodalindex[Constituentpolygonsnodalindex.size()-1]+myFaceNumberOfNodes);
		  insert_vector(Reversedescendingconnectivityvalue, 2*(NumberOfConstituent+NumberOfNewFaces-1), i+1 + getNumberOf(MED_CELL,MED_ALL_ELEMENTS)); // add polyhedra i to reverse descending connectivity for face_poly (in 1st place)
		  insert_vector(Reversedescendingconnectivityindex, NumberOfConstituent+NumberOfNewFaces-1, 2*(NumberOfConstituent+NumberOfNewFaces-1)+1); // idem with index
		}
	    }
	}

      if (getNumberOfPolyhedron() > 0)
	{
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
          int * tmp_PolyhedronIndex = new int[getNumberOfPolyhedron()+1] ;
          const MED_EN::med_int * PolyhedronIndex = _polyhedronNodal->getPolyhedronIndex() ;
          int ii ;
          for ( ii = 0 ; ii < getNumberOfPolyhedron()+1 ; ii++ )
             tmp_PolyhedronIndex[ii] = PolyhedronIndex[ii] ;
//CCRT : copy of PolyhedronIndex
	  _polyhedronDescending = new MEDSKYLINEARRAY(getNumberOfPolyhedron(),_polyhedronNodal->getNumberOfFaces(),tmp_PolyhedronIndex,&PolyDescending[0]); // polyhedron index are the same for nodal and descending connectivities
#else
	  _polyhedronDescending = new MEDSKYLINEARRAY(getNumberOfPolyhedron(),_polyhedronNodal->getNumberOfFaces(),_polyhedronNodal->getPolyhedronIndex(),&PolyDescending[0]); // polyhedron index are the same for nodal and descending connectivities
#endif

	  if (_constituent->_polygonsNodal != NULL)
	    delete [] _constituent->_polygonsNodal;
	  _constituent->_polygonsNodal = new MEDSKYLINEARRAY(Constituentpolygonsnodalindex.size()-1,Constituentpolygonsnodalvalue.size(),&Constituentpolygonsnodalindex[0],&Constituentpolygonsnodalvalue[0]);
	}

      // delete _constituent->_nodal
      _constituent->_nodal = new MEDSKYLINEARRAY(NumberOfConstituent,
						 SizeOfConstituentNodal,
						 &Constituentnodalindex[0],
						 &Constituentnodalvalue[0]);

      int NumberOfConstituentWithPolygons = NumberOfConstituent + NumberOfNewFaces;
      Reversedescendingconnectivityindex.push_back(Reversedescendingconnectivityindex[Reversedescendingconnectivityindex.size()-1]+2); // we complete the index
      _reverseDescendingConnectivity = new MEDSKYLINEARRAY(NumberOfConstituentWithPolygons+1,
							   2*NumberOfConstituentWithPolygons,
							   &Reversedescendingconnectivityindex[0],
							   &Reversedescendingconnectivityvalue[0]);
      ////
    }
  END_OF(LOC);
  }

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

/*!
  Idem as getElementType method except that it manages polygon and polyhedron.
*/
//--------------------------------------------------------------------//
medGeometryElement CONNECTIVITY::getElementTypeWithPoly(medEntityMesh Entity,int globalNumber) const
//--------------------------------------------------------------------//
{
  int globalNumberMaxOfClassicType;
  if(_entity==Entity)
    {
      globalNumberMaxOfClassicType=_count[_numberOfTypes];
      if(globalNumber>=1)
	{
	  if(globalNumber<globalNumberMaxOfClassicType)
	    {
	      for(int i=1; i<=_numberOfTypes;i++)
		if (globalNumber<_count[i])
		  return _geometricTypes[i-1];
	      throw MEDEXCEPTION("never happens just for compilo");
	    }
	  else
	    {
	      int localNumberInPolyArray=globalNumber-globalNumberMaxOfClassicType;
	      int nbOfPol=getNumberOfElementOfPolyType(_entity);
	      if(localNumberInPolyArray<nbOfPol)
		return getPolyTypeRelativeTo();
	      else
		throw MEDEXCEPTION("getElementTypeWithPoly : unexisting element type");
	    }
	}
      else
	throw MEDEXCEPTION("getElementTypeWithPoly : globalNumber < 1");
    }
  else
    {
      if(_constituent!=NULL)
	return _constituent->getElementTypeWithPoly(Entity,globalNumber);
      else
	throw MEDEXCEPTION("getElementTypeWithPoly : Entity not defined !");
    }
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
	    const int * connectivity = co.getConnectivity(co._typeConnectivity, co._entity, co._geometricTypes[i]);
	    int numberofnodespercell = co._geometricTypes[i]%100;
	    for (int j=0;j<numberofelements;j++)
	    {
		os << setw(6) << j+1 << " : ";
		for (int k=0;k<numberofnodespercell;k++)
		    os << connectivity[j*numberofnodespercell+k]<<" ";
		os << endl;
	    }
	}

	os << endl << "MED_POLYGON : " << co.getNumberOfPolygons() << " polygons" << endl;
	if (co.getNumberOfPolygons() > 0)
	  {
	    const int* polygonsconnectivity = co.getPolygonsConnectivity(MED_NODAL,co.getEntity());
	    const int* polygonsconnectivityindex = co.getPolygonsConnectivityIndex(MED_NODAL,co.getEntity());

	    for (int i=0; i<co.getNumberOfPolygons(); i++)
	      {
		int numberofnodesperpolygon = polygonsconnectivityindex[i+1]-polygonsconnectivityindex[i];
		for (int j=0; j<numberofnodesperpolygon; j++)
		  os << polygonsconnectivity[polygonsconnectivityindex[i]-1+j] << " ";
		os << endl;
	      }
	  }

	os << endl << "MED_POLYHEDRA : " << co.getNumberOfPolyhedron() << " polyhedron" << endl;
	if (co.getNumberOfPolyhedron() > 0)
	  {
	    const int* polyhedronconnectivity = co.getPolyhedronConnectivity(MED_NODAL);
	    const int* polyhedronfacesindex = co.getPolyhedronFacesIndex();
	    const int* polyhedronindex = co.getPolyhedronIndex(MED_NODAL);

	    for (int i=0; i<co.getNumberOfPolyhedron(); i++)
	      {
		int numberoffacesperpolyhedra = polyhedronindex[i+1]-polyhedronindex[i];
		for (int j=0; j<numberoffacesperpolyhedra; j++)
		  {
		    int numberofnodesperface = polyhedronfacesindex[polyhedronindex[i]-1+j+1]-polyhedronfacesindex[polyhedronindex[i]-1+j];
		    for (int k=0; k<numberofnodesperface; k++)
		      os << polyhedronconnectivity[polyhedronfacesindex[polyhedronindex[i]-1+j]-1+k] << " ";
		    if (j != numberoffacesperpolyhedra-1) os << "| ";
		  }
		os << endl;
	      }
	  }

    }
    else if (co._typeConnectivity == MED_DESCENDING)
    {
	int numberofelements = co.getNumberOf( co._entity , MED_ALL_ELEMENTS);
	if (numberofelements > 0)
	  {
	    const int *connectivity =  co.getConnectivity( co._typeConnectivity, co._entity, MED_ALL_ELEMENTS);
	    const int *connectivity_index =  co.getConnectivityIndex( co._typeConnectivity, co._entity );

	    for ( int j=0; j!=numberofelements; j++ )
	      {
		os << "Element " << j+1 << " : ";
		for ( int k=connectivity_index[j]; k!=connectivity_index[j+1]; k++ )
		  os << connectivity[k-1] << " ";
		os << endl;
	      }
	  }

	os << endl << "MED_POLYGON : " << co.getNumberOfPolygons() << " polygons" << endl;
	if (co.getNumberOfPolygons() > 0)
	  {
	    const int* polygonsconnectivity = co.getPolygonsConnectivity(MED_DESCENDING,co.getEntity());
	    const int* polygonsconnectivityindex = co.getPolygonsConnectivityIndex(MED_DESCENDING,co.getEntity());

	    for (int i=0; i<co.getNumberOfPolygons(); i++)
	      {
		int numberofedgesperpolygon = polygonsconnectivityindex[i+1]-polygonsconnectivityindex[i];
		for (int j=0; j<numberofedgesperpolygon; j++)
		  os << polygonsconnectivity[polygonsconnectivityindex[i]-1+j] << " ";
		os << endl;
	      }
	  }

	os << endl << "MED_POLYHEDRA : " << co.getNumberOfPolyhedron() << " polyhedron" << endl;
	if (co.getNumberOfPolyhedron() > 0)
	  {
	    const int* polyhedronconnectivity = co.getPolyhedronConnectivity(MED_DESCENDING);
	    const int* polyhedronindex = co.getPolyhedronIndex(MED_DESCENDING);

	    for (int i=0; i<co.getNumberOfPolyhedron(); i++)
	      {
		int numberoffacesperpolyhedra = polyhedronindex[i+1]-polyhedronindex[i];
		for (int j=0; j<numberoffacesperpolyhedra; j++)
		  os << polyhedronconnectivity[polyhedronindex[i]-1+j] << " ";
		os << endl;
	      }
	  }

    }

    if (co._constituent)
	os << endl << *co._constituent << endl;

    return os;
}

/*!
  method that adds to vector 'nodes' all the nodes of polyhedron with id 'polyhedronId'.
  WARNING the returned pointer should be deallocated. Returned nodes and polyhedronId are in form [1,...]
 */
int *CONNECTIVITY::getNodesOfPolyhedron(int polyhedronId, int& lgthOfTab) const
{
  const int *nodes=getPolyhedronConnectivity(MED_EN::MED_NODAL);
  const int *faces=getPolyhedronFacesIndex();
  const int *glob=getPolyhedronIndex(MED_EN::MED_NODAL);
  int offsetWithClassicType=getNumberOf(_entity,MED_ALL_ELEMENTS);
  set<int> retInSet;
  int startFace=glob[polyhedronId-offsetWithClassicType-1]-1;
  int endFace=glob[polyhedronId-offsetWithClassicType]-1;
  int i;
  for(i=startFace;i!=endFace;i++)
    {
      for(int j=faces[i];j<faces[i+1];j++)
	retInSet.insert(nodes[j-1]);
    }
  lgthOfTab=retInSet.size();
  int *ret=new int[lgthOfTab];
  set<int>::iterator iter=retInSet.begin();
  i=0;
  for(iter=retInSet.begin();iter!=retInSet.end();iter++)
    ret[i++]=*iter;
  return ret;
}

/*!
  Idem as MESH::getNodesOfPolyhedron except that returned nodes are sorted by face. 'nbOfNodesPerFaces' is an array of size 'nbOfFaces'.
  Returned int** has a size of 'nbOfNodesPerFaces' too, and for each element j in int** the size is  nbOfNodesPerFaces[j].
  Warning both returned 'nbOfNodesPerFaces' and returned value should be deallocated. Returned nodes and 'polyhedronId' are in form [1,...]
 */
int **CONNECTIVITY::getNodesPerFaceOfPolyhedron(int polyhedronId, int& nbOfFaces, int* & nbOfNodesPerFaces) const
{
  int i;
  const int *nodes=getPolyhedronConnectivity(MED_EN::MED_NODAL);
  const int *faces=getPolyhedronFacesIndex();
  const int *glob=getPolyhedronIndex(MED_EN::MED_NODAL);
  int offsetWithClassicType=getNumberOf(_entity,MED_ALL_ELEMENTS);

  int startFace=glob[polyhedronId-offsetWithClassicType-1]-1;
  nbOfFaces=glob[polyhedronId-offsetWithClassicType]-startFace-1;
  nbOfNodesPerFaces=new int[nbOfFaces];
  int **ret=new int* [nbOfFaces];
  for(i=0;i<nbOfFaces;i++)
    {
      int startNode=faces[startFace+i]-1;
      nbOfNodesPerFaces[i]=faces[startFace+i+1]-startNode-1;
      ret[i]=(int *)(nodes)+startNode;
    }
  return ret;
}

int CONNECTIVITY::getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const
{
  SCRUTE(_entity);
  SCRUTE(Entity);
  if (_entity==Entity)
    {
      SCRUTE(_numberOfTypes);
      SCRUTE(getNumberOfPolyType());
      return _numberOfTypes+getNumberOfPolyType();
    }
  else if (_constituent!=NULL)
    return _constituent->getNumberOfTypesWithPoly(Entity);
  else
    return 0;
}

int CONNECTIVITY::getNumberOfPolyType()  const
{
  if (_entity==MED_CELL && _entityDimension==3)
    {
      if(getNumberOfPolyhedron()>0)
	return 1;
    }
  else if ((_entity==MED_CELL && _entityDimension==2) || (_entity==MED_FACE && _entityDimension==2))
    if(getNumberOfPolygons()>0)
      return 1;
  return 0;
}

int CONNECTIVITY::getNumberOfElementOfPolyType(MED_EN::medEntityMesh Entity)  const
{
  if(_entity==Entity)
    {
      if (_entity==MED_CELL && _entityDimension==3)
	return getNumberOfPolyhedron();
      else if ((_entity==MED_CELL && _entityDimension==2) || (_entity==MED_FACE && _entityDimension==2))
	return getNumberOfPolygons();
      return 0;
    }
  else
    {
      if(_constituent!=NULL)
	return _constituent->getNumberOfElementOfPolyType(Entity);
      else
	throw MEDEXCEPTION("_constituent required to evaluate getNumberOfElementOfPolyType");
    }
}

/*
  Method equivalent to getGeometricTypes except that it includes not only classical Types but polygons/polyhedra also.
  WARNING the returned array MUST be deallocated.
 */
MED_EN::medGeometryElement * CONNECTIVITY::getGeometricTypesWithPoly(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION)
{
  if(_entity==Entity)
    {
      int nbOfTypesTotal=getNumberOfTypesWithPoly(Entity);
      int nbOfTypesWithoutPoly=getNumberOfTypes(Entity);
      medGeometryElement *ret=new medGeometryElement[nbOfTypesTotal];
      memcpy(ret,getGeometricTypes(Entity),nbOfTypesWithoutPoly*sizeof(medGeometryElement));
      if(nbOfTypesTotal>nbOfTypesWithoutPoly)
	{
	  if (Entity==MED_CELL && _entityDimension==3)
	    ret[nbOfTypesWithoutPoly]=MED_EN::MED_POLYHEDRA;
	  else if((Entity==MED_CELL && _entityDimension==2) || (Entity==MED_FACE && _entityDimension==2))
	    ret[nbOfTypesWithoutPoly]=MED_EN::MED_POLYGON;
	}
      return ret;
    }
  else
    {
      if(_constituent)
	return _constituent->getGeometricTypesWithPoly(Entity);
      throw MEDEXCEPTION("_constituent required to evaluate getGeometricTypesWithPoly");
    }
}

/*
  Method used in CalculateDescendingConnectivity. So it's typically a private method.
  The aim of this method is to hide to CalculateDescendingConnectivity algorithm the fact that in reverse connectivity polygons and polyhedrons
  are not in separate data structure, contrary to not reverse connectivities.
 */
int CONNECTIVITY::getIndexOfEndClassicElementInReverseNodal(const int *reverseNodalValue, const int *reverseNodalIndex, int rk)  const
{
  int nbOfLastElt=getNumberOf(_entity,MED_ALL_ELEMENTS);
  int ret=reverseNodalIndex[rk];
  for(int i=reverseNodalIndex[rk];i<reverseNodalIndex[rk+1];i++)
    {
      if(reverseNodalValue[i-1]<=nbOfLastElt)
	ret++;
    }
  return ret;
}

/*
  Method that inverts the face with faceId 'faceId' in the data structure. If it's a polygon face 'faceId' is a value between 1 and nbOfPolygons.
  This method has to be applied on a instance of MEDMEM::CONNECTIVITY with _entityDimension==3.
  WARNING calculateDescendingConnectivity must have been called before.
 */
void CONNECTIVITY::invertConnectivityForAFace(int faceId, const int *nodalConnForFace, bool polygonFace)
{
  // we have always 2 neighbourings
  int cell1 = _reverseDescendingConnectivity->getIJ(faceId,1);
  int cell2 = _reverseDescendingConnectivity->getIJ(faceId,2);

  if (cell2 != 0) { // we are not on border, update compulsary. If we aren't on border no update necessary so WARNING because user specified a bad oriented face
    // Updating _reverseDescendingConnectivity
    _reverseDescendingConnectivity->setIJ(faceId,1,cell2);
    _reverseDescendingConnectivity->setIJ(faceId,2,cell1);
    // Updating _constituent->_nodal because of reversity
    const int *descendingNodalIndex=(!polygonFace)?_constituent->_nodal->getIndex():_constituent->_polygonsNodal->getIndex();
    MEDSKYLINEARRAY *currentNodal=(!polygonFace)?_constituent->_nodal:_constituent->_polygonsNodal;
    int faceIdRelative=(!polygonFace)?faceId:faceId-getNumberOf(MED_FACE,MED_ALL_ELEMENTS);
    for(int iarray=1;iarray<=(descendingNodalIndex[faceIdRelative]-descendingNodalIndex[faceIdRelative-1]);iarray++)
      currentNodal->setIJ(faceIdRelative,iarray,nodalConnForFace[iarray-1]);

    // Updating _descending for cell1 and cell2
    const int NB_OF_CELLS_SHARING_A_FACE=2;
    int cellsToUpdate[NB_OF_CELLS_SHARING_A_FACE]; cellsToUpdate[0]=cell1; cellsToUpdate[1]=cell2;
    for(int curCell=0;curCell<NB_OF_CELLS_SHARING_A_FACE;curCell++)
      {
	int cell=cellsToUpdate[curCell];
	bool polyhCell=(getElementTypeWithPoly(MED_CELL,cell)==MED_POLYHEDRA);
	if(polyhCell)
	  cell-=getNumberOf(MED_CELL,MED_ALL_ELEMENTS);
	const int *newDescendingIndex=(!polyhCell)?_descending->getIndex():_polyhedronDescending->getIndex();
	MEDSKYLINEARRAY *currentDescending=(!polyhCell)?_descending:_polyhedronDescending;
	for(int iface=newDescendingIndex[cell-1];iface<newDescendingIndex[cell];iface++)
	  {
	    int curValue=currentDescending->getIndexValue(iface);
	    if (abs(curValue)==faceId)
	      currentDescending->setIndexValue(iface,-curValue);
	  }
      }
  }
}

/*
  Method with 2 output : the connectivity required and its length 'lgth'. This method gives the connectivity independently it is a polygons/polyhedrons or normal
  element.
 */
const int * CONNECTIVITY::getConnectivityOfAnElementWithPoly(MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity, int Number, int &lgth)
{
  if(Entity==MED_EN::MED_NODE)
    throw  MEDEXCEPTION("No connectivity attached to a node entity");
  if(Entity==_entity)
    {
      if(_entity==MED_EDGE && _entityDimension==1)
	{
	  const int * newConstituentValue = 0;
	  const int * newConstituentIndex = 0;
	  newConstituentValue = getConnectivity(ConnectivityType,Entity,MED_ALL_ELEMENTS);
	  newConstituentIndex = getConnectivityIndex(ConnectivityType,Entity);
	  lgth=newConstituentIndex[Number]-newConstituentIndex[Number-1];
	  return newConstituentValue+newConstituentIndex[Number-1]-1;
	}
      int nbOfClassicalElements=getNumberOf(_entity,MED_ALL_ELEMENTS);
      if((_entity==MED_FACE && _entityDimension==2) || (_entity==MED_CELL && _entityDimension==2))
	{
	  const int * newConstituentValue = 0;
	  const int * newConstituentIndex = 0;
	  if(Number<=nbOfClassicalElements)
	    {
	      newConstituentValue = getConnectivity(ConnectivityType,Entity,MED_ALL_ELEMENTS);
	      newConstituentIndex = getConnectivityIndex(ConnectivityType,Entity);
	      lgth=newConstituentIndex[Number]-newConstituentIndex[Number-1];
	      return newConstituentValue+newConstituentIndex[Number-1]-1;
	    }
	  else
	    {
	      int localNumber=Number-nbOfClassicalElements-1;
	      if(localNumber<getNumberOfPolygons())
		{
		  newConstituentValue = getPolygonsConnectivity(ConnectivityType,Entity);
		  newConstituentIndex = getPolygonsConnectivityIndex(ConnectivityType,Entity);
		  lgth=newConstituentIndex[localNumber+1]-newConstituentIndex[localNumber];
		  return newConstituentValue+newConstituentIndex[localNumber]-1;
		}
	      else
		throw  MEDEXCEPTION("Unknown number");
	    }
	}
      else if(_entity==MED_CELL && _entityDimension==3)
	{
	  const int * newConstituentValue = 0;
	  const int * newConstituentIndex = 0;
	  if(Number<=nbOfClassicalElements)
	    {
	      newConstituentValue = getConnectivity(ConnectivityType,Entity,MED_ALL_ELEMENTS);
	      newConstituentIndex = getConnectivityIndex(ConnectivityType,Entity);
	      lgth=newConstituentIndex[Number]-newConstituentIndex[Number-1];
	      return newConstituentValue+newConstituentIndex[Number-1]-1;
	    }
	  else
	    {
	      int localNumber=Number-nbOfClassicalElements-1;
	      if(localNumber<getNumberOfPolyhedron())
		{
		  if(ConnectivityType==MED_NODAL)
		    throw  MEDEXCEPTION("NODAL Connectivity required for a polyhedron");
// 		  newConstituentValue = _polyhedronDescending->getValue();
// 		  newConstituentIndex = _polyhedronDescending->getIndex();
		  newConstituentValue = getPolyhedronConnectivity( ConnectivityType );
		  newConstituentIndex = getPolyhedronIndex( ConnectivityType );
		  lgth=newConstituentIndex[localNumber+1]-newConstituentIndex[localNumber];
		  return newConstituentValue+newConstituentIndex[localNumber]-1;
		}
	      else
		throw  MEDEXCEPTION("Unknown number");
	    }
	}
      else
	throw MEDEXCEPTION("No connectivity available");
    }
  else
    {
      if(_constituent==NULL)
	calculateDescendingConnectivity();
      return _constituent->getConnectivityOfAnElementWithPoly(ConnectivityType,Entity,Number,lgth);
    }
}

int CONNECTIVITY::getNumberOfElementsWithPoly(MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const
{
  if(Entity==MED_EN::MED_NODE)
    return _numberOfNodes;
  if(Entity==_entity)
    {
      if(Type==MED_POLYGON || Type==MED_POLYHEDRA)
	return getNumberOfElementOfPolyType(_entity);
      else
	return getNumberOf(_entity,Type);
    }
  else
    {
      if(_constituent)
	return _constituent->getNumberOfElementsWithPoly(Entity,Type);
      else
	throw MEDEXCEPTION("CONNECTIVITY::getNumberOfElementsWithPoly : _constituent needed");
    }
}

/*!
  Perform a deep comparison of the 2 connectivities in NODAL mode and on all elements.
*/
bool CONNECTIVITY::deepCompare(const CONNECTIVITY& other) const
{
  CONNECTIVITY* temp=(CONNECTIVITY* )this;
  const int *conn1=temp->getConnectivity(MED_NODAL,_entity,MED_ALL_ELEMENTS);
  int size1=temp->getConnectivityLength(MED_NODAL,_entity,MED_ALL_ELEMENTS);
  temp=(CONNECTIVITY* )(&other);
  const int *conn2=temp->getConnectivity(MED_NODAL,_entity,MED_ALL_ELEMENTS);
  int size2=temp->getConnectivityLength(MED_NODAL,_entity,MED_ALL_ELEMENTS);
  if(size1!=size2)
    return false;
  bool ret=true;
  for(int i=0;i<size1 && ret;i++)
    {
      ret=(conn1[i]==conn2[i]);
    }
  return ret;
}
