// Copyright (C) 2007-2014  CEA/DEN, EDF R&D
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

// Author : Guillaume Boulant (EDF)

#ifndef _MEDOP_MODULE_HXX_
#define _MEDOP_MODULE_HXX_

#include "StandardApp_Module.hxx"

#include "WorkspaceController.hxx"
#include "XmedDataModel.hxx"
#include "DatasourceController.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Component)

#include "MEDOPGUI.hxx"

/*!
 * This class defines the gui of the MEDOP module.
 */
class MEDOPGUI_EXPORT MEDOPModule: public StandardApp_Module
{
  Q_OBJECT

  // ===========================================================
  // This part implements the standard interface of a SALOME gui
  // ===========================================================

public:
  MEDOPModule();

protected:
  virtual Engines::EngineComponent_ptr getEngine() const;
  virtual QString studyIconName();
  virtual void createModuleWidgets();
  virtual void createModuleActions();
  virtual bool activateModule( SUIT_Study* theStudy );
  virtual bool deactivateModule( SUIT_Study* theStudy );

private:
  DatasourceController * _datasourceController;
  WorkspaceController *  _workspaceController;
  XmedDataModel *        _xmedDataModel;
};

#endif
