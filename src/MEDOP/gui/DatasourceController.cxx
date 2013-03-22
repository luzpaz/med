
// Author : Guillaume Boulant (EDF) 

#include "DatasourceController.hxx"
#include "DatasourceConstants.hxx"

#include "MEDOPFactoryClient.hxx"
#include "QtHelper.hxx"

#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SUIT_FileDlg.h>
#include <SUIT_Desktop.h>

#include <QStringList>
#include <QString>
#include <QMessageBox>

#include "DlgAlias.hxx"

//
// ==============================================================
// Datasource controller
// ==============================================================
//
DatasourceController::DatasourceController(StandardApp_Module * salomeModule) {
  STDLOG("Creating a DatasourceController");
  _salomeModule = salomeModule;
  _studyEditor = new SALOME_AppStudyEditor(_salomeModule->getApp());
  _dlgChangeUnderlyingMesh = new DlgChangeUnderlyingMesh(_studyEditor);

  connect(_dlgChangeUnderlyingMesh,SIGNAL(inputValidated()),
          this, SLOT(OnChangeUnderlyingMeshInputValidated()));

}

DatasourceController::~DatasourceController() {
  STDLOG("Deleting the DatasourceController");
  delete _studyEditor;
}

void DatasourceController::createActions() {
  // 
  // Main actions (toolbar and menubar)
  //
  QString label   = QString("Add Data Source");
  QString tooltip = QString("Add a file data source (file providing med data)");
  QString icon    = QString("datasource_add.png");
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnAddDatasource()),icon,tooltip);
  _salomeModule->addActionInToolbar(actionId);

  label   = QString("Add Image Source");
  tooltip = QString("Create a Data Source from an image file");
  icon    = QString("image_add.png");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnAddImagesource()),icon,tooltip);
  _salomeModule->addActionInToolbar(actionId);
    
  //
  // Actions for popup menu only
  //
  // Expand field timeseries
  label = QString("Expand field timeseries");
  icon  = QString("datasource_expandfield.png");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnExpandField()),icon);
  _salomeModule->addActionInPopupMenu(actionId);
  
  // Create a control view
  label = QString("Visualize");
  icon  = QString("datasource_view.png");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnVisualize()),icon);
  _salomeModule->addActionInPopupMenu(actionId);

  // Use in workspace
  label = QString("Use in workspace");
  icon  = QString("datasource_use.png");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnUseInWorkspace()),icon);
  _salomeModule->addActionInPopupMenu(actionId);

  // Change underlying mesh (note that this action creates a new field in
  // the workspace that corresponds to a copy of the selected field
  // modified by the change of the underlying mesh.
  label = QString("Change underlying mesh");
  icon  = QString("datasource_changeUnderlyingMesh.png");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnChangeUnderlyingMesh()),icon);
  _salomeModule->addActionInPopupMenu(actionId);
}

/**
 * This function adds the specified MED file as a datasource in the
 * dataspace. Technically speaking, the engine loads the
 * meta-information concerning med data from the file, gives this
 * informations to the GUI, and the GUI creates a tree view of these
 * data in the study object browser.
 */
MEDOP::DatasourceHandler * DatasourceController::addDatasource(const char * filename) {

  MEDOP::DatasourceHandler * datasourceHandler =
    MEDOPFactoryClient::getDataManager()->addDatasource(filename);

  // We need a studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Create a datasource SObject as a father of the module root
  SALOMEDS::SComponent_var root = _studyEditor->findRoot(QCHARSTAR(_salomeModule->moduleName()));
  SALOMEDS::SObject_var soDatasource = _studyEditor->newObject(root);
  _studyEditor->setName(soDatasource,datasourceHandler->name);
  _studyEditor->setIcon(soDatasource,"datasource.png");
  _studyEditor->setParameterInt(soDatasource,OBJECT_ID,datasourceHandler->id);
  

  // We can add the meshes as children of the datasource
  MEDOP::MeshHandlerList * meshHandlerList = 
    MEDOPFactoryClient::getDataManager()->getMeshList(datasourceHandler->id);
  
  for(CORBA::ULong iMesh=0; iMesh<meshHandlerList->length(); iMesh++) {
    MEDOP::MeshHandler meshHandler = (*meshHandlerList)[iMesh];
    SALOMEDS::SObject_var soMesh = _studyEditor->newObject(soDatasource);
    _studyEditor->setName(soMesh,meshHandler.name);
    _studyEditor->setIcon(soMesh,"datasource_mesh.png");
    _studyEditor->setParameterInt(soMesh,OBJECT_ID,meshHandler.id);
    _studyEditor->setParameterBool(soMesh,OBJECT_IS_IN_WORKSPACE,false);
    

    // We add the field timeseries defined on this mesh, as children
    // of the mesh SObject
    MEDOP::FieldseriesHandlerList * fieldseriesHandlerList =
      MEDOPFactoryClient::getDataManager()->getFieldseriesListOnMesh(meshHandler.id);
    
    for(CORBA::ULong iFieldseries=0; iFieldseries<fieldseriesHandlerList->length(); iFieldseries++) {
      MEDOP::FieldseriesHandler fieldseriesHandler = (*fieldseriesHandlerList)[iFieldseries];
      SALOMEDS::SObject_var soFieldseries = _studyEditor->newObject(soMesh);

      std::string label(fieldseriesHandler.name);
      label +=" ("+std::string(XmedDataObject::mapTypeOfFieldLabel[fieldseriesHandler.type])+")";
      _studyEditor->setName(soFieldseries,label.c_str());

      _studyEditor->setIcon(soFieldseries,"datasource_field.png");
      _studyEditor->setParameterInt(soFieldseries,OBJECT_ID,fieldseriesHandler.id);
      _studyEditor->setParameterBool(soFieldseries,OBJECT_IS_IN_WORKSPACE,false);
    }
  }

  return datasourceHandler;
}


