#  MED MED : implemetation of MED idl descriptions
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : MED_test2.py
#  Author : 
#  Module : MED
#  $Header$

import salome
import SALOME
import os

filePath=os.getenv("SALOME_BUILD")
filePath=filePath+"/data/"

med=salome.lcc.FindOrLoadComponent("FactoryServer", "Med")

try:
  mesh=med.readMeshInFile(filePath+"/mesh.med", salome.myStudyName,"Mesh 1")
except SALOME.SALOME_Exception, ex:
  print ex.details
  print ex.details.type
  print ex.details.text
  print ex.details.sourceFile
  print ex.details.lineNumber

  raise

print mesh.getName()
print mesh.getNumberOfNodes()

try:
  field=med.readFieldInFile(filePath+"/pointe.med",salome.myStudyName,"fieldcelldouble",-1,-1)
except SALOME.SALOME_Exception, ex:
  print ex.details
  print ex.details.type
  print ex.details.text
  print ex.details.sourceFile
  print ex.details.lineNumber

  raise

print field.getName()
print field.getDescription()
print field.getNumberOfComponents()

