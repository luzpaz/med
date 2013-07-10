#!/usr/bin/env python
#  -*- coding: iso-8859-1 -*-

# This script illustrates the basic usage of MEDCoupling and MEDLoader
# to read fields in a med file and make basics operation on the values
# (gboulant - 11/07/2011)


# _T1A
medfilename = "timeseries.med" # med source filename
meshName = "Grid_80x80"        # name of the support mesh
dimrestriction = 0             # 0=no restriction
fieldName = "Pulse"            # name of the field series
# _T1B

# ==============================================================
# Make a scaling. This does not require the loading of the mesh
# _T2A
from MEDLoader import MEDLoader, ON_NODES

iteration, order = (3,-1)      # timestamps to consider
field=MEDLoader.ReadField(ON_NODES,
                          medfilename, meshName, dimrestriction,
                          fieldName, iteration, order)

field.applyFunc("f*3")
outfilename = "scaling.med"
MEDLoader.WriteField(outfilename,field,True)
# _T2B


# ==============================================================
# Make an addition. This requires to load first the mesh

# _T3A
# Load the support mesh
mesh = MEDLoader.ReadUMeshFromFile(medfilename, meshName, dimrestriction)

# Load the field at timestamps 3
iteration, order = (3,-1)
p3=MEDLoader.ReadField(ON_NODES,
                       medfilename,meshName,dimrestriction,
                       fieldName,iteration,order)
# Associate the mesh
p3.setMesh(mesh)

# Load the field at timestamps 4 
iteration, order = (4,-1)
p4=MEDLoader.ReadField(ON_NODES,
                       medfilename, meshName, dimrestriction,
                       fieldName, iteration, order)
# Associate the mesh
p4.setMesh(mesh)

# Make the addition
result = p3+p4
result.setName("p3+p4")

# We can finally save the result together with the operandes fields
outfilename = "addition.med"
MEDLoader.WriteField(outfilename,result,True)
MEDLoader.WriteField(outfilename,p3,False)
MEDLoader.WriteField(outfilename,p4,False)
# _T3B

# ==============================================================
# Make an addition. A different way that loads fields all at once
mesh = MEDLoader.ReadUMeshFromFile(medfilename, meshName, dimrestriction)
timestamps = [(3,-1),(4,-1)]
p3,p4 = MEDLoader.ReadFieldsOnSameMesh(ON_NODES,medfilename,
                                       meshName,dimrestriction,
                                       fieldName,timestamps)
p3.setMesh(mesh)
p4.setMesh(mesh)
result = p3+p4
result.setName("p3+p4")

outfilename = "addition_01.med"
MEDLoader.WriteField(outfilename,result,True)
