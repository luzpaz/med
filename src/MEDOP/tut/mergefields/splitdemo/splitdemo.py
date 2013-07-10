from MEDLoader import *

fns=['DomainMesh_1.med', 'DomainMesh_2.med','DomainMesh_3.med', 'DomainMesh_4.med']
outs=["out1.vtu","out2.vtu","out3.vtu","out4.vtu"]
trs=[(-0.714,-0.714,0.),(0.,0.,0.),(-0.577,-0.577,-0.577),(-0.577,0.3,-0.577)]
scal=0.5
fs=4*[None]
dds=4*[None]
mns=4*[None]
i=0

splitview=True

for fn in fns:
    m=MEDFileMesh.New(fn)
    m=m.getMeshAtLevel(0)
    m.writeVTK(outs[i])
    mns[i]=m.deepCpy()
    if splitview:
        m.translate([scal*elt for elt in trs[i]])
    f=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME)
    f.setMesh(m)
    da=DataArrayDouble.New()
    da.alloc(m.getNumberOfCells(),1)
    da.fillWithValue(i)
    dds[i]=da.deepCpy()
    f.setArray(da)
    fs[i]=f
    i+=1
    pass


f=MEDCouplingFieldDouble.MergeFields(fs)
f.setName("PID")

MEDCouplingFieldDouble.WriteVTK("Domain.vtu",[f])
dd=DataArrayDouble.Aggregate(dds)
m=MEDCouplingUMesh.MergeUMeshes(mns)
if splitview:
    m.translate([4.*elt for elt in [0.577,0.577,0.577]])
f2=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME)
f2.setMesh(m)
f2.setArray(dd)
f2.setName("PID")
MEDCouplingFieldDouble.WriteVTK('DomainMesh.vtu',[f2])
#m=MEDCouplingUMesh.MergeUMeshes(mns)
#m.writeVTK("DomainMesh.vtu")
