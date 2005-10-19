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

import os

filePath=os.environ["DATA_DIR"]
filePath=filePath+"/MedFiles/"

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

def getFieldDoubleObjectFromStudy(number,subnumber):
    mySO = batchmode_salome.myStudy.FindObject("MEDFIELD")
    mysub = mySO.FindSubObject(number)[1]
    if mysub:
        mysubsub = mysub.FindSubObject(subnumber)[1]
        if mysubsub:
            Builder = batchmode_salome.myStudy.NewBuilder()
            anAttr = Builder.FindOrCreateAttribute(mysubsub, "AttributeIOR")
            obj = batchmode_salome.orb.string_to_object(anAttr.Value())
            anAttr = Builder.FindOrCreateAttribute(mysubsub, "AttributeName")
            myObj = obj._narrow(SALOME_MED.FIELDDOUBLE)
            return myObj
        else:
            print "ERROR: No FieldDouble Object stored in this Study"
            return None
    else:
        print "ERROR: No FieldDouble Object stored in this Study"
        return None


#==============================================================================

def getFieldIntObjectFromStudy(number,subnumber):
    mySO = batchmode_salome.myStudy.FindObject("MEDFIELD")
    mysub = mySO.FindSubObject(number)[1]
    if mysub:
        mysubsub = mysub.FindSubObject(subnumber)[1]
        if mysubsub:
            Builder = batchmode_salome.myStudy.NewBuilder()
            anAttr = Builder.FindOrCreateAttribute(mysubsub, "AttributeIOR")
            obj = batchmode_salome.orb.string_to_object(anAttr.Value())
            myObj = obj._narrow(SALOME_MED.FIELDINT)
            return myObj
        else:
            print "ERROR: No FieldInt Object stored in this Study"
            return None
    else:
        print "ERROR: No FieldInt Object stored in this Study"
        return None

#==============================================================================

def getMedObjectFromStudy():
    mySO = batchmode_salome.myStudy.FindObject("Objet MED")
    Builder = batchmode_salome.myStudy.NewBuilder()
    anAttr = Builder.FindOrCreateAttribute(mySO, "AttributeIOR")
    obj = batchmode_salome.orb.string_to_object(anAttr.Value())
    myObj = obj._narrow(SALOME_MED.MED)
    return myObj

studyCurrent = batchmode_salome.myStudyName
studyCurrentId = batchmode_salome.myStudyId

med_comp = batchmode_salome.lcc.FindOrLoadComponent("FactoryServer", "MED")
