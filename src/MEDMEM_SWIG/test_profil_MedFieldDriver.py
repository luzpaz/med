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
# this Python script is testing the profil functionality using the FIELD
# constructor via a MED driver.
############################################################################
#
from libMEDMEM_Swig import *

# users inputs
#test 1 --- zzzz121b_without_tr6.med RESUZERODEPL____________________ 1 1
#test 2 --- zzzz121b_without_tr6.med RESUZEROERRE_ELGA_NORE__________ 1 1
#test 3 --- maill.00.med Indicateur_d_erreur_00 2 2

medFile = "zzzz121b_without_tr6.med"
#medFile = "maill.00.med"

fieldName = "RESUZERODEPL____________________"
#fieldName = "RESUZEROERRE_ELGA_NORE__________"
#fieldName = "Indicateur_d_erreur_00"

# default  value, may be changed according to what field is stored in medFile
##iterationNumber = -1
##orderNumber = -1
iterationNumber = 1
orderNumber = 1
#iterationNumber = 2
#orderNumber = 2

import os
#
#befor running this script, please be sure about the path the file fileName
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFile = os.path.join(filePath, medFile)

def analyseField(field):
    fieldName = field.getName()
    fieldType = field.getValueType()
    fieldDesc = field.getDescription()
    fieldIterationNumber = field.getIterationNumber()
    fieldOrderNumber = field.getOrderNumber()
    fieldTime = field.getTime()
    fieldNbOfComp = field.getNumberOfComponents()
    print "The field ",fieldName," with the description ",fieldDesc," typed ",fieldType
    print "Iteration number ",fieldIterationNumber," OrderNumber ",fieldOrderNumber," Time ",fieldTime
    print "It has ",fieldNbOfComp,"components"
    for k in range(fieldNbOfComp):
        kp1 = k+1
        fieldCompName = field.getComponentName(kp1)
        fieldCompDesc = field.getComponentDescription(kp1)
        fieldCompUnit = field.getMEDComponentUnit(kp1)
        print "      * Component:",kp1
        print "          Name:",fieldCompName
        print "          Description:",fieldCompDesc
        print "          Unit:",fieldCompUnit
        pass
    fieldSupport = field.getSupport()
    fieldMeshName = fieldSupport.getMeshName()
    fieldSupportOnAll = fieldSupport.isOnAllElements()
    fieldNbEntities = fieldSupport.getNumberOfElements(MED_ALL_ELEMENTS)
    fieldEntityType = fieldSupport.getEntity()
    fieldSupportNumber = range(1,fieldNbEntities+1)
    if (not fieldSupportOnAll):
        fieldSupportNumber = fieldSupport.getNumber(MED_ALL_ELEMENTS)
        pass
    print " fieldSupportNumber ", fieldSupportNumber
    fieldInterlacingType = field.getInterlacingType()
    print "It relies on the mesh named ",fieldMeshName
    print "its interlacing type ",fieldInterlacingType
    print "and it is on ",fieldNbEntities," entities of the type ",fieldEntityType
    if (fieldInterlacingType == MED_FULL_INTERLACE):
        for i in range(fieldNbEntities):
            value = field.getRow(fieldSupportNumber[i])
            print " * ",fieldSupportNumber[i]," --- ",value
            pass
        pass
    elif (fieldInterlacingType == MED_FULL_INTERLACE):
        for i in range(fieldNbOfComp):
            value = field.getColumn(fieldSupportNumber[i])
            print " * ",fieldSupportNumber[i]," --- ",value
            pass
        pass
    pass

field = FIELDDOUBLE(MED_DRIVER, medFile, fieldName,
                    iterationNumber, orderNumber)

analyseField(field)

print "END of the Pyhton script ..... Ctrl D to exit"
