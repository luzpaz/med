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
#include <Basics_Utils.hxx>

using namespace std;

WidgetPresentationParameters::WidgetPresentationParameters(QWidget* parent)
  : QWidget(parent), _blockSig(false)
{
  _ui.setupUi(this); // To be done first

  toggleWidget(false);
  QObject::connect(_ui.comboBoxCompo,          SIGNAL(currentIndexChanged(int)),
                   this,                       SLOT(onComboCompoIndexChanged(int)) );
  QObject::connect(_ui.comboBoxScalarBarRange, SIGNAL(currentIndexChanged(int)),
                   this,                       SLOT(onComboScalarBarRangeIndexChanged(int)) );
  QObject::connect(_ui.comboBoxColorMap,       SIGNAL(currentIndexChanged(int)),
                   this,                       SLOT(onComboColorMapIndexChanged(int)) );
}

void
WidgetPresentationParameters::onComboCompoIndexChanged(int idx)
{
  if (!_blockSig) emit comboCompoIndexChanged(idx);
}

void
WidgetPresentationParameters::onComboColorMapIndexChanged(int idx)
{
  if (!_blockSig) emit comboColorMapIndexChanged(idx);
}

void
WidgetPresentationParameters::onComboScalarBarRangeIndexChanged(int idx)
{
  if (!_blockSig) emit comboScalarBarRangeIndexChanged(idx);
}


void
WidgetPresentationParameters::toggleWidget(bool show)
{
  if (!show)
    {
      _blockSig = true;
      _ui.widgetDynamic->hide();
      setPresName("Choose a presentation");
      // reset colorMap and scalarBarRange:
      setColorMap(MEDCALC::COLOR_MAP_DEFAULT);
      setScalarBarRange(MEDCALC::SCALAR_BAR_RANGE_DEFAULT);
    }
  else
    {
      _ui.widgetDynamic->show();
      // It is the WidgetHelper responsability to re-show the widgets it needs
      _ui.labelCompo->hide();
      _ui.comboBoxCompo->hide();
      _ui.labelSpinBox->hide();
      _ui.spinBox->hide();
      _ui.labelSliceOrient->hide();
      _ui.comboBoxSliceOrient->hide();
      _blockSig = false;
    }
}

bool
WidgetPresentationParameters::isShown() const
{
  return _ui.widgetDynamic->isVisible();
}

string
WidgetPresentationParameters::getComponent() const
{
  if (_ui.comboBoxCompo->currentIndex() == 0) // Euclidean norm
      return "";

  return _ui.comboBoxCompo->currentText().toStdString();
}

void
WidgetPresentationParameters::setComponents(vector<string> compos, int selecIndex)
{
  // Show the widget:
  _ui.labelCompo->show();
  _ui.comboBoxCompo->show();

  _blockSig = true;
  _ui.comboBoxCompo->clear();
  _ui.comboBoxCompo->addItem(tr("LAB_EUCLIDEAN_NORM"));
  for(vector<string>::const_iterator it = compos.begin(); it != compos.end(); ++it)
    _ui.comboBoxCompo->addItem(QString::fromStdString(*it));
  _ui.comboBoxCompo->setCurrentIndex(selecIndex);
  _blockSig = false;
}

void
WidgetPresentationParameters::setScalarBarRange(MEDCALC::MEDPresentationScalarBarRange sbrange)
{
  int idx;
  if (sbrange == MEDCALC::SCALAR_BAR_ALL_TIMESTEPS)
    idx = _ui.comboBoxScalarBarRange->findText(tr("LAB_ALL_TIMESTEPS"));
  else if (sbrange == MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP)
    idx = _ui.comboBoxScalarBarRange->findText(tr("LAB_CURRENT_TIMESTEP"));

  if (idx >= 0)
    {
      _blockSig = true;
      _ui.comboBoxScalarBarRange->setCurrentIndex(idx);
      _blockSig = false;
    }
  else
    STDLOG("Strange!! No matching found - unable to set scalar bar range in GUI.");
}

void
WidgetPresentationParameters::setColorMap(MEDCALC::MEDPresentationColorMap colorMap)
{
  int idx;
  if (colorMap == MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW)
    idx = _ui.comboBoxColorMap->findText(tr("LAB_BLUE_TO_RED"));
  else if (colorMap == MEDCALC::COLOR_MAP_COOL_TO_WARM)
    idx = _ui.comboBoxColorMap->findText(tr("LAB_COOL_TO_WARM"));

  if (idx >= 0)
    {
      _blockSig = true;
      _ui.comboBoxColorMap->setCurrentIndex(idx);
      _blockSig = false;
    }

  else
    STDLOG("Strange!! No matching found - unable to set color map in GUI.");
}


MEDCALC::MEDPresentationScalarBarRange
WidgetPresentationParameters::getScalarBarRange() const
{
  QString sbrange = _ui.comboBoxScalarBarRange->currentText();
  if (sbrange == tr("LAB_ALL_TIMESTEPS")) {
    return MEDCALC::SCALAR_BAR_ALL_TIMESTEPS;
  }
  else if (sbrange == tr("LAB_CURRENT_TIMESTEP")) {
    return MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP;
  }
  // Should not happen
  STDLOG("Strange!! No matching found - returning SCALAR_BAR_ALL_TIMESTEPS.");
  return MEDCALC::SCALAR_BAR_ALL_TIMESTEPS;
}

//double
//WidgetPresentationParameters::getScalarBarTimestep() const
//{
//  return _ui.doubleSpinBoxTimeStep->value();
//}
//
//double
//WidgetPresentationParameters::getScalarBarMinVal() const
//{
//  return _ui.doubleSpinBoxMinVal->value();
//}
//
//double
//WidgetPresentationParameters::getScalarBarMaxVal() const
//{
//  return _ui.doubleSpinBoxMaxVal->value();
//}

MEDCALC::MEDPresentationColorMap
WidgetPresentationParameters::getColorMap() const
{
  QString colorMap = _ui.comboBoxColorMap->currentText();
  if (colorMap == tr("LAB_BLUE_TO_RED")) {
    return MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW;
  }
  else if (colorMap == tr("LAB_COOL_TO_WARM")) {
    return MEDCALC::COLOR_MAP_COOL_TO_WARM;
  }
  // Should not happen
  STDLOG("Strange!! No matching color map found - returning blue to red.");
  return MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW;
}

QComboBox *
WidgetPresentationParameters::getComboBoxCompo()
{
  return _ui.comboBoxCompo;
}

void
WidgetPresentationParameters::setPresName(const std::string& name)
{
  _ui.labelPresName->setText(QString::fromStdString(name));
  QFont f(_ui.labelPresName->font());
  f.setItalic(true);
  _ui.labelPresName->setFont(f);
}
