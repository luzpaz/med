#!/usr/bin/env python
#  -*- coding: iso-8859-1 -*-
#
# This file is a set of basic use case to test (from the python
# context) the functions developped in XMED and MED modules for the
# visualization of fields using VISU or PARAVIS.
#
# (gboulant - 6/7/2011)
#

import xmed
from xmed import properties
from xmed.fieldproxy import FieldProxy

# Load some test data in the MedDataManager
filepath  = properties.testFilePath
fieldHandlerList = xmed.dataManager.addFieldsFromFile(filepath)

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

def TEST_driver_visu():
    from xmed import fieldtools
    fieldtools.view_using_visu(f1)

if __name__ == "__main__":
    TEST_driver_visu()
