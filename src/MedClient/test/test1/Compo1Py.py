import sys
from omniORB import CORBA, PortableServer
import CosNaming
import Engines, Engines__POA
from SALOME_ComponentPy import *
from libMedCorba_Swig import *

import Compo1

class Compo1Py( Engines__POA.Compo1Py, SALOME_ComponentPy_i):

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
        
