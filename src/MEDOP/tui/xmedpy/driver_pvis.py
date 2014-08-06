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

import MEDCoupling
__mapTypeOfField2ParavisLabel={
    MEDCoupling.ON_CELLS:"OnCell",
    MEDCoupling.ON_NODES:"OnPoint",
    MEDCoupling.ON_GAUSS_PT:"ON_GAUSS_PT",
    MEDCoupling.ON_GAUSS_NE:"ON_GAUSS_NE"}

def pvis_typeOfFieldLabel(typeOfField):
    # A field name could identify several MEDCoupling fields, that
    # differ by their spatial discretization on the mesh (values on
    # cells, values on nodes, ...). This spatial discretization is
    # specified (at least) by the TypeOfField that is an integer value
    # in this list:
    # 0 = ON_CELLS = OnCell
    # 1 = ON_NODES = OnPoint
    # 2 = ON_GAUSS_PT
    # 3 = ON_GAUSS_NE
    try:
        return __mapTypeOfField2ParavisLabel[typeOfField]
    except IndexError, e:
        return "UNCKNOWN"

def pvis_scalarmap(filename,meshname,fieldname,typeOfField,iteration=-1):
    """
    This is the minimalist function to render a scalar map on a field
    load from a med file using the PARAVIS module.
    """
    import pvsimple
    
    reader = pvsimple.MEDReader( FileName=filename )
    # For fiels defined on cells, it seems to be required to specify
    # the type of fields
    if typeOfField == MEDCoupling.ON_CELLS:
        reader.Groups = ["GROUP/%s/%s/No_Group"%(meshname,pvis_typeOfFieldLabel(typeOfField))]

    representation = pvsimple.GetDisplayProperties(reader)
    representation.ColorArrayName = fieldname

    lookupTable = pvsimple.GetLookupTableForArray(
        fieldname, 1, NanColor=[0.25, 0.0, 0.0],
        RGBPoints=[25.0, 0.23, 0.30, 0.754, 245.0, 0.71, 0.016, 0.15],
        VectorMode='Magnitude', ColorSpace='Diverging', ScalarRangeInitialized=1.0 )
    representation.LookupTable = lookupTable

    pvsimple.Render()

    return True


def TEST_scalarmap():
    import os
    from xmed import properties
    #properties.setup(properties.testdata_02) # test with nodes
    properties.setup(properties.testdata_03) # test with cells
    pvis_scalarmap(properties.testFilePath,
                   properties.testMeshName,
                   properties.testFieldName,
                   properties.testTypeOfField,
                   properties.testFieldIt)
    
if __name__ == "__main__":
    TEST_scalarmap()


