// Copyright (C) 2011-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// Author : Guillaume Boulant (EDF) 

#ifndef XMEDCONSOLEDRIVER_HXX
#define XMEDCONSOLEDRIVER_HXX

#include <SalomeApp_Application.h>
#include <PyConsole_Console.h>

#include <QStringList>

#include "MEDOPGUI.hxx"

class MEDOPGUI_EXPORT XmedConsoleDriver {

public:
  XmedConsoleDriver(SalomeApp_Application * application);

  void setup();
  void exec(QStringList commands);

private:
  PyConsole_Console * _pyConsole;
  bool _importXmedDone;
};

#endif //XMEDCONSOLEDRIVER_HXX
