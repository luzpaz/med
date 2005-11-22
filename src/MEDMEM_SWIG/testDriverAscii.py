from libMEDMEM_Swig import *

import os

#
#before running this script, please be sure about the path the file fileName
#

filePath=os.environ["MED_ROOT_DIR"]
filePath=filePath+"/share/salome/resources/"

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
