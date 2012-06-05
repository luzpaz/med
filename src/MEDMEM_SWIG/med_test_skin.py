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

#% Test function MESH::getSkin() on mesh from file cube_hexa8_quad4.med
#% The med file can be obtained by running create_mesh_c3h8q4 executable
#
from libMEDMEM_Swig import *
import os
#
#befor running this script, please be sure about the path the file fileName
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFile = os.path.join(filePath, "cube_hexa8_quad4.med")

print ""
print "Read file", medFile
print ""

md = MEDFILEBROWSER(medFile)

mesh_name = md.getMeshName(0)
mesh = MESH(MED_DRIVER,medFile,mesh_name)

print "Building the support on all (8) Cells of the mesh."
supportCell = mesh.getSupportOnAll(MED_CELL)

print "Getting skin of an all cell support"

supportSkin = mesh.getSkin(supportCell)
faceNumbers = supportSkin.getNumber( supportSkin.getTypes()[0] )
print "--------", faceNumbers
if faceNumbers != [2, 3, 6, 8, 10, 11, 12, 13, 16, 17, 19, 20, 22, 23, 24, 27, 28, 29, 30, 31, 32, 34, 35, 36]:
  raise  RuntimeError, "Wrong skin of an all cell support"
  
print "Build the support on 1 Cell (#8) of the mesh."
supportCell = SUPPORT( supportCell )
supportCell.setAll( 0 )
nbGeomTypes = 1
nbTotalEntity = 1
GeometricType = mesh.getTypes(MED_CELL)
nbEntityList = [1, 2]
EntityNbs = [8]
supportCell.setpartial("1 Cell support",nbGeomTypes,nbTotalEntity,GeometricType,nbEntityList,EntityNbs)

print "Getting skin of 1 cell support"
supportSkin = mesh.getSkin(supportCell)
faceNumbers = supportSkin.getNumber( supportSkin.getTypes()[0] )
print "Skin Support FACE numbers:"
print "--------", faceNumbers
if faceNumbers != [18, 26, 33, 34, 35, 36]:
  raise  RuntimeError, "Wrong skin of 1 cell support"



print "Building the support on 2 Cells (#1 #2) of the mesh."
nbTotalEntity = 2
nbEntityList = [1, 3]
EntityNbs = [1, 2]
supportCell.setpartial("2 Cell support",nbGeomTypes,nbTotalEntity,GeometricType,nbEntityList,EntityNbs)

print "Getting skin of 2 cell support"
supportSkin = mesh.getSkin(supportCell)
faceNumbers = supportSkin.getNumber( supportSkin.getTypes()[0] )
print "Skin Support FACE numbers:", faceNumbers
if faceNumbers != [1, 2, 3, 4, 6, 7, 8, 9, 10, 11]:
  raise  RuntimeError, "Wrong skin of 2 cells support"
