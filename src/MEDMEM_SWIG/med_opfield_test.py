############################################################################
#
# this Python script is testing all operations between FIELD(DOUBLE,INT)
#
############################################################################

from libMEDMEM_Swig import *
from random import *

medFile = "pointe.med"

def add_one(i):
    return i+1

def print_ord(i):
    if i == 0:
        return 'first'
    elif i == 1:
        return 'second'
    elif i == 2:
        return 'third'
    else:
        return `i`+'th'

md = MED()

mdDriver = MED_MED_RDONLY_DRIVER(medFile,md)

mdDriver.open()
mdDriver.readFileStruct()
mdDriver.close()

nbMeshes = md.getNumberOfMeshes()

nbFields = md.getNumberOfFields()

print "The med file", medFile, "contains", nbMeshes, "mesh(es) and", nbFields, "field(s)"

if (nbMeshes>0):
    print "Mesh(es) Name(s) is(are) "

    for i in range(nbMeshes):
        mesh_name = md.getMeshName(i)
        print "   - ",mesh_name

if (nbFields>0):
    print "Field(s) Name(s) is(are) "

    for i in range(nbFields):
        field_name = md.getFieldName(i)
        print "   - ",field_name

print ""

mesh_name = md.getMeshName(0)
mesh = md.getMesh(mesh_name)
mesh.read()
spaceDim = mesh.getSpaceDimension()
meshDim = mesh.getMeshDimension()
nbNodes = mesh.getNumberOfNodes()
print "The mesh",mesh_name,"is a",spaceDim,"D mesh on a",meshDim,"D geometry and has",nbNodes,"Nodes"

