#  -*- coding: iso-8859-1 -*-

from MEDSPLITTER import *
import os

#Beginning of the computation

#Loading the mesh collection
print "reading files"

data_dir = os.getenv("MED_ROOT_DIR")
filename_rd = os.path.join( data_dir, "share/salome/resources/med/pointe_import22.med")
meshname="maa1"

collection = MESHCollection(filename_rd,meshname)

print "MEDSPLITTER - computing partition "
  
 
new_topo = collection.createPartition(4,METIS);

# Creating a new mesh collection from the partitioning
new_collection = MESHCollection(collection, new_topo, 1, 1);


new_collection.setSubdomainBoundaryCreates(1);

print "MEDSPLITTER - writing output files "
new_collection.write("/tmp/myOutput.med")

# Casting the fields on the new collection
new_collection.castAllFields(collection);

