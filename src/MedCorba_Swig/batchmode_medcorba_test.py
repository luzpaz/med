#  MED MedCorba_Swig : binding of MED CORBA objects woth Python
#
#  Copyright (C) 2003  CEA/DEN, EDF R&D
#
#
#
#  File   : batchmode_medcorba_test.py
#  Module : MED

import batchmode_salome

import SALOME_MED

from libMedCorba_Swig import *

from random import *

#==============================================================================

def AnalyzeField(field):
    name = field.getName()
    desc = field.getDescription()
    nbComp = field.getNumberOfComponents()
    itNum = field.getIterationNumber()
    ordNum = field.getOrderNumber()

    print "Analysis of the field ",name," with the description ",desc
    print "iteration number ",itNum," order Number ",ordNum
    print "It has ",nbComp," component(s)"

    fieldValue = field.getValue(SALOME_MED.MED_FULL_INTERLACE)
    fieldSupport = field.getSupport()
    fieldMesh = fieldSupport.getMesh()
    fieldEntity = fieldSupport.getEntity()
    bool = fieldSupport.isOnAllElements()

    if bool:
        print "The support of this field is on all entities ",fieldEntity," of the mesh ",fieldMesh.getName()
        if fieldEntity == SALOME_MED.MED_NODE:
            nbValByComp = fieldMesh.getNumberOfNodes()
        else:
            nbValByComp = fieldMesh.getNumberOfElements(fieldEntity,SALOME_MED.MED_ALL_ELEMENTS)
        print "and its dimension (number of values by component of the field) is ",nbValByComp
    else:
        print "The support of this field is partially on entities ",fieldEntity," of the mesh ",fieldMesh.getName()
        nbValByComp = fieldSupport.getNumberOfElements(SALOME_MED.MED_ALL_ELEMENTS)
        print "and its dimension (number of values by component of the field) is ",nbValByComp

    for i in range(nbComp):
        compName = field.getComponentName(i+1)
        compUnit = field.getComponentUnit(i+1)
        print "The ",(i+1),"-th  component ",compName," with the unit ",compUnit

    for i in range(nbValByComp):
        print "  * ",fieldValue[i*nbComp:(i+1)*nbComp]

#==============================================================================

def getMeshObjectFromStudy(number):
    mySO = batchmode_salome.myStudy.FindObject("MEDMESH")
    mysub = mySO.FindSubObject(number)[1]
    if mysub:
        Builder = batchmode_salome.myStudy.NewBuilder()
        anAttr = Builder.FindOrCreateAttribute(mysub, "AttributeIOR")
        obj = batchmode_salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.MESH)
        return myObj
    else:
        print "ERROR: No Mesh Object stored in this Study"
        return None

#==============================================================================

def getMedObjectFromStudy():
    mySO = batchmode_salome.myStudy.FindObject("Objet MED")
    Builder = batchmode_salome.myStudy.NewBuilder()
    anAttr = Builder.FindOrCreateAttribute(mySO, "AttributeIOR")
    obj = batchmode_salome.orb.string_to_object(anAttr.Value())
    myObj = obj._narrow(SALOME_MED.MED)
    return myObj

#==============================================================================
#
# Since Corba pointeurs will be generated in this Python script
# a POA has to be registred to activate those Corba pointeurs
# and make them available by any clients (in general). In our
# case, it is to make those pointeurs avail able to itself
#==============================================================================

print "Activation of a POA to make available any Corba pointeurs"
poa = batchmode_salome.orb.resolve_initial_references("RootPOA")
poaManager = poa._get_the_POAManager()
poaManager.activate()

#==============================================================================

studyCurrent = batchmode_salome.myStudyName
studyCurrentId = batchmode_salome.myStudyId

print "We are working in the study ",studyCurrent," with the ID ",studyCurrentId
print ""

#medFile = "carre_en_quad4_seg2.med"
medFile = "cube_hexa8_quad4.med"

print "Loading of the Med Component"
print ""

med_comp = batchmode_salome.lcc.FindOrLoadComponent("FactoryServer", "MED")

med_comp.readStructFile(medFile,studyCurrent)

med_obj = getMedObjectFromStudy()

nbMeshes = med_obj.getNumberOfMeshes()

nbFields = med_obj.getNumberOfFields()

print ""
print "The med file ",medFile," has ",nbMeshes," Meshe(s) and ",nbFields," Field(s)"
print ""

meshcorba = getMeshObjectFromStudy(1)

name = meshcorba.getName()

nbNodes = meshcorba.getNumberOfNodes()

spaceDim = meshcorba.getSpaceDimension()

print "The mesh from the Study is ",name,".It is a ",spaceDim,"-D mesh and it has ",nbNodes," Nodes"
print ""

