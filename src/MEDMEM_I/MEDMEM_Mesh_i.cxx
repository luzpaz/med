//=============================================================================
// File      : MEDMEM_Mesh_i.cxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Mesh_i.cxx
//=============================================================================

#include <vector>

#include "utilities.h"
#include "Utils_CorbaException.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include "MEDMEM_convert.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_Support_i.hxx"
#include "MEDMEM_Family_i.hxx"
#include "MEDMEM_Group_i.hxx"
#include "MEDMEM_FieldDouble_i.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_CellModel.hxx"
using namespace MEDMEM;

// Initialisation des variables statiques
map < int, ::MESH *> MESH_i::meshMap ;
int MESH_i::meshIndex = 0 ;



//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MESH_i::MESH_i(): _mesh(constructConstMesh()),
		  _corbaIndex(MESH_i::meshIndex++),
  		  _meshId("") 
{
        BEGIN_OF("Default Constructor MESH_i");
        MESH_i::meshMap[_corbaIndex]=_mesh;
        END_OF("Default Constructor MESH_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MESH_i::~MESH_i()
{
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
MESH_i::MESH_i(::MESH * const m ) :_mesh(m),
			_corbaIndex(MESH_i::meshIndex++),
  		        _meshId("") 
{
        BEGIN_OF("Constructor MESH_i(::MESH * const m )");
        MESH_i::meshMap[_corbaIndex]=_mesh;
	SCRUTE(_mesh);

        END_OF("Constructor MESH_i(::MESH * const m )");
}
//=============================================================================
/*!
 * Copy Constructor 
 */
//=============================================================================
//MESH_i::MESH_i(const MESH_i & m) :_mesh(m._mesh),
MESH_i::MESH_i( MESH_i & m) :_mesh(m._mesh),
		             _corbaIndex(MESH_i::meshIndex++),
  		             _meshId("") 
{
        BEGIN_OF("Constructor MESH_i");
        MESH_i::meshMap[_corbaIndex]=_mesh;
        END_OF("Constructor MESH_i");
}
//=============================================================================
/*!
 * Internal Method in order to have a const ptr
 */
//=============================================================================
::MESH * MESH_i::constructConstMesh() const
{
        ::MESH * const ptrMesh =new ::MESH();
	return ptrMesh;
}
//=============================================================================
/*!
 * CORBA: Accessor for Name
 */
//=============================================================================
char * MESH_i::getName() 	
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);

	try
        {
		return CORBA::string_dup(_mesh->getName().c_str());
	}
	catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the mesh name");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: Accessor for corbaindex cuisine interne 
 */
//=============================================================================
CORBA::Long MESH_i::getCorbaIndex() 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	return _corbaIndex;
}
//=============================================================================
/*!
 * CORBA: Accessor for Space Dimension
 */
//=============================================================================
CORBA::Long MESH_i::getSpaceDimension() 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	try
        {
		return _mesh->getSpaceDimension();
	}
	catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the space dimension ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: Accessor for Mesh Dimension
 */
//=============================================================================
CORBA::Long MESH_i::getMeshDimension()
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	try
        {
		return _mesh->getMeshDimension();
	}
	catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the mesh dimension ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: boolean indicating if mesh is a Grid
 */
//=============================================================================
CORBA::Boolean MESH_i::getIsAGrid()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getIsAGrid();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces mesh flag isAGrid");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: boolean indicating if connectivity exists
 */
//=============================================================================
CORBA::Boolean MESH_i::existConnectivity
                       (SALOME_MED::medConnectivity connectivityType,
                        SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->existConnectivity(connectivityType,
                                                convertIdlEntToMedEnt(entity));        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces mesh flag existConnectivity");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: Accessor for Coordinates System
 */
//=============================================================================
char * MESH_i::getCoordinatesSystem()
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);

	try
        {
		return CORBA::string_dup(_mesh->getCoordinatesSystem().c_str());
	}
	catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the type of CoordinatesSystem");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: Accessor for a specific coordinate
 */
//=============================================================================
double MESH_i::getCoordinate(CORBA::Long Number, CORBA::Long Axis)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                 return _mesh->getCoordinate(Number,Axis);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces this coordinate");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates
 */
//=============================================================================
SALOME_MED::double_array * MESH_i::getCoordinates(SALOME_MED::medModeSwitch typeSwitch)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	SALOME_MED::double_array_var myseq = new SALOME_MED::double_array;
	try
	{
		int spaceDimension=_mesh->getSpaceDimension();
		int nbNodes=_mesh->getNumberOfNodes();
		const double * coordinates =_mesh->getCoordinates(
			               convertIdlModeToMedMode(typeSwitch));

		myseq->length(nbNodes*spaceDimension);
		for (int i=0; i<nbNodes*spaceDimension; i++)
		{
			myseq[i]=coordinates[i];
		};
	}
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces the coordinates");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates Names
 */
//=============================================================================
SALOME_MED::string_array  * MESH_i::getCoordinatesNames() 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	SALOME_MED::string_array_var myseq = new SALOME_MED::string_array;
	try
	{
		int spaceDimension=_mesh->getSpaceDimension();
		const string * coordinatesName =_mesh->getCoordinatesNames();
		myseq->length(spaceDimension);
		for (int i=0; i<spaceDimension; i++)
        	{
               		myseq[i]=CORBA::string_dup(coordinatesName[i].c_str());
        	}
	}
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces the coordinates names");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();

}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates Units
 */
