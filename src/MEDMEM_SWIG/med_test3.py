###################################################################################
#
# This Python script is parsing a MED file using MED Memory from SALOME platform:
# It tests the setValue(I) function on MED fields
#
###################################################################################

from libMEDMEM_Swig import *
from random import *

medFile = "carre_en_quad4_seg2.med"
#medFile = "cube_hexa8_quad4.med"

def print_ord(i):
    if i == 0:
        return 'first'
    elif i == 1:
        return 'second'
    elif i == 2:
        return 'third'
    else:
        return `i`+'th'

md = MED()

mdDriver = MED_MED_RDONLY_DRIVER(medFile,md)

mdDriver.open()
mdDriver.readFileStruct()
mdDriver.close()

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
        mesh = md.getMesh(mesh_name)
        mesh.read()
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
                        fieldFamilyIntg.setValue(mode,value)
                        valueverif = fieldFamilyIntg.getValue(mode)
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode is no interlace"
                        mode = MED_NO_INTERLACE
                        fieldFamilyIntg.setValue(mode,value)
                        valueverif = fieldFamilyIntg.getValue(mode)
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
                        fieldFamilyIntg.setValue(mode_full,value)
                        valueverif = fieldFamilyIntg.getValue(mode_no)
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode for set is no interlace and full interlace for get"
                        fieldFamilyIntg.setValue(mode_no,value)
                        valueverif = fieldFamilyIntg.getValue(mode_full)
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        print "      Randomly set (via setValueI) and get (via getValueI) to check with the same mode ..!"
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode is full interlace"
                        mode = MED_FULL_INTERLACE
                        for k in range(nbOf):
                            fieldFamilyIntg.setValueI(mode,(k+1),value[k*nbOfComp:(k+1)*nbOfComp])
                        valueverif = []
                        for k in range(nbOf):
                            valueverif.extend(fieldFamilyIntg.getValueI(mode,(k+1)))
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode is no interlace"
                        mode = MED_NO_INTERLACE
                        for k in range(nbOfComp):
                            fieldFamilyIntg.setValueI(mode,(k+1),value[k*nbOf:(k+1)*nbOf])
                        valueverif = []
                        for k in range(nbOfComp):
                            valueverif.extend(fieldFamilyIntg.getValueI(mode,(k+1)))
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        print "      Randomly set (via setValueI) and get (via getValueI) to check with different mode ..!"
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode for set is full interlace and no interlace for get"
                        mode_full = MED_FULL_INTERLACE
                        mode_no = MED_NO_INTERLACE
                        for k in range(nbOf):
                            fieldFamilyIntg.setValueI(mode_full,(k+1),value[k*nbOfComp:(k+1)*nbOfComp])
                        valueverif = []
                        for k in range(nbOfComp):
                            valueverif.extend(fieldFamilyIntg.getValueI(mode_no,(k+1)))
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
                        value = []
                        for k in range(nbOf*nbOfComp):
                            value.append(randint(0,100))
                        print "      the mode for set is no interlace and full interlace for get"
                        for k in range(nbOfComp):
                            fieldFamilyIntg.setValueI(mode_no,(k+1),value[k*nbOf:(k+1)*nbOf])
                        valueverif = []
                        for k in range(nbOf):
                            valueverif.extend(fieldFamilyIntg.getValueI(mode_full,(k+1)))
                        for k in range(nbOf*nbOfComp):
                            print "      Set/Get Entry * ",value[k]," / ",valueverif[k]
                        print ""
