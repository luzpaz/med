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

#include "PresentationController.hxx"
#include <MEDCalcConstants.hxx>
#include "MEDModule.hxx"
#include "Basics_Utils.hxx"
#include "QtxActionGroup.h"
#include "QtxActionToolMgr.h"
#include "MEDFactoryClient.hxx"
#include "MEDPresentationManager_i.hxx"

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_DataObject.h>

#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <QMessageBox>
#include <sstream>

static const int OPTIONS_VIEW_MODE_ID = 943;
static const int OPTIONS_VIEW_MODE_REPLACE_ID = 944;
static const int OPTIONS_VIEW_MODE_OVERLAP_ID = 945;
static const int OPTIONS_VIEW_MODE_NEW_LAYOUT_ID = 946;
static const int OPTIONS_VIEW_MODE_SPLIT_VIEW_ID = 947;

PresentationController::PresentationController(MEDModule* salomeModule) :
    _salomeModule(salomeModule),
    _studyEditor(salomeModule->getStudyEditor())
{
  STDLOG("Creating a PresentationController");

  _widgetPresentationParameters = new WidgetPresentationParameters();

  QMainWindow* parent = salomeModule->getApp()->desktop();
  _dockWidget = new QDockWidget(parent);
  _dockWidget->setVisible(false);
  _dockWidget->setWindowTitle(tr("TITLE_PRESENTATION_PARAMETERS"));
  _dockWidget->setObjectName(tr("TITLE_PRESENTATION_PARAMETERS"));
  _dockWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
  _dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _dockWidget->setWidget(_widgetPresentationParameters);
  parent->addDockWidget(Qt::LeftDockWidgetArea, _dockWidget);
  //_dockWidget->show();
}

PresentationController::~PresentationController()
{
  STDLOG("Deleting the PresentationController");
}

std::string
PresentationController::_getIconName(const std::string& name)
{
  SUIT_ResourceMgr* mgr = SUIT_Session::session()->resourceMgr();
  if (!mgr)
    return name;

  // Read value from preferences and suffix name to select icon theme
  int theme = mgr->integerValue("MEDCalc", "icons");
  if (theme == 0) {
    return name + "_MODERN";
  } else if (theme == 1) {
    return name + "_CLASSIC";
  }
  return name + "_DEFAULT";
}

void
PresentationController::createActions()
{
  STDLOG("Creating PresentationController actions");

  // View Mode
  int viewModeToolbarId = _salomeModule->createTool("View Mode", "ViewModeToolbar");
  QtxActionGroup* ag = _salomeModule->createActionGroup(OPTIONS_VIEW_MODE_ID, true);
  ag->setText("View mode");
  ag->setUsesDropDown(true);
  QString label   = tr("LAB_VIEW_MODE_REPLACE");
  QString tooltip = tr("TIP_VIEW_MODE_REPLACE");
  QAction* a = _salomeModule->createAction(OPTIONS_VIEW_MODE_REPLACE_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  a->setChecked(true);
  ag->add(a);

  label   = tr("LAB_VIEW_MODE_OVERLAP");
  tooltip = tr("TIP_VIEW_MODE_OVERLAP");
  a = _salomeModule->createAction(OPTIONS_VIEW_MODE_OVERLAP_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  ag->add(a);

  label   = tr("LAB_VIEW_MODE_NEW_LAYOUT");
  tooltip = tr("TIP_VIEW_MODE_NEW_LAYOUT");
  a = _salomeModule->createAction(OPTIONS_VIEW_MODE_NEW_LAYOUT_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  ag->add(a);

  label   = tr("LAB_VIEW_MODE_SPLIT_VIEW");
  tooltip = tr("TIP_VIEW_MODE_SPLIT_VIEW");
  a = _salomeModule->createAction(OPTIONS_VIEW_MODE_SPLIT_VIEW_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  ag->add(a);

  _salomeModule->createTool(OPTIONS_VIEW_MODE_ID, viewModeToolbarId);

  // Presentations
  int presentationToolbarId = _salomeModule->createTool("Presentations", "PresentationToolbar");
  int presentationMenuId = _salomeModule->createMenu(tr("MENU_PRESENTATIONS"), -1, 1);

  label   = tr("LAB_PRESENTATION_SCALAR_MAP");
  tooltip = tr("TIP_PRESENTATION_SCALAR_MAP");
  QString icon = tr(_getIconName("ICO_PRESENTATION_SCALAR_MAP").c_str());
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnVisualizeScalarMap()),icon,tooltip);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_CONTOUR");
  tooltip = tr("TIP_PRESENTATION_CONTOUR");
  icon    = tr(_getIconName("ICO_PRESENTATION_CONTOUR").c_str());
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnVisualizeContour()),icon,tooltip);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_VECTOR_FIELD");
  tooltip = tr("TIP_PRESENTATION_VECTOR_FIELD");
  icon    = tr(_getIconName("ICO_PRESENTATION_VECTOR_FIELD").c_str());
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnVisualizeVectorField()),icon,tooltip);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_SLICES");
  tooltip = tr("TIP_PRESENTATION_SLICES");
  icon    = tr(_getIconName("ICO_PRESENTATION_SLICES").c_str());
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnVisualizeSlices()),icon,tooltip);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_DEFLECTION_SHAPE");
  tooltip = tr("TIP_PRESENTATION_DEFLECTION_SHAPE");
  icon    = tr(_getIconName("ICO_PRESENTATION_DEFLECTION_SHAPE").c_str());
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnVisualizeDeflectionShape()),icon,tooltip);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_POINT_SPRITE");
  tooltip = tr("TIP_PRESENTATION_POINT_SPRITE");
  icon    = tr(_getIconName("ICO_PRESENTATION_POINT_SPRITE").c_str());
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnVisualizePointSprite()),icon,tooltip);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_DELETE_PRESENTATION");
  tooltip = tr("TIP_DELETE_PRESENTATION");
  icon    = tr(_getIconName("ICO_DELETE_PRESENTATION").c_str());
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnDeletePresentation()),icon,tooltip);
//  _salomeModule->createTool(actionId, presentationToolbarId);
//  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

}

