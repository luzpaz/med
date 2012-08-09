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

###################################################################################
# This Python script is testing the writing in Med format V2.1 V2.2
# You can check if the analysis of the families is OK.
###################################################################################
#
from libMEDMEM_Swig import *
import string
import os
#
#befor running this script, please be sure about the path the files
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=os.path.join(filePath, "share", "salome", "resources", "med")

medFiles = []
#medFiles.append("geomMesh21.med")
medFiles.append("geomMesh22.med")
nbOfFiles = len(medFiles)

for i in range(nbOfFiles):
    medFile = medFiles[i]
    medFile = os.path.join( filePath, medFile )
    medFiles[i] = medFile
    pass

meshName = "GeomMesh"

for i in range(nbOfFiles):
    medFile = medFiles[i]

    rootFile = string.split(medFile,".")[0]
    medFileOut21 = rootFile + "Out21.med"
    medFileOut22 = rootFile + "Out22.med"

    print "Mounting in memory of the mesh ",meshName," from the file ",medFile
    mesh = MESH(MED_DRIVER,medFile,meshName)
    
    print ""
    print "Families analysis of the mesh ",meshName," from the file ",medFile
    print ""

    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        if (entity == MED_NODE) & (nbFam > 0):
            print "This mesh has",nbFam,"Node Family(ies)"
            pass
        elif (entity == MED_CELL) & (nbFam > 0):
            print "This mesh has",nbFam,"Cell Family(ies)"
            pass
        elif (entity == MED_FACE) & (nbFam > 0):
            print "This mesh has",nbFam,"Face Family(ies)"
            pass
        elif (entity == MED_EDGE) & (nbFam > 0):
            print "This mesh has",nbFam,"Edge Family(ies)"
            pass

        if nbFam > 0:
            for j in range(nbFam):
                print ""
                family = mesh.getFamily(entity,j+1)
                familyName = family.getName()
                familyBool = family.isOnAllElements()
                if (familyBool):
                    print "  -this famyly named ",familyName," is on all entities"
                    pass
                else:
                    familyNumber = family.getNumber(MED_ALL_ELEMENTS)
                    print "  -this famyly named ",familyName," has ",len(familyNumber)," entities"
                    pass
                pass
            pass
        pass
    
    print ""
    print "  adding a Med file V2.2 format driver to the mesh with the file ",medFileOut22
    idMedV22 = mesh.addDriver(MED_DRIVER,medFileOut22,mesh.getName())

    print ""
    print "  writing the mesh in Med file V2.2 format"
    mesh.write(idMedV22)
    print ""
    pass

medFiles2 = []

for i in range(nbOfFiles):
    medFile = medFiles[i]
    rootFile = string.split(medFile,".")[0]
    medFileOut22 = rootFile + "Out22.med"

    medFiles2.append(medFileOut22)
    pass

nbOfFiles = len(medFiles2)

for i in range(nbOfFiles):
    medFile = medFiles2[i]
    mesh = MESH(MED_DRIVER,medFile,meshName)

    print "Families analysis of the mesh ",meshName," from the file ",medFile
    print ""
    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        if (entity == MED_NODE) & (nbFam > 0):
            print "This mesh has",nbFam,"Node Family(ies)"
            pass
        elif (entity == MED_CELL) & (nbFam > 0):
            print "This mesh has",nbFam,"Cell Family(ies)"
            pass
        elif (entity == MED_FACE) & (nbFam > 0):
            print "This mesh has",nbFam,"Face Family(ies)"
            pass
        elif (entity == MED_EDGE) & (nbFam > 0):
            print "This mesh has",nbFam,"Edge Family(ies)"
            pass

        if nbFam > 0:
            for j in range(nbFam):
                print ""
                family = mesh.getFamily(entity,j+1)
                familyName = family.getName()
                familyBool = family.isOnAllElements()
                if (familyBool):
                    print "  -this famyly named ",familyName," is on all entities"
                    pass
                else:
                    familyNumber = family.getNumber(MED_ALL_ELEMENTS)
                    print "  -this famyly named ",familyName," has ",len(familyNumber)," entities"
                    pass
                pass
            pass
        pass
    pass

print ""
print "END of the Pyhton script ..... Ctrl D to exit"
