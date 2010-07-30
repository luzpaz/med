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

from libMEDCoupling_Swig import *
import math
import os

class MEDCouplingCorbaServBasicsTest:
    def build1DMesh(self):
        coords=[ 0.0, 0.3, 0.75, 1.0 ]
        conn=[ 0,1, 1,2, 2,3 ]
        mesh=MEDCouplingUMesh.New("1DMeshForCorba",1);
        mesh.allocateCells(3);
        mesh.insertNextCell(NORM_SEG2,2,conn[0:2]);
        mesh.insertNextCell(NORM_SEG2,2,conn[2:4]);
        mesh.insertNextCell(NORM_SEG2,2,conn[4:6]);
        mesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(coords,4,1);
        mesh.setCoords(myCoords);
        mesh.changeSpaceDimension(3);
        myCoords=mesh.getCoords();
        myCoords.setInfoOnComponent(0,"X1D(m)");
        myCoords.setInfoOnComponent(1,"Y1D (dm)");
        myCoords.setInfoOnComponent(2,"Z1D (pm)");
        center=[0.,0.,0.]
        vector=[0.,1.,0.]
        mesh.rotate(center,vector,-math.pi/2.);
        return mesh
        
    def build2DMesh(self):
        targetCoords=[-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 ];
        targetConn=[0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4]
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(2);
        targetMesh.setName("MyMesh2D");
        targetMesh.allocateCells(5);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[0:4]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[4:7]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[7:10]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[10:14]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[14:18]);
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,9,2);
        targetMesh.setCoords(myCoords);
        return targetMesh;
    
    def build3DMesh(self):
        targetCoords=[ 0., 0., 0., 50., 0., 0. , 200., 0., 0.  , 0., 50., 0., 50., 50., 0. , 200., 50., 0.,   0., 200., 0., 50., 200., 0. , 200., 200., 0. ,
                       0., 0., 50., 50., 0., 50. , 200., 0., 50.  , 0., 50., 50., 50., 50., 50. , 200., 50., 50.,   0., 200., 50., 50., 200., 50. , 200., 200., 50. ,
                       0., 0., 200., 50., 0., 200. , 200., 0., 200.  , 0., 50., 200., 50., 50., 200. , 200., 50., 200.,   0., 200., 200., 50., 200., 200. , 200., 200., 200. ];
        targetConn=[0,1,4,3,9,10,13,12, 1,2,5,4,10,11,14,13, 3,4,7,6,12,13,16,15, 4,5,8,7,13,14,17,16,
                    9,10,13,12,18,19,22,21, 10,11,14,13,19,20,23,22, 12,13,16,15,21,22,25,24, 13,14,17,16,22,23,26,25];
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(3);
        targetMesh.setName("MyMesh3D");
        targetMesh.allocateCells(12);
        for i in xrange(8):
            targetMesh.insertNextCell(NORM_HEXA8,8,targetConn[8*i:8*(i+1)]);
            pass
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,27,3);
        targetMesh.setCoords(myCoords)
        return targetMesh;

    def build3DSurfMesh(self):
        targetCoords=[-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5];
        targetConn=[0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4];
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(2);
        targetMesh.setName("MyMesh3DSurf");
        targetMesh.allocateCells(5);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[0:4]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[4:7]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[7:10]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[10:14]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[14:18]);
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,9,3);
        targetMesh.setCoords(myCoords);
        myCoords.setInfoOnComponent(0,"X (m)");
        myCoords.setInfoOnComponent(1,"X (dm)");
        myCoords.setInfoOnComponent(2,"X (m)");
        return targetMesh;

    def build0DMesh(self):
        targetCoords=[-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5];
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(0);
        targetMesh.allocateCells(8);
        targetMesh.setName("Wonderfull 0D mesh");
        targetConn=[]
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.insertNextCell(NORM_POINT0,0,targetConn);
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,9,3);
        targetMesh.setCoords(myCoords);
        myCoords.setInfoOnComponent(0,"X (m)");
        myCoords.setInfoOnComponent(1,"YY (Pm)");
        myCoords.setInfoOnComponent(2,"ZZZ (m)");
        targetMesh.checkCoherency();
        return targetMesh;

    def buildM1DMesh(self):
        meshM1D=MEDCouplingUMesh.New("wonderfull -1 D mesh",-1);
        meshM1D.checkCoherency();
        return meshM1D;

    def buildExtrudedMesh(self):
        m2D=self.build2DMesh();
        m2D.changeSpaceDimension(3);
        m1D=self.build1DMesh();
        retu=m2D.buildExtrudedMeshFromThis(m1D,0);
        ret=MEDCouplingExtrudedMesh.New(retu,m2D,2);
        ret.setName("ExtrudedTestForCorbaTest");
        return ret;

    def buildFieldScalarOn2DNT(self):
        mesh=self.build2DMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME);
        fieldOnCells.setName("toto");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*6*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),6);
        fieldOnCells.setArray(array)
        fieldOnCells.checkCoherency();
        return fieldOnCells;

    def buildFieldNodeScalarOn2DNT(self):
        mesh=self.build2DMesh();
        fieldOnNodes=MEDCouplingFieldDouble.New(ON_NODES,NO_TIME);
        fieldOnNodes.setName("toto2");
        fieldOnNodes.setDescription("my wonderful field toto2");
        fieldOnNodes.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfNodes()*5*[7.1234]
        array.setValues(tmp,mesh.getNumberOfNodes(),5);
        fieldOnNodes.setArray(array);
        fieldOnNodes.checkCoherency();
        return fieldOnNodes;

    def buildFieldScalarOn3DNT(self):
        mesh=self.build3DMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME);
        fieldOnCells.setNature(ConservativeVolumic);
        fieldOnCells.setName("toto");
        fieldOnCells.setDescription("my wonderful 3D field toto2");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*6*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),6);
        fieldOnCells.setArray(array);
        fieldOnCells.checkCoherency();
        return fieldOnCells;

    def buildFieldScalarOn3DSurfWT(self):
        mesh=self.build3DSurfMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        fieldOnCells.setName("toto25");
        fieldOnCells.setDescription("my wonderful 3D surf field toto25");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*3*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),3);
        fieldOnCells.setArray(array);
        fieldOnCells.setTime(6.7,1,4);
        fieldOnCells.checkCoherency();
        return fieldOnCells;

    def buildFieldScalarOn3DSurfCOTI(self):
        mesh=self.build3DSurfMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,CONST_ON_TIME_INTERVAL);
        fieldOnCells.setName("toto26");
        fieldOnCells.setDescription("my wonderful 3D surf field toto26");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*3*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),3);
        fieldOnCells.setArray(array);
        fieldOnCells.setStartTime(6.7,1,4);
        fieldOnCells.setEndTime(7.2,2,8);
        fieldOnCells.checkCoherency();
        return fieldOnCells;

    def buildFieldScalarOn2DLT(self):
        mesh=self.build2DMesh()
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,LINEAR_TIME)
        fieldOnCells.setName("toto27");
        fieldOnCells.setDescription("my wonderful 2D field toto27");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        arr1=[1.2,1.02,1.002,1.0002, 3.4,3.04,3.004,3.0004, 5.6,5.06,5.006,5.0006, 7.8,7.08,7.008,7.0008, 9.1,9.01,9.001,9.0001]
        array.setValues(arr1,mesh.getNumberOfCells(),4);
        fieldOnCells.setArray(array);
        array=DataArrayDouble.New();
        arr2=[71.2,71.02,71.002,71.0002, 73.4,73.04,73.004,73.0004, 75.6,75.06,75.006,75.0006, 77.8,77.08,77.008,77.0008, 79.1,79.01,79.001,79.0001]
        array.setValues(arr2,mesh.getNumberOfCells(),4);
        fieldOnCells.setEndArray(array)
        fieldOnCells.setStartTime(6.7,25,26);
        fieldOnCells.setEndTime(17.2,125,126);
        fieldOnCells.checkCoherency();
        return fieldOnCells;

    def buildFieldGaussPt2DWT(self):
        _a=0.446948490915965;
        _b=0.091576213509771;
        _p1=0.11169079483905;
        _p2=0.0549758718227661;
        refCoo1=[ 0.,0., 1.,0., 0.,1. ]
        gsCoo1=[ 2*_b-1, 1-4*_b, 2*_b-1, 2.07*_b-1, 1-4*_b,
                 2*_b-1, 1-4*_a, 2*_a-1, 2*_a-1, 1-4*_a, 2*_a-1, 2*_a-1]
        wg1=[ 4*_p2, 4*_p2, 4*_p2, 4*_p1, 4*_p1, 4*_p1 ]
        _refCoo1=refCoo1
        _gsCoo1=gsCoo1
        _wg1=wg1
        m=self.build2DMesh();
        f=MEDCouplingFieldDouble.New(ON_GAUSS_PT,ONE_TIME);
        f.setTime(6.7,1,4);
        f.setMesh(m);
        f.setGaussLocalizationOnType(NORM_TRI3,_refCoo1,_gsCoo1,_wg1);
        refCoo2=[ 0.,0., 1.,0., 1.,1., 0.,1. ]
        _refCoo2=refCoo2
        _gsCoo1=_gsCoo1[0:4]
        _wg1=_wg1[0:2]
        f.setGaussLocalizationOnType(NORM_QUAD4,_refCoo2,_gsCoo1,_wg1);
        array=DataArrayDouble.New();
        ptr=18*2*[None]
        for i in xrange(18*2):
            ptr[i]=float(i+1);
            pass
        array.setValues(ptr,18,2);
        array.setInfoOnComponent(0,"Power(MW)");
        array.setInfoOnComponent(1,"Density (kg/m^3)");
        f.setArray(array);
        f.setName("MyFirstFieldOnGaussPoint");
        f.setDescription("mmmmmmmmmmmm");
        f.checkCoherency();
        return f;

    def buildFieldGaussPtNE2DWT(self):
        m=self.build2DMesh();
        f=MEDCouplingFieldDouble.New(ON_GAUSS_NE,ONE_TIME);
        f.setTime(6.8,11,8);
        f.setMesh(m);
        f.setName("MyFirstFieldOnNE");
        f.setDescription("MyDescriptionNE");
        array=DataArrayDouble.New();
        ptr=18*2*[None]
        for i in xrange(18*2):
            ptr[i]=float(i+7)
        array.setValues(ptr,18,2);
        array.setInfoOnComponent(0,"Power(MW)");
        array.setInfoOnComponent(1,"Density (kg/m^3)");
        f.setArray(array);
        #
        f.checkCoherency();
        return f;

    def buildFileNameForIOR(self):
        ret=os.getenv("TMP");
        ret+="/entryPointMEDCouplingCorba.ior";
        return ret;
    pass

