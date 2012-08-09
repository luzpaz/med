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
# This Python script is parsing a MED file using MED Memory from SALOME platform:
# It analyses all meshes in the MED file (coordinates, connectivity of d-cells as
# well as (d-1)-cells, families). It analyses fields stored in the MED file (d is
# the space dimension). You can assume that it is a kind of MED file parser.
###################################################################################
#
from libMEDMEM_Swig import *
from random import *
import os
#
#befor running this script, please be sure about the path the file fileName
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFile = os.path.join(filePath, "carre_en_quad4_seg2.med")
#medFile = os.path.join(filePath, "cube_hexa8_quad4.med")

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

nbFields = md.getNumberOfFields()

print "The med file", medFile, "contains", nbMeshes, "mesh(es) and", nbFields, "field(s)"

if (nbMeshes>0):
    print "Mesh(es) Name(s) is(are) "

    for i in range(nbMeshes):
        mesh_name = md.getMeshName(i)
        print "   - ",mesh_name

if (nbFields>0):
    print "Field(s) Name(s) is(are) "

    for i in range(nbFields):
        field_name = md.getFieldName(i)
        print "   - ",field_name

print ""

if (nbMeshes>0):
    print "Mesh(es) Analysis "
    for i in range(nbMeshes):
        mesh_name = md.getMeshName(i)
        mesh = MESH(MED_DRIVER,md.getFileName(),mesh_name)
        spaceDim = mesh.getSpaceDimension()
        meshDim = mesh.getMeshDimension()
        print "The",print_ord(i), "mesh, '",mesh_name,"', is a",spaceDim,"D mesh on a",meshDim,"D geometry"
        nbNodes = mesh.getNumberOfNodes()
        print "The mesh has",nbNodes,"Nodes"
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
        print "Show the Nodal Connectivity of Cells:"
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
        print "Show the Nodal Connectivity of constituents (Faces in 3D or Edges in 2D):"

        if (spaceDim == 3) : constituent = MED_FACE
        elif (spaceDim == 2) : constituent = MED_EDGE
        else : print "proble with the sapce dimension : ",spaceDim

        nbTypesConst = mesh.getNumberOfTypes(constituent)
        print ""
        if (nbTypesConst>0):
            print "The Mesh has",nbTypesConst,"Type(s) of Constituent"
            types = mesh.getTypes(constituent)
            for k in range(nbTypesConst):
                type = types[k]
                nbElemType = mesh.getNumberOfElements(constituent,type)
                print "For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
                connectivity = mesh.getConnectivity(MED_NODAL,constituent,type)
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
            if (entity == MED_NODE) & (nbFam > 0):
                print "This mesh has",nbFam,"Node Family(ies)"
            elif (entity == MED_CELL) & (nbFam > 0):
                print "This mesh has",nbFam,"Cell Family(ies)"
            elif (entity == MED_FACE) & (nbFam > 0):
                print "This mesh has",nbFam,"Face Family(ies)"
            elif (entity == MED_EDGE) & (nbFam > 0):
                print "This mesh has",nbFam,"Edge Family(ies)"

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
                        numberFamily = family.getNumber(MED_ALL_ELEMENTS)
                        print "    * Getting an Integer Field on the family ",familyName
                        fieldFamilyIntg = FIELDINT(family,spaceDim)
                        fieldFamilyIntg.setIterationNumber(0)
                        fieldFamilyIntg.setOrderNumber(0)
                        fieldFamilyIntg.setTime(0.0)
                        for kcomp in range(spaceDim):
                            kcomp1 = kcomp+1
                            if kcomp == 0:
                                fieldCompName = "comp1"
                                fieldCompDesc = "desc1"
                                fieldCompUnit = "unit1"
                            if kcomp == 1:
                                fieldCompName = "comp2"
                                fieldCompDesc = "desc2"
                                fieldCompUnit = "unit2"
                            if kcomp == 2:
                                fieldCompName = "comp2"
                                fieldCompDesc = "desc2"
                                fieldCompUnit = "unit2"

                            fieldFamilyIntg.setComponentName(kcomp1,fieldCompName)
                            fieldFamilyIntg.setComponentDescription(kcomp1,fieldCompDesc)
                            fieldFamilyIntg.setMEDComponentUnit(kcomp1,fieldCompUnit)
                        fieldFamilyName = "Integer Field on "+familyName
                        fieldFamilyIntg.setName(fieldFamilyName)
                        field_name = fieldFamilyIntg.getName()
                        type_field = fieldFamilyIntg.getValueType()
                        nbOfComp = fieldFamilyIntg.getNumberOfComponents()
                        print "      The field",field_name,"is with the type",type_field
                        print "      It has",nbOfComp,"Component(s)"
                        for kcomp in range(nbOfComp):
                            kcomp1 = kcomp+1
                            compName = fieldFamilyIntg.getComponentName(kcomp1)
                            compDesc = fieldFamilyIntg.getComponentDescription(kcomp1)
                            compUnit = fieldFamilyIntg.getMEDComponentUnit(kcomp1)
                            print "      * Component:",kcomp1
                            print "          Name:",compName
                            print "          Description:",compDesc
                            print "          Unit:",compUnit

                        nbOf = fieldFamilyIntg.getSupport().getNumberOfElements(MED_ALL_ELEMENTS)
                        print "      Values:",nbOf
                        print "      Randomly set and get to check ..!"
                        for k in range(nbOf):
                            valueI = []
                            for kcomp in range(nbOfComp):
                                valueI.append(randint(0,100))

