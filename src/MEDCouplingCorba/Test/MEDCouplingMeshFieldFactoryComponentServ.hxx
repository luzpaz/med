//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

#ifndef __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTSERV_HXX__
#define __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTSERV_HXX__

#include <cppunit/extensions/HelperMacros.h>
#include "SALOMEconfig.h"

#include CORBA_SERVER_HEADER(MEDCouplingCorbaServantTest)

namespace ParaMEDMEM
{
  class MEDCouplingUMesh;
}

namespace SALOME_TEST
{
  class MEDCouplingMeshFieldFactoryComponent : public POA_SALOME_TEST::MEDCouplingMeshFieldFactory
  {
  public:
    MEDCouplingMeshFieldFactoryComponent(CORBA::ORB_ptr orb);
    void destroyFactory();
    void shutdownOrb();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get1DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get2DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get3DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get3DSurfMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get0DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr getM1DMesh();
    SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr getExtrudedMesh();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn2DNT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldNodeScalarOn2DNT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn3DNT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn3DSurfWT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn3DSurfCOTI();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn2DLT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldGaussPt2DWT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldGaussPtNE2DWT();
  private:
    CORBA::ORB_ptr _orb;
  };
}

#endif
