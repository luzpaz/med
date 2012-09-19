
// Author : Guillaume Boulant (EDF) 

#ifndef _WORKSPACE_CONTROLLER_HXX
#define _WORKSPACE_CONTROLLER_HXX

#include "TreeGuiManager.hxx"
#include "MEDEventListener_i.hxx"
#include "XmedConsoleDriver.hxx"
#include "DatasourceController.hxx"
#include "MEDOPGUI.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDDataManager)
#include <StandardApp_Module.hxx>

#include <vector>

/*!
 * This class defines a DockWidget plugged in the SALOME application,
 * and containing a tree view for rendering a hierarchical data
 * model. This datamodel contains the objects used in the workspace.
 */
class MEDOPGUI_EXPORT WorkspaceController: public TreeGuiManager {

  Q_OBJECT

public:
  WorkspaceController(StandardApp_Module * salomeModule);
  ~WorkspaceController();

  void createActions();

public slots:
  // Slots overiding TreeGuiManager
  void processItemList(QStringList itemNameIdList, int actionId);

  // Internal slots
  void processMedEvent(const MEDOP::MedEvent * event);
  void processDatasourceEvent(const DatasourceEvent * event);
  void OnSaveWorkspace();
  void OnCleanWorkspace();

private:
  XmedConsoleDriver * _consoleDriver;
  StandardApp_Module * _salomeModule;
  void _importItem(QString itemNameId);
  void _importItemList(QStringList itemNameIdList);
  void _importFieldIntoConsole(MEDOP::FieldHandler * fieldHandler,
			       bool askForOptions,
			       const char * alias=NULL);

  void _exportItemList(QStringList itemNameIdList);
  void _saveItemList(QStringList itemNameIdList);
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