MEDCALC::MEDPresentationViewMode
PresentationController::getSelectedViewMode()
{
  if (_salomeModule->action(OPTIONS_VIEW_MODE_REPLACE_ID)->isChecked()) {
    return MEDCALC::VIEW_MODE_REPLACE;
  }
  else if (_salomeModule->action(OPTIONS_VIEW_MODE_OVERLAP_ID)->isChecked()) {
    return MEDCALC::VIEW_MODE_OVERLAP;
  }
  else if (_salomeModule->action(OPTIONS_VIEW_MODE_NEW_LAYOUT_ID)->isChecked()) {
    return MEDCALC::VIEW_MODE_NEW_LAYOUT;
  }
  else if (_salomeModule->action(OPTIONS_VIEW_MODE_SPLIT_VIEW_ID)->isChecked()) {
    return MEDCALC::VIEW_MODE_SPLIT_VIEW;
  }
  // Should not happen
  STDLOG("Strange!! No matching view mode found - returning VIEW_MODE_REPLACE.");
  return MEDCALC::VIEW_MODE_REPLACE;
}

MEDCALC::MEDPresentationColorMap
PresentationController::getSelectedColorMap()
{
  return _widgetPresentationParameters->getColorMap();
}

void
PresentationController::visualize(PresentationEvent::EventType eventType)
{
  // We need a _studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();

  // For each object, emit a signal to the workspace to request a
  // visualisation using the tui command (so that the user can see how
  // to make a view of an object from the tui console).
  for (int i=0; i<listOfSObject->size(); i++) {
    SALOMEDS::SObject_var soField = listOfSObject->at(i);
    int fieldId = -1;
    try {
        fieldId = _studyEditor->getParameterInt(soField,FIELD_ID);    }
    catch(...)    { }
    if (fieldId < 0)  // is it a field serie ?
      {
        int fieldSeriesId = -1;
        try {
            fieldSeriesId = _studyEditor->getParameterInt(soField,FIELD_SERIES_ID);      }
        catch(...)  { }
        // If fieldId and fieldSeriesId equals -1, then it means that it is not a field
        // managed by the MED module, and we stop this function process.
        if ( fieldSeriesId < 0)
          continue;
        MEDCALC::FieldHandlerList* fieldHandlerList = MEDFactoryClient::getDataManager()->getFieldListInFieldseries(fieldSeriesId);
        if (fieldHandlerList->length() < 0)
          continue;
        // For a field series, get the first real field entry:
        MEDCALC::FieldHandler fieldHandler = (*fieldHandlerList)[0];
        fieldId = fieldHandler.id;
      }

    MEDCALC::FieldHandler* fieldHandler = MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);
    if (! fieldHandler) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("No field is defined"));
      return;
    }

    PresentationEvent* event = new PresentationEvent();
    event->eventtype = eventType;
    XmedDataObject* dataObject = new XmedDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    event->objectdata = dataObject;
    emit presentationSignal(event); // --> WorkspaceController::processPresentationEvent
  }
}

void
PresentationController::OnVisualizeScalarMap()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_SCALAR_MAP);
}

void
PresentationController::OnVisualizeContour()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_CONTOUR);
}

