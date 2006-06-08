import salome
import SALOME
import os

from libMEDClient import *

filePath=os.environ["MED_ROOT_DIR"]
filePath=filePath+"/share/salome/resources/"

medFiles = []
medFiles.append("extendedtransport53_triangles.med")
medFiles.append("maillage_5_5_5.med")
medFiles.append("maillage_chemvalIV_cas1_40elts.med")

meshNames = []
meshNames.append("TestA3_2094_0.1_rsurf_tri")
meshNames.append("maillage_5_5_5")
meshNames.append("maillage_chemvalIV_cas1_40elts")

nbOfFiles = len(medFiles)

med=salome.lcc.FindOrLoadComponent("FactoryServer", "MED")

for i in range(nbOfFiles):
  medFile = filePath + medFiles[i]
  meshName = meshNames[i]


  try:
    meshCorba = med.readMeshInFile(medFile, salome.myStudyName,meshName)
  except SALOME.SALOME_Exception, ex:
    print ex.details
    print ex.details.type
    print ex.details.text
    print ex.details.sourceFile
    print ex.details.lineNumber

    raise

  print "meshName = ",meshCorba.getName()
  print "mesh number of nodes", meshCorba.getNumberOfNodes()






  meshLocalCopy = MESHClient(meshCorba)
  print "      getting information from the local copy of the distant mesh"
  name = meshLocalCopy.getName()
  spaceDimension = meshLocalCopy.getSpaceDimension()
  meshDimension = meshLocalCopy.getMeshDimension()
  numberOfNodes = meshLocalCopy.getNumberOfNodes()
  print "          Name = ", name, " space Dim = ", spaceDimension, " mesh Dim = ", meshDimension, " Nb of Nodes = ", numberOfNodes
  coordSyst = meshLocalCopy.getCoordinatesSystem()
  print "          The coordinates system is",coordSyst
  print "          The Coordinates :"
  coordNames = []
  coordUnits = []
  for isd in range(spaceDimension):
      coordNames.append(meshLocalCopy.getCoordinateName(isd))
      coordUnits.append(meshLocalCopy.getCoordinateUnit(isd))

  print "          names:", coordNames
  print "          units", coordUnits
  print "          values:"
  coordinates = meshLocalCopy.getCoordinates(MED_FULL_INTERLACE)
  for k in range(numberOfNodes):
      kp1 = k+1
      print "         ---- ", coordinates[k*spaceDimension:(kp1*spaceDimension)]
  print ""
  print "          The Cell Nodal Connectivity of the Cells:"
  nbTypesCell = meshLocalCopy.getNumberOfTypes(MED_CELL)
  print ""
  if (nbTypesCell>0):
      print "      The Mesh has",nbTypesCell,"Type(s) of Cell"
      types = meshLocalCopy.getTypes(MED_CELL)
      for k in range(nbTypesCell):
          type = types[k]
          nbElemType = meshLocalCopy.getNumberOfElements(MED_CELL,type)
          print "     For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
          connectivity = meshLocalCopy.getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,type)
          nbNodesPerCell = type%100
          for j in range(nbElemType):
              print "       Element",(j+1)," ",connectivity[j*nbNodesPerCell:(j+1)*nbNodesPerCell]
              pass
          pass
      pass

  ##
  ## TEST METHODS ABOUT POLY ELEMENTS ##
  ##
  nbTypesCellWithPoly = meshLocalCopy.getNumberOfTypesWithPoly(MED_CELL)
  if (nbTypesCell == nbTypesCellWithPoly):
      print ""
      print "          No Poly Cells in the mesh"
      print ""
      pass
  else:
      print ""
      print "          The Cell Nodal Connectivity of the Poly Cells:"
      print ""
      print "      The Mesh has",nbTypesCellWithPoly-nbTypesCell,"Type(s) of Poly Cell"
      types = meshLocalCopy.getTypesWithPoly(MED_CELL)
      for k in range(nbTypesCellWithPoly):
          type = types[k]
          if type == MED_POLYGON:
              nbElemType = meshLocalCopy.getNumberOfPolygons()
          elif type == MED_POLYHEDRA:
              nbElemType = meshLocalCopy.getNumberOfPolyhedron()
          else:
              continue
          print ""
          print "     For the type:",type,"there is(are)",nbElemType,"elemnt(s)"
          if type == MED_POLYGON:
              connectivity = meshLocalCopy.getPolygonsConnectivity(MED_NODAL,MED_CELL)
              index = meshLocalCopy.getPolygonsConnectivityIndex(MED_NODAL,MED_CELL)
              for j in range(nbElemType):
                  print "       Polygon",(j+1)," ",connectivity[ index[j]-1 : index[j+1]-1 ]
                  pass
              pass
          else:
              connectivity = meshLocalCopy.getPolyhedronConnectivity(MED_NODAL)
              fIndex = meshLocalCopy.getPolyhedronFacesIndex()
              index = meshLocalCopy.getPolyhedronIndex(MED_NODAL)
              for j in range(nbElemType):
                  print     "       Polyhedra",(j+1)
                  iF1, iF2 = index[ j ]-1, index[ j+1 ]-1
                  for f in range( iF2 - iF1 ):
                      iN1, iN2 = fIndex[ iF1+f ]-1, fIndex[ iF1+f+1 ]-1
                      print "         Face",f+1," ",connectivity[ iN1 : iN2 ]
                      pass
                  pass
              pass
          pass
      pass
  pass

print "END of the Pyhton script ..... Ctrl D to exit"
