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

#  MED MedCorba_Swig : binding of MED CORBA objects woth Python
#  File   : batchmode_medcorba_test.py
#  Module : MED
#
import batchmode_salome

import SALOME_MED

from libMedCorba_Swig import *
from libMEDClient import *

from random import *

import os

filePath = os.environ["MED_ROOT_DIR"]
filePath = os.path.join( filePath, "share", "salome", "resources", "med" )

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

def getMeshObjectFromStudy(meshName):
    objNameInStudy = "/Med/MEDMESH/"+meshName
    mySO = batchmode_salome.myStudy.FindObjectByPath(objNameInStudy)
    if (mySO == None) :
        print "ERROR: ",objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = batchmode_salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.MESH)
        if (myObj == None) :
            print "ERROR: ",objNameInStudy," has been found in the Study but with the wrong type"
        return myObj


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

def getMedObjectFromStudy(file):
    objNameInStudy = "MED_OBJECT_FROM_FILE_"+file
    compNameInStudy= "MED"
    listOfSO = batchmode_salome.myStudy.FindObjectByName(objNameInStudy,
                                                         compNameInStudy)
    listLength = len(listOfSO)
    if (listLength == 0) :
        print "ERROR: ",objNameInStudy," cannot be found in the Study under the component ",compNameInStudy
        return None
    elif (listLength > 1) :
        print "ERROR: there are more than one instance of ",objNameInStudy," in the Study under the component ",compNameInStudy
        return None
    mySO = listOfSO[0]
    if (mySO == None) :
        print "ERROR: ",objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = batchmode_salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.MED)
        if (myObj == None) :
            print "ERROR: ",objNameInStudy," has been found in the Study but with the wrong type"
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

fileName = "cube_hexa8_quad4.med"
#medFile = "carre_en_quad4_seg2.med"
medFile = os.path.join( filePath, fileName )

print "Loading of the Med Component"
print ""

med_comp = batchmode_salome.lcc.FindOrLoadComponent("FactoryServer", "MED")

med_comp.readStructFileWithFieldType(medFile,studyCurrent)

med_obj = getMedObjectFromStudy(fileName)

nbMeshes = med_obj.getNumberOfMeshes()

nbFields = med_obj.getNumberOfFields()

meshNames = med_obj.getMeshNames()

print ""
print "The med file ",medFile," has ",nbMeshes," Meshe(s) and ",nbFields," Field(s)"
print ""

meshName = meshNames[0]

meshcorba = getMeshObjectFromStudy(meshName)

nbNodes = meshcorba.getNumberOfNodes()

spaceDim = meshcorba.getSpaceDimension()

print "The mesh from the Study is ",meshName,".It is a ",spaceDim,"-D mesh and it has ",nbNodes," Nodes"
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

                supportOutLocal = SUPPORTClient( familycorba )
                supportOutCorba = createCorbaSupport( supportOutLocal )

                print "\nGenerate a Local scalar double field"
                fieldScalDblLoc = createLocalFieldDouble(nbOfComp,lengthValue)
                value = [ random() for k in range(lengthValue*nbOfComp) ]
                fieldScalDblLoc.setValue( value ) ## the only way as field support not defined
                valueIverif = fieldScalDblLoc.getValue()
                print "     Set ",value
                print "     Get ",valueIverif

                print "\nGenerate a Corba scalar double field"
                fieldScalDblLoc.setSupport( supportOutLocal )
                fieldScalDblCorba = createCorbaFieldDouble(supportOutCorba,fieldScalDblLoc)
                AnalyzeField(fieldScalDblCorba)

                print "Generate a Local scalar integer field"
                fieldScalIntLoc = createLocalFieldInt(nbOfComp,lengthValue)
                value = [ randint(0,100) for k in range(lengthValue*nbOfComp) ]
                fieldScalIntLoc.setValue( value ) ## the only way as field support not defined
                valueIverif = fieldScalIntLoc.getValue()
                print "     Set ",value
                print "     Get ",valueIverif

                print "\nGenerate a Corba scalar integer field"
                fieldScalIntLoc.setSupport( supportOutLocal )
                fieldScalIntCorba = createCorbaFieldInt(supportOutCorba,fieldScalIntLoc)
                AnalyzeField(fieldScalIntCorba)

                nbOfComp = spaceDim
                print "\nGenerate a Local vector double field"
                fieldVectDblLoc = createLocalFieldDouble(nbOfComp,lengthValue)
                value = [ random() for k in range(lengthValue*nbOfComp) ]
                fieldVectDblLoc.setValue(value)
                valueIverif = fieldVectDblLoc.getValue()
                print "     Set ",value
                print "     Get ",valueIverif

                print "\nGenerate a Corba vector double field"
                fieldVectDblLoc.setSupport( supportOutLocal )
                fieldVectDblCorba = createCorbaFieldDouble(supportOutCorba,fieldVectDblLoc)
                AnalyzeField(fieldVectDblCorba)

                print "\nGenerate a Local vector integer field"
                fieldVectIntLoc = createLocalFieldInt(nbOfComp,lengthValue)
                value = [ randint(0,100) for k in range(lengthValue*nbOfComp) ]
                fieldVectIntLoc.setValue(value)
                valueIverif = fieldVectIntLoc.getValue()
                print "     Set ",value
                print "     Get ",valueIverif

                print "\nGenerate a Corba vector integer field"
                fieldVectIntLoc.setSupport( supportOutLocal )
                fieldVectIntCorba = createCorbaFieldInt(supportOutCorba,fieldVectIntLoc)
                AnalyzeField(fieldVectIntCorba)
                print ""
print "Fin du script Python ...."


