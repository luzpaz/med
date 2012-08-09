#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

# Check that numpy arrays can be used to define coordinates, connectivities and field values

import unittest
from libMEDMEM_Swig import *

class medNumPyTest(unittest.TestCase):
    def test0(self):

        try:
            from numpy import array, arange, ndarray
        except:
            # numpy is not available, it is not an error
            print "numpy unavailable"
            return

        myMeshing = MESHING()
        myMeshing.setName("meshing")

        spaceDimension = 3
        numberOfNodes = 19
        coordinates = [0.0, 0.0, 0.0  ,
                       0.0, 0.0, 1.0  ,
                       2.0, 0.0, 1.0  ,
                       0.0, 2.0, 1.0  ,
                       -2.0, 0.0, 1.0 ,
                       0.0, -2.0, 1.0 ,
                       1.0, 1.0, 2.0  ,
                       -1.0, 1.0, 2.0 ,
                       -1.0, -1.0, 2.0,
                       1.0, -1.0, 2.0 ,
                       1.0, 1.0, 3.0  ,
                       -1.0, 1.0, 3.0 ,
                       -1.0, -1.0, 3.0,
                       1.0, -1.0, 3.0 ,
                       1.0, 1.0, 4.0  ,
                       -1.0, 1.0, 4.0 ,
                       -1.0, -1.0, 4.0,
                       1.0, -1.0, 4.0 ,
                       0.0, 0.0, 5.0  ]

        myMeshing.setCoordinates(spaceDimension,numberOfNodes, array( coordinates ),
                                 "CARTESIAN",MED_FULL_INTERLACE)
        coord = myMeshing.getCoordinates(MED_FULL_INTERLACE)
        self.assertTrue( isinstance( coord, ndarray ))
        
        units = ["cm      ", "cm      ", "cm      "]
        names = ["X       ", "Y       ", "Z       "]
        myMeshing.setCoordinatesNames(names)
        myMeshing.setCoordinatesUnits(units)

        # definition of connectivities

        numberOfTypes = 1
        entity = MED_CELL

        types = [MED_TETRA4]
        numberOfElements = [12]

        myMeshing.setNumberOfTypes(numberOfTypes,entity)
        myMeshing.setTypes(types,entity)
        myMeshing.setNumberOfElements(numberOfElements,entity)

        connectivityTetra =  [1,2,3,6,
                              1,2,4,3 ,
                              1,2,5,4 ,
                              1,2,6,5 ,
                              2,7,4,3 ,
                              2,8,5,4 ,
                              2,9,6,5 ,
                              2,10,3,6,
                              2,7,3,10,
                              2,8,4,7 ,
                              2,9,5,8 ,
                              2,10,6,9]

        myMeshing.setConnectivity(entity,types[0], array( connectivityTetra ))
        conn = myMeshing.getConnectivity(MED_NODAL,entity,MED_ALL_ELEMENTS)
        self.assertTrue( isinstance( conn, ndarray ))

        myGroup = GROUP()
        myGroup.setName("SomeCells")
        myGroup.setMesh(myMeshing)
        myGroup.setEntity(MED_CELL)
        myGroup.setNumberOfGeometricType(1)

        myGroup.setGeometricType(types)
        myNumberOfElements = [4,1,2]
        myGroup.setNumberOfElements(myNumberOfElements)

        index = [1,5,6,8]
        values = [2,7,8,12,13,15,16]
        myGroup.setNumber( array( index ), array( values ))

        myMeshing.addGroup(myGroup)

        # saving the generated mesh in MED

        import os
        tmpDir  = os.getenv("TMP")
        if not tmpDir:
            tmpDir = os.getenv("TMPDIR", "/tmp")

        filename = os.path.join(tmpDir,"medNumPy_test.med")
        myMeshing.write(MED_DRIVER,filename)

        # we build now 2 fields

        supportOnNodes = SUPPORT(myMeshing,"On_All_Nodes",MED_NODE)
        numberOfNodes = supportOnNodes.getNumberOfElements(MED_ALL_ELEMENTS)

        fieldDoubleScalarOnNodes = FIELDDOUBLE(supportOnNodes,1)
        fieldDoubleScalarOnNodes.setName("fieldScalarDoubleNode")
        fieldDoubleScalarOnNodes.setIterationNumber(-1)
        fieldDoubleScalarOnNodes.setOrderNumber(-1)
        fieldDoubleScalarOnNodes.setTime(0.0)

        fieldDoubleScalarOnNodes.setComponentName(1,"Vx")
        fieldDoubleScalarOnNodes.setComponentDescription(1,"comp1")
        fieldDoubleScalarOnNodes.setMEDComponentUnit(1,"unit1")

        nodeValues = arange( numberOfNodes, dtype=float )
        fieldDoubleScalarOnNodes.setValue( nodeValues )

        resValue = fieldDoubleScalarOnNodes.getValue()
        self.assertTrue( isinstance( resValue, ndarray ))

        intArray = arange( numberOfNodes, dtype=int )
        self.assertRaises( TypeError, fieldDoubleScalarOnNodes.setValue, intArray )

        fieldDoubleScalarOnNodes.write(MED_DRIVER,filename)


        supportOnCells = SUPPORT(myMeshing,"On_All_Cells",MED_CELL)
        numberOfCells = supportOnCells.getNumberOfElements(MED_ALL_ELEMENTS)

        fieldIntScalarOnCells = FIELDINT(supportOnCells,1)
        fieldIntScalarOnCells.setName("fieldScalarIntCell")
        fieldIntScalarOnCells.setIterationNumber(-1)
        fieldIntScalarOnCells.setOrderNumber(-1)
        fieldIntScalarOnCells.setTime(0.0)

        fieldIntScalarOnCells.setComponentName(1,"Vx")
        fieldIntScalarOnCells.setComponentDescription(1,"comp1")
        fieldIntScalarOnCells.setMEDComponentUnit(1,"unit1")

        cellValues = arange(numberOfCells)
        fieldIntScalarOnCells.setValue( cellValues )

        resValue = fieldIntScalarOnCells.getValue()
        self.assertTrue( isinstance( resValue, ndarray ))
        self.assertTrue( len(resValue) == numberOfCells)

        dblArray = arange( numberOfNodes, dtype=float )
        self.assertRaises( TypeError, fieldIntScalarOnCells.setValue, dblArray )

        fieldIntScalarOnCells.write(MED_DRIVER,filename)

        os.remove( filename )

    def setUp(self):
        pass

unittest.main()
