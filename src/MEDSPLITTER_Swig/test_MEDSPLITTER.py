#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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

from MEDSPLITTER import *
import os

#Beginning of the computation

#Loading the mesh collection
print "reading files"

data_dir = os.getenv("MED_ROOT_DIR")
filename_rd = os.path.join( data_dir, "share/salome/resources/med/pointe.med")
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

