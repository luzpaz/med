//=============================================================================
// File      : MEDMEM_Support_i.cxx
// Project   : SALOME
// Author    : EDF
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Support_i.cxx
//=============================================================================

#include "utilities.h"
#include "Utils_CorbaException.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)


#include "MEDMEM_define.hxx"
#include "MEDMEM_Support.hxx"

#include "MEDMEM_Support_i.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_convert.hxx"
using namespace MEDMEM;

// Initialisation des variables statiques
map < int, ::SUPPORT *> SUPPORT_i::supportMap ;
int SUPPORT_i::supportIndex = 0 ;


//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
SUPPORT_i::SUPPORT_i() :_support((::SUPPORT *)NULL)
{
	BEGIN_OF("Default Constructor SUPPORT_i");
	END_OF("Default Constructor SUPPORT_i");
}

//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SUPPORT_i::SUPPORT_i(const ::SUPPORT * const s) :_support(s),
	           _corbaIndex(SUPPORT_i::supportIndex++)
{
	BEGIN_OF("Constructor SUPPORT_i");
	SUPPORT_i::supportMap[_corbaIndex]=(::SUPPORT *)_support;
	END_OF("Constructor SUPPORT_i");
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SUPPORT_i::SUPPORT_i(const SUPPORT_i &s) :_support(s._support),
	                    _corbaIndex(SUPPORT_i::supportIndex++)
{
	BEGIN_OF("Constructor SUPPORT_i");
	SUPPORT_i::supportMap[_corbaIndex]=(::SUPPORT *)_support;
	END_OF("Constructor SUPPORT_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================

SUPPORT_i::~SUPPORT_i()
{
}
//=============================================================================
/*!
 * CORBA: Accessor for Corba Index 
 */
//=============================================================================

CORBA::Long SUPPORT_i::getCorbaIndex()
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
	return _corbaIndex;
}

//=============================================================================
/*!
 * CORBA: Accessor for Name 
 */
//=============================================================================

char * SUPPORT_i::getName() 	
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
	try
	{
		return CORBA::string_dup(_support->getName().c_str());
	}
	catch (MEDEXCEPTION &ex)
	{
                MESSAGE("Unable to access the name of the support ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}

}

//=============================================================================
/*!
 * CORBA: Accessor for Description 
 */
//=============================================================================

char*  SUPPORT_i::getDescription()
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
	try
	{
  		return CORBA::string_dup(_support->getDescription().c_str());
	}
	catch (MEDEXCEPTION &ex)
	{
                MESSAGE("Unable to access the description of the support ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}

//=============================================================================
/*!
 * CORBA: Accessor for Description and Name
 */
//=============================================================================

SALOME_MED::SUPPORT::supportInfos * SUPPORT_i::getSupportGlobal()
throw (SALOME::SALOME_Exception)
{
        if (_support==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::SUPPORT::supportInfos_var all = new SALOME_MED::SUPPORT::supportInfos;
        try
        {
                all->name               = CORBA::string_dup(_support->getName().c_str());
                all->description        = CORBA::string_dup(_support->getDescription().c_str());
                const int numberOfTypes = _support->getNumberOfTypes();
                all->numberOfGeometricType = numberOfTypes;

                all->types.length(numberOfTypes);
                all->nbEltTypes.length(numberOfTypes);
                const medGeometryElement * elemts = _support->getTypes();
                for (int i=0;i<numberOfTypes;i++)
                {
                        all->types[i]      = convertMedEltToIdlElt(elemts[i]);
                        all->nbEltTypes[i] = _support->getNumberOfElements(elemts[i]);
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to access the description of the support ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return all._retn();

}


//=============================================================================
/*!
 * CORBA: Accessor for Mesh 
 */
//=============================================================================

SALOME_MED::MESH_ptr SUPPORT_i::getMesh()
throw (SALOME::SALOME_Exception)
{
  BEGIN_OF("SALOME_MED::MESH_ptr SUPPORT_i::getMesh()");

	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        try
        {
		MESH * mesh = _support->getMesh();

		SCRUTE(mesh) ;

		MESH_i * m1 = new MESH_i(mesh);
		SALOME_MED::MESH_ptr m2 = m1->POA_SALOME_MED::MESH::_this();
		MESSAGE("SALOME_MED::MESH_ptr SUPPORT_i::getMesh() checking des pointeurs CORBA");

		SCRUTE(m1);
		SCRUTE(m2);

		m1->_remove_ref();

		END_OF("SALOME_MED::MESH_ptr SUPPORT_i::getMesh()");

	        return (SALOME_MED::MESH::_duplicate(m2));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to access the assoicated mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: boolean indicating if support concerns all elements 
 */
//=============================================================================

CORBA::Boolean SUPPORT_i::isOnAllElements()
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
	try
	{
		return _support->isOnAllElements();
	}
	catch (MEDEXCEPTION &ex)
	{
                MESSAGE("Unable to access the type of the support");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}

//=============================================================================
/*!
 * CORBA:
 */
//=============================================================================
CORBA::Long SUPPORT_i::getNumberOfTypes()
throw (SALOME::SALOME_Exception)
{
        if (_support==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _support->getNumberOfTypes();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to access  number of support different types");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }

}


//=============================================================================
/*!
 * CORBA: Accessor for type of support's entity 
 */
//=============================================================================

SALOME_MED::medEntityMesh SUPPORT_i::getEntity() 
throw (SALOME::SALOME_Exception)
{
  BEGIN_OF("SALOME_MED::medEntityMesh SUPPORT_i::getEntity()");

  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
  try
    {
      END_OF("SALOME_MED::medEntityMesh SUPPORT_i::getEntity()");
      return convertMedEntToIdlEnt(_support->getEntity());
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to access support s entity");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
}

//=============================================================================
/*!
 * CORBA: Accessor for types of geometry elements 
 */
//=============================================================================

SALOME_MED::medGeometryElement_array * SUPPORT_i::getTypes() 
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
	SALOME_MED::medGeometryElement_array_var myseq = new SALOME_MED::medGeometryElement_array;
	try
	{
		int mySeqLength=_support->getNumberOfTypes();
		myseq->length(mySeqLength);
		const medGeometryElement * elemts = _support->getTypes();
		for (int i=0;i<mySeqLength;i++)
		{
			myseq[i]=convertMedEltToIdlElt(elemts[i]);
		}
        }
	catch (MEDEXCEPTION &ex)
        {
      		MESSAGE("Unable to access support different types");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}

//=============================================================================
/*!
 * CORBA: Number of different types of geometry elements
 *        existing in the support
 */
//=============================================================================
CORBA::Long SUPPORT_i::getNumberOfElements(SALOME_MED::medGeometryElement geomElement) 
throw (SALOME::SALOME_Exception)
{

  SCRUTE(geomElement);
  SCRUTE(SALOME_MED::MED_ALL_ELEMENTS);

	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        try
        {
		return _support->getNumberOfElements(convertIdlEltToMedElt(geomElement));
	}
	catch (MEDEXCEPTION &ex)
        {
      		MESSAGE("Unable to access the number of support different types");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}

}

//=============================================================================
/*!
 * CORBA: get Nodes 
 */
//=============================================================================

SALOME_MED::long_array *  SUPPORT_i::getNumber(SALOME_MED::medGeometryElement geomElement) 
throw (SALOME::SALOME_Exception)
{
  SCRUTE(_support);
  SCRUTE(geomElement);
  SCRUTE(convertIdlEltToMedElt(geomElement));

	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
                int nbelements=_support->getNumberOfElements(convertIdlEltToMedElt(geomElement));
                myseq->length(nbelements);
SCRUTE(_support->getName());
SCRUTE(nbelements);
SCRUTE(convertIdlEltToMedElt(geomElement));
                const int * numbers=_support->getNumber(convertIdlEltToMedElt(geomElement));
                for (int i=0;i<nbelements;i++)
                {
                        myseq[i]=numbers[i];
SCRUTE(numbers[i]);
                }
        }
        catch (MEDEXCEPTION &ex)
        {
      		MESSAGE("Unable to access the support optionnal index");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
	
}
//=============================================================================
/*!
 * CORBA: Global Nodes Index (optionnaly designed by the user)
 * CORBA:  ??????????????????????????????
 */
//=============================================================================

SALOME_MED::long_array *  SUPPORT_i::getNumberIndex()
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
		MESSAGE ("Nombre d'elements  mis de façon stupide a MED_ALL_ELEMENTS");
                int nbelements=_support->getNumberOfElements(::MED_ALL_ELEMENTS);
                myseq->length(nbelements);
                const int * numbers=_support->getNumberIndex();
                for (int i=0;i<nbelements;i++)
                {
                        myseq[i]=numbers[i];
                }
        }
        catch (MEDEXCEPTION &ex)
        {
      		MESSAGE("Unable to access the support index");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
	
}
//=============================================================================
/*!
 * CORBA:
 */
//=============================================================================

void SUPPORT_i::getBoundaryElements()
throw (SALOME::SALOME_Exception)
{
        if (_support==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                (const_cast< ::SUPPORT *>(_support))->getBoundaryElements();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to access elements");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}


//=============================================================================
/*!
 * CORBA: Array containing indexes for elements included in the support  
 */
//=============================================================================

CORBA::Long SUPPORT_i::getNumberOfGaussPoint(SALOME_MED::medGeometryElement geomElement)
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        try
        {
		return _support->getNumberOfGaussPoint(convertIdlEltToMedElt(geomElement));
	}
	catch (MEDEXCEPTION &ex)
        {
      		MESSAGE("Unable to access number of Gauss points");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
	}
}
//=============================================================================
/*!
 * CORBA: Global Nodes Index (optionnaly designed by the user)
 */
//=============================================================================
SALOME_MED::long_array *  SUPPORT_i::getNumbersOfGaussPoint()
throw (SALOME::SALOME_Exception)
{
        if (_support==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq= new SALOME_MED::long_array;
        try
        {
                int mySeqLength=_support->getNumberOfTypes();
                myseq->length(mySeqLength);
                const medGeometryElement * elemts = _support->getTypes();
                for (int i=0;i<mySeqLength;i++)
                {
                        myseq[i]= _support->getNumberOfGaussPoint(elemts[i]);
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to access number of Gauss points");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: add the Support in the StudyManager 
 */
//=============================================================================
void SUPPORT_i::addInStudy (SALOMEDS::Study_ptr myStudy, SALOME_MED::SUPPORT_ptr myIor)
  throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
  const char * LOC = "SUPPORT_i::addInStudy";
  BEGIN_OF(LOC);

  if ( _supportId != "" )
  {
      MESSAGE(LOC << "Support already in Study");
      THROW_SALOME_CORBA_EXCEPTION("Support already in Study", \
				   SALOME::BAD_PARAM);
  };

  if ( CORBA::is_nil(myStudy) )
  {
      MESSAGE(LOC << "Study not found");
      THROW_SALOME_CORBA_EXCEPTION("Study deleted !!!",
                                    SALOME::INTERNAL_ERROR);
  }

  
  SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributeIOR_var     aIOR;
  
  // Find SComponent labelled 'Med'
  MESSAGE(LOC << " Find SComponent labelled 'MED'");
  SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
  if ( CORBA::is_nil(medfather) ) 
  { 
    MESSAGE(LOC << "MED not found");
    THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);
  }

  // Find SObject MESH (represent mesh in support)
  SALOMEDS::SObject_var medmeshfather = myStudy->FindObjectByPath("/Med/MEDMESH");
  if ( CORBA::is_nil(medmeshfather) )
  { 
    MESSAGE(LOC << " No /Med/MEDMESH Found in study")
    THROW_SALOME_CORBA_EXCEPTION("SObject labelled 'MEDMESH' not Found",SALOME::INTERNAL_ERROR);
  }
  MESSAGE(LOC << " Find SObject MESH (represent mesh in support)");

  string meshName = getMesh()->getName() ;
  string meshNameStudy = meshName;

  for (string::size_type pos=0; pos<meshNameStudy.size();++pos)
    {
      if (isspace(meshNameStudy[pos])) meshNameStudy[pos] = '_';
    }

		// seulement sous Med : il peut y avoir le meme sous SMESH !!!
  SALOMEDS::SObject_var medsupportfather = myStudy->FindObject(meshName.c_str()); 
  if ( CORBA::is_nil(medsupportfather) ) 
    THROW_SALOME_CORBA_EXCEPTION("SObject Mesh in Support not Found",SALOME::INTERNAL_ERROR);
  // perhaps add MESH automatically ?
  
  MESSAGE("Add a support Object under /Med/MESH/MESHNAME");

  char * medsupfatherName;
  int lenName = 15 + strlen(meshName.c_str());
  medsupfatherName = new char[lenName];
  medsupfatherName = strcpy(medsupfatherName,"MEDSUPPORTS_OF_");
  medsupfatherName = strcat(medsupfatherName,meshNameStudy.c_str());

  SCRUTE(medsupfatherName);

  SALOMEDS::SObject_var medsupfather = myStudy->FindObject(medsupfatherName);
  if ( CORBA::is_nil(medsupfather) )
  {
                MESSAGE("Add Object MEDSUPPORT");
                medsupfather = myBuilder->NewObject(medmeshfather);
                anAttr = myBuilder->FindOrCreateAttribute(medsupfather, "AttributeName");
                aName = SALOMEDS::AttributeName::_narrow(anAttr);
                aName->SetValue(medsupfatherName);

  } ;

  //myBuilder->NewCommand();

  string supportName = _support->getName();

  SCRUTE(supportName);

  SCRUTE(meshNameStudy);

  char * supportEntryPath;
  lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str())+1;
  supportEntryPath = new char[lenName];
  supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
  supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
  supportEntryPath = strcat(supportEntryPath,meshNameStudy.c_str());
  supportEntryPath = strcat(supportEntryPath,"/");
  supportEntryPath = strcat(supportEntryPath,supportName.c_str());

  //SCRUTE(supportEntryPath);

  MESSAGE("supportEntryPath in support " << supportEntryPath << " length " << lenName);

//   SALOMEDS::SObject_var supportEntry = myStudy->FindObject(_support->getName().c_str());
			 // c'est pas bon, car il faut rechercher uniquement sous le bon MESH !!!
  SALOMEDS::SObject_var supportEntry = myStudy->FindObjectByPath(supportEntryPath);

  if ( CORBA::is_nil(supportEntry) ) 
  {
    MESSAGE(" supportEntry is a nil corba object and is going to be created");
    // not already in study : we create it !
    SALOMEDS::SObject_var newObj = myBuilder->NewObject(medsupfather);
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
    CORBA::ORB_var &orb = init(0,0);
    string iorStr = orb->object_to_string(myIor);
    anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
    aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    aIOR->SetValue(iorStr.c_str());
    anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue(_support->getName().c_str());
    _supportId = newObj->GetID();
  } 
  else 
  {
    // already in study : put new AttributeIOR !
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
    CORBA::ORB_var &orb = init(0,0);
    string iorStr = orb->object_to_string(myIor);
    anAttr = myBuilder->FindOrCreateAttribute(supportEntry, "AttributeIOR");
    aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    aIOR->SetValue(iorStr.c_str());
  }
  myBuilder->CommitCommand();

  SALOMEDS::SObject_var supportEntryBis = myStudy->FindObjectByPath(supportEntryPath);

  MESSAGE("Just for checking, reuse of the corba pointer");

  if ( CORBA::is_nil(supportEntry) ) 
    {
      MESSAGE("The reuse is OK");
    }
  else 
    {
      MESSAGE("well !! the reuse is not OK and there was a problem in the storage in the study");
    }

  delete [] medsupfatherName;
  delete [] supportEntryPath;

  END_OF(LOC);
}
