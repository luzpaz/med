// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
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
