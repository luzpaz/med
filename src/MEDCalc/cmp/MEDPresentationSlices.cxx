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

#include "MEDPresentationSlices.hxx"

void
MEDPresentationSlices::internalGeneratePipeline()
{
  PyGILState_STATE _gil_state = PyGILState_Ensure();

  std::string cmd = std::string("import pvsimple as pvs;");
  cmd += getRenderViewCommand(_params.viewMode); // define __view1

  cmd += std::string("__obj1 = pvs.MEDReader(FileName='") + _fileName + std::string("');");
  cmd += std::string("__slice1 = pvs.Slice(Input=__obj1);");
  cmd += std::string("__disp1 = pvs.Show(__slice1, __view1);");
  cmd += std::string("pvs.ColorBy(__disp1, ('") + _fieldType + std::string("', '") + _fieldName + std::string("'));");
  cmd += std::string("__disp1.SetScalarBarVisibility(__view1, True);");
  cmd += std::string("__disp1.RescaleTransferFunctionToDataRangeOverTime();");
  cmd += std::string("pvs.Render();");

  cmd += getResetCameraCommand();

  //std::cerr << "Python command:" << std::endl;
  //std::cerr << cmd << std::endl;
  PyRun_SimpleString(cmd.c_str());
  // Retrieve Python object for internal storage:
  PyObject* obj = getPythonObjectFromMain("__slice1");
  PyObject* disp = getPythonObjectFromMain("__disp1");
  pushInternal(obj, disp);

  PyGILState_Release(_gil_state);
}
