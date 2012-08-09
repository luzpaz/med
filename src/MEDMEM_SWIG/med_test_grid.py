#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

#    Python script for testing T5.10 task
# ###########################################
#% Test structured mesh (GRID) reading from file test19.med
#% test19.med can be obtained by running test19_c.out executable
# or locally in MEDMEM directory create_grid
#
from libMEDMEM_Swig import *

import os
#
#befor running this script, please be sure about the path the file fileName
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFile = os.path.join(filePath, "test19.med")
print "Read file", medFile

md = MEDFILEBROWSER(medFile)

nbMeshes = md.getNumberOfMeshes()
print "Nb meshes:", nbMeshes
if nbMeshes == 0:
  raise  RuntimeError, "Can't read med file"

##############################
# test "CartGrid"
##############################

mesh_name = md.getMeshName(0)
print "Read", mesh_name
grid = GRID(MED_DRIVER,medFile,mesh_name)

if grid.getIsAGrid() == 0:
  raise  RuntimeError, "Mesh 0 is not a grid"

I = grid.getArrayLength(1)
print "_iArrayLength", I
if I != 4:
  raise  RuntimeError, "Error in grid.getArrayLength(1)"

J = grid.getArrayLength(2)
print "_jArrayLength", J
if J != 4:
  raise  RuntimeError, "Error in grid.getArrayLength(2)"

K = grid.getArrayLength(3)
print "_kArrayLength", K
if K != 0:
  raise  RuntimeError, "Error in grid.getArrayLength(3)"

grid_type = grid.getGridType()
print "grid_type =", grid_type
if grid_type != MED_CARTESIAN:
  raise  RuntimeError, "Wrong grid type"

spaceDim = grid.getSpaceDimension()
print "spaceDim =", spaceDim
if spaceDim != 2:
  raise  RuntimeError, "Wrong space dimention"

meshDim = grid.getMeshDimension()
print "meshDim =", meshDim
if spaceDim != meshDim:
  raise  RuntimeError, "Wrong mesh dimention"

nbNodes = grid.getNumberOfNodes()
print "nbNodes =", nbNodes
if nbNodes != 16:
  raise  RuntimeError, "Wrong nb of nodes"

coordSyst = grid.getCoordinatesSystem()
print "coordSyst =", coordSyst
if coordSyst != "CARTESIAN":
  raise  RuntimeError, "Wrong coordinates system"

mesh = grid.convertInMESH()
coordinates = mesh.getCoordinates(MED_FULL_INTERLACE)
print "coordinates =", coordinates
xy = [ grid.getArrayValue(1, I-1 ) , grid.getArrayValue(2, J-1 ) ]
xy2 = coordinates[(nbNodes-1)*spaceDim:(nbNodes*spaceDim)]
print list(xy)
print xy2
if not list(xy) == list(xy2):
  raise  RuntimeError, "Error in grid.getCoordinates(MED_FULL_INTERLACE)"

nbTypesCell = grid.getNumberOfTypes(MED_CELL)
print "nbTypesCell =", nbTypesCell
if nbTypesCell != 1:
  raise  RuntimeError, "Wrong nbTypesCell"

types = grid.getTypes(MED_CELL)
print "Cell types =", types
if types != [204]:
  raise  RuntimeError, "Wrong Cell types"
            	
nbElemType = grid.getNumberOfElements(MED_CELL,types[0])
print "Nb cells =", nbElemType
if nbElemType != 9:
  raise  RuntimeError, "Wrong Nb cells"

print "getEntityPosition : convert node number to i,j,k"

n1 = grid.getEntityPosition(MED_NODE,10)
n2 = grid.getEntityPosition(MED_NODE,16)

if n1 == [1,2]: o1 = "Ok"
else:           o1 = "KO"
if n2 == [3,3]: o2 = "Ok"
else:           o2 = "KO"

print "Node 10 -> i,j = ", n1, " : ", o1
print "Node 16 -> i,j = ", n2, " : ", o2

