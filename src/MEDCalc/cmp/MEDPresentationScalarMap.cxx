#include "MEDPresentationScalarMap.hxx"

void
MEDPresentationScalarMap::internalGeneratePipeline()
{
  //MEDPresentation::TypeID fieldHandlerId = params.fieldHandlerId;
  //MEDCALC::MEDPresentationViewMode viewMode = params.viewMode;

  // :TODO: consider viewMode

  /*
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());
  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);

  std::cout << "\tfieldHandlerId: " << fieldHandlerId << std::endl;
  std::cout << "\tviewMode: " << viewMode << std::endl;

  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);
  std::cout << "\tfileName: " <<  dataSHandler->uri << std::endl;
  std::cout << "\tfiedName: " << fieldHandler->fieldname << std::endl;
  */

  PyGILState_STATE _gil_state = PyGILState_Ensure();

  std::string cmd = std::string("import pvsimple as pvs;");
  cmd += std::string("__obj1 = pvs.MEDReader(FileName='") + _fileName + std::string("');");
  cmd += std::string("__disp1 = pvs.Show(__obj1);");
  cmd += std::string("pvs.ColorBy(__disp1, ('") + _fieldType + std::string("', '") + _fieldName + std::string("'));");
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
