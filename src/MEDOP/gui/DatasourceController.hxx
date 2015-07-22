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

#ifndef DATASOURCE_CONTROLLER_H
#define DATASOURCE_CONTROLLER_H

#include "MEDEventListener_i.hxx"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDDataManager)
#include <StandardApp_Module.hxx>
#include <SALOME_AppStudyEditor.hxx>

#include <map>

#include "XmedDataModel.hxx"
#include "DlgChangeUnderlyingMesh.hxx"
#include "DlgInterpolateField.hxx"
#include "MEDOPGUI.hxx"
//
// The DatasourceController defines data structures and classes to
// manage the usage of objects in the data space. The data space is
// technically the study, whose graphical representation is the
// objects browser.
//

// A datasource event is used by the DatasourceController to notify a
// job to the Workspace controller (Qt signal/slot pattern connected
// by the main controller MEDOPModule). The setting of attributes and
// usage of these attributes in the WorkspaceController depends on the
// event type. For example, in the case of a IMPORT event, we just
// have to transmit the object, while in the USE event we have to
// transmit an object AND an alias.
typedef struct {
  enum EventType {
    //
    EVENT_IMPORT_OBJECT, // Simply import the object in the workspace
    EVENT_USE_OBJECT,    // Import in the workspace AND define a proxy
                         // variable in the tui console to use it
    EVENT_VIEW_OBJECT_SCALAR_MAP,
    // these ones forward actions to workspace (and then to python console)
    EVENT_ADD_DATASOURCE,
    EVENT_ADD_IMAGE_AS_DATASOURCE
  };
  int eventtype;
  XmedDataObject * objectdata;
  QString objectalias;
} DatasourceEvent;

//
// ==============================================================
// Datasource controller
// ==============================================================
//
class MEDOPGUI_EXPORT DatasourceController: public QObject {
  Q_OBJECT

public:
  DatasourceController(StandardApp_Module * salomeModule);
  ~DatasourceController();

  void createActions();

public slots:
  // Callback connected to dialog box validation signals
  void OnChangeUnderlyingMeshInputValidated();
  void OnInterpolateFieldInputValidated();
  void processWorkspaceEvent(const MEDOP::MedEvent * event);

signals:
  void datasourceSignal(const DatasourceEvent * event);

protected slots:
  void OnAddDatasource();
  void OnAddImagesource();
  void OnExpandField();
  void OnVisualizeScalarMap();
  void OnUseInWorkspace();
  void OnChangeUnderlyingMesh();
  void OnInterpolateField();

private:
  void visualize(DatasourceEvent::EventType);
  void addDatasource(const char* filename);
  void updateTreeViewWithNewDatasource(const MEDOP::DatasourceHandler*);

private:
  StandardApp_Module * _salomeModule;
  SALOME_AppStudyEditor * _studyEditor;

  DlgChangeUnderlyingMesh * _dlgChangeUnderlyingMesh;
  DlgInterpolateField * _dlgInterpolateField;

};

#endif