print "getEntityPosition : convert cell number to i,j,k"

n1 = grid.getEntityPosition(MED_CELL,3)
n2 = grid.getEntityPosition(MED_CELL,4)

if n1 == [2,0]: o1 = "Ok"
else:           o1 = "KO"
if n2 == [0,1]: o2 = "Ok"
else:           o2 = "KO"

print "Cell 3 -> i,j = ", n1, " : ", o1
print "Cell 4 -> i,j = ", n2, " : ", o2

print "getEntityPosition : convert face number to i,j,k"

n1 = grid.getEntityPosition(MED_FACE,6)
n2 = grid.getEntityPosition(MED_FACE,8)

if n1 == [1,2,1]: o1 = "Ok"
else:             o1 = "KO"
if n2 == [1,1,2]: o2 = "Ok"
else:             o2 = "KO"

print "Face 6 -> i,j = ", n1, " : ", o1
print "Face 8 -> i,j = ", n2, " : ", o2

print "getNodeNumber : convert i,j,k to node number"

n1 = grid.getNodeNumber(2,1,0)
n2 = grid.getNodeNumber(1,2,0)

if n1 == 7:  o1 = "Ok"
else:        o1 = "KO"
if n2 == 10: o2 = "Ok"
else:        o2 = "KO"

print "getNodeNumber 2,1,0 = ", n1, " : ", o1
print "getNodeNumber 1,2,0 = ", n2, " : ", o2

print "getCellNumber : convert i,j,k to cell number"

n1 = grid.getCellNumber(1,0,0)
n2 = grid.getCellNumber(2,1,0)

if n1 == 2: o1 = "Ok"
else:       o1 = "KO"
if n2 == 6: o2 = "Ok"
else:       o2 = "KO"

print "getCellNumber 1,0,0 = ", n1, " : ", o1
print "getCellNumber 2,1,0 = ", n2, " : ", o2

print "getFaceNumber : convert i,j,k to face number"

n1 = grid.getFaceNumber(0,0,2,0)
n2 = grid.getFaceNumber(0,1,1,0)

if n1 == 7: o1 = "Ok"
else:       o1 = "KO"
if n2 == 5: o2 = "Ok"
else:       o2 = "KO"

print "getFaceNumber 1,0,2,0 = ", n1, " : ", o1
print "getFaceNumber 1,1,1,0 = ", n2, " : ", o2

print "Connectivity"
n1 = grid.getNodeNumber(0,0,0)
n2 = grid.getNodeNumber(0,1,0)
n3 = grid.getNodeNumber(1,1,0)
n4 = grid.getNodeNumber(1,0,0)

conn1 = [n1,n2,n3,n4]

Connectivity = mesh.getConnectivity(MED_NODAL,MED_CELL,types[0])
print "connectivity =", Connectivity

print "Cell 1 nodes:", conn1, Connectivity[0:4]
if list(conn1) != list(Connectivity[0:4]):
  raise  RuntimeError, "Wrong nodal connectivity"


ReverseConnectivity = mesh.getReverseConnectivity(MED_NODAL)
ReverseConnectivityIndex = mesh.getReverseConnectivityIndex(MED_NODAL)
n = 6
beg = ReverseConnectivityIndex[n-1]-1
end = ReverseConnectivityIndex[n]-1
print "Node",n,"rev connectivity =",ReverseConnectivity[beg:end]
if end-beg != 4:
  raise  RuntimeError, "Wrong reverse connectivity"
if ReverseConnectivity[beg] != 1:
  raise  RuntimeError, "Wrong reverse connectivity"

edgfecon = mesh.getConnectivity(MED_NODAL,MED_EDGE,MED_ALL_ELEMENTS)
print edgfecon
print len(edgfecon)

