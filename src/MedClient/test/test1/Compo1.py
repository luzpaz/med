# Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
#           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either 
# version 2.1 of the License.
# 
# This library is distributed in the hope that it will be useful 
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
from libMEDMEM_Swig import *


class Compo1:

    def __init__(self):
        print "Initialisation Compo1"
        self.mesh = None
        self.support = None
        self.md = None
        self.medFile = None
        
    def __del__(self):
        print "destruction Compo1"

    def Initialise(self, medFile):
        if ((self.medFile != medFile) | (self.md is None)):
            print "fichier MED monte"
            self.md = MED()
            mdDriver = MED_MED_RDONLY_DRIVER(medFile, self.md)
            mdDriver.open()
            mdDriver.readFileStruct()
            mdDriver.close()
            self.mesh = None
            self.support = None
            self.medFile = medFile
        else:
            print "fichier MED deja monte"
            
    def Test(self):
        if (self.md is None):
            raise RuntimeError, 'call Initialise first'

    def Calcul1(self):

        self.Test();
        if (self.mesh is None):
            self.mesh = self.md.getMesh(self.md.getMeshName(0))
            self.mesh.read()
            
        return self.mesh

    def Calcul2(self):

        self.Test();
        if (self.mesh is None):
            self.Calcul1()
            
        if (self.support is None):
            self.support = SUPPORT(self.mesh, "my_support", MED_CELL)
            self.support.setAll(1)
            
        return self.support
