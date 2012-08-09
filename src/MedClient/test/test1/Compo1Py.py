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

import sys
from omniORB import CORBA, PortableServer
import CosNaming
import Compo1Py_ORB, Compo1Py_ORB__POA
from SALOME_ComponentPy import *
from libMedCorba_Swig import *

import Compo1

class Compo1Py( Compo1Py_ORB__POA.Compo1Py, SALOME_ComponentPy_i):

        def __init__(self, orb, poa, contID, \
                     containerName, instanceName, interfaceName):
            notif = 1
            SALOME_ComponentPy_i.__init__(self, orb, poa, contID, \
                                          containerName, instanceName, \
                                          interfaceName, notif)

            self.C = Compo1.Compo1()
            
            print "Compo1Py_Impl::__init__"


	def Initialise(self, medFile):
	    self.C.Initialise(medFile);
	    
        def Calcul1(self):
            m = self.C.Calcul1()
            return createCorbaMesh(m)
	    
        def Calcul2(self):
            s = self.C.Calcul2()
            return createCorbaSupport(s)
        
