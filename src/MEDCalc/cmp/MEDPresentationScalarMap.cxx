// Copyright (C) 2016  CEA/DEN, EDF R&D
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

#include "MEDPresentationScalarMap.hxx"
#include "MEDPresentationException.hxx"

#include <PyInterp_Utils.h>
#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationScalarMap::TYPE_NAME = "MEDPresentationScalarMap";

MEDPresentationScalarMap::MEDPresentationScalarMap(const MEDCALC::ScalarMapParameters& params,
                                                   const MEDCALC::MEDPresentationViewMode viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap), _params(params)
{
}

void
MEDPresentationScalarMap::internalGeneratePipeline()
{
  PyLockWrapper lock;

  std::ostringstream oss_o, oss_d,oss_l, oss, oss_v;
  oss_o << "__obj" << _objId;      std::string obj(oss_o.str());
  oss_d << "__disp" << _dispId;    std::string disp(oss_d.str());
  oss_l << "__lut" << _lutId;    std::string lut(oss_l.str());

  pushAndExecPyLine( "import pvsimple as pvs;");
  pushAndExecPyLine( getRenderViewCommand() ); // define __viewXXX

  oss_v << "__view" << _renderViewPyId;    std::string view(oss_v.str());

  oss << obj << " = pvs.MEDReader(FileName='" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Populate internal array of available components:
  fillAvailableFieldComponents();
//  dumpIntProperties();
//  dumpStringProperties();

  oss << disp << " = pvs.Show(" << obj << ", " << view << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << "pvs.ColorBy(" << disp << ", ('" << _fieldType << "', '" << _fieldName << "'));";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << disp <<  ".SetScalarBarVisibility(" << view << ", True);";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << getRescaleCommand();
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << lut << " = pvs.GetColorTransferFunction('" << _fieldName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  pushAndExecPyLine(getColorMapCommand()); oss.str("");
  pushAndExecPyLine(getResetCameraCommand());
  pushAndExecPyLine("pvs.Render();");

  // Retrieve Python object for internal storage:
//  PyObject* p_obj = getPythonObjectFromMain(obj.c_str());
//  PyObject* p_disp = getPythonObjectFromMain(disp.c_str());
//  pushPyObjects(std::make_pair(_objId, p_obj), std::make_pair(disp_id, p_disp));
}

void
MEDPresentationScalarMap::updatePipeline(const MEDCALC::ScalarMapParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.displayedComponent != _params.displayedComponent)
    updateComponent<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(std::string(params.displayedComponent));
  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.scalarBarRange);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.colorMap);
}

