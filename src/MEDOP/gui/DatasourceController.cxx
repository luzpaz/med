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

  _dlgInterpolateField = new DlgInterpolateField(_studyEditor);
  connect(_dlgInterpolateField,SIGNAL(inputValidated()),
          this, SLOT(OnInterpolateFieldInputValidated()));

}

DatasourceController::~DatasourceController() {
  STDLOG("Deleting the DatasourceController");
  delete _studyEditor;
}

void DatasourceController::createActions() {
  //
  // Main actions (toolbar and menubar)
  //
  QString label   = tr("LAB_ADD_DATA_SOURCE");
  QString tooltip = tr("TIP_ADD_DATA_SOURCE");
  QString icon    = tr("ICO_DATASOURCE_ADD");
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnAddDatasource()),icon,tooltip);
  _salomeModule->addActionInToolbar(actionId);

  // This action has to be placed in the general file menu with the label "Import MED file"
  int menuId = _salomeModule->createMenu( tr( "MEN_FILE" ), -1,  1 );
  _salomeModule->addActionInMenubar(actionId, menuId);

  label   = tr("LAB_ADD_IMAGE_SOURCE");
  tooltip = tr("TIP_ADD_IMAGE_SOURCE");
  icon    = tr("ICO_IMAGE_ADD");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnAddImagesource()),icon,tooltip);
  _salomeModule->addActionInToolbar(actionId);

  //
  // Actions for popup menu only
  //
  // Expand field timeseries
  label = tr("LAB_EXPAND_FIELD");
  icon  = tr("ICO_DATASOURCE_EXPAND_FIELD");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnExpandField()),icon);
  _salomeModule->addActionInPopupMenu(actionId);

  // Create a view submenu with usual visualization functions
  label = tr("LAB_VISUALIZE_SCALARMAP");
  icon  = tr("ICO_DATASOURCE_VIEW");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnVisualizeScalarMap()),icon);
  _salomeModule->addActionInPopupMenu(actionId, tr("LAB_VISUALIZE"));

  // Use in workspace
  label = tr("LAB_USE_IN_WORKSPACE");
  icon  = tr("ICO_DATASOURCE_USE");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnUseInWorkspace()),icon);
  _salomeModule->addActionInPopupMenu(actionId);

  // Change underlying mesh (note that this action creates a new field in
  // the workspace that corresponds to a copy of the selected field
  // modified by the change of the underlying mesh.
  label = tr("LAB_CHANGE_MESH");
  icon  = tr("ICO_DATASOURCE_CHANGE_MESH");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnChangeUnderlyingMesh()),icon);
  _salomeModule->addActionInPopupMenu(actionId);

  label = tr("LAB_INTERPOLATE_FIELD");
  icon  = tr("ICO_DATASOURCE_INTERPOLATE_FIELD");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnInterpolateField()),icon);
  _salomeModule->addActionInPopupMenu(actionId);
}

/**
 * This function adds the specified MED file as a datasource in the
 * dataspace. Technically speaking, the engine loads the
 * meta-information concerning med data from the file, gives this
 * informations to the GUI, and the GUI creates a tree view of these
 * data in the study object browser.
 */
void
DatasourceController::addDatasource(const char* filename)
{
  DatasourceEvent* event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_ADD_DATASOURCE;
  event->objectalias = filename;
  emit datasourceSignal(event);
}
// call to this function is trigerred by workspace (itself from python console)
void
DatasourceController::updateTreeViewWithNewDatasource(const MEDOP::DatasourceHandler* datasourceHandler)
{
  if (!datasourceHandler) {
    return;
  }

  // We need a studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Create a datasource SObject as a father of the module root
  SALOMEDS::SComponent_var root = _studyEditor->findRoot(QCHARSTAR(_salomeModule->moduleName()));
  SALOMEDS::SObject_var soDatasource = _studyEditor->newObject(root);
  _studyEditor->setName(soDatasource,datasourceHandler->name);
  _studyEditor->setIcon(soDatasource,tr("ICO_DATASOURCE").toStdString().c_str());
  _studyEditor->setParameterInt(soDatasource,OBJECT_ID,datasourceHandler->id);


  // We can add the meshes as children of the datasource
  MEDOP::MeshHandlerList * meshHandlerList =
    MEDOPFactoryClient::getDataManager()->getMeshList(datasourceHandler->id);

  for(CORBA::ULong iMesh=0; iMesh<meshHandlerList->length(); iMesh++) {
    MEDOP::MeshHandler meshHandler = (*meshHandlerList)[iMesh];
    SALOMEDS::SObject_var soMesh = _studyEditor->newObject(soDatasource);
    _studyEditor->setName(soMesh,meshHandler.name);
    _studyEditor->setIcon(soMesh,tr("ICO_DATASOURCE_MESH").toStdString().c_str());
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

      _studyEditor->setIcon(soFieldseries,tr("ICO_DATASOURCE_FIELD").toStdString().c_str());
      _studyEditor->setParameterInt(soFieldseries,OBJECT_ID,fieldseriesHandler.id);
      _studyEditor->setParameterBool(soFieldseries,OBJECT_IS_IN_WORKSPACE,false);
    }
  }
}


