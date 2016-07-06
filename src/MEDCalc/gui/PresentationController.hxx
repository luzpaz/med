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

#ifndef PRESENTATION_CONTROLLER_HXX
#define PRESENTATION_CONTROLLER_HXX

#include <QObject>
#include "MEDCALCGUI.hxx"

#include "MEDEventListener_i.hxx"
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDPresentationManager)
#include CORBA_CLIENT_HEADER(MEDDataManager)

#include "XmedDataModel.hxx"
#include <SALOME_AppStudyEditor.hxx>
#include "WidgetPresentationParameters.hxx"

typedef struct {
  enum EventType {
    EVENT_VIEW_OBJECT_CONTOUR,
    EVENT_VIEW_OBJECT_DEFLECTION_SHAPE,
    EVENT_VIEW_OBJECT_POINT_SPRITE,
    EVENT_VIEW_OBJECT_SCALAR_MAP,
    EVENT_VIEW_OBJECT_SLICES,
    EVENT_VIEW_OBJECT_VECTOR_FIELD,
    EVENT_DELETE_PRESENTATION
  };
  int eventtype;
  XmedDataObject* objectdata;
} PresentationEvent;

class MEDModule;
class QDockWidget;

class MEDCALCGUI_EXPORT PresentationController : public QObject {
  Q_OBJECT

public:
  PresentationController(MEDModule* salomeModule);
  ~PresentationController();

  void createActions();

  MEDCALC::MEDPresentationViewMode getSelectedViewMode();
  MEDCALC::MEDPresentationColorMap getSelectedColorMap();

  void showDockWidgets(bool isVisible);

signals:
  void presentationSignal(const PresentationEvent*);

protected slots:
  void OnVisualizeScalarMap();
  void OnVisualizeContour();
  void OnVisualizeVectorField();
  void OnVisualizeSlices();
  void OnVisualizeDeflectionShape();
  void OnVisualizePointSprite();

  void OnDeletePresentation();

  void processWorkspaceEvent(const MEDCALC::MedEvent*);

private:
  void visualize(PresentationEvent::EventType);
  void updateTreeViewWithNewPresentation(long, long);
  void updateTreeViewForPresentationRemoval(long);
  std::string _getIconName(const std::string&);

private:
  MEDModule* _salomeModule;
  SALOME_AppStudyEditor* _studyEditor; // borrowed to MEDModule

  QDockWidget *_dockWidget;
  WidgetPresentationParameters* _widgetPresentationParameters;
};

#endif /* PRESENTATION_CONTROLLER_HXX */
