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

def getMedObjectFromStudy(fileName=None):
    myObj=None; Builder = batchmode_salome.myStudy.NewBuilder()
    if fileName is not None:
        objNameInStudy = "MED_OBJECT_FROM_FILE_"+fileName
        mySO = batchmode_salome.myStudy.FindObject(objNameInStudy)
        if mySO is not None:
            anAttr = Builder.FindOrCreateAttribute(mySO, "AttributeIOR")
            obj = batchmode_salome.orb.string_to_object(anAttr.Value())
            if obj is not None:
                myObj = obj._narrow(SALOME_MED.MED)
            else:
                print "ERROR: ",myObj," has been found in the Study, but with the type different of SALOME_MED.MED!!!"
        else:
            print "ERROR: ",objNameInStudy," hasn't been found in the Study!!!"
    else:
        SObj_root = batchmode_salome.myStudy.FindObjectByPath("/Med/")
        if SObj_root is not None:
            iter = batchmode_salome.myStudy.NewChildIterator(SObj_root)
            try:
                iter.Init(); 
                while iter.More():
                    Obj = iter.Value()
                    if Obj is not None:
                        Ok, anAttr = Builder.FindAttribute(Obj, "AttributeIOR")
                        if Ok:
                            if len(anAttr.Value()) > 0:
                                obj = batchmode_salome.orb.string_to_object(anAttr.Value())
                                if obj is not None:
                                    myObj = obj._narrow(SALOME_MED.MED)
                                    if myObj is not None:
                                        break
                    iter.Next()
            except:
                print "Exception!!!"
        else: print "Root object Med hasn't been found in the study!!!"
    return myObj
        
studyCurrent = batchmode_salome.myStudyName
studyCurrentId = batchmode_salome.myStudyId

med_comp = batchmode_salome.lcc.FindOrLoadComponent("FactoryServer", "MED")
