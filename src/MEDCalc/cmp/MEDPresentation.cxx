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
                                 const MEDCALC::MEDPresentationViewMode viewMode,
                                 const MEDCALC::MEDPresentationColorMap colorMap,
                                 const MEDCALC::MEDPresentationScalarBarRange sbRange)
    : _fieldHandlerId(fieldHandlerId), _propertiesStr(),
      //_pipeline(0), _display(0)
      _selectedComponentIndex(-1),
      _viewMode(viewMode),
      _colorMap(colorMap),
      _sbRange(sbRange),
      _renderViewPyId(-1),  // will be set by getRenderViewCommand()
      _objId(GeneratePythonId()),
      _dispId(_objId), _lutId(_objId),
      _globalDict(0)
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());
  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(fieldHandlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMesh(fieldHandler->meshid);
  MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid);

  _fileName = dataSHandler->uri;
  _fieldName = fieldHandler->fieldname;
  _fieldType = getFieldTypeString((MEDCoupling::TypeOfField) fieldHandler->type);

  if (_fileName.substr(0, 7) != std::string("file://")) {
    const char* msg = "MEDPresentation(): Data source is not a file! Can not proceed.";
    STDLOG(msg);
    throw MEDPresentationException(msg);
  }
  _fileName = _fileName.substr(7, _fileName.size());

  setStringProperty(MEDPresentation::PROP_NAME, name);

  setIntProperty(MEDPresentation::PROP_NB_COMPONENTS, 0);
  setIntProperty(MEDPresentation::PROP_SELECTED_COMPONENT, 0);

  setIntProperty(MEDPresentation::PROP_COLOR_MAP, static_cast<int>(MEDCALC::COLOR_MAP_DEFAULT));
  setIntProperty(MEDPresentation::PROP_SCALAR_BAR_RANGE, static_cast<int>(MEDCALC::SCALAR_BAR_RANGE_DEFAULT));
}

MEDPresentation::~MEDPresentation()
{
  STDLOG("~MEDPresentation(): clear display");
  {
    MEDPyLockWrapper lock;
    std::ostringstream oss_o, oss_v, oss;
    oss_o << "__obj" << _objId;
    oss_v << "__view" << _renderViewPyId;
    oss << "pvs.Hide(" << oss_o.str() <<  ", view=" << oss_v.str() << ");";
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
//  STDLOG("@@@@ MEDPresentation::execPyLine() about to exec >> " << lin);
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
MEDPresentation::getRenderViewCommand() const
{
  std::ostringstream oss, oss2;

  oss << "__view" << _renderViewPyId;
  std::string view(oss.str());
  oss2 << "pvs._DisableFirstRenderCameraReset();" << std::endl;
  if (_viewMode == MEDCALC::VIEW_MODE_OVERLAP) {
      // this might potentially re-assign to an existing view variable, but this is OK, we
      // normally reassign exaclty the same RenderView object.
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');" << std::endl;
  } else if (_viewMode == MEDCALC::VIEW_MODE_REPLACE) {
      // same as above
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');" << std::endl;
      oss2 << "pvs.active_objects.source and pvs.Hide(view=" << view << ");" << std::endl;
      oss2 << "pvs.Render();" << std::endl;
  } else if (_viewMode == MEDCALC::VIEW_MODE_NEW_LAYOUT) {
      oss2 <<  "__layout1 = pvs.servermanager.misc.ViewLayout(registrationGroup='layouts');" << std::endl;
      oss2 << view << " = pvs.CreateView('RenderView');" << std::endl;
  } else if (_viewMode == MEDCALC::VIEW_MODE_SPLIT_VIEW) {
      oss2 << view << " = pvs.CreateView('RenderView');" << std::endl;
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
MEDPresentation::getComponentSelectionCommand() const
{
  std::ostringstream oss, oss_l;
  oss_l << "__lut" << _lutId;    std::string lut(oss_l.str());

  if (_selectedComponentIndex != -1)
    {
      oss << lut << ".VectorMode = 'Component';\n";
      oss << lut << ".VectorComponent = " << _selectedComponentIndex << ";";
    }
  else  // Euclidean norm
    {
      oss << lut << ".VectorMode = 'Magnitude';";
    }
  return oss.str();
}

std::string
MEDPresentation::getColorMapCommand() const
{
  std::ostringstream oss, oss_l;
  oss_l << "__lut" << _lutId;    std::string lut(oss_l.str());
  switch (_colorMap) {
  case MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW:
    oss << lut << ".ApplyPreset('Blue to Red Rainbow',True);";
    break;
  case MEDCALC::COLOR_MAP_COOL_TO_WARM:
    oss << lut << ".ApplyPreset('Cool to Warm',True);";
    break;
  default:
    STDLOG("MEDPresentation::getColorMapCommand(): invalid colormap!");
    throw KERNEL::createSalomeException("MEDPresentation::getColorMapCommand(): invalid colormap!");
  }
  return oss.str();
}
std::string
MEDPresentation::getRescaleCommand() const
{
  std::ostringstream oss, oss_d;
  oss_d << "__disp" << _dispId;    std::string disp(oss_d.str());
  switch(_sbRange)
  {
    case MEDCALC::SCALAR_BAR_ALL_TIMESTEPS:
      oss << disp <<  ".RescaleTransferFunctionToDataRangeOverTime();";
      break;
    case MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP:
      oss << disp <<  ".RescaleTransferFunctionToDataRange(False);";
      break;
    default:
      STDLOG("MEDPresentation::getRescaleCommand(): invalid range!");
      throw KERNEL::createSalomeException("MEDPresentation::getRescaleCommand(): invalid range!");
  }
  return oss.str();
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

  std::ostringstream oss;
  oss << "pvs.SetActiveView(__view" << _renderViewPyId << ");";
  pushAndExecPyLine(oss.str());
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

  std::ostringstream oss_o;
  oss_o << "__obj" << _objId;      std::string obj(oss_o.str());
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
  oss << "__nbCompo = " << obj << "." << typ << ".GetArray('" <<  _fieldName << "').GetNumberOfComponents();";
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
      oss2 << "__compo = " << obj << "." << typ << ".GetArray('" <<  _fieldName << "').GetComponentName(" << i << ");";
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

