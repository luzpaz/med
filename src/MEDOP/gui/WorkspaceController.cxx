// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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

#include "WorkspaceController.hxx"
#include "QtHelper.hxx"
#include "MEDOPFactoryClient.hxx"
#include "XmedDataModel.hxx"
#include "DlgAlias.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDEventListener)

#include <SalomeApp_Application.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include <SUIT_FileDlg.h>
#include <SUIT_Desktop.h>

/*!
 * This class defines a DockWidget plugged in the SALOME application,
 * and containing a tree view for rendering a hierarchical data
 * model. This datamodel contains the objects used in the workspace.
 */
WorkspaceController::WorkspaceController(StandardApp_Module * salomeModule)
  : TreeGuiManager(salomeModule->getApp(), "Workspace")
{
  _salomeModule = salomeModule;

  this->tabifyDockWidgets(false);

  // -------------------------------------------------------------
  // Setup the MEDEventListener to manage notification from the
  // python console.

  // We create a MEDEventListener CORBA object inside this GUI class
  // with the role of listening events coming from the python console
  // (or even the components if needed). The events arising in the
  // python console are send as CORBA request to this CORBA
  // servant. Then this object can process the event by notifying the
  // GUI of something to update for example (using signals and slots
  // of course).
  _medEventListener = MEDEventListener_i::getInstance();
  MEDOP::MEDEventListener_ptr medEventListenerServant = _medEventListener->_this();

  // We store the IOR inside the MEDDataManager to share this data
  // with other parts of the application, in particular the python
  // console that could retrieve this IOR using the
  // getEventListenerIOR() function of the MEDDataManager.
  SalomeApp_Application * salomeApp = salomeModule->getApp();
  const char * medEventListenerIOR = 
    salomeApp->orb()->object_to_string(medEventListenerServant);
  MEDOPFactoryClient::getDataManager()->setEventListenerIOR(medEventListenerIOR);

  // Connect the signals emitted from the MEDEventListener to slot of
  // this class.
  connect(_medEventListener, SIGNAL(medEventSignal(const MEDOP::MedEvent*)),
    this, SLOT(processMedEvent(const MEDOP::MedEvent*)));
  // >>> WARN:
  // Note that this class must be mocked (Q_OBJECT + moc file
  // generated from header file) so that to be able to connect a
  // signal to a slot of this class.

  // -------------------------------------------------------------
  // Customize the treeview rendering the datamodel with specific
  // action for the popup menu
  this->getDataTreeView()->clearActions();
  _actionIds.display    = this->getDataTreeView()->addAction(QObject::tr("Visualiser  (une carte scalaire)"));
  _actionIds.useInTui   = this->getDataTreeView()->addAction(QObject::tr("Utiliser    (dans la console)"));
  _actionIds.exportToPv = this->getDataTreeView()->addAction(QObject::tr("Exporter    (vers PARAVIS)"));
  _actionIds.save       = this->getDataTreeView()->addAction(QObject::tr("Sauvegarder (dans un fichier med)"));
  _actionIds.remove     = this->getDataTreeView()->addAction(QObject::tr("Supprimer   (de lespace de travail)"));
  
  // -------------------------------------------------------------
  // Initialize the python console. Note that this must be done at
  // last because the setup will try to initiate a connection to the
  // event listener.
  _consoleDriver = new XmedConsoleDriver(salomeApp);
  _consoleDriver->setup();
}

WorkspaceController::~WorkspaceController() {
  MEDEventListener_i::release();  
}

/**
 * This creates the GUI actions for driving the Workspace. The
 * WorkspaceController creates itself this actions and implements the
 * connected slots. 
 */
void WorkspaceController::createActions() {

  QString label   = QString("Save workspace");
  QString tooltip = QString("Save the workspace (fields and meshes) in a med file");
  QString icon    = QString("workspace_save.png");
  int actionId = _salomeModule->createStandardAction(label,this,SLOT(OnSaveWorkspace()),icon,tooltip);
  _salomeModule->addActionInToolbar(actionId);

  label   = QString("Clean workspace");
  tooltip = QString("Clean all data in the workspace");
  icon    = QString("workspace_clean.png");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnCleanWorkspace()),icon,tooltip);
  _salomeModule->addActionInToolbar(actionId);
}

