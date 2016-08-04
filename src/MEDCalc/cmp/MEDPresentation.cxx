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

#include "MEDPyLockWrapper.hxx"
#include "MEDFactoryClient.hxx"
#include "MEDPresentation.hxx"
#include "MEDPresentationException.hxx"
#include "MEDCouplingRefCountObject.hxx"
#include <SALOME_KernelServices.hxx>
#undef LOG
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentation::PROP_NAME  = "name";
const std::string MEDPresentation::PROP_NB_COMPONENTS = "nbComponents";
const std::string MEDPresentation::PROP_SELECTED_COMPONENT = "selectedComponent";
const std::string MEDPresentation::PROP_COMPONENT = "component_";
const std::string MEDPresentation::PROP_COLOR_MAP = "colorMap";
const std::string MEDPresentation::PROP_SCALAR_BAR_RANGE = "scalarBarRange";

MEDPresentation::MEDPresentation(MEDPresentation::TypeID fieldHandlerId, const std::string& name,
                                 const MEDCALC::ViewModeType viewMode,
                                 const MEDCALC::ColorMapType colorMap,
                                 const MEDCALC::ScalarBarRangeType sbRange)
    : _fieldHandlerId(fieldHandlerId), _propertiesStr(),
      //_pipeline(0), _display(0)
      _selectedComponentIndex(-1),
      _viewMode(viewMode),
      _colorMap(colorMap),
      _sbRange(sbRange),
      _renderViewPyId(-1),  // will be set by getRenderViewCommand()
      _globalDict(0)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());
  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  _fileName = dataSHandler->uri;
  _fieldName = fieldHandler->fieldname;
  _fieldType = getFieldTypeString((MEDCoupling::TypeOfField) fieldHandler->type);
  _meshName = meshHandler->name;

  if (_fileName.substr(0, 7) != std::string("file://")) {
    const char* msg = "MEDPresentation(): Data source is not a file! Can not proceed.";
    STDLOG(msg);
    throw MEDPresentationException(msg);
  }
  _fileName = _fileName.substr(7, _fileName.size());

  setStringProperty(MEDPresentation::PROP_NAME, name);

  setIntProperty(MEDPresentation::PROP_NB_COMPONENTS, 0);
  setIntProperty(MEDPresentation::PROP_SELECTED_COMPONENT, 0);

  setIntProperty(MEDPresentation::PROP_COLOR_MAP, static_cast<int>(colorMap));
  setIntProperty(MEDPresentation::PROP_SCALAR_BAR_RANGE, static_cast<int>(sbRange));

  // Python variables:
  int id = GeneratePythonId();
  std::ostringstream oss_o, oss_d, oss_l, oss_s;
  oss_o << "__obj" << id;
  oss_s << "__srcObj" << id;
  oss_d << "__disp" << id;
  oss_l << "__lut" << id;
  _objVar = oss_o.str();
  _srcObjVar = oss_s.str();
  _dispVar = oss_d.str();
  _lutVar = oss_l.str();
}

MEDPresentation::~MEDPresentation()
{
  STDLOG("~MEDPresentation(): clear display");
  {
    MEDPyLockWrapper lock;
    std::ostringstream oss_v, oss;
    oss_v << "__view" << _renderViewPyId;
    oss << "pvs.Hide(" << _objVar <<  ", view=" << oss_v.str() << ");";
    oss << "pvs.Render();";

    PyRun_SimpleString(oss.str().c_str());
  }
}

void
MEDPresentation::generatePipeline()
{
  // Might be more complicated in the future:

  this->internalGeneratePipeline();
}

//void
//MEDPresentation::pushPyObjects(PyObjectId obj, PyObjectId disp)
//{
//  _pipeline.push_back(obj);
//  _display.push_back(disp);
//}

void
MEDPresentation::pushAndExecPyLine(const std::string & lin)
{
  execPyLine(lin);
  _pythonCmds.push_back(lin);
}

void
MEDPresentation::execPyLine(const std::string & lin)
{
  MEDPyLockWrapper lock;
  STDLOG("@@@@ MEDPresentation::execPyLine() about to exec >> " << lin);
  if(PyRun_SimpleString(lin.c_str()))
    {
      std::ostringstream oss;
      oss << "MEDPresentation::execPyLine(): following Python command failed!\n";
      oss << ">> " << lin;
      STDLOG(oss.str());
      throw KERNEL::createSalomeException(oss.str().c_str());
    }
}

void
MEDPresentation::setStringProperty(const std::string& propName, const std::string& propValue)
{
  _propertiesStr[propName] = propValue;
}

