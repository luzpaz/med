#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import MEDCouplingCorbaSwigTest
import MEDCouplingCorbaServantTest_idl

from libMEDCouplingCorba_Swig import *

class MEDCouplingMeshFieldFactoryComponentPy(MEDCouplingCorbaServantTest_idl._0_SALOME_TEST__POA.MEDCouplingMeshFieldFactory):
    def __init__(self,orb):
        self._orb=orb
        self._test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        pass
    
    def destroyFactory(self):
        oid = self._default_POA().servant_to_id(self);
        MEDCouplingCorbaServantTest_idl._0_SALOME_TEST__POA.MEDCouplingMeshFieldFactory._default_POA().deactivate_object(self,oid);
        MEDCouplingCorbaServantTest_idl._0_SALOME_TEST__POA.MEDCouplingMeshFieldFactory._remove_ref(self);
        
    def shutdownOrb(self):
        self._orb.shutdown(0)

    def get1DMesh(self):
        mesh=self._test.build1DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def get2DMesh(self):
        mesh=self._test.build2DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def get3DMesh(self):
        mesh=self._test.build3DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def get3DSurfMesh(self):
        mesh=self._test.build3DSurfMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def get0DMesh(self):
        mesh=self._test.build0DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def getM1DMesh(self):
        mesh=self._test.buildM1DMesh()
        return MEDCouplingUMeshServant._this(mesh)

    def getExtrudedMesh(self):
        mesh=self._test.buildExtrudedMesh()
        return MEDCouplingExtrudedMeshServant._this(mesh)

    def getCMesh(self):
        mesh=self._test.buildCMesh()
        return MEDCouplingCMeshServant._this(mesh)
    
    def getFieldScalarOn2DNT(self):
        field=self._test.buildFieldScalarOn2DNT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldNodeScalarOn2DNT(self):
        field=self._test.buildFieldNodeScalarOn2DNT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldScalarOn3DNT(self):
        field=self._test.buildFieldScalarOn3DNT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldScalarOn3DSurfWT(self):
        field=self._test.buildFieldScalarOn3DSurfWT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldScalarOn3DSurfCOTI(self):
        field=self._test.buildFieldScalarOn3DSurfCOTI()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldScalarOn2DLT(self):
        field=self._test.buildFieldScalarOn2DLT()
        return MEDCouplingFieldDoubleServant._this(field)

    def getFieldGaussPt2DWT(self):
        field=self._test.buildFieldGaussPt2DWT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldGaussPtNE2DWT(self):
        field=self._test.buildFieldGaussPtNE2DWT()
        return MEDCouplingFieldDoubleServant._this(field)

    def getFieldVectorOnExtrudedWT(self):
        field=self._test.buildFieldVectorOnExtrudedWT()
        return MEDCouplingFieldDoubleServant._this(field)

    def getFieldVectorOnCMeshWT(self):
        field=self._test.buildFieldVectorOnCMeshWT()
        return MEDCouplingFieldDoubleServant._this(field)
    pass
