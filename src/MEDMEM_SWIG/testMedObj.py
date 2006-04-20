###################################################################################
#
# This Python script is testing the mounting in memory of a med file,
# via the object MED and the writing of this object MED in Med file format
# V2.1 V2.2
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
medFileOut21 = rootFile + "Out21.med"
medFileOut22 = rootFile + "Out22.med"

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
print "  adding a Med file V2.1 format driver to the MED object with the file ",medFileOut21
idMedV21 = medObj.addDriver(MED_DRIVER,medFileOut21)

print ""
print "  writing the mesh in Med file V2.1 format"
medObj.write(idMedV21)

medFileVersion = getMedFileVersionForWriting()
if (medFileVersion == V21):
    setMedFileVersionForWriting(V22)
    pass

print ""
print "  adding a Med file V2.2 format driver to the mesh with the file ",medFileOut22
idMedV22 = medObject.addDriver(MED_DRIVER,medFileOut22)

print ""
print "  writing the mesh in Med file V2.2 format"
mesh.write(idMedV22)

print ""
print "END of the Pyhton script ..... Ctrl D to exit"
