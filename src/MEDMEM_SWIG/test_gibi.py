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

###################################################################################
###################################################################################
#
from libMEDMEM_Swig import *
from random import *
import string
import os
#
#befor running this script, please be sure about the path the file fileName
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")
gibiFile = "elle_2D_QT_2x2.sauve"
mesh1Name = string.split(gibiFile,".")[0]+"_fromGibiDriver"

gibiFile = os.path.join(filePath, gibiFile)
medFile = string.split(gibiFile,".")[0]+"_fromGibiDriver.med"
vtkFile = string.split(gibiFile,".")[0]+"_fromGibiDriver.vtk"
mesh2Name = mesh1Name

mesh = MESH()
meshDriver = GIBI_MESH_RDONLY_DRIVER(gibiFile,mesh)

meshDriver.open()
meshDriver.read()
meshDriver.close()

#print mesh

meshName = mesh.getName()
spaceDim = mesh.getSpaceDimension()
meshDim = mesh.getMeshDimension()
print "The mesh ",meshName," is a ",spaceDim,"D mesh on a ",meshDim,"D geometry"
print "and is mounted via the GIBI driver"
nbNodes = mesh.getNumberOfNodes()
print "The mesh ",meshName," has",nbNodes,"Nodes"
coordSyst = mesh.getCoordinatesSystem()
print "The coordinates system is",coordSyst
print "The Coordinates :"
coordNames = []
coordUnits = []
for isd in range(spaceDim):
    coordNames.append(mesh.getCoordinateName(isd))
    coordUnits.append(mesh.getCoordinateUnit(isd))

print "names:", coordNames
print "units", coordUnits
print "values:"
coordinates = mesh.getCoordinates(MED_FULL_INTERLACE)
for k in range(nbNodes):
    kp1 = k+1
    coords = []
    for isd in range(spaceDim):
        isdp1 = isd+1
        coords.append(mesh.getCoordinate(kp1,isdp1))

    print coords," ---- ", coordinates[k*spaceDim:((k+1)*spaceDim)]

print ""
print "Show the Nodal Connectivity:"
nbTypesCell = mesh.getNumberOfTypes(MED_CELL)
print ""
if (nbTypesCell>0):
    print "The Mesh has",nbTypesCell,"Type(s) of Cell"
    types = mesh.getTypes(MED_CELL)
    for k in range(nbTypesCell):
        type = types[k]
        nbElemType = mesh.getNumberOfElements(MED_CELL,type)
        print "For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
        connectivity = mesh.getConnectivity(MED_NODAL,MED_CELL,type)
        nbNodesPerCell = type%100
        for j in range(nbElemType):
            print "Element",(j+1)," ",connectivity[j*nbNodesPerCell:(j+1)*nbNodesPerCell]

print ""
print "Show the Reverse Nodal Connectivity:"
ReverseConnectivity = mesh.getReverseConnectivity(MED_NODAL)
ReverseConnectivityIndex = mesh.getReverseConnectivityIndex(MED_NODAL)
print ""
for j in range(nbNodes):
    begin = ReverseConnectivityIndex[j]-1
    end = ReverseConnectivityIndex[j+1]-1
    print "Node",(j+1),"-->",ReverseConnectivity[begin:end]

print ""
print "Show the Descending Connectivity:"
mesh.calculateConnectivity(MED_DESCENDING,MED_CELL)
nbElemts = mesh.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS)
Connectivity = mesh.getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS)
ConnectivityIndex = mesh.getConnectivityIndex(MED_DESCENDING,MED_CELL)
print ""
for j in range(nbElemts):
    begin = ConnectivityIndex[j]-1
    end = ConnectivityIndex[j+1]-1
    print "Element",(j+1),"-->",Connectivity[begin:end]

print ""
for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
    nbFam = mesh.getNumberOfFamilies(entity)
    nbGrp = mesh.getNumberOfGroups(entity)
    if (entity == MED_NODE) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Node Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Node Group(s)"
    elif (entity == MED_CELL) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Cell Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Cell Group(s)"
    elif (entity == MED_FACE) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Face Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Face Group(s)"
    elif (entity == MED_EDGE) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Edge Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Edge Group(s)"

