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

#include "DatasourceController.hxx"
#include "DatasourceConstants.hxx"

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_DataObject.h>

#include <SALOME_ListIO.hxx>
#include <LightApp_SelectionMgr.h>

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>

#include "MEDFactoryClient.hxx"
#include "MEDModule.hxx"
#include "QtHelper.hxx"

#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)
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
//DatasourceController::DatasourceController(StandardApp_Module * salomeModule)
DatasourceController::DatasourceController(MEDModule * salomeModule)
{
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
  //QWidget* dsk = _salomeModule->getApp()->desktop();
  //SUIT_ResourceMgr* resMgr = _salomeModule->getApp()->resourceMgr();
  int toolbarId = _salomeModule->createTool("Datasource", "DatasourceToolbar");

  //
  // Main actions (toolbar and menubar)
  //
  QString label   = tr("LAB_ADD_DATA_SOURCE");
  QString tooltip = tr("TIP_ADD_DATA_SOURCE");
  QString icon    = tr("ICO_DATASOURCE_ADD");
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnAddDatasource()),icon,tooltip);
  //_salomeModule->addActionInToolbar(actionId);
  _salomeModule->createTool(actionId, toolbarId);

  // This action has to be placed in the general file menu with the label "Import MED file"
  int menuId = _salomeModule->createMenu( tr( "MEN_FILE" ), -1,  1 );
  //_salomeModule->addActionInMenubar(actionId, menuId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, menuId, 10);

  label   = tr("LAB_ADD_IMAGE_SOURCE");
  tooltip = tr("TIP_ADD_IMAGE_SOURCE");
  icon    = tr("ICO_IMAGE_ADD");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnAddImagesource()),icon,tooltip);
  // _salomeModule->addActionInToolbar(actionId);
  _salomeModule->createTool(actionId, toolbarId);

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
// This function emits a signal that will be caught by workspace to delegate command (datasource creation) to python console.
void
DatasourceController::addDatasource(const char* filename)
{
  DatasourceEvent* event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_ADD_DATASOURCE;
  event->objectalias = filename;
  emit datasourceSignal(event);
}
// After above data source creation, python console emits a signal, forwarded by workspace, to update the GUI
void
DatasourceController::updateTreeViewWithNewDatasource(const MEDCALC::DatasourceHandler* datasourceHandler)
{
  if (!datasourceHandler) {
    return;
  }

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(_salomeModule->application()->activeStudy());
  _PTR(Study) studyDS = study->studyDS();

  _salomeModule->engine()->addDatasourceToStudy(_CAST(Study, studyDS)->GetStudy(), *datasourceHandler);

  // update Object browser
  _salomeModule->getApp()->updateObjectBrowser(true);
}

void
DatasourceController::updateTreeViewWithNewPresentation(long fieldId, long presentationId)
{
  if (presentationId < 0) {
    std::cerr << "Unknown presentation\n";
    return;
  }

  std::string name = MEDFactoryClient::getPresentationManager()->getPresentationProperty(presentationId, "name");
  name = tr(name.c_str()).toStdString();
  std::string label = tr("ICO_MED_PRESENTATION").toStdString();

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(_salomeModule->application()->activeStudy());
  _PTR(Study) studyDS = study->studyDS();

  _salomeModule->engine()->registerPresentation(_CAST(Study, studyDS)->GetStudy(), fieldId, name.c_str(), label.c_str());

  // update Object browser
  _salomeModule->getApp()->updateObjectBrowser(true);
}

void DatasourceController::OnAddDatasource()
{
  // Dialog to get the filename where the input data are read from
  QStringList filter;
  filter.append(tr("FILE_FILTER_MED"));

  QString anInitialPath = "";
  if ( SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    anInitialPath = QDir::currentPath();

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
//#include <stdio.h>
//#include <stdlib.h>
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
  /*
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
  */

  DatasourceEvent* event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_ADD_IMAGE_AS_DATASOURCE;
  event->objectalias = imageFilename;
  emit datasourceSignal(event);
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
    MEDCALC::FieldHandlerList * fieldHandlerList =
      MEDFactoryClient::getDataManager()->getFieldListInFieldseries(fieldseriesId);

    // Finally, create an entry for each of the field
    for(CORBA::ULong iField=0; iField<fieldHandlerList->length(); iField++) {
      MEDCALC::FieldHandler fieldHandler = (*fieldHandlerList)[iField];
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

    MEDCALC::FieldHandler * fieldHandler = MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);
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

    MEDCALC::FieldHandler * fieldHandler =
      MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);

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
    MEDFactoryClient::getDataManager()->markAsPersistent(fieldId, true);
  }
  else {
    // In this case, we don't ask the user to specify an alias for
    // each item, we just import the whole set of items.
    for (int i=0; i<listOfSObject->size(); i++) {
      SALOMEDS::SObject_var soField = listOfSObject->at(i);

      bool isInWorkspace = _studyEditor->getParameterBool(soField,OBJECT_IS_IN_WORKSPACE);
      if ( !isInWorkspace ) {
        int fieldId = _studyEditor->getParameterInt(soField,OBJECT_ID);
        MEDCALC::FieldHandler * fieldHandler =
          MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);
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
        MEDFactoryClient::getDataManager()->markAsPersistent(fieldId, true);
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
  MEDCALC::FieldHandler * fieldHandler =
    MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);

  // We don't modify the original field but create first a duplicate
  MEDCALC::FieldHandler * duplicate = MEDFactoryClient::getCalculator()->dup(*fieldHandler);
  MEDFactoryClient::getDataManager()->changeUnderlyingMesh(duplicate->id, meshId);

  // Request once more the duplicate to update the meta-data on this
  // client side
  duplicate = MEDFactoryClient::getDataManager()->getFieldHandler(duplicate->id);

  // >>>
  // WARN: the following is a temporary code for test purpose
  // Automatically add in ws
  DatasourceEvent * event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_IMPORT_OBJECT;
  XmedDataObject * dataObject = new XmedDataObject();
  dataObject->setFieldHandler(*duplicate);
  event->objectdata = dataObject;
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
  MEDCALC::InterpolationParameters params;
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
  MEDCALC::FieldHandler* fieldHandler = MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);

  // We don't modify the original field but create first a duplicate
  // MEDCALC::FieldHandler* duplicate = MEDFactoryClient::getCalculator()->dup(*fieldHandler);
  //MEDFactoryClient::getDataManager()->changeUnderlyingMesh(duplicate->id, meshId);
  MEDCALC::FieldHandler* result = NULL;
  try {
    result = MEDFactoryClient::getDataManager()->interpolateField(fieldId, meshId, params);
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
  // duplicate = MEDFactoryClient::getDataManager()->getFieldHandler(duplicate->id);

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
DatasourceController::processWorkspaceEvent(const MEDCALC::MedEvent* event)
{
  if ( event->type == MEDCALC::EVENT_ADD_DATASOURCE ) {
    MEDCALC::DatasourceHandler* datasourceHandler = MEDFactoryClient::getDataManager()->getDatasourceHandler(event->filename);
    this->updateTreeViewWithNewDatasource(datasourceHandler);
  }
  else if ( event->type == MEDCALC::EVENT_ADD_PRESENTATION ) {
    this->updateTreeViewWithNewPresentation(event->dataId, event->presentationId);
  }
}
