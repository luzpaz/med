import salome
import SALOME
import os

filePath=os.environ["MED_ROOT_DIR"]
filePath=filePath+"/share/salome/resources/med/"

import string

import SALOME_MED

from libSALOME_Swig import *
sg = SALOMEGUI_Swig()

def print_ord(i):
    if i == 0:
        return 'first'
    elif i == 1:
        return 'second'
    elif i == 2:
        return 'third'
    else:
        return `(i+1)`+'th'

def changeBlankToUnderScore(stringWithBlank):
    blank = ' '
    underscore = '_'
    decompString = string.split(stringWithBlank,blank)
    length = len(decompString)
    stringWithUnderScore = decompString[0]
    for i in range(1,length):
        stringWithUnderScore += underscore
        stringWithUnderScore += decompString[i]
    return stringWithUnderScore

def getMedObjectFromStudy(file):
    objNameInStudy = "MED_OBJECT_FROM_FILE_"+file
    compNameInStudy= "MED"
    listOfSO = salome.myStudy.FindObjectByName(objNameInStudy,compNameInStudy)
    listLength = len(listOfSO)
    if (listLength == 0) :
        print objNameInStudy," cannot be found in the Study under the component ",compNameInStudy
        return None
    elif (listLength > 1) :
        print "there are more than one instance of ",objNameInStudy," in the Study under the component ",compNameInStudy
        return None
    mySO = listOfSO[0]
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.MED)
        if (myObj == None) :
            print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

def getMeshObjectFromStudy(meshName):
    objNameInStudy = "/Med/MEDMESH/"+meshName
    mySO = salome.myStudy.FindObjectByPath(objNameInStudy)
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.MESH)
        if (myObj == None) :
            print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

def getSupportObjectFromStudy(meshName,supportName):
    meshNameStudy = changeBlankToUnderScore(meshName)
    objNameInStudy = "/Med/MEDMESH/MEDSUPPORTS_OF_"+meshNameStudy+"/"+supportName
    mySO = salome.myStudy.FindObjectByPath(objNameInStudy)
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.SUPPORT)
        if (myObj == None) :
            print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

def getFieldObjectFromStudy(dt,it,fieldName,supportName,meshName):
    meshNameStudy = changeBlankToUnderScore(meshName)
    objNameInStudy = "/Med/MEDFIELD/"+fieldName+"/("+str(dt)+","+str(it)+")_ON_"+supportName+"_OF_"+meshNameStudy
    mySO = salome.myStudy.FindObjectByPath(objNameInStudy)
    if (mySO == None) :
        print objNameInStudy," cannot be found in the Study"
        return mySO
    else:
        anAttr = mySO.FindAttribute("AttributeIOR")[1]
        obj = salome.orb.string_to_object(anAttr.Value())
        myObj = obj._narrow(SALOME_MED.FIELDINT)
        if (myObj == None):
            myObj = obj._narrow(SALOME_MED.FIELDDOUBLE)
            if (myObj == None) :
                print objNameInStudy," has been found in the Study but with the wrong type"
        return myObj

medFiles = []
medFiles.append("ChampsDarcy.med")
medFiles.append("darcy_1.1_res.med")
medFiles.append("darcy_1.3_resCASTEM.med")
medFiles.append("darcy_1.3_resPORFLOW.med")
medFiles.append("darcy_1.3_resTRACES.med")
medFiles.append("darcy2_Castem_EFMH.med")
medFiles.append("darcy2_Castem_qua_EFMH.med")
medFiles.append("darcy2_Castem_qua_VF.med")
medFiles.append("Deff_fdt_5.8_castem_efmh_diff_conc_dom.med")
medFiles.append("Deff_fdt_5.8_castem_vf_diff_conc_dom.med")
medFiles.append("extendedtransport53_triangles.med")
medFiles.append("H_CastCast_EFMH_I129_COUPLEX1.med")
medFiles.append("H_CastCast_VF_I129_COUPLEX1.med")
medFiles.append("H_CastCast_VF_Se79_COUPLEX1.med")
medFiles.append("H_CastPorf_I129_COUPLEX1.med")
medFiles.append("H_CastPorf_Se79_COUPLEX1.med")
medFiles.append("H_PorfCast_EFMH_I129_COUPLEX1.med")
medFiles.append("H_PorfCast_EFMH_Se79_COUPLEX1.med")
medFiles.append("H_PorfPorf_I129_COUPLEX1.med")
medFiles.append("H_Traces_I129_COUPLEX1.med")
medFiles.append("H_Traces_Se79_COUPLEX1.med")
medFiles.append("maillage_5_5_5.med")
medFiles.append("maillage_chemvalIV_cas1_40elts.med")
medFiles.append("Old_ChampsDarcy.med")
medFiles.append("Old_darcy_1.1_res.med")
medFiles.append("Old_darcy_1.3_resCASTEM.med")
medFiles.append("Old_darcy_1.3_resPORFLOW.med")
medFiles.append("Old_darcy_1.3_resTRACES.med")
medFiles.append("Old_darcy2_Castem_EFMH.med")
medFiles.append("Old_darcy2_Castem_qua_EFMH.med")
medFiles.append("Old_darcy2_Castem_qua_VF.med")
medFiles.append("Old_Deff_fdt_5.8_castem_efmh_diff_conc_dom.med")
medFiles.append("Old_Deff_fdt_5.8_castem_vf_diff_conc_dom.med")
medFiles.append("Old_H_CastCast_EFMH_I129_COUPLEX1.med")
medFiles.append("Old_H_CastCast_VF_I129_COUPLEX1.med")
medFiles.append("Old_H_CastCast_VF_Se79_COUPLEX1.med")
medFiles.append("Old_H_CastPorf_I129_COUPLEX1.med")
medFiles.append("Old_H_CastPorf_Se79_COUPLEX1.med")
medFiles.append("Old_H_PorfCast_EFMH_I129_COUPLEX1.med")
medFiles.append("Old_H_PorfCast_EFMH_Se79_COUPLEX1.med")
medFiles.append("Old_H_PorfPorf_I129_COUPLEX1.med")
medFiles.append("Old_H_PorfPorf_Se79_COUPLEX1.med")
medFiles.append("Old_H_Traces_I129_COUPLEX1.med")
medFiles.append("Old_H_Traces_Se79_COUPLEX1.med")
medFiles.append("Old_maillage_chemvalIV_cas1_40elts.med")

nbOfFiles = len(medFiles)

med = salome.lcc.FindOrLoadComponent("FactoryServer", "MED")

for i in range(nbOfFiles):
  medFile = medFiles[i]
  print "Testing with the file ",medFile
  medFile = filePath + medFile
  med.readStructFile(medFile,salome.myStudyName)

print "END of the Pyhton script ..... Ctrl D to exit"
