//  MED MEDGUI : MED component GUI implemetation 
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : MEDGUI.h
//  Module : MED

#ifndef _MEDGUI_H_
#define _MEDGUI_H_

#include "SALOMEGUI.h"
#include "QAD_Desktop.h"
#include "QAD_StudyFrame.h"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MED_Gen)
//#include CORBA_CLIENT_HEADER(SMESH_Mesh)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class MedGUI: public SALOMEGUI
{
  Q_OBJECT

public:
  MedGUI( const QString& name = "", QObject* parent = 0 );

  virtual bool OnGUIEvent        (int theCommandID, QAD_Desktop* parent);
  virtual bool OnKeyPress        (QKeyEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
  virtual bool OnMousePress      (QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
  virtual bool OnMouseMove       (QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
  virtual bool SetSettings       ( QAD_Desktop* parent );
  virtual bool CustomPopup       ( QAD_Desktop* parent, QPopupMenu* popup, const QString & theContext,
                                   const QString & theParent, const QString & theObject );
  virtual void DefinePopup       ( QString & theContext, QString & theParent, QString & theObject );
  virtual bool ActiveStudyChanged( QAD_Desktop* parent );

  static SALOME_MED::MED_Gen_ptr InitMedGen(QAD_Desktop* parent);

  static bool DumpMesh( SALOME_MED::MESH_var aMesh );
  //  static bool DumpSubMesh( SMESH::SMESH_subMesh_ptr aSubMesh ) ;
  static bool DumpSubMesh( SALOME_MED::FAMILY_var Fam ) ;

  static void setOrb();

protected:

private:

};

#endif
