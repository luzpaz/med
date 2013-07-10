#
# Another possibility (to illustrate the creation of values) is to
# create the source field using the function fillFromAnalytic
#
from MEDLoader import *
from MEDCouplingRemapper import *

# Read the source mesh and create a field on it
msource  = MEDLoader.ReadUMeshFromFile("meshsource.med","meshsource",0)

equation = "319.*cos(((x)*(x)*3+(y-0.52)*(y-0.52)+(z-0.1)*(z-0.1))*7)"
fsource=msource.fillFromAnalytic(ON_CELLS,1,equation)
fsource.setName("Temperature")
fsource.setNature(ConservativeVolumic)

# Read the target mesh
mtarget = MEDLoader.ReadUMeshFromFile("meshtarget.med","meshtarget",0)

# Remapper of type P0P0 (interpolation from cells to cells)
remap = MEDCouplingRemapper()
remap.prepare(msource,mtarget,"P0P0")

defaultValue = 1e100
ftarget = remap.transferField(fsource,defaultValue)
ftarget.setName("Temperature")

outfilename = "createsource_fieldtarget.med"
MEDLoader.WriteField(outfilename,ftarget,True)

