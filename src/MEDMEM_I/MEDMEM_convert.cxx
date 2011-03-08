//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
    case MEDMEM_NONE    :  return SALOME_MED::MEDMEM_NONE;
    case MEDMEM_POINT1  :  return SALOME_MED::MEDMEM_POINT1;
    case MEDMEM_SEG2    :  return SALOME_MED::MEDMEM_SEG2;
    case MEDMEM_SEG3    :  return SALOME_MED::MEDMEM_SEG3;
    case MEDMEM_TRIA3   :  return SALOME_MED::MEDMEM_TRIA3;
    case MEDMEM_QUAD4   :  return SALOME_MED::MEDMEM_QUAD4;
    case MEDMEM_TRIA6   :  return SALOME_MED::MEDMEM_TRIA6;
    case MEDMEM_QUAD8   :  return SALOME_MED::MEDMEM_QUAD8;
    case MEDMEM_TETRA4  :  return SALOME_MED::MEDMEM_TETRA4;
    case MEDMEM_PYRA5   :  return SALOME_MED::MEDMEM_PYRA5;
    case MEDMEM_PENTA6  :  return SALOME_MED::MEDMEM_PENTA6;
    case MEDMEM_HEXA8   :  return SALOME_MED::MEDMEM_HEXA8;
    case MEDMEM_TETRA10 :  return SALOME_MED::MEDMEM_TETRA10;
    case MEDMEM_PYRA13  :  return SALOME_MED::MEDMEM_PYRA13;
    case MEDMEM_PENTA15 :  return SALOME_MED::MEDMEM_PENTA15;
    case MEDMEM_HEXA20  :  return SALOME_MED::MEDMEM_HEXA20;
    case MEDMEM_POLYGON      :  return SALOME_MED::MEDMEM_POLYGON;
    case MEDMEM_POLYHEDRA    :  return SALOME_MED::MEDMEM_POLYHEDRA;
    case MEDMEM_ALL_ELEMENTS :  return SALOME_MED::MEDMEM_ALL_ELEMENTS;
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
    case SALOME_MED::MEDMEM_NONE     :  return MEDMEM_NONE;
    case SALOME_MED::MEDMEM_POINT1   :  return MEDMEM_POINT1;
    case SALOME_MED::MEDMEM_SEG2     :  return MEDMEM_SEG2;
    case SALOME_MED::MEDMEM_SEG3     :  return MEDMEM_SEG3;
    case SALOME_MED::MEDMEM_TRIA3    :  return MEDMEM_TRIA3;
    case SALOME_MED::MEDMEM_QUAD4    :  return MEDMEM_QUAD4;
    case SALOME_MED::MEDMEM_TRIA6    :  return MEDMEM_TRIA6;
    case SALOME_MED::MEDMEM_QUAD8    :  return MEDMEM_QUAD8;
    case SALOME_MED::MEDMEM_TETRA4   :  return MEDMEM_TETRA4;
    case SALOME_MED::MEDMEM_PYRA5    :  return MEDMEM_PYRA5;
    case SALOME_MED::MEDMEM_PENTA6   :  return MEDMEM_PENTA6;
    case SALOME_MED::MEDMEM_HEXA8    :  return MEDMEM_HEXA8;
    case SALOME_MED::MEDMEM_TETRA10  :  return MEDMEM_TETRA10;
    case SALOME_MED::MEDMEM_PYRA13   :  return MEDMEM_PYRA13;
    case SALOME_MED::MEDMEM_PENTA15  :  return MEDMEM_PENTA15;
    case SALOME_MED::MEDMEM_HEXA20   :  return MEDMEM_HEXA20;
    case SALOME_MED::MEDMEM_POLYGON      :  return MEDMEM_POLYGON;
    case SALOME_MED::MEDMEM_POLYHEDRA    :  return MEDMEM_POLYHEDRA;
    case SALOME_MED::MEDMEM_ALL_ELEMENTS :  return MEDMEM_ALL_ELEMENTS;
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
           case SALOME_MED::MEDMEM_TRIA3    : coherent=true;
           case SALOME_MED::MEDMEM_QUAD4    : coherent=true;
           case SALOME_MED::MEDMEM_TRIA6    : coherent=true;
           case SALOME_MED::MEDMEM_QUAD8    : coherent=true;
           case SALOME_MED::MEDMEM_POLYGON  : coherent=true;
           }
         break;
       }
    case SALOME_MED::MED_CELL :
       {
         switch (geomElement)
           {
           case SALOME_MED::MEDMEM_POINT1   : coherent=true;
           case SALOME_MED::MEDMEM_SEG2     : coherent=true;
           case SALOME_MED::MEDMEM_SEG3     : coherent=true;
           case SALOME_MED::MEDMEM_TRIA3    : coherent=true;
           case SALOME_MED::MEDMEM_TRIA6    : coherent=true;
           case SALOME_MED::MEDMEM_QUAD4    : coherent=true;
           case SALOME_MED::MEDMEM_QUAD8    : coherent=true;
           case SALOME_MED::MEDMEM_TETRA4   : coherent=true;
           case SALOME_MED::MEDMEM_TETRA10  : coherent=true;
           case SALOME_MED::MEDMEM_HEXA8    : coherent=true;
           case SALOME_MED::MEDMEM_HEXA20   : coherent=true;
           case SALOME_MED::MEDMEM_PENTA6   : coherent=true;
           case SALOME_MED::MEDMEM_PENTA15  : coherent=true;
           case SALOME_MED::MEDMEM_PYRA5    : coherent=true;
           case SALOME_MED::MEDMEM_PYRA13   : coherent=true;
           case SALOME_MED::MEDMEM_POLYGON  : coherent=true;
           case SALOME_MED::MEDMEM_POLYHEDRA: coherent=true;
           }
         break;
       }
    case SALOME_MED::MED_EDGE :
       {
         switch (geomElement)
           {
           case SALOME_MED::MEDMEM_SEG2     : coherent=true;
           case SALOME_MED::MEDMEM_SEG3     : coherent=true;
           }
         break;
       }         
    case SALOME_MED::MED_NODE :
       {
         switch (geomElement)
           {
           case SALOME_MED::MEDMEM_POINT1   : coherent=true;
           }
         break;
       }
    case SALOME_MED::MED_ALL_ENTITIES :
       {
         switch (geomElement)
           {
           case SALOME_MED::MEDMEM_NONE     : coherent=true;
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
           case SALOME_MED::MEDMEM_ALL_ELEMENTS     : coherent=true;
     }
    return coherent;
}