/*!
 * Implementation of the slot processItemList inherited from TreeGuiManager
 */
void WorkspaceController::processItemList(QStringList itemNameIdList, int actionId) {
  if ( actionId == _actionIds.display ) {
    STDLOG("WorkspaceController::processItemList: display");
    this->_viewItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.useInTui ) {
    STDLOG("WorkspaceController::processItemList: use");
    this->_importItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.exportToPv ) {
    STDLOG("WorkspaceController::processItemList: export");
    this->_exportItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.save ) {
    STDLOG("WorkspaceController::processItemList: save");
    this->_saveItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.remove ) {
    STDLOG("WorkspaceController::processItemList: remove");
  }
  else {
    STDLOG("WorkspaceController::processItemList: ERR : action unknown ");
  }
}

/*!
 * This function import in the console all the fields associated to
 * the model items of the specified list. "Import a fields" means
 * "define a field proxy variable in the python context to manipulate
 * the real field in the database".
 */
void WorkspaceController::_importItemList(QStringList itemNameIdList) {
  LOG("WorkspaceController: signal received : display item list "<<itemNameIdList);
  QStringList::const_iterator it;
  for (it = itemNameIdList.constBegin(); it != itemNameIdList.constEnd(); ++it) {
    QString itemNameId = *it;
    this->_importItem(itemNameId);
  }
}

/*!
 * This function is the unit function used to import field in the
 * console (see _importItemList).
 */
void WorkspaceController::_importItem(QString itemNameId) {
  XmedDataModel * dataModel = (XmedDataModel *)this->getDataModel();
  if ( dataModel == NULL ) {
    LOG("No data model associated to this tree view");
    return;
  }

  // We can request the dataModel to obtain the dataObject associated
  // to this item (iteNameId is a TreeView id, Qt stuff only).
  XmedDataObject * dataObject =
    (XmedDataObject *)dataModel->getDataObject(QS2S(itemNameId));
  
  if ( dataObject == NULL ) {
    LOG("WorkspaceController: WARN! No data object associated to the item "<<itemNameId);
    return;
  }
  
  // Then, we can request this data object to obtain the associated
  // FieldHandler.
  MEDOP::FieldHandler * fieldHandler = dataObject->getFieldHandler();
  STDLOG("Field: mesh="<<fieldHandler->meshname<<" name="<<fieldHandler->fieldname);
  
  // Finally, we can import the field 
  bool askForOptions = true;
  _importFieldIntoConsole(fieldHandler, askForOptions);
}

/**
 * This function import the specified field into the tui console. This
 * means to define a field proxy variable in the python context to
 * manipulate the field. We can raise a gui to specify some import
 * options or simply specify the alias (i.e. the name of the python
 * variable).   
 */
void WorkspaceController::_importFieldIntoConsole(MEDOP::FieldHandler * fieldHandler,
              bool askForOptions,
              const char * alias)
{
  STDLOG("alias="<<alias);

  // By default, the alias is the name of the field
  QString *effectiveAlias;
  if ( alias == NULL ) {
    effectiveAlias = new QString(fieldHandler->fieldname);
  }
  else {
    effectiveAlias = new QString(alias);
  }

  // We can propose to the user to specify some additionnal
  // informations concerning what must be imported.
  // 
  // In this version, we just ask the alias the field will be
  // manipulated with. The default alias is the field name. This alias
  // should be asked to the user to get a short name to manipulate.
  if ( askForOptions ) {
    DlgAlias dialog;
    dialog.setAlias(*effectiveAlias);
    int choice = dialog.exec();
    if ( choice == QDialog::Rejected ) {
      // The user decides to cancel the operation
      return;
    }
    *effectiveAlias = dialog.getAlias();
  }

  //
  // Then, the list of python commands can be written and executed to
  // define the field in the console
  //
  QStringList commands;
  commands+=QString("%1=xmed.fieldproxy.newFieldProxy(fieldHandlerId=%2)")
    .arg(*effectiveAlias)
    .arg(fieldHandler->id);

  _consoleDriver->exec(commands);
}

