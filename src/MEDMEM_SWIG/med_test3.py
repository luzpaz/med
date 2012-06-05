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
# It tests the setValue, setRow and setColum functions on MED fields
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

                            kcomp1 = kcomp+1
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
                        print "      Randomly set (via setValue) and get (via getValue) to check with the same mode ..!"
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode is full interlace"
                        mode = MED_FULL_INTERLACE
                        fieldFamilyIntg.setValue(value)
                        valueverif = fieldFamilyIntg.getValue()
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode is no interlace"
                        mode = MED_NO_INTERLACE
                        fieldFamilyIntg.setValue(value)
                        valueverif = fieldFamilyIntg.getValue()
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        print "      Randomly set (via setValue) and get (via getValue) to check with different mode ..!"
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode for set is full interlace and no interlace for get"
                        mode_full = MED_FULL_INTERLACE
                        mode_no = MED_NO_INTERLACE
                        fieldFamilyIntg.setValue(value)
                        valueverif = fieldFamilyIntg.getValue()
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode for set is no interlace and full interlace for get"
                        fieldFamilyIntg.setValue(value)
                        valueverif = fieldFamilyIntg.getValue()
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        print "      Randomly set (via setRow/setColumn) and get (via getRow/getColumn) to check with the same mode ..!"
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        mode = MED_FULL_INTERLACE
                        print "      the mode is full interlace ", mode
                        print "      the field interlacing mode ",fieldFamilyIntg.getInterlacingType()
                        for k in range(nbOf):
                            valInd = numberFamily[k]
                            fieldFamilyIntg.setRow(valInd,value[k*nbOfComp:(k+1)*nbOfComp])
                        valueverif = []
                        for k in range(nbOf):
                            valInd = numberFamily[k]
                            valueverif.extend(fieldFamilyIntg.getRow(valInd))
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        mode = MED_NO_INTERLACE
                        print "      the mode is no interlace ", mode
                        print "      the field interlacing mode ",fieldFamilyIntg.getInterlacingType()
                        for k in range(nbOf):
                            valInd = numberFamily[k]
                            fieldFamilyIntg.setRow(valInd,value[k*nbOfComp:(k+1)*nbOfComp])
                        fieldFamilyIntgOtherMode = createFieldIntConvertNoInterlace(fieldFamilyIntg)
                        valueverif = []
                        for k in range(nbOfComp):
                            valueverif.extend(fieldFamilyIntgOtherMode.getColumn(k+1))
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
print "END of the Pyhton script ..... Ctrl D to exit"
