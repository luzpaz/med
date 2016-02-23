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

std::string
WidgetPresentationParameters::getColorMap()
{
  return this->ui.comboBoxColorMap->currentText().toStdString();
}
