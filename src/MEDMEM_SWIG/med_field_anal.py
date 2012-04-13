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
# This Python script is testing the generation of MED field using a
# user-callable function with different number of components and
# different SUPPORTs.
############################################################################
#
from math import *

def f_scal_dbl_2d(x, y):
    ret = []
    ret.append(x + y)
    return ret

def f_scal_dbl_3d(x, y, z):
    ret = []
    ret.append(x + y + z)
    return ret

def f_vect_dbl_2d(x, y):
    ret = []
    ret.append(x + y)
    ret.append(2.*(x + y))
    return ret

def f_vect_dbl_3d(x, y, z):
    ret = []
    ret.append(x + y + z)
    ret.append(2.*(x + y + z))
    ret.append(3.*(x + y + z))
    return ret

def f_scal_int_2d(x, y):
    ret = []
    ret_tmp = x + y
    ret_tmp = int(ret_tmp)
    ret.append(ret_tmp)
    return ret

def f_scal_int_3d(x, y, z):
    ret = []
    ret_tmp = x + y + z
    ret_tmp = int(ret_tmp)
    ret.append(ret_tmp)
    return ret

def f_vect_int_2d(x, y):
    ret = []
    ret.append(int(x + y))
    ret.append(int(2.*(x + y)))
    return ret

def f_vect_int_3d(x, y, z):
    ret = []
    ret.append(int(x + y + z))
    ret.append(int(2.*(x + y + z)))
    ret.append(int(3.*(x + y + z)))
    return ret

from libMEDMEM_Swig import *
from random import *
import os
#
#befor running this script, please be sure about the path the file fileName
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFile = os.path.join(filePath, "carre_en_quad4_seg2.med")
medFile = os.path.join(filePath, "cube_hexa8_quad4.med")

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

print "The med file", medFile, "contains", nbMeshes, "mesh(es)"

mesh_name = md.getMeshName(0)
mesh = MESH(MED_DRIVER,medFile,mesh_name)
spaceDim = mesh.getSpaceDimension()
meshDim = mesh.getMeshDimension()
nbNodes = mesh.getNumberOfNodes()

print ""
print "The mesh",mesh_name,"is a",spaceDim,"D mesh on a",meshDim,"D geometry and has",nbNodes,"Nodes"

supportOnCell = mesh.getSupportOnAll(MED_CELL)

supportOnNode = mesh.getSupportOnAll(MED_NODE)

if (spaceDim == 3) :
    supportOnConst = mesh.getSupportOnAll(MED_FACE)
elif (spaceDim == 2) :
    supportOnConst = mesh.getSupportOnAll(MED_EDGE)

##print ""
##print supportOnCell

if (spaceDim == 2) :
    fieldDoubleScalOnCell = createFieldDoubleFromAnalytic(supportOnCell,1,
                                                          f_scal_dbl_2d)
    fieldDoubleVectOnCell = createFieldDoubleFromAnalytic(supportOnCell,
                                                          spaceDim,
                                                          f_vect_dbl_2d)
    fieldIntScalOnCell = createFieldIntFromAnalytic(supportOnCell,1,
                                                    f_scal_int_2d)
    fieldIntVectOnCell = createFieldIntFromAnalytic(supportOnCell,spaceDim,
                                                    f_vect_int_2d)
elif (spaceDim == 3) :
    fieldDoubleScalOnCell = createFieldDoubleFromAnalytic(supportOnCell,1,
                                                          f_scal_dbl_3d)
    fieldDoubleVectOnCell = createFieldDoubleFromAnalytic(supportOnCell,
                                                          spaceDim,
                                                          f_vect_dbl_3d)
    fieldIntScalOnCell = createFieldIntFromAnalytic(supportOnCell,1,
                                                    f_scal_int_3d)
    fieldIntVectOnCell = createFieldIntFromAnalytic(supportOnCell,spaceDim,
                                                    f_vect_int_3d)

fieldDoubleScalOnCell.setName("Scalar Double Field on all Cells")
fieldDoubleScalOnCell.setDescription("Generated via a Python function")

