// Copyright (C) 2011-2014  CEA/DEN, EDF R&D
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
// Author : Guillaume Boulant (EDF)

#include "XmedConsoleDriver.hxx"
#include "Utils_SALOME_Exception.hxx"

XmedConsoleDriver::XmedConsoleDriver(SalomeApp_Application * application) {

  bool forcePythonConsole = true;
  _pyConsole = application->pythonConsole(forcePythonConsole);
  if ( !_pyConsole ) {
    const char * msg = "The python console can't be obtained from the SALOME application";
    throw SALOME_Exception(msg);
  }
  _importXmedDone = false;
}

/*!
 * This function sends instructions to the python console to setup the
 * environment for med operations (in particular, it imports the medop
 * commands). Even if the function is called twice, then the
 * instructions are sent once.
 */
void XmedConsoleDriver::setup() {

  if ( !_importXmedDone ) {
    QStringList commands;
    // First import xmed to initialize the main objects, in particular
    // the corba pointer to the event listener.
    commands+="import xmed";
    // Set the globals dictionnary so that the fields tools work properly.
    commands+="xmed.setConsoleGlobals(globals())";
    // Import the tools required for field operations
    commands+="from xmed import load, get, put, dup, ls, la, save, view, doc, wipe, remove, clean";
    // A last one to clear the console screen
    //commands+="wipe";

    // start PVServer and show render view
    commands+="import pvsimple as pvs";
    commands+="";
    //commands+="pvs.ShowParaviewView()";

    this->exec(commands);
    _importXmedDone = true;
  }
}

/*!
 * This function sends the specified list of commands to the console.
 */
void XmedConsoleDriver::exec(QStringList commands) {
  QStringListIterator it(commands);
  while (it.hasNext()) {
    _pyConsole->exec(it.next());
  }
}
