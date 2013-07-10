from MEDLoader import *

it,dt = (-1,-1)
mesh = MEDLoader.ReadUMeshFromFile("field1.med","DomainMesh_1",0)
f1 = MEDLoader.ReadField(ON_CELLS,"field1.med","DomainMesh_1",0,"field1",it,dt)
f1.setMesh(mesh)

mesh = MEDLoader.ReadUMeshFromFile("field2.med","DomainMesh_2",0)
f2 = MEDLoader.ReadField(ON_CELLS,"field2.med","DomainMesh_2",0,"field2",it,dt)
f2.setMesh(mesh)

mesh = MEDLoader.ReadUMeshFromFile("field3.med","DomainMesh_3",0)
f3 = MEDLoader.ReadField(ON_CELLS,"field3.med","DomainMesh_3",0,"field3",it,dt)
f3.setMesh(mesh)

#mesh = MEDLoader.ReadUMeshFromFile("field4.med","DomainMesh_4",0)
#f4 = MEDLoader.ReadField(ON_CELLS,"field4.med","DomainMesh_4",0,"field4",it,dt)
#f4.setMesh(mesh)

fmerge=MEDCouplingFieldDouble.MergeFields([f1,f2,f3])
fmerge.setName("merge")

MEDLoader.WriteField("merge.med",fmerge,True)

