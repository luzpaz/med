###################################################################################
#
# This Python script is testing all functionalities of the Med Memory through its
# Python API; for this purpose a battery of med file as well as gibi files are
# scanned using the parser written in the Python script med_test1.py with some
# addings to test other functionality written in othe Python scripts in the bin
# directory of the installation of MED
#
###################################################################################

from libMEDMEM_Swig import *
from random import *

import sys,os,string

filesPath=os.environ["MED_ROOT_DIR"]
filesPath=filesPath+"/share/salome/resources/"

files = []
meshNameFiles = []

def print_ord(i):
    if i == 0:
        return 'first'
    elif i == 1:
        return 'second'
    elif i == 2:
        return 'third'
    else:
        return `i`+'th'

def add_one(i):
    return i+1

###################################################################################
#
# Here is the part you should change to add another file to the test files battery
#
###################################################################################
#
# med file list
#

##files.append("maill.0.med")
##meshNameFiles.append("MAILTRQU")

files.append("maillage_UniSegFam.med")
meshNameFiles.append("maillage_CHEMVAL_100elts")

##files.append("mesh.med")
##meshNameFiles.append("Mesh 1")

##files.append("zzzz121b.med")
##meshNameFiles.append("MUN")

files.append("carre_en_quad4.med")
meshNameFiles.append("carre_en_quad4")

files.append("cube_hexa8.med")
meshNameFiles.append("CUBE_EN_HEXA8")

files.append("test19.med")
meshNameFiles.append("CartGrid")

files.append("test19.med")
meshNameFiles.append("bodyfitted")

files.append("test19.med")
meshNameFiles.append("maa1")

files.append("carre_en_quad4_seg2.med")
meshNameFiles.append("carre_en_quad4_seg2")

files.append("cube_hexa8_quad4.med")
meshNameFiles.append("CUBE_EN_HEXA8_QUAD4")

files.append("pointe.med")
meshNameFiles.append("maa1")

#
# Castem or Gibi file list
#

files.append("Darcy3_3D_H_10x10x10.sauve")
meshNameFiles.append("")

files.append("dx200_dy1_avec_2couches.sauve")
meshNameFiles.append("")

files.append("elle_2D_QT_10x10.sauve")
meshNameFiles.append("")

files.append("elle_2D_QT_2x2.sauve")
meshNameFiles.append("")

files.append("elle_2D_QT_40x40.sauve")
meshNameFiles.append("")

files.append("elle_2D_QT_4x4.sauve")
meshNameFiles.append("")

files.append("elle_3D_HPr_10x10x10.sauve")
meshNameFiles.append("")

files.append("elle_3D_HPr_2x2x2.sauve")
meshNameFiles.append("")

files.append("elle_3D_HPr_4x4x4.sauve")
meshNameFiles.append("")

files.append("inclusion_2d_raf.sauve")
meshNameFiles.append("")

files.append("inclusion_2d.sauve")
meshNameFiles.append("")

files.append("mail_ktest1-3-hexa.sauve")
meshNameFiles.append("")

files.append("mail_ktest1-3-tetra.sauve")
meshNameFiles.append("")

files.append("mail_ktest3-1.sauve")
meshNameFiles.append("")

files.append("mail_ktest3-2.sauve")
meshNameFiles.append("")

files.append("maillage_andra2_100elts.sauve")
meshNameFiles.append("")

files.append("maillage_cas2_2d.sauve")
meshNameFiles.append("")

files.append("maillage_cas4_234elts.sauve")
meshNameFiles.append("")

files.append("maillage_CHEMVAL_100elts.sauve")
meshNameFiles.append("")

files.append("maillage_CHEMVAL_40elts.sauve")
meshNameFiles.append("")

files.append("maillage_chemvalIV_cas1_100elts.sauve")
meshNameFiles.append("")

files.append("maillage_chemvalIV_cas1_40elts.sauve")
meshNameFiles.append("")

files.append("mail_test1-1-qua.sauve")
meshNameFiles.append("")

files.append("mail_test1-1-tri.sauve")
meshNameFiles.append("")

files.append("mail_test1-2-qua.sauve")
meshNameFiles.append("")

files.append("mail_test1-2-tri.sauve")
meshNameFiles.append("")

files.append("mail-test1-4-1.sauve")
meshNameFiles.append("")

files.append("mail-test1-4-2.sauve")
meshNameFiles.append("")

#
# Porflow file list
#

files.append("boitenew.inp")
meshNameFiles.append("")

files.append("Case1.inp")
meshNameFiles.append("")

files.append("cube.inp")
meshNameFiles.append("")

files.append("test3.inp")
meshNameFiles.append("")

files.append("titi.inp")
meshNameFiles.append("")

###################################################################################

nbOfFiles = len(files)

for i in range(nbOfFiles):
    files[i] = filesPath + files[i]

###################################################################################
#
# Loop on all files
#
###################################################################################

