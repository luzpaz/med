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

#ifndef _WORKSPACE_CONTROLLER_HXX
#define _WORKSPACE_CONTROLLER_HXX

#include "TreeGuiManager.hxx"
#include "MEDEventListener_i.hxx"
#include "XmedConsoleDriver.hxx"
#include "DatasourceController.hxx"
#include "MEDCALCGUI.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDDataManager)
#include <StandardApp_Module.hxx>

#include <vector>

/*!
 * This class defines a DockWidget plugged in the SALOME application,
 * and containing a tree view for rendering a hierarchical data
 * model. This datamodel contains the objects used in the workspace.
 */
class MEDCALCGUI_EXPORT WorkspaceController: public TreeGuiManager {

  Q_OBJECT

public:
  WorkspaceController(StandardApp_Module * salomeModule);
  ~WorkspaceController();

  void createActions();

public slots:
  // Slots overiding TreeGuiManager
  void processItemList(QStringList itemNameIdList, int actionId);

  // Internal slots
  void processMedEvent(const MEDCALC::MedEvent * event);
  void processDatasourceEvent(const DatasourceEvent * event);
  void OnSaveWorkspace();
  void OnCleanWorkspace();

signals:
  void workspaceSignal(const MEDCALC::MedEvent * event);

private:
  XmedConsoleDriver * _consoleDriver;
  StandardApp_Module * _salomeModule;
  void _importItem(QString itemNameId);
  void _importItemList(QStringList itemNameIdList);
  void _importFieldIntoConsole(MEDCALC::FieldHandler * fieldHandler,
              bool askForOptions,
              const char * alias=NULL);

  void _exportItemList(QStringList itemNameIdList);
  void _saveItemList(QStringList itemNameIdList);
  void _removeItemList(QStringList itemNameIdList);
  void _viewItemList(QStringList itemNameIdList);

  MEDEventListener_i * _medEventListener;

  // This structure is intended to memorized in a bundle the whole set
  // of integers identifying the actions of the popup menu associated
  // to the TreeView embedded in this TreeGuiManager
  typedef struct {
    int display;
    int useInTui;
    int exportToPv;
    int remove;
    int save;
  } ActionIdentifiers;

  ActionIdentifiers _actionIds;

};

#endif // _WORKSPACE_CONTROLLER_HXX
