# _T1A
from MEDLoader import MEDLoader, ON_CELLS
from MEDCouplingRemapper import MEDCouplingRemapper, ConservativeVolumic

# Read the source mesh and the source field
it,dt = (-1,-1)
msource = MEDLoader.ReadUMeshFromFile("fieldsource.med","meshsource",0)
fsource = MEDLoader.ReadField(ON_CELLS,"fieldsource.med","meshsource",0,
                              "Temperature",it,dt)
fsource.setMesh(msource)
fsource.setNature(ConservativeVolumic)

# Read the target mesh
mtarget = MEDLoader.ReadUMeshFromFile("meshtarget.med","meshtarget",0)

# Remapper of type P0P0 (interpolation from cells to cells)
remap = MEDCouplingRemapper()
remap.prepare(msource,mtarget,"P0P0")

defaultValue = 1e100
ftarget = remap.transferField(fsource,defaultValue)
ftarget.setName("Temperature")

outfilename = "loadsource_fieldtarget.med"
MEDLoader.WriteField(outfilename,ftarget,True)
# _T1B
