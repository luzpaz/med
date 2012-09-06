#  -*- coding: iso-8859-1 -*-
#
# This file is a set of basic use case to test (from the python
# context) the functions developped in XMED and MED modules for the
# field operations.
#
# (gboulant - 20/10/2010)
#
import salome
salome.salome_init()

from xmed import properties
from xmed import SALOMEMED_helper
from xmed import fieldproxy
from xmed import medproxy

medComp=salome.lcc.FindOrLoadComponent("FactoryServer", "MED")
filePathName = properties.testFilePath

print "Reading the file ",filePathName,". NOTE: the data ARE NOT pushed in the SALOME study"
import SALOME_MED
medObj = medComp.readStructFile(filePathName,salome.myStudyName)


def TEST_getField():
    salomefield = medObj.getField("testfield1",-1,-1)
    SALOMEMED_helper.analyzeField(salomefield)

def TEST_usingIOR():
    # This illustrates the usage of IOR to manage the corba
    # object. The IOR is the CORBA serialization of an object.
    # To get the IOR of an object or the object from an IOR, we need
    # the orb. In the salome python context, the orb can be obtained
    # from the salome module.
    medIOR = salome.orb.object_to_string(medObj)
    print "medIOR = ", medIOR
    # The IOR can be used to restore a reference to this object
    anOtherRefToMedObj = salome.orb.string_to_object(medIOR)
    print "mesh names=", anOtherRefToMedObj.getMeshNames()
    # Note that the IOR is NOT a persistent information, it is just a
    # serialization of the reference to the instance of the object.

def TEST_MedOp_basics():
    medOp = medObj.createMedOperator()
    medOp.test()

def TEST_MedOp_addition():
    f1 = medObj.getField("testfield1",-1,-1)
    f2 = medObj.getField("testfield2",-1,-1)

    medOp = medObj.createMedOperator()
    r = medOp.add(f1,f2)
    print r.getName()
    
def TEST_fieldproxy_basics():
    # The constructor of a field proxy works with a medIOR
    medIOR = salome.orb.object_to_string(medObj)

    fieldProxy = fieldproxy.getFieldFromMedIOR(medIOR, "testfield1", -1, -1)

    # The field proxy can be used as a SALOME_MED::FIELD, for example:
    print fieldProxy.getValue(SALOME_MED.MED_FULL_INTERLACE)
    print fieldProxy.getName()
    SALOMEMED_helper.analyzeField(fieldProxy)

def TEST_fieldproxy_addition():
    medIOR = salome.orb.object_to_string(medObj)

    f1 = fieldproxy.getFieldFromMedIOR(medIOR, "testfield1", -1, -1)
    f2 = fieldproxy.getFieldFromMedIOR(medIOR, "testfield2", -1, -1)
    r = f1 + f2

    # We can check if the result is in the med data structure
    print medObj.getFieldNames()


def TEST_MedOp_export2med():
    """
    Illustrates the basic code to export the result in a med file.
    """
    medIOR = salome.orb.object_to_string(medObj)
    f1 = fieldproxy.getFieldFromMedIOR(medIOR, "testfield1", -1, -1)
    f2 = fieldproxy.getFieldFromMedIOR(medIOR, "testfield2", -1, -1)
    r = f1 + f2

    outputMedFileId = medObj.addDriver(SALOME_MED.MED_DRIVER,"/tmp/output.med")
    medObj.write(outputMedFileId)
    

def TEST_MedOp_visualize():
    """
    Illustrates the basic code to visualize the result field    
    """
    medIOR = salome.orb.object_to_string(medObj)
    f1 = fieldproxy.getFieldFromMedIOR(medIOR, "testfield1", -1, -1)
    f2 = fieldproxy.getFieldFromMedIOR(medIOR, "testfield2", -1, -1)
    r = f1 + f2

    r.visu()
    
def TEST_medproxy_save():
    f1 = fieldproxy.getFieldFromMed(medObj, "testfield1", -1, -1)
    f2 = fieldproxy.getFieldFromMed(medObj, "testfield2", -1, -1)
    r = f1 + f2

    med = medproxy.MedProxy(medObj)
    med.save("/tmp/output.med")
    

if __name__ == "__main__":
    #TEST_getField()
    #TEST_usingIOR()
    #TEST_MedOp_basics()
    #TEST_MedOp_addition()
    #TEST_fieldproxy_basics()
    #TEST_fieldproxy_addition()
    #TEST_MedOp_export2med()
    #TEST_medproxy_save()
    
    pass