/*!
 * This function is a Qt slot connected to the signal medEventSignal
 * emitted from the MEDEventListener. It processes events coming from
 * the python console.
 */
void WorkspaceController::processMedEvent(const MEDOP::MedEvent * event) {
  STDLOG("WorkspaceController::processMedEvent");
  STDLOG("fieldid  :"<<event->fieldid);

  XmedDataModel * dataModel = (XmedDataModel *)this->getDataModel();
  if ( dataModel == NULL ) {
    STDLOG("No data model associated to this tree view");
    return;
  }
  
  if ( event->type == MEDOP::EVENT_ADDNEW_FIELD ) {
    STDLOG("add new field");
    MEDOP::FieldHandler * fieldHandler =
      MEDOPFactoryClient::getDataManager()->getFieldHandler(event->fieldid);
    
    XmedDataObject * dataObject = (XmedDataObject *)dataModel->newDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    this->getDataTreeModel()->addData(dataObject);
  }

}

/*!
 * This function save a list of fields in a med file. The med file
 * name is requested to the user using a file chooser dialog box 
 */
void WorkspaceController::_saveItemList(QStringList itemNameIdList) {
  XmedDataProcessor * dataProcessor = new XmedDataProcessor(this->getDataModel());
  dataProcessor->process(itemNameIdList);
  MEDOP::FieldIdList_var fieldIdList = dataProcessor->getResultingFieldIdList();
  delete dataProcessor;

  QStringList filter;
  filter.append(QObject::tr("MED files (*.med)"));
  QString filename = SUIT_FileDlg::getFileName(_salomeModule->getApp()->desktop(),
                                               "",
                                               filter,
                                               QObject::tr("Save selected fields"),
                                               false);

  if ( filename.isEmpty() ) return;

  MEDOPFactoryClient::getDataManager()->saveFields(QCHARSTAR(filename), fieldIdList);
}

/**
 * This function export the list of specified field item to PARAVIS
 * module. This consists in create a med file gathering the selected
 * items, then to import this file in PARAVIS, and finally to create a
 * scalar map of the first item to start the job. 
 */
void WorkspaceController::_exportItemList(QStringList itemNameIdList) {
  XmedDataProcessor * dataProcessor = new XmedDataProcessor(this->getDataModel());
  dataProcessor->process(itemNameIdList);
  MEDOP::FieldIdList_var fieldIdList = dataProcessor->getResultingFieldIdList();
  delete dataProcessor;

  // _GBO_ We use a temporary file to proceed with this export to
  // paravis. I'm sure it could be better in a futur version or when I
  // will get a better understanding of paravis API.
  const char * tmpfilename = "/tmp/medop_export2paravis.med";
  MEDOPFactoryClient::getDataManager()->saveFields(tmpfilename, fieldIdList);

  // We import the whole file but create a scalar map for the first
  // selected field only (it's just an export to continue the job in
  // paravis)
  XmedDataModel * dataModel = (XmedDataModel *)this->getDataModel();
  if ( dataModel == NULL ) {
    STDLOG("No data model associated to this tree view");
    return;
  }
  QString itemNameId = itemNameIdList[0];
  XmedDataObject * dataObject = (XmedDataObject *)dataModel->getDataObject(QS2S(itemNameId));
  if ( dataObject == NULL ) {
    LOG("WorkspaceController: WARN! No data object associated to the item "<<itemNameId);
    return;
  }
  MEDOP::FieldHandler * fieldHandler = dataObject->getFieldHandler();
  QStringList commands;
  commands+=QString("from xmed.driver_pvis import pvis_scalarmap");
  commands+=QString("pvis_scalarmap('%1','%2','%3',%4,%5)")
    .arg(tmpfilename)
    .arg(QString(fieldHandler->meshname))
    .arg(QString(fieldHandler->fieldname))
    .arg(fieldHandler->type)
    .arg(fieldHandler->iteration);
  _consoleDriver->exec(commands);

}


/*!
 * This function sends a request to the SALOME data visualisation
 * (module VISU or PARAVIS) for displaying a scalar map of the fields
 * associated to the model items in the specified list.
 * 
 */
