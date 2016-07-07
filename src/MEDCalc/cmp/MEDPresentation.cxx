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
#include "Basics_Utils.hxx"
#include "PyInterp_Utils.h"

#include <sstream>

MEDPresentation::MEDPresentation(MEDPresentation::TypeID fieldHandlerId, const std::string& name)
    : _fieldHandlerId(fieldHandlerId), _pipeline(0), _display(0), _properties(),
      _renderViewPyId(GeneratePythonId())
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

MEDPresentation::~MEDPresentation()
{
  STDLOG("~MEDPresentation(): clear display");
  {
    PyLockWrapper lock;
    std::ostringstream oss_o, oss_v, oss;
    // Get top level object and display:
    oss_o << "__obj" << _pipeline.front().first;
    oss_v << "__view" << _renderViewPyId;
    oss << "pvs.Hide(" << oss_o.str() <<  ", view=" << oss_v.str() << ");";
    oss << "pvs.Render();";

//    std::cerr << oss.str() << std::endl;
    PyRun_SimpleString(oss.str().c_str());
  }
}

void
MEDPresentation::generatePipeline()
{
  // Might be more complicated in the future:

  this->internalGeneratePipeline();
}

void
MEDPresentation::pushPyObjects(PyObjectId obj, PyObjectId disp)
{
  _pipeline.push_back(obj);
  _display.push_back(disp);
}

void
MEDPresentation::pushAndExecPyLine(const std::string & lin)
{
  // TODO: store internally for low level dump
  PyLockWrapper lock;
//  std::cerr << lin << std::endl;
  PyRun_SimpleString(lin.c_str());
}

void
MEDPresentation::setProperty(const std::string& propName, const std::string& propValue)
{
  // LIMITED!!! For now switch the first display element to Wireframe
  /*
  PyLockWrapper lock;
  PyObject_CallMethod(_display[0], (char*)"SetRepresentationType", (char*)"(s)", "Wireframe");
  */

  _properties[propName] = propValue;
}

const std::string
MEDPresentation::getProperty(const std::string& propName) const
{
  std::map<std::string, std::string>::const_iterator it = _properties.find(propName);
  if (it != _properties.end()) {
    return (*it).second;
  }
  else {
    STDLOG("MEDPresentation::getProperty(): no property named " + propName);
    return std::string();
  }
}

PyObject*
MEDPresentation::getPythonObjectFromMain(const char* python_var) const
{
  // TODO: improve to avoid getting dict at each call

  // All the calls below returns *borrowed* references
  PyObject* main_module = PyImport_AddModule((char*)"__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  return PyDict_GetItemString(global_dict, python_var);
}

std::string
MEDPresentation::getFieldTypeString(MEDCoupling::TypeOfField fieldType) const
{
  switch(fieldType)
  {
    case MEDCoupling::ON_CELLS:
      return "CELLS";
    case MEDCoupling::ON_NODES:
      return "POINTS";
    default:
      STDLOG("MEDPresentation::getFieldTypeString() -- Not implemented ! Gauss points?");
      return "";
  }
}

std::string
MEDPresentation::getRenderViewCommand(MEDCALC::MEDPresentationViewMode viewMode) const
{
  std::ostringstream oss, oss2;
  oss << "__view" << _renderViewPyId;
  std::string view(oss.str());
  oss2 << "pvs._DisableFirstRenderCameraReset();";
  if (viewMode == MEDCALC::VIEW_MODE_OVERLAP) {
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');";
  } else if (viewMode == MEDCALC::VIEW_MODE_REPLACE) {
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');";
      oss2 << "pvs.active_objects.source and pvs.Hide(view=" << view << ");";
      oss2 << "pvs.Render();";
  } else if (viewMode == MEDCALC::VIEW_MODE_NEW_LAYOUT) {
      oss2 <<  "__layout1 = pvs.servermanager.misc.ViewLayout(registrationGroup='layouts');";
      oss2 << view << " = pvs.CreateView('RenderView');";
  } else if (viewMode == MEDCALC::VIEW_MODE_SPLIT_VIEW) {
      oss2 << view << " = pvs.CreateView('RenderView');";
  }
  return oss2.str();
}

std::string
MEDPresentation::getResetCameraCommand() const
{
  std::ostringstream oss;
  oss << "__view" << _renderViewPyId << ".ResetCamera();";
  return oss.str();
}

std::string
MEDPresentation::getColorMapCommand(MEDCALC::MEDPresentationColorMap colorMap) const
{
  switch (colorMap) {
  case MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW: return "Blue to Red Rainbow";
  case MEDCALC::COLOR_MAP_COOL_TO_WARM: return "Cool to Warm";
  }
}

int
MEDPresentation::GeneratePythonId()
{
  static int INIT_ID = 0;
  return INIT_ID++;
}

void
MEDPresentation::activateView() const
{
  PyLockWrapper lock;
  std::ostringstream oss;
  oss << "pvs.SetActiveView(__view" << _renderViewPyId << ");";
  std::cerr << oss.str() << std::endl;
  PyRun_SimpleString(oss.str().c_str());
}
