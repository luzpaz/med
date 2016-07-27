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

#include "MEDWidgetHelperScalarMap.hxx"
#include "MEDPresentationScalarMap.hxx"  // from component side.
#include "PresentationController.hxx"

#include <sstream>

MEDWidgetHelperScalarMap::MEDWidgetHelperScalarMap(const PresentationController * presController, MEDCALC::MEDPresentationManager_ptr presManager, int presId,
                                                   const std::string & presName, WidgetPresentationParameters * paramW):
  MEDWidgetHelper(presController, presManager, presId, presName, paramW)
{
}

MEDWidgetHelperScalarMap::~MEDWidgetHelperScalarMap()
{}

void MEDWidgetHelperScalarMap::udpateWidget()
{
  MEDWidgetHelper::udpateWidget();

  // Scalar map presentation needs the component name, that's all.
  _paramWidget->setComponents(_allCompos, _selectedCompo);

  // Connect combo box changes
  QObject::connect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                    _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
  QObject::connect( _paramWidget, SIGNAL(comboCompoIndexChanged(int)), this, SLOT(onComponentChanged(int)) );
}

void MEDWidgetHelperScalarMap::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  QObject::disconnect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                       _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
  QObject::disconnect( _paramWidget, SIGNAL(comboCompoIndexChanged(int)), this, SLOT(onComponentChanged(int)) );
}
