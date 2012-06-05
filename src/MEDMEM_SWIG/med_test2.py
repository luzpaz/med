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
# well as (d-1)-cells, families), it tests all fields generated in the MESH class
# and write them in a 2 new med files (V2.1 and V2.2), it gives only the number of
# fields stored in the MED file (d is the mesh/space dimension).
###################################################################################
#
from libMEDMEM_Swig import *
from random import *

#==============================================================================

def AnalyzeField(field):
    name = field.getName()
    desc = field.getDescription()
    nbComp = field.getNumberOfComponents()
    itNum = field.getIterationNumber()
    ordNum = field.getOrderNumber()
    type = field.getValueType()

    print "Analysis of the field ",name," with the description ",desc
    print "iteration number ",itNum," order Number ",ordNum
    print "It has ",nbComp," component(s) with the type ",type

    fieldValue = field.getValue()
    fieldSupport = field.getSupport()
    fieldMesh = fieldSupport.getMesh()
    fieldEntity = fieldSupport.getEntity()
    bool = fieldSupport.isOnAllElements()

    if bool:
        print "The support of this field is on all entities ",fieldEntity," of the mesh ",fieldMesh.getName()
        if fieldEntity == MED_NODE:
            nbValByComp = fieldMesh.getNumberOfNodes()
        else:
            nbValByComp = fieldMesh.getNumberOfElements(fieldEntity,MED_ALL_ELEMENTS)
        print "and its dimension (number of values by component of the field) is ",nbValByComp
    else:
        print "The support of this field is partially on entities ",fieldEntity," of the mesh ",fieldMesh.getName()
        nbValByComp = fieldSupport.getNumberOfElements(MED_ALL_ELEMENTS)
        print "and its dimension (number of values by component of the field) is ",nbValByComp

    for i in range(nbComp):
        ip1 = i + 1
        compName = field.getComponentName(ip1)
        compDesc = field.getComponentDescription(ip1)
        compUnit = field.getMEDComponentUnit(ip1)
        print "The ",(i+1),"-th  component ",compName," with the dexription ",compDesc," and the unit ",compUnit

    for i in range(nbValByComp):
        print "  * ",fieldValue[i*nbComp:(i+1)*nbComp]

#==============================================================================
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
                    if (entity != MED_NODE):
                        print ""
                        print "Getting barycenter on this family"
                        barycenterfamily = mesh.getBarycenter(family)
                        if (not familyBool): numberFamily = family.getNumber(MED_ALL_ELEMENTS)
                        nbVal = barycenterfamily.getSupport().getNumberOfElements(MED_ALL_ELEMENTS)
                        nbComp = barycenterfamily.getNumberOfComponents()
                        for j in range(nbVal):
                            valInd = j+1
                            if (not familyBool): valInd = numberFamily[j]
                            barycenterfamilyentity = barycenterfamily.getRow(valInd)
                            print "    * ",barycenterfamilyentity[:nbComp]
                print ""

        print "Building of the support on all Cells of the mesh."
        supportCell = mesh.getSupportOnAll( MED_CELL )
        print ""
        print "Getting barycenter of all Cells of the mesh"
        barycenter = mesh.getBarycenter(supportCell)
        for j in range(nbElemts):
            barycenterCell = barycenter.getRow(j+1)
            print "    * ",barycenterCell[:spaceDim]

        print "Writing on file the mesh"
        writeMed21File = medFile[0:(len(medFile)-4)]+"_V21_fields.med"
        writeMed22File = medFile[0:(len(medFile)-4)]+"_V22_fields.med"
        fieldsMesh = barycenter.getSupport().getMesh()
        fieldsMeshName = "Fields Mesh"
        fieldsMesh.setName(fieldsMeshName)

        index22Mesh = fieldsMesh.addDriver(MED_DRIVER,writeMed22File,fieldsMeshName)
        fieldsMesh.write(index22Mesh)

        AnalyzeField(barycenter)

        print "Writing on file the cells barycenter field"

        barycenterName = barycenter.getName()

        index22FieldBarycenter = barycenter.addDriver(MED_DRIVER,writeMed22File,barycenterName)
        barycenter.write(index22FieldBarycenter)

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

            AnalyzeField(volume)

            print "Writing on file the cells volume field"

            volumeName = volume.getName()

            index22FieldVolume = volume.addDriver(MED_DRIVER,writeMed22File,volumeName)
            volume.write(index22FieldVolume)

            print ""
            print "Building of the support on all Faces of the mesh."
            supportFace = SUPPORT(mesh,"Support on all faces of the mesh",MED_FACE)
            supportFace.update()
            nbFace = mesh.getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS)
            print ""
            print "Getting normal of each face of this support",nbFace
            nbTypeFace = mesh.getNumberOfTypes(MED_FACE)
            TypeFace = mesh.getTypes(MED_FACE)
            print "nbTypeFace:",nbTypeFace,"----",TypeFace[:nbTypeFace]
            normal = mesh.getNormal(supportFace)
            for j in range(nbFace):
                normalFace = normal.getRow(j+1)
                value1 = normalFace[0]
                value2 = normalFace[1]
                value3 = normalFace[2]
                norm = (value1*value1 + value2*value2 + value3*value3)**(0.5)
                print "    * ",normalFace[:spaceDim],"norm:",norm
            print ""

            AnalyzeField(normal)

            print "Writing on file the face normal field"

            normalName = normal.getName()

            index22FieldNormal = normal.addDriver(MED_DRIVER,writeMed22File,normalName)
            normal.write(index22FieldNormal)

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

            AnalyzeField(area)

            print "Writing on file the cells area field"

            areaName = area.getName()

            index22FieldArea = area.addDriver(MED_DRIVER,writeMed22File,areaName)
            area.write(index22FieldArea)

            print ""
            print "Getting the support on all Edges of the mesh."
            supportEdge = mesh.getSupportOnAll(MED_EDGE)
            nbEdge = mesh.getNumberOfElements(MED_EDGE,MED_ALL_ELEMENTS)
            print ""
            print "Getting normal of each edge of this support",nbEdge
            nbTypeEdge = mesh.getNumberOfTypes(MED_EDGE)
            TypeEdge = mesh.getTypes(MED_EDGE)
            print "nbTypeEdge:",nbTypeEdge,"----",TypeEdge[:nbTypeEdge]
            normal = mesh.getNormal(supportEdge)
            for j in range(nbEdge):
                normalEdge = normal.getRow(j+1)
                value1 = normalEdge[0]
                value2 = normalEdge[1]
                norm = (value1*value1 + value2*value2)**(0.5)
                print "    * ",normalEdge[:spaceDim],"norm:",norm
            print ""

            AnalyzeField(normal)

            print "Writing on file the edge normal field"

            normalName = normal.getName()

            index22FieldNormal = normal.addDriver(MED_DRIVER,writeMed22File,normalName)
            normal.write(index22FieldNormal)
        print ""

print "END of the Pyhton script ..... Ctrl D to exit"
