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
# This script tests conversion of EnSight to MEDMEM by performing following
# operations on all available EnSight files:
# - read EnSight file into MEDMEM and breifly dump it's content;
# - store MEDMEM to med file;
# - write EnSight file in different formats: Gold and EnSight6, ASCII and binary;
# - checks generated EnSight files using ens_checker utility (if available).
# EnSight samples are in EXAMPLES salome CVS repository, in directory SAMPLES_SRC/EnSight.
# DATA_DIR should contain path to SAMPLES_SRC
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

dataDir = os.getenv("DATA_DIR")
tmpDir  = os.getenv("TMP")
if not tmpDir:
    tmpDir = os.getenv("TMPDIR", "/tmp")

# EnSight samples are in EXAMPLES CVS repository, in directory SAMPLES_SRC/EnSight.
# DATA_DIR should contain path to SAMPLES_SRC
inDir = os.path.join( dataDir, "EnSight")
if not os.access(inDir, os.F_OK):
    raise RuntimeError, "Path to EnSight files not found, check DATA_DIR environment"

outDir = os.path.join( tmpDir,"Ensight_out")
if not os.access(outDir, os.F_OK):
    os.mkdir( outDir )
    pass

# find out if ens_checker is available
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
    if not has_ens_checker:
        return
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

inFiles = [
    "frame.case"
#     ,"blow1_bin.case"
#     ,"zmat2d_esca.case"
#     ,"dyna.case"
#     ,"en6.case"
#     ,"engold.case"
#     ,"engoldup.case"
#     ,"frame.case"
#     ,"ghost_structured.case"
#     ,"ghost_unstructured.case"
#     ,"ironProt_ascii.case"
#     ,"ironProt_bin.case"
#     ,"mandelbrot1.case"
#     ,"mandelbrot2.case"
#     ,"naca.bin.case"
#     ,"office6_bin.case"
#     ,"office_ascii.case"
#     ,"office_bin.case"
#     ,"performance.case"
#     ,"range_structured.case"
#     ,"x29.case"
#     ,"RectGrid_ascii.case"
#     ,"RectGrid_bin.case"
#     ,"ami.case"
#     ,"ami6.case"
#     ,"anim.case"
#     ,"blow2_ascii.case"
#     ,"blow2_bin.case"
#     ,"blow3_bin.case"
#     ,"blow4_bin.case"
#     ,"blow5_ascii.case"
#     ,"crash.case"
#     ,"cube.case"
#     ,"cubeE.case"
    ]
inFiles = listdir( inDir )


for inFile in inFiles: # loop on all files in inDir

    # filter .case files
    basename, ext = os.path.splitext( inFile )
    if ext != ".case": continue

    # read EnSight into MEDMEM

    setIgnoreIncompatibility(0)
    
    ensFile = os.path.join( inDir, inFile )
    print "\nreading",ensFile
    incompatible = False
    meshes = []
    fields = []
    try:
        medFromEns = ENSIGHT_MED_RDONLY_DRIVER(ensFile);
        fields = medFromEns.read()
        if not fields:
            mesh = MESH(ENSIGHT_DRIVER,ensFile,"");
            meshes.append( mesh )
        else:
            meshNames = []
            for f in fields:
                mesh = f.getSupport().getMesh()
                if mesh and not mesh.getName() in meshNames:
                    meshes.append( mesh )
                    meshNames.append( mesh.getName() )
                    pass
                pass
            pass
        pass
    except:
        if not compatibilityPb():
            sys.exit(1)
        else:
            continue

    # show MEDMEM contents
    mesh = meshes[0]
    if dumpMesh:
        ShowMesh( mesh, 10, [10,10,10] )
        ShowGroups( mesh )
        pass
    if dumpField:
        ShowFields( fields, 10 )

    # write MEDMEM into MED

    medFile = os.path.join( outDir, basename + ".med" )
    deleteFile(medFile)
    print "write",medFile
    for m in meshes:
        m.write( MED_DRIVER,medFile )
        pass
    for f in fields:
        fTyped = f.castToTypedField()
        fTyped.write(MED_DRIVER,medFile)
        pass

    # write MEDMEM into EnSight

    for format,bin in formats:
        ensFile = os.path.join( outDir, basename )
        if format == ENSIGHT_GOLD:
            formatName = "ENSIGHT GOLD"
            ensFile += ".g"
        else:
            formatName = "ENSIGHT 6"
            ensFile += '.6'
            pass
        if bin:
            formatName += " Binary"
            ensFile += 'b'
        else:
            formatName += " ASCII"
            ensFile += 'a'
            pass
        ensFile += ".case"
        print '*'*80
        print "Format: \t",formatName
        print '*'*80

        setEnSightFormatForWriting( format, bin )

        print "writting", ensFile
        incompatible = False
        try:
            if fields:
                medEnsDriver = ENSIGHT_MED_WRONLY_DRIVER (ensFile, fields)
                medEnsDriver.write()
            else:
                mesh2EnsDriver = ENSIGHT_MESH_WRONLY_DRIVER( ensFile, mesh )
                mesh2EnsDriver.write()
        except:
            if not compatibilityPb():
                sys.exit(1)
                pass
            # try to write anayway, maybe it will be ok
            incompatible = True
            setIgnoreIncompatibility(1)
            try:
                if fields:
                    medEnsDriver.write()
                else:
                    mesh2EnsDriver.write()
            except:
                if not compatibilityPb():
                    sys.exit(1)
                    pass
                continue
            pass

        # check generated EnSight
        try:
            checkFile = os.path.join( outDir, basename + ".ens_checker" )
            check_ens(ensFile, checkFile)
        except:
            if incompatible:
                # Ok I was warned about it
                continue
            sys.exit(1)
            pass
        pass
    print "\n\n\n"


# remove created files
for f in listdir( outDir ):
    deleteFile( os.path.join( outDir, f ))