for i in range(nbOfFiles):
    file = files[i]
    decompFile = string.split(file,".")
    lenDecompFileM1 = len(decompFile)-1

    if (lenDecompFileM1 == 0) :
        print "testMedMemGeneral The file ",file," should have at least a . in its name "
        sys.exit(1)

    extensionFile = decompFile[lenDecompFileM1]

    rootFile = decompFile[0]
    for k in range(1,lenDecompFileM1):
        rootFile = rootFile + "." + decompFile[k]

    medFileName = rootFile + "_test.med"
    vtkFileName = rootFile + "_test.vtk"

    try:
        mesh = MESH()
        if (extensionFile == "med"):
            print "testMedMemGeneral The file ",file," is a MED file and the name of the mesh is ", meshNameFiles[i]
            meshDriver = MED_MESH_RDONLY_DRIVER(file,mesh)
            meshDriver.setMeshName(meshNameFiles[i])
        elif (extensionFile == "sauve"):
            print "testMedMemGeneral The file ",file," is a GIBI file"
            meshDriver = GIBI_MESH_RDONLY_DRIVER(file,mesh)
        elif (extensionFile == "inp"):
            print "testMedMemGeneral The file ",file," is a PORFLOW file"
            meshDriver = PORFLOW_MESH_RDONLY_DRIVER(file,mesh)
        else :
            print "testMedMemGeneral the file ",file,"has an unknow extension"
            sys.exit(1)

        meshDriver.open()
        meshDriver.read()
    except:
        meshDriver.close()
        print "testMedMemGeneral The mesh stored in the file ",file," is perhaps a GRID."
        try:
            print "testMedMemGeneral ... of MED_CARTESIAN type ?"
            type = MED_CARTESIAN
            mesh = GRID()
            mesh.setGridType(type)
            if (extensionFile == "med"):
                meshDriver = MED_MESH_RDONLY_DRIVER(file,mesh)
                meshDriver.setMeshName(meshNameFiles[i])
            elif (extensionFile == "sauve"):
                meshDriver = GIBI_MESH_RDONLY_DRIVER(file,mesh)
            elif (extensionFile == "inp"):
                print "testMedMemGeneral The file ",file," is a PORFLOW file"
                meshDriver = PORFLOW_MESH_RDONLY_DRIVER(file,mesh)
            else :
                print "testMedMemGeneral the file ",file,"has an unknow extension"
                sys.exit(1)

            meshDriver.open()
            meshDriver.read()
        except:
            meshDriver.close()
            try:
                print "testMedMemGeneral ... of MED_POLAR type ?"
                mesh = GRID()
                type = MED_POLAR
                mesh.setGridType(type)
                if (extensionFile == "med"):
                    meshDriver = MED_MESH_RDONLY_DRIVER(file,mesh)
                    meshDriver.setMeshName(meshNameFiles[i])
                elif (extensionFile == "sauve"):
                    meshDriver = GIBI_MESH_RDONLY_DRIVER(file,mesh)
                else :
                    print "testMedMemGeneral the file ",file,"has an unknow extension"
                    sys.exit(1)

                meshDriver.open()
                meshDriver.read()
            except:
                meshDriver.close()
                print "testMedMemGeneral ... of MED_BODY_FITTED type ?"
                mesh = GRID()
                type = MED_BODY_FITTED
                mesh.setGridType(type)
                if (extensionFile == "med"):
                    meshDriver = MED_MESH_RDONLY_DRIVER(file,mesh)
                    meshDriver.setMeshName(meshNameFiles[i])
                elif (extensionFile == "sauve"):
                    meshDriver = GIBI_MESH_RDONLY_DRIVER(file,mesh)
                else :
                    print "testMedMemGeneral the file ",file,"has an unknow extension"
                    sys.exit(1)

                meshDriver.open()
                meshDriver.read()

    meshDriver.close()

    meshName = mesh.getName()
    spaceDim = mesh.getSpaceDimension()
    meshDim = mesh.getMeshDimension()
    print "testMedMemGeneral The mesh ",meshName," is a ",spaceDim,"D mesh on a ",meshDim,"D geometry"

    if (extensionFile == "med"):
        print "testMedMemGeneral and is mounted via the MED driver"
    elif (extensionFile == "sauve"):
        print "testMedMemGeneral and is mounted via the GIBI driver"
    elif (extensionFile == "inp"):
        print "testMedMemGeneral and is mounted via the PORFLOW driver"

    nbNodes = mesh.getNumberOfNodes()
    print "testMedMemGeneral The mesh ",meshName," has",nbNodes,"Nodes"
    coordSyst = mesh.getCoordinatesSystem()
    print "testMedMemGeneral The coordinates system is",coordSyst
    print "testMedMemGeneral The Coordinates :"
    coordNames = []
    coordUnits = []
    for isd in range(spaceDim):
        coordNames.append(mesh.getCoordinateName(isd))
        coordUnits.append(mesh.getCoordinateUnit(isd))

    print "testMedMemGeneral names:", coordNames
    print "testMedMemGeneral units", coordUnits
    print "testMedMemGeneral values:"
    coordinates = mesh.getCoordinates(MED_FULL_INTERLACE)
    for k in range(nbNodes):
        kp1 = k+1
        coords = []
        for isd in range(spaceDim):
            isdp1 = isd+1
            coords.append(mesh.getCoordinate(kp1,isdp1))
        print "testMedMemGeneral ",coords," ---- ", coordinates[k*spaceDim:((k+1)*spaceDim)]

    print "testMedMemGeneral "
    print "testMedMemGeneral Show the Cell Nodal Connectivity of the Cells:"
    nbTypesCell = mesh.getNumberOfTypes(MED_CELL)
    print "testMedMemGeneral "
    if (nbTypesCell>0):
        print "testMedMemGeneral The Mesh has",nbTypesCell,"Type(s) of Cell"
        types = mesh.getTypes(MED_CELL)
        for k in range(nbTypesCell):
            type = types[k]
            nbElemType = mesh.getNumberOfElements(MED_CELL,type)
            print "testMedMemGeneral For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
            connectivity = mesh.getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,type)
            nbNodesPerCell = type%100
            for j in range(nbElemType):
                print "testMedMemGeneral Element",(j+1)," ",connectivity[j*nbNodesPerCell:(j+1)*nbNodesPerCell]

    print "testMedMemGeneral "
    print "testMedMemGeneral Show the Cell Reverse Nodal Connectivity:"
    ReverseConnectivity = mesh.getReverseConnectivity(MED_NODAL)
    ReverseConnectivityIndex = mesh.getReverseConnectivityIndex(MED_NODAL)
    print "testMedMemGeneral "
    for j in range(nbNodes):
        begin = ReverseConnectivityIndex[j]-1
        end = ReverseConnectivityIndex[j+1]-1
        print "testMedMemGeneral Node",(j+1),"-->",ReverseConnectivity[begin:end]

    print "testMedMemGeneral "
    print "testMedMemGeneral Show the Cell Descending Connectivity:"
    mesh.calculateConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL)
    nbElemts = mesh.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS)
    Connectivity = mesh.getConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS)
    ConnectivityIndex = mesh.getConnectivityIndex(MED_DESCENDING,MED_CELL)
    print "testMedMemGeneral "
    for j in range(nbElemts):
        begin = ConnectivityIndex[j]-1
        end = ConnectivityIndex[j+1]-1
        print "testMedMemGeneral Element",(j+1),"-->",Connectivity[begin:end]

    print "testMedMemGeneral "

    if (spaceDim == 3):
        constituent = MED_FACE
    elif (spaceDim == 2):
        constituent = MED_EDGE

    print "testMedMemGeneral Show the Face/Edge Nodal Connectivity:"
    nbTypesConst = mesh.getNumberOfTypes(constituent)
    print "testMedMemGeneral "
    if (nbTypesConst>0):
        print "testMedMemGeneral The Mesh has",nbTypesConst,"Type(s) of Constituent"
        types = mesh.getTypes(constituent)
        for k in range(nbTypesConst):
            type = types[k]
            nbElemType = mesh.getNumberOfElements(constituent,type)
            print "testMedMemGeneral For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
            connectivity = mesh.getConnectivity(MED_FULL_INTERLACE,MED_NODAL,constituent,type)
            nbNodesPerConst = type%100
            for j in range(nbElemType):
                print "testMedMemGeneral Element",(j+1)," ",connectivity[j*nbNodesPerConst:(j+1)*nbNodesPerConst]

    print "testMedMemGeneral "
    print "testMedMemGeneral Show the Face/Edge Reverse Nodal Connectivity:"
    ReverseConnectivity = mesh.getReverseConnectivity(MED_NODAL,constituent)
    ReverseConnectivityIndex = mesh.getReverseConnectivityIndex(MED_NODAL,constituent)
    print "testMedMemGeneral "
    for j in range(nbNodes):
        begin = ReverseConnectivityIndex[j]-1
        end = ReverseConnectivityIndex[j+1]-1
        print "testMedMemGeneral Node",(j+1),"-->",ReverseConnectivity[begin:end]

    print "testMedMemGeneral "
    try:
        print "testMedMemGeneral Show the Face/Edge Descending Connectivity:"
        mesh.calculateConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,constituent)
        nbElemts = mesh.getNumberOfElements(constituent,MED_ALL_ELEMENTS)
        Connectivity = mesh.getConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,constituent,MED_ALL_ELEMENTS)
        ConnectivityIndex = mesh.getConnectivityIndex(MED_DESCENDING,constituent)
        print "testMedMemGeneral "
        for j in range(nbElemts):
            begin = ConnectivityIndex[j]-1
            end = ConnectivityIndex[j+1]-1
            print "testMedMemGeneral Element",(j+1),"-->",Connectivity[begin:end]

        print "testMedMemGeneral "
    except :
        pass

    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        nbGrp = mesh.getNumberOfGroups(entity)
        if (entity == MED_NODE) :
            if (nbFam > 0) : print "testMedMemGeneral This mesh has",nbFam,"Node Family(ies)"
            if (nbGrp > 0) : print "testMedMemGeneral This mesh has",nbGrp,"Node Group(s)"
        elif (entity == MED_CELL) :
            if (nbFam > 0) : print "testMedMemGeneral This mesh has",nbFam,"Cell Family(ies)"
            if (nbGrp > 0) : print "testMedMemGeneral This mesh has",nbGrp,"Cell Group(s)"
        elif (entity == MED_FACE) :
            if (nbFam > 0) : print "testMedMemGeneral This mesh has",nbFam,"Face Family(ies)"
            if (nbGrp > 0) : print "testMedMemGeneral This mesh has",nbGrp,"Face Group(s)"
        elif (entity == MED_EDGE) :
            if (nbFam > 0) : print "testMedMemGeneral This mesh has",nbFam,"Edge Family(ies)"
            if (nbGrp > 0) : print "testMedMemGeneral This mesh has",nbGrp,"Edge Group(s)"

    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        nbGrp = mesh.getNumberOfGroups(entity)
        if nbFam > 0:
            for j in range(nbFam):
                print "testMedMemGeneral "
                family = mesh.getFamily(entity,j+1)
                familyName = family.getName()
                familyDescription = family.getDescription()
                familyEntity = family.getEntity()
                familyBool = family.isOnAllElements()
                print "testMedMemGeneral   -Name:",familyName
                print "testMedMemGeneral   -Description:",familyDescription
                print "testMedMemGeneral   -Entity:",familyEntity
                familyIdentifier = family.getIdentifier()
                nbOfAtt = family.getNumberOfAttributes()
                print "testMedMemGeneral   -Identifier:",familyIdentifier
                print "testMedMemGeneral   -Number Of Attributes:",nbOfAtt
                attributesids = family.getAttributesIdentifiers()
                attributesvals = family.getAttributesValues()
                for k in range(nbOfAtt):
                    print "testMedMemGeneral     * Attributes:",attributesids[k],":",attributesvals[k],",",family.getAttributeDescription(k+1)
                nbOfGrp = family.getNumberOfGroups()
                print "testMedMemGeneral   -Number Of Groups:",nbOfGrp
                for k in range(nbOfGrp):
                    print "testMedMemGeneral     * Group:",family.getGroupName(k+1)
                print "testMedMemGeneral   -Entities list:"
                if (familyBool):
                    print "testMedMemGeneral   -Is on all entities"
                else:
                    nbOfTypes = family.getNumberOfTypes()
                    types = family.getTypes()
                    print "testMedMemGeneral   -Number Of Types:",nbOfTypes
                    for k in range(nbOfTypes):
                        type = types[k]
                        nbOfElmtsOfType = family.getNumberOfElements(type)
                        number = family.getNumber(type)
                        print "testMedMemGeneral     * Type",type
                        print "testMedMemGeneral     * Number",number[0:nbOfElmtsOfType]
                    print "testMedMemGeneral "
        print "testMedMemGeneral "

        if nbGrp > 0:
            for j in range(nbGrp):
                print "testMedMemGeneral "
                group = mesh.getGroup(entity,j+1)
                groupName = group.getName()
                groupDescription = group.getDescription()
                groupEntity = group.getEntity()
                groupBool = group.isOnAllElements()
                print "testMedMemGeneral   -Name:",groupName
                print "testMedMemGeneral   -Description:",groupDescription
                print "testMedMemGeneral   -Entity:",groupEntity
                nbOfFam = group.getNumberOfFamilies()
                print "testMedMemGeneral   -Number Of Families:",nbOfFam
                for k in range(nbOfFam):
                    print "testMedMemGeneral     * Family:",group.getFamily(k+1).getName()
                print "testMedMemGeneral   -Entities list:"
                if (groupBool):
                    print "testMedMemGeneral   -Is on all entities"
                else:
                    nbOfTypes = group.getNumberOfTypes()
                    types = group.getTypes()
                    print "testMedMemGeneral   -Number Of Types:",nbOfTypes
                    for k in range(nbOfTypes):
                        type = types[k]
                        nbOfElmtsOfType = group.getNumberOfElements(type)
                        number = group.getNumber(type)
                        print "testMedMemGeneral     * Type",type
                        print "testMedMemGeneral     * Number",number[0:nbOfElmtsOfType]
                    print "testMedMemGeneral "
            print "testMedMemGeneral "


    print "testMedMemGeneral Saving in file the mesh under the med and vtk format"
    print "testMedMemGeneral Med file = ",medFileName
    print "testMedMemGeneral vtk file = ",vtkFileName
    idMed = mesh.addDriver(MED_DRIVER,medFileName,mesh.getName())
    mesh.write(idMed)

    idVtk = mesh.addDriver(VTK_DRIVER,vtkFileName,mesh.getName())
    mesh.write(idVtk)
    print "testMedMemGeneral "

    print "testMedMemGeneral Building of the support on all Cells of the mesh."
    supportCell = SUPPORT(mesh)
    supportCell.update()
    print "testMedMemGeneral "
    barycenter = mesh.getBarycenter(supportCell)
    print "testMedMemGeneral Getting barycenter of all Cells of the mesh"
    for j in range(nbElemts):
        barycenterCell = barycenter.getValueI(MED_FULL_INTERLACE,j+1)
        print "testMedMemGeneral ",j,"    * ",barycenterCell[:spaceDim]
    print "testMedMemGeneral "

    print "testMedMemGeneral Saving in file the cell barycenter field under the med and vtk format"
    print "testMedMemGeneral Med file = ",medFileName
    print "testMedMemGeneral vtk file = ",vtkFileName
    idMed = barycenter.addDriver(MED_DRIVER,medFileName,barycenter.getName())
    barycenter.write(idMed)

    idVtk = barycenter.addDriver(VTK_DRIVER,vtkFileName,barycenter.getName())
    barycenter.write(idVtk)
    print "testMedMemGeneral "

    if spaceDim == 3 :
        print "testMedMemGeneral Getting volume of all Cells of the mesh:"
        volume = mesh.getVolume(supportCell)
        voltot = 0.
        for j in range(nbElemts):
            volumeCell = volume.getValueIJ(j+1,1)
            print "testMedMemGeneral     * ",volumeCell
            voltot = voltot + volumeCell
        print "testMedMemGeneral Volume of the mesh:",voltot
        print "testMedMemGeneral "

        print "testMedMemGeneral Saving in file the cell volume field under the med and vtk format"
        print "testMedMemGeneral Med file = ",medFileName
        print "testMedMemGeneral vtk file = ",vtkFileName
        idMed = volume.addDriver(MED_DRIVER,medFileName,volume.getName())
        volume.write(idMed)

        idVtk = volume.addDriver(VTK_DRIVER,vtkFileName,volume.getName())
        volume.write(idVtk)
        print "testMedMemGeneral "

        print "testMedMemGeneral Building of the support on all Faces of the mesh."
        supportFace = SUPPORT(mesh,"Support on all faces of the mesh",MED_FACE)
        nbFace = mesh.getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS)
        print "testMedMemGeneral "
        print "testMedMemGeneral Getting normal of each face of this support",nbFace
        nbTypeFace = mesh.getNumberOfTypes(MED_FACE)
        TypeFace = mesh.getTypes(MED_FACE)
        print "testMedMemGeneral nbTypeFace:",nbTypeFace,"----",TypeFace[:nbTypeFace]
        normal = mesh.getNormal(supportFace)
        area = mesh.getArea(supportFace)
        for j in range(nbFace):
            normalFace = normal.getValueI(MED_FULL_INTERLACE,j+1)
            areaFace = area.getValueI(MED_FULL_INTERLACE,j+1)
            value1 = normalFace[0]
            value2 = normalFace[1]
            value3 = normalFace[2]
            norm = (value1*value1 + value2*value2 + value3*value3)**(0.5)
            if norm == -0.0 :
                norm = 0.0
            print "testMedMemGeneral ",j,"    * ",normalFace[:spaceDim],"norm:",norm," --> area ",areaFace[0]
        print "testMedMemGeneral "

        print "testMedMemGeneral Saving in file the face normal field under the med format"
        print "testMedMemGeneral Med file = ",medFileName
        idMed = normal.addDriver(MED_DRIVER,medFileName,normal.getName())
        normal.write(idMed)
        print "testMedMemGeneral but not in vtk format because vtk does not offer the possibility to view a field on edges or faces"
        print "testMedMemGeneral "

    elif spaceDim == 2:
        print "testMedMemGeneral Getting area on all Cells of the mesh:"
        area = mesh.getArea(supportCell)
        areatot = 0.
        for j in range(nbElemts):
            areaCell = area.getValueIJ(j+1,1)
            print "testMedMemGeneral     * ",areaCell
            areatot = areatot + areaCell
        print "testMedMemGeneral Area of the mesh:",areatot
        print "testMedMemGeneral "            

        print "testMedMemGeneral Saving in file the cell area field under the med and vtk format"
        print "testMedMemGeneral Med file = ",medFileName
        print "testMedMemGeneral vtk file = ",vtkFileName
        idMed = area.addDriver(MED_DRIVER,medFileName,area.getName())
        area.write(idMed)

        idVtk = area.addDriver(VTK_DRIVER,vtkFileName,area.getName())
        area.write(idVtk)
        print "testMedMemGeneral "

        print "testMedMemGeneral Building of the support on all Edges of the mesh."
        supportEdge = SUPPORT(mesh,"Support on all edges of the mesh",MED_EDGE)
        nbEdge = mesh.getNumberOfElements(MED_EDGE,MED_ALL_ELEMENTS)
        print "testMedMemGeneral "
        print "testMedMemGeneral Getting normal of each edge of this support",nbEdge
        nbTypeEdge = mesh.getNumberOfTypes(MED_EDGE)
        TypeEdge = mesh.getTypes(MED_EDGE)
        print "testMedMemGeneral nbTypeEdge:",nbTypeEdge,"----",TypeEdge[:nbTypeEdge]
        normal = mesh.getNormal(supportEdge)
        length = mesh.getLength(supportEdge)
        for j in range(nbEdge):
            normalEdge = normal.getValueI(MED_FULL_INTERLACE,j+1)
            lengthEdge = length.getValueI(MED_FULL_INTERLACE,j+1)
            if normalEdge[0] == -0.0 :
                normalEdge[0] = 0.0
            if normalEdge[1] == -0.0 :
                normalEdge[1] = 0.0
            value1 = normalEdge[0]
            value2 = normalEdge[1]
            norm = (value1*value1 + value2*value2)**(0.5)
            print "testMedMemGeneral ",j,"    * ",normalEdge[:spaceDim],"norm:",norm," --> length ",lengthEdge[0]
        print "testMedMemGeneral "

        print "testMedMemGeneral Saving in file the face normal field under the med format"
        print "testMedMemGeneral Med file = ",medFileName
        idMed = normal.addDriver(MED_DRIVER,medFileName,normal.getName())
        normal.write(idMed)
        print "testMedMemGeneral but no in vtk format because vtk does not offer the possibility to view a field on edges or faces"

    print "testMedMemGeneral "
    print "testMedMemGeneral Building support on Elements of the boundary"
    if spaceDim == 3 :
        suppBound = mesh.getBoundaryElements(MED_FACE)
        nbElmBound = suppBound.getNumberOfElements(MED_ALL_ELEMENTS)
        print "testMedMemGeneral Getting normal field on the boundary",nbElmBound
        normalBound = mesh.getNormal(suppBound)
        for j in range(nbElmBound):
            normalBoundJ = normalBound.getValueI(MED_FULL_INTERLACE,j+1)
            if normalBoundJ[0] == -0.0 :
                normalBoundJ[0] = 0.0
            if normalBoundJ[1] == -0.0 :
                normalBoundJ[1] = 0.0
            if normalBoundJ[2] == -0.0 :
                normalBoundJ[2] = 0.0
            value1 = normalBoundJ[0]
            value2 = normalBoundJ[1]
            value3 = normalBoundJ[2]
            norm = (value1*value1 + value2*value2 + value3*value3)**(0.5)
            print "testMedMemGeneral ",j,"    * ",normalBoundJ[:spaceDim],"norm:",norm
    elif spaceDim == 2:
        suppBound = mesh.getBoundaryElements(MED_EDGE)
        nbElmBound = suppBound.getNumberOfElements(MED_ALL_ELEMENTS)
        print "testMedMemGeneral Getting normal field on the boundary",nbElmBound
        normalBound = mesh.getNormal(suppBound)
        for j in range(nbElmBound):
            normalBoundJ = normalBound.getValueI(MED_FULL_INTERLACE,j+1)
            if normalBoundJ[0] == -0.0 :
                normalBoundJ[0] = 0.0
            if normalBoundJ[1] == -0.0 :
                normalBoundJ[1] = 0.0
            value1 = normalBoundJ[0]
            value2 = normalBoundJ[1]
            norm = (value1*value1 + value2*value2)**(0.5)
            print "testMedMemGeneral ",j,"    * ",normalBoundJ[:spaceDim],"norm:",norm
    print "testMedMemGeneral "

    if (extensionFile == "med"):
        md = MED()

        mdDriver = MED_MED_RDONLY_DRIVER(file,md)

        mdDriver.open()
        mdDriver.readFileStruct()
        mdDriver.close()
        nbMeshes = md.getNumberOfMeshes()
        nbFields = md.getNumberOfFields()

        print "testMedMemGeneral The med file", file, "contains", nbMeshes, "mesh(es) and", nbFields, "field(s)"

        if (nbMeshes>0):
            print "testMedMemGeneral Mesh(es) Name(s) is(are) "

            for imsh in range(nbMeshes):
                mesh_name = md.getMeshName(imsh)
                print "testMedMemGeneral    - ",mesh_name
        print "testMedMemGeneral "
        if (nbFields>0):
            print "testMedMemGeneral Field(s) Name(s) is(are) "

            for ifld in range(nbFields):
                field_name = md.getFieldName(ifld)
                print "testMedMemGeneral    - ",field_name

        print "testMedMemGeneral "

        mesh_name = md.getMeshName(0)
        mesh = md.getMesh(mesh_name)
        mesh.read()
        spaceDim = mesh.getSpaceDimension()
        meshDim = mesh.getMeshDimension()
        nbNodes = mesh.getNumberOfNodes()
        print "testMedMemGeneral The first mesh",mesh_name,"is a",spaceDim,"D mesh on a",meshDim,"D geometry and has",nbNodes,"Nodes"

        if (nbFields>0):
            print "testMedMemGeneral Updating supports in the Med Object"
            md.updateSupport()
            print "testMedMemGeneral Field(s) Analysis "
            for ifld in range(nbFields):
                field_name = md.getFieldName(ifld)
                nbOfIt = md.getFieldNumberOfIteration(field_name)
                print "testMedMemGeneral The",print_ord(ifld),"field is",field_name,"with",nbOfIt,"iteration(s)"
                for j in range(nbOfIt):
                    dtitfield = md.getFieldIteration(field_name,j)
                    dt = dtitfield.getdt()
                    it = dtitfield.getit()
                    field = md.getField(field_name,dt,it)
                    type = field.getValueType()
                    print "testMedMemGeneral      * Iteration:",dt,"Order number:",it,"Type:",type
                    if type == MED_INT32:
                        fieldint = createFieldIntFromField(field)
                        fieldint.read()
                        name = fieldint.getName()
                        desc = fieldint.getDescription()
                        nbOfComp = fieldint.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fieldint.getIterationNumber()
                        orderNb = fieldint.getOrderNumber()
                        time = fieldint.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        print "testMedMemGeneral      Norme  2  : ", fieldint.norm2()
                        print "testMedMemGeneral      Norme Max : ", fieldint.normMax()

                        if fielddouble.getSupport().getEntity()!=MED_NODE:
                            fieldint_vol=fieldint.getSupport().getMesh().getVolume(fieldint.getSupport())
                            print "testMedMemGeneral      Norme L1  : ", fieldint.normL1()
                            print "testMedMemGeneral      Norme L2  : ", fieldint.normL2()
                            print "testMedMemGeneral      Norme L2(vol) : ", fieldint.normL2(fieldint_vol)

                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fieldint.getComponentName(kp1)
                            compDesc = fieldint.getComponentDescription(kp1)
                            compUnit = fieldint.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit
                            if fielddouble.getSupport().getEntity()!=MED_NODE:
                                print "testMedMemGeneral           Norme L1  : ", fieldint.normL1(kp1)
                                print "testMedMemGeneral           Norme L2  : ", fieldint.normL2(kp1)
                                print "testMedMemGeneral           Norme L2(vol) : ", fieldint.normL2(kp1,fieldint_vol)

                        support = fieldint.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fieldint.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        fieldint2 = FIELDINT(fieldint)
                        print "testMedMemGeneral "
                        fieldintadd = fieldint + fieldint2
                        print "testMedMemGeneral Test of the addition of two integer fields with creation a new one"
                        print "testMedMemGeneral "
                        name = fieldintadd.getName()
                        desc = fieldintadd.getDescription()
                        nbOfComp = fieldintadd.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fieldintadd.getIterationNumber()
                        orderNb = fieldintadd.getOrderNumber()
                        time = fieldintadd.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fieldintadd.getComponentName(kp1)
                            compDesc = fieldintadd.getComponentDescription(kp1)
                            compUnit = fieldintadd.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit

                        support = fieldintadd.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fieldintadd.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        print "testMedMemGeneral "
                        fieldintsub = fieldint - fieldint2
                        print "testMedMemGeneral Test of the substraction of two integer fields with creation a new one"
                        print "testMedMemGeneral "
                        name = fieldintsub.getName()
                        desc = fieldintsub.getDescription()
                        nbOfComp = fieldintsub.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fieldintsub.getIterationNumber()
                        orderNb = fieldintsub.getOrderNumber()
                        time = fieldintsub.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fieldintsub.getComponentName(kp1)
                            compDesc = fieldintsub.getComponentDescription(kp1)
                            compUnit = fieldintsub.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit

                        support = fieldintsub.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fieldintsub.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        print "testMedMemGeneral "
                        fieldintmul = fieldint * fieldint2
                        print "testMedMemGeneral Test of the multiplication of two integer fields with creation a new one"
                        print "testMedMemGeneral "
                        name = fieldintmul.getName()
                        desc = fieldintmul.getDescription()
                        nbOfComp = fieldintmul.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fieldintmul.getIterationNumber()
                        orderNb = fieldintmul.getOrderNumber()
                        time = fieldintmul.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fieldintmul.getComponentName(kp1)
                            compDesc = fieldintmul.getComponentDescription(kp1)
                            compUnit = fieldintmul.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit

                        support = fieldintmul.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fieldintmul.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        print "testMedMemGeneral "
                        print "testMedMemGeneral Try of the division of two integer fields with creation a new one"
                        try :
                            fieldintdiv = fieldint / fieldint2
                            print "testMedMemGeneral Test of the division of two integer fields with creation a new one"
                            print "testMedMemGeneral "
                            name = fieldintdiv.getName()
                            desc = fieldintdiv.getDescription()
                            nbOfComp = fieldintdiv.getNumberOfComponents()
                            print "testMedMemGeneral      Field",name," : ",desc
                            print "testMedMemGeneral      Number Of Components:",nbOfComp
                            iterationNb = fieldintdiv.getIterationNumber()
                            orderNb = fieldintdiv.getOrderNumber()
                            time = fieldintdiv.getTime()
                            print "testMedMemGeneral      Iteration Number",iterationNb
                            print "testMedMemGeneral      Order Number",orderNb
                            print "testMedMemGeneral      Time",time
                            for k in range(nbOfComp):
                                kp1 = k+1
                                compName = fieldintdiv.getComponentName(kp1)
                                compDesc = fieldintdiv.getComponentDescription(kp1)
                                compUnit = fieldintdiv.getMEDComponentUnit(kp1)
                                print "testMedMemGeneral       * Component:",kp1
                                print "testMedMemGeneral           Name:",compName
                                print "testMedMemGeneral           Description:",compDesc
                                print "testMedMemGeneral           Unit:",compUnit

                            support = fieldintdiv.getSupport()
                            nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                            print "testMedMemGeneral      Values:",nbOf
                            for k in range(nbOf):
                                valueI = fieldintdiv.getValueI(MED_FULL_INTERLACE,k+1)
                                print "testMedMemGeneral      *",valueI[:nbOfComp]
                            print "testMedMemGeneral "
                        except :
                            print "testMedMemGeneral  fieldintdiv = fieldint / fieldint2 catch/except error"
                        print "testMedMemGeneral TESTS OPERATIONS SUR FIELDINT : "
                        fieldintadd = fieldint+fieldint2
                        fieldintsub = fieldint-fieldint2
                        fieldintmul = fieldint*fieldint2
                        try :
                            fieldintdiv = fieldint/fieldint2
                        except :
                            fieldintdiv = None
                            print "testMedMemGeneral  fieldintdiv = fieldint/fieldint2 catch/except error"
                        fieldintasso = fieldint+fieldint*fieldint
                        fieldintSP=createFieldIntScalarProduct(fieldint, fieldint2)

                        print "testMedMemGeneral  f1     : ",fieldint.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral  f2     : ",fieldint2.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral --------------------------------------------------------------------------------------------------------------"
                        print "testMedMemGeneral   +     : ",fieldintadd.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral   -     : ",fieldintsub.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral   *     : ",fieldintmul.getValue(MED_FULL_INTERLACE)
                        if fieldintdiv == None :
                            print "testMedMemGeneral   /     : None"
                        else :
                            print "testMedMemGeneral   /     : ",fieldintdiv.getValue(MED_FULL_INTERLACE)
                        fieldint+=fieldint2;
                        print "testMedMemGeneral   +=    : ",fieldint.getValue(MED_FULL_INTERLACE)
                        fieldint-=fieldint2;
                        print "testMedMemGeneral   -=    : ",fieldint.getValue(MED_FULL_INTERLACE)
                        fieldint*=fieldint2;
                        print "testMedMemGeneral   *=    : ",fieldint.getValue(MED_FULL_INTERLACE)
                        try :
                            fieldint/=fieldint2;
                            print "testMedMemGeneral   /=    : ",fieldint.getValue(MED_FULL_INTERLACE)
                        except :
                            fieldint = None
                            print "testMedMemGeneral   /=    : Catch/Except : None"
                        print "testMedMemGeneral f1+f2*f2: ",fieldintasso.getValue(MED_FULL_INTERLACE)
                        if fieldint != None :
                            fieldint.applyLin(4,1);
                            print "testMedMemGeneral  4f1+1  : ",fieldint.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral  f1.f2  : ",fieldintSP.getValue(MED_FULL_INTERLACE)
                        fieldint2.applyPyFunc(add_one)
                        print "testMedMemGeneral  CB:f2+1: ",fieldint2.getValue(MED_FULL_INTERLACE)
                    elif type == MED_REEL64:
                        fielddouble = createFieldDoubleFromField(field)
                        fielddouble.read()
                        name = fielddouble.getName()
                        desc = fielddouble.getDescription()
                        nbOfComp = fielddouble.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fielddouble.getIterationNumber()
                        orderNb = fielddouble.getOrderNumber()
                        time = fielddouble.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        print "testMedMemGeneral      Norme  2  : ", fielddouble.norm2()
                        print "testMedMemGeneral      Norme Max : ", fielddouble.normMax()
                        print "testMedMemGeneral try sobolev",fielddouble.getSupport().getEntity()
                        if fielddouble.getSupport().getEntity()!=MED_NODE:
                            if (spaceDim == 3):
                                fielddouble_vol=fielddouble.getSupport().getMesh().getVolume(fielddouble.getSupport())
                            elif (spaceDim == 2):
                                fielddouble_vol=fielddouble.getSupport().getMesh().getArea(fielddouble.getSupport())
                            print "testMedMemGeneral Norme L1  : ", fielddouble.normL1()
                            print "testMedMemGeneral Norme L2  : ", fielddouble.normL2()
                            print "testMedMemGeneral Norme L2(vol) : ", fielddouble.normL2(fielddouble_vol)

                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fielddouble.getComponentName(kp1)
                            compDesc = fielddouble.getComponentDescription(kp1)
                            compUnit = fielddouble.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit
                            if fielddouble.getSupport().getEntity()!=MED_NODE:
                                print "testMedMemGeneral           Norme L1  : ", fielddouble.normL1(kp1)
                                print "testMedMemGeneral           Norme L2  : ", fielddouble.normL2(kp1)
                                print "testMedMemGeneral           Norme L2(vol) : ", fielddouble.normL2(kp1, fielddouble_vol)

                        support = fielddouble.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fielddouble.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        fielddouble2 = FIELDDOUBLE(fielddouble)
                        print "testMedMemGeneral "
                        fielddoubleadd = fielddouble + fielddouble2
                        print "testMedMemGeneral Test of the addition of two double fields with creation a new one"
                        print "testMedMemGeneral "
                        name = fielddoubleadd.getName()
                        desc = fielddoubleadd.getDescription()
                        nbOfComp = fielddoubleadd.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fielddoubleadd.getIterationNumber()
                        orderNb = fielddoubleadd.getOrderNumber()
                        time = fielddoubleadd.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fielddoubleadd.getComponentName(kp1)
                            compDesc = fielddoubleadd.getComponentDescription(kp1)
                            compUnit = fielddoubleadd.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit

                        support = fielddoubleadd.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fielddoubleadd.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        print "testMedMemGeneral "
                        fielddoublesub = fielddouble - fielddouble2
                        print "testMedMemGeneral Test of the substraction of two double fields with creation a new one"
                        print "testMedMemGeneral "
                        name = fielddoublesub.getName()
                        desc = fielddoublesub.getDescription()
                        nbOfComp = fielddoublesub.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fielddoublesub.getIterationNumber()
                        orderNb = fielddoublesub.getOrderNumber()
                        time = fielddoublesub.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fielddoublesub.getComponentName(kp1)
                            compDesc = fielddoublesub.getComponentDescription(kp1)
                            compUnit = fielddoublesub.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit

                        support = fielddoublesub.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fielddoublesub.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        print "testMedMemGeneral "
                        fielddoublemul = fielddouble * fielddouble2
                        print "testMedMemGeneral Test of the multiplication of two double fields with creation a new one"
                        print "testMedMemGeneral "
                        name = fielddoublemul.getName()
                        desc = fielddoublemul.getDescription()
                        nbOfComp = fielddoublemul.getNumberOfComponents()
                        print "testMedMemGeneral      Field",name," : ",desc
                        print "testMedMemGeneral      Number Of Components:",nbOfComp
                        iterationNb = fielddoublemul.getIterationNumber()
                        orderNb = fielddoublemul.getOrderNumber()
                        time = fielddoublemul.getTime()
                        print "testMedMemGeneral      Iteration Number",iterationNb
                        print "testMedMemGeneral      Order Number",orderNb
                        print "testMedMemGeneral      Time",time
                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fielddoublemul.getComponentName(kp1)
                            compDesc = fielddoublemul.getComponentDescription(kp1)
                            compUnit = fielddoublemul.getMEDComponentUnit(kp1)
                            print "testMedMemGeneral       * Component:",kp1
                            print "testMedMemGeneral           Name:",compName
                            print "testMedMemGeneral           Description:",compDesc
                            print "testMedMemGeneral           Unit:",compUnit

                        support = fielddoublemul.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "testMedMemGeneral      Values:",nbOf
                        for k in range(nbOf):
                            valueI = fielddoublemul.getValueI(MED_FULL_INTERLACE,k+1)
                            print "testMedMemGeneral      *",valueI[:nbOfComp]
                        print "testMedMemGeneral "
                        print "testMedMemGeneral Try of the division of two double fields with creation a new one"
                        try :
                            fielddoublediv = fielddouble / fielddouble2
                            print "testMedMemGeneral Test of the division of two double fields with creation a new one"
                            print "testMedMemGeneral "
                            name = fielddoublediv.getName()
                            desc = fielddoublediv.getDescription()
                            nbOfComp = fielddoublediv.getNumberOfComponents()
                            print "testMedMemGeneral      Field",name," : ",desc
                            print "testMedMemGeneral      Number Of Components:",nbOfComp
                            iterationNb = fielddoublediv.getIterationNumber()
                            orderNb = fielddoublediv.getOrderNumber()
                            time = fielddoublediv.getTime()
                            print "testMedMemGeneral      Iteration Number",iterationNb
                            print "testMedMemGeneral      Order Number",orderNb
                            print "testMedMemGeneral      Time",time
                            for k in range(nbOfComp):
                                kp1 = k+1
                                compName = fielddoublediv.getComponentName(kp1)
                                compDesc = fielddoublediv.getComponentDescription(kp1)
                                compUnit = fielddoublediv.getMEDComponentUnit(kp1)
                                print "testMedMemGeneral       * Component:",kp1
                                print "testMedMemGeneral           Name:",compName
                                print "testMedMemGeneral           Description:",compDesc
                                print "testMedMemGeneral           Unit:",compUnit
                            support = fielddoublediv.getSupport()
                            nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                            print "testMedMemGeneral      Values:",nbOf
                            for k in range(nbOf):
                                valueI = fielddoublediv.getValueI(MED_FULL_INTERLACE,k+1)
                                print "testMedMemGeneral      *",valueI[:nbOfComp]
                            print "testMedMemGeneral "
                        except :
                            print "testMedMemGeneral  fielddoublediv = fielddouble / fielddouble2 catch/except error"
                        print "testMedMemGeneral TESTS OPERATIONS SUR FIELDDOUBLE : "
                        fielddoublesub = fielddouble-fielddouble2
                        fielddoublemul = fielddouble*fielddouble2
                        try :
                            fielddoublediv = fielddouble/fielddouble2
                        except :
                            print "testMedMemGeneral  fielddoublediv = fielddouble/fielddouble2 catch/except error"
                            fielddoublediv = None
                        fielddoubleasso = fielddouble+fielddouble2*fielddouble2
                        fielddoubleSP=createFieldDoubleScalarProduct(fielddouble, fielddouble2)
                        print "testMedMemGeneral  f1     : ",fielddouble.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral  f2     : ",fielddouble2.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral --------------------------------------------------------------------------------------------------------------"
                        print "testMedMemGeneral   +     : ",fielddoubleadd.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral   -     : ",fielddoublesub.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral   *     : ",fielddoublemul.getValue(MED_FULL_INTERLACE)
                        if fielddoublediv == None :
                            print "testMedMemGeneral   /     : None"
                        else :
                            print "testMedMemGeneral   /     : ",fielddoublediv.getValue(MED_FULL_INTERLACE)
                        fielddouble+=fielddouble2;
                        print "testMedMemGeneral   +=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
                        fielddouble-=fielddouble2;
                        print "testMedMemGeneral   -=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
                        fielddouble*=fielddouble2;
                        print "testMedMemGeneral   *=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
                        try :
                            fielddouble/=fielddouble2;
                            print "testMedMemGeneral   /=    : ",fielddouble.getValue(MED_FULL_INTERLACE)
                        except :
                            fielddouble = None
                            print "testMedMemGeneral    /=    : catch/except error"
                        print "testMedMemGeneral f1+f2*f2: ",fielddoubleasso.getValue(MED_FULL_INTERLACE)
                        if fielddouble != None :
                            fielddouble.applyLin(4,1);
                            print "testMedMemGeneral  4f1+1  : ",fielddouble.getValue(MED_FULL_INTERLACE)
                        print "testMedMemGeneral  f1.f2  : ",fielddoubleSP.getValue(MED_FULL_INTERLACE)
                        fielddouble2.applyPyFunc(add_one)
                        print "testMedMemGeneral  CB:f2+1: ",fielddouble2.getValue(MED_FULL_INTERLACE)
                    else:
                        print "testMedMemGeneral   !!!! Bad type of Field !!!!"

print "testMedMemGeneral END of the Pyhton script ..... Ctrl D to exit"
