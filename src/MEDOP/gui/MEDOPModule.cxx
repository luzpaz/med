// Copyright (C) 2007-2015  CEA/DEN, EDF R&D
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

#include "MEDOPModule.hxx"
#include "QtHelper.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS)

#include "MEDOPFactoryClient.hxx"
#ifndef DISABLE_PVVIEWER
#include "PVViewer_ViewModel.h"
#endif

MEDOPModule::MEDOPModule() :
  StandardApp_Module()
{
  // Note also that we can't use the getApp() function here because
  // the initialize(...) function has not been called yet.

  this->setModuleName("MED");
}

//
// =====================================================================
// This part implements the mandatory interface
// =====================================================================
//

/*!
 * Returns the engine of the XMED module, i.e. the SALOME component
 * associated to the study root of the module.
 */
Engines::EngineComponent_ptr MEDOPModule::getEngine() const {
  return MEDOPFactoryClient::getFactory();
}

/*!
 * Returns the base file name of the image used for the icon's
 * module. The file is supposed to be located in the resource
 * directory of the module.
 */
QString MEDOPModule::studyIconName() {
  return tr("ICO_MEDOP_SMALL");
}

//
// =====================================================================
// This part add custom widgets (a dockwidget containing a tree view
// in this example) and add special actions in the toolbox of the
// module.
// =====================================================================
//

/*!
 * This function implements the interface StandardApp_Module. It
 * creates the widgets specific for this module, in particular the
 * workspace widget and the dataspace widget.
 */
void MEDOPModule::createModuleWidgets() {
  _datasourceController = new DatasourceController(this);
  _workspaceController = new WorkspaceController(this);
  _xmedDataModel  = new XmedDataModel();
  _workspaceController->setDataModel(_xmedDataModel);

  connect(_datasourceController, SIGNAL(datasourceSignal(const DatasourceEvent *)),
    _workspaceController, SLOT(processDatasourceEvent(const DatasourceEvent *)));

  connect(_workspaceController, SIGNAL(workspaceSignal(const MEDOP::MedEvent *)),
    _datasourceController, SLOT(processWorkspaceEvent(const MEDOP::MedEvent *)));
}

bool MEDOPModule::activateModule( SUIT_Study* theStudy )
{
  bool bOk = StandardApp_Module::activateModule( theStudy );
  _workspaceController->showDockWidgets(true);
  this->setDockLayout(StandardApp_Module::DOCKLAYOUT_LEFT_VLARGE);
  return bOk;
}
bool MEDOPModule::deactivateModule( SUIT_Study* theStudy )
{
  _workspaceController->showDockWidgets(false);
  this->unsetDockLayout();
  return StandardApp_Module::deactivateModule( theStudy );
}

void MEDOPModule::createModuleActions() {
  // Creating actions concerning the dataspace
  _datasourceController->createActions();
  // Creating actions concerning the workspace
  _workspaceController->createActions();
}

void MEDOPModule::viewManagers( QStringList& list ) const
{
#ifndef DISABLE_PVVIEWER
  list.append( PVViewer_Viewer::Type() );
#endif
}