const std::string
MEDPresentation::getStringProperty(const std::string& propName) const
{
  std::map<std::string, std::string>::const_iterator it = _propertiesStr.find(propName);
  if (it != _propertiesStr.end()) {
      return (*it).second;
  }
  else {
      STDLOG("MEDPresentation::getStringProperty(): no property named " + propName);
      throw MEDPresentationException("MEDPresentation::getStringProperty(): no property named " + propName);
  }
}

void
MEDPresentation::setIntProperty(const std::string& propName, const int propValue)
{
  _propertiesInt[propName] = propValue;
}

int
MEDPresentation::getIntProperty(const std::string& propName) const
{
  std::map<std::string, int>::const_iterator it = _propertiesInt.find(propName);
  if (it != _propertiesInt.end()) {
      return (*it).second;
  }
  else {
      STDLOG("MEDPresentation::getIntProperty(): no property named " + propName);
      throw MEDPresentationException("MEDPresentation::getIntProperty(): no property named " + propName);
  }
}

 void
 MEDPresentation::dumpIntProperties() const
 {
   std::map<std::string, int>::const_iterator it = _propertiesInt.begin();
   STDLOG("@@@ Dumping INT properties");
   for(; it != _propertiesInt.end(); ++it)
     {
       std::ostringstream oss;
       oss << (*it).first << "  ->   " << (*it).second;
       STDLOG(oss.str());
     }
 }

 void
 MEDPresentation::dumpStringProperties() const
 {
   std::map<std::string, std::string>::const_iterator it = _propertiesStr.begin();
   STDLOG("@@@ Dumping STR properties");
   for(; it != _propertiesStr.end(); ++it)
     {
       std::ostringstream oss;
       oss << (*it).first << "  ->   " << (*it).second;
       STDLOG(oss.str());
     }
 }

 void
 MEDPresentation::internalGeneratePipeline()
 {
   MEDPyLockWrapper lock;
   pushAndExecPyLine( "import pvsimple as pvs;");
 }


