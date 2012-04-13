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

###################################################################################
# This Python script is to test the API of the C++ GAUSS_LOCALIZATION class
# defined in MEDMEM_GaussLocalization.hxx. This test is the Python equivalent
# of the test_GaussLocalization.cxx program.
###################################################################################
#
from libMEDMEM_Swig import *

a = 0.446948490915965
b = 0.091576213509771
p1 = 0.11169079483905
p2 = 0.0549758718227661

typeGeo = MED_TRIA6

ngauss = 6

gaussLocName = "Model n1"

refCoo = [-1.0,1.0,
          -1.0,-1.0,
          1.0,-1.0,
          -1.0,0.0,
          0.0,-1.0,
          0.0,0.0]

gsCoo = [2*b-1, 1-4*b,
         2*b-1, 2*b-1,
         1-4*b, 2*b-1,
         1-4*a, 2*a-1,
         2*a-1, 1-4*a,
         2*a-1, 2*a-1]

weight = [4*p2, 4*p2, 4*p2, 4*p1, 4*p1, 4*p1]

interlacingType = MED_FULL_INTERLACE

gaussLoc = GAUSS_LOCALIZATION_FULL(gaussLocName, typeGeo, ngauss, refCoo,
                                   gsCoo, weight)

print "Gauss Localization via the str operator: ", gaussLoc

print "Gauss Localization via the get methods from the class API"

typeGeoVerif = gaussLoc.getType()

ngaussVerif = gaussLoc.getNbGauss()

gaussLocNameVerif = gaussLoc.getName()

refCooVerif = gaussLoc.getRefCoo()

gsCooVerif = gaussLoc.getGsCoo()

weightVerif = gaussLoc.getWeight()

interlacingTypeVerif = gaussLoc.getInterlacingType()

lenrefCoo = len(refCoo)
lenrefCooVerif = len(refCooVerif)

lengsCoo = len(gsCoo)
lengsCooVerif = len(gsCooVerif)

lenweight = len(weight)
lenweightVerif = len(weightVerif)

if (typeGeo != typeGeoVerif):
    print "the geometric type does not match"
    print typeGeo, " verif ", typeGeo
    pass

if (ngauss != ngaussVerif):
    print "the number of Gauss points does not match"
    print ngauss, " verif ", ngaussVerif
    pass

if (interlacingType != interlacingTypeVerif):
    print "the interlacing type does not match"
    print interlacingType, " verif ", interlacingTypeVerif
    pass

if (gaussLocName != gaussLocNameVerif):
    print "the Gauss Location name does not match"
    print gaussLocName, " verif ", gaussLocNameVerif
    pass

if (lenrefCoo == lenrefCooVerif):
    print "refCoo verification -- length ",lenrefCoo
    for i in range(lenrefCoo):
        if (refCoo[i] != refCooVerif[i]):
            print " * ",i," ",refCoo[i], " --- ",refCooVerif[i]
else:
    print "the array of referrence element coordinates does not match"

if (lengsCoo == lengsCooVerif):
    print "gsCoo verification -- length ",lengsCoo
    for i in range(lengsCoo):
        if (gsCoo[i] != gsCooVerif[i]):
            print " * ",i," ",gsCoo[i], " --- ",gsCooVerif[i]
else:
    print "the array of gauss points coordinates does not match"

if (lenweight == lenweightVerif):
    print "weight verification -- length ",lenweight
    for i in range(lenweight):
        if (weight[i] != weightVerif[i]):
            print " * ",i," ",weight[i], " --- ",weightVerif[i]
else:
    print "the array of gauss points weight does not match"

print "seems to be OK"
print ""
print "END of the Pyhton script ..... Ctrl D to exit"
