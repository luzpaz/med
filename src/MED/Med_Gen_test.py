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

####################################################################################################
# Test the Med Component: mounting in Memory a .med file and trying to get information through
# the CORBA Med API
####################################################################################################
#
import string

import salome

import SALOME_MED

from libSALOME_Swig import *
sg = SALOMEGUI_Swig()

def print_ord(i):
    if i == 0:
        return 'first'
    elif i == 1:
        return 'second'
    elif i == 2:
        return 'third'
    else:
        return `(i+1)`+'th'

def changeBlankToUnderScore(stringWithBlank):
    blank = ' '
    underscore = '_'
    decompString = string.split(stringWithBlank,blank)
    length = len(decompString)
    stringWithUnderScore = decompString[0]
    for i in range(1,length):
        stringWithUnderScore += underscore
        stringWithUnderScore += decompString[i]
    return stringWithUnderScore

def getMedObjectFromStudy(file):
    objNameInStudy = "MED_OBJECT_FROM_FILE_"+file
    compNameInStudy= "MED"
    listOfSO = salome.myStudy.FindObjectByName(objNameInStudy,compNameInStudy)
    listLength = len(listOfSO)
    if (listLength == 0) :
        print objNameInStudy," cannot be found in the Study under the component ",compNameInStudy
        return None
    elif (listLength > 1) :
        print "there are more than one instance of ",objNameInStudy," in the Study under the component ",compNameInStudy
        return None
    mySO = listOfSO[0]
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.MED)
        if (myObj == None) :
            print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

def getMeshObjectFromStudy(meshName):
    objNameInStudy = "/Med/MEDMESH/"+meshName
    mySO = salome.myStudy.FindObjectByPath(objNameInStudy)
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.MESH)
        if (myObj == None) :
            print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

def getSupportObjectFromStudy(meshName,supportName):
    meshNameStudy = changeBlankToUnderScore(meshName)
    objNameInStudy = "/Med/MEDMESH/MEDSUPPORTS_OF_"+meshNameStudy+"/"+supportName
    mySO = salome.myStudy.FindObjectByPath(objNameInStudy)
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.SUPPORT)
        if (myObj == None) :
            print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

def getFieldObjectFromStudy(dt,it,fieldName,supportName,meshName):
    meshNameStudy = changeBlankToUnderScore(meshName)
    objNameInStudy = "/Med/MEDFIELD/"+fieldName+"/("+str(dt)+","+str(it)+")_ON_"+supportName+"_OF_"+meshNameStudy
    mySO = salome.myStudy.FindObjectByPath(objNameInStudy)
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.FIELDINT)
        if (myObj == None):
            myObj = obj._narrow(SALOME_MED.FIELDDOUBLE)
            if (myObj == None) :
                print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

fileName = "cube_hexa8_quad4.med"

#fileName = "carre_en_quad4_seg2.med"

medComp=salome.lcc.FindOrLoadComponent("FactoryServer", "MED")

import os

filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

filePathName = os.path.join(filePath, fileName)

print "Reading the .med file ",filePathName," and pushing corba objects in the SALOME study"
medComp.readStructFileWithFieldType(filePathName,salome.myStudyName)
sg.updateObjBrowser(1)

print "getting the MED object from the study"
medObj = getMedObjectFromStudy(fileName)

nbOfMeshes = medObj.getNumberOfMeshes()
meshNames = medObj.getMeshNames()

