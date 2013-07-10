from MEDLoader import *
from MEDCouplingRemapper import *

ms=MEDFileMesh.New("MeshSource.med") ; ms=ms.getMeshAtLevel(0)
mt=MEDFileMesh.New("MeshTarget.med") ; mt=mt.getMeshAtLevel(0)
fs=ms.fillFromAnalytic(ON_CELLS,1,"319.*cos(((x)*(x)*3+(y-0.52)*(y-0.52)+(z-0.1)*(z-0.1))*7)")
fs.setNature(ConservativeVolumic)
fs.setName("Temperature")
MEDCouplingFieldDouble.WriteVTK("MeshSource.vtu",[fs])

mt2=mt.deepCpy()
mt2.translate([0.4,0.,0.])
mt2.writeVTK("MeshTarget.vtu")
#
remap=MEDCouplingRemapper()
remap.prepare(ms,mt,"P0P0")
ft=remap.transferField(fs,1e100)
ft.setName("Temperature")
ft.getMesh().translate([0.4,0.,0.])
MEDCouplingFieldDouble.WriteVTK("FieldTarget.vtu",[ft])


# Image illustrating the starting point:
# - load MeshSource.vtu (contains mesh+field)
# - load MeshTarget.vtu (contains only the mesh with translation)
#
# Image illustrating the result point:
# - load MeshSource.vtu
# - load FieldTarget.vtu
 
