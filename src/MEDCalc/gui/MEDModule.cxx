// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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

#include "MEDModule.hxx"
#include "QtHelper.hxx"

#include "SALOME_LifeCycleCORBA.hxx"
#include "QtxPopupMgr.h"

#include <LightApp_Preferences.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Study.h>

#ifndef DISABLE_PVVIEWER
#include "PVViewer_ViewModel.h"
#endif

//! The only instance of the reference to engine
MED_ORB::MED_Gen_var MEDModule::myEngine;

MEDModule::MEDModule() :
  SalomeApp_Module("MED"), _studyEditor(0), _datasourceController(0), _workspaceController(0), _presentationController(0), _processingController(0)
{
  // Note also that we can't use the getApp() function here because
  // the initialize(...) function has not been called yet.

  init(); // internal initialization
}

MEDModule::~MEDModule()
{
  if (_studyEditor)
    delete _studyEditor;
  if (_datasourceController)
    delete _datasourceController;
  //if (_workspaceController)
  //  delete _workspaceController;
  if (_presentationController)
    delete _presentationController;
  if (_processingController)
    delete _processingController;
}

MED_ORB::MED_Gen_var
MEDModule::engine()
{
  init(); // initialize engine, if necessary
  return myEngine;
}

void
MEDModule::init()
{
  // initialize MED module engine (load, if necessary)
  if ( CORBA::is_nil( myEngine ) ) {
    Engines::EngineComponent_var comp =
      SalomeApp_Application::lcc()->FindOrLoad_Component( "FactoryServer", "MED" );
    myEngine = MED_ORB::MED_Gen::_narrow( comp );
  }
}

void
MEDModule::initialize( CAM_Application* app )
{
  // call the parent implementation
  SalomeApp_Module::initialize( app );

  // The following initializes the GUI widget and associated actions
  this->createModuleWidgets();
  this->createModuleActions();
}

QString
MEDModule::engineIOR() const
{
  init(); // initialize engine, if necessary
  CORBA::String_var anIOR = getApp()->orb()->object_to_string( myEngine.in() );
  return QString( anIOR.in() );
}

QString
MEDModule::iconName() const
{
  return tr("ICO_MED_SMALL");
}

void
MEDModule::windows( QMap<int, int>& theMap ) const
{
  // want Object browser, in the left area
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser,
                 Qt::LeftDockWidgetArea );
#ifndef DISABLE_PYCONSOLE
  // want Python console, in the bottom area
  theMap.insert( SalomeApp_Application::WT_PyConsole,
                 Qt::BottomDockWidgetArea );
#endif
}

void
MEDModule::viewManagers( QStringList& list ) const
{
#ifndef DISABLE_PVVIEWER
  list.append( PVViewer_Viewer::Type() );
#endif
}

void
MEDModule::createPreferences()
{
  int genTab = addPreference(tr("PREF_TAB_GENERAL"));

  int themeGroup = addPreference(tr("PREF_THEME_GROUP"), genTab);
  setPreferenceProperty(themeGroup, "columns", 2);
  int icons = addPreference(tr("PREF_ICONS"), themeGroup, LightApp_Preferences::Selector, "MEDCalc", "icons" );
  QStringList iconsThemes;
  iconsThemes.append(tr("PREF_ICON_THEME_MODERN"));
  iconsThemes.append(tr("PREF_ICON_THEME_CLASSIC"));
  QList<QVariant> indices;
  indices.append(0);
  indices.append(1);
  setPreferenceProperty(icons, "strings", iconsThemes);
  setPreferenceProperty(icons, "indexes", indices);
}

bool
MEDModule::activateModule( SUIT_Study* theStudy )
{
  if ( CORBA::is_nil( myEngine ) )
    return false;

  // call parent implementation
  bool bOk = SalomeApp_Module::activateModule( theStudy );
  if (!bOk)
    return false;

  // show own menus
  setMenuShown( true );
  // show own toolbars
  setToolShown( true );

  //this->createStudyComponent(theStudy);
  _workspaceController->showDockWidgets(true);
  _presentationController->showDockWidgets(true);
  //this->setDockLayout(StandardApp_Module::DOCKLAYOUT_LEFT_VLARGE);

  // return the activation status
  return bOk;
}