fieldDoubleVectOnCell.setName("Vector Double Field on all Cells")
fieldDoubleVectOnCell.setDescription("Generated via a Python function")

fieldIntScalOnCell.setName("Scalar Integer Field on all Cells")
fieldIntScalOnCell.setDescription("Generated via a Python function")

fieldIntVectOnCell.setName("Vector Integer Field on all Cells")
fieldIntVectOnCell.setDescription("Generated via a Python function")

name = fieldDoubleScalOnCell.getName()
desc = fieldDoubleScalOnCell.getDescription()
nbOfComp = fieldDoubleScalOnCell.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldDoubleScalOnCell.getIterationNumber()
orderNb = fieldDoubleScalOnCell.getOrderNumber()
time = fieldDoubleScalOnCell.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldDoubleScalOnCell.getComponentName(kp1)
    compDesc = fieldDoubleScalOnCell.getComponentDescription(kp1)
    compUnit = fieldDoubleScalOnCell.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldDoubleScalOnCell.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldDoubleScalOnCell.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldDoubleVectOnCell.getName()
desc = fieldDoubleVectOnCell.getDescription()
nbOfComp = fieldDoubleVectOnCell.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldDoubleVectOnCell.getIterationNumber()
orderNb = fieldDoubleVectOnCell.getOrderNumber()
time = fieldDoubleVectOnCell.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldDoubleVectOnCell.getComponentName(kp1)
    compDesc = fieldDoubleVectOnCell.getComponentDescription(kp1)
    compUnit = fieldDoubleVectOnCell.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldDoubleVectOnCell.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldDoubleVectOnCell.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldIntScalOnCell.getName()
desc = fieldIntScalOnCell.getDescription()
nbOfComp = fieldIntScalOnCell.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldIntScalOnCell.getIterationNumber()
orderNb = fieldIntScalOnCell.getOrderNumber()
time = fieldIntScalOnCell.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldIntScalOnCell.getComponentName(kp1)
    compDesc = fieldIntScalOnCell.getComponentDescription(kp1)
    compUnit = fieldIntScalOnCell.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldIntScalOnCell.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldIntScalOnCell.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldIntVectOnCell.getName()
desc = fieldIntVectOnCell.getDescription()
nbOfComp = fieldIntVectOnCell.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldIntVectOnCell.getIterationNumber()
orderNb = fieldIntVectOnCell.getOrderNumber()
time = fieldIntVectOnCell.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldIntVectOnCell.getComponentName(kp1)
    compDesc = fieldIntVectOnCell.getComponentDescription(kp1)
    compUnit = fieldIntVectOnCell.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldIntVectOnCell.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldIntVectOnCell.getRow(k+1)
    print "     *",valueI[:nbOfComp]

##print ""
##print supportOnNode

if (spaceDim == 2) :
    fieldDoubleScalOnNode = createFieldDoubleFromAnalytic(supportOnNode,1,
                                                          f_scal_dbl_2d)
    fieldDoubleVectOnNode = createFieldDoubleFromAnalytic(supportOnNode,
                                                          spaceDim,
                                                          f_vect_dbl_2d)
    fieldIntScalOnNode = createFieldIntFromAnalytic(supportOnNode,1,
                                                    f_scal_int_2d)
    fieldIntVectOnNode = createFieldIntFromAnalytic(supportOnNode, spaceDim,
                                                    f_vect_int_2d)
elif (spaceDim == 3) :
    fieldDoubleScalOnNode = createFieldDoubleFromAnalytic(supportOnNode,1,
                                                          f_scal_dbl_3d)
    fieldDoubleVectOnNode = createFieldDoubleFromAnalytic(supportOnNode,
                                                          spaceDim,
                                                          f_vect_dbl_3d)
    fieldIntScalOnNode = createFieldIntFromAnalytic(supportOnNode,1,
                                                    f_scal_int_3d)
    fieldIntVectOnNode = createFieldIntFromAnalytic(supportOnNode, spaceDim,
                                                    f_vect_int_3d)

