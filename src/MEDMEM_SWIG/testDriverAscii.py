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

from libMEDMEM_Swig import *

import os

#
#before running this script, please be sure about the path the file fileName
#

filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFile = os.path.join(filePath, "test_2D.med")

asciiFile = "tyst.txt"

md=MEDFILEBROWSER(medFile)

nbMeshes = md.getNumberOfMeshes()

nbFields = md.getNumberOfFields()

print "The med file", medFile, "contains", nbMeshes, "mesh(es) and", nbFields, "field(s)"
f1Name=md.getFieldName(0)
mesh_name=md.getMeshName(f1Name)
mesh=MESH(MED_DRIVER,medFile,mesh_name)
it=md.getFieldIteration(f1Name)[0]
f1=FIELDDOUBLE(MED_DRIVER,medFile,f1Name,it.dt,it.it,mesh)
id2=f1.addDriver(ASCII_DRIVER,asciiFile,"Default Field Name",MED_ECRI)
f1.write(id2)

print "END of the Pyhton script ..... Ctrl D to exit"