//=============================================================================
SALOME_MED::string_array *  MESH_i::getCoordinatesUnits()
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	SALOME_MED::string_array_var myseq = new SALOME_MED::string_array;
	try
	{
		int spaceDimension=_mesh->getSpaceDimension();
		const string * coordinatesUnits =_mesh->getCoordinatesUnits();
		myseq->length(spaceDimension);
		for (int i=0; i<spaceDimension; i++)
        	{
               		 myseq[i]=CORBA::string_dup(coordinatesUnits[i].c_str());
        	};
	}
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces the coordinates units");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Number of Nodes
 */
//=============================================================================
CORBA::Long  MESH_i::getNumberOfNodes() 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
        	return _mesh->getNumberOfNodes();
	}
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces number of nodes");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for number of Types
 */
//=============================================================================
CORBA::Long MESH_i::getNumberOfTypes(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
        	return _mesh->getNumberOfTypes(convertIdlEntToMedEnt(entity));
	}
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces number of differents types");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for existing geometry element types
 */
//=============================================================================
SALOME_MED::medGeometryElement MESH_i::getElementType (SALOME_MED::medEntityMesh entity,
                                                       CORBA::Long number)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getElementType(convertIdlEntToMedEnt(entity),number);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of differents element types");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: Accessor for existing geometry element types 
 * 	  Not implemented for MED_ALL_ENTITIES
 */
//=============================================================================
SALOME_MED::medGeometryElement_array * MESH_i::getTypes (SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	if (entity ==SALOME_MED::MED_ALL_ENTITIES)
                THROW_SALOME_CORBA_EXCEPTION("Not implemented for MED_ALL_ENTITIES",\
                                             SALOME::BAD_PARAM);
	SALOME_MED::medGeometryElement_array_var myseq = 
					new SALOME_MED::medGeometryElement_array;
	try
	{
		int nbTypes=_mesh->getNumberOfTypes(convertIdlEntToMedEnt(entity));
		const medGeometryElement * elemts  =_mesh->getTypes(
			               convertIdlEntToMedEnt(entity));
		myseq->length(nbTypes);
		for (int i=0; i<nbTypes; i++)
		{
                        myseq[i]=convertMedEltToIdlElt(elemts[i]);
		};
	}
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces coordinates");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns number of elements of type medGeometryElement
 *	  Not implemented for MED_ALL_ELEMENTS 
* 	  implemented for MED_ALL_ENTITIES
 */
//=============================================================================
CORBA::Long MESH_i::getNumberOfElements(SALOME_MED::medEntityMesh entity, 
			                SALOME_MED::medGeometryElement geomElement)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	if (verifieParam(entity,geomElement)==false)
                THROW_SALOME_CORBA_EXCEPTION("parameters don't match",\
                                             SALOME::BAD_PARAM);
	
	try
        {
		return _mesh->getNumberOfElements(convertIdlEntToMedEnt(entity),
						  convertIdlEltToMedElt(geomElement));
	}
	catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of elements");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_MED::long_array *  MESH_i::getConnectivity(SALOME_MED::medModeSwitch typeSwitch,
					       SALOME_MED::medConnectivity mode, 
					       SALOME_MED::medEntityMesh entity, 
					       SALOME_MED::medGeometryElement geomElement)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	if (verifieParam(entity,geomElement)==false)
                THROW_SALOME_CORBA_EXCEPTION("parameters don't match",\
                                             SALOME::BAD_PARAM);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
                int nbelements; 
		int elt1 = _mesh->getNumberOfElements(
					convertIdlEntToMedEnt(entity),
                                        convertIdlEltToMedElt(geomElement));
