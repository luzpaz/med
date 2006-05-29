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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Interpolation.hxx"

#include <deque>

#include "stdio.h"

using namespace MEDMEM;
using namespace MED_EN;

main () {
  const char * fromFileName   = "ResultatSyrthes.med";
  const char * toFileName     = "MaillageAster.med";
  const char * resultFileName = "ResultatInterpolation.med";

  const char * fromFieldName  = "THERDEP_TEMP____________________";

  const char * fromMeshName   = "MA";
  const char * toMeshName     = "MAILLAGE_IDEAS";
  int handle;

  try {
    
    
    MED fromMED(MED_DRIVER,fromFileName); 

    fromMED.updateSupport();

    MESH toMesh(MED_DRIVER,toFileName,toMeshName); 
    
    deque<DT_IT_> pasDeTemps=fromMED.getFieldIteration (fromFieldName); 
    
    deque<DT_IT_>::const_iterator currentStep;
    
    INTERPOLATION<3>  * myInter ;
    FIELD<double>   * toField   ;
    int flagNewMapping = 1;

    for (currentStep=pasDeTemps.begin();currentStep!=pasDeTemps.end();currentStep++)
    	{
    	FIELD_ * fromField_ = fromMED.getField(fromFieldName,(*currentStep).dt,(*currentStep).it) ; 

    	FIELD<double> * fromField = dynamic_cast<FIELD<double> *>(fromField_); 

    	fromField->getSupport()->getMesh()->read();
        
    	fromField->read(); 
	
	if (currentStep==pasDeTemps.begin())
		{
    		myInter = new INTERPOLATION<3>(*fromField,toMesh) ; 

    		toField = myInter->interpolate(1,1); 
		}
	else
		{
    		toField = myInter->interpolateNextStep(*fromField,flagNewMapping); 
		}
    
    	toField->addDriver(MED_DRIVER,resultFileName,toField->getName()); 
    
    	toField->write(); 
    
    	if (flagNewMapping==1)
		{
    		handle = toMesh.addDriver(MED_DRIVER,resultFileName,toMesh.getName()) ; 
    
    		toMesh.write(handle); 
		}
	}

  } catch (MEDEXCEPTION& ex){
    MESSAGE(ex.what()) ;
  }
}