mesh.calculateConnectivity(MED_DESCENDING,MED_CELL)
Connectivity = mesh.getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS)
ConnectivityIndex = mesh.getConnectivityIndex(MED_DESCENDING,MED_CELL)
n = 1
beg = ConnectivityIndex[n-1]-1
end = ConnectivityIndex[n]-1
print "Element",n,"descending connectivity -->",Connectivity[beg:end]
if list(Connectivity[beg:end]) != [13, 4, 14, 1]:
  raise  RuntimeError, "Wrong descending  connectivity"

print "getReverseConnectivity(MED_DESCENDING)", mesh.getReverseConnectivity(MED_DESCENDING)
print "grid.getReverseConnectivityIndex(MED_DESCENDING)",mesh.getReverseConnectivityIndex(MED_DESCENDING)

##############################
# test "bodyfitted"
##############################

mesh_name = md.getMeshName(1)
print "Read", mesh_name
grid = GRID(MED_DRIVER,medFile,mesh_name)

if grid.getIsAGrid() == 0:
  raise  RuntimeError, "Mesh 1 is not a grid"

mesh = grid.convertInMESH()

I = grid.getArrayLength(1)
print "_iArrayLength", I
if I != 2:
  raise  RuntimeError, "Error in grid.getArrayLength(1)"

J = grid.getArrayLength(2)
print "_jArrayLength", J
if J != 2:
  raise  RuntimeError, "Error in grid.getArrayLength(2)"

K = grid.getArrayLength(3)
print "_kArrayLength", K
if K != 0:
  raise  RuntimeError, "Error in grid.getArrayLength(3)"

grid_type = grid.getGridType()
print "grid_type =", grid_type
if grid_type != MED_BODY_FITTED:
  raise  RuntimeError, "Wrong grid type"

spaceDim = grid.getSpaceDimension()
print "spaceDim =", spaceDim
if spaceDim != 2:
  raise  RuntimeError, "Wrong space dimention"

meshDim = grid.getMeshDimension()
print "meshDim =", meshDim
if spaceDim != meshDim:
  raise  RuntimeError, "Wrong mesh dimention"

nbNodes = grid.getNumberOfNodes()
print "nbNodes =", nbNodes
if nbNodes != 4:
  raise  RuntimeError, "Wrong nb of nodes"

coordSyst = grid.getCoordinatesSystem()
print "coordSyst =", coordSyst
if coordSyst != "CARTESIAN":
  raise  RuntimeError, "Wrong coordinates system"

nbTypesCell = grid.getNumberOfTypes(MED_CELL)
print "nbTypesCell =", nbTypesCell
if nbTypesCell != 1:
  raise  RuntimeError, "Wrong nbTypesCell"

types = grid.getTypes(MED_CELL)
print "Cell types =", types
if types != [204]:
  raise  RuntimeError, "Wrong Cell types"
            	
nbElemType = grid.getNumberOfElements(MED_CELL,types[0])
print "Nb cells =", nbElemType
if nbElemType != 1:
  raise  RuntimeError, "Wrong Nb cells"

Connectivity = mesh.getConnectivity(MED_NODAL,MED_CELL,types[0])
print "connectivity =", Connectivity
n1 = grid.getNodeNumber(0,0,0)
n2 = grid.getNodeNumber(0,1,0)
n3 = grid.getNodeNumber(1,1,0)
n4 = grid.getNodeNumber(1,0,0)
conn1 = [n1,n2,n3,n4]
print "Cell 1 nodes:", conn1, Connectivity[0:4]
if conn1 != list(Connectivity[0:4]):
  raise  RuntimeError, "Wrong nodal connectivity"

##############################################
# test "maa1" which in fact is not a pure GRID
##############################################

mesh_name = md.getMeshName(2)

print "Read", mesh_name
mesh = MESH(MED_DRIVER,md.getFileName(),mesh_name)

if mesh.getIsAGrid() == 0:
  print "Mesh ",mesh_name," is not a grid"

print "END of the Pyhton script ..... Ctrl D to exit"