print "in this med file there is(are) ",nbOfMeshes," mesh(es):"
for i in range(nbOfMeshes):
    meshName = meshNames[i]
    print "    - the ",print_ord(i)," mesh is named ",meshName
    print "      getting the MESH object using the API of the corba object MED"
    meshObj = medObj.getMeshByName(meshName)
    print "      getting mesh information (including corba object) using the API of the corba object MESH"
    for entity in [SALOME_MED.MED_NODE,SALOME_MED.MED_CELL,
                   SALOME_MED.MED_FACE,SALOME_MED.MED_EDGE]:
        nbFam = meshObj.getNumberOfFamilies(entity)
        nbGrp = meshObj.getNumberOfGroups(entity)
        if (entity == SALOME_MED.MED_NODE):
            print "      this mesh has ",nbFam," Node Family(ies) and ",nbGrp," Node Group(s)"
        elif (entity == SALOME_MED.MED_CELL):
            print "                    ",nbFam," Cell Family(ies) and ",nbGrp," Cell Group(s)"
        elif (entity == SALOME_MED.MED_FACE):
            print "                    ",nbFam," Face Family(ies) and ",nbGrp," Face Group(s)"
        elif (entity == SALOME_MED.MED_EDGE):
            print "                    ",nbFam," Edge Family(ies) and ",nbGrp," Cell Group(s)"

        if nbFam > 0:
            for j in range(nbFam):
                familyObj = meshObj.getFamily(entity,j+1)
                print familyObj

        if nbGrp > 0:
            for j in range(nbGrp):
                groupObj = meshObj.getGroup(entity,j+1)
                print groupObj

    print ""
    print "      getting the MESH object from the Study"
    meshObj = getMeshObjectFromStudy(meshName)
    print meshObj
    print "      getting mesh information using the API of the corba object MESH but corba objects are obtained from the Study"
    for entity in [SALOME_MED.MED_NODE,SALOME_MED.MED_CELL,
                   SALOME_MED.MED_FACE,SALOME_MED.MED_EDGE]:
        nbFam = meshObj.getNumberOfFamilies(entity)
        nbGrp = meshObj.getNumberOfGroups(entity)
        if (entity == SALOME_MED.MED_NODE):
            print "      this mesh has ",nbFam," Node Family(ies) and ",nbGrp," Node Group(s)"
        elif (entity == SALOME_MED.MED_CELL):
            print "                    ",nbFam," Cell Family(ies) and ",nbGrp," Cell Group(s)"
        elif (entity == SALOME_MED.MED_FACE):
            print "                    ",nbFam," Face Family(ies) and ",nbGrp," Face Group(s)"
        elif (entity == SALOME_MED.MED_EDGE):
            print "                    ",nbFam," Edge Family(ies) and ",nbGrp," Cell Group(s)"

        if nbFam > 0:
            for j in range(nbFam):
                familyName = (meshObj.getFamily(entity,j+1)).getName()
                familyObj = getSupportObjectFromStudy(meshName,familyName)
                print familyObj

        if nbGrp > 0:
            for j in range(nbGrp):
                groupName = (meshObj.getGroup(entity,j+1)).getName()
                groupObj = getSupportObjectFromStudy(meshName,groupName)
                print groupObj

    print "let's get other SUPPORT object from  the Study"
    for entity in [SALOME_MED.MED_NODE,SALOME_MED.MED_CELL,
                   SALOME_MED.MED_FACE,SALOME_MED.MED_EDGE]:

        if entity == SALOME_MED.MED_NODE :
            entitySupport = "MED_NOEUD"
        elif entity == SALOME_MED.MED_CELL :
            entitySupport = "MED_MAILLE"
        elif entity == SALOME_MED.MED_FACE :
            entitySuppor = "MED_FACE"
        elif entity == SALOME_MED.MED_EDGE :
            entitySupport = "MED_ARETE"

        supportName = "SupportOnAll_"+entitySupport
        supportObj = getSupportObjectFromStudy(meshName,supportName)

nbOfFields = medObj.getNumberOfFields()
print "in the considered .med file there is(are) ",nbOfFields," field(s):"
fieldNames = medObj.getFieldNames()
for i in range(nbOfFields):
    fieldName = fieldNames[i]
    nbOfIt = medObj.getFieldNumberOfIteration(fieldName)
    print "    - the ",print_ord(i)," field is name ",fieldName," and has ",nbOfIt," iteration(s)"
    for j in range(nbOfIt):
        dtitfield = medObj.getFieldIteration(fieldName,j)
        dt = dtitfield[0]
        it = dtitfield[1]
        print "     * Iteration:",dt,"Order number:",it
        for k in range(nbOfMeshes):
            meshName = meshNames[k]
            for entity in [SALOME_MED.MED_NODE,SALOME_MED.MED_CELL,
                           SALOME_MED.MED_FACE,SALOME_MED.MED_EDGE]:
                if entity == SALOME_MED.MED_NODE :
                    entitySupport = "MED_NOEUD"
                elif entity == SALOME_MED.MED_CELL :
                    entitySupport = "MED_MAILLE"
                elif entity == SALOME_MED.MED_FACE :
                    entitySuppor = "MED_FACE"
                elif entity == SALOME_MED.MED_EDGE :
                    entitySupport = "MED_ARETE"
                supportName = "SupportOnAll_"+entitySupport
                print "getting a corba object Field from the study iteration ",dt," order number ",it," on the support ",supportName," from the mesh ",meshName
                fieldObj = getFieldObjectFromStudy(dt,it,fieldName,supportName,meshName)
                print fieldObj

print ""
print "END of the Pyhton script ..... Ctrl D to exit"