for entity in [SALOME_MED.MED_NODE,SALOME_MED.MED_CELL,SALOME_MED.MED_FACE,SALOME_MED.MED_EDGE]:
    nbFam = meshcorba.getNumberOfFamilies(entity)
    if (entity == SALOME_MED.MED_NODE) & (nbFam > 0):
        print "This mesh has",nbFam,"Node Family(ies)"
    elif (entity == SALOME_MED.MED_CELL) & (nbFam > 0):
        print "This mesh has",nbFam,"Cell Family(ies)"
    elif (entity == SALOME_MED.MED_FACE) & (nbFam > 0):
        print "This mesh has",nbFam,"Face Family(ies)"
    elif (entity == SALOME_MED.MED_EDGE) & (nbFam > 0):
        print "This mesh has",nbFam,"Edge Family(ies)"

    if nbFam > 0:
        for j in range(nbFam):
            print ""
            familycorba = meshcorba.getFamily(entity,j+1)
            familyName = familycorba.getName()
            familyDescription = familycorba.getDescription()
            familyEntity = familycorba.getEntity()
            familyBool = familycorba.isOnAllElements()
            print "  -Name:",familyName
            print "  -Description:",familyDescription
            print "  -Entity:",familyEntity
            familyIdentifier = familycorba.getIdentifier()
            nbOfAtt = familycorba.getNumberOfAttributes()
            print "  -Identifier:",familyIdentifier
            print "  -Number Of Attributes:",nbOfAtt
            attributesids = familycorba.getAttributesIdentifiers()
            attributesvals = familycorba.getAttributesValues()
            for k in range(nbOfAtt):
                print "    * Attributes:",attributesids[k],":",attributesvals[k],",",familycorba.getAttributeDescription(k+1)
            print "  -Entities list:"
            if (familyBool):
                print "  -Is on all entities"
            else:
                types = familycorba.getTypes()
                nbOfTypes = len(types)
                print "  -Number Of Types:",nbOfTypes
                for k in range(nbOfTypes):
                    type = types[k]
                    nbOfElmtsOfType = familycorba.getNumberOfElements(type)
                    number = familycorba.getNumber(type)
                    print "    * Type",type
                    print "    * Number",number[0:nbOfElmtsOfType]
                print ""
                lengthValue = familycorba.getNumberOfElements(SALOME_MED.MED_ALL_ELEMENTS)
                nbOfComp = 1
                print "Generate a Local scalar double field"
                fieldScalDblLoc = createLocalFieldDouble(nbOfComp,lengthValue)
                for k in range(lengthValue):
                    valueI = []
                    for kcomp in range(nbOfComp):
                        valueI.append(random())
                    fieldScalDblLoc.setValueI(MED_FULL_INTERLACE,k+1,valueI)
                    valueIverif = fieldScalDblLoc.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     Set/Get Entry *",(k+1)," ",valueI[:nbOfComp],"  /  ",valueIverif[:nbOfComp]
                print "Generate a Corba scalar double field"
                fieldScalDblCorba = createCorbaFieldDouble(familycorba,fieldScalDblLoc)
                AnalyzeField(fieldScalDblCorba)
                print ""
                print "Generate a Local scalar integer field"
                fieldScalIntLoc = createLocalFieldInt(nbOfComp,lengthValue)
                for k in range(lengthValue):
                    valueI = []
                    for kcomp in range(nbOfComp):
                        valueI.append(randint(0,100))
                    fieldScalIntLoc.setValueI(MED_FULL_INTERLACE,k+1,valueI)
                    valueIverif = fieldScalIntLoc.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     Set/Get Entry *",(k+1)," ",valueI[:nbOfComp],"  /  ",valueIverif[:nbOfComp]
                print "Generate a Corba scalar integer field"
                fieldScalIntCorba = createCorbaFieldInt(familycorba,fieldScalIntLoc)
                AnalyzeField(fieldScalIntCorba)
                print ""
                nbOfComp = spaceDim
                print "Generate a Local vector double field"
                fieldVectDblLoc = createLocalFieldDouble(nbOfComp,lengthValue)
                for k in range(lengthValue):
                    valueI = []
                    for kcomp in range(nbOfComp):
                        valueI.append(random())
                    fieldVectDblLoc.setValueI(MED_FULL_INTERLACE,k+1,valueI)
                    valueIverif = fieldVectDblLoc.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     Set/Get Entry *",(k+1)," ",valueI[:nbOfComp],"  /  ",valueIverif[:nbOfComp]
                print "Generate a Corba vector double field"
                fieldVectDblCorba = createCorbaFieldDouble(familycorba,fieldVectDblLoc)
                AnalyzeField(fieldVectDblCorba)
                print ""
                print "Generate a Local vector integer field"
                fieldVectIntLoc = createLocalFieldInt(nbOfComp,lengthValue)
                for k in range(lengthValue):
                    valueI = []
                    for kcomp in range(nbOfComp):
                        valueI.append(randint(0,100))
                    fieldVectIntLoc.setValueI(MED_FULL_INTERLACE,k+1,valueI)
                    valueIverif = fieldVectIntLoc.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     Set/Get Entry *",(k+1)," ",valueI[:nbOfComp],"  /  ",valueIverif[:nbOfComp]
                print "Generate a Corba vector integer field"
                fieldVectIntCorba = createCorbaFieldInt(familycorba,fieldVectIntLoc)
                AnalyzeField(fieldVectIntCorba)
                print ""
print "Fin du script Python ...."