fieldDoubleScalOnNode.setName("Scalar Double Field on all Nodes")
fieldDoubleScalOnNode.setDescription("Generated via a Python function")

fieldDoubleVectOnNode.setName("Vector Double Field on all Nodes")
fieldDoubleVectOnNode.setDescription("Generated via a Python function")

fieldIntScalOnNode.setName("Scalar Integer Field on all Nodes")
fieldIntScalOnNode.setDescription("Generated via a Python function")

fieldIntVectOnNode.setName("Vector Integer Field on all Nodes")
fieldIntVectOnNode.setDescription("Generated via a Python function")

print ""
name = fieldDoubleScalOnNode.getName()
desc = fieldDoubleScalOnNode.getDescription()
nbOfComp = fieldDoubleScalOnNode.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldDoubleScalOnNode.getIterationNumber()
orderNb = fieldDoubleScalOnNode.getOrderNumber()
time = fieldDoubleScalOnNode.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldDoubleScalOnNode.getComponentName(kp1)
    compDesc = fieldDoubleScalOnNode.getComponentDescription(kp1)
    compUnit = fieldDoubleScalOnNode.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldDoubleScalOnNode.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldDoubleScalOnNode.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldDoubleVectOnNode.getName()
desc = fieldDoubleVectOnNode.getDescription()
nbOfComp = fieldDoubleVectOnNode.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldDoubleVectOnNode.getIterationNumber()
orderNb = fieldDoubleVectOnNode.getOrderNumber()
time = fieldDoubleVectOnNode.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldDoubleVectOnNode.getComponentName(kp1)
    compDesc = fieldDoubleVectOnNode.getComponentDescription(kp1)
    compUnit = fieldDoubleVectOnNode.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldDoubleVectOnNode.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldDoubleVectOnNode.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldIntScalOnNode.getName()
desc = fieldIntScalOnNode.getDescription()
nbOfComp = fieldIntScalOnNode.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldIntScalOnNode.getIterationNumber()
orderNb = fieldIntScalOnNode.getOrderNumber()
time = fieldIntScalOnNode.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldIntScalOnNode.getComponentName(kp1)
    compDesc = fieldIntScalOnNode.getComponentDescription(kp1)
    compUnit = fieldIntScalOnNode.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldIntScalOnNode.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldIntScalOnNode.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldIntVectOnNode.getName()
desc = fieldIntVectOnNode.getDescription()
nbOfComp = fieldIntVectOnNode.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldIntVectOnNode.getIterationNumber()
orderNb = fieldIntVectOnNode.getOrderNumber()
time = fieldIntVectOnNode.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldIntVectOnNode.getComponentName(kp1)
    compDesc = fieldIntVectOnNode.getComponentDescription(kp1)
    compUnit = fieldIntVectOnNode.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldIntVectOnNode.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldIntVectOnNode.getRow(k+1)
    print "     *",valueI[:nbOfComp]

##print ""
##print supportOnConst

if (spaceDim == 2) :
    fieldDoubleScalOnConst = createFieldDoubleFromAnalytic(supportOnConst,1,
                                                           f_scal_dbl_2d)
    fieldDoubleVectOnConst = createFieldDoubleFromAnalytic(supportOnConst,
                                                           spaceDim,
                                                           f_vect_dbl_2d)
    fieldIntScalOnConst = createFieldIntFromAnalytic(supportOnConst,1,
                                                     f_scal_int_2d)
    fieldIntVectOnConst = createFieldIntFromAnalytic(supportOnConst, spaceDim,
                                                     f_vect_int_2d)
