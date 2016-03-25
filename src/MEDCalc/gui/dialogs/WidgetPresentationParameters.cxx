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

#include "WidgetPresentationParameters.hxx"

WidgetPresentationParameters::WidgetPresentationParameters(QWidget* parent)
  : QWidget(parent)
{
  ui.setupUi(this); // To be done first
}

MEDCALC::MEDPresentationViewMode
WidgetPresentationParameters::getViewMode() {
  QString viewMode = this->ui.comboBoxViewMode->currentText();
  if (viewMode == tr("LAB_VIEW_MODE_REPLACE")) {
    return MEDCALC::VIEW_MODE_REPLACE;
  }
  else if (viewMode == tr("LAB_VIEW_MODE_OVERLAP")) {
    return MEDCALC::VIEW_MODE_OVERLAP;
  }
  else if (viewMode == tr("LAB_VIEW_MODE_NEW_LAYOUT")) {
    return MEDCALC::VIEW_MODE_NEW_LAYOUT;
  }
  else if (viewMode == tr("LAB_VIEW_MODE_SPLIT_VIEW")) {
    return MEDCALC::VIEW_MODE_SPLIT_VIEW;
  }
}

std::string
WidgetPresentationParameters::getField()
{
  return this->ui.comboBoxField->currentText().toStdString();
}

std::string
WidgetPresentationParameters::getScalarBarRange()
{
  return this->ui.comboBoxScalarBarRange->currentText().toStdString();
}

double
WidgetPresentationParameters::getScalarBarTimestep()
{
  return this->ui.doubleSpinBoxTimeStep->value();
}

double
WidgetPresentationParameters::getScalarBarMinVal()
{
  return this->ui.doubleSpinBoxMinVal->value();
}

double
WidgetPresentationParameters::getScalarBarMaxVal()
{
  return this->ui.doubleSpinBoxMaxVal->value();
}

MEDCALC::MEDPresentationColorMap
WidgetPresentationParameters::getColorMap()
{
  QString colorMap = this->ui.comboBoxColorMap->currentText();
  if (colorMap == tr("LAB_BLUE_TO_RED")) {
    return MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW;
  }
  else if (colorMap == tr("LAB_COOL_TO_WARM")) {
    return MEDCALC::COLOR_MAP_COOL_TO_WARM;
  }
}
