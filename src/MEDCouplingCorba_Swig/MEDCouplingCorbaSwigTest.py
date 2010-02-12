from libMEDCoupling_Swig import *

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

FileIOR1="toto1.ior"
FileIOR2="toto2.ior"
