// Copyright (C) 2015  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include "MED.hxx"

#include "MEDFactoryClient.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include <SALOMEDS_SObject.hxx>

#include <Utils_ExceptHandlers.hxx>
#include <TCollection_AsciiString.hxx>

#include <string>

/*!
  \brief Constructor

  Creates an instance of the MED component engine

  \param orb reference to the ORB
  \param poa reference to the POA
  \param contId CORBA object ID, pointing to the owner SALOME container
  \param instanceName SALOME component instance name
  \param interfaceName SALOME component interface name
*/
MED::MED(CORBA::ORB_ptr orb,
         PortableServer::POA_ptr poa,
         PortableServer::ObjectId* contId,
         const char* instanceName,
         const char* interfaceName)
  : Engines_Component_i(orb, poa, contId, instanceName, interfaceName),
    _fieldSeriesEntries()
{
  _thisObj = this;
  _id = _poa->activate_object(_thisObj); // register and activate this servant object
}

MED::~MED()
{
  // nothing to do
}

// Duplicate gui/DatasourceConstants
#define OBJECT_ID              "objectid"
#define OBJECT_IS_IN_WORKSPACE "isInWorkspace"

// Duplicate gui/XmedDataModel
static const int NB_TYPE_OF_FIELDS = 4;
static const char* mapTypeOfFieldLabel[NB_TYPE_OF_FIELDS] =
  {"ON_CELLS", "ON_NODES", "ON_GAUSS_PT", "ON_GAUSS_NE" };

MED_ORB::status
MED::addDatasourceToStudy(SALOMEDS::Study_ptr study,
                          const MEDCALC::DatasourceHandler& datasourceHandler)
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  // set result status to error initially
  MED_ORB::status result = MED_ORB::OP_ERROR;

  // check if reference to study is valid
  if (!CORBA::is_nil(study)) {
    // get full object path
    std::string fullName = CORBA::string_dup(datasourceHandler.name);
    // check if the object with the same name is already registered in the study
    SALOMEDS::SObject_var sobj = study->FindObjectByPath(fullName.c_str());
    if (CORBA::is_nil(sobj)) {
      // object is not registered yet -> register
      SALOMEDS::GenericAttribute_var anAttr;
      SALOMEDS::AttributeParameter_var aParam;
      SALOMEDS::StudyBuilder_var studyBuilder = study->NewBuilder();
      SALOMEDS::UseCaseBuilder_var useCaseBuilder = study->GetUseCaseBuilder();

      // find MED component; create it if not found
      SALOMEDS::SComponent_var father = study->FindComponent("MED");
      if (CORBA::is_nil(father)) {
        // create component
        father = studyBuilder->NewComponent("MED");
        // set name attribute
        father->SetAttrString("AttributeName", "MEDCalc");
        // set icon attribute
        father->SetAttrString("AttributePixMap", "ICO_MED");
        // register component in the study
        studyBuilder->DefineComponentInstance(father, MED_Gen::_this());
        // add component to the use case tree
        // (to support tree representation customization and drag-n-drop)
        useCaseBuilder->SetRootCurrent();
        useCaseBuilder->Append(father); // component object is added as the top level item
      }

      // create new sub-object, as a child of the component object
      SALOMEDS::SObject_var soDatasource = studyBuilder->NewObject(father);
      soDatasource->SetAttrString("AttributeName", fullName.c_str());
      soDatasource->SetAttrString("AttributePixMap", "ICO_DATASOURCE");
      anAttr = studyBuilder->FindOrCreateAttribute(soDatasource, "AttributeParameter");
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      aParam->SetInt(OBJECT_ID, datasourceHandler.id);
      useCaseBuilder->AppendTo(soDatasource->GetFather(), soDatasource);

      // We can add the meshes as children of the datasource
      MEDCALC::MeshHandlerList* meshHandlerList =
        MEDFactoryClient::getDataManager()->getMeshList(datasourceHandler.id);

      for(CORBA::ULong iMesh=0; iMesh<meshHandlerList->length(); iMesh++) {
        MEDCALC::MeshHandler meshHandler = (*meshHandlerList)[iMesh];
        SALOMEDS::SObject_var soMesh = studyBuilder->NewObject(soDatasource);
        soMesh->SetAttrString("AttributeName", meshHandler.name);
        soMesh->SetAttrString("AttributePixMap", "ICO_DATASOURCE_MESH");
        anAttr = studyBuilder->FindOrCreateAttribute(soMesh, "AttributeParameter");
        aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
        aParam->SetInt(OBJECT_ID, meshHandler.id);
        anAttr = studyBuilder->FindOrCreateAttribute(soMesh, "AttributeParameter");
        aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
        aParam->SetBool(OBJECT_IS_IN_WORKSPACE, false);
        useCaseBuilder->AppendTo(soMesh->GetFather(), soMesh);

        // We add the field timeseries defined on this mesh, as children of the mesh SObject
        MEDCALC::FieldseriesHandlerList * fieldseriesHandlerList =
          MEDFactoryClient::getDataManager()->getFieldseriesListOnMesh(meshHandler.id);

        for(CORBA::ULong iFieldseries=0; iFieldseries<fieldseriesHandlerList->length(); iFieldseries++) {
          MEDCALC::FieldseriesHandler fieldseriesHandler = (*fieldseriesHandlerList)[iFieldseries];
          SALOMEDS::SObject_var soFieldseries = studyBuilder->NewObject(soMesh);
          _fieldSeriesEntries[fieldseriesHandler.id] = soFieldseries->GetID();

          std::string label(fieldseriesHandler.name);
          label +=" ("+std::string(mapTypeOfFieldLabel[fieldseriesHandler.type])+")";
          soFieldseries->SetAttrString("AttributeName", label.c_str());
          soFieldseries->SetAttrString("AttributePixMap", "ICO_DATASOURCE_FIELD");
          anAttr = studyBuilder->FindOrCreateAttribute(soFieldseries, "AttributeParameter");
          aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
          aParam->SetInt(OBJECT_ID, fieldseriesHandler.id);
          anAttr = studyBuilder->FindOrCreateAttribute(soFieldseries, "AttributeParameter");
          aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
          aParam->SetBool(OBJECT_IS_IN_WORKSPACE, false);

          useCaseBuilder->AppendTo(soFieldseries->GetFather(), soFieldseries);
          soFieldseries->UnRegister();
        }
        soMesh->UnRegister();
      }

      // cleanup
      father->UnRegister();
      soDatasource->UnRegister();
    }
  }

  result = MED_ORB::OP_OK;
  return result;
}

