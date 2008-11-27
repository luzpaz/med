#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
from libMEDMEM_Swig import *

import os

#
#before running this script, please be sure about the path the file fileName
#

filePath=os.environ["MED_ROOT_DIR"]
filePath=filePath+"/share/salome/resources/med/"

medFile = filePath + "test_2D.med"

asciiFile = "tyst.txt"

md=MED()
dr=MED_MED_RDONLY_DRIVER(medFile,md)

dr.open()
dr.readFileStruct()
dr.read()
dr.close()

nbMeshes = md.getNumberOfMeshes()

nbFields = md.getNumberOfFields()

print "The med file", medFile, "contains", nbMeshes, "mesh(es) and", nbFields, "field(s)"
f1Name=md.getFieldName(0)
f1It=md.getFieldNumberOfIteration(f1Name)
it=md.getFieldIteration(f1Name,0)
f1=md.getField(f1Name,it.dt,it.it)
f1=createFieldDoubleFromField(f1)
#f1.read()
id2=f1.addDriver(ASCII_DRIVER,asciiFile,"Default Field Name",MED_ECRI)
#dr=ASCII_FIELDDOUBLE_DRIVER(asciiFile,f1,DESCENDING)
#dr.open()
#dr.write()
#dr.close()
##print f1It
f1.write(id2)

print "END of the Pyhton script ..... Ctrl D to exit"
