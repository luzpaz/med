###################################################################################
#
# This Python script is testing the mounting in memory of a med file,
# via the object MED and the writing of this object MED in Med file format
# V2.1 V2.2
#
# Then from the 2 file produced before, a mounting and a writing of those files in
# different Med file format is tested
#
###################################################################################

from libMEDMEM_Swig import *
import string
import os
#
#befor running this script, please be sure about the path the files
#
filePath=os.environ["MED_ROOT_DIR"]
filePath=filePath+"/share/salome/resources/"

medFile = "pointe.med"

medFile = filePath + medFile
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