if (nbFields>0):
    print "Updating supports in the Med Object"
    md.updateSupport()
    print ""
    print "Field(s) Analysis "
    for i in range(nbFields):
        print ""
        field_name = md.getFieldName(i)
        nbOfIt = md.getFieldNumberOfIteration(field_name)
        print "The",print_ord(i),"field is",field_name,"with",nbOfIt,"iteration(s)"
        for j in range(nbOfIt):
            print ""
            dtitfield = md.getFieldIteration(field_name,j)
            dt = dtitfield.getdt()
            it = dtitfield.getit()
            field = md.getField(field_name,dt,it)
            type = field.getValueType()
            print "     * Iteration:",dt,"Order number:",it,"Type:",type
            if type == MED_INT32:
                fieldint = createFieldIntFromField(field)
                fieldint.read()
                name = fieldint.getName()
                desc = fieldint.getDescription()
                nbOfComp = fieldint.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fieldint.getIterationNumber()
                orderNb = fieldint.getOrderNumber()
                time = fieldint.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
		print "Norme  2  : ", fieldint.norm2()
		print "Norme Max : ", fieldint.normMax()

		if fielddouble.getSupport().getEntity()!=MED_NODE:
		    fieldint_vol=fieldint.getSupport().getMesh().getVolume(fieldint.getSupport())
		    print "Norme L1  : ", fieldint.normL1()
		    print "Norme L2  : ", fieldint.normL2()
		    print "Norme L2(vol) : ", fieldint.normL2(fieldint_vol)

                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fieldint.getComponentName(kp1)
                    compDesc = fieldint.getComponentDescription(kp1)
                    compUnit = fieldint.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit
		    if fielddouble.getSupport().getEntity()!=MED_NODE:
			print "          Norme L1  : ", fieldint.normL1(kp1)
			print "          Norme L2  : ", fieldint.normL2(kp1)
			print "          Norme L2(vol) : ", fieldint.normL2(kp1,fieldint_vol)

                support = fieldint.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fieldint.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                fieldint2 = FIELDINT(fieldint)
                print ""
                fieldintadd = fieldint + fieldint2
                print "Test of the addition of two integer fields with creation a new one"
                print ""
                name = fieldintadd.getName()
                desc = fieldintadd.getDescription()
                nbOfComp = fieldintadd.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fieldintadd.getIterationNumber()
                orderNb = fieldintadd.getOrderNumber()
                time = fieldintadd.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fieldintadd.getComponentName(kp1)
                    compDesc = fieldintadd.getComponentDescription(kp1)
                    compUnit = fieldintadd.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fieldintadd.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fieldintadd.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                print ""
                fieldintsub = fieldint - fieldint2
                print "Test of the substraction of two integer fields with creation a new one"
                print ""
                name = fieldintsub.getName()
                desc = fieldintsub.getDescription()
                nbOfComp = fieldintsub.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fieldintsub.getIterationNumber()
                orderNb = fieldintsub.getOrderNumber()
                time = fieldintsub.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fieldintsub.getComponentName(kp1)
                    compDesc = fieldintsub.getComponentDescription(kp1)
                    compUnit = fieldintsub.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fieldintsub.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fieldintsub.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                print ""
                fieldintmul = fieldint * fieldint2
                print "Test of the multiplication of two integer fields with creation a new one"
                print ""
                name = fieldintmul.getName()
                desc = fieldintmul.getDescription()
                nbOfComp = fieldintmul.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fieldintmul.getIterationNumber()
                orderNb = fieldintmul.getOrderNumber()
                time = fieldintmul.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fieldintmul.getComponentName(kp1)
                    compDesc = fieldintmul.getComponentDescription(kp1)
                    compUnit = fieldintmul.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fieldintmul.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fieldintmul.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                print ""
                fieldintdiv = fieldint / fieldint2
                print "Test of the division of two integer fields with creation a new one"
                print ""
                name = fieldintdiv.getName()
                desc = fieldintdiv.getDescription()
                nbOfComp = fieldintdiv.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fieldintdiv.getIterationNumber()
                orderNb = fieldintdiv.getOrderNumber()
                time = fieldintdiv.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fieldintdiv.getComponentName(kp1)
                    compDesc = fieldintdiv.getComponentDescription(kp1)
                    compUnit = fieldintdiv.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fieldintdiv.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fieldintdiv.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
            elif type == MED_REEL64:
                fielddouble = createFieldDoubleFromField(field)
                fielddouble.read()
                name = fielddouble.getName()
                desc = fielddouble.getDescription()
                nbOfComp = fielddouble.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fielddouble.getIterationNumber()
                orderNb = fielddouble.getOrderNumber()
                time = fielddouble.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
		print "Norme  2  : ", fielddouble.norm2()
		print "Norme Max : ", fielddouble.normMax()
		print "try sobolev",fielddouble.getSupport().getEntity()
		if fielddouble.getSupport().getEntity()!=MED_NODE:
		    fielddouble_vol=fielddouble.getSupport().getMesh().getVolume(fielddouble.getSupport())
		    print "Norme L1  : ", fielddouble.normL1()
		    print "Norme L2  : ", fielddouble.normL2()
		    print "Norme L2(vol) : ", fielddouble.normL2(fielddouble_vol)
		    
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fielddouble.getComponentName(kp1)
                    compDesc = fielddouble.getComponentDescription(kp1)
                    compUnit = fielddouble.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit
		    if fielddouble.getSupport().getEntity()!=MED_NODE:
			print "          Norme L1  : ", fielddouble.normL1(kp1)
			print "          Norme L2  : ", fielddouble.normL2(kp1)
			print "          Norme L2(vol) : ", fielddouble.normL2(kp1, fielddouble_vol)

                support = fielddouble.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fielddouble.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                fielddouble2 = FIELDDOUBLE(fielddouble)
                print ""
                fielddoubleadd = fielddouble + fielddouble2
                print "Test of the addition of two double fields with creation a new one"
                print ""
                name = fielddoubleadd.getName()
                desc = fielddoubleadd.getDescription()
                nbOfComp = fielddoubleadd.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fielddoubleadd.getIterationNumber()
                orderNb = fielddoubleadd.getOrderNumber()
                time = fielddoubleadd.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fielddoubleadd.getComponentName(kp1)
                    compDesc = fielddoubleadd.getComponentDescription(kp1)
                    compUnit = fielddoubleadd.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fielddoubleadd.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fielddoubleadd.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                print ""
                fielddoublesub = fielddouble - fielddouble2
                print "Test of the substraction of two double fields with creation a new one"
                print ""
                name = fielddoublesub.getName()
                desc = fielddoublesub.getDescription()
                nbOfComp = fielddoublesub.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fielddoublesub.getIterationNumber()
                orderNb = fielddoublesub.getOrderNumber()
                time = fielddoublesub.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fielddoublesub.getComponentName(kp1)
                    compDesc = fielddoublesub.getComponentDescription(kp1)
                    compUnit = fielddoublesub.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fielddoublesub.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fielddoublesub.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                print ""
                fielddoublemul = fielddouble * fielddouble2
                print "Test of the multiplication of two double fields with creation a new one"
                print ""
                name = fielddoublemul.getName()
                desc = fielddoublemul.getDescription()
                nbOfComp = fielddoublemul.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fielddoublemul.getIterationNumber()
                orderNb = fielddoublemul.getOrderNumber()
                time = fielddoublemul.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fielddoublemul.getComponentName(kp1)
                    compDesc = fielddoublemul.getComponentDescription(kp1)
                    compUnit = fielddoublemul.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fielddoublemul.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fielddoublemul.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
                print ""
                fielddoublediv = fielddouble / fielddouble2
                print "Test of the division of two double fields with creation a new one"
                print ""
                name = fielddoublediv.getName()
                desc = fielddoublediv.getDescription()
                nbOfComp = fielddoublediv.getNumberOfComponents()
                print "     Field",name," : ",desc
                print "     Number Of Components:",nbOfComp
                iterationNb = fielddoublediv.getIterationNumber()
                orderNb = fielddoublediv.getOrderNumber()
                time = fielddoublediv.getTime()
                print "     Iteration Number",iterationNb
                print "     Order Number",orderNb
                print "     Time",time
                for k in range(nbOfComp):
                    kp1 = k+1
                    compName = fielddoublediv.getComponentName(kp1)
                    compDesc = fielddoublediv.getComponentDescription(kp1)
                    compUnit = fielddoublediv.getMEDComponentUnit(kp1)
                    print "      * Component:",kp1
                    print "          Name:",compName
                    print "          Description:",compDesc
                    print "          Unit:",compUnit

                support = fielddoublediv.getSupport()
                nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                print "     Values:",nbOf
                for k in range(nbOf):
                    valueI = fielddoublediv.getValueI(MED_FULL_INTERLACE,k+1)
                    print "     *",valueI[:nbOfComp]
            else:
                print "  !!!! Bad type of Field !!!!"