#                            print "     Set Entry *",(k+1)," ",valueI[:nbOfComp]
                            valInd = numberFamily[k]
                            fieldFamilyIntg.setRow(valInd,valueI)
                            valueIverif = fieldFamilyIntg.getRow(valInd)
                            print "     Set/Get Entry *",(k+1)," ",valueI[:nbOfComp],"  /  ",valueIverif[:nbOfComp]
                        print "    * Getting a Real Field"
                        fieldFamilyDble = FIELDDOUBLE(family,spaceDim)
                        fieldFamilyDble.setIterationNumber(0)
                        fieldFamilyDble.setOrderNumber(0)
                        fieldFamilyDble.setTime(0.0)
                        for kcomp in range(spaceDim):
                            kcomp1 = kcomp+1
                            if kcomp == 0:
                                fieldCompName = "comp1"
                                fieldCompDesc = "desc1"
                                fieldCompUnit = "unit1"
                            if kcomp == 1:
                                fieldCompName = "comp2"
                                fieldCompDesc = "desc2"
                                fieldCompUnit = "unit2"
                            if kcomp == 2:
                                fieldCompName = "comp2"
                                fieldCompDesc = "desc2"
                                fieldCompUnit = "unit2"

                            fieldFamilyDble.setComponentName(kcomp1,fieldCompName)
                            fieldFamilyDble.setComponentDescription(kcomp1,fieldCompDesc)
                            fieldFamilyDble.setMEDComponentUnit(kcomp1,fieldCompUnit)

                        fieldFamilyName = "Real Field on "+familyName
                        fieldFamilyDble.setName(fieldFamilyName)
                        field_name = fieldFamilyDble.getName()
                        type_field = fieldFamilyDble.getValueType()
                        nbOfComp = fieldFamilyDble.getNumberOfComponents()
                        print "      The field",field_name,"is with the type",type_field
                        print "      It has",nbOfComp,"Component(s)"
                        for kcomp in range(nbOfComp):
                            kcomp1 = kcomp+1
                            compName = fieldFamilyDble.getComponentName(kcomp1)
                            compDesc = fieldFamilyDble.getComponentDescription(kcomp1)
                            compUnit = fieldFamilyDble.getMEDComponentUnit(kcomp1)
                            print "      * Component:",kcomp1
                            print "          Name:",compName
                            print "          Description:",compDesc
                            print "          Unit:",compUnit

                        nbOf = fieldFamilyDble.getSupport().getNumberOfElements(MED_ALL_ELEMENTS)
                        print "      Values:",nbOf
                        print "      Randomly set and get to check ..!"
                        for k in range(nbOf):
                            valueI = []
                            for kcomp in range(nbOfComp):
                                valueI.append(random())