void DatasourceController::OnAddDatasource()
{
  // Dialog to get the filename where the input data are read from
  QStringList filter;
  filter.append(QObject::tr("MED files (*.med)"));

  QString filename = SUIT_FileDlg::getFileName(_salomeModule->getApp()->desktop(),
                                               "",
                                               filter,
                                               QObject::tr("Import MED fields"),
                                               true);

  if ( filename.isEmpty() ) return;

  this->addDatasource(QCHARSTAR(filename));
  _salomeModule->updateObjBrowser(true);
}

#include "DlgImageToMed.hxx"
#include <stdio.h>
#include <stdlib.h>
void DatasourceController::OnAddImagesource()
{

  DlgImageToMed dialog;
  dialog.setAutoLoaded(true);
  int choice = dialog.exec();
  if ( choice == QDialog::Rejected ) {
    // The user decides to cancel the operation
    return;
  }

  QString imageFilename = dialog.getImageFilepath();
  QString medFilename   = dialog.getMedFilepath();
  bool autoLoad         = dialog.isAutoLoaded();
  
  std::string ROOT_DIR(getenv("MED_ROOT_DIR"));
  std::string command(ROOT_DIR+"/bin/salome/med/image2med.py");
  command += " -i "+QS2S(imageFilename);
  command += " -m "+QS2S(medFilename);
  int error = system(command.c_str());
  if ( error != 0 ) {
    QMessageBox::critical(_salomeModule->getApp()->desktop(),
           tr("Operation failed"),
           tr("The creation of med data from the image file failed"));
    return;
  }

  if ( autoLoad ) {
    this->addDatasource(QCHARSTAR(medFilename));
    _salomeModule->updateObjBrowser(true);
  }

}

void DatasourceController::OnExpandField()
{
  // We need a studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList * listOfSObject = _studyEditor->getSelectedObjects();
  for (int i=0; i<listOfSObject->size(); i++) {  
    SALOMEDS::SObject_var soFieldseries = listOfSObject->at(i);

    // First retrieve the fieldseries id associated to this study object 
    long fieldseriesId = _studyEditor->getParameterInt(soFieldseries,OBJECT_ID);
    STDLOG("Expand the field timeseries "<<fieldseriesId);

    // Then retrieve the list of fields in this timeseries
    MEDOP::FieldHandlerList * fieldHandlerList =
      MEDOPFactoryClient::getDataManager()->getFieldListInFieldseries(fieldseriesId);

    // Finally, create an entry for each of the field
    for(CORBA::ULong iField=0; iField<fieldHandlerList->length(); iField++) {
      MEDOP::FieldHandler fieldHandler = (*fieldHandlerList)[iField];
      SALOMEDS::SObject_var soField = _studyEditor->newObject(soFieldseries);
      std::string label("it="); label += ToString(fieldHandler.iteration);
      _studyEditor->setName(soField,label.c_str());
      _studyEditor->setParameterInt(soField, OBJECT_ID, fieldHandler.id);
      _studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,false);
    }
  }
  _salomeModule->updateObjBrowser(true);
}

void DatasourceController::OnVisualize() {
  STDLOG("OnVisualize: To Be Implemented");

  // We need a _studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList * listOfSObject = _studyEditor->getSelectedObjects();

  // For each object, emit a signal to the workspace to request a
  // visualisation using the tui command (so that the user can see how
  // to make a view of an object from the tui console).
  for (int i=0; i<listOfSObject->size(); i++) {
    SALOMEDS::SObject_var soField = listOfSObject->at(i);
    int fieldId = _studyEditor->getParameterInt(soField,OBJECT_ID);
    MEDOP::FieldHandler * fieldHandler = MEDOPFactoryClient::getDataManager()->getFieldHandler(fieldId);

    DatasourceEvent * event = new DatasourceEvent();
    event->eventtype = DatasourceEvent::EVENT_VIEW_OBJECT;
    XmedDataObject * dataObject = new XmedDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    event->objectdata  = dataObject;
    emit datasourceSignal(event);    
  }

}

