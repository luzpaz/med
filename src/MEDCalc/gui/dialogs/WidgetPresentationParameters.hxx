#ifndef WIDGET_PRESENTATION_PARAMETERS_HXX
#define WIDGET_PRESENTATION_PARAMETERS_HXX

#include "MEDCALCGUIdialogs.hxx"

#include "ui_WidgetPresentationParameters.h"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDPresentationManager)

class WidgetPresentationParameters : public QWidget
{
public:
  WidgetPresentationParameters(QWidget* parent = 0);

  MEDCALC::MEDPresentationViewMode getViewMode();
  std::string getField();
  std::string getScalarBarRange();
  double getScalarBarTimestep();
  double getScalarBarMinVal();
  double getScalarBarMaxVal();
  MEDCALC::MEDPresentationColorMap getColorMap();

 private:
  Ui_WidgetPresentationParameters ui; // instance of the class defined in ui_WidgetPresentationParameters.h
};


#endif
