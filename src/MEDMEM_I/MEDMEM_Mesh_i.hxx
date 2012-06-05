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
// File      : MEDMEM_Mesh_i.hxx
// Project   : SALOME
// Author    : EDF 
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Mesh_i.hxx
//=============================================================================
//
#ifndef _MED_MESH_I_HXX_
#define _MED_MESH_I_HXX_

#include <MEDMEM_I.hxx>

#include <map>
#include <string>

#include <SALOMEconfig.h>
#include "SALOMEMultiComm.hxx"
#include "SALOME_GenericObj_i.hh"
#include "MEDMEM_GMesh_i.hxx"
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)
#include CORBA_SERVER_HEADER(SALOME_Comm)

namespace MEDMEM 
{
  class MESH;
  class MEDMEM_I_EXPORT MESH_i: virtual public POA_SALOME_MED::MESH,
                                virtual public GMESH_i
  {
  public:

    // Constructors and associated internal methods
    MESH_i();
    MESH_i(::MEDMEM::MESH * const m);
    MESH_i(MESH_i & m);
    ~MESH_i();

    // IDL Methods 
    CORBA::Boolean  existConnectivity ( SALOME_MED::medConnectivity connectivityType,
                                        SALOME_MED::medEntityMesh entity)
      throw (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfDouble* getCoordinates(SALOME_MED::medModeSwitch typeSwitch)
      throw (SALOME::SALOME_Exception);

    SALOME::SenderDouble_ptr getSenderForCoordinates(SALOME_MED::medModeSwitch typeSwitch)
      throw (SALOME::SALOME_Exception);

    CORBA::Double  getCoordinate(CORBA::Long Number, CORBA::Long Axis)
      throw (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfLong*   getConnectivity(SALOME_MED::medConnectivity mode, 
                                                SALOME_MED::medEntityMesh entity, 
                                                SALOME_MED::medGeometryElement geomElement)
      throw (SALOME::SALOME_Exception);

    SALOME::SenderInt_ptr getSenderForConnectivity(SALOME_MED::medConnectivity mode, 
                                                   SALOME_MED::medEntityMesh entity, 
                                                   SALOME_MED::medGeometryElement geomElement)
      throw (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfLong*   getConnectivityIndex(SALOME_MED::medConnectivity mode,
                                                     SALOME_MED::medEntityMesh entity)
      throw (SALOME::SALOME_Exception);

    SALOME::SenderInt_ptr getSenderForConnectivityIndex(SALOME_MED::medConnectivity mode,
                                                        SALOME_MED::medEntityMesh entity,
                                                        SALOME_MED::medGeometryElement geomElement)
      throw (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfLong*   getGlobalNumberingIndex(SALOME_MED::medEntityMesh entity)
      throw (SALOME::SALOME_Exception);

    CORBA::Long            getElementNumber(SALOME_MED::medConnectivity mode,
                                            SALOME_MED::medEntityMesh entity,
                                            SALOME_MED::medGeometryElement type,
                                            const SALOME_TYPES::ListOfLong& connectivity)
      throw  (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfLong*   getReverseConnectivity(SALOME_MED::medConnectivity mode)
      throw (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfLong*   getReverseConnectivityIndex(SALOME_MED::medConnectivity mode)
      throw (SALOME::SALOME_Exception);


    //                                  Cuisine interne
    ::MEDMEM::MESH * constructConstMesh() const;
    SALOME_MED::MESH::connectivityInfos * getConnectGlobal (SALOME_MED::medEntityMesh entity)
      throw (SALOME::SALOME_Exception);
    CORBA::Boolean areEquals(SALOME_MED::MESH_ptr other);
  };
}

#endif /* _MED_MESH_I_HXX_ */
