// Copyright (C) 2011-2016  CEA/DEN, EDF R&D
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

#include "MEDPresentationManager_i.hxx"
#include "MEDFactoryClient.hxx"
#include "MEDPresentation.hxx"
#include "MEDPresentationScalarMap.hxx"
#include "MEDPresentationContour.hxx"
#include "MEDPresentationVectorField.hxx"
#include "MEDPresentationSlices.hxx"
#include "MEDPresentationDeflectionShape.hxx"
#include "MEDPresentationPointSprite.hxx"

MEDPresentationManager_i* MEDPresentationManager_i::_instance = NULL;

MEDPresentationManager_i*
MEDPresentationManager_i::getInstance() {
  if ( _instance == NULL )
    _instance = new MEDPresentationManager_i();
  return _instance;
}

MEDPresentationManager_i::MEDPresentationManager_i()
{
}

MEDPresentationManager_i::~MEDPresentationManager_i()
{
  /*
  std::vector<MEDPresentation*>::iterator itr = this->_presentations.begin();
  for ( ; itr != this->_presentations.end(); ++itr) {
    delete *itr;
    *itr = NULL;
  }
  this->_presentations.clear();
  */
}

TypeID MEDPresentationManager_i::GenerateID()
{
  static TypeID START_ID = -1;
  START_ID++;
  return START_ID;
}

#include <iostream>

void
MEDPresentationManager_i::setPresentationProperty(TypeID presentationID, const char * propName, const char * propValue)
{
  if (_presentations.find(presentationID) != _presentations.end())
    {
      MEDPresentation * pres(_presentations[presentationID]);
      pres->setProperty(propName, propValue);
    }
  else
    {
      std::cerr << "setPresentationProperty(): presentation not found!!" << std::endl;
    }
}

char*
MEDPresentationManager_i::getPresentationProperty(TypeID presentationID, const char* propName)
{
  if (_presentations.find(presentationID) != _presentations.end()) {
    MEDPresentation* pres = _presentations[presentationID];
    return (char*) pres->getProperty(propName).c_str();
  }
  else {
    std::cerr << "getPresentationProperty(): presentation not found!!" << std::endl;
    return (char*) "";
  }
}

TypeID
MEDPresentationManager_i::makeScalarMap(const MEDCALC::ScalarMapParameters& params)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());

  TypeID fieldHandlerId = params.fieldHandlerId;
  MEDCALC::MEDPresentationViewMode viewMode = params.viewMode;

  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  std::cout << "\tfieldHandlerId: " << fieldHandlerId << std::endl;
  std::cout << "\tviewMode: " << viewMode << std::endl;
  std::cout << "\tfileName: " <<  dataSHandler->uri << std::endl;
  std::cout << "\tfiedName: " << fieldHandler->fieldname << std::endl;

  // Create a new presentation instance
  TypeID newID = MEDPresentationManager_i::GenerateID();
  MEDPresentationScalarMap * presentation = new MEDPresentationScalarMap(fieldHandler, true);  // on stack or on heap?? stack for now
  _presentations.insert( std::pair<TypeID, MEDPresentation *>(newID, presentation) );

  presentation->generatePipeline();

  return newID;
}

TypeID
MEDPresentationManager_i::makeContour(const MEDCALC::ContourParameters& params)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());

  TypeID fieldHandlerId = params.fieldHandlerId;
  MEDCALC::MEDPresentationViewMode viewMode = params.viewMode;

  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  std::cout << "\tfieldHandlerId: " << fieldHandlerId << std::endl;
  std::cout << "\tviewMode: " << viewMode << std::endl;
  std::cout << "\tfileName: " <<  dataSHandler->uri << std::endl;
  std::cout << "\tfiedName: " << fieldHandler->fieldname << std::endl;

  // Create a new presentation instance
  TypeID newID = MEDPresentationManager_i::GenerateID();
  MEDPresentationContour * presentation = new MEDPresentationContour(fieldHandler, true);  // on stack or on heap?? stack for now
  _presentations.insert( std::pair<TypeID, MEDPresentation *>(newID, presentation) );

  presentation->generatePipeline();

  return newID;
}

