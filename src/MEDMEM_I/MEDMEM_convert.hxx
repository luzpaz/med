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

//=============================================================================
// File      : MEDMEM_convert.hxx
// Created   : mer fév 20 15:47:57 CET 2002
// Author    : EDF
// Project   : SALOME
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_convert.hxx
//=============================================================================
//
# if ! defined ( __CONVERT_H__ )
# define __CONVERT_H__

#include "MEDMEM_I.hxx"

#include <SALOMEconfig.h> 
#include CORBA_SERVER_HEADER(MED)
#include "MEDMEM_define.hxx"

using namespace MED_EN;

#include "MEDMEM_GenDriver.hxx"

MEDMEM_I_EXPORT SALOME_MED::medGeometryElement convertMedEltToIdlElt(medGeometryElement element) 
                               throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT SALOME_MED::medEntityMesh      convertMedEntToIdlEnt(medEntityMesh entity) 
                               throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT SALOME_MED::medModeSwitch      convertMedModeToIdlMode(medModeSwitch mode) 
                               throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT SALOME_MED::medDriverTypes     convertMedDriverToIdlDriver(MEDMEM::driverTypes driverType)
                               throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT SALOME_MED::medConnectivity    convertMedConnToIdlConn(medConnectivity connectivite)
                               throw (SALOME::SALOME_Exception);

MEDMEM_I_EXPORT medGeometryElement convertIdlEltToMedElt (SALOME_MED::medGeometryElement element) 
                      throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT medEntityMesh   convertIdlEntToMedEnt(SALOME_MED::medEntityMesh entity) 
                      throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT medModeSwitch       convertIdlModeToMedMode(SALOME_MED::medModeSwitch mode) 
                      throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT MEDMEM::driverTypes           convertIdlDriverToMedDriver(SALOME_MED::medDriverTypes driverType)
                      throw (SALOME::SALOME_Exception);
MEDMEM_I_EXPORT medConnectivity      convertIdlConnToMedConn(SALOME_MED::medConnectivity connectivite)
                      throw (SALOME::SALOME_Exception);


MEDMEM_I_EXPORT bool verifieParam (SALOME_MED::medEntityMesh entity, SALOME_MED::medGeometryElement geomElement)
     throw (SALOME::SALOME_Exception);

# endif         /* # if ! defined ( __CONVERT_H__ ) */
