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
# This Python script is testing all functionalities of the Med Memory through its
# Python API; for this purpose a battery of med file as well as gibi files are
# scanned using the parser written in the Python script med_test1.py with some
# addings to test other functionality written in othe Python scripts in the bin
# directory of the installation of MED
###################################################################################
#
from libMEDMEM_Swig import *
from random import *

import sys,os,string

filesPath = os.environ["MED_ROOT_DIR"]
filesPath = os.path.join(filesPath, "share", "salome", "resources", "med")

tmpDir = os.getenv("TEMP")
if tmpDir == None:
  tmpDir = "/tmp"

tmpMask = os.path.join(tmpDir, "*_test.*")
os.system("rm -rf " + tmpMask)

fileNames = []
meshNames = []

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
# from CODE_ASTER
#

##fileNames.append("maill.0.med")
##meshNames.append("MAILTRQU")

##fileNames.append("zzzz121b.med")
##meshNames.append("MUN")

#
# from the SMESH Salome Module
#

fileNames.append("mesh.med")
meshNames.append("Mesh 1")

#
# from other source including LGLS ones
#

fileNames.append("maillage_UniSegFam.med")
meshNames.append("maillage_CHEMVAL_100elts")

fileNames.append("carre_en_quad4.med")
meshNames.append("carre_en_quad4")

fileNames.append("cube_hexa8.med")
meshNames.append("CUBE_EN_HEXA8")

##fileNames.append("test19.med")
##meshNames.append("CartGrid")

##fileNames.append("test19.med")
##meshNames.append("bodyfitted")

##fileNames.append("test19.med")
##meshNames.append("maa1")

fileNames.append("carre_en_quad4_seg2.med")
meshNames.append("carre_en_quad4_seg2")

fileNames.append("cube_hexa8_quad4.med")
meshNames.append("CUBE_EN_HEXA8_QUAD4")

fileNames.append("pointe.med")
meshNames.append("maa1")

fileNames.append("Mistrat.med")
meshNames.append("Mistrat_Hexa")

##fileNames.append("TimeStamps.med")
##meshNames.append("dom")

fileNames.append("Darcy3_3D_H_10x10x10_2.med")
meshNames.append("Darcy3_3D_H_10x10x10")

fileNames.append("elle_3D_HPr_10x10x10_2.med")
meshNames.append("elle_3D_HPr_10x10x10")

fileNames.append("elle_3D_HPr_2x2x2_2.med")
meshNames.append("elle_3D_HPr_2x2x2")

fileNames.append("elle_3D_HPr_4x4x4_2.med")
meshNames.append("elle_3D_HPr_4x4x4")



fileNames.append("ChampsDarcy.med")
meshNames.append("2D_I129")

fileNames.append("darcy_1.1_res.med")
meshNames.append("mail_test1-1-tri")

fileNames.append("darcy_1.3_resCASTEM.med")
meshNames.append("mail_ktest1-3-tetra")

fileNames.append("darcy_1.3_resPORFLOW.med")
meshNames.append("mail_ktest1-3-hexa")

fileNames.append("darcy_1.3_resTRACES.med")
meshNames.append("mail_ktest1-3-tetra")

fileNames.append("darcy2_Castem_EFMH.med")
meshNames.append("mail_test1-2-tri")

fileNames.append("darcy2_Castem_qua_EFMH.med")
meshNames.append("mail_test1-2-qua")

fileNames.append("darcy2_Castem_qua_VF.med")
meshNames.append("mail_test1-2-qua")

# there is a field with too long name (38 > MED_TAILLE_NOM==32):
# "analytical_field - CONCENTRATION of A1"
# so that invalid writing is sometimes fatal
# fileNames.append("Deff_fdt_5.8_castem_efmh_diff_conc_dom.med")
# meshNames.append("maillage_deffec_fdt")

# there is a field with too long name (38 > MED_TAILLE_NOM==32):
# "analytical_field - CONCENTRATION of A1"
# so that invalid writing is sometimes fatal
# fileNames.append("Deff_fdt_5.8_castem_vf_diff_conc_dom.med")
# meshNames.append("maillage_deffec_fdt")

fileNames.append("extendedtransport53_triangles.med")
meshNames.append("TestA3_2094_0.1_rsurf_tri")

