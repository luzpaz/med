#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Interpolation.hxx"

#include "stdio.h"

using namespace MEDMEM;
using namespace MED_EN;

main () {
  
  const char * fromFileName  = "fromMesh.med";
  const char * toFileName    = "toMesh.med";
  const char * fieldName     = "fieldnodedouble";

  const char * fromMeshName  = "fromMesh";
  const char * toMeshName    = "toMesh";
  
  const int flagConvex = 1;
  const int interpolationType = 1;

  try {
    
    MESH	    fromMesh	(MED_DRIVER,fromFileName,fromMeshName); 	 
    SUPPORT	    fromSupport (&fromMesh,"XsupportX",MED_NODE);		 
    FIELD<double>   fromField	(&fromSupport,MED_DRIVER,fromFileName,fieldName);
    MESH	    toMesh	(MED_DRIVER,toFileName,toMeshName);		 
    
    INTERPOLATION<3> myInter (fromField,toMesh);
    
    FIELD<double>   * toField = myInter.interpolate(interpolationType,flagConvex);
    
    toField->addDriver(MED_DRIVER,toFileName,toField->getName()) ;

    toField->write();


  } catch (MEDEXCEPTION& ex){
    MESSAGE(ex.what()) ;
  }
}
