#  -*- coding: iso-8859-1 -*-
#  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

###################################################################################
# This Python script is testing the mounting in memory of a med file,
# via the object MED and the writing of this object MED in Med file format
# V2.1 V2.2
# Then from the 2 file produced before, a mounting and a writing of those files in
# different Med file format is tested
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

medFile = "pointe.med"

medFile = os.path.join(filePath, medFile)
rootFile = string.split(medFile,".")[0]
medFileOut21 = rootFile + "_Out21.med"
medFileOut22 = rootFile + "_Out22.med"

print "Mounting in memory of the MED object from the file ",medFile
medObj = MED(MED_DRIVER,medFile)

print ""
print "Reading the object"
medObj.read()

medFileVersion = getMedFileVersionForWriting()

if (medFileVersion == V22):
    setMedFileVersionForWriting(V21)
    pass

print ""
print "Adding a med file V2.1 format driver to the MED object with the file ",medFileOut21
idMedV21 = medObj.addDriver(MED_DRIVER,medFileOut21)

print ""
print "Writing the MED object in the med file V2.1 format file"
medObj.write(idMedV21)

medFileVersion = getMedFileVersionForWriting()
if (medFileVersion == V21):
    setMedFileVersionForWriting(V22)
    pass

print ""
print "Adding a med file V2.2 format driver to the MED object with the file ",medFileOut22
idMedV22 = medObj.addDriver(MED_DRIVER,medFileOut22)

print ""
print "Writing the MED object in med file V2.2 format file"
medObj.write(idMedV22)

print ""
print "Mounting in memory of the two files produced and writing them again"

medFiles = []
medFiles.append(medFileOut21)
medFiles.append(medFileOut22)
nbOfFiles = len(medFiles)

for i in range(nbOfFiles):
    medFile = medFiles[i]

    rootFile = string.split(medFile,".")[0]
    medFileOut21 = rootFile + "_Out21.med"
    medFileOut22 = rootFile + "_Out22.med"

    print ""
    print "   Mounting in memory of the MED object from the file ",medFile
    medObj = MED(MED_DRIVER,medFile)

    print ""
    print "   Reading the object"
    medObj.read()

    medFileVersion = getMedFileVersionForWriting()

    if (medFileVersion == V22):
        setMedFileVersionForWriting(V21)
        pass

    print ""
    print "   Adding a med file V2.1 format driver to the MED object with the file ",medFileOut21
    idMedV21 = medObj.addDriver(MED_DRIVER,medFileOut21)

    print ""
    print "   Writing the MED object in the med file V2.1 format file"
    medObj.write(idMedV21)

    medFileVersion = getMedFileVersionForWriting()
    if (medFileVersion == V21):
        setMedFileVersionForWriting(V22)
        pass

    print ""
    print "   Adding a med file V2.2 format driver to the MED object with the file ",medFileOut22
    idMedV22 = medObj.addDriver(MED_DRIVER,medFileOut22)

    print ""
    print "   Writing the MED object in med file V2.2 format file"
    medObj.write(idMedV22)
    pass

print ""
print "END of the Pyhton script ..... Ctrl D to exit"
