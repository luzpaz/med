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

#include "MEDPresentationContour.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationContour::TYPE_NAME = "MEDPresentationContour";
const std::string MEDPresentationContour::PROP_NB_CONTOUR = "nbContour";

MEDPresentationContour::MEDPresentationContour(const MEDCALC::ContourParameters& params,
                                               const MEDCALC::MEDPresentationViewMode viewMode) :
        MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
        _params(params)
{
  setIntProperty(MEDPresentationContour::PROP_NB_CONTOUR, params.nbContours);
}

std::string
MEDPresentationContour::getContourCommand() const
{
  std::ostringstream oss1;

  oss1 << "min_max = " << _srcObjVar << ".PointData.GetArray('" << _fieldName << "').GetRange();\n";
  oss1 << "delta = (min_max[1]-min_max[0])/float(" << _params.nbContours << ");\n";
  oss1 << _objVar << ".Isosurfaces = [min_max[0]+0.5*delta+i*delta for i in range(" << _params.nbContours << ")];\n";
  return oss1.str();
}

void
MEDPresentationContour::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  std::ostringstream oss_o, oss;
  std::string view(getRenderViewVar());

  oss << _srcObjVar << " = pvs.MEDReader(FileName='" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  if (getIntProperty(MEDPresentation::PROP_NB_COMPONENTS) > 1)
    {
      const char * msg = "Contour presentation only works for scalar field!"; // this message will appear in GUI too
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg);
    }

  pushAndExecPyLine( getRenderViewCommand() ); // instanciate __viewXXX

  if(_fieldType == "CELLS")
    {
      // In case of a CELLS field need to convert to POINT field, and update source
      STDLOG("Applying CellDatatoPointData filter");
      std::ostringstream oss2, oss4;
      // Apply Cell data to point data:
      oss2 << "__obj" << GeneratePythonId();
      oss << oss2.str() << " = pvs.CellDatatoPointData(Input=" << _srcObjVar << ");";
      pushAndExecPyLine(oss.str()); oss.str("");
      // Now the source becomes the result of the CellDatatoPointData:
      _srcObjVar = oss2.str();
    }
  oss << _objVar << " = pvs.Contour(Input=" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".ContourBy = ['POINTS', '" << _fieldName << "'];";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Set number of contours
  pushAndExecPyLine(getContourCommand());

  oss << _dispVar << " = pvs.Show(" << _objVar << ", " << view << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << "pvs.ColorBy(" << _dispVar << ", ('POINTS', '" << _fieldName << "'));";  // necessarily POINTS
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar <<  ".SetScalarBarVisibility(" << view << ", True);";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << getRescaleCommand();
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _lutVar << " = pvs.GetColorTransferFunction('" << _fieldName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  pushAndExecPyLine(getColorMapCommand()); oss.str("");
  pushAndExecPyLine(getResetCameraCommand());
  pushAndExecPyLine("pvs.Render();");
}

void
MEDPresentationContour::updatePipeline(const MEDCALC::ContourParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationContour, MEDCALC::ContourParameters>(params.scalarBarRange);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationContour, MEDCALC::ContourParameters>(params.colorMap);

  if (params.nbContours != _params.nbContours)
    updateNbContours(params.nbContours);
}

void
MEDPresentationContour::updateNbContours(const int nbContours)
{
  _params.nbContours = nbContours;

  // GUI helper:
  setIntProperty(MEDPresentationContour::PROP_NB_CONTOUR, nbContours);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    std::string cmd = getContourCommand();
    pushAndExecPyLine(cmd);
    pushAndExecPyLine("pvs.Render();");
  }
}