def testMesh():
    tab4=[1, 2, 8, 7, 2, 3, 9, 8, 3,
          4, 10, 9, 4, 5, 11, 10, 5,
          0, 6, 11, 0, 1, 7, 6 ]
    nbOfNodes=12
    nbOfCells=6
    coords=[ 0.024155, 0.04183768725682622, -0.305, 0.04831000000000001, -1.015761910347357e-17,
             -0.305, 0.09662000000000001, -1.832979297858306e-18, -0.305, 0.120775, 0.04183768725682623,
             -0.305, 0.09662000000000001, 0.08367537451365245, -0.305, 0.04831000000000001, 0.08367537451365246,
             -0.305, 0.024155, 0.04183768725682622, -0.2863, 0.04831000000000001, -1.015761910347357e-17, -0.2863, 
             0.09662000000000001, -1.832979297858306e-18, -0.2863, 0.120775, 0.04183768725682623, -0.2863, 0.09662000000000001,
             0.08367537451365245, -0.2863, 0.04831000000000001, 0.08367537451365246, -0.2863 ]
    mesh=MEDCouplingUMesh.New()
    mesh.setMeshDimension(2)
    mesh.allocateCells(8);
    mesh.setName("mesh1")
    for i in range(nbOfCells):
        mesh.insertNextCell(NORM_QUAD4,4,tab4[4*i:4*(i+1)]);
        pass
    mesh.finishInsertingCells()
    myCoords=DataArrayDouble.New()
    myCoords.setValues(coords,nbOfNodes,3);
    mesh.setCoords(myCoords);
    mesh.checkCoherency();
    myFalseConn=DataArrayInt.New()
    myFalseConn.setValues(tab4,6,4)
    return mesh

def testField():
    #
    mesh=testMesh()
    nbOfCells=mesh.getNumberOfCells()
    field=MEDCouplingFieldDouble.New(ON_CELLS)
    field.setMesh(mesh)
    field.setNature(Integral)
    myCoords=DataArrayDouble.New()
    sampleTab=[]
    for i in range(nbOfCells*9):
        sampleTab.append(float(i))
        myCoords.setValues(sampleTab,nbOfCells,9);
        pass
    field.setArray(myCoords)
    return field

