#==============================================================================
#  File      : MED_test2.py
#  Created   :
#  Author    : 
#  Project   : SALOME
#  Copyright : EDF 2002
#  $Header$
#==============================================================================

import salome
import SALOME
import os

filePath=os.getenv("SALOME_BUILD")
filePath=filePath+"/../SALOME_ROOT/data/"

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