SCRUTE(elt1);
		if ( mode == SALOME_MED::MED_DESCENDING)
		{
MESSAGE("MED_DESCENDING");
		  int elt2 =(_mesh->getCellsTypes(MED_CELL))->getNumberOfConstituents(1);
		   //int elt2 =(_mesh->getTypes(convertIdlEltToMedElt(geomElement)))->getNumberOfConstituents(1);
		   nbelements= elt2 * elt1;
SCRUTE(elt2);
		}
		else
		{
MESSAGE("MED_NODAL");
			const int * tab=_mesh->getConnectivityIndex(
				convertIdlConnToMedConn(mode),
				convertIdlEntToMedEnt(entity));
			nbelements = elt1*(convertIdlEltToMedElt(geomElement)%100);
			//			nbelements = tab[elt1 ] - 1 ;
		}
SCRUTE(entity);
SCRUTE(geomElement);
SCRUTE(nbelements);
                myseq->length(nbelements);
                const int * numbers=_mesh->getConnectivity(convertIdlModeToMedMode(typeSwitch),
						     convertIdlConnToMedConn(mode),
						     convertIdlEntToMedEnt(entity),
						     convertIdlEltToMedElt(geomElement));
                for (int i=0;i<nbelements;i++)
                {
                        myseq[i]=numbers[i];
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces connectivities");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_MED::long_array* MESH_i::getConnectivityIndex(SALOME_MED::medConnectivity mode, 
						  SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
		int nbelements = _mesh->getNumberOfElements(
					convertIdlEntToMedEnt(entity),
					MED_ALL_ELEMENTS);
                myseq->length(nbelements);
                const int * numbers=_mesh->getConnectivityIndex(convertIdlConnToMedConn(mode),
						          convertIdlEntToMedEnt(entity));
                for (int i=0;i<nbelements;i++)
                {
                        myseq[i]=numbers[i];
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces connectivities index");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_MED::long_array* MESH_i::getGlobalNumberingIndex( SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
                int nbelements = _mesh->getNumberOfTypes( convertIdlEntToMedEnt(entity)) + 1;
                myseq->length(nbelements);
                const int * numbers=_mesh->getGlobalNumberingIndex( convertIdlEntToMedEnt(entity));
                for (int i=0;i<nbelements;i++)
                {
                        myseq[i]=numbers[i];
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces global index");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}

//=============================================================================
/*!
 * CORBA: get global element number in connectivity array
 * not implemented for MED_NODE and MED_ALL_ENTITIES,
 * MED_NONE and MED_ALL_ELEMENTS.
 */
//=============================================================================
CORBA::Long MESH_i::getElementNumber(SALOME_MED::medConnectivity mode,
				     SALOME_MED::medEntityMesh entity,
				     SALOME_MED::medGeometryElement type,
				     const SALOME_MED::long_array& connectivity)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  int numberOfValue = connectivity.length() ;
  int * myConnectivity = new int[numberOfValue] ;
  for (int i=0; i<numberOfValue; i++)
    myConnectivity[i]=connectivity[i] ;

  int result ;
  try {
    result = _mesh->getElementNumber(convertIdlConnToMedConn(mode),
				     convertIdlEntToMedEnt(entity),
				     convertIdlEltToMedElt(type),
				     myConnectivity) ;
  }
  catch (MEDEXCEPTION &ex) {
    THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
  }
  return result ;
}
//=============================================================================
/*!
 * CORBA: Accessor for Ascendant connectivities
 * not implemented for MED_ALL_ENTITIES and MED_MAILLE
 */
//=============================================================================
SALOME_MED::long_array* MESH_i::getReverseConnectivity(SALOME_MED::medConnectivity mode)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
                int nbelements; 
		if ( mode == SALOME_MED::MED_DESCENDING)
		{
		   nbelements =(_mesh->getNumberOfNodes())+1;
		}
		else
		{
		   nbelements = _mesh->getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS);
		}
		SCRUTE(nbelements);
                myseq->length(nbelements);
                const int * numbers=_mesh->getReverseConnectivity(convertIdlConnToMedConn(mode));
                for (int i=0;i<nbelements;i++)
                {
                        myseq[i]=numbers[i];
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces reverse connectivities");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_MED::long_array* MESH_i::getReverseConnectivityIndex(SALOME_MED::medConnectivity mode)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
                int nbelements; 
		if ( mode == SALOME_MED::MED_DESCENDING)
		{
		   nbelements =_mesh->getNumberOfNodes();
		}
		else
		{
		   int dim=_mesh->getMeshDimension();
		   if ( dim == 3)	
		   	nbelements = _mesh->getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS);
		   else
		   if (dim == 2)	
		   	nbelements = _mesh->getNumberOfElements(MED_EDGE,MED_ALL_ELEMENTS);
		   else
                	THROW_SALOME_CORBA_EXCEPTION("Pb ", \
                                             SALOME::INTERNAL_ERROR);
		}

                myseq->length(nbelements);
                const int * numbers=_mesh->getReverseConnectivityIndex(convertIdlConnToMedConn(mode));
                for (int i=0;i<nbelements;i++)
                {
                        myseq[i]=numbers[i];
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces reverse connectivities index");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns number of families within the mesh
 */
//=============================================================================
CORBA::Long MESH_i::getNumberOfFamilies(SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	try
        {
		return _mesh->getNumberOfFamilies(convertIdlEntToMedEnt(entity));
	}
	catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of families of the mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: Returns number of groups within the mesh
 */
//=============================================================================
CORBA::Long MESH_i::getNumberOfGroups(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
	try
        {
		return _mesh->getNumberOfGroups(convertIdlEntToMedEnt(entity));
	}
	catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of groups of the mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: Returns references for families within the mesh
 */
//=============================================================================
SALOME_MED::Family_array * MESH_i::getFamilies(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::Family_array_var myseq = new SALOME_MED::Family_array;
        try
        {
                int nbfam= _mesh->getNumberOfFamilies(convertIdlEntToMedEnt(entity));
                myseq->length(nbfam);
                vector<FAMILY*> fam(nbfam);
                fam = _mesh->getFamilies(convertIdlEntToMedEnt(entity));
                for (int i=0;i<nbfam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(fam[i]);
                        SALOME_MED::FAMILY_ptr f2 =
                                        f1->POA_SALOME_MED::FAMILY::_this();
                        f1->_remove_ref();
                        myseq[i] = f2;
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces families of the mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns Coordinates global informations
 */
//=============================================================================
SALOME_MED::MESH::coordinateInfos *  MESH_i::getCoordGlobal()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::MESH::coordinateInfos_var all = new SALOME_MED::MESH::coordinateInfos;
        try
        {
                all->coordSystem = CORBA::string_dup(_mesh->getCoordinatesSystem().c_str());

                int spaceDimension=_mesh->getSpaceDimension();
                const string * coordinatesUnits =_mesh->getCoordinatesUnits();
                const string * coordinatesName =_mesh->getCoordinatesNames();

                all->coordUnits.length(spaceDimension);
                all->coordNames.length(spaceDimension);
                for (int i=0; i<spaceDimension; i++)
                {
                         all->coordUnits[i]=CORBA::string_dup(coordinatesUnits[i].c_str());
                         all->coordNames[i]=CORBA::string_dup(coordinatesName[i].c_str());
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces coordinate information ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return all._retn();
}
//=============================================================================
/*!
 * CORBA: Returns connectivity global informations
 */
//=============================================================================
SALOME_MED::MESH::connectivityInfos * MESH_i::getConnectGlobal
                        (SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::MESH::connectivityInfos_var all=new SALOME_MED::MESH::connectivityInfos;
        try
        {
                all->numberOfNodes  = _mesh->getNumberOfNodes();

                int nbTypes=_mesh->getNumberOfTypes(convertIdlEntToMedEnt(entity));
                const medGeometryElement * elemts  =_mesh->getTypes(
                                       convertIdlEntToMedEnt(entity));
                all->meshTypes.length(nbTypes);
                all->numberOfElements.length(nbTypes);
                for (int i=0; i<nbTypes; i++)
                {
                        all->meshTypes[i]=convertMedEltToIdlElt(elemts[i]);
                        all->numberOfElements[i]=_mesh->getNumberOfElements(
                                       convertIdlEntToMedEnt(entity),elemts[i]);
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces connectivities informations");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return all._retn();
}

//=============================================================================
/*!
 * CORBA: Returns references for family i within the mesh
 */
//=============================================================================
SALOME_MED::FAMILY_ptr MESH_i::getFamily(SALOME_MED::medEntityMesh entity,
				         CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                const FAMILY * fam = _mesh->getFamily(convertIdlEntToMedEnt(entity),i);
                FAMILY_i * f1=new FAMILY_i(fam);
                SALOME_MED::FAMILY_ptr f2 = f1->POA_SALOME_MED::FAMILY::_this();
                f1->_remove_ref();
	        return (SALOME_MED::FAMILY::_duplicate(f2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified family of the mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }

}
//=============================================================================
/*
 * CORBA: Returns Mesh global informations
 */
//=============================================================================
SALOME_MED::MESH::meshInfos *  MESH_i::getMeshGlobal()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::MESH::meshInfos_var all = new SALOME_MED::MESH::meshInfos;
        try
        {
                all->name = CORBA::string_dup(_mesh->getName().c_str());
                all->spaceDimension = _mesh->getSpaceDimension();
                all->meshDimension  = _mesh->getMeshDimension();
                all->numberOfNodes  = _mesh->getNumberOfNodes();
                all->isAGrid        = _mesh->getIsAGrid();

                int nbFam= _mesh->getNumberOfFamilies(MED_NODE);
                all->famNode.length(nbFam);
                vector<FAMILY*> vNode (nbFam);
                vNode = _mesh->getFamilies(MED_NODE);
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vNode[i]);
                        SALOME_MED::FAMILY_ptr f2 = f1->POA_SALOME_MED::FAMILY::_this();
                        f1->_remove_ref();
                        all->famNode[i] = f2;
                }

                nbFam = _mesh->getNumberOfFamilies(MED_EDGE);
                all->famEdge.length(nbFam);
                vector<FAMILY*> vEdge (nbFam);
                vEdge = _mesh->getFamilies(MED_EDGE);
                for (int i=0;i<nbFam;i++)
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vEdge[i]);
                        SALOME_MED::FAMILY_ptr f2 = f1->POA_SALOME_MED::FAMILY::_this();
                        f1->_remove_ref();
                        all->famEdge[i] = f2;
                }

                nbFam = _mesh->getNumberOfFamilies(MED_FACE);
                all->famFace.length(nbFam);
                vector<FAMILY*> vFace (nbFam);
                vFace = _mesh->getFamilies(MED_FACE);
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vFace[i]);
                        SALOME_MED::FAMILY_ptr f2 = f1->POA_SALOME_MED::FAMILY::_this();
                        f1->_remove_ref();
                        all->famFace[i] = f2;
                }

                nbFam = _mesh->getNumberOfFamilies(MED_CELL);
                all->famCell.length(nbFam);
                vector<FAMILY*> vCell (nbFam);
                vCell = _mesh->getFamilies(MED_CELL);
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vCell[i]);
                        SALOME_MED::FAMILY_ptr f2 = f1->POA_SALOME_MED::FAMILY::_this();
                        f1->_remove_ref();
                        all->famCell[i] = f2;
                }

                int nbGroup = _mesh->getNumberOfGroups(MED_NODE);
                all->groupNode.length(nbGroup);
                vector<GROUP*> gNode (nbGroup);
                gNode = _mesh->getGroups(MED_NODE);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gNode[i]);
                        SALOME_MED::GROUP_ptr f2 = f1->POA_SALOME_MED::GROUP::_this();
                        f1->_remove_ref();
                        all->groupNode[i] = f2;
                }

                nbGroup = _mesh->getNumberOfGroups(MED_EDGE);
                all->groupEdge.length(nbGroup);
                vector<GROUP*> gEdge (nbGroup);
                gEdge = _mesh->getGroups(MED_EDGE);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gEdge[i]);
                        SALOME_MED::GROUP_ptr f2 = f1->POA_SALOME_MED::GROUP::_this();
                        f1->_remove_ref();
                        all->groupEdge[i] = f2;
                }
                nbGroup = _mesh->getNumberOfGroups(MED_FACE);
                all->groupFace.length(nbGroup);
                vector<GROUP*> gFace (nbGroup);
                gFace = _mesh->getGroups(MED_FACE);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gFace[i]);
                        SALOME_MED::GROUP_ptr f2 = f1->POA_SALOME_MED::GROUP::_this();
                        f1->_remove_ref();
                        all->groupFace[i] = f2;
                }

                nbGroup = _mesh->getNumberOfGroups(MED_CELL);
                all->groupCell.length(nbGroup);
                vector<GROUP*> gCell (nbGroup);
                gCell = _mesh->getGroups(MED_CELL);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gCell[i]);
                        SALOME_MED::GROUP_ptr f2 = f1->POA_SALOME_MED::GROUP::_this();
                        f1->_remove_ref();
                        all->groupCell[i] = f2;
                }

        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return all._retn();
}
//=============================================================================
 /*
 * CORBA: Returns references for groups within the mesh
 */
