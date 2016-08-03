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

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_

#include <Python.h>
#include "MEDCouplingRefCountObject.hxx"
#include "MEDCALC.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDPresentationManager)

#include <vector>
#include <map>
#include <string>

class MEDCALC_EXPORT MEDPresentation
{
  friend class MEDPresentationManager_i;

public:
  typedef ::CORBA::Long TypeID;

  virtual ~MEDPresentation();

  static const std::string PROP_NAME;                 // name of the presentation
  static const std::string PROP_NB_COMPONENTS;        // number of field components
  static const std::string PROP_SELECTED_COMPONENT;   // index of the selected component - 0 means Euclidean norm
  static const std::string PROP_COMPONENT;            // string prefix for all properties storing component names
  static const std::string PROP_COLOR_MAP;            // color map - TODO: arch fix - should be in params only
  static const std::string PROP_SCALAR_BAR_RANGE;     // scalar bar range - TODO: arch fix - should be in params only

  virtual void setStringProperty(const std::string& propName, const std::string& propValue);
  const std::string getStringProperty(const std::string& propName) const;

  virtual void setIntProperty(const std::string& propName, const int propValue);
  int getIntProperty(const std::string& propName) const;

  void activateView();  // non const because generates a Python trace

  void dumpIntProperties() const;
  void dumpStringProperties() const;

  std::string paravisDump() const;

  long getPyViewID() const { return _renderViewPyId; }
  void setPyViewID(long id) { _renderViewPyId = id; }

protected:
  typedef std::pair<int, PyObject *> PyObjectId;
  static int GeneratePythonId();

  MEDPresentation(MEDPresentation::TypeID fieldHandlerId, const std::string& name,
                  const MEDCALC::ViewModeType viewMode,
                  const MEDCALC::ColorMapType colorMap,
                  const MEDCALC::ScalarBarRangeType sbRange);
  std::string getRenderViewVar() const;

  // The most common elements of the ParaView pipeline:
  void setOrCreateRenderView();
  void createSource();
  void selectFieldComponent();
  void showObject();
  void colorBy(const std::string & fieldType);
  void showScalarBar();
  void rescaleTransferFunction();
  void selectColorMap();
  void resetCameraAndRender();

  virtual void internalGeneratePipeline();
  PyObject* getPythonObjectFromMain(const char* var) const;
//  void pushPyObjects(PyObjectId obj, PyObjectId disp);
  void execPyLine(const std::string & lin);
  void pushAndExecPyLine(const std::string & lin);

  MEDPresentation::TypeID getID() const { return _fieldHandlerId; }

  void fillAvailableFieldComponents();
  void applyCellToPointIfNeeded();
  void convertTo3DVectorField();
//  double computeCellAverageSize();
  //void computeFieldRange

//  virtual MEDCALC::ViewModeType getViewMode() = 0;

  template<typename PresentationType, typename PresentationParameters>
  void updateComponent(const std::string& newCompo);

  template<typename PresentationType, typename PresentationParameters>
  void updateColorMap(MEDCALC::ColorMapType colorMap);

  template<typename PresentationType, typename PresentationParameters>
  void updateScalarBarRange(MEDCALC::ScalarBarRangeType sbRange);

  template<typename PresentationType, typename PresentationParameters>
  void getParameters(PresentationParameters& params) const;

  template<typename PresentationType, typename PresentationParameters>
  void setParameters(const PresentationParameters& params);

private:
  std::string getFieldTypeString(MEDCoupling::TypeOfField fieldType) const;

  // The following functions are reserved to friend class MEDPresentationManager
  void generatePipeline();

  template<typename PresentationType, typename PresentationParameters>
  void updatePipeline(const PresentationParameters& params);

protected:
  std::string _meshName;
  std::string _fileName;
  std::string _fieldName;
  std::string _fieldType;

  MEDPresentation::TypeID _fieldHandlerId;

  int _selectedComponentIndex;
  MEDCALC::ViewModeType _viewMode;
  MEDCALC::ColorMapType _colorMap;
  MEDCALC::ScalarBarRangeType _sbRange;

  ///! Identifier (in the Python dump) of the render view
  int _renderViewPyId;
  ///! ParaView object variable in the Python scripting commands (source object)
  std::string _srcObjVar;
  ///! ParaView object variable in the Python scripting commands (final object on which all display commands are done)
  std::string _objVar;
  ///! ParaView display variable in the Python scripting commands
  std::string _dispVar;
  ///! ParaView LUT variable in the Python scripting commands
  std::string _lutVar;

private:
  ///! Pipeline elements
//  std::vector<PyObjectId> _pipeline;

  ///! Corresponding display object, if any:
//  std::vector<PyObjectId> _display;

  ///! Presentation properties <key,value>
  std::map<std::string, std::string> _propertiesStr;
  std::map<std::string, int> _propertiesInt;

  std::vector<std::string> _pythonCmds;

  mutable PyObject* _globalDict;
};

#include "MEDPresentation.txx"

#endif /* SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_ */
