#  MED MEDMEM_SWIG : binding of C++ implementation and Python
#
#  Copyright (C) 2003  CEA/DEN, EDF R&D
#
#
#
#  File   : medMeshing_test.py
#  Module : MED

from libMEDMEM_Swig import *

# file name to save the generated MESH(ING)

medFileName = "toto.med"

vtkFileName = "toto.vtk"

myMeshing = MESHING()

myMeshing.setName("meshing")

# definition of the coordinates

spaceDimension = 3

numberOfNodes = 19

coordinates = []

coordinate = [0.0, 0.0, 0.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [0.0, 0.0, 1.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [2.0, 0.0, 1.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [0.0, 2.0, 1.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [-2.0, 0.0, 1.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [0.0, -2.0, 1.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [1.0, 1.0, 2.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [-1.0, 1.0, 2.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [-1.0, -1.0, 2.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [1.0, -1.0, 2.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [1.0, 1.0, 3.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [-1.0, 1.0, 3.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [-1.0, -1.0, 3.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [1.0, -1.0, 3.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [1.0, 1.0, 4.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [-1.0, 1.0, 4.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [-1.0, -1.0, 4.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [1.0, -1.0, 4.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])
coordinate = [0.0, 0.0, 5.0]
coordinates.append(coordinate[0])
coordinates.append(coordinate[1])
coordinates.append(coordinate[2])

myMeshing.setCoordinates(spaceDimension,numberOfNodes,coordinates,"CARTESIAN",MED_FULL_INTERLACE)

for i in range(spaceDimension):
    unit = "cm      "
    if (i == 0):
        name = "X       "
    elif (i == 1):
        name = "Y       "
    elif (i == 2):
        name = "Z       "

    myMeshing.setCoordinateName(name,i)
    myMeshing.setCoordinateUnit(unit,i)

# definition of connectivities
# cell part

numberOfTypes = 3
entity = MED_CELL

types = []
numberOfElements = []

types.append(MED_TETRA4)
numberOfElements.append(12)

types.append(MED_PYRA5)
numberOfElements.append(2)

types.append(MED_HEXA8)
numberOfElements.append(2)

myMeshing.setNumberOfTypes(numberOfTypes,entity)
myMeshing.setTypes(types,entity)
myMeshing.setNumberOfElements(numberOfElements,entity)

connectivityTetra = []

connectivity =  [1,2,3,6]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [1,2,4,3]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [1,2,5,4]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [1,2,6,5]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,7,4,3]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,8,5,4]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,9,6,5]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,10,3,6]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,7,3,10]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,8,4,7]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,9,5,8]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])
connectivity =  [2,10,6,9]
connectivityTetra.append(connectivity[0])
connectivityTetra.append(connectivity[1])
connectivityTetra.append(connectivity[2])
connectivityTetra.append(connectivity[3])

myMeshing.setConnectivity(connectivityTetra,entity,types[0])

connectivityPyra = []
connectivity =  [7,8,9,10,2]
connectivityPyra.append(connectivity[0])
connectivityPyra.append(connectivity[1])
connectivityPyra.append(connectivity[2])
connectivityPyra.append(connectivity[3])
connectivityPyra.append(connectivity[4])
connectivity =  [15,18,17,16,19]
connectivityPyra.append(connectivity[0])
connectivityPyra.append(connectivity[1])
connectivityPyra.append(connectivity[2])
connectivityPyra.append(connectivity[3])
connectivityPyra.append(connectivity[4])

myMeshing.setConnectivity(connectivityPyra,entity,types[1])

connectivityHexa = []
connectivity =  [11,12,13,14,7,8,9,10]
connectivityHexa.append(connectivity[0])
connectivityHexa.append(connectivity[1])
connectivityHexa.append(connectivity[2])
connectivityHexa.append(connectivity[3])
connectivityHexa.append(connectivity[4])
connectivityHexa.append(connectivity[5])
connectivityHexa.append(connectivity[6])
connectivityHexa.append(connectivity[7])
connectivity =  [15,16,17,18,11,12,13,14]
connectivityHexa.append(connectivity[0])
connectivityHexa.append(connectivity[1])
connectivityHexa.append(connectivity[2])
connectivityHexa.append(connectivity[3])
connectivityHexa.append(connectivity[4])
connectivityHexa.append(connectivity[5])
connectivityHexa.append(connectivity[6])
connectivityHexa.append(connectivity[7])

myMeshing.setConnectivity(connectivityHexa,entity,types[2])

# face part

# edge part

# saving of the generated mesh in MED and VTK format

idMed = myMeshing.addDriver(MED_DRIVER,medFileName,myMeshing.getName())
myMeshing.write(idMed)

idVtk = myMeshing.addDriver(MED_DRIVER,vtkFileName,myMeshing.getName())
myMeshing.write(idVtk)
