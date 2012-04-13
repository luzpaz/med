// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

//=============================================================================
// File      : MEDMEM_Support_i.cxx
// Project   : SALOME
// Author    : EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Support_i.cxx
//=============================================================================
//
#include "utilities.h"
#include "Utils_CorbaException.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)


#include "MEDMEM_define.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"

#include "MEDMEM_Support_i.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_convert.hxx"

#include "SenderFactory.hxx"
#include "MultiCommException.hxx"
using namespace MEDMEM;

// Initialisation des variables statiques
map < int, ::SUPPORT *> SUPPORT_i::supportMap ;
int SUPPORT_i::supportIndex = 0 ;


//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
SUPPORT_i::SUPPORT_i() :_support((::SUPPORT *)NULL),_corbaIndex(SUPPORT_i::supportIndex++)
{
}

//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SUPPORT_i::SUPPORT_i(const ::SUPPORT * const s) :_support(s),
                                                 _corbaIndex(SUPPORT_i::supportIndex++)
{
  if ( _support ) _support->addReference();
  SUPPORT_i::supportMap[_corbaIndex]=(::SUPPORT *)_support;
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SUPPORT_i::SUPPORT_i(const SUPPORT_i &s) :_support(s._support),
                                          _corbaIndex(SUPPORT_i::supportIndex++)
{
  if ( _support ) _support->addReference();
  SUPPORT_i::supportMap[_corbaIndex]=(::SUPPORT *)_support;
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================

SUPPORT_i::~SUPPORT_i()
{
  if ( SUPPORT_i::supportMap[_corbaIndex] )
    SUPPORT_i::supportMap[_corbaIndex]->removeReference();
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  SALOME_MED::SUPPORT::supportInfos_var all = new SALOME_MED::SUPPORT::supportInfos;
  try
  {
    all->name               = CORBA::string_dup(_support->getName().c_str());
    all->description        = CORBA::string_dup(_support->getDescription().c_str());
    const int numberOfTypes = _support->getNumberOfTypes();
    all->numberOfGeometricType = numberOfTypes;
    all->entity = _support->getEntity();
    all->isOnAllElements    = _support->isOnAllElements();
    all->types.length(numberOfTypes);
    all->nbEltTypes.length(numberOfTypes);
    all->nodalConnectivityLength.length(numberOfTypes);
    const medGeometryElement * types = _support->getTypes();
    const MESH* mesh = _support->getMesh()->convertInMESH();
    for (int i=0;i<numberOfTypes;i++)
    {
      int nbelements = _support->getNumberOfElements(types[i]);
      int connLength = 0;
      switch ( types[i] )
      {
      case MED_EN::MED_POLYGON: 
      case MED_EN::MED_POLYHEDRA: 
{
        if (_support->isOnAllElements() ) 
{
          connLength = mesh->getConnectivityLength(MED_EN::MED_NODAL,
                                                   _support->getEntity(),MED_EN::MED_ALL_ELEMENTS);
        }
        else 
{
          const int * index = mesh->getConnectivityIndex(MED_EN::MED_NODAL,_support->getEntity());
          const int * numbers=_support->getNumber(types[i]);
          for (int j=0;j<nbelements;j++)
          {
            int elem = numbers[j] ;
            connLength += index[elem] - index[elem-1];
          }
        }
        break;
      }
      default:
        connLength = nbelements * ( types[i] % 100 );
      }
      all->types[i]                   = convertMedEltToIdlElt(types[i]);
      all->nbEltTypes[i]              = nbelements;
      all->nodalConnectivityLength[i] = connLength;
    }
    mesh->removeReference();
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

SALOME_MED::GMESH_ptr SUPPORT_i::getMesh()
  throw (SALOME::SALOME_Exception)
{
  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  try
  {
    const GMESH * mesh = _support->getMesh();

    SCRUTE(mesh) ;

    GMESH_i * m1 = new GMESH_i(const_cast<GMESH*>( mesh ));
    SALOME_MED::GMESH_ptr m2 = m1->_this();
    MESSAGE("SALOME_MED::GMESH_ptr SUPPORT_i::getMesh() checking des pointeurs CORBA");

    SCRUTE(m1);
    SCRUTE(m2);

    return (m2);
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
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
  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  try
  {
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
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

SALOME_TYPES::ListOfLong *  SUPPORT_i::getNumber(SALOME_MED::medGeometryElement geomElement) 
  throw (SALOME::SALOME_Exception)
{
  SCRUTE(_support);
  SCRUTE(geomElement);
  SCRUTE(convertIdlEltToMedElt(geomElement));

  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
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
 * CORBA: get Nodes from file
 */
//=============================================================================
SALOME_TYPES::ListOfLong *  SUPPORT_i::getNumberFromFile(SALOME_MED::medGeometryElement geomElement) 
  throw (SALOME::SALOME_Exception)
{
  SCRUTE(_support);
  SCRUTE(geomElement);
  SCRUTE(convertIdlEltToMedElt(geomElement));

  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
  try
  {
    int nbelements=_support->getNumberOfElements(convertIdlEltToMedElt(geomElement));
    myseq->length(nbelements);
    SCRUTE(_support->getName());
    SCRUTE(nbelements);
    SCRUTE(convertIdlEltToMedElt(geomElement));
    const int * numbers=_support->getNumberFromFile(convertIdlEltToMedElt(geomElement));
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
 * CORBA: 2nd get Nodes 
 */
//=============================================================================
SALOME::SenderInt_ptr SUPPORT_i::getSenderForNumber(SALOME_MED::medGeometryElement geomElement) 
  throw (SALOME::SALOME_Exception)
{
  SCRUTE(_support);
  SCRUTE(geomElement);
  SCRUTE(convertIdlEltToMedElt(geomElement));
  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  SALOME::SenderInt_ptr ret;
  try
  {
    int nbelements=_support->getNumberOfElements(convertIdlEltToMedElt(geomElement));
    const int * numbers=_support->getNumber(convertIdlEltToMedElt(geomElement));
    ret=SenderFactory::buildSender(*this,numbers,nbelements);
  }
  catch (MEDEXCEPTION &ex)
  {
    MESSAGE("Unable to access the support optionnal index");
    THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
  }
  catch(MultiCommException &ex2)
    THROW_SALOME_CORBA_EXCEPTION(ex2.what(),SALOME::INTERNAL_ERROR);
  return ret;
}
//=============================================================================
/*!
 * CORBA: Global Nodes Index (optionnaly designed by the user)
 * CORBA:  ??????????????????????????????
 */
//=============================================================================

SALOME_TYPES::ListOfLong *  SUPPORT_i::getNumberIndex()
  throw (SALOME::SALOME_Exception)
{
  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
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
 * CORBA: 2nd Global Nodes Index (optionnaly designed by the user)
 */
//=============================================================================

SALOME::SenderInt_ptr SUPPORT_i::getSenderForNumberIndex() 
  throw (SALOME::SALOME_Exception)
{
  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  SALOME::SenderInt_ptr ret;
  try
  {
    MESSAGE ("Nombre d'elements  mis de façon stupide a MED_ALL_ELEMENTS");
    int nbelements=_support->getNumberOfElements(::MED_ALL_ELEMENTS);
    const int * numbers=_support->getNumberIndex();
    ret=SenderFactory::buildSender(*this,numbers,nbelements);
  }
  catch (MEDEXCEPTION &ex)
  {
    MESSAGE("Unable to access the support index");
    THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
  }
  catch(MultiCommException &ex2)
    THROW_SALOME_CORBA_EXCEPTION(ex2.what(),SALOME::INTERNAL_ERROR);
  return ret;
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
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", SALOME::INTERNAL_ERROR);
  try
  {
    (const_cast< ::SUPPORT *>(_support))->getBoundaryElements();
  }
  catch (MEDEXCEPTION &)
  {
    MESSAGE("Unable to access elements");
    THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object",
                                 SALOME::INTERNAL_ERROR);
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
  if ( _supportId != "" )
  {
    MESSAGE("Support already in Study");
    THROW_SALOME_CORBA_EXCEPTION("Support already in Study", SALOME::BAD_PARAM);
  };

  if ( CORBA::is_nil(myStudy) )
  {
    MESSAGE("Study not found");
    THROW_SALOME_CORBA_EXCEPTION("Study deleted !!!",
                                 SALOME::INTERNAL_ERROR);
  }


  SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributeIOR_var     aIOR;

  // Find SComponent labelled 'Med'
  MESSAGE(" Find SComponent labelled 'MED'");
  SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
  if ( CORBA::is_nil(medfather) ) 
  {
    MESSAGE("MED not found");
    THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);
  }

  // Find SObject MESH (represent mesh in support)
  SALOMEDS::SObject_var medmeshfather = myStudy->FindObjectByPath("/Med/MEDMESH");
  if ( CORBA::is_nil(medmeshfather) )
  {
    MESSAGE(" No /Med/MEDMESH Found in study")
      THROW_SALOME_CORBA_EXCEPTION("SObject labelled 'MEDMESH' not Found",SALOME::INTERNAL_ERROR);
  }
  MESSAGE(" Find SObject MESH (represent mesh in support)");

  string meshName = _support->getMeshName() ;
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
  int lenName = 15 + strlen(meshName.c_str()) + 1;
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

  string supportEntryPath = getEntryPath( _support );

  SALOMEDS::SObject_var supportEntry = myStudy->FindObjectByPath(supportEntryPath.c_str());



  if ( CORBA::is_nil(supportEntry) ) 
  {
    MESSAGE(" supportEntry is a nil corba object and is going to be created");
    // not already in study : we create it !
    SALOMEDS::SObject_var newObj = myBuilder->NewObject(medsupfather);
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
    CORBA::ORB_var &orb = init(0,0);
    CORBA::String_var iorStr = orb->object_to_string(myIor);
    anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
    aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    aIOR->SetValue(iorStr.in());
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
    CORBA::String_var iorStr = orb->object_to_string(myIor);
    anAttr = myBuilder->FindOrCreateAttribute(supportEntry, "AttributeIOR");
    aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    aIOR->SetValue(iorStr.in());
  }
  myBuilder->CommitCommand();

  SALOMEDS::SObject_var supportEntryBis = myStudy->FindObjectByPath(supportEntryPath.c_str());

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


  // register the Corba pointer: increase the referrence count
  MESSAGE("Registering of the Corba Support pointer");
  Register();
}

//=======================================================================
//function : getEntryPath
//purpose  : 
//=======================================================================

string SUPPORT_i::getEntryPath(const ::MEDMEM::SUPPORT * aSupport)
{
  string meshNameStudy( aSupport->getMeshName() );
  for (string::size_type pos=0; pos<meshNameStudy.size();++pos)
    if (isspace(meshNameStudy[pos])) meshNameStudy[pos] = '_';

  string supportName = aSupport->getName();
  string supportNameStudy( supportName.c_str(), strlen( supportName.c_str() ));
  string supportEntryPath =
    "/Med/MEDMESH/MEDSUPPORTS_OF_" + meshNameStudy + "/" + supportNameStudy;
  SCRUTE( supportEntryPath );

  return supportEntryPath;
}