#                            print "     Set Entry *",(k+1)," ",valueI[:nbOfComp]
                            valInd = numberFamily[k]
                            fieldFamilyDble.setRow(valInd,valueI)
                            valueIverif = fieldFamilyDble.getRow(valInd)
                            print "     Set/Get Entry *",(k+1)," ",valueI[:nbOfComp],"  /  ",valueIverif[:nbOfComp]
                print ""
        print ""
        for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
            nbGrp = mesh.getNumberOfGroups(entity)
            if (entity == MED_NODE) & (nbGrp > 0):
                print "This mesh has",nbGrp,"Node Group(s)"
            elif (entity == MED_CELL) & (nbGrp > 0):
                print "This mesh has",nbGrp,"Cell Group(s)"
            elif (entity == MED_FACE) & (nbGrp > 0):
                print "This mesh has",nbGrp,"Face Group(s)"
            elif (entity == MED_EDGE) & (nbGrp > 0):
                print "This mesh has",nbGrp,"Edge Group(s)"

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

        print "Building of the support on all Cells of the mesh."
        supportCell = mesh.getSupportOnAll( MED_CELL )
        print ""
        barycenter = mesh.getBarycenter(supportCell)
        print "Getting barycenter of all Cells of the mesh"
        for j in range(nbElemts):
            barycenterCell = barycenter.getRow(j+1)
            print "    * ",barycenterCell[:spaceDim]
        print ""
        if spaceDim == 3 :
            print "Getting volume of all Cells of the mesh:"
            volume = mesh.getVolume(supportCell)
            voltot = 0.
            for j in range(nbElemts):
                volumeCell = volume.getValueIJ(j+1,1)
                print "    * ",volumeCell
                voltot = voltot + volumeCell
            print "Volume of the mesh:",voltot
            print ""
            print "Building of the support on all Faces of the mesh."
            supportFace = SUPPORT(mesh,"Support on all faces of the mesh",MED_FACE)
            nbFace = mesh.getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS)
            print ""
            print "Getting normal of each face of this support",nbFace
            nbTypeFace = mesh.getNumberOfTypes(MED_FACE)
            TypeFace = mesh.getTypes(MED_FACE)
            print "nbTypeFace:",nbTypeFace,"----",TypeFace[:nbTypeFace]
            normal = mesh.getNormal(supportFace)
            area = mesh.getArea(supportFace)
            for j in range(nbFace):
                normalFace = normal.getRow(j+1)
                areaFace = area.getRow(j+1)
                value1 = normalFace[0]
                value2 = normalFace[1]
                value3 = normalFace[2]
                norm = (value1*value1 + value2*value2 + value3*value3)**(0.5)
                print "    * ",normalFace[:spaceDim],"norm:",norm," --> area ",areaFace[0]
        elif spaceDim == 2:
            print "Getting area on all Cells of the mesh:"
            area = mesh.getArea(supportCell)
            areatot = 0.
            for j in range(nbElemts):
                areaCell = area.getValueIJ(j+1,1)
                print "    * ",areaCell
                areatot = areatot + areaCell
            print "Area of the mesh:",areatot
            print ""            
            print "Building of the support on all Edges of the mesh."
            supportEdge = mesh.getSupportOnAll(MED_EDGE)
            nbEdge = mesh.getNumberOfElements(MED_EDGE,MED_ALL_ELEMENTS)
            print ""
            print "Getting normal of each edge of this support",nbEdge
            nbTypeEdge = mesh.getNumberOfTypes(MED_EDGE)
            TypeEdge = mesh.getTypes(MED_EDGE)
            print "nbTypeEdge:",nbTypeEdge,"----",TypeEdge[:nbTypeEdge]
            normal = mesh.getNormal(supportEdge)
            length = mesh.getLength(supportEdge)
            for j in range(nbEdge):
                normalEdge = normal.getRow(j+1)
                lengthEdge = length.getRow(j+1)
                value1 = normalEdge[0]
                value2 = normalEdge[1]
                norm = (value1*value1 + value2*value2)**(0.5)
                print "    * ",normalEdge[:spaceDim],"norm:",norm," --> length ",lengthEdge[0]
        print ""
        print "Building support on Elements of the boundary"
        if spaceDim == 3 :
            suppBound = mesh.getBoundaryElements(MED_FACE)
            nbElmBound = suppBound.getNumberOfElements(MED_ALL_ELEMENTS)
            print "Getting normal field on the boundary",nbElmBound
            normalBound = mesh.getNormal(suppBound)
            if suppBound.isOnAllElements():
                numberSuppBound = range(1,nbElmBound+1)
            else:
                numberSuppBound = suppBound.getNumber(MED_ALL_ELEMENTS)
            for j in range(nbElmBound):
                valInd = numberSuppBound[j]
                normalBoundJ = normalBound.getRow(valInd)
                value1 = normalBoundJ[0]
                value2 = normalBoundJ[1]
                value3 = normalBoundJ[2]
                norm = (value1*value1 + value2*value2 + value3*value3)**(0.5)
                print "    * ",normalBoundJ[:spaceDim],"norm:",norm
        elif spaceDim == 2:
            suppBound = mesh.getBoundaryElements(MED_EDGE)
            nbElmBound = suppBound.getNumberOfElements(MED_ALL_ELEMENTS)
            print "Getting normal field on the boundary",nbElmBound
            normalBound = mesh.getNormal(suppBound)
            if suppBound.isOnAllElements():
                numberSuppBound = range(1,nbElmBound+1)
            else:
                numberSuppBound = suppBound.getNumber(MED_ALL_ELEMENTS)
            for j in range(nbElmBound):
                valInd = numberSuppBound[j]
                normalBoundJ = normalBound.getRow(valInd)
                value1 = normalBoundJ[0]
                value2 = normalBoundJ[1]
                norm = (value1*value1 + value2*value2)**(0.5)
                print "    * ",normalBoundJ[:spaceDim],"norm:",norm
        print ""
