#!/usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

############################################################################
# This script tests conversion of MEDMEM to EnSight by performing following
# operations on available med files:
# - read med file into MEDMEM and breifly dump it's content;
# - write EnSight file in different formats: Gold and EnSight6, ASCII and binary;
# - checks generated EnSight files using ens_checker utility (if available);
# - read generated EnSight file into MEDMEM and breifly dump it's content;
# - store MEDMEM to med file.
############################################################################
#
from medmem import *
from dumpMEDMEM import *
from re import search
import sys

ASCII = False
Binary = True
formats = [
    (ENSIGHT_GOLD,ASCII)
    ,(ENSIGHT_GOLD,Binary)
    ,(ENSIGHT_6,ASCII)
    ,(ENSIGHT_6,Binary)
    ]

tmpDir  = os.getenv("TMP")
if not tmpDir:
    tmpDir = os.getenv("TMPDIR","/tmp")

inDir = os.path.join(os.getenv("MED_ROOT_DIR"), "share","salome","resources","med")
if not os.access(inDir, os.F_OK):
    raise RuntimeError, "Path to med files not found, MED_ROOT_DIR incorrect?"

outDir = os.path.join( tmpDir,"Ensight_out")
if not os.access(outDir, os.F_OK):
    os.mkdir( outDir )
    pass

# find out if ens_checker is present
has_ens_checker = False
tmpFile = os.path.join( outDir, "has_ens_checker" )
os.system("ens_checker _.case > %s 2>&1" % tmpFile)
tmpFile = open( tmpFile ).read()
if search("EnSight Data Format Checker", tmpFile):
    has_ens_checker = True
else:
    print "\nWarning: ens_checker utility NOT available"
    pass

dumpMesh = False
dumpMesh = True

dumpField= False
dumpField= True

def check_ens(casefile, logfile):
    cmd = "(cd %s; ens_checker %s > %s 2>&1)" % (outDir, casefile, logfile)
    err = os.system( cmd )
    if os.access(logfile, os.F_OK):
        log = open(logfile).read()
        if search("bummer", log):
            print log
            raise RuntimeError, "cd %s; ens_checker %s" % (outDir, casefile)
        if search("Warning:", log):
            print log
        pass
    pass

def compatibilityPb():
    "Print traceback and return true if exception is due to EnSight-MEDMEM incompatibility"
    isCompatibilityPb = ( sys.exc_value.__str__().find("compatibility problem") > 0 )
    if isCompatibilityPb:
        print '$$$$$$$$$$$$$$$$$$$$ COMPATIBILITY PROBLEM $$$$$$$$$$$$$$$$$$$$$$$$'
        print sys.exc_value, "\n"
    else:
        import traceback
        traceback.print_exc()
    return isCompatibilityPb

from dircache import listdir