fileNames.append("H_CastCast_EFMH_I129_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_CastCast_VF_I129_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_CastCast_VF_Se79_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_CastPorf_I129_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_CastPorf_Se79_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_PorfCast_EFMH_I129_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_PorfCast_EFMH_Se79_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_PorfPorf_I129_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_Traces_I129_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("H_Traces_Se79_COUPLEX1.med")
meshNames.append("COUPLEX1")

fileNames.append("maillage_5_5_5.med")
meshNames.append("maillage_5_5_5")

fileNames.append("maillage_chemvalIV_cas1_40elts.med")
meshNames.append("maillage_chemvalIV_cas1_40elts")




#
# Castem or Gibi file list
#

fileNames.append("Darcy3_3D_H_10x10x10.sauve")
meshNames.append("")

fileNames.append("dx200_dy1_avec_2couches.sauve")
meshNames.append("")

fileNames.append("elle_2D_QT_10x10.sauve")
meshNames.append("")

fileNames.append("elle_2D_QT_2x2.sauve")
meshNames.append("")

fileNames.append("elle_2D_QT_40x40.sauve")
meshNames.append("")

fileNames.append("elle_2D_QT_4x4.sauve")
meshNames.append("")

fileNames.append("elle_3D_HPr_10x10x10.sauve")
meshNames.append("")

fileNames.append("elle_3D_HPr_2x2x2.sauve")
meshNames.append("")

fileNames.append("elle_3D_HPr_4x4x4.sauve")
meshNames.append("")

fileNames.append("inclusion_2d_raf.sauve")
meshNames.append("")

fileNames.append("inclusion_2d.sauve")
meshNames.append("")

fileNames.append("mail_ktest1-3-hexa.sauve")
meshNames.append("")

fileNames.append("mail_ktest1-3-tetra.sauve")
meshNames.append("")

fileNames.append("mail_ktest3-1.sauve")
meshNames.append("")

fileNames.append("mail_ktest3-2.sauve")
meshNames.append("")

fileNames.append("maillage_andra2_100elts.sauve")
meshNames.append("")

fileNames.append("maillage_cas2_2d.sauve")
meshNames.append("")

fileNames.append("maillage_cas4_234elts.sauve")
meshNames.append("")

fileNames.append("maillage_CHEMVAL_100elts.sauve")
meshNames.append("")

fileNames.append("maillage_CHEMVAL_40elts.sauve")
meshNames.append("")

fileNames.append("maillage_chemvalIV_cas1_100elts.sauve")
meshNames.append("")

fileNames.append("maillage_chemvalIV_cas1_40elts.sauve")
meshNames.append("")

fileNames.append("mail_test1-1-qua.sauve")
meshNames.append("")

fileNames.append("mail_test1-1-tri.sauve")
meshNames.append("")

fileNames.append("mail_test1-2-qua.sauve")
meshNames.append("")

fileNames.append("mail_test1-2-tri.sauve")
meshNames.append("")

fileNames.append("mail-test1-4-1.sauve")
meshNames.append("")

fileNames.append("mail-test1-4-2.sauve")
meshNames.append("")

#
# Porflow file list
#

fileNames.append("boitenew.inp")
meshNames.append("")

fileNames.append("Case1.inp")
meshNames.append("")

fileNames.append("cube.inp")
meshNames.append("")

fileNames.append("test3.inp")
meshNames.append("")

fileNames.append("titi.inp")
meshNames.append("")

###################################################################################

nbOfFiles = len(fileNames)
filesFull = []

for i in range(nbOfFiles):
    filesFull.append(os.path.join(filesPath, fileNames[i]))

###################################################################################
#
# Loop on all files
#
###################################################################################

print " This test is running on ",nbOfFiles," files"
print ""

for i in range(nbOfFiles):
    fileName = fileNames[i]
    fileFull = filesFull[i]
    decompFile = string.split(fileName,".")
    lenDecompFileM1 = len(decompFile)-1

    if (lenDecompFileM1 == 0) :
        print "The file ",fileName," should have at least a . in its name "
        sys.exit(1)

    extensionFile = decompFile[lenDecompFileM1]

    rootFile = decompFile[0]
    for k in range(1,lenDecompFileM1):
        rootFile = rootFile + "." + decompFile[k]

    rootFileFull = os.path.join(tmpDir, rootFile)

    medV22FileName = rootFileFull + "V22_test.med"
    vtkFileName    = rootFileFull + "_test.vtk"

    try:
        mesh = MESH()
        if (extensionFile == "med"):
            print "The file ",fileName," is a MED file and the name of the mesh is ", meshNames[i]
            meshDriver = MED_MESH_RDONLY_DRIVER(fileFull,mesh)
            meshDriver.setMeshName(meshNames[i])
        elif (extensionFile == "sauve"):
            print "The file ",fileName," is a GIBI file"
            meshDriver = GIBI_MESH_RDONLY_DRIVER(fileFull,mesh)
        elif (extensionFile == "inp"):
            print "The file ",fileName," is a PORFLOW file"
            meshDriver = PORFLOW_MESH_RDONLY_DRIVER(fileFull,mesh)
        else :
            print "the file ",fileName,"has an unknow extension"
            sys.exit(1)

        mesh.read(meshDriver)
    except:
        print "The mesh stored in the file ",fileName," is perhaps a GRID."
        try:
            print "... of MED_CARTESIAN type ?"
            type = MED_CARTESIAN
            mesh = GRID()
            mesh.setGridType(type)
            if (extensionFile == "med"):
                meshDriver = MED_MESH_RDONLY_DRIVER(fileFull,mesh)
                meshDriver.setMeshName(meshNames[i])
            elif (extensionFile == "sauve"):
                meshDriver = GIBI_MESH_RDONLY_DRIVER(fileFull,mesh)
            elif (extensionFile == "inp"):
                print "The file ",fileName," is a PORFLOW file"
                meshDriver = PORFLOW_MESH_RDONLY_DRIVER(fileFull,mesh)
            else :
                print "the file ",fileName,"has an unknow extension"
                sys.exit(1)

            meshDriver.open()
            meshDriver.read()
        except:
            meshDriver.close()
            try:
                print "... of MED_POLAR type ?"
                mesh = GRID()
                type = MED_POLAR
                mesh.setGridType(type)
                if (extensionFile == "med"):
                    meshDriver = MED_MESH_RDONLY_DRIVER(fileFull,mesh)
                    meshDriver.setMeshName(meshNames[i])
                elif (extensionFile == "sauve"):
                    meshDriver = GIBI_MESH_RDONLY_DRIVER(fileFull,mesh)
                else :
                    print "the file ",fileName,"has an unknow extension"
                    sys.exit(1)

                meshDriver.open()
                meshDriver.read()
            except:
                meshDriver.close()
                print "... of MED_BODY_FITTED type ?"
                mesh = GRID()
                type = MED_BODY_FITTED
                mesh.setGridType(type)
                if (extensionFile == "med"):
                    meshDriver = MED_MESH_RDONLY_DRIVER(fileFull,mesh)
                    meshDriver.setMeshName(meshNames[i])
                elif (extensionFile == "sauve"):
                    meshDriver = GIBI_MESH_RDONLY_DRIVER(fileFull,mesh)
                else :
                    print "the file ",fileName,"has an unknow extension"
                    sys.exit(1)

                meshDriver.open()
                meshDriver.read()

    meshDriver.close()

    meshName = mesh.getName()
    spaceDim = mesh.getSpaceDimension()
    meshDim = mesh.getMeshDimension()
    print "The mesh ",meshName," is a ",spaceDim,"D mesh on a ",meshDim,"D geometry"

    if (extensionFile == "med"):
        print "and is mounted via the MED driver"
    elif (extensionFile == "sauve"):
        print "and is mounted via the GIBI driver"
    elif (extensionFile == "inp"):
        print "and is mounted via the PORFLOW driver"

    nbNodes = mesh.getNumberOfNodes()
    print "The mesh ",meshName," has",nbNodes,"Nodes"
    coordSyst = mesh.getCoordinatesSystem()
    print "The coordinates system is",coordSyst
    print "The Coordinates :"
    coordNames = []
    coordUnits = []
    for isd in range(spaceDim):
        coordNames.append(mesh.getCoordinateName(isd))
        coordUnits.append(mesh.getCoordinateUnit(isd))

    print "names:", coordNames
    print "units", coordUnits
    print "values:"
    coordinates = mesh.getCoordinates(MED_FULL_INTERLACE)
    for k in range(nbNodes):
        kp1 = k+1
        coords = []
        for isd in range(spaceDim):
            isdp1 = isd+1
            coords.append(mesh.getCoordinate(kp1,isdp1))

        print coords," ---- ", coordinates[k*spaceDim:((k+1)*spaceDim)]

    print ""
    print "Show the Cell Nodal Connectivity of the Cells:"
    nbTypesCell = mesh.getNumberOfTypes(MED_CELL)
    print ""
    if (nbTypesCell>0):
        print "The Mesh has",nbTypesCell,"Type(s) of Cell"
        types = mesh.getTypes(MED_CELL)
        for k in range(nbTypesCell):
            type = types[k]
            nbElemType = mesh.getNumberOfElements(MED_CELL,type)
            print "For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
            connectivity = mesh.getConnectivity(MED_NODAL,MED_CELL,type)
            nbNodesPerCell = type%100
            for j in range(nbElemType):
                print "Element",(j+1)," ",connectivity[j*nbNodesPerCell:(j+1)*nbNodesPerCell]

    print ""
    print "Show the Cell Reverse Nodal Connectivity:"
    ReverseConnectivity = mesh.getReverseConnectivity(MED_NODAL)
    ReverseConnectivityIndex = mesh.getReverseConnectivityIndex(MED_NODAL)
    print ""
    for j in range(nbNodes):
        begin = ReverseConnectivityIndex[j]-1
        end = ReverseConnectivityIndex[j+1]-1
        print "Node",(j+1),"-->",ReverseConnectivity[begin:end]

    print ""
    print "Show the Cell Descending Connectivity:"
    mesh.calculateConnectivity(MED_DESCENDING,MED_CELL)
    nbElemts = mesh.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS)
    Connectivity = mesh.getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS)
    ConnectivityIndex = mesh.getConnectivityIndex(MED_DESCENDING,MED_CELL)
    print ""
    for j in range(nbElemts):
        begin = ConnectivityIndex[j]-1
        end = ConnectivityIndex[j+1]-1
        print "Element",(j+1),"-->",Connectivity[begin:end]

    print ""

    if (spaceDim == 3):
        constituent = MED_FACE
    elif (spaceDim == 2):
        constituent = MED_EDGE

    print "Show the Face/Edge Nodal Connectivity:"
    nbTypesConst = mesh.getNumberOfTypes(constituent)
    print ""
    if (nbTypesConst>0):
        print "The Mesh has",nbTypesConst,"Type(s) of Constituent"
        types = mesh.getTypes(constituent)
        for k in range(nbTypesConst):
            type = types[k]
            nbElemType = mesh.getNumberOfElements(constituent,type)
            print "For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
            connectivity = mesh.getConnectivity(MED_NODAL,constituent,type)
            nbNodesPerConst = type%100
            for j in range(nbElemType):
                print "Element",(j+1)," ",connectivity[j*nbNodesPerConst:(j+1)*nbNodesPerConst]

    if (meshDim == 3):
        print ""
        print "Show the Face/Edge Reverse Nodal Connectivity:"
        ReverseConnectivity = mesh.getReverseConnectivity(MED_NODAL,constituent)
        ReverseConnectivityIndex = mesh.getReverseConnectivityIndex(MED_NODAL,constituent)
        print ""
        for j in range(nbNodes):
            begin = ReverseConnectivityIndex[j]-1
            end = ReverseConnectivityIndex[j+1]-1
            print "Node",(j+1),"-->",ReverseConnectivity[begin:end]

        print ""
        try:
            print "Show the Face/Edge Descending Connectivity:"
            mesh.calculateConnectivity(MED_DESCENDING,constituent)
            nbElemts = mesh.getNumberOfElements(constituent,MED_ALL_ELEMENTS)
            Connectivity = mesh.getConnectivity(MED_DESCENDING,constituent,MED_ALL_ELEMENTS)
            ConnectivityIndex = mesh.getConnectivityIndex(MED_DESCENDING,constituent)
            print ""
            for j in range(nbElemts):
                begin = ConnectivityIndex[j]-1
                end = ConnectivityIndex[j+1]-1
                print "Element",(j+1),"-->",Connectivity[begin:end]

            print ""
        except :
            pass

    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        nbGrp = mesh.getNumberOfGroups(entity)
        if (entity == MED_NODE) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Node Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Node Group(s)"
        elif (entity == MED_CELL) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Cell Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Cell Group(s)"
        elif (entity == MED_FACE) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Face Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Face Group(s)"
        elif (entity == MED_EDGE) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Edge Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Edge Group(s)"

    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        nbGrp = mesh.getNumberOfGroups(entity)
        if nbFam > 0:
            for j in range(nbFam):
                print ""
                family = mesh.getFamily(entity,j+1)
                familyName = family.getName()
                familyDescription = family.getDescription()
                familyEntity = family.getEntity()
                familyBool = family.isOnAllElements()
                print "  -Name:",familyName
                print "  -Description:",familyDescription
                print "  -Entity:",familyEntity
                familyIdentifier = family.getIdentifier()
                nbOfAtt = family.getNumberOfAttributes()
                print "  -Identifier:",familyIdentifier
                print "  -Number Of Attributes:",nbOfAtt
                attributesids = family.getAttributesIdentifiers()
                attributesvals = family.getAttributesValues()
                for k in range(nbOfAtt):
                    print "    * Attributes:",attributesids[k],":",attributesvals[k],",",family.getAttributeDescription(k+1)
                nbOfGrp = family.getNumberOfGroups()
                print "  -Number Of Groups:",nbOfGrp
                for k in range(nbOfGrp):
                    print "    * Group:",family.getGroupName(k+1)
                print "  -Entities list:"
                if (familyBool):
                    print "  -Is on all entities"
                else:
                    nbOfTypes = family.getNumberOfTypes()
                    types = family.getTypes()
                    print "  -Number Of Types:",nbOfTypes
                    for k in range(nbOfTypes):
                        type = types[k]
                        nbOfElmtsOfType = family.getNumberOfElements(type)
                        number = family.getNumber(type)
                        print "    * Type",type
                        print "    * Number",number[0:nbOfElmtsOfType]
                    print ""
        print ""

        if nbGrp > 0:
            for j in range(nbGrp):
                print ""
                group = mesh.getGroup(entity,j+1)
                groupName = group.getName()
                groupDescription = group.getDescription()
                groupEntity = group.getEntity()
                groupBool = group.isOnAllElements()
                print "  -Name:",groupName
                print "  -Description:",groupDescription
                print "  -Entity:",groupEntity
                nbOfFam = group.getNumberOfFamilies()
                print "  -Number Of Families:",nbOfFam
                for k in range(nbOfFam):
                    print "    * Family:",group.getFamily(k+1).getName()
                print "  -Entities list:"
                if (groupBool):
                    print "  -Is on all entities"
                else:
                    nbOfTypes = group.getNumberOfTypes()
                    types = group.getTypes()
                    print "  -Number Of Types:",nbOfTypes
                    for k in range(nbOfTypes):
                        type = types[k]
                        nbOfElmtsOfType = group.getNumberOfElements(type)
                        number = group.getNumber(type)
                        print "    * Type",type
                        print "    * Number",number[0:nbOfElmtsOfType]
                    print ""
            print ""


    print "Saving in file the mesh under the med and vtk format"
    print "Med V22 file = ",medV22FileName
    print "vtk file = ",vtkFileName

    idMedV22 = mesh.addDriver(MED_DRIVER,medV22FileName,mesh.getName(),RDWR)
    mesh.write(idMedV22)

    idVtk = mesh.addDriver(VTK_DRIVER,vtkFileName,mesh.getName())
    mesh.write(idVtk)
    print ""

    print "Building of the support on all Cells of the mesh."
    supportCell = mesh.getSupportOnAll( MED_CELL )
    print ""
    barycenter = mesh.getBarycenter(supportCell)
    print "Getting barycenter of all Cells of the mesh"
    for j in range(nbElemts):
        barycenterCell = barycenter.getRow(j+1)
        print "    * ",barycenterCell[:spaceDim]
    print ""

    print "Saving in file the cell barycenter field under the med and vtk format"
    print "Med V22 file = ",medV22FileName
    print "vtk file = ",vtkFileName

    idMedV22 = barycenter.addDriver(MED_DRIVER,medV22FileName,barycenter.getName())
    barycenter.write(idMedV22)

    idVtk = barycenter.addDriver(VTK_DRIVER,vtkFileName,barycenter.getName())
    barycenter.write(idVtk)
    print ""

    if (spaceDim == 3) and (meshDim == spaceDim) :
        print "Getting volume of all Cells of the mesh:"
        volume = mesh.getVolume(supportCell)
        voltot = 0.
        for j in range(nbElemts):
            volumeCell = volume.getValueIJ(j+1,1)
            print "    * ",volumeCell
            voltot = voltot + volumeCell
        print "Volume of the mesh:",voltot
        print ""

        print "Saving in file the cell volume field under the med and vtk format"
        print "Med V22 file = ",medV22FileName
        print "vtk file = ",vtkFileName

        idMedV22 = volume.addDriver(MED_DRIVER,medV22FileName,volume.getName())
        volume.write(idMedV22)

        idVtk = volume.addDriver(VTK_DRIVER,vtkFileName,volume.getName())
        volume.write(idVtk)
        print ""

        print "Building of the support on all Faces of the mesh."
        supportFace = mesh.getSupportOnAll(MED_FACE)
        nbFace = mesh.getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS)
        print ""
        print "Getting normal of each face of this support",nbFace
        nbTypeFace = mesh.getNumberOfTypes(MED_FACE)
        TypeFace = mesh.getTypes(MED_FACE)
        print "nbTypeFace:",nbTypeFace,"----",TypeFace[:nbTypeFace]
        normal = mesh.getNormal(supportFace)
        area = mesh.getArea(supportFace)
        for j in range(nbFace):
            normalFace = normal.getRow(j+1)
            areaFace = area.getRow(j+1)
            value1 = normalFace[0]
            value2 = normalFace[1]
            value3 = normalFace[2]
            norm = (value1*value1 + value2*value2 + value3*value3)**(0.5)
            print "    * ",normalFace[:spaceDim],"norm:",norm," --> area ",areaFace[0]
        print ""

        print "Saving in file the face normal field under the med format"
        print "Med V22 file = ",medV22FileName

        idMedV22 = normal.addDriver(MED_DRIVER,medV22FileName,normal.getName())
        normal.write(idMedV22)

        print "but not in vtk format because vtk does not offer the possibility to view a field on edges or faces"
        print ""

    elif (spaceDim == 2) and (meshDim == spaceDim):
        print "Getting area on all Cells of the mesh:"
        area = mesh.getArea(supportCell)
        areatot = 0.
        for j in range(nbElemts):
            areaCell = area.getValueIJ(j+1,1)
            print "    * ",areaCell
            areatot = areatot + areaCell
        print "Area of the mesh:",areatot
        print ""

        print "Saving in file the cell area field under the med and vtk format"
        print "Med V22 file = ",medV22FileName
        print "vtk file = ",vtkFileName


        idMedV22 = area.addDriver(MED_DRIVER,medV22FileName,area.getName())
        area.write(idMedV22)

        idVtk = area.addDriver(VTK_DRIVER,vtkFileName,area.getName())
        area.write(idVtk)
        print ""

        print "Building of the support on all Edges of the mesh."
        supportEdge = mesh.getSupportOnAll(MED_EDGE)
        nbEdge = mesh.getNumberOfElements(MED_EDGE,MED_ALL_ELEMENTS)
        print ""
        print "Getting normal of each edge of this support",nbEdge
        nbTypeEdge = mesh.getNumberOfTypes(MED_EDGE)
        TypeEdge = mesh.getTypes(MED_EDGE)
        print "nbTypeEdge:",nbTypeEdge,"----",TypeEdge[:nbTypeEdge]
        normal = mesh.getNormal(supportEdge)
        length = mesh.getLength(supportEdge)
        for j in range(nbEdge):
            normalEdge = normal.getRow(j+1)
            lengthEdge = length.getRow(j+1)
            value1 = normalEdge[0]
            value2 = normalEdge[1]
            norm = (value1*value1 + value2*value2)**(0.5)
            print "    * ",normalEdge[:spaceDim],"norm:",norm," --> length ",lengthEdge[0]
        print ""

        print "Saving in file the face normal field under the med format"
        print "Med V22 file = ",medV22FileName

        idMedV22 = normal.addDriver(MED_DRIVER,medV22FileName,normal.getName())
        normal.write(idMedV22)

        print "but no in vtk format because vtk does not offer the possibility to view a field on edges or faces"

    print ""
    print "Building support on Elements of the boundary"
    if (spaceDim == 3) and (meshDim == spaceDim) :
        suppBound = mesh.getBoundaryElements(MED_FACE)
        nbElmBound = suppBound.getNumberOfElements(MED_ALL_ELEMENTS)
        print "Getting normal field on the boundary",nbElmBound
        normalBound = mesh.getNormal(suppBound)
        numberSuppBound = suppBound.getNumber(MED_ALL_ELEMENTS)
        for j in range(nbElmBound):
            valInd = numberSuppBound[j]
            normalBoundJ = normalBound.getRow(valInd)
            value1 = normalBoundJ[0]
            value2 = normalBoundJ[1]
            value3 = normalBoundJ[2]
            norm = (value1*value1 + value2*value2 + value3*value3)**(0.5)
            print "    * ",normalBoundJ[:spaceDim],"norm:",norm
    elif (spaceDim == 2) and (meshDim == spaceDim):
        suppBound = mesh.getBoundaryElements(MED_EDGE)
        nbElmBound = suppBound.getNumberOfElements(MED_ALL_ELEMENTS)
        print "Getting normal field on the boundary",nbElmBound
        normalBound = mesh.getNormal(suppBound)
        numberSuppBound = suppBound.getNumber(MED_ALL_ELEMENTS)
        for j in range(nbElmBound):
            valInd = numberSuppBound[j]
            normalBoundJ = normalBound.getRow(valInd)
            value1 = normalBoundJ[0]
            value2 = normalBoundJ[1]
            norm = (value1*value1 + value2*value2)**(0.5)
            print "    * ",normalBoundJ[:spaceDim],"norm:",norm
    print ""

    if (extensionFile == "med"):
        md = MEDFILEBROWSER(fileFull)

        nbMeshes = md.getNumberOfMeshes()
        nbFields = md.getNumberOfFields()

        print "The med file", fileName, "contains", nbMeshes, "mesh(es) and", nbFields, "field(s)"

        if (nbMeshes>0):
            print "Mesh(es) Name(s) is(are) "

            for imsh in range(nbMeshes):
                mesh_name = md.getMeshName(imsh)
                print "   - ",mesh_name
        print ""
        if (nbFields>0):
            print "Field(s) Name(s) is(are) "

            for ifld in range(nbFields):
                field_name = md.getFieldName(ifld)
                print "   - ",field_name

        print ""

