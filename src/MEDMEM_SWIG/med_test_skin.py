#% Test function MESH::getSkin() on mesh from file cube_hexa8_quad4.med
#% The med file can be obtained by running create_mesh_c3h8q4 executable

from libMEDMEM_Swig import *

medFile = "cube_hexa8_quad4.med"

md = MED()

mdDriver = MED_MED_RDONLY_DRIVER(medFile,md)

print ""
print "Read file", medFile
print ""

mdDriver.open()
mdDriver.readFileStruct()
mdDriver.close()

mesh_name = md.getMeshName(0)
mesh = md.getMesh(mesh_name)
mesh.read()

print "Building the support on all (8) Cells of the mesh."
supportCell = SUPPORT(mesh)
supportCell.update()

print "Getting skin of an all cell support"

supportSkin = mesh.getSkin(supportCell)
faceNumbers = supportSkin.getNumber( supportSkin.getTypes()[0] )
print "--------", faceNumbers
if faceNumbers != [2, 3, 6, 8, 10, 11, 12, 13, 16, 17, 19, 20, 22, 23, 24, 27, 28, 29, 30, 31, 32, 34, 35, 36]:
  raise  RuntimeError, "Wrong skin of an all cell support"
  
print "Build the support on 1 Cell (#8) of the mesh."
supportCell.setAll( 0 )
nbGeomTypes = 1
nbTotalEntity = 1
GeometricType = mesh.getTypes(MED_CELL)
nbEntityList = [1, 2]
EntityNbs = [8]
supportCell.setpartial("1 Cell support",nbGeomTypes,nbTotalEntity,GeometricType,nbEntityList,EntityNbs)

print "Getting skin of 1 cell support"
supportSkin = mesh.getSkin(supportCell)
faceNumbers = supportSkin.getNumber( supportSkin.getTypes()[0] )
print "Skin Support FACE numbers:"
print "--------", faceNumbers
if faceNumbers != [18, 26, 33, 34, 35, 36]:
  raise  RuntimeError, "Wrong skin of 1 cell support"



print "Building the support on 2 Cells (#1 #2) of the mesh."
nbTotalEntity = 2
nbEntityList = [1, 3]
EntityNbs = [1, 2]
supportCell.setpartial("2 Cell support",nbGeomTypes,nbTotalEntity,GeometricType,nbEntityList,EntityNbs)

print "Getting skin of 2 cell support"
supportSkin = mesh.getSkin(supportCell)
faceNumbers = supportSkin.getNumber( supportSkin.getTypes()[0] )
print "Skin Support FACE numbers:", faceNumbers
if faceNumbers != [1, 2, 3, 4, 6, 7, 8, 9, 10, 11]:
  raise  RuntimeError, "Wrong skin of 2 cells support"