# all files sorted by size increasing
medFiles = [
     "ChampsDarcy.med"
#     ,"Old_maillage_chemvalIV_cas1_40elts.med"
#     ,"maillage_UniSegFam.med"
#     ,"champ1.med"
#     ,"carre_en_quad4.med"
#     ,"poly3D.med"
#     ,"Old_Deff_fdt_5.8_castem_vf_diff_conc_dom.med"
#     ,"Old_Deff_fdt_5.8_castem_efmh_diff_conc_dom.med"
#     ,"test18.med"
#     ,"cas_defaut_domaine_fluide.med"
#     ,"carre_en_quad4_seg2.med"
#     ,"polyedres.med"
#     ,"Fields_group3D.med"
#     ,"maillage_5_5_5.med"
#     ,"cube_hexa8.med"
#     ,"new_pointe.med"
#     ,"mesh.med"
#     ,"cube_hexa8_quad4.med"
#     ,"jurassique.med"
#     ,"test_2D.med"
#     ,"pointe_4fields.med"
#     ,"pointe.med"
#     ,"test19.med"
#     ,"extendedtransport53_triangles.med"
#     ,"recoll_bord.med"
#     ,"polygones.med"
#     ,"zzzz121b_without_tr6.med"
#     ,"trio_2D.med"
#     ,"essaiminimail.med"
#     ,"maill.0.med"
#     ,"elle_3D_HPr_2x2x2_2.med"
#     ,"maill.00_nofield.med"
#     ,"elle_3D_HPr_4x4x4_2.med"
#     ,"maill.00_nomesh.med"
#     ,"fra1.med"
#     ,"fra.med"
#     ,"maill.00_without_seg2.med"
#     ,"freebord.med"
#     ,"maill.00.med"
#     ,"zzzz121b.med"
#     ,"ResOK_0000.med"
#     ,"Darcy3_3D_H_10x10x10_2.med"
#     ,"maill.0_volumes.med"
#     ,"Old_darcy_1.3_resTRACES.med"
#     ,"Old_darcy_1.3_resCASTEM.med"
#     ,"darcy_1.3_resTRACES.med"
#     ,"darcy_1.3_resCASTEM.med"
#     ,"geomMesh22.med"
#     ,"geomMesh21.med"
#     ,"Old_darcy_1.3_resPORFLOW.med"
#     ,"darcy_1.3_resPORFLOW.med"
#     ,"test_hydro_darcy1a_out.med"
#     ,"maillage_2D.med"
#     ,"elle_3D_HPr_10x10x10_2.med"
#     ,"Old_darcy_1.1_res.med"
#     ,"darcy_1.1_res.med"
#     ,"maill.1.med"
#     ,"clo.med"
#     ,"clo2.med"
#     ,"Old_H_Traces_Se79_COUPLEX1.med"
#     ,"Old_H_Traces_I129_COUPLEX1.med"
#     ,"Old_H_PorfPorf_Se79_COUPLEX1.med"
#     ,"Old_H_PorfPorf_I129_COUPLEX1.med"
#     ,"Old_H_PorfCast_EFMH_Se79_COUPLEX1.med"
#     ,"Old_H_PorfCast_EFMH_I129_COUPLEX1.med"
#     ,"Old_H_CastPorf_Se79_COUPLEX1.med"
#     ,"Old_H_CastPorf_I129_COUPLEX1.med"
#     ,"Old_H_CastCast_VF_Se79_COUPLEX1.med"
#     ,"Old_H_CastCast_VF_I129_COUPLEX1.med"
#     ,"Old_H_CastCast_EFMH_I129_COUPLEX1.med"
#     ,"H_Traces_Se79_COUPLEX1.med"
#     ,"H_Traces_I129_COUPLEX1.med"
#     ,"H_PorfPorf_I129_COUPLEX1.med"
#     ,"H_PorfCast_EFMH_Se79_COUPLEX1.med"
#     ,"H_PorfCast_EFMH_I129_COUPLEX1.med"
#     ,"H_CastPorf_Se79_COUPLEX1.med"
#     ,"H_CastPorf_I129_COUPLEX1.med"
#     ,"H_CastCast_VF_Se79_COUPLEX1.med"
#     ,"H_CastCast_VF_I129_COUPLEX1.med"
#     ,"H_CastCast_EFMH_I129_COUPLEX1.med"
#     ,"puitsinst.med"
#     ,"Old_darcy2_Castem_EFMH.med"
#     ,"darcy2_Castem_EFMH.med"
#     ,"Old_darcy2_Castem_qua_VF.med"
#     ,"Old_darcy2_Castem_qua_EFMH.med"
#     ,"darcy2_Castem_qua_VF.med"
#     ,"darcy2_Castem_qua_EFMH.med"
#     ,"maillage_migr3d.med"
#     ,"Mistrat.med"
#     ,"Old_ChampsDarcy.med"
#     ,"ChampsDarcy.med"
#     ,"resu.2.med"
#     ,"maill.2.med"
#     ,"TimeStamps.med"
#     ,"hexa_28320_ELEM.med"
    ]

badFiles = [ # files that can't be read by MEDMEM
    "Deff_fdt_5.8_castem_efmh_diff_conc_dom.med"
    ,"Deff_fdt_5.8_castem_vf_diff_conc_dom.med"
    ]

medFiles = listdir( inDir )


for medFile in medFiles: # loop on med files in inDir

    basename, ext = os.path.splitext( medFile )
    if ext != ".med" or medFile in badFiles: continue

    # read MED file
    medFile = os.path.join( inDir, medFile )
    print "reading",medFile
    fields = []
    try:
        med = MEDFILEBROWSER(medFile)
        if med.getNumberOfMeshes() < 1: continue
        m2m_nom  = med.getMeshNames()[0]
        if med.isStructuredMesh(m2m_nom):
            mesh = GRID(MED_DRIVER,medFile,m2m_nom);
        else:
            mesh = MESH(MED_DRIVER,medFile,m2m_nom);
            pass

        for f_name in med.getFieldNames():
            if med.getMeshName(f_name) != mesh.getName(): continue
            dtit_list = med.getFieldIteration( f_name )
            for dtit in dtit_list:
                if med.getFieldType( f_name ) == MED_REEL64:
                    field = FIELDDOUBLE(MED_DRIVER, medFile, f_name, dtit.dt, dtit.it, mesh )
                else:
                    field = FIELDINT(MED_DRIVER, medFile, f_name, dtit.dt, dtit.it, mesh )
                    pass
                fields.append( field )
    except:
        print sys.exc_value
        continue

    ShowMesh( mesh )
    print "Nb Fields",len(fields)
    #ShowGroups( mesh )
    #ShowFields( fields, 10 )

    basename = os.path.join( outDir, basename ) + '.'
    checkFile = basename + "ens_checker"

    for format,bin in formats:
        if format == ENSIGHT_GOLD:
            formatName = "ENSIGHT GOLD"
            formatSuffix = "g"
        else:
            formatName = "ENSIGHT 6"
            formatSuffix = '6'
            pass
        if bin:
            formatName += " Binary"
            formatSuffix += 'b'
        else:
            formatName += " ASCII"
            formatSuffix += 'a'
            pass
        ensFile = basename + formatSuffix + ".case"
        print '*'*80
        print "Format: \t",formatName
        print '*'*80

        setEnSightFormatForWriting( format, bin )

        # write a mesh to EnSight
        # print "write a mesh to EnSight"
        # myMeshDriver = ENSIGHT_MESH_WRONLY_DRIVER(ensFile,mesh)
        # myMeshDriver.write()

        # if med.getNumberOfFields() > 0:
        #     fName = med.getFieldName( 0 )
        #     dt_it_= med.getFieldIteration( fName, 0 )
        #     field = med.getField( fName, dt_it_.dt, dt_it_.it )
        #     print "add a field to EnSight"
        #     myFieldDriver = ENSIGHT_FIELD_WRONLY_DRIVER(ensFile,field);
        #     myFieldDriver.write();

        # check_ens(ensFile, checkFile)

        # write EnSight
        print "writting", ensFile
        try:
            if fields:
                medEnsDriver = ENSIGHT_MED_WRONLY_DRIVER (ensFile, fields)
                medEnsDriver.write()
            else:
                mesh.write( ENSIGHT_DRIVER, ensFile )
        except:
            if not compatibilityPb():
                sys.exit(1)
            continue

        # check generated EnSight
        check_ens(ensFile, checkFile)

        # read generated EnSight into MEDMEM
        print "reading",ensFile
        if fields:
            medFromEns = ENSIGHT_MED_RDONLY_DRIVER(ensFile)
            fields = medFromEns.read()
            mesh = fields[0].getSupport().getMesh()
        else:
            mesh = MESH(ENSIGHT_DRIVER, ensFile, "")

        # dump generated EnSight
        if dumpMesh:
            #ShowMesh( mesh )
            ShowMesh( mesh, 10, [10,10,10] )
            #ShowGroups( mesh )
            pass
        if dumpField:
            ShowFields( fields, 10 )
            pass

        # write EnSight to MED
        wFile = basename + formatSuffix + ".med"
        deleteFile(wFile)
        print "write",wFile
        try:
            mesh.write( MED_DRIVER,wFile )
            for f in fields:
                fTyped = f.castToTypedField()
                fTyped.write(MED_DRIVER,wFile)
        except:
            import traceback
            traceback.print_exc()
            pass
        pass

    print "\n\n\n"
            

# remove created files
for f in listdir( outDir ):
    deleteFile( os.path.join( outDir, f ))
