//  MED MedMem : MED idl descriptions implementation based on the classes of MEDMEM
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
//  File   : Support_i.cxx
//  Author : EDF
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Support_i.cxx

using namespace std;
#include "utilities.h"
#include "Utils_CorbaException.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)


#include "MEDMEM_define.hxx"
#include "MEDMEM_Support.hxx"

#include "Support_i.hxx"
#include "Mesh_i.hxx"
#include "convert.hxx"

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
	catch(...)
	{
                MESSAGE("Unable to access the name of the support ");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
						,SALOME::INTERNAL_ERROR);
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
	catch(...)
	{
                MESSAGE("Unable to access the description of the support ");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
						,SALOME::INTERNAL_ERROR);
	}
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
        catch(...)
        {
                MESSAGE("Unable to access the assoicated mesh");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
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
	catch(...)
	{
                MESSAGE("Unable to access the type of the support");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
						,SALOME::INTERNAL_ERROR);
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
  catch(...)
    {
      MESSAGE("Unable to access support s entity");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object", \
				   SALOME::INTERNAL_ERROR);
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
	catch(...)
        {
      		MESSAGE("Unable to access support different types");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object", \
                                              SALOME::INTERNAL_ERROR);
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
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        try
        {
		return _support->getNumberOfElements(convertIdlEltToMedElt(geomElement));
	}
	catch(...)
        {
      		MESSAGE("Unable to access the number of support different types");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
	}

}

//=============================================================================
/*!
 * CORBA: get Nodes 
 */
//=============================================================================

Engines::long_array *  SUPPORT_i::getNumber(SALOME_MED::medGeometryElement geomElement) 
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        Engines::long_array_var myseq= new Engines::long_array;
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
        catch(...)
        {
      		MESSAGE("Unable to access the support optionnal index");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
	
}
//=============================================================================
/*!
 * CORBA: Global Nodes Index (optionnaly designed by the user)
 * CORBA:  ??????????????????????????????
 */
//=============================================================================

Engines::long_array *  SUPPORT_i::getNumberIndex()
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        Engines::long_array_var myseq= new Engines::long_array;
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
        catch(...)
        {
      		MESSAGE("Unable to access the support index");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
	
}

//=============================================================================
/*!
 * CORBA: Array containing indexes for elements included in the support  
 */
//=============================================================================

CORBA::Long SUPPORT_i::getNumberOfGaussPoints(SALOME_MED::medGeometryElement geomElement)
throw (SALOME::SALOME_Exception)
{
	if (_support==NULL)
		THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				             SALOME::INTERNAL_ERROR);
        try
        {
		return _support->getNumberOfGaussPoint(convertIdlEltToMedElt(geomElement));
	}
	catch(...)
        {
      		MESSAGE("Unable to access number of Gauss points");
		THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
	}
}

//=============================================================================
/*!
 * CORBA: add the Support in the StudyManager 
 */
//=============================================================================
void SUPPORT_i::addInStudy (SALOMEDS::Study_ptr myStudy, SALOME_MED::SUPPORT_ptr myIor)
  throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection)
{
  BEGIN_OF("SUPPORT_i::addInStudy");

  if ( _supportId != "" )
    {
      MESSAGE("Support already in Study");
      THROW_SALOME_CORBA_EXCEPTION("Support already in Study", \
				   SALOME::BAD_PARAM);
    };
  
  SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributeIOR_var     aIOR;
  
  // Find SComponent labelled 'Med'
  MESSAGE("Find SComponent labelled 'Med'");
  SALOMEDS::SComponent_var medfather = myStudy->FindComponent("Med");
  if ( CORBA::is_nil(medfather) ) 
    THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);
  
  // Find SObject labelled 'MEDMESH'
  MESSAGE("Find SObject labelled 'MEDMESH'");
  cerr<<flush;
  MESSAGE(flush);
  if ( CORBA::is_nil(myStudy) ) 
    THROW_SALOME_CORBA_EXCEPTION("Study deleted !!!",SALOME::INTERNAL_ERROR);
  SALOMEDS::SObject_var medmeshfather = myStudy->FindObject("MEDMESH");
  MESSAGE("Find SObject labelled 'MEDMESH' 2");
  cerr<<flush;
  MESSAGE(flush);
  if ( CORBA::is_nil(medmeshfather) ){ 
    MESSAGE( "On a leve l exception" << flush);
    THROW_SALOME_CORBA_EXCEPTION("SObject labelled 'MEDMESH' not Found",SALOME::INTERNAL_ERROR);
  }
  // Find SObject MESH (represent mesh in support)
  MESSAGE( "Find SObject MESH (represent mesh in support)"<< flush);
  cerr<<flush;
  MESSAGE(flush);
  string meshName = getMesh()->getName() ;
  SALOMEDS::SObject_var medsupportfather = myStudy->FindObject(meshName.c_str()); // seulement sous Med : il peut y avoir le meme sous SMESH !!!
  if ( CORBA::is_nil(medsupportfather) ) 
    THROW_SALOME_CORBA_EXCEPTION("SObject Mesh in Support not Found",SALOME::INTERNAL_ERROR);
  // perhaps add MESH automatically ?
  
  MESSAGE("Add a support Object under MED/MESH/MESHNAME");
  cerr<<flush;
  MESSAGE(flush);
  myBuilder->NewCommand();

  SALOMEDS::SObject_var supportEntry = myStudy->FindObject(_support->getName().c_str()); // c'est pas bon, car il faut rechercher uniquement sous le bon MESH !!!
  if ( CORBA::is_nil(supportEntry) ) { 
    // not already in study : we create it !
    SALOMEDS::SObject_var newObj = myBuilder->NewObject(medsupportfather);
    
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
  } else {
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
  
  END_OF("SUPPORT_i::addInStudy");
}