void DatasourceController::OnAddDatasource()
{
  // Dialog to get the filename where the input data are read from
  QStringList filter;
  filter.append(tr("FILE_FILTER_MED"));

  QString anInitialPath = "";
  if ( SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    anInitialPath = QDir::currentPath();

  /*
  QString filename = SUIT_FileDlg::getFileName(_salomeModule->getApp()->desktop(),
                                               "",
                                               filter,
                                               tr("IMPORT_MED_FIELDS"),
                                               true);
  */

  QStringList filenames = SUIT_FileDlg::getOpenFileNames( _salomeModule->getApp()->desktop(),
                                                          anInitialPath,
                                                          filter,
                                                          tr("IMPORT_MED_FIELDS") );

  if ( filenames.count() <= 0 ) return;
  for ( QStringList::ConstIterator itFile = filenames.begin();
        itFile != filenames.end(); ++itFile ) {
    QString filename = *itFile;
    this->addDatasource(QCHARSTAR(filename));
    _salomeModule->updateObjBrowser(true);
  }
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

    // If fieldseriesId equals -1, then it means that it is not a
    // fieldseries managed by the MED module, and we stop this
    // function process.
    if ( fieldseriesId < 0 )
      continue;
    // _GBO_ A better correction should be to no display the
    // contextual menu if the selected object is not conform

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

void DatasourceController::visualize(DatasourceEvent::EventType eventType) {
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
    // If fieldId equals -1, then it means that it is not a field
    // managed by the MED module, and we stop this function process.
    if ( fieldId < 0 )
      continue;

    MEDOP::FieldHandler * fieldHandler = MEDOPFactoryClient::getDataManager()->getFieldHandler(fieldId);
    if (! fieldHandler) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("No field is defined"));
      return;
    }

    DatasourceEvent * event = new DatasourceEvent();
    event->eventtype = eventType;
    XmedDataObject * dataObject = new XmedDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    event->objectdata  = dataObject;
    emit datasourceSignal(event);
  }
}

void DatasourceController::OnVisualizeScalarMap() {
  this->visualize(DatasourceEvent::EVENT_VIEW_OBJECT_SCALAR_MAP);
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

    // If fieldId equals -1, then it means that it is not a field
    // managed by the MED module, and we stop this function process.
    if ( fieldId < 0 ) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("This element is not a field object"));
      return;
    }

    MEDOP::FieldHandler * fieldHandler =
      MEDOPFactoryClient::getDataManager()->getFieldHandler(fieldId);

    if (! fieldHandler) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("No field is defined"));
      return;
    }

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
    //    _studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
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
        //        _studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
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

void DatasourceController::OnInterpolateField() {
  // We need a studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Get the selected objects in the study (SObject). In case of a
  // multiple selection, we consider only the first item. At least one
  // item must be selected.
  SALOME_StudyEditor::SObjectList * listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soField = listOfSObject->at(0);
    int fieldId = _studyEditor->getParameterInt(soField,OBJECT_ID);
    // _GBO_ : the dialog should not be modal, so that we can choose a
    // mesh in the browser. Then we have to emit a signal from the
    // dialog.accept, connected to a slot of the DatasourceControler
    _dlgInterpolateField->setFieldId(fieldId);
    Qt::WindowFlags flags = _dlgInterpolateField->windowFlags();
    _dlgInterpolateField->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    _dlgInterpolateField->open();
  }
}

void DatasourceController::OnInterpolateFieldInputValidated() {
  MEDOP::InterpolationParameters params;
  params.precision = _dlgInterpolateField->getPrecision();
  STDLOG("precision = " << params.precision);
  params.defaultValue = _dlgInterpolateField->getDefaultValue();
  STDLOG("defaultValue = " << params.defaultValue);
  params.reverse = _dlgInterpolateField->getReverse();
  STDLOG("reverse = " << params.reverse);
  params.intersectionType = _dlgInterpolateField->getIntersectionType().c_str();
  STDLOG("intersectionType = " << params.intersectionType);
  params.method = _dlgInterpolateField->getMethod().c_str();
  STDLOG("method = " << params.method);
  params.nature = _dlgInterpolateField->getFieldNature().c_str();
  STDLOG("nature = " << params.nature);

  int meshId = _dlgInterpolateField->getMeshId();
  STDLOG("meshId = " << ToString(meshId));
  int fieldId = _dlgInterpolateField->getFieldId();
  MEDOP::FieldHandler* fieldHandler = MEDOPFactoryClient::getDataManager()->getFieldHandler(fieldId);

  // We don't modify the original field but create first a duplicate
  // MEDOP::FieldHandler* duplicate = MEDOPFactoryClient::getCalculator()->dup(*fieldHandler);
  //MEDOPFactoryClient::getDataManager()->changeUnderlyingMesh(duplicate->id, meshId);
  MEDOP::FieldHandler* result = NULL;
  try {
    result = MEDOPFactoryClient::getDataManager()->interpolateField(fieldId, meshId, params);
  }
  catch(...) {
    STDLOG("Unable to process field interpolation; please check interpolation parameters");
    QMessageBox::critical(_salomeModule->getApp()->desktop(),
                          tr("Operation failed"),
                          tr("Unable to process field interpolation; please check interpolation parameters"));
    return;
  }

  // Request once more the duplicate to update the meta-data on this
  // client side
  // duplicate = MEDOPFactoryClient::getDataManager()->getFieldHandler(duplicate->id);

  // >>>
  // WARN: the following is a temporary code for test purpose
  // Automatically add in ws
  DatasourceEvent * event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_IMPORT_OBJECT;
  XmedDataObject * dataObject = new XmedDataObject();
  dataObject->setFieldHandler(*result);
  event->objectdata = dataObject;
  emit datasourceSignal(event);
  // Note that this signal is processed by the WorkspaceController

  // // Tag the item to prevent double import
  // //_studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
}

void
DatasourceController::processWorkspaceEvent(const MEDOP::MedEvent* event)
{
  if ( event->type == MEDOP::EVENT_ADD_DATASOURCE ) {
    MEDOP::DatasourceHandler* datasourceHandler = MEDOPFactoryClient::getDataManager()->getDatasourceHandler(event->filename);
    this->updateTreeViewWithNewDatasource(datasourceHandler);
    _salomeModule->updateObjBrowser(true);
  }
}