void DatasourceController::OnUseInWorkspace() {
  // We need a studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList * listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() == 1 ) {
    // In this case we ask the name of the variable for the python
    // console

    // >>>
    // _GBO_ Note that it works only for a single field but the
    // XmedDataObject will be improved to deal with mesh, timeseries
    // and single field in a futur version. We suppose here that a
    // single field has been selected. 
    // <<<

    SALOMEDS::SObject_var soField = listOfSObject->at(0);

    bool isInWorkspace = _studyEditor->getParameterBool(soField,OBJECT_IS_IN_WORKSPACE);
    if ( isInWorkspace ) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("This field is already defined in the workspace"));
      return;
    }

    int fieldId = _studyEditor->getParameterInt(soField,OBJECT_ID);
    MEDOP::FieldHandler * fieldHandler =
      MEDOPFactoryClient::getDataManager()->getFieldHandler(fieldId);

    QString alias(fieldHandler->fieldname);
    DlgAlias dialog;
    dialog.setAlias(alias);
    int choice = dialog.exec();
    if ( choice == QDialog::Rejected ) {
      // The user decides to cancel the operation
      return;
    }
    alias = dialog.getAlias();

    DatasourceEvent * event = new DatasourceEvent();
    event->eventtype = DatasourceEvent::EVENT_USE_OBJECT;
    XmedDataObject * dataObject = new XmedDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    event->objectdata  = dataObject;
    event->objectalias = alias;
    emit datasourceSignal(event);
    // Tag the item to prevent double import
    _studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
    // Tag the field as persistent on the server. It means that a
    // saving of the workspace will save at least this field (maybe it
    // should be an option?)
    MEDOPFactoryClient::getDataManager()->markAsPersistent(fieldId, true);
  }
  else {
    // In this case, we don't ask the user to specify an alias for
    // each item, we just import the whole set of items.
    for (int i=0; i<listOfSObject->size(); i++) {
      SALOMEDS::SObject_var soField = listOfSObject->at(i);

      bool isInWorkspace = _studyEditor->getParameterBool(soField,OBJECT_IS_IN_WORKSPACE);
      if ( !isInWorkspace ) {
  int fieldId = _studyEditor->getParameterInt(soField,OBJECT_ID);
  MEDOP::FieldHandler * fieldHandler =
    MEDOPFactoryClient::getDataManager()->getFieldHandler(fieldId);
  DatasourceEvent * event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_IMPORT_OBJECT;
  XmedDataObject * dataObject = new XmedDataObject();
  dataObject->setFieldHandler(*fieldHandler);
  event->objectdata  = dataObject;
  emit datasourceSignal(event);
  // Note that this signal is processed by the WorkspaceController

  // Tag the item to prevent double import
  _studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
  // Tag the field as persistent on the server. It means that a
  // saving of the workspace will save at least this field (maybe it
  // should be an option?)
  MEDOPFactoryClient::getDataManager()->markAsPersistent(fieldId, true);
      }
      else {
  STDLOG("The field "<<_studyEditor->getName(soField)<<
         " is already defined in the workspace");
      }
    }
  }
}


void DatasourceController::OnChangeUnderlyingMesh() {
  // We need a studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Get the selected objects in the study (SObject). In cas of a
  // multiple selection, we consider only the first item. At least one
  // item must be selected.
  SALOME_StudyEditor::SObjectList * listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soField = listOfSObject->at(0);
    int fieldId = _studyEditor->getParameterInt(soField,OBJECT_ID);    
    // _GBO_ : the dialog should not be modal, so that we can choose a
    // mesh in the browser. Then we have to emit a signal from the
    // dialog.accept, connected to a slot of the DatasourceControler
    _dlgChangeUnderlyingMesh->setFieldId(fieldId);
    Qt::WindowFlags flags = _dlgChangeUnderlyingMesh->windowFlags();
    _dlgChangeUnderlyingMesh->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    _dlgChangeUnderlyingMesh->open();
  }
}

void DatasourceController::OnChangeUnderlyingMeshInputValidated() {
  int meshId = _dlgChangeUnderlyingMesh->getMeshId();
  STDLOG("meshId = " << ToString(meshId));
  int fieldId = _dlgChangeUnderlyingMesh->getFieldId();
  MEDOP::FieldHandler * fieldHandler =
    MEDOPFactoryClient::getDataManager()->getFieldHandler(fieldId);
  
  // We don't modify the original field but create first a duplicate
  MEDOP::FieldHandler * duplicate = MEDOPFactoryClient::getCalculator()->dup(*fieldHandler);
  MEDOPFactoryClient::getDataManager()->changeUnderlyingMesh(duplicate->id, meshId);

  // Request once more the duplicate to update the meta-data on this
  // client side
  duplicate = MEDOPFactoryClient::getDataManager()->getFieldHandler(duplicate->id);

  // >>>
  // WARN: the following is a temporary code for test purpose
  // Automatically add in ws
  DatasourceEvent * event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_IMPORT_OBJECT;
  XmedDataObject * dataObject = new XmedDataObject();
  dataObject->setFieldHandler(*duplicate);
  event->objectdata  = dataObject;
  emit datasourceSignal(event);
  // Note that this signal is processed by the WorkspaceController
  
  // Tag the item to prevent double import
  //_studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
  
  
}
