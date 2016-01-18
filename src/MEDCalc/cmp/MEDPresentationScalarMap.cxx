#include "MEDPresentationScalarMap.hxx"
#include "MEDFactoryClient.hxx"

#include <iostream>

void
MEDPresentationScalarMap::internalGeneratePipeline()
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

  std::string cmd = std::string("import pvsimple as pvs;");
  cmd += std::string("__obj1 = pvs.MEDReader(FileName='") + fileName + std::string("');");
  cmd += std::string("__disp1 = pvs.Show(__obj1);");
  cmd += std::string("pvs.ColorBy(__disp1, ('") + fieldType + std::string("', '") + fieldName + std::string("'));");
  cmd += std::string("pvs.GetActiveViewOrCreate('RenderView').ResetCamera();");
  cmd += std::string("__disp1.RescaleTransferFunctionToDataRangeOverTime();");

  //std::cerr << "Python command:" << std::endl;
  //std::cerr << cmd << std::endl;
  PyRun_SimpleString(cmd.c_str());
  // Retrieve Python object for internal storage:
  PyObject * obj = getPythonObjectFromMain("__obj1");
  PyObject * disp = getPythonObjectFromMain("__disp1");
  pushInternal(obj, disp);

  PyGILState_Release(_gil_state);
}
