#!/usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2015  CEA/DEN, EDF R&D
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

# This file is a set of basic use case to test (from the python
# context) the functions developped in MED modules for the field
# operations.
#
# (gboulant - 16/6/2011)
#
import xmed
from xmed import properties
from xmed import fieldproxy
from xmed.fieldproxy import FieldProxy
#from xmed.fieldtools import dup, stat, get, save
#from xmed.fieldguide import doc

# Don't forget to set the globals dictionnary for the fields tools to
# work properly
xmed.setConsoleGlobals(globals())

# Load some test data in the MedDataManager
filepath  = properties.testFilePath
xmed.dataManager.loadDatasource(filepath)
fieldHandlerList = xmed.dataManager.getFieldHandlerList()

def setup():
    """
    This function defines a set of field variable for quick tests in
    the python console. You just have to execute the function to get
    the variables defined in the global context.
    """
    fh1=fieldHandlerList[0]
    fh2=fieldHandlerList[1]
    f1 = FieldProxy(fh1)
    f2 = FieldProxy(fh2)
    return fh1, fh2, f1, f2

# Setup for quick tests in the python console
fh1, fh2, f1, f2 = setup()

def TEST_addition():
    fieldHandler0 = fieldHandlerList[0]
    fieldHandler1 = fieldHandlerList[1]

    # The addition can be done using field handler directly
    addFieldHandler = xmed.calculator.add(fieldHandler0, fieldHandler1)
    print addFieldHandler

    # Or with a field proxy that ease the writing of operations
    fieldProxy0 = FieldProxy(fieldHandler0)
    fieldProxy1 = FieldProxy(fieldHandler1)

    res = fieldProxy0 + fieldProxy1
    if res is None: return False

    return True

def TEST_arithmetics():
    fieldProxy0 = FieldProxy(fieldHandlerList[0])
    fieldProxy1 = FieldProxy(fieldHandlerList[1])

    # Standard operations where operandes are fields
    res = fieldProxy0 + fieldProxy1
    if res is None: return False
    res = fieldProxy0 - fieldProxy1
    if res is None: return False
    res = fieldProxy0 * fieldProxy1
    if res is None: return False
    res = fieldProxy0 / fieldProxy1

    # Standard operations with scalar operandes
    res = fieldProxy0 + 3.4
    if res is None: return False
    res = 3.4 + fieldProxy0
    if res is None: return False
    res = fieldProxy0 - 3.4
    if res is None: return False
    res = 3.4 - fieldProxy0
    if res is None: return False
    res = fieldProxy0 * 3.4
    if res is None: return False
    res = 3.4 * fieldProxy0
    if res is None: return False
    res = fieldProxy0 / 3.4
    if res is None: return False
    res = 3.4 / fieldProxy0
    if res is None: return False

    return True

def TEST_unary_operations():
    fieldProxy0 = FieldProxy(fieldHandlerList[0])

    res = fieldProxy0.dup()
    if res is None: return False
    res = xmed.dup(fieldProxy0)
    if res is None: return False
    res = pow(fieldProxy0,2)
    if res is None: return False

    return True

def TEST_composition():
    # In this test, we combine operandes that are supposed
    # to be compatible. We expect that no error occur.
    fieldProxy0 = FieldProxy(fieldHandlerList[0])
    fieldProxy1 = FieldProxy(fieldHandlerList[1])

    res = pow(fieldProxy0,2) + fieldProxy1
    if res is None: return False

    return True

def TEST_litteral_equation():
    fieldProxy0 = FieldProxy(fieldHandlerList[0])
    res = fieldProxy0.ope("abs(u)^2")
    if res is None: return False
    return True

def TEST_use_restriction():
    fieldProxy0 = FieldProxy(fieldHandlerList[0])
    res = fieldProxy0("c=1;g='toto'")
    if res is None: return False
    return True

def TEST_modification_of_attributes():
    fieldProxy0 = FieldProxy(fieldHandlerList[0])
    id_ref = fieldProxy0.id
    fieldname_ref = fieldProxy0.fieldname
    meshname_ref = fieldProxy0.meshname

    #
    # This operations are not allowed, or not that way
    #
    # This should print that it is not allowed:
    fieldProxy0.id = id_ref+3
    if fieldProxy0.id != id_ref:
        print "ERR: the id should be %d (%d found)"%(id_ref,fieldProxy0.id)
        return False
    # This should print that it must be done using the command update
    fieldProxy0.fieldname = fieldname_ref+"toto"
    if fieldProxy0.fieldname != fieldname_ref:
        print "ERR: the fieldname should be %s (%s found)"%(fieldname_ref,fieldProxy0.fieldname)
        return False
    # This should print that it is not allowed:
    fieldProxy0.meshname = meshname_ref+"titi"
    if fieldProxy0.meshname != meshname_ref:
        print "ERR: the meshname should be %s (%s found)"%(meshname_ref,fieldProxy0.meshname)
        return False

    return True

def TEST_update_metadata():
    fieldProxyRef = FieldProxy(fieldHandlerList[0])
    id = fieldProxyRef.id

    name_ref = "toto"
    fieldProxyRef.update(name=name_ref)

    fieldProxyRes = xmed.get(id)
    name_res = fieldProxyRes.fieldname
    if name_res != name_ref:
        print "ERR: the fieldname should be %s (%s found)"%(name_ref,name_res)
        return False
    return True

#
# =============================================================
# Unit tests
# =============================================================
#
import unittest
from salome.kernel import pyunittester
class MyTestSuite(unittest.TestCase):
    def test_addition(self):
        result = pyunittester.execAndConvertExceptionToBoolean(TEST_addition)
        self.assertTrue(result)

    def test_arithmetics(self):
        result = pyunittester.execAndConvertExceptionToBoolean(TEST_arithmetics)
        self.assertTrue(result)

    def test_unary_operations(self):
        result = pyunittester.execAndConvertExceptionToBoolean(TEST_unary_operations)
        self.assertTrue(result)

    def test_composition(self):
        result = pyunittester.execAndConvertExceptionToBoolean(TEST_composition)
        self.assertTrue(result)

    def test_litteral_equation(self):
        result = pyunittester.execAndConvertExceptionToBoolean(TEST_litteral_equation)
        self.assertTrue(result)

    def test_modification_of_attributes(self):
        self.assertTrue(TEST_modification_of_attributes())

    def test_update_metadata(self):
        self.assertTrue(TEST_update_metadata())

def myunittests():
    pyunittester.run(MyTestSuite)

def myusecases():
    TEST_addition()
    #TEST_arithmetics()
    #TEST_unary_operations()
    #TEST_update_metadata()
    #TEST_composition()

if __name__ == "__main__":
    #myusecases()
    myunittests()
    pass