#        mesh_name = md.getMeshName(0)
        mesh_name = meshNames[i]
        if md.isStructuredMesh( mesh_name ):
            mesh = GRID(MED_DRIVER,md.getFileName(),mesh_name)
        else:
            mesh = MESH(MED_DRIVER,md.getFileName(),mesh_name)
        spaceDim = mesh.getSpaceDimension()
        meshDim = mesh.getMeshDimension()
        nbNodes = mesh.getNumberOfNodes()
        print "The first mesh",mesh_name,"is a",spaceDim,"D mesh on a",meshDim,"D geometry and has",nbNodes,"Nodes"

        if (nbFields>0):
            print "Field(s) Analysis "
            for ifld in range(nbFields):
                field_name = md.getFieldName(ifld)
                dtits = md.getFieldIteration(field_name)
                print "The",print_ord(ifld),"field is",field_name,"with",len(dtits),"iteration(s)"
                for dtitfield in dtits:
                    dt = dtitfield.getdt()
                    it = dtitfield.getit()
                    type = md.getFieldType(field_name)
                    print "     * Iteration:",dt,"Order number:",it,"Type:",type
                    if type == MED_INT32:
                        fieldint = FIELDINT(MED_DRIVER,md.getFileName(),field_name,dt,it,mesh)
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
                        print "     Norme  2  : ", fieldint.norm2()
                        print "     Norme Max : ", fieldint.normMax()

                        if fieldint.getSupport().getEntity()!=MED_NODE:
                            fieldint_vol=fieldint.getSupport().getMesh().getVolume(fieldint.getSupport())
                            print "     Norme L1  : ", fieldint.normL1()
                            print "     Norme L2  : ", fieldint.normL2()
                            print "     Norme L2(vol) : ", fieldint.normL2(fieldint_vol)

                        for k in range(nbOfComp):
                            kp1 = k+1
                            compName = fieldint.getComponentName(kp1)
                            compDesc = fieldint.getComponentDescription(kp1)
                            compUnit = fieldint.getMEDComponentUnit(kp1)
                            print "      * Component:",kp1
                            print "          Name:",compName
                            print "          Description:",compDesc
                            print "          Unit:",compUnit
                            if fieldint.getSupport().getEntity()!=MED_NODE:
                                print "          Norme L1  : ", fieldint.normL1(kp1)
                                print "          Norme L2  : ", fieldint.normL2(kp1)
                                print "          Norme L2(vol) : ", fieldint.normL2(kp1,fieldint_vol)

                        support = fieldint.getSupport()
                        nbOf = support.getNumberOfElements(MED_ALL_ELEMENTS)
                        print "     Values:",nbOf
                        for k in range(nbOf):
                            valueI = fieldint.getRow(k+1)
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
                            valueI = fieldintadd.getRow(k+1)
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
                            valueI = fieldintsub.getRow(k+1)
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
                            valueI = fieldintmul.getRow(k+1)
                            print "     *",valueI[:nbOfComp]
                        print ""
                        try:
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
                                valueI = fieldintdiv.getRow(k+1)
                                print "     *",valueI[:nbOfComp]
                            print ""
                            print "TESTS OPERATIONS SUR FIELDINT : "
                        except:
                            print "testMedMemGeneral  fieldintdiv = fieldint / fieldint2 catch/except error"
                        fieldintadd = fieldint+fieldint2
                        fieldintsub = fieldint-fieldint2
                        fieldintmul = fieldint*fieldint2
                        try:
                            fieldintdiv = fieldint/fieldint2
                        except:
                            fieldintdiv = None
                            print "testMedMemGeneral  fieldintdiv = fieldint/fieldint2 catch/except error"
                        fieldintasso = fieldint+fieldint*fieldint
                        fieldintSP=createFieldIntScalarProduct(fieldint, fieldint2)

                        print " f1     : ",fieldint.getValue()
                        print " f2     : ",fieldint2.getValue()
                        print "--------------------------------------------------------------------------------------------------------------"
                        print "  +     : ",fieldintadd.getValue()
                        print "  -     : ",fieldintsub.getValue()
                        print "  *     : ",fieldintmul.getValue()
                        if fieldintdiv == None :
                            print "testMedMemGeneral   /     : None"
                        else:
                            print "  /     : ",fieldintdiv.getValue()
                        fieldint+=fieldint2;
                        print "  +=    : ",fieldint.getValue()
                        fieldint-=fieldint2;
                        print "  -=    : ",fieldint.getValue()
                        fieldint*=fieldint2;
                        print "  *=    : ",fieldint.getValue()
                        try:
                            fieldint/=fieldint2;
                            print "  /=    : ",fieldint.getValue()
                        except :
                            fieldint = None
                            print "  /=    : Catch/Except : None"
                        print "f1+f2*f2: ",fieldintasso.getValue()
                        if fieldint != None :
                            fieldint.applyLin(4,1);
                            print " 4f1+1  : ",fieldint.getValue()
                        print " f1.f2  : ",fieldintSP.getValue()
                        fieldint2.applyPyFunc(add_one)
                        print " CB:f2+1: ",fieldint2.getValue()
                    elif type == MED_REEL64:
                        fielddouble = FIELDDOUBLE(MED_DRIVER,md.getFileName(),field_name,dt,it,mesh)
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
                        print "     Norme  2  : ", fielddouble.norm2()
                        print "     Norme Max : ", fielddouble.normMax()
                        fielddouble.getSupport().update()
                        fieldEntity = fielddouble.getSupport().getEntity()
                        print "try sobolev",fieldEntity
                        if fieldEntity !=MED_NODE:
                            if (fieldEntity == MED_CELL):
                                if (spaceDim == 3):
                                    fielddouble_vol=fielddouble.getSupport().getMesh().getVolume(fielddouble.getSupport())
                                elif (spaceDim == 2):
                                    fielddouble_vol=fielddouble.getSupport().getMesh().getArea(fielddouble.getSupport())
                            elif (fieldEntity == MED_FACE):
                                fielddouble_vol=fielddouble.getSupport().getMesh().getArea(fielddouble.getSupport())
                            elif (fieldEntity == MED_EDGE):
                                fielddouble_vol=fielddouble.getSupport().getMesh().getLength(fielddouble.getSupport())
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
                            valueI = fielddouble.getRow(k+1)
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
                            valueI = fielddoubleadd.getRow(k+1)
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
                            valueI = fielddoublesub.getRow(k+1)
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
                            valueI = fielddoublemul.getRow(k+1)
                            print "     *",valueI[:nbOfComp]
                        print ""
                        try:
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
                                valueI = fielddoublediv.getRow(k+1)
                                print "     *",valueI[:nbOfComp]
                        except:
                            print "testMedMemGeneral  fielddoublediv = fielddouble / fielddouble2 catch/except error"

                        print ""
                        print "TESTS OPERATIONS SUR FIELDDOUBLE : "
                        fielddoublesub = fielddouble-fielddouble2
                        fielddoublemul = fielddouble*fielddouble2
                        try:
                            fielddoublediv = fielddouble/fielddouble2
                        except:
                            print "testMedMemGeneral    /=    : catch/except error"
                            fielddoublediv = None
                        fielddoubleasso = fielddouble+fielddouble2*fielddouble2
                        fielddoubleSP=createFieldDoubleScalarProduct(fielddouble, fielddouble2)
                        print " f1     : ",fielddouble.getValue()
                        print " f2     : ",fielddouble2.getValue()
                        print "--------------------------------------------------------------------------------------------------------------"
                        print "  +     : ",fielddoubleadd.getValue()
                        print "  -     : ",fielddoublesub.getValue()
                        print "  *     : ",fielddoublemul.getValue()
                        if fielddoublediv != None:
                            print "  /     : ",fielddoublediv.getValue()
                            pass
                        fielddouble+=fielddouble2;
                        print "  +=    : ",fielddouble.getValue()
                        fielddouble-=fielddouble2;
                        print "  -=    : ",fielddouble.getValue()
                        fielddouble*=fielddouble2;
                        print "  *=    : ",fielddouble.getValue()
                        try:
                            fielddouble/=fielddouble2;
                            print "  /=    : ",fielddouble.getValue()
                        except:
                            print "testMedMemGeneral   /=    : "
                        print "f1+f2*f2: ",fielddoubleasso.getValue()
                        fielddouble.applyLin(4,1);
                        print " 4f1+1  : ",fielddouble.getValue()
                        print " f1.f2  : ",fielddoubleSP.getValue()
                        fielddouble2.applyPyFunc(add_one)
                        print " CB:f2+1: ",fielddouble2.getValue()
                    else:
                        print "  !!!! Bad type of Field !!!!"

print "END of the Pyhton script ..... Ctrl D to exit"
