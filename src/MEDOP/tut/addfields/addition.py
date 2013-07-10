from MEDLoader import MEDLoader, ON_NODES

medfilename = "timeseries.med" # med source filename
meshname = "Grid_80x80"        # name of the support mesh
dimrestriction = 0             # 0=no restriction
fieldname = "Pulse"            # name of the field series

# Load the support mesh
mesh = MEDLoader.ReadUMeshFromFile(medfilename,meshname,dimrestriction)

# Load the field at timestamps 3
iteration, order = (3,-1)      # timestamps to consider
p3=MEDLoader.ReadField(ON_NODES,medfilename,meshname,dimrestriction,
                       fieldname,iteration,order)
p3.setMesh(mesh)

# Load the field at timestamps 4 
iteration, order = (4,-1)      # timestamps to consider
p4=MEDLoader.ReadField(ON_NODES,medfilename,meshname,dimrestriction,
                       fieldname,iteration,order)
p4.setMesh(mesh)

result = p3+p4
result.setName("p3+p4")

outfilename = "addition.med"
MEDLoader.WriteField(outfilename,result,True)
MEDLoader.WriteField(outfilename,p3,False)
MEDLoader.WriteField(outfilename,p4,False)

