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
        return "UNKNOWN"

def pvis_scalarmap(filename,meshname,fieldname,typeOfField,iteration=-1):
    """
    This is the minimalist function to render a scalar map on a field
    load from a med file using the PARAVIS module.
    """
    import pvsimple

    reader = pvsimple.MEDReader( FileName=filename )

    representation = pvsimple.Show()
    if typeOfField == MEDCoupling.ON_CELLS:
        representation.ColorArrayName = ("CELLS", fieldname)
        data = reader.CellData
    else:
        representation.ColorArrayName = ("POINTS", fieldname)
        data = reader.PointData

    # :TODO: Determine nb components
    nb_cmp = 1
    mode = "Magnitude" # "Component" if nb_cmp > 1

    # Get data range (mini/maxi)
    for n in range(data.GetNumberOfArrays()):
        if data.GetArray(n).GetName() == fieldname:
            mini,maxi = data.GetArray(n).GetRange()

    stepvalue = (maxi-mini)/100. # 100 steps

    # Build Lookup table
    RGBPoints = [mini, 0.0, 0.0, 1.0, maxi, 1.0, 0.0, 0.0]
    nb = int((maxi-mini)/stepvalue)-1
    Table = pvsimple.GetLookupTableForArray("", nb_cmp, VectorMode=mode, ColorSpace='HSV')
    Table.Discretize = 1
    Table.NumberOfTableValues = nb
    Table.RGBPoints = RGBPoints

    representation.Representation = 'Surface'
    representation.LookupTable = Table

    # Build scalar bar
    scalarbar = pvsimple.CreateScalarBar(LabelFormat = '%.1f',Title= "",LabelFontSize=12,Enabled=1,LookupTable=Table,TitleFontSize=12,LabelColor=[0.0, 0.0, 0.0],TitleColor=[0.0, 0.0, 0.0],)
    pvsimple.SetActiveSource(reader)
    pvsimple.GetRenderView().Representations.append(scalarbar)
    pvsimple.SetActiveSource(reader)
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


