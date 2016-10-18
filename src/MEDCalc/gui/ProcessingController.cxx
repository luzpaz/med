// Copyright (C) 2016  CEA/DEN, EDF R&D
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

#include "ProcessingController.hxx"
#include <MEDCalcConstants.hxx>

#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include "MEDFactoryClient.hxx"
#include "MEDModule.hxx"
#include "Basics_Utils.hxx"

#include <SUIT_Desktop.h>
#include <QMessageBox>

ProcessingController::ProcessingController(MEDModule* salomeModule)
{
  STDLOG("Creating a ProcessingController");
  _salomeModule = salomeModule;
  _studyEditor = _salomeModule->getStudyEditor();

  _dlgChangeUnderlyingMesh = new DlgChangeUnderlyingMesh(_studyEditor);
  connect(_dlgChangeUnderlyingMesh,SIGNAL(inputValidated()),
          this, SLOT(OnChangeUnderlyingMeshInputValidated()));

  _dlgInterpolateField = new DlgInterpolateField(_studyEditor);
  connect(_dlgInterpolateField,SIGNAL(inputValidated()),
          this, SLOT(OnInterpolateFieldInputValidated()));
}

ProcessingController::~ProcessingController()
{
  STDLOG("Deleting the ProcessingController");
}

void
ProcessingController::createActions()
{
  STDLOG("Creating ProcessingController actions");

  int processingToolbarId = _salomeModule->createTool("Processing", "ProcessingToolbar");
  int processingMenuId = _salomeModule->createMenu(tr("MENU_PROCESSING"), -1, -1, 12);

  // Change underlying mesh (note that this action creates a new field in
  // the workspace that corresponds to a copy of the selected field
  // modified by the change of the underlying mesh.
  QString label   = tr("LAB_PROCESSING_CHANGE_MESH");
  QString icon = tr("ICO_PROCESSING_CHANGE_MESH");
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnChangeUnderlyingMesh()),icon,label);
  _salomeModule->createTool(actionId, processingToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, processingMenuId);

  label   = tr("LAB_PROCESSING_INTERPOLATE_FIELD");
  icon = tr("ICO_PROCESSING_INTERPOLATE_FIELD");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnInterpolateField()),icon,label);
  _salomeModule->createTool(actionId, processingToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, processingMenuId);
}

void
ProcessingController::OnChangeUnderlyingMesh()
{
  // Get the selected objects in the study (SObject). In cas of a
  // multiple selection, we consider only the first item. At least one
  // item must be selected.
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soField = listOfSObject->at(0);
    std::string name(_studyEditor->getName(soField));
    if (soField->_is_nil() || name == "MEDCalc")
      return;
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeParameter_var aParam;
    if ( soField->FindAttribute(anAttr,"AttributeParameter") ) {
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      if (! aParam->IsSet(FIELD_SERIES_ID, PT_INTEGER))
        return;
    }
    int fieldId = aParam->GetInt(FIELD_SERIES_ID);
    // _GBO_ : the dialog should not be modal, so that we can choose a
    // mesh in the browser. Then we have to emit a signal from the
    // dialog.accept, connected to a slot of the DatasourceControler
    _dlgChangeUnderlyingMesh->setFieldId(fieldId);
    Qt::WindowFlags flags = _dlgChangeUnderlyingMesh->windowFlags();
    _dlgChangeUnderlyingMesh->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    _dlgChangeUnderlyingMesh->open();
  }
}

void
ProcessingController::OnChangeUnderlyingMeshInputValidated()
{
  int meshId = _dlgChangeUnderlyingMesh->getMeshId();
  STDLOG("meshId = " << ToString(meshId));
  int fieldId = _dlgChangeUnderlyingMesh->getFieldId();
  MEDCALC::FieldHandler* fieldHandler =
    MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);

  // We don't modify the original field but create first a duplicate
  MEDCALC::FieldHandler* duplicate = MEDFactoryClient::getCalculator()->dup(*fieldHandler);
  MEDFactoryClient::getDataManager()->changeUnderlyingMesh(duplicate->id, meshId);

  // Request once more the duplicate to update the meta-data on this
  // client side
  duplicate = MEDFactoryClient::getDataManager()->getFieldHandler(duplicate->id);

  // >>>
  // WARN: the following is a temporary code for test purpose
  // Automatically add in ws
  ProcessingEvent* event = new ProcessingEvent();
  event->eventtype = ProcessingEvent::EVENT_IMPORT_OBJECT;
  XmedDataObject* dataObject = new XmedDataObject();
  dataObject->setFieldHandler(*duplicate);
  event->objectdata = dataObject;
  emit processingSignal(event);
  // Note that this signal is processed by the WorkspaceController

  // Tag the item to prevent double import
  //_studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
}

void
ProcessingController::OnInterpolateField()
{
  // Get the selected objects in the study (SObject). In case of a
  // multiple selection, we consider only the first item. At least one
  // item must be selected.
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soField = listOfSObject->at(0);
    std::string name(_studyEditor->getName(soField));
    if (soField->_is_nil() || name == "MEDCalc")
      return;
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeParameter_var aParam;
    if ( soField->FindAttribute(anAttr,"AttributeParameter") ) {
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      if (! aParam->IsSet(FIELD_SERIES_ID, PT_INTEGER))
        return;
    }
    int fieldId = aParam->GetInt(FIELD_SERIES_ID);
    // _GBO_ : the dialog should not be modal, so that we can choose a
    // mesh in the browser. Then we have to emit a signal from the
    // dialog.accept, connected to a slot of the DatasourceControler
    _dlgInterpolateField->setFieldId(fieldId);
    Qt::WindowFlags flags = _dlgInterpolateField->windowFlags();
    _dlgInterpolateField->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    _dlgInterpolateField->open();
  }
}

void
ProcessingController::OnInterpolateFieldInputValidated()
{
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
  ProcessingEvent* event = new ProcessingEvent();
  event->eventtype = ProcessingEvent::EVENT_IMPORT_OBJECT;
  XmedDataObject* dataObject = new XmedDataObject();
  dataObject->setFieldHandler(*result);
  event->objectdata = dataObject;
  emit processingSignal(event);
  // Note that this signal is processed by the WorkspaceController

  // // Tag the item to prevent double import
  // //_studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
}