//=============================================================================
SALOME_MED::Group_array *  MESH_i::getGroups(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::Group_array_var myseq = new SALOME_MED::Group_array;
        try
        {
                int nbgroups = _mesh->getNumberOfGroups(convertIdlEntToMedEnt(entity));
                myseq->length(nbgroups);
                vector<GROUP*> groups(nbgroups);
                groups = _mesh->getGroups(convertIdlEntToMedEnt(entity));
                for (int i=0;i<nbgroups;i++)
                {
                	GROUP_i * f1=new GROUP_i(groups[i]);
                	SALOME_MED::GROUP_ptr f2 = f1->POA_SALOME_MED::GROUP::_this();
                        f1->_remove_ref();
                        myseq[i] = f2;
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of groups of the mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns references for group i within the mesh
 */
//=============================================================================
SALOME_MED::GROUP_ptr MESH_i::getGroup(SALOME_MED::medEntityMesh entity,
				       CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                const GROUP * grou = _mesh->getGroup(convertIdlEntToMedEnt(entity),i);
                GROUP_i * f1=new GROUP_i(grou);
                SALOME_MED::GROUP_ptr f2 = f1->POA_SALOME_MED::GROUP::_this();
                f1->_remove_ref();
	        return (SALOME_MED::GROUP::_duplicate(f2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified group of the mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA:
 */
//=============================================================================
SALOME_MED::SUPPORT_ptr MESH_i::getBoundaryElements(SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                SUPPORT * myNewSupport = _mesh->getBoundaryElements(convertIdlEntToMedEnt(entity));
                SUPPORT_i * mySupportI = new SUPPORT_i(myNewSupport);
                SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this() ;
                return (SALOME_MED::SUPPORT::_duplicate(mySupportIOR));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the volume ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr MESH_i::getVolume(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR); try
        {
		int sup = mySupport->getCorbaIndex();
		ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
		const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getVolume( myCppSupport);
		FIELDDOUBLE_i * medf = new FIELDDOUBLE_i(mySupport,f);
		POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i> * f1 = 
		    new POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i>(medf,true);
		SALOME_MED::FIELDDOUBLE_ptr f2 = f1->_this();
                f1->_remove_ref();
	        return (SALOME_MED::FIELD::_duplicate(f2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the volume ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA:
 */
//=============================================================================
SALOME_MED::SUPPORT_ptr MESH_i::getSkin(SALOME_MED::SUPPORT_ptr mySupport3D)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                int sup = mySupport3D->getCorbaIndex();
                ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
                const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                SUPPORT * myNewSupport = _mesh->getSkin(myCppSupport);
                SUPPORT_i * mySupportI = new SUPPORT_i(myNewSupport);
                SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this() ;
                return (SALOME_MED::SUPPORT::_duplicate(mySupportIOR));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the volume ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr MESH_i::getArea(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
		int sup = mySupport->getCorbaIndex();
		ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
		const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getArea( myCppSupport);
		FIELDDOUBLE_i * medf = new FIELDDOUBLE_i(mySupport,f);
		POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i> * f1 = 
		    new POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i>(medf,true);
		SALOME_MED::FIELDDOUBLE_ptr f2 = f1->_this();
                f1->_remove_ref();
	        return (SALOME_MED::FIELD::_duplicate(f2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the area ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr MESH_i::getLength(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
		int sup = mySupport->getCorbaIndex();
		ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
		const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getLength( myCppSupport);
		FIELDDOUBLE_i * medf = new FIELDDOUBLE_i(mySupport,f);
		POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i> * f1 = 
		    new POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i>(medf,true);
		SALOME_MED::FIELDDOUBLE_ptr f2 = f1->_this();
                f1->_remove_ref();
	        return (SALOME_MED::FIELD::_duplicate(f2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the length ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr MESH_i::getNormal(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
		int sup = mySupport->getCorbaIndex();
		ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
		const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getNormal( myCppSupport);
		FIELDDOUBLE_i * medf = new FIELDDOUBLE_i(mySupport,f);
		POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i> * f1 = 
		    new POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i>(medf,true);
		SALOME_MED::FIELDDOUBLE_ptr f2 = f1->_this();
                f1->_remove_ref();
	        return (SALOME_MED::FIELD::_duplicate(f2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the normal ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr MESH_i::getBarycenter(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
		int sup = mySupport->getCorbaIndex();
		ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
		const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getBarycenter( myCppSupport);
		FIELDDOUBLE_i * medf = new FIELDDOUBLE_i(mySupport,f);
		POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i> * f1 = 
		    new POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i>(medf,true);
		SALOME_MED::FIELDDOUBLE_ptr f2 = f1->_this();
                f1->_remove_ref();
	        return (SALOME_MED::FIELD::_duplicate(f2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the barycenter ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr MESH_i::getNeighbourhood(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
	MESSAGE("Not Implemented");
}
//=============================================================================
/*!
 * CORBA: add the Mesh in the StudyManager 
 */
//=============================================================================
void MESH_i::addInStudy(SALOMEDS::Study_ptr myStudy,SALOME_MED::MESH_ptr myIor )
throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
	BEGIN_OF("MED_Mesh_i::addInStudy");
	if ( _meshId != "" )
	{
		MESSAGE("Mesh already in Study");
		    THROW_SALOME_CORBA_EXCEPTION("Mesh already in Study", \
                                 SALOME::BAD_PARAM);
	};

	SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;

 	// Find SComponent labelled 'MED'
	SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        myBuilder->NewCommand();
  	if ( CORBA::is_nil(medfather) ) 
	  THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);

 	// Create SObject labelled 'MESH' if it doesn't already exit
	SALOMEDS::SObject_var medmeshfather = myStudy->FindObject("MEDMESH");
  	if ( CORBA::is_nil(medmeshfather) ) 
	{
    		MESSAGE("Add Object MEDMESH");
    		medmeshfather = myBuilder->NewObject(medfather);
		//medmeshfather.Name = "MEDMESH" ;
                anAttr = myBuilder->FindOrCreateAttribute(medmeshfather, "AttributeName");
                aName = SALOMEDS::AttributeName::_narrow(anAttr);
                aName->SetValue("MEDMESH");

  	} ;

   	MESSAGE("Add a mesh Object under MED/MEDMESH");
  	SALOMEDS::SObject_var newObj = myBuilder->NewObject(medmeshfather);

	ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
        CORBA::ORB_var &orb = init(0,0);
	string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(_mesh->getName().c_str());
  	_meshId = newObj->GetID();
  	myBuilder->CommitCommand();

	END_OF("Mesh_i::addInStudy(SALOMEDS::Study_ptr myStudy)");
}
//=============================================================================
/*!
 * CORBA: add the Mesh in the StudyManager
 */
//=============================================================================
void MESH_i::addInStudy(SALOMEDS::Study_ptr myStudy,SALOME_MED::MESH_ptr myIor,const string & fileName )
throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
        BEGIN_OF("MED_Mesh_i::addInStudy");
        if ( _meshId != "" )
        {
                MESSAGE("Mesh already in Study");
                    THROW_SALOME_CORBA_EXCEPTION("Mesh already in Study", \
                                 SALOME::BAD_PARAM);
        };

        SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;
        SALOMEDS::AttributeComment_var aComment;

        // Find SComponent labelled 'Med'
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        if ( CORBA::is_nil(medfather) )
          THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);

        // Create SObject labelled 'MESH' if it doesn't already exit
        SALOMEDS::SObject_var medmeshfather = myStudy->FindObject("MEDMESH");
        if ( CORBA::is_nil(medmeshfather) )
        {
                MESSAGE("Add Object MEDMESH");
                medmeshfather = myBuilder->NewObject(medfather);
                //medmeshfather.Name = "MEDMESH" ;
                anAttr = myBuilder->FindOrCreateAttribute(medmeshfather, "AttributeName");
                aName = SALOMEDS::AttributeName::_narrow(anAttr);
                aName->SetValue("MEDMESH");

        } ;

        MESSAGE("Add a mesh Object under MED/MEDMESH");
        myBuilder->NewCommand();
        SALOMEDS::SObject_var newObj = myBuilder->NewObject(medmeshfather);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
        CORBA::ORB_var &orb = init(0,0);
        string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(_mesh->getName().c_str());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeComment");
        aComment = SALOMEDS::AttributeComment::_narrow(anAttr);
        aComment->SetValue(fileName.c_str());
        _meshId = newObj->GetID();
        myBuilder->CommitCommand();

        END_OF("Mesh_i::addInStudy(SALOMEDS::Study_ptr myStudy)");
}
//=============================================================================

//=============================================================================
/*!
 * CORBA: write mesh in a med file
 */
//=============================================================================
void MESH_i::write(CORBA::Long i, const char* driverMeshName) 	     
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                _mesh->write(i,driverMeshName);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to write the mesh ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: read mesh in a med file
 */
//=============================================================================
void MESH_i::read(CORBA::Long i) 	     
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                _mesh->read(i);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to read the mesh ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA : release driver
 */
//=============================================================================
void MESH_i::rmDriver(CORBA::Long i) 	     
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                _mesh->rmDriver();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to unlink the mesh from the driver ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA : attach driver
 */
//=============================================================================
CORBA::Long MESH_i::addDriver(SALOME_MED::medDriverTypes driverType, 
                              const char* fileName, const char* meshName)
throw (SALOME::SALOME_Exception)
{
	if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                int drivernum=_mesh->addDriver(
                                        convertIdlDriverToMedDriver(driverType),
                                        fileName,
                                        meshName);
                return drivernum;
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to link the mesh to the driver ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

