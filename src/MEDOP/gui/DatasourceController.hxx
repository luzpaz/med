
// Author : Guillaume Boulant (EDF) 

#ifndef DATASOURCE_CONTROLLER_H
#define DATASOURCE_CONTROLLER_H

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDDataManager)
#include <StandardApp_Module.hxx>
#include <SALOME_AppStudyEditor.hxx>

#include <map>

#include "XmedDataModel.hxx"
#include "DlgChangeUnderlyingMesh.hxx"
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
    EVENT_VIEW_OBJECT    // View with a salome viewer (SMESH/VISU/PARAVIS)
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
  MEDOP::DatasourceHandler * addDatasource(const char * filename);

public slots:
  // Callback connected to dialog box validation signals
  void OnChangeUnderlyingMeshInputValidated();

signals:
  void datasourceSignal(const DatasourceEvent * event);

protected slots:
  void OnAddDatasource();
  void OnAddImagesource();
  void OnExpandField();
  void OnVisualize();
  void OnUseInWorkspace();
  void OnChangeUnderlyingMesh();


private:
  StandardApp_Module * _salomeModule;
  SALOME_AppStudyEditor * _studyEditor;

  DlgChangeUnderlyingMesh * _dlgChangeUnderlyingMesh;

};

#endif