for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
    nbFam = mesh.getNumberOfFamilies(entity)
    nbGrp = mesh.getNumberOfGroups(entity)
    if nbFam > 0:
        for j in range(nbFam):
            print ""
            family = mesh.getFamily(entity,j+1)
            familyName = family.getName()
            familyDescription = family.getDescription()
            familyEntity = family.getEntity()
            familyBool = family.isOnAllElements()
            print "  -Name:",familyName
            print "  -Description:",familyDescription
            print "  -Entity:",familyEntity
            familyIdentifier = family.getIdentifier()
            nbOfAtt = family.getNumberOfAttributes()
            print "  -Identifier:",familyIdentifier
            print "  -Number Of Attributes:",nbOfAtt
            attributesids = family.getAttributesIdentifiers()
            attributesvals = family.getAttributesValues()
            for k in range(nbOfAtt):
                print "    * Attributes:",attributesids[k],":",attributesvals[k],",",family.getAttributeDescription(k+1)
            nbOfGrp = family.getNumberOfGroups()
            print "  -Number Of Groups:",nbOfGrp
            for k in range(nbOfGrp):
                print "    * Group:",family.getGroupName(k+1)
            print "  -Entities list:"
            if (familyBool):
                print "  -Is on all entities"
            else:
                nbOfTypes = family.getNumberOfTypes()
                types = family.getTypes()
                print "  -Number Of Types:",nbOfTypes
                for k in range(nbOfTypes):
                    type = types[k]
                    nbOfElmtsOfType = family.getNumberOfElements(type)
                    number = family.getNumber(type)
                    print "    * Type",type
                    print "    * Number",number[0:nbOfElmtsOfType]
                print ""
    print ""

    if nbGrp > 0:
        for j in range(nbGrp):
            print ""
            group = mesh.getGroup(entity,j+1)
            groupName = group.getName()
            groupDescription = group.getDescription()
            groupEntity = group.getEntity()
            groupBool = group.isOnAllElements()
            print "  -Name:",groupName
            print "  -Description:",groupDescription
            print "  -Entity:",groupEntity
            nbOfFam = group.getNumberOfFamilies()
            print "  -Number Of Families:",nbOfFam
            for k in range(nbOfFam):
                print "    * Family:",group.getFamily(k+1).getName()
            print "  -Entities list:"
            if (groupBool):
                print "  -Is on all entities"
            else:
                nbOfTypes = group.getNumberOfTypes()
                types = group.getTypes()
                print "  -Number Of Types:",nbOfTypes
                for k in range(nbOfTypes):
                    type = types[k]
                    nbOfElmtsOfType = group.getNumberOfElements(type)
                    number = group.getNumber(type)
                    print "    * Type",type
                    print "    * Number",number[0:nbOfElmtsOfType]
                print ""
        print ""

print "Printing of this mesh in med and vtk format with the name ", mesh.getName()

idMed = mesh.addDriver(MED_DRIVER,medFile,mesh1Name)
mesh.write(idMed)

idVtk = mesh.addDriver(VTK_DRIVER,vtkFile,mesh2Name)
mesh.write(idVtk)
print ""
print "Mounting this mesh using the MED driver"
print ""

mesh1 = MESH()
mesh1Driver = MED_MESH_RDONLY_DRIVER(medFile,mesh1)

mesh1Driver.setMeshName(mesh1Name)

mesh1Driver.open()
mesh1Driver.read()
mesh1Driver.close()

#print mesh

mesh_name = mesh1Driver.getMeshName()
meshName = mesh1.getName()
spaceDim = mesh1.getSpaceDimension()
meshDim = mesh1.getMeshDimension()
print "The mesh ",meshName," is a ",spaceDim,"D mesh on a ",meshDim,"D geometry"
nbNodes = mesh1.getNumberOfNodes()
print "The mesh has",nbNodes,"Nodes"
coordSyst = mesh1.getCoordinatesSystem()
print "The coordinates system is",coordSyst
print "The Coordinates :"
coordNames = []
coordUnits = []
for isd in range(spaceDim):
    coordNames.append(mesh1.getCoordinateName(isd))
    coordUnits.append(mesh1.getCoordinateUnit(isd))

print "names:", coordNames
print "units", coordUnits
print "values:"
coordinates = mesh1.getCoordinates(MED_FULL_INTERLACE)
for k in range(nbNodes):
    kp1 = k+1
    coords = []
    for isd in range(spaceDim):
        isdp1 = isd+1
        coords.append(mesh1.getCoordinate(kp1,isdp1))

    print coords," ---- ", coordinates[k*spaceDim:((k+1)*spaceDim)]

print ""
print "Show the Nodal Connectivity:"
nbTypesCell = mesh1.getNumberOfTypes(MED_CELL)
print ""
if (nbTypesCell>0):
    print "The Mesh has",nbTypesCell,"Type(s) of Cell"
    types = mesh1.getTypes(MED_CELL)
    for k in range(nbTypesCell):
        type = types[k]
        nbElemType = mesh1.getNumberOfElements(MED_CELL,type)
        print "For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
        connectivity = mesh1.getConnectivity(MED_NODAL,MED_CELL,type)
        nbNodesPerCell = type%100
        for j in range(nbElemType):
            print "Element",(j+1)," ",connectivity[j*nbNodesPerCell:(j+1)*nbNodesPerCell]

