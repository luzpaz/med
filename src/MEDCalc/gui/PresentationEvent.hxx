#ifndef SRC_MEDCALC_GUI_PRESENTATIONEVENT_HXX_
#define SRC_MEDCALC_GUI_PRESENTATIONEVENT_HXX_

#include <string>

typedef struct {
  enum EventType {
    EVENT_VIEW_OBJECT_MESH_VIEW,
    EVENT_VIEW_OBJECT_CONTOUR,
    EVENT_VIEW_OBJECT_DEFLECTION_SHAPE,
    EVENT_VIEW_OBJECT_POINT_SPRITE,
    EVENT_VIEW_OBJECT_SCALAR_MAP,
    EVENT_VIEW_OBJECT_SLICES,
    EVENT_VIEW_OBJECT_VECTOR_FIELD,

    EVENT_DELETE_PRESENTATION,

    EVENT_CHANGE_COMPONENT,
    EVENT_CHANGE_COLORMAP,
    EVENT_CHANGE_TIME_RANGE,

    EVENT_CHANGE_MESH_MODE,

    EVENT_CHANGE_NB_CONTOUR,

    EVENT_CHANGE_SLICE_ORIENTATION,
    EVENT_CHANGE_NB_SLICES
  };

  EventType eventtype;
  MEDCALC::FieldHandler * fieldHandler;
  MEDCALC::MeshHandler * meshHandler;
  int presentationId;
  int anInteger;                // multi-purpose variable
  //int anInteger2;             // multi-purpose variable
  std::string aString;          // multi-purpose variable
} PresentationEvent;

#endif