bool
MEDModule::deactivateModule( SUIT_Study* theStudy )
{
  _workspaceController->showDockWidgets(false);
  _presentationController->showDockWidgets(false);
  //this->unsetDockLayout();

  // hide own menus
  setMenuShown( false );
  // hide own toolbars
  setToolShown( false );

  // call parent implementation and return the activation status
  return SalomeApp_Module::deactivateModule( theStudy );
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
void
MEDModule::createModuleWidgets() {
  _studyEditor = new SALOME_AppStudyEditor(getApp());
  _datasourceController = new DatasourceController(this);
  _workspaceController = new WorkspaceController(this);
  _xmedDataModel  = new XmedDataModel();
  _workspaceController->setDataModel(_xmedDataModel);
  _presentationController = new PresentationController(this);
  _processingController = new ProcessingController(this);

  connect(_datasourceController, SIGNAL(datasourceSignal(const DatasourceEvent*)),
    _workspaceController, SLOT(processDatasourceEvent(const DatasourceEvent*)));

  connect(_presentationController, SIGNAL(presentationSignal(const PresentationEvent*)),
    _workspaceController, SLOT(processPresentationEvent(const PresentationEvent*)));

  connect(_processingController, SIGNAL(processingSignal(const ProcessingEvent*)),
    _workspaceController, SLOT(processProcessingEvent(const ProcessingEvent*)));

  connect(_workspaceController, SIGNAL(workspaceSignal(const MEDCALC::MedEvent*)),
    _datasourceController, SLOT(processWorkspaceEvent(const MEDCALC::MedEvent*)));

  connect(_workspaceController, SIGNAL(workspaceSignal(const MEDCALC::MedEvent*)),
    _presentationController, SLOT(processWorkspaceEvent(const MEDCALC::MedEvent*)));
}

void
MEDModule::createModuleActions() {
  _datasourceController->createActions();
  _workspaceController->createActions();
  _presentationController->createActions();
  _processingController->createActions();
}

int
MEDModule::createStandardAction(const QString& label,
                                QObject* slotobject,
                                const char* slotmember,
                                const QString& iconName,
                                const QString& tooltip)
{
  SUIT_Desktop* dsk = getApp()->desktop();
  SUIT_ResourceMgr* resMgr = getApp()->resourceMgr();

  // If the tooltip is not defined, we choose instead the label text.
  QString effToolTip(tooltip);
  if ( effToolTip.isEmpty() )
    effToolTip = label;

  QAction* action = createAction(-1,
                                 label,
                                 resMgr->loadPixmap("MED", iconName),
                                 label,
                                 effToolTip,
                                 0,
                                 dsk,
                                 false,
                                 slotobject,
                                 slotmember
                                 );
  return actionId(action);
}

void
MEDModule::addActionInPopupMenu(int actionId,const QString& menus,const QString& rule)
{
  // _GBO_ for a fine customization of the rule (for example with a
  // test on the type of the selected object), see the LIGTH module:
  // implement "LightApp_Selection*    createSelection() const;"
  int parentId = -1;
  QtxPopupMgr* mgr = this->popupMgr();
  this->action( actionId )->setIconVisibleInMenu(true);
  if (! menus.isEmpty())
    mgr->insert ( this->action( actionId ), menus, parentId, 0 );
  else
    mgr->insert ( this->action( actionId ), parentId, 0 );
  mgr->setRule( this->action( actionId ), rule, QtxPopupMgr::VisibleRule );
}

MEDCALC::MEDPresentationViewMode
MEDModule::getSelectedViewMode()
{
  return _presentationController->getSelectedViewMode();
}

MEDCALC::MEDPresentationColorMap
MEDModule::getSelectedColorMap()
{
  return _presentationController->getSelectedColorMap();
}