elif (spaceDim == 3) :
    fieldDoubleScalOnConst = createFieldDoubleFromAnalytic(supportOnConst,1,
                                                           f_scal_dbl_3d)
    fieldDoubleVectOnConst = createFieldDoubleFromAnalytic(supportOnConst,
                                                           spaceDim,
                                                           f_vect_dbl_3d)
    fieldIntScalOnConst = createFieldIntFromAnalytic(supportOnConst,1,
                                                     f_scal_int_3d)
    fieldIntVectOnConst = createFieldIntFromAnalytic(supportOnConst, spaceDim,
                                                     f_vect_int_3d)

fieldDoubleScalOnConst.setName("Scalar Double Field on all Faces/Edges")
fieldDoubleScalOnConst.setDescription("Generated via a Python function")

fieldDoubleVectOnConst.setName("Vector Double Field on all Faces/Edges")
fieldDoubleVectOnConst.setDescription("Generated via a Python function")

fieldIntScalOnConst.setName("Scalar Integer Field on all Faces/Edges")
fieldIntScalOnConst.setDescription("Generated via a Python function")

fieldIntVectOnConst.setName("Vector Integer Field on all Faces/Edges")
fieldIntVectOnConst.setDescription("Generated via a Python function")

print ""
name = fieldDoubleScalOnConst.getName()
desc = fieldDoubleScalOnConst.getDescription()
nbOfComp = fieldDoubleScalOnConst.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldDoubleScalOnConst.getIterationNumber()
orderNb = fieldDoubleScalOnConst.getOrderNumber()
time = fieldDoubleScalOnConst.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldDoubleScalOnConst.getComponentName(kp1)
    compDesc = fieldDoubleScalOnConst.getComponentDescription(kp1)
    compUnit = fieldDoubleScalOnConst.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldDoubleScalOnConst.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldDoubleScalOnConst.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldDoubleVectOnConst.getName()
desc = fieldDoubleVectOnConst.getDescription()
nbOfComp = fieldDoubleVectOnConst.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldDoubleVectOnConst.getIterationNumber()
orderNb = fieldDoubleVectOnConst.getOrderNumber()
time = fieldDoubleVectOnConst.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldDoubleVectOnConst.getComponentName(kp1)
    compDesc = fieldDoubleVectOnConst.getComponentDescription(kp1)
    compUnit = fieldDoubleVectOnConst.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldDoubleVectOnConst.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldDoubleVectOnConst.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldIntScalOnConst.getName()
desc = fieldIntScalOnConst.getDescription()
nbOfComp = fieldIntScalOnConst.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldIntScalOnConst.getIterationNumber()
orderNb = fieldIntScalOnConst.getOrderNumber()
time = fieldIntScalOnConst.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldIntScalOnConst.getComponentName(kp1)
    compDesc = fieldIntScalOnConst.getComponentDescription(kp1)
    compUnit = fieldIntScalOnConst.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldIntScalOnConst.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldIntScalOnConst.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
name = fieldIntVectOnConst.getName()
desc = fieldIntVectOnConst.getDescription()
nbOfComp = fieldIntVectOnConst.getNumberOfComponents()
print "     Field",name," : ",desc
print "     Number Of Components:",nbOfComp
iterationNb = fieldIntVectOnConst.getIterationNumber()
orderNb = fieldIntVectOnConst.getOrderNumber()
time = fieldIntVectOnConst.getTime()
print "     Iteration Number",iterationNb
print "     Order Number",orderNb
print "     Time",time
for k in range(nbOfComp):
    kp1 = k+1
    compName = fieldIntVectOnConst.getComponentName(kp1)
    compDesc = fieldIntVectOnConst.getComponentDescription(kp1)
    compUnit = fieldIntVectOnConst.getMEDComponentUnit(kp1)
    print "      * Component:",kp1
    print "          Name:",compName
    print "          Description:",compDesc
    print "          Unit:",compUnit

support = fieldIntVectOnConst.getSupport()
nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
print "     Values:",nbOf
for k in range(nbOf):
    valueI = fieldIntVectOnConst.getRow(k+1)
    print "     *",valueI[:nbOfComp]

print ""
print "END of the Pyhton script ..... Ctrl D to exit"