void
PresentationController::OnVisualizeVectorField()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_VECTOR_FIELD);
}

void
PresentationController::OnVisualizeSlices()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_SLICES);
}

void
PresentationController::OnVisualizeDeflectionShape()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_DEFLECTION_SHAPE);
}

void
PresentationController::OnVisualizePointSprite()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_POINT_SPRITE);
}

void
PresentationController::OnDeletePresentation()
{
  // We need a _studyEditor updated on the active study
  _studyEditor->updateActiveStudy();

  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();

  // For each object, emit a signal to the workspace to request pres deletion
  for (int i=0; i<listOfSObject->size(); i++) {
    SALOMEDS::SObject_var soPres = listOfSObject->at(i);
    int presId = _studyEditor->getParameterInt(soPres,PRESENTATION_ID);
    // If fieldId equals -1, then it means that it is not a field
    // managed by the MED module, and we stop this function process.
    if ( presId < 0 )
      continue;

    STDLOG("Requesting deletion of presentation: ")
    std::ostringstream oss;
    oss << presId;
    STDLOG("    - Pres id:          " + oss.str());

    PresentationEvent* event = new PresentationEvent();
    event->eventtype = PresentationEvent::EVENT_DELETE_PRESENTATION;
    XmedDataObject* dataObject = new XmedDataObject();
    dataObject->setPresentationId(presId);
    event->objectdata = dataObject;
    emit presentationSignal(event); // --> WorkspaceController::processPresentationEvent
  }
}

void
PresentationController::updateTreeViewWithNewPresentation(long fieldId, long presentationId)
{
  if (presentationId < 0) {
    std::cerr << "Unknown presentation\n";
    return;
  }

  std::string name = MEDFactoryClient::getPresentationManager()->getPresentationProperty(presentationId, "name");
  std::string icon = std::string("ICO_") + name;
  icon = _getIconName(icon);
  name = tr(name.c_str()).toStdString();
  std::string label = tr(icon.c_str()).toStdString();

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(_salomeModule->application()->activeStudy());
  _PTR(Study) studyDS = study->studyDS();

  _salomeModule->engine()->registerPresentation(_CAST(Study, studyDS)->GetStudy(), fieldId, name.c_str(), label.c_str(), presentationId);


  MEDCALC::MEDPresentationViewMode viewMode = MEDFactoryClient::getPresentationManager()->getPresentationViewMode(presentationId);

  // Remove sibling presentations if view mode is set to REPLACE
  if (viewMode == MEDCALC::VIEW_MODE_REPLACE) {
    MED_ORB::PresentationsList* presList = _salomeModule->engine()->getSiblingPresentations(_CAST(Study, studyDS)->GetStudy(), presentationId);
    CORBA::ULong size = presList->length();

    std::stringstream sstm;
    sstm << "Removing sibling presentation(s): ";
    for (int i = 0; i < size; ++i)
      sstm << (*presList)[i] << "  ";
    STDLOG(sstm.str());

    for (int i = 0; i < size; ++i) {
      PresentationEvent* event = new PresentationEvent();
      event->eventtype = PresentationEvent::EVENT_DELETE_PRESENTATION;
      XmedDataObject* dataObject = new XmedDataObject();
      dataObject->setPresentationId((*presList)[i]);
      event->objectdata = dataObject;
      emit presentationSignal(event); // --> WorkspaceController::processPresentationEvent
    }

    delete presList;
  }

  // update Object browser
  _salomeModule->getApp()->updateObjectBrowser(true);
}

void
PresentationController::updateTreeViewForPresentationRemoval(long presentationId)
{
  if (presentationId < 0) {
    std::cerr << "Unknown presentation\n";
    return;
  }

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(_salomeModule->application()->activeStudy());
  _PTR(Study) studyDS = study->studyDS();

  _salomeModule->engine()->unregisterPresentation(_CAST(Study, studyDS)->GetStudy(), presentationId);

  // update Object browser
  _salomeModule->getApp()->updateObjectBrowser(true);
}

void
PresentationController::processWorkspaceEvent(const MEDCALC::MedEvent* event)
{
  if ( event->type == MEDCALC::EVENT_ADD_PRESENTATION ) {
    this->updateTreeViewWithNewPresentation(event->dataId, event->presentationId);
  }
  else if ( event->type == MEDCALC::EVENT_REMOVE_PRESENTATION ) {
    this->updateTreeViewForPresentationRemoval(event->presentationId);
  }
}

void
PresentationController::showDockWidgets(bool isVisible)
{
  STDLOG("Switching PresentationController visibility to: " << isVisible);
  _dockWidget->setVisible(isVisible);
}
