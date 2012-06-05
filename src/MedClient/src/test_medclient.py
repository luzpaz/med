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

import salome
import SALOME_TESTMEDCLIENT
import SALOME_MED

def getMedObjectFromStudy():
    mySO = salome.myStudy.FindObject("Objet MED")
    Builder = salome.myStudy.NewBuilder()
    anAttr = Builder.FindOrCreateAttribute(mySO, "AttributeIOR")
    obj = salome.orb.string_to_object(anAttr.Value())
    myObj = obj._narrow(SALOME_MED.MED)
    return myObj

#Truc1,Truc2 are Containers launched with SALOME_Container exe.

med_comp = salome.lcc.FindOrLoadComponent("Truc1", "MED")
my_comp = salome.lcc.FindOrLoadComponent("Truc2","TESTMEDCLIENT")
studyCurrent = salome.myStudyName

## First test

##med_obj = med_comp.readStructFile("cube_tetra4_12.med",studyCurrent)
##my_comp.go2(med_obj)

## Second test

med_obj = med_comp.readStructFile("cube_hexa8_quad4.med",studyCurrent)
my_comp.go(med_obj)
