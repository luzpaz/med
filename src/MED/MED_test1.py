#==============================================================================
#  File      : MED_test1.py
#  Created   :
#  Author    : 
#  Project   : SALOME
#  Copyright : EDF 2002
#  $Header$
#==============================================================================

import salome

med=salome.lcc.FindOrLoadComponent("FactoryServer", "MED")

import os
filePath=os.getenv("SALOME_BUILD")
filePath=filePath+"/../SALOME_ROOT/data/"
fileName="pointe.med"
fileTout=filePath+fileName

print fileTout

med.readStructFile(fileTout,salome.myStudyName)