void WorkspaceController::_viewItemList(QStringList itemNameIdList) {

  // __GBO__: In this version, we consider only the first field in the selection
  QString itemNameId = itemNameIdList[0];

  XmedDataModel * dataModel = (XmedDataModel *)this->getDataModel();
  if ( dataModel == NULL ) {
    LOG("No data model associated to this tree view");
    return;
  }

  // We can request the dataModel to obtain the dataObject associated
  // to this item (iteNameId is a TreeView id, Qt stuff only).
  XmedDataObject * dataObject =
    (XmedDataObject *)dataModel->getDataObject(QS2S(itemNameId));
  if ( dataObject == NULL ) {
    LOG("WorkspaceController: WARN! No data object associated to the item "<<itemNameId);
    return;
  }
  
  // Then, we can request this data object to obtain the associated
  // FieldHandler.
  MEDOP::FieldHandler * fieldHandler = dataObject->getFieldHandler();

  // And finally, we can create the set of medop instructions to
  // generate the scalar map on this field.
  QStringList commands;
  commands+=QString("view(get(%1))").arg(fieldHandler->id);
  _consoleDriver->exec(commands);
}

/**
 * This slot can process the event coming from the
 * DatasourceController. The connection between the datasource signal
 * and this slot is realized by the main class MEDOPModule.
 */
void WorkspaceController::processDatasourceEvent(const DatasourceEvent * event) {
  XmedDataModel * dataModel = (XmedDataModel *)this->getDataModel();
  if ( dataModel == NULL ) {
    STDLOG("No data model associated to this tree view");
    return;
  }
  
  // >>>
  // __GBO__ To know what to do we should test the type, because the
  // object could be a mesh, a timeseries or a single field. We test
  // here the case of a single field. Moreover, there could have
  // options such that "change the underlying mesh".
  // <<<
  
  XmedDataObject * dataObject = event->objectdata;

  if ( event->eventtype == DatasourceEvent::EVENT_IMPORT_OBJECT ) {
    STDLOG("IMPORT object in workspace:\n"<<dataObject->toString());
    // _GBO_ QUESTION: tag automatically the object as a peristant object ??
    // We first add the data object to the internal data model
    dataModel->addDataObject(dataObject);
    // Then we request the tree view to consider this new object
    this->getDataTreeModel()->addData(dataObject);
  }
  else if ( event->eventtype == DatasourceEvent::EVENT_USE_OBJECT ) {
    STDLOG("USE object in workspace:\n"<<dataObject->toString());
    // We first add the data object to the internal data model
    dataModel->addDataObject(dataObject);
    // Then we request the tree view to consider this new object
    this->getDataTreeModel()->addData(dataObject);

    // We define a proxy for this object in the tui console.
    STDLOG("Define a proxy variable in the console with name : "<<QCHARSTAR(event->objectalias));
    bool askForOptions = false;
    _importFieldIntoConsole(dataObject->getFieldHandler(),
          askForOptions,
          QCHARSTAR(event->objectalias));
  }
  else if ( event->eventtype == DatasourceEvent::EVENT_VIEW_OBJECT ) {
    QStringList commands;
    commands+=QString("view(get(%1))").arg(dataObject->getFieldHandler()->id);
    _consoleDriver->exec(commands);
  }
  else {
    STDLOG("The event "<<event->eventtype<<" is not implemented yet");
  }

}

void WorkspaceController::OnSaveWorkspace() {

  // Dialog to get the filename where the workspace must be saved into
  QStringList filter;
  filter.append(QObject::tr("MED files (*.med)"));

  QString filename = SUIT_FileDlg::getFileName(_salomeModule->getApp()->desktop(),
                                               "",
                                               filter,
                                               QObject::tr("Save workspace data"),
                                               false);

  if ( filename.isEmpty() ) return;

  STDLOG("OnWorkspaceSave: save the workspace in the file " << QCHARSTAR(filename));
  QStringList commands;
  commands+=QString("save('%1')").arg(filename);
  _consoleDriver->exec(commands);
}

#include <QMessageBox>
void WorkspaceController::OnCleanWorkspace() {
  QMessageBox::warning(_salomeModule->getApp()->desktop(),
           tr("Not implemented yet"),
           tr("This function is not implemented yet"));
}
