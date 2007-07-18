//  MED MEDGUI : MED component GUI implemetation
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : MEDGUI.h
//  Module : MED

#ifndef _MEDGUI_H_
#define _MEDGUI_H_

#include <SalomeApp_Module.h>
#include <SUIT_Desktop.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MED_Gen)
//#include CORBA_CLIENT_HEADER(SMESH_Mesh)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class MedGUI: public SalomeApp_Module
{
  Q_OBJECT

public:
  MedGUI();

  virtual void                        initialize( CAM_Application* );
  virtual QString                     engineIOR() const;
  virtual void                        windows( QMap<int, int>& mappa ) const;
  
  virtual bool                        OnGUIEvent(int theCommandID);
  virtual bool                        OnKeyPress(QKeyEvent* pe, SUIT_ViewWindow* );
  virtual bool                        OnMousePress(QMouseEvent* pe, SUIT_ViewWindow* );
  virtual bool                        OnMouseMove(QMouseEvent* pe, SUIT_ViewWindow* );

  void createMedAction( const int, const QString&, const QString& = "" );

  virtual void                        contextMenuPopup( const QString&, QMenu*, QString& );

  //virtual bool SetSettings       ();
  //virtual bool CustomPopup       ( QAD_Desktop* parent, QPopupMenu* popup, const QString & theContext,
  //                                 const QString & theParent, const QString & theObject );
  //virtual void DefinePopup       ( QString & theContext, QString & theParent, QString & theObject );
  //virtual bool ActiveStudyChanged( QAD_Desktop* parent );

  /*static*/ SALOME_MED::MED_Gen_ptr InitMedGen() const;

  static bool DumpMesh( SALOME_MED::MESH_var aMesh );
  static bool DumpSubMesh( SALOME_MED::FAMILY_var Fam ) ;

  static void setOrb();

  void EmitSignalCloseAllDialogs();

signals :
  void                        SignalCloseAllDialogs();

public slots:
  virtual bool                deactivateModule( SUIT_Study* );
  virtual bool                activateModule( SUIT_Study* );

protected:
private slots:
  void onGUIEvent();
  void onWindowActivated( SUIT_ViewWindow* );

};

#endif