/**
* @return a borrowed reference. Do not DECRREF!
*/
PyObject*
MEDPresentation::getPythonObjectFromMain(const char* python_var) const
{
  if (! _globalDict)
    {
      // All the calls below returns *borrowed* references
      PyObject* main_module = PyImport_AddModule((char*)"__main__");
      _globalDict = PyModule_GetDict(main_module);
    }
  return PyDict_GetItemString(_globalDict, python_var);
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
MEDPresentation::getRenderViewVar() const
{
  std::ostringstream oss;
  oss << "__view" << _renderViewPyId;
  return oss.str();
}

void
MEDPresentation::createSource()
{
  std::ostringstream oss;
  oss << _srcObjVar << " = pvs.MEDReader(FileName='" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _srcObjVar << ".GenerateVectors = 1;";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentation::setOrCreateRenderView()
{
  std::ostringstream oss2;

  std::string view(getRenderViewVar());
  oss2 << "pvs._DisableFirstRenderCameraReset();";
  pushAndExecPyLine(oss2.str()); oss2.str("");
  if (_viewMode == MEDCALC::VIEW_MODE_OVERLAP) {
      // this might potentially re-assign to an existing view variable, but this is OK, we
      // normally reassign exaclty the same RenderView object.
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  } else if (_viewMode == MEDCALC::VIEW_MODE_REPLACE) {
      // same as above
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << "pvs.active_objects.source and pvs.Hide(view=" << view << ");";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << "pvs.Render();";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  } else if (_viewMode == MEDCALC::VIEW_MODE_NEW_LAYOUT) {
      oss2 <<  "__layout1 = pvs.servermanager.misc.ViewLayout(registrationGroup='layouts');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << view << " = pvs.CreateView('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  } else if (_viewMode == MEDCALC::VIEW_MODE_SPLIT_VIEW) {
      oss2 << view << " = pvs.CreateView('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  }
}

void
MEDPresentation::resetCameraAndRender()
{
  pushAndExecPyLine(getRenderViewVar() + ".ResetCamera();");
  pushAndExecPyLine("pvs.Render();");
}

void
MEDPresentation::selectFieldComponent()
{
  std::ostringstream oss, oss_l;
  std::string ret;

  if (_selectedComponentIndex != -1)
    {
      oss << _lutVar << ".VectorMode = 'Component';";
      pushAndExecPyLine(oss.str()); oss.str("");
      oss << _lutVar << ".VectorComponent = " << _selectedComponentIndex << ";";
      pushAndExecPyLine(oss.str()); oss.str("");
    }
  else  // Euclidean norm
    {
      oss << _lutVar << ".VectorMode = 'Magnitude';";
      pushAndExecPyLine(oss.str()); oss.str("");
    }
}

/**
 * Needs the LUT, so to be called after selectColorMap for the first time.
 */
void
MEDPresentation::fixScalarBarTitle()
{
  // get selected component name:
  std::string compoName("Magnitude");
  if (_selectedComponentIndex != -1)
    {
      std::ostringstream oss1;
      oss1 << MEDPresentation::PROP_COMPONENT << _selectedComponentIndex;
      compoName = getStringProperty(oss1.str());
    }
  std::ostringstream oss;
  oss << "pvs.GetScalarBar(" << _lutVar << ").ComponentTitle = '" << compoName << "';";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentation::selectColorMap()
{
  std::ostringstream oss, oss2;

  oss2 << _lutVar << " = pvs.GetColorTransferFunction('" << _fieldName << "');";
  pushAndExecPyLine(oss2.str());

  switch (_colorMap) {
  case MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW:
    oss << _lutVar << ".ApplyPreset('Blue to Red Rainbow',True);";
    break;
  case MEDCALC::COLOR_MAP_COOL_TO_WARM:
    oss << _lutVar << ".ApplyPreset('Cool to Warm',True);";
    break;
  default:
    STDLOG("MEDPresentation::getColorMapCommand(): invalid colormap!");
    throw KERNEL::createSalomeException("MEDPresentation::getColorMapCommand(): invalid colormap!");
  }
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::showObject()
{
  std::ostringstream oss;
  oss << _dispVar << " = pvs.Show(" << _objVar << ", " << getRenderViewVar() << ");";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::showScalarBar()
{
  std::ostringstream oss;
  oss << _dispVar <<  ".SetScalarBarVisibility(" << getRenderViewVar() << ", True);";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::colorBy(const std::string & fieldType)
{
  std::ostringstream oss;
  oss << "pvs.ColorBy(" << _dispVar << ", ('" << fieldType << "', '" << _fieldName << "'));";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::rescaleTransferFunction()
{
  std::ostringstream oss;
  switch(_sbRange)
  {
    case MEDCALC::SCALAR_BAR_ALL_TIMESTEPS:
      oss << _dispVar << ".RescaleTransferFunctionToDataRangeOverTime();";
      break;
    case MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP:
      oss << _dispVar << ".RescaleTransferFunctionToDataRange(False);";
      break;
    default:
      STDLOG("MEDPresentation::getRescaleCommand(): invalid range!");
      throw KERNEL::createSalomeException("MEDPresentation::getRescaleCommand(): invalid range!");
  }
  pushAndExecPyLine(oss.str()); oss.str("");
}

int
MEDPresentation::GeneratePythonId()
{
  static int INIT_ID = 0;
  return INIT_ID++;
}

void
MEDPresentation::activateView()
{
  MEDPyLockWrapper lock;
  pushAndExecPyLine("pvs.SetActiveView(" + getRenderViewVar() + ");");
}


std::string
MEDPresentation::paravisDump() const
{
  using namespace std;
  ostringstream oss;
  for (vector<string>::const_iterator it=_pythonCmds.begin(); it != _pythonCmds.end(); ++it)
    {
      oss << (*it);
      oss << "\n";
    }
  return oss.str();
}

/**
 * Query all available component names for the field associated with this presentation.
 * Fills in all the corresponding string properties:
 *  - PROP_COMPONENT1
 *  - PROP_COMPONENT2
 *    etc...
 *  and the number of components.
 */
void
MEDPresentation::fillAvailableFieldComponents()
{
  MEDPyLockWrapper lock;  // GIL!
  std::string typ;

  if(_fieldType == "CELLS") {
      typ = "CellData";
  }
  else if (_fieldType == "POINTS") {
      typ = "PointData";
  }
  else {
      std::string msg("Unsupported spatial discretisation: " + _fieldType);
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg.c_str());
  }

  std::ostringstream oss;
  oss << "__nbCompo = " << _srcObjVar << "." << typ << ".GetArray('" <<  _fieldName << "').GetNumberOfComponents();";
  execPyLine(oss.str());
  PyObject* p_obj = getPythonObjectFromMain("__nbCompo");
  long nbCompo;
  if (p_obj && PyInt_Check(p_obj))
    nbCompo = PyInt_AS_LONG(p_obj);
  else
    {
      STDLOG("Unexpected Python error");
      throw KERNEL::createSalomeException("Unexpected Python error");
    }
  setIntProperty(MEDPresentation::PROP_NB_COMPONENTS, nbCompo);
  for (long i = 0; i<nbCompo; i++)
    {
      std::ostringstream oss2;
      oss2 << "__compo = " << _srcObjVar << "." << typ << ".GetArray('" <<  _fieldName << "').GetComponentName(" << i << ");";
      execPyLine(oss2.str());
      PyObject* p_obj = getPythonObjectFromMain("__compo");
      std::string compo;
      if (p_obj && PyString_Check(p_obj))
        compo = std::string(PyString_AsString(p_obj));  // pointing to internal Python memory, so make a copy!!
      else
        {
          STDLOG("Unexpected Python error");
          throw KERNEL::createSalomeException("Unexpected Python error");
        }
      std::ostringstream oss_p;
      oss_p << MEDPresentation::PROP_COMPONENT << i;
      setStringProperty(oss_p.str(), compo);
    }
}

/**
 * In case where a CELLS field needs to be converted to POINT field.
 * This updates the source object to become the result of the CellDatatoPointData filter.
 */
void
MEDPresentation::applyCellToPointIfNeeded()
{
  std::ostringstream oss, oss2;
  // Apply Cell data to point data:
  oss2 << "__srcObj" << GeneratePythonId();
  oss << oss2.str() << " = pvs.CellDatatoPointData(Input=" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  // Now the source becomes the result of the CellDatatoPointData:
  _srcObjVar = oss2.str();
}

/**
 * Convert a vector field into a 3D vector field:
 *  - if the vector field is already 3D, nothing to do
 *  - if it is 2D, then add a null component
 *  - otherwise (tensor field, scalar field) throw
 */
void
MEDPresentation::convertTo3DVectorField()
{
  std::ostringstream oss, oss1, oss2, oss3;

  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);
  if (nbCompo < 2 || nbCompo > 3)
    {
      oss << "The field '" << _fieldName << "' must have 2 or 3 components for this presentation!";
      STDLOG(oss.str());
      throw KERNEL::createSalomeException(oss.str().c_str());
    }
  if (nbCompo == 3)
    return;

  // Apply calculator:
  oss2 << "__srcObj" << GeneratePythonId();
  oss << oss2.str() << " = pvs.Calculator(Input=" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  // Now the source becomes the result of the CellDatatoPointData:
  _srcObjVar = oss2.str();
  std::string typ;
  if(_fieldType == "CELLS")
    typ = "Cell Data";
  else if(_fieldType == "POINTS")
    typ = "Point Data";
  else
    {
      oss3 << "Field '" << _fieldName << "' has invalid field type";
      STDLOG(oss3.str());
      throw KERNEL::createSalomeException(oss3.str().c_str());
    }
  oss << _srcObjVar << ".AttributeMode = '" <<  typ << "';";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _srcObjVar << ".ResultArrayName = '" <<  _fieldName << "_CALC';";  // will never be needed I think
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _srcObjVar << ".Function = '" <<  _fieldName << "_0*iHat + " << _fieldName << "_1*jHat + 0.0*zHat';";
  pushAndExecPyLine(oss.str()); oss.str("");
}

//double
//MEDPresentation::computeCellAverageSize()
//{
//  std::ostringstream oss;
//  oss << "import MEDLoader;";
//  pushAndExecPyLine(oss.str()); oss.str("");
//  oss << "__mesh = MEDLoader.ReadMeshFromFile('" << _fileName << "', '" << _meshName << "');";
//  pushAndExecPyLine(oss.str()); oss.str("");
//
//  oss << "__bb = __mesh.getBoundingBox()";
//  pushAndExecPyLine(oss.str()); oss.str("");
//  oss << "__deltas = [x[1]-x[0] for x in __bb];";
//  pushAndExecPyLine(oss.str()); oss.str("");
//  oss << "__vol = reduce(lambda x,y:x*y, __deltas, 1.0);";
//  pushAndExecPyLine(oss.str()); oss.str("");
//  // Average cell size is the the n-th root of average volume of a cell, with n being the space dimension
//  oss << "__cellSize = (__vol/__mesh.getNumberOfCells())**(1.0/len(__bb));";
//  pushAndExecPyLine(oss.str()); oss.str("");
//
//  PyObject * pyObj = getPythonObjectFromMain("__cellSize");
//  bool err = false;
//  if (!pyObj || !PyFloat_Check(pyObj)) {  /* nothing to do, err handler below */}
//  else {
//      double ret= PyFloat_AsDouble(pyObj);
//      if(!PyErr_Occurred())
//        return ret;
//    }
//  // From here, an error for sure.
//  const char * msg = "MEDPresentation::computeCellAverageSize(): Python error.";
//  STDLOG(msg);
//  throw KERNEL::createSalomeException(msg);
//}
