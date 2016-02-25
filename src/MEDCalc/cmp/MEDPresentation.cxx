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
// Authors: A Bruneton (CEA), C Aguerre (EdF)

#include "MEDFactoryClient.hxx"
#include "MEDPresentation.hxx"
#include "MEDPresentationException.hxx"
#include "MEDCouplingRefCountObject.hxx"
#include <iostream>

MEDPresentation::MEDPresentation(MEDPresentation::TypeID fieldHandlerId, std::string name)
    : _fieldHandlerId(fieldHandlerId), _pipeline(0), _display(0), _properties()
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());
  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  _fileName = dataSHandler->uri;
  _fieldName = fieldHandler->fieldname;
  _fieldType = getFieldTypeString((MEDCoupling::TypeOfField) fieldHandler->type);

  if (_fileName.substr(0, 7) != std::string("file://")) {
    const char* msg = "Data source is not a file! Can not proceed.";
    throw MEDPresentationException(msg);
  }
  _fileName = _fileName.substr(7, _fileName.size());

  setProperty("name", name);
}

void MEDPresentation::generatePipeline()
{
  // Might be more complicated in the future:

  this->internalGeneratePipeline();
}

void MEDPresentation::pushInternal(PyObject * obj, PyObject * disp)
{
  _pipeline.push_back(obj);
  _display.push_back(disp);
}

void MEDPresentation::setProperty(const std::string& propName, const std::string& propValue)
{
  // LIMITED!!! For now switch the first display element to Wireframe
  /*
  PyLockWrapper lock;
  PyObject_CallMethod(_display[0], (char*)"SetRepresentationType", (char*)"(s)", "Wireframe");
  */

  _properties[propName] = propValue;
}

const std::string
MEDPresentation::getProperty(const std::string& propName)
{
  if (_properties.find(propName) != _properties.end()) {
    return _properties[propName];
  }
  else {
    std::cerr << "getProperty(): no property named " << propName << std::endl;
    return std::string();
  }
}

PyObject * MEDPresentation::getPythonObjectFromMain(const char * python_var)
{
  // TODO: improve to avoid getting dict at each call

  // All the calls below returns *borrowed* references
  PyObject* main_module = PyImport_AddModule((char*)"__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  return PyDict_GetItemString(global_dict, python_var);
}

std::string MEDPresentation::getFieldTypeString(MEDCoupling::TypeOfField fieldType)
{
  switch(fieldType)
  {
    case MEDCoupling::ON_CELLS:
      return "CELLS";
    case MEDCoupling::ON_NODES:
      return "POINTS";
    default:
      std::cerr << "MEDPresentation::getFieldTypeString() -- Not implemented ! Gauss points?";
      return "";
  }
}

std::string
MEDPresentation::getRenderViewCommand(MEDCALC::MEDPresentationViewMode viewMode)
{
  std::string cmd = std::string("pvs._DisableFirstRenderCameraReset();");
  if (viewMode == MEDCALC::VIEW_MODE_OVERLAP) {
    cmd += std::string("__view1 = pvs.GetActiveViewOrCreate('RenderView');");
  } else if (viewMode == MEDCALC::VIEW_MODE_REPLACE) {
    cmd += std::string("__view1 = pvs.GetActiveViewOrCreate('RenderView');");
    cmd += std::string("pvs.active_objects.source and pvs.Hide(view=__view1);");
    cmd += std::string("pvs.Render();");
  } else if (viewMode == MEDCALC::VIEW_MODE_NEW_LAYOUT) {
    cmd += std::string("__layout1 = pvs.servermanager.misc.ViewLayout(registrationGroup='layouts');");
    cmd += std::string("__view1 = pvs.CreateView('RenderView');");
  } else if (viewMode == MEDCALC::VIEW_MODE_SPLIT_VIEW) {
    cmd += std::string("__view1 = pvs.CreateView('RenderView');");
  }
  cmd += std::string("__view1.ResetCamera();");
  return cmd;
}

std::string
MEDPresentation::getColorMapCommand(MEDCALC::MEDPresentationColorMap colorMap)
{
  switch(colorMap) {
  case MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW: return "Blue to Red Rainbow";
  case MEDCALC::COLOR_MAP_COOL_TO_WARM: return "Cool to Warm";
  }
}