if (nbFields>0):
    print "Field(s) Analysis "
    for i in range(nbFields):
        field_name = md.getFieldName(i)
        dtits = md.getFieldIteration(field_name)
        nbOfIt = len(dtits)
        print "The",print_ord(i),"field is",field_name,"with",nbOfIt,"iteration(s)"
        for dtitfield in dtits:
            dt = dtitfield.getdt()
            it = dtitfield.getit()
            type = md.getFieldType(field_name)
            print "     * Iteration:",dt,"Order number:",it,"Type:",type
            if type == MED_INT32:
                fieldint = FIELDINT(MED_DRIVER,md.getFileName(),field_name,dt,it,mesh)
                name = fieldint.getName()
                desc = fieldint.getDescription()
                nbOfComp = fieldint.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fieldint.getIterationNumber()
                orderNb = fieldint.getOrderNumber()
                time = fieldint.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fieldint.getComponentName(kp1)
                    compDesc = fieldint.getComponentDescription(kp1)
                    compUnit = fieldint.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fieldint.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fieldint.getRow(k+1)
                    print "     *",valueI[:nbOfComp]
            elif type == MED_REEL64:
                fielddouble = FIELDDOUBLE(MED_DRIVER,md.getFileName(),field_name,dt,it,mesh)
                name = fielddouble.getName()
                desc = fielddouble.getDescription()
                nbOfComp = fielddouble.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fielddouble.getIterationNumber()
                orderNb = fielddouble.getOrderNumber()
                time = fielddouble.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fielddouble.getComponentName(kp1)
                    compDesc = fielddouble.getComponentDescription(kp1)
                    compUnit = fielddouble.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fielddouble.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fielddouble.getRow(k+1)
                    print "     *",valueI[:nbOfComp]
            else:
                print "  !!!! Bad type of Field !!!!"

print "END of the Pyhton script ..... Ctrl D to exit"