MED_ORB::status
MED::registerPresentation(SALOMEDS::Study_ptr study,
                          CORBA::Long fieldId,
                          const char* name,
                          const char* label)
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  // set result status to error initially
  MED_ORB::status result = MED_ORB::OP_ERROR;

  if (_fieldSeriesEntries.find(fieldId) == _fieldSeriesEntries.end()) {
    std::cerr << "Field not found\n";
    return MED_ORB::OP_ERROR ;
  }
  std::string entry = _fieldSeriesEntries[fieldId];
  SALOMEDS::SObject_var sobject = study->FindObjectID(entry.c_str());
  SALOMEDS::SObject_ptr soFieldseries = sobject._retn();

  if (soFieldseries->IsNull()) {
    std::cerr << "Entry not found\n";
    return  MED_ORB::OP_ERROR;
  }

  SALOMEDS::StudyBuilder_var studyBuilder = study->NewBuilder();
  SALOMEDS::SObject_var soPresentation = studyBuilder->NewObject(soFieldseries);

  soPresentation->SetAttrString("AttributeName", name);
  soPresentation->SetAttrString("AttributePixMap", label);


  result = MED_ORB::OP_OK;
  return result;
}

Engines::TMPFile*
MED::DumpPython(CORBA::Object_ptr theStudy,
                CORBA::Boolean isPublished,
                CORBA::Boolean isMultiFile,
                CORBA::Boolean& isValidScript)
{
  std::cout << "In MED::DumpPython\n";

  SALOMEDS::Study_var aStudy = SALOMEDS::Study::_narrow(theStudy);
  if(CORBA::is_nil(aStudy)) {
    std::cerr << "Error: Cannot find the study\n";
    return new Engines::TMPFile(0);
  }

  SALOMEDS::SObject_var aSO = aStudy->FindComponent("MED");
  if(CORBA::is_nil(aSO)) {
    std::cerr << "Error: Cannot find component MED\n";
    return new Engines::TMPFile(0);
  }

  TCollection_AsciiString aScript;

  MEDCALC::CommandsList* history = MEDFactoryClient::getCommandsHistoryManager()->getCommandsHistory();
  for (CORBA::ULong i = 0; i < history->length(); ++i) {
    aScript += (*history)[i];
    aScript += "\n";
  }

  int aLen = aScript.Length();
  unsigned char* aBuffer = new unsigned char[aLen+1];
  strcpy((char*)aBuffer, aScript.ToCString());

  CORBA::Octet* anOctetBuf =  (CORBA::Octet*)aBuffer;
  Engines::TMPFile_var aStreamFile = new Engines::TMPFile(aLen+1, aLen+1, anOctetBuf, 1);

  isValidScript = true;
  return aStreamFile._retn();
}

extern "C"
{
  /*!
    \brief Exportable factory function: create an instance of the MED component engine
    \param orb reference to the ORB
    \param poa reference to the POA
    \param contId CORBA object ID, pointing to the owner SALOME container
    \param instanceName SALOME component instance name
    \param interfaceName SALOME component interface name
    \return CORBA object identifier of the registered servant
  */
  PortableServer::ObjectId* MEDEngine_factory(CORBA::ORB_ptr orb,
                                              PortableServer::POA_ptr poa,
                                              PortableServer::ObjectId* contId,
                                              const char* instanceName,
                                              const char* interfaceName)
  {
    MED* component = new MED(orb, poa, contId, instanceName, interfaceName);
    return component->getId();
  }
}
