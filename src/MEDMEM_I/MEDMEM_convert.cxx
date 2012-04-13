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
// File      : MEDMEM_convert.cxx
// Created   : mer fév 20 15:47:57 CET 2002
// Author    : EDF
// Project   : SALOME
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_convert.cxx
//=============================================================================
//
#include "utilities.h"
#include "Utils_CorbaException.hxx"

#include "MEDMEM_convert.hxx"


SALOME_MED::medGeometryElement convertMedEltToIdlElt(medGeometryElement element) 
throw (SALOME::SALOME_Exception)
{
  switch (element)
    {
    case MED_NONE    :  return SALOME_MED::MED_NONE;
    case MED_POINT1  :  return SALOME_MED::MED_POINT1;
    case MED_SEG2    :  return SALOME_MED::MED_SEG2;
    case MED_SEG3    :  return SALOME_MED::MED_SEG3;
    case MED_TRIA3   :  return SALOME_MED::MED_TRIA3;
    case MED_QUAD4   :  return SALOME_MED::MED_QUAD4;
    case MED_TRIA6   :  return SALOME_MED::MED_TRIA6;
    case MED_QUAD8   :  return SALOME_MED::MED_QUAD8;
    case MED_TETRA4  :  return SALOME_MED::MED_TETRA4;
    case MED_PYRA5   :  return SALOME_MED::MED_PYRA5;
    case MED_PENTA6  :  return SALOME_MED::MED_PENTA6;
    case MED_HEXA8   :  return SALOME_MED::MED_HEXA8;
    case MED_TETRA10 :  return SALOME_MED::MED_TETRA10;
    case MED_PYRA13  :  return SALOME_MED::MED_PYRA13;
    case MED_PENTA15 :  return SALOME_MED::MED_PENTA15;
    case MED_HEXA20  :  return SALOME_MED::MED_HEXA20;
    case MED_POLYGON      :  return SALOME_MED::MED_POLYGON;
    case MED_POLYHEDRA    :  return SALOME_MED::MED_POLYHEDRA;
    case MED_ALL_ELEMENTS :  return SALOME_MED::MED_ALL_ELEMENTS;
    default :
        { MESSAGE("Unknown Geometry Element");
          THROW_SALOME_CORBA_EXCEPTION("Unknown Geometry Element",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}

medGeometryElement convertIdlEltToMedElt(SALOME_MED::medGeometryElement element) 
throw (SALOME::SALOME_Exception)
{

  SCRUTE(element);

  switch (element)
    {
    case SALOME_MED::MED_NONE     :  return MED_NONE;
    case SALOME_MED::MED_POINT1   :  return MED_POINT1;
    case SALOME_MED::MED_SEG2     :  return MED_SEG2;
    case SALOME_MED::MED_SEG3     :  return MED_SEG3;
    case SALOME_MED::MED_TRIA3    :  return MED_TRIA3;
    case SALOME_MED::MED_QUAD4    :  return MED_QUAD4;
    case SALOME_MED::MED_TRIA6    :  return MED_TRIA6;
    case SALOME_MED::MED_QUAD8    :  return MED_QUAD8;
    case SALOME_MED::MED_TETRA4   :  return MED_TETRA4;
    case SALOME_MED::MED_PYRA5    :  return MED_PYRA5;
    case SALOME_MED::MED_PENTA6   :  return MED_PENTA6;
    case SALOME_MED::MED_HEXA8    :  return MED_HEXA8;
    case SALOME_MED::MED_TETRA10  :  return MED_TETRA10;
    case SALOME_MED::MED_PYRA13   :  return MED_PYRA13;
    case SALOME_MED::MED_PENTA15  :  return MED_PENTA15;
    case SALOME_MED::MED_HEXA20   :  return MED_HEXA20;
    case SALOME_MED::MED_POLYGON      :  return MED_POLYGON;
    case SALOME_MED::MED_POLYHEDRA    :  return MED_POLYHEDRA;
    case SALOME_MED::MED_ALL_ELEMENTS :  return MED_ALL_ELEMENTS;
    default :
        { MESSAGE("unknown Geometry Element");
          THROW_SALOME_CORBA_EXCEPTION("Unknown Geometry Element",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}

SALOME_MED::medEntityMesh convertMedEntToIdlEnt(medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
  switch (entity)
    {
    case MED_CELL    :  return SALOME_MED::MED_CELL;
    case MED_FACE    :  return SALOME_MED::MED_FACE;
    case MED_EDGE    :  return SALOME_MED::MED_EDGE;
    case MED_NODE    :  return SALOME_MED::MED_NODE;
    case MED_ALL_ENTITIES : return SALOME_MED::MED_ALL_ENTITIES;
    default :
        { MESSAGE("Unknown entity element");
          THROW_SALOME_CORBA_EXCEPTION("Unknown Entity Element",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}

medEntityMesh convertIdlEntToMedEnt(SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
  switch (entity)
    {
    case SALOME_MED::MED_CELL    :  return MED_CELL; 
    case SALOME_MED::MED_FACE    :  return MED_FACE;
    case SALOME_MED::MED_EDGE    :  return MED_EDGE;
    case SALOME_MED::MED_NODE    :  return MED_NODE;
    case SALOME_MED::MED_ALL_ENTITIES : return MED_ALL_ENTITIES;
    default :
        { MESSAGE("Unknown Entity Element");
          THROW_SALOME_CORBA_EXCEPTION("Unknown Entity Element",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}

medModeSwitch convertIdlModeToMedMode(SALOME_MED::medModeSwitch mode)
throw (SALOME::SALOME_Exception)
{
  switch (mode)
    {
    case SALOME_MED::MED_FULL_INTERLACE       : return MED_FULL_INTERLACE; 
    case SALOME_MED::MED_NO_INTERLACE         : return MED_NO_INTERLACE; 
    case SALOME_MED::MED_NO_INTERLACE_BY_TYPE : return MED_NO_INTERLACE_BY_TYPE; 
    default :
        { MESSAGE("Unknown interlace mode");
          THROW_SALOME_CORBA_EXCEPTION("Unknown interlace mode",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}

SALOME_MED::medModeSwitch convertMedModeToIdlMode(medModeSwitch mode)
throw (SALOME::SALOME_Exception)
{
  switch (mode)
    {
    case MED_FULL_INTERLACE       :  return SALOME_MED::MED_FULL_INTERLACE; 
    case MED_NO_INTERLACE         :  return SALOME_MED::MED_NO_INTERLACE; 
    case MED_NO_INTERLACE_BY_TYPE :  return SALOME_MED::MED_NO_INTERLACE_BY_TYPE; 
    default :
        { MESSAGE("Unknown interlace mode");
          THROW_SALOME_CORBA_EXCEPTION("Unknown interlace mode",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}
SALOME_MED::medConnectivity convertMedConnToIdlConn(medConnectivity connectivite)
throw (SALOME::SALOME_Exception)
{
  switch (connectivite)
    {
    case MED_NODAL       :  return SALOME_MED::MED_NODAL; 
    case MED_DESCENDING  :  return SALOME_MED::MED_DESCENDING; 
    default :
        { MESSAGE("Unknown connectivity type");
          THROW_SALOME_CORBA_EXCEPTION("Unknown connectivity type",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}
medConnectivity convertIdlConnToMedConn(SALOME_MED::medConnectivity connectivite)
throw (SALOME::SALOME_Exception)
{
  switch (connectivite)
    {
    case SALOME_MED::MED_NODAL       :  return MED_NODAL; 
    case SALOME_MED::MED_DESCENDING  :  return MED_DESCENDING; 
    default :
        { MESSAGE("Unknown connectivity type");
          THROW_SALOME_CORBA_EXCEPTION("Unknown connectivity type",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}
SALOME_MED::medDriverTypes convertMedDriverToIdlDriver(MEDMEM::driverTypes driverType)
throw (SALOME::SALOME_Exception)
{
  switch (driverType)
    {
    case MEDMEM::MED_DRIVER  :  return SALOME_MED::MED_DRIVER; 
    case MEDMEM::VTK_DRIVER  :  return SALOME_MED::VTK_DRIVER; 
    case MEDMEM::NO_DRIVER   :  return SALOME_MED::NO_DRIVER;
    default :
        { MESSAGE("Unknown driver type");
          THROW_SALOME_CORBA_EXCEPTION("Unknown driver type",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}

MEDMEM::driverTypes convertIdlDriverToMedDriver(SALOME_MED::medDriverTypes driverType)
throw (SALOME::SALOME_Exception)
{
  switch (driverType)
    {
    case SALOME_MED::MED_DRIVER  :  return MEDMEM::MED_DRIVER; 
    case SALOME_MED::VTK_DRIVER  :  return MEDMEM::VTK_DRIVER; 
    case SALOME_MED::NO_DRIVER   :  return MEDMEM::NO_DRIVER;
    default :
        { MESSAGE("Unknown driver type");
          THROW_SALOME_CORBA_EXCEPTION("Unknown driver type",
                                        SALOME::INTERNAL_ERROR);
        } 
   }
}
bool verifieParam (SALOME_MED::medEntityMesh entity,SALOME_MED::medGeometryElement geomElement)
throw (SALOME::SALOME_Exception)
{
  SCRUTE(entity);
  SCRUTE(geomElement);
  bool coherent=false;
  switch (entity)
   { 
    case SALOME_MED::MED_FACE :
       {
         switch (geomElement)
           {
           case SALOME_MED::MED_TRIA3    : coherent=true;
           case SALOME_MED::MED_QUAD4    : coherent=true;
           case SALOME_MED::MED_TRIA6    : coherent=true;
           case SALOME_MED::MED_QUAD8    : coherent=true;
           case SALOME_MED::MED_POLYGON  : coherent=true;
           }
         break;
       }
    case SALOME_MED::MED_CELL :
       {
         switch (geomElement)
           {
           case SALOME_MED::MED_POINT1   : coherent=true;
           case SALOME_MED::MED_SEG2     : coherent=true;
           case SALOME_MED::MED_SEG3     : coherent=true;
           case SALOME_MED::MED_TRIA3    : coherent=true;
           case SALOME_MED::MED_TRIA6    : coherent=true;
           case SALOME_MED::MED_QUAD4    : coherent=true;
           case SALOME_MED::MED_QUAD8    : coherent=true;
           case SALOME_MED::MED_TETRA4   : coherent=true;
           case SALOME_MED::MED_TETRA10  : coherent=true;
           case SALOME_MED::MED_HEXA8    : coherent=true;
           case SALOME_MED::MED_HEXA20   : coherent=true;
           case SALOME_MED::MED_PENTA6   : coherent=true;
           case SALOME_MED::MED_PENTA15  : coherent=true;
           case SALOME_MED::MED_PYRA5    : coherent=true;
           case SALOME_MED::MED_PYRA13   : coherent=true;
           case SALOME_MED::MED_POLYGON  : coherent=true;
           case SALOME_MED::MED_POLYHEDRA: coherent=true;
           }
         break;
       }
    case SALOME_MED::MED_EDGE :
       {
         switch (geomElement)
           {
           case SALOME_MED::MED_SEG2     : coherent=true;
           case SALOME_MED::MED_SEG3     : coherent=true;
           }
         break;
       }         
    case SALOME_MED::MED_NODE :
       {
         switch (geomElement)
           {
           case SALOME_MED::MED_POINT1   : coherent=true;
           }
         break;
       }
    case SALOME_MED::MED_ALL_ENTITIES :
       {
         switch (geomElement)
           {
           case SALOME_MED::MED_NONE     : coherent=true;
           }
         break;
       }
    default :
        { MESSAGE("Unknown geometric entity");
          THROW_SALOME_CORBA_EXCEPTION("Unknown geometric entity",
                                        SALOME::INTERNAL_ERROR);
        } 
         break;
    } 

     switch (geomElement)
     {
           case SALOME_MED::MED_ALL_ELEMENTS     : coherent=true;
     }
    return coherent;
}
