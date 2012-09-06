#!/usr/bin/env python
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