print ""
print "Show the Reverse Nodal Connectivity:"
ReverseConnectivity = mesh1.getReverseConnectivity(MED_NODAL)
ReverseConnectivityIndex = mesh1.getReverseConnectivityIndex(MED_NODAL)
print ""
for j in range(nbNodes):
    begin = ReverseConnectivityIndex[j]-1
    end = ReverseConnectivityIndex[j+1]-1
    print "Node",(j+1),"-->",ReverseConnectivity[begin:end]

print ""
print "Show the Descending Connectivity:"
mesh1.calculateConnectivity(MED_DESCENDING,MED_CELL)
nbElemts = mesh1.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS)
Connectivity = mesh1.getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS)
ConnectivityIndex = mesh1.getConnectivityIndex(MED_DESCENDING,MED_CELL)
print ""
for j in range(nbElemts):
    begin = ConnectivityIndex[j]-1
    end = ConnectivityIndex[j+1]-1
    print "Element",(j+1),"-->",Connectivity[begin:end]

print ""
for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
    nbFam = mesh1.getNumberOfFamilies(entity)
    nbGrp = mesh1.getNumberOfGroups(entity)
    if (entity == MED_NODE) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Node Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Node Group(s)"
    elif (entity == MED_CELL) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Cell Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Cell Group(s)"
    elif (entity == MED_FACE) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Face Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Face Group(s)"
    elif (entity == MED_EDGE) :
        if (nbFam > 0) : print "This mesh has",nbFam,"Edge Family(ies)"
        if (nbGrp > 0) : print "This mesh has",nbGrp,"Edge Group(s)"

for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
    nbFam = mesh1.getNumberOfFamilies(entity)
    nbGrp = mesh1.getNumberOfGroups(entity)
    if nbFam > 0:
        for j in range(nbFam):
            print ""
            family = mesh1.getFamily(entity,j+1)
            familyName = family.getName()
            familyDescription = family.getDescription()
            familyEntity = family.getEntity()
            familyBool = family.isOnAllElements()
            print "  -Name:",familyName
            print "  -Description:",familyDescription
            print "  -Entity:",familyEntity
            familyIdentifier = family.getIdentifier()
            nbOfAtt = family.getNumberOfAttributes()
            print "  -Identifier:",familyIdentifier
            print "  -Number Of Attributes:",nbOfAtt
            attributesids = family.getAttributesIdentifiers()
            attributesvals = family.getAttributesValues()
            for k in range(nbOfAtt):
                print "    * Attributes:",attributesids[k],":",attributesvals[k],",",family.getAttributeDescription(k+1)
            nbOfGrp = family.getNumberOfGroups()
            print "  -Number Of Groups:",nbOfGrp
            for k in range(nbOfGrp):
                print "    * Group:",family.getGroupName(k+1)
            print "  -Entities list:"
            if (familyBool):
                print "  -Is on all entities"
            else:
                nbOfTypes = family.getNumberOfTypes()
                types = family.getTypes()
                print "  -Number Of Types:",nbOfTypes
                for k in range(nbOfTypes):
                    type = types[k]
                    nbOfElmtsOfType = family.getNumberOfElements(type)
                    number = family.getNumber(type)
                    print "    * Type",type
                    print "    * Number",number[0:nbOfElmtsOfType]
                print ""
    print ""

    if nbGrp > 0:
        for j in range(nbGrp):
            print ""
            group = mesh1.getGroup(entity,j+1)
            groupName = group.getName()
            groupDescription = group.getDescription()
            groupEntity = group.getEntity()
            groupBool = group.isOnAllElements()
            print "  -Name:",groupName
            print "  -Description:",groupDescription
            print "  -Entity:",groupEntity
            nbOfFam = group.getNumberOfFamilies()
            print "  -Number Of Families:",nbOfFam
            for k in range(nbOfFam):
                print "    * Family:",group.getFamily(k+1).getName()
            print "  -Entities list:"
            if (groupBool):
                print "  -Is on all entities"
            else:
                nbOfTypes = group.getNumberOfTypes()
                types = group.getTypes()
                print "  -Number Of Types:",nbOfTypes
                for k in range(nbOfTypes):
                    type = types[k]
                    nbOfElmtsOfType = group.getNumberOfElements(type)
                    number = group.getNumber(type)
                    print "    * Type",type
                    print "    * Number",number[0:nbOfElmtsOfType]
                print ""
        print ""

print "END of the Pyhton script ..... Ctrl D to exit"
