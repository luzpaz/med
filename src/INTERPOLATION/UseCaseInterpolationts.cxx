#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Interpolation.hxx"

#include <deque>

#include "stdio.h"

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
