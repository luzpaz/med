#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2011-2015  CEA/DEN, EDF R&D
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
# Author : Guillaume Boulant (EDF)

#
# ===================================================================
# The functions of this module must be imported as global functions in
# the SALOME python console. It provides the console context with Tool
# functions to manipulate field throw there fieldproxy.
# ===================================================================
#

import xmed
from xmed.fieldproxy import FieldProxy, newFieldProxy

# ===================================================================
# Operations on fields

def dup(aFieldProxy):
    """
    This function returns a duplicate of the specified field.
    It's equivalent to the call '>>> copy = f.dup()'.
    """
    try:
        clone = aFieldProxy.dup()
    except Exception:
        print "ERR: the argument can't be duplicated as a field object"
        return None
    return clone

# ===================================================================
# Get list a field with information using the commands ls and/or la

# IMPORTANT NOTE:
# the pyConsoleGlobals variable should holds the globals() dictionnary of
# the python console context
pyConsoleGlobals=None

def status(local=True,remote=False):
    """
    This function return the status of the medop context, i.e. the
    list of fields defined in this python session.
    """
    status=""
    if local is True:
        dvars = pyConsoleGlobals
        if dvars is None:
            xmed.wrn("The stat function required the specification of the python context")
            xmed.inf("Type this command \"import xmed; xmed.setConsoleGlobals(globals())")
        if remote is True:
            status="========= Fields used in the current context ===\n"
        for varkey in dvars.keys():
            var = dvars[varkey]
            if isinstance(var, FieldProxy):
                status+="%s \t(id=%s, name=%s)\n"%(varkey,var.id,var.fieldname)

    if remote is True:
        if local is True:
            status+="\n========= Fields available in the data manager ===\n"
        fieldHandlerList = xmed.dataManager.getFieldHandlerList()
        for fieldHandler in fieldHandlerList:
            status+="id=%s\tname\t= %s\n\tmesh\t= %s\n\t(it,dt)\t= (%s,%s)\n\tsource\t= %s\n"%(
                fieldHandler.id,
                fieldHandler.fieldname,
                fieldHandler.meshname,
                fieldHandler.iteration,
                fieldHandler.order,
                fieldHandler.source)
            status+="---------\n"

        if len(fieldHandlerList) > 0:
            status+="(use 'f=get(id)' to get a field in the current context)"

    return status

# For simpler typing, one can create a python command for status
# (avoid to type "print status()")
class ListFields(object):
    """
    A stat object displays the status of the med operating context, i.e. the
    list of fields defined in this python session.
    """
    def __init__(self,all=False):
        self.__local  = True
        self.__remote = all
        # all = True means that the local metadata (fieldproxy) and the
        # remote metadata on the engine (MEDCouplingFieldDouble) are
        # displayed by the stat command. Otherwise, only the local
        # metadata are displayed.

    def __repr__(self):
        return status(self.__local, self.__remote)

# Creating the commands list (ls) and list all (la)
ls=ListFields(all=False)
la=ListFields(all=True)

# ===================================================================
# Remove variable from console
from xmed.fieldproxy import notifyGui_remove
def remove(aFieldProxy):
    dvars = pyConsoleGlobals
    if dvars is None:
        return
    for varkey, var in dvars.items():
        if isinstance(var, FieldProxy) and var.id == aFieldProxy.id:
            exec("del %s"%varkey) in pyConsoleGlobals
    notifyGui_remove(aFieldProxy.id)
#

# Clean workspace
from xmed.fieldproxy import notifyGui_clean
def clean():
    dvars = pyConsoleGlobals
    if dvars is None:
        return
    all_keys = []
    for varkey, var in dvars.items():
        if isinstance(var, FieldProxy):
            all_keys.append("%s"%varkey)
    if len(all_keys) > 0:
        exec "del "+",".join(all_keys) in pyConsoleGlobals
    notifyGui_clean()
#

