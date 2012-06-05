// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  MED MEDGUI : MED component GUI implemetation
//  File   : MEDGUI.h
//  Module : MED
//
#ifndef __MEDGUI_HXX_
#define __MEDGUI_HXX__

#include <SalomeApp_Module.h>
#include <SUIT_Desktop.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MED_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class MEDGUIDataBaseDockWidget;

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

  //virtual void                        contextMenuPopup( const QString&, QMenu*, QString& );

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
  MEDGUIDataBaseDockWidget *_data_base;

private slots:
  void onGUIEvent();
  void onWindowActivated( SUIT_ViewWindow* );

};

#endif
