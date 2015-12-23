// Copyright (C) 2011-2015  CEA/DEN, EDF R&D
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
#include "MEDCouplingRefCountObject.hxx"
#include <iostream>

MEDPresentation::MEDPresentation(MEDCALC::FieldHandler* fieldHdl, std::string name)
    : _fieldHandler(fieldHdl), _pipeline(0), _display(0), _properties()
{
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

std::string MEDPresentation::getFieldTypeString()
{
  ParaMEDMEM::TypeOfField typ = (ParaMEDMEM::TypeOfField)_fieldHandler->type;
  switch(typ)
  {
    case ParaMEDMEM::ON_CELLS:
      return "CELLS";
    case ParaMEDMEM::ON_NODES:
      return "NODES";
    default:
      std::cerr << "MEDPresentation::getFieldTypeString() -- Not implemented ! Gauss points?";
      return "";
  }
}


void MEDPresentationScalarMap::internalGeneratePipeline()
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());

  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(_fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  std::string fileName(dataSHandler->uri);
  std::string fieldName(_fieldHandler->fieldname);
  std::string fieldType = getFieldTypeString();

  std::cout << "Generating pipeline for SCALAR MAP:" <<std::endl;
  std::cout << "\tfileName: " <<  fileName << std::endl;
  std::cout << "\tfiedName: " << fieldName << std::endl;
  if (fileName.substr(0, 7) != std::string("file://"))
    {
      std::cerr << "\tData source is not a file! Can not proceed." << std::endl;
      return;
    }

  fileName = fileName.substr(7, fileName.size());
  std::cout << "\tfileName: " <<  fileName << std::endl;

  PyGILState_STATE _gil_state = PyGILState_Ensure();

  PyRun_SimpleString("print 'hello world'");
  std::string cmd = std::string(
        "import pvsimple as pvs;"
        "__obj1 = pvs.MEDReader(FileName='") + fileName + std::string("');"
        "__disp1 = pvs.Show(__obj1);"
        "pvs.ColorBy(__disp1, ('") + fieldType + std::string("', '") + fieldName + std::string("'));"
        "pvs.GetActiveViewOrCreate('RenderView').ResetCamera()");

  std::cerr << "Python command:" << std::endl;
  std::cerr << cmd << std::endl;
  PyRun_SimpleString(cmd.c_str());
  // Retrieve Python object for internal storage:
  PyObject * obj = getPythonObjectFromMain("__obj1");
  PyObject * disp = getPythonObjectFromMain("__disp1");
  pushInternal(obj, disp);

  PyGILState_Release(_gil_state);
}