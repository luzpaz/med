import salome
import SALOME_TESTMEDCLIENT
import SALOME_MED

def getMedObjectFromStudy():
    mySO = salome.myStudy.FindObject("Objet MED")
    Builder = salome.myStudy.NewBuilder()
    anAttr = Builder.FindOrCreateAttribute(mySO, "AttributeIOR")
    obj = salome.orb.string_to_object(anAttr.Value())
    myObj = obj._narrow(SALOME_MED.MED)
    return myObj

#Truc1,Truc2 are Containers launched with SALOME_Container exe.

med_comp = salome.lcc.FindOrLoadComponent("Truc1", "MED")
my_comp = salome.lcc.FindOrLoadComponent("Truc2","TESTMEDCLIENT")
studyCurrent = salome.myStudyName

## First test

##med_obj = med_comp.readStructFile("cube_tetra4_12.med",studyCurrent)
##my_comp.go2(med_obj)

## Second test

med_obj = med_comp.readStructFile("cube_hexa8_quad4.med",studyCurrent)
my_comp.go(med_obj)