print ""
print "TESTS OPERATIONS SUR FIELDDOUBLE : "


fielddoublesub = fielddouble-fielddouble2
fielddoublemul = fielddouble*fielddouble2
fielddoublediv = fielddouble/fielddouble2
fielddoubleasso = fielddouble+fielddouble2*fielddouble2
fielddoubleSP=createFieldDoubleScalarProduct(fielddouble, fielddouble2)


print " f1     : ",fielddouble.getValue(MED_FULL_INTERLACE)
print " f2     : ",fielddouble2.getValue(MED_FULL_INTERLACE)
print "--------------------------------------------------------------------------------------------------------------"
print "  +     : ",fielddoubleadd.getValue(MED_FULL_INTERLACE)
print "  -     : ",fielddoublesub.getValue(MED_FULL_INTERLACE)
print "  *     : ",fielddoublemul.getValue(MED_FULL_INTERLACE)
print "  /     : ",fielddoublediv.getValue(MED_FULL_INTERLACE)

fielddouble+=fielddouble2;
print "  +=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
fielddouble-=fielddouble2;
print "  -=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
fielddouble*=fielddouble2;
print "  *=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
fielddouble/=fielddouble2;
print "  /=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
print "f1+f2*f2: ",fielddoubleasso.getValue(MED_FULL_INTERLACE)
fielddouble.applyLin(4,1);
print " 4f1+1  : ",fielddouble.getValue(MED_FULL_INTERLACE)
print " f1.f2  : ",fielddoubleSP.getValue(MED_FULL_INTERLACE)
fielddouble2.applyPyFunc(add_one)
print " CB:f2+1: ",fielddouble2.getValue(MED_FULL_INTERLACE)

print ""
print "TESTS OPERATIONS SUR FIELDINT : "


fieldintadd = fieldint+fieldint2
fieldintsub = fieldint-fieldint2
fieldintmul = fieldint*fieldint2
fieldintdiv = fieldint/fieldint2
fieldintasso = fieldint+fieldint*fieldint
fieldintSP=createFieldIntScalarProduct(fieldint, fieldint2)

print " f1     : ",fieldint.getValue(MED_FULL_INTERLACE)
print " f2     : ",fieldint2.getValue(MED_FULL_INTERLACE)
print "--------------------------------------------------------------------------------------------------------------"
print "  +     : ",fieldintadd.getValue(MED_FULL_INTERLACE)
print "  -     : ",fieldintsub.getValue(MED_FULL_INTERLACE)
print "  *     : ",fieldintmul.getValue(MED_FULL_INTERLACE)
print "  /     : ",fieldintdiv.getValue(MED_FULL_INTERLACE)
fieldint+=fieldint2;
print "  +=    : ",fieldint.getValue(MED_FULL_INTERLACE)
fieldint-=fieldint2;
print "  -=    : ",fieldint.getValue(MED_FULL_INTERLACE)
fieldint*=fieldint2;
print "  *=    : ",fieldint.getValue(MED_FULL_INTERLACE)
fieldint/=fieldint2;
print "  /=    : ",fieldint.getValue(MED_FULL_INTERLACE)
print "f1+f2*f2: ",fieldintasso.getValue(MED_FULL_INTERLACE)
fieldint.applyLin(4,1);
print " 4f1+1  : ",fieldint.getValue(MED_FULL_INTERLACE)
print " f1.f2  : ",fieldintSP.getValue(MED_FULL_INTERLACE)
fieldint2.applyPyFunc(add_one)
print " CB:f2+1: ",fieldint2.getValue(MED_FULL_INTERLACE)
myname="toto"
fieldint.setName(myname)
name = fieldint.getName()
print "test de setName : ",name
print "END of the Pyhton script ..... Ctrl D to exit"
