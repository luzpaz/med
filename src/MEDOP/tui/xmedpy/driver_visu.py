#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2011-2014  CEA/DEN, EDF R&D
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

import VISU
import MEDCoupling
__mapMedType2VisuType={
    MEDCoupling.ON_CELLS:VISU.CELL,
    MEDCoupling.ON_NODES:VISU.NODE,
    MEDCoupling.ON_GAUSS_PT:VISU.TGAUSSPOINTS
    }

def visu_typeOfField(medTypeOfField):
    """
    This function gives the visu type corresponding to the specified
    med type.
    """
    try:
        return __mapMedType2VisuType[medTypeOfField]
    except IndexError, e:
        return "UNCKNOWN"


import salome
from libSALOME_Swig import SALOMEGUI_Swig

def visu_scalarmap(filename,meshname,fieldname,typeOfField,iteration=-1):
    """
    This is the minimalist function to render a scalar map on a field
    load from a med file using the VISU module.
    """

    # We first have to prepare a pointer to the VISU component engine.
    visuComp = salome.lcc.FindOrLoadComponent("FactoryServer", "VISU")
    visuComp.SetCurrentStudy(salome.myStudy)

    visumed = visuComp.CreateResult(filename)
    visumed.SetBuildGroups(True)
    visumed.SetBuildFields(True, True)
    visumed.Build(False, True)
    if not visumed.IsDone() :
        print "ERR: can't create a representation of med data"
        return False
    
    visuComp.RenameEntityInStudy(visumed, meshname, VISU.NODE, 'onNodes')
    visuComp.RenameEntityInStudy(visumed, meshname, VISU.CELL, 'onCells')

    visuType = visu_typeOfField(typeOfField)
    scalarmap = visuComp.ScalarMapOnField(visumed,
                                          meshname,
                                          visuType,
                                          fieldname,
                                          iteration)
    if scalarmap is None:
        print "ERR: can't create a scalar map"
        return False
    
    # __GBO__ maybe it could be appreciated to select the component to
    # display. In this interface, the modulus of the field is
    # considered.
    component  = 1
    scalarmap.SetScalarMode(component)
    scalarmap.SetSourceRange()
    scalarmap.SetScaling(VISU.LINEAR)
    scalarmap.SetTitle(fieldname)
    
    # This final part is to automatically display the scalar map in a
    # VISU viewer.
    sg = SALOMEGUI_Swig()
    sg.updateObjBrowser(1)
    
    # Display the scalar map in the viewer
    myViewManager = visuComp.GetViewManager()
    myView = myViewManager.Create3DView()
    myView.Maximize()
    myView.Display(scalarmap);
    myView.SetFocalPoint([0,0,0]);
    myView.FitAll();

    return True


def TEST_scalarmap():
    import os
    from xmed import properties
    #properties.setup(properties.testdata_02) # test with nodes
    properties.setup(properties.testdata_03) # test with cells

    # __GBO__: WARN due to a specific feature of VISU, when only one
    # field timestamps exists in the med file, we have to specify an
    # iteration number of 1, whatever the iteration value is in the
    # med file.
    #iteration = properties.testFieldIt
    iteration = 1
    
    visu_scalarmap(properties.testFilePath,
                   properties.testMeshName,
                   properties.testFieldName,
                   properties.testTypeOfField,
                   iteration)
    
if __name__ == "__main__":
    TEST_scalarmap()
