"""

"""

# force GEOM importation at interpretor initialization
# see salome_shared_modules.py
# (avoids incomplete import at run time)

print "============== import MED_idl ======================="
print "============== import Med_Gen_idl ======================="

import MED_idl
import Med_Gen_idl

# this function is required

def init_shared_modules():
   """
      This function initializes shared modules that need to be
   """
   pass