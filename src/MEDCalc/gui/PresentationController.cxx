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
#include "MEDModule.hxx"
#include "Basics_Utils.hxx"
#include "QtxActionGroup.h"

static const int OPTIONS_VIEW_MODE_ID = 943;
static const int OPTIONS_VIEW_MODE_REPLACE_ID = 944;
static const int OPTIONS_VIEW_MODE_OVERLAP_ID = 945;
static const int OPTIONS_VIEW_MODE_NEW_LAYOUT_ID = 946;
static const int OPTIONS_VIEW_MODE_SPLIT_VIEW_ID = 947;

PresentationController::PresentationController(MEDModule* salomeModule)
{
  STDLOG("Creating a PresentationController");
  _salomeModule = salomeModule;
}

PresentationController::~PresentationController()
{
  STDLOG("Deleting the PresentationController");
}

void
PresentationController::createActions()
{
  STDLOG("Creating PresentationController actions");
  int toolbarId = _salomeModule->createTool("View Mode", "PresentationToolbar");

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

  _salomeModule->createTool(OPTIONS_VIEW_MODE_ID, toolbarId);


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
}
