#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# This file defines some properties for test purposes

import os

try:
    MED_ROOT_DIR=os.environ["MED_ROOT_DIR"]
except KeyError, e:
    raise RuntimeError("MED_ROOT_DIR should be defined to load the test data")

RESDIR=os.path.join(MED_ROOT_DIR,"share","salome","resources","med","medop_testfiles")

def testdata_01():
    testFileName = "testfield.med"
    testMeshName = "Mesh"
    testFieldName= "testfield1"
    testFieldIt  = -1
    testFieldDt  = -1
    testTypeOfField = 0 # On cells
    return testFileName, testMeshName, testFieldName, testTypeOfField, testFieldIt, testFieldDt

def testdata_02():
    testFileName = "smallmesh_varfield.med"
    testMeshName = "My2DMesh"
    testFieldName= "testfield2"
    testFieldIt  = 1
    testFieldDt  = 1
    testTypeOfField = 1 # On nodes
    return testFileName, testMeshName, testFieldName, testTypeOfField, testFieldIt, testFieldDt

def testdata_03():
    testFileName = "fieldlena.med"
    testMeshName = "lenamesh"
    testFieldName= "lenafield"
    testFieldIt  = 3
    testFieldDt  = 0
    testTypeOfField = 0 # On cells
    return testFileName, testMeshName, testFieldName, testTypeOfField, testFieldIt, testFieldDt

# Default values
testdata=testdata_02
testFileName, testMeshName, testFieldName, testTypeOfField, testFieldIt, testFieldDt = testdata()
testFilePath = os.path.join(RESDIR,testFileName)

# Setup test values
def setup(testdata=testdata_02):
    global testFileName, testMeshName, testFieldName
    global testFieldIt, testFieldDt, testFilePath, testTypeOfField
    testFileName, testMeshName, testFieldName, testTypeOfField, testFieldIt, testFieldDt = testdata()
    testFilePath = os.path.join(RESDIR,testFileName)

def display():
    """
    Displays the properties values defined in this script.
    """
    print 60*"=" + " Properties values - START"
    print "MED_ROOT_DIR = ",MED_ROOT_DIR
    print "RESDIR       = ",RESDIR
    print "testFileName = ",testFileName
    print "testFilePath = ",testFilePath
    print 60*"=" + " Properties values - END"

#display()

def getFilePath(filename):
    """
    Returns the absolute path for a given file base name. The base
    name must match with a file contained in the test files directory.
    """
    filepath = os.path.join(RESDIR,filename)
    if not os.path.exists(filepath):
        raise RuntimeError("The file %s does not exists"%filepath)
    return filepath