TypeID
MEDPresentationManager_i::makeVectorField(const MEDCALC::VectorFieldParameters& params)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());

  TypeID fieldHandlerId = params.fieldHandlerId;
  MEDCALC::MEDPresentationViewMode viewMode = params.viewMode;

  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  std::cout << "\tfieldHandlerId: " << fieldHandlerId << std::endl;
  std::cout << "\tviewMode: " << viewMode << std::endl;
  std::cout << "\tfileName: " <<  dataSHandler->uri << std::endl;
  std::cout << "\tfiedName: " << fieldHandler->fieldname << std::endl;

  // Create a new presentation instance
  TypeID newID = MEDPresentationManager_i::GenerateID();
  MEDPresentationVectorField * presentation = new MEDPresentationVectorField(fieldHandler, true);  // on stack or on heap?? stack for now
  _presentations.insert( std::pair<TypeID, MEDPresentation *>(newID, presentation) );

  presentation->generatePipeline();

  return newID;
}

TypeID
MEDPresentationManager_i::makeSlices(const MEDCALC::SlicesParameters& params)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());

  TypeID fieldHandlerId = params.fieldHandlerId;
  MEDCALC::MEDPresentationViewMode viewMode = params.viewMode;

  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  std::cout << "\tfieldHandlerId: " << fieldHandlerId << std::endl;
  std::cout << "\tviewMode: " << viewMode << std::endl;
  std::cout << "\tfileName: " <<  dataSHandler->uri << std::endl;
  std::cout << "\tfiedName: " << fieldHandler->fieldname << std::endl;

  // Create a new presentation instance
  TypeID newID = MEDPresentationManager_i::GenerateID();
  MEDPresentationSlices * presentation = new MEDPresentationSlices(fieldHandler, true);  // on stack or on heap?? stack for now
  _presentations.insert( std::pair<TypeID, MEDPresentation *>(newID, presentation) );

  presentation->generatePipeline();

  return newID;
}

TypeID
MEDPresentationManager_i::makeDeflectionShape(const MEDCALC::DeflectionShapeParameters& params)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());

  TypeID fieldHandlerId = params.fieldHandlerId;
  MEDCALC::MEDPresentationViewMode viewMode = params.viewMode;

  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  std::cout << "\tfieldHandlerId: " << fieldHandlerId << std::endl;
  std::cout << "\tviewMode: " << viewMode << std::endl;
  std::cout << "\tfileName: " <<  dataSHandler->uri << std::endl;
  std::cout << "\tfiedName: " << fieldHandler->fieldname << std::endl;

  // Create a new presentation instance
  TypeID newID = MEDPresentationManager_i::GenerateID();
  MEDPresentationDeflectionShape * presentation = new MEDPresentationDeflectionShape(fieldHandler, true);  // on stack or on heap?? stack for now
  _presentations.insert( std::pair<TypeID, MEDPresentation *>(newID, presentation) );

  presentation->generatePipeline();

  return newID;
}

TypeID
MEDPresentationManager_i::makePointSprite(const MEDCALC::PointSpriteParameters& params)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());

  TypeID fieldHandlerId = params.fieldHandlerId;
  MEDCALC::MEDPresentationViewMode viewMode = params.viewMode;

  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  std::cout << "\tfieldHandlerId: " << fieldHandlerId << std::endl;
  std::cout << "\tviewMode: " << viewMode << std::endl;
  std::cout << "\tfileName: " <<  dataSHandler->uri << std::endl;
  std::cout << "\tfiedName: " << fieldHandler->fieldname << std::endl;

  // Create a new presentation instance
  TypeID newID = MEDPresentationManager_i::GenerateID();
  MEDPresentationPointSprite * presentation = new MEDPresentationPointSprite(fieldHandler, true);  // on stack or on heap?? stack for now
  _presentations.insert( std::pair<TypeID, MEDPresentation *>(newID, presentation) );

  presentation->generatePipeline();

  return newID;
}
