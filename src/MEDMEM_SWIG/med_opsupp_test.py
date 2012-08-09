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

############################################################################
# This Python script is testing the merge and the intersection of
# several SUPPORTs
############################################################################
#
from libMEDMEM_Swig import *
from random import *
import os
#
#befor running this script, please be sure about the path the file fileName
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFile = os.path.join( filePath, "pointe.med" )

def print_ord(i):
    if i == 0:
        return 'first'
    elif i == 1:
        return 'second'
    elif i == 2:
        return 'third'
    else:
        return `i`+'th'

md = MEDFILEBROWSER(medFile)
nbMeshes = md.getNumberOfMeshes()

print "The med file", medFile, "contains", nbMeshes, "mesh(es)"

mesh_name = md.getMeshName(0)
mesh = MESH(MED_DRIVER,medFile,mesh_name)
spaceDim = mesh.getSpaceDimension()
meshDim = mesh.getMeshDimension()
nbNodes = mesh.getNumberOfNodes()
print "The mesh",mesh_name,"is a",spaceDim,"D mesh on a",meshDim,"D geometry and has",nbNodes,"Nodes"

print ""
print "Mesh Families analysis:"
print ""

for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
    nbFam = mesh.getNumberOfFamilies(entity)
    if (entity == MED_NODE) & (nbFam > 0):
        print "This mesh has",nbFam,"Node Family(ies)"
    elif (entity == MED_NODE) & (nbFam == 0):
        print "This mesh has no Node Family(ies)"
    elif (entity == MED_CELL) & (nbFam > 0):
        print "This mesh has",nbFam,"Cell Family(ies)"
    elif (entity == MED_CELL) & (nbFam == 0):
        print "This mesh has no Cell Family(ies)"
    elif (entity == MED_FACE) & (nbFam > 0):
        print "This mesh has",nbFam,"Face Family(ies)"
    elif (entity == MED_FACE) & (nbFam == 0):
        print "This mesh has no Face Family(ies)"
    elif (entity == MED_EDGE) & (nbFam > 0):
        print "This mesh has",nbFam,"Edge Family(ies)"
    elif (entity == MED_EDGE) & (nbFam == 0):
        print "This mesh has no Edge Family(ies)"
    print ""
    
    if nbFam > 0:
        for j in range(nbFam):
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

print "we are going to do some operation on Node families"
print "by definition families have no intersection"
print ""
entity = MED_NODE
familyNode1 = mesh.getFamily(entity,1)
familyNode2 = mesh.getFamily(entity,2)
familyNode3 = mesh.getFamily(entity,3)
familyNode4 = mesh.getFamily(entity,4)

listOfSupports = []
listOfSupports.append(familyNode1)
listOfSupports.append(familyNode2)

support = mesh.mergeSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

support = mesh.intersectSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

listOfSupports.append(familyNode3)

support = mesh.mergeSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

print "we are going to do some operation on Cell families"
print "by definition families have no intersection"
print ""
entity = MED_CELL
familyCell1 = mesh.getFamily(entity,1)
familyCell2 = mesh.getFamily(entity,2)
familyCell3 = mesh.getFamily(entity,3)

listOfSupports = []
listOfSupports.append(familyCell1)
listOfSupports.append(familyCell2)

support = mesh.mergeSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

support = mesh.intersectSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

listOfSupports.append(familyCell3)

support = mesh.mergeSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

print "Let's now do an example of union and intersection of two supports with a non empty intersection"
print ""
support1 = SUPPORT()
support1.setMesh(mesh)
support1.setName("Partial_Support_1")
support1Description = "Partial Support 1"
support1NbOfTypes = 3
support1TotNbElm = 8
support1GeoType = [MED_TETRA4,MED_HEXA8,MED_PYRA5]
support1NbOfElm = [4,2,2]
support1NbValue = [1,2,3,4,15,16,13,14]

support1.setpartial(support1Description,support1NbOfTypes,support1TotNbElm,
                    support1GeoType,support1NbOfElm,support1NbValue)

supportName = support1.getName()
supportDescription = support1.getDescription()
supportMeshname = support1.getMesh().getName()
supportEntity = support1.getEntity()
supportBool = support1.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support1.getNumberOfTypes()
    types = support1.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support1.getNumberOfElements(type)
        number = support1.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

support2 = SUPPORT()
support2.setMesh(mesh)
support2.setName("Partial_Support_2")
support2Description = "Partial Support 2"
support2NbOfTypes = 3
support2TotNbElm = 8
support2GeoType = [MED_TETRA4,MED_HEXA8,MED_PYRA5]
support2NbOfElm = [6,1,1]
support2NbValue = [3,4,9,10,11,12,16,14]

support2.setpartial(support2Description,support2NbOfTypes,support2TotNbElm,
                    support2GeoType,support2NbOfElm,support2NbValue)

supportName = support2.getName()
supportDescription = support2.getDescription()
supportMeshname = support2.getMesh().getName()
supportEntity = support2.getEntity()
supportBool = support2.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support2.getNumberOfTypes()
    types = support2.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support2.getNumberOfElements(type)
        number = support2.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

listOfSupports = []
listOfSupports.append(support1)
listOfSupports.append(support2)

print "The union"
print ""

support = mesh.mergeSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

print "The intersection"
print ""

support = mesh.intersectSupports(listOfSupports)

supportName = support.getName()
supportDescription = support.getDescription()
supportMeshname = support.getMesh().getName()
supportEntity = support.getEntity()
supportBool = support.isOnAllElements()
print "  -Name:",supportName
print "  -Description:",supportDescription
print "  -Entity:",supportEntity
if (supportBool):
    print "  -Is on all entities"
else:
    nbOfTypes = support.getNumberOfTypes()
    types = support.getTypes()
    print "  -Number Of Types:",nbOfTypes
    for k in range(nbOfTypes):
        type = types[k]
        nbOfElmtsOfType = support.getNumberOfElements(type)
        number = support.getNumber(type)
        print "    * Type",type
        print "    * Number",number[0:nbOfElmtsOfType]
    print ""

print "END of the Pyhton script ..... Ctrl D to exit"