# ===================================================================
# Field Data Management
from xmed import properties
filepath  = properties.testFilePath

def load(medFileName=filepath):
    """
    This function indicates that we want to use the fields from the
    specified med file. The fields meta-data are loaded in the engine
    part of the module. To get a fieldproxy on a field, call the get
    function with the id of the required field. To display the whole
    list of fields loaded in the engine, type 'ls' (la verbose).
    """
    xmed.dataManager.addDatasource(filepath)
    print status(local=False,remote=True)

def get(fieldHandlerId):
    """
    This return a field proxy on the field identified by the specified
    field handler id.
    """
    return newFieldProxy(fieldHandlerId)

from xmed.fieldproxy import notifyGui_add
def put(aFieldProxy):
    """
    This function puts a reference to this field in the GUI data
    model. When a field is referenced in the GUI data model, then it
    belongs to the workspace. When the workspace is saved, all the
    field that belongs to the workspace are saved.
    """
    xmed.dataManager.markAsPersistent(aFieldProxy.id, True)
    notifyGui_add(aFieldProxy.id)

import SALOME
def save(filename):
    """
    Dump your job in a med file. Only the fields marked as persistent
    are saved in the specified file.
    """
    try:
        xmed.dataManager.savePersistentFields(filename)
    except SALOME.SALOME_Exception, ex:
        xmed.err(ex.details.text)

# ===================================================================
# Field Data visualization
VIEWER_VISU    = "VISU"
VIEWER_PARAVIS = "PARAVIS"
VIEWER_DEFAULT = VIEWER_PARAVIS

import tempfile

def view_using_paravis(aFieldProxy):
    temp = tempfile.NamedTemporaryFile(prefix="medop_viewer", suffix='.med', delete=False)

    # __GBO__ TO BE IMPROVED: we used a tmp file in this first step of
    # development, but we should used at last a MEDCoupling corba
    # object (see how to use the stuff in PARAVIS/src/Plugins)
    xmed.dataManager.saveFields(temp.name, [aFieldProxy.id])

    from xmed.driver_pvis import pvis_scalarmap
    pvis_scalarmap(temp.name,
                   aFieldProxy.meshname,
                   aFieldProxy.fieldname,
                   aFieldProxy.type,
                   aFieldProxy.iteration)
    temp.close()
#

def view_using_visu(aFieldProxy):
    # No more used
    raise Exception("view_using_visu: No more used")

    VIEWER_TMP_FILE = "/tmp/medop_viewer.med"

    # __GBO__ TO BE IMPROVED: we used a tmp file in this first step of
    # development, but we should used at last a MEDCoupling corba
    # object (see how to use the stuff in PARAVIS/src/Plugins)
    xmed.dataManager.saveFields(VIEWER_TMP_FILE, [aFieldProxy.id])

    # __GBO__: WARN due to a specific feature of VISU, when only one
    # field timestamps exists in the med file, we have to specify an
    # iteration number of 1, whatever the iteration value is in the
    # med file.
    iteration = 1 # __GBO__ for bug VISU
    # instead of:
    # iteration = aFieldProxy.iteration

    from xmed.driver_visu import visu_scalarmap
    result = visu_scalarmap(VIEWER_TMP_FILE,
                            aFieldProxy.meshname,
                            aFieldProxy.fieldname,
                            aFieldProxy.type,
                            iteration)
    if result is False:
        xmed.err("the field can't be displayed")

def view(aFieldProxy, using=VIEWER_DEFAULT):
    """
    This displays a 3D view of the field using VISU or PARAVIS,
    depending on the configuration. This view is for quick visual
    control of a field and is not intended to be parametrizable. For
    custom display, you should import the fields in VISU or PARAVIS
    and continue the job in one of this SALOME module.
    """
    if using == VIEWER_PARAVIS:
        view_using_paravis(aFieldProxy)
    else:
        view_using_visu(aFieldProxy)

