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

from libMEDMEM_Swig import *

import os,string

filesPath=os.environ["MED_ROOT_DIR"]
filesPath=os.path.join(filesPath, "share", "salome", "resources", "med")

testExecfiles = []
argListTest = []
rootFileTest = []
meshTest = []
fieldTest = []

# executables list to test functionalities

testExecfiles.append("testUArray")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUCellModel")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUCoordinate")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUGeoNameMeshEntities")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUMedException")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUModulusArray")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUPointerOf")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUSkyLineArray")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("testUUnit")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_affect_medarray")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_copie_coordinate")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_copie_medarray")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_MEDMEM_Array")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_MEDMEM_CellModel")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_MEDMEM_ModulusArray")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_MEDMEM_SkyLineArray")
argListTest.append("")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_MEDMEM_Meshing")
argListTest.append("toto")
rootFileTest.append("")
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("Darcy3_3D_H_10x10x10")
argListTest.append(os.path.join(filesPath,"Darcy3_3D_H_10x10x10.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("dx200_dy1_avec_2couches")
argListTest.append(os.path.join(filesPath,"dx200_dy1_avec_2couches.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("elle_2D_QT_10x10")
argListTest.append(os.path.join(filesPath,"elle_2D_QT_10x10.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("elle_2D_QT_2x2")
argListTest.append(os.path.join(filesPath,"elle_2D_QT_2x2.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("elle_2D_QT_40x40")
argListTest.append(os.path.join(filesPath,"elle_2D_QT_40x40.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("elle_2D_QT_4x4")
argListTest.append(os.path.join(filesPath,"elle_2D_QT_4x4.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("elle_3D_HPr_10x10x10")
argListTest.append(os.path.join(filesPath,"elle_3D_HPr_10x10x10.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("elle_3D_HPr_2x2x2")
argListTest.append(os.path.join(filesPath,"elle_3D_HPr_2x2x2.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("elle_3D_HPr_4x4x4")
argListTest.append(os.path.join(filesPath,"elle_3D_HPr_4x4x4.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("inclusion_2d_raf")
argListTest.append(os.path.join(filesPath,"inclusion_2d_raf.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("inclusion_2d")
argListTest.append(os.path.join(filesPath,"inclusion_2d.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_ktest1-3-hexa")
argListTest.append(os.path.join(filesPath,"mail_ktest1-3-hexa.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_ktest1-3-tetra")
argListTest.append(os.path.join(filesPath,"mail_ktest1-3-tetra.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_ktest3-1")
argListTest.append(os.path.join(filesPath,"mail_ktest3-1.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_ktest3-2")
argListTest.append(os.path.join(filesPath,"mail_ktest3-2.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("maillage_andra2_100elts")
argListTest.append(os.path.join(filesPath,"maillage_andra2_100elts.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("maillage_cas2_2d")
argListTest.append(os.path.join(filesPath,"maillage_cas2_2d.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("maillage_cas4_234elts")
argListTest.append(os.path.join(filesPath,"maillage_cas4_234elts.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("maillage_CHEMVAL_100elts")
argListTest.append(os.path.join(filesPath,"maillage_CHEMVAL_100elts.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("maillage_CHEMVAL_40elts")
argListTest.append(os.path.join(filesPath,"maillage_CHEMVAL_40elts.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("maillage_chemvalIV_cas1_100elts")
argListTest.append(os.path.join(filesPath,"maillage_chemvalIV_cas1_100elts.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("maillage_chemvalIV_cas1_40elts")
argListTest.append(os.path.join(filesPath,"maillage_chemvalIV_cas1_40elts.sauve"))
meshTest.append("")
fieldTest.append("")

# no resource file named maill_mistra_elim.sauve
# testExecfiles.append("test_gibi_driver")
# rootFileTest.append("maill_mistra_elim")
# argListTest.append(os.path.join(filesPath,"maill_mistra_elim.sauve"))
# meshTest.append("")
# fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_test1-1-qua")
argListTest.append(os.path.join(filesPath,"mail_test1-1-qua.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_test1-1-tri")
argListTest.append(os.path.join(filesPath,"mail_test1-1-tri.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_test1-2-qua")
argListTest.append(os.path.join(filesPath,"mail_test1-2-qua.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail_test1-2-tri")
argListTest.append(os.path.join(filesPath,"mail_test1-2-tri.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail-test1-4-1")
argListTest.append(os.path.join(filesPath,"mail-test1-4-1.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_gibi_driver")
rootFileTest.append("mail-test1-4-2")
argListTest.append(os.path.join(filesPath,"mail-test1-4-2.sauve"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_porflow_driver")
rootFileTest.append("boitenew")
argListTest.append(os.path.join(filesPath,"boitenew.inp"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_porflow_driver")
rootFileTest.append("Case1")
argListTest.append(os.path.join(filesPath,"Case1.inp"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_porflow_driver")
rootFileTest.append("cube")
argListTest.append(os.path.join(filesPath,"cube.inp"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_porflow_driver")
rootFileTest.append("test3")
argListTest.append(os.path.join(filesPath,"test3.inp"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_porflow_driver")
rootFileTest.append("titi")
argListTest.append(os.path.join(filesPath,"titi.inp"))
meshTest.append("")
fieldTest.append("")

testExecfiles.append("test_copie_connectivity")
rootFileTest.append("mesh")
meshTest.append("Mesh 1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"mesh.med") + " " + "'Mesh 1'")

testExecfiles.append("test_copie_connectivity")
rootFileTest.append("maillage_UniSegFam")
meshTest.append("maillage_CHEMVAL_100elts")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"maillage_UniSegFam.med") + " " + "maillage_CHEMVAL_100elts")
                   
testExecfiles.append("test_copie_connectivity")
rootFileTest.append("carre_en_quad4")
meshTest.append("carre_en_quad4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"carre_en_quad4.med") + " " + "carre_en_quad4")

testExecfiles.append("test_copie_connectivity")
rootFileTest.append("cube_hexa8")
meshTest.append("CUBE_EN_HEXA8")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8.med") + " " + "CUBE_EN_HEXA8")

testExecfiles.append("test_copie_connectivity")
rootFileTest.append("test19")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"test19.med") + " " + "maa1")

testExecfiles.append("test_copie_connectivity")
rootFileTest.append("carre_en_quad4_seg2")
meshTest.append("carre_en_quad4_seg2")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"carre_en_quad4_seg2.med") + " " + "carre_en_quad4_seg2")

testExecfiles.append("test_copie_connectivity")
rootFileTest.append("cube_hexa8_quad4")
meshTest.append("CUBE_EN_HEXA8_QUAD4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8_quad4.med") + " " + "CUBE_EN_HEXA8_QUAD4")

testExecfiles.append("test_copie_connectivity")
rootFileTest.append("pointe")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"pointe.med") + " " + "maa1")

testExecfiles.append("test_copie_family")
rootFileTest.append("mesh")
meshTest.append("Mesh 1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"mesh.med") + " " + "'Mesh 1'")

testExecfiles.append("test_copie_family")
rootFileTest.append("pointe")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"pointe.med") + " " + "maa1")

# no nodal families in carre_en_quad4.med
# testExecfiles.append("test_copie_family")
# rootFileTest.append("carre_en_quad4")
# meshTest.append("carre_en_quad4")
# fieldTest.append("")
# argListTest.append(os.path.join(filesPath,"carre_en_quad4.med") + " " + "carre_en_quad4")

testExecfiles.append("test_copie_family")
rootFileTest.append("cube_hexa8")
meshTest.append("CUBE_EN_HEXA8")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8.med") + " " + "CUBE_EN_HEXA8")

testExecfiles.append("test_copie_family")
rootFileTest.append("test19")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"test19.med") + " " + "maa1")

# no nodal families in carre_en_quad4_seg2.med
# testExecfiles.append("test_copie_family")
# rootFileTest.append("carre_en_quad4_seg2")
# meshTest.append("carre_en_quad4_seg2")
# fieldTest.append("")
# argListTest.append(os.path.join(filesPath,"carre_en_quad4_seg2.med") + " " + "carre_en_quad4_seg2")

testExecfiles.append("test_copie_family")
rootFileTest.append("cube_hexa8_quad4")
meshTest.append("CUBE_EN_HEXA8_QUAD4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8_quad4.med") + " " + "CUBE_EN_HEXA8_QUAD4")

testExecfiles.append("test_copie_family")
rootFileTest.append("pointe")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"pointe.med") + " " + "maa1")

# no nodal grous in mesh.med
# testExecfiles.append("test_copie_group")
# rootFileTest.append("mesh")
# meshTest.append("Mesh 1")
# fieldTest.append("")
# argListTest.append(os.path.join(filesPath,"mesh.med") + " " + "'Mesh 1'")

# no nodal grous in maillage_UniSegFam.med
# testExecfiles.append("test_copie_group")
# rootFileTest.append("maillage_UniSegFam")
# meshTest.append("maillage_CHEMVAL_100elts")
# fieldTest.append("")
# argListTest.append(os.path.join(filesPath,"maillage_UniSegFam.med") + " " + "maillage_CHEMVAL_100elts")

# no nodal grous in maillage_UniSegFam.med
# testExecfiles.append("test_copie_group")
# rootFileTest.append("carre_en_quad4")
# meshTest.append("carre_en_quad4")
# fieldTest.append("")
# argListTest.append(os.path.join(filesPath,"carre_en_quad4.med") + " " + "carre_en_quad4")

testExecfiles.append("test_copie_group")
rootFileTest.append("cube_hexa8")
meshTest.append("CUBE_EN_HEXA8")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8.med") + " " + "CUBE_EN_HEXA8")

testExecfiles.append("test_copie_group")
rootFileTest.append("test19")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"test19.med") + " " + "maa1")

# no nodal grous in carre_en_quad4_seg2.med
# testExecfiles.append("test_copie_group")
# rootFileTest.append("carre_en_quad4_seg2")
# meshTest.append("carre_en_quad4_seg2")
# fieldTest.append("")
# argListTest.append(os.path.join(filesPath,"carre_en_quad4_seg2.med") + " " + "carre_en_quad4_seg2")

testExecfiles.append("test_copie_group")
rootFileTest.append("cube_hexa8_quad4")
meshTest.append("CUBE_EN_HEXA8_QUAD4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8_quad4.med") + " " + "CUBE_EN_HEXA8_QUAD4")

testExecfiles.append("test_copie_group")
rootFileTest.append("pointe")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"pointe.med") + " " + "maa1")

testExecfiles.append("test_copie_mesh")
rootFileTest.append("mesh")
meshTest.append("Mesh 1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"mesh.med") + " " + "'Mesh 1'")

testExecfiles.append("test_copie_mesh")
rootFileTest.append("maillage_UniSegFam")
meshTest.append("maillage_CHEMVAL_100elts")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"maillage_UniSegFam.med") + " " + "maillage_CHEMVAL_100elts")
                   
testExecfiles.append("test_copie_mesh")
rootFileTest.append("carre_en_quad4")
meshTest.append("carre_en_quad4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"carre_en_quad4.med") + " " + "carre_en_quad4")

testExecfiles.append("test_copie_mesh")
rootFileTest.append("cube_hexa8")
meshTest.append("CUBE_EN_HEXA8")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8.med") + " " + "CUBE_EN_HEXA8")

testExecfiles.append("test_copie_mesh")
rootFileTest.append("test19")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"test19.med") + " " + "maa1")

testExecfiles.append("test_copie_mesh")
rootFileTest.append("carre_en_quad4_seg2")
meshTest.append("carre_en_quad4_seg2")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"carre_en_quad4_seg2.med") + " " + "carre_en_quad4_seg2")

testExecfiles.append("test_copie_mesh")
rootFileTest.append("cube_hexa8_quad4")
meshTest.append("CUBE_EN_HEXA8_QUAD4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8_quad4.med") + " " + "CUBE_EN_HEXA8_QUAD4")

testExecfiles.append("test_copie_mesh")
rootFileTest.append("pointe")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"pointe.med") + " " + "maa1")

testExecfiles.append("test_copie_support")
rootFileTest.append("mesh")
meshTest.append("Mesh 1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"mesh.med") + " " + "'Mesh 1'")

testExecfiles.append("test_copie_support")
rootFileTest.append("maillage_UniSegFam")
meshTest.append("maillage_CHEMVAL_100elts")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"maillage_UniSegFam.med") + " " + "maillage_CHEMVAL_100elts")
                   
testExecfiles.append("test_copie_support")
rootFileTest.append("carre_en_quad4")
meshTest.append("carre_en_quad4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"carre_en_quad4.med") + " " + "carre_en_quad4")

testExecfiles.append("test_copie_support")
rootFileTest.append("cube_hexa8")
meshTest.append("CUBE_EN_HEXA8")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8.med") + " " + "CUBE_EN_HEXA8")

testExecfiles.append("test_copie_support")
rootFileTest.append("test19")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"test19.med") + " " + "maa1")

testExecfiles.append("test_copie_support")
rootFileTest.append("carre_en_quad4_seg2")
meshTest.append("carre_en_quad4_seg2")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"carre_en_quad4_seg2.med") + " " + "carre_en_quad4_seg2")

testExecfiles.append("test_copie_support")
rootFileTest.append("cube_hexa8_quad4")
meshTest.append("CUBE_EN_HEXA8_QUAD4")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"cube_hexa8_quad4.med") + " " + "CUBE_EN_HEXA8_QUAD4")

testExecfiles.append("test_copie_support")
rootFileTest.append("pointe")
meshTest.append("maa1")
fieldTest.append("")
argListTest.append(os.path.join(filesPath,"pointe.med") + " " + "maa1")

###############################################################################

nbOfTests = len(testExecfiles)

print "Running all test programs with several arguments ... total = ",nbOfTests

testOutfiles = []
testErrfiles = []

for i in range(nbOfTests):
    if (rootFileTest[i] == ""):
        testOutfiles.append(testExecfiles[i] + ".out")
    else:
        if (meshTest[i] == ""):
            testOutfiles.append(testExecfiles[i]+ "_" + rootFileTest[i] + ".out")
        else:
            if (fieldTest[i] == ""):
                meshName = string.replace(meshTest[i]," ","_")
                testOutfiles.append(testExecfiles[i]+ "_" + rootFileTest[i] + "_" + meshName + ".out")
            else:
                meshName = string.replace(meshTest[i]," ","_")
                fieldName = string.replace(fieldTest[i]," ","_")
                testOutfiles.append(testExecfiles[i]+ "_" + rootFileTest[i] + "_" + meshName + "_" + fieldName + ".out")
    testErrfiles.append(testExecfiles[i] + ".err")

for i in range(nbOfTests):
    aCommand = testExecfiles[i] + " " + argListTest[i] + " > " + testOutfiles[i] + " 2> " + testErrfiles[i]
    status = os.system(aCommand)
    if (status != 0):
        print "problem with the command ", aCommand

print "END of the Pyhton script ..... Ctrl D to exit"
