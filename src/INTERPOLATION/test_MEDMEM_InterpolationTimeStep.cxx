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
    
    string flag="================[MAIN MESSAGES]================> ";
    
    cout<<flag<<"Lecture de la structure MED               : "<<flush; 
    MED             fromMED     (MED_DRIVER,fromFileName); 
    cout<<"OK !"<<endl;
    
    // Utilisation completement débile, on ne devrait pas avoir a faire l'appel suivant
    fromMED.updateSupport();

    cout<<flag<<"Lecture du Mailllage Cible                : "<<flush; 
    MESH            toMesh      (MED_DRIVER,toFileName,toMeshName); 
    cout<<"OK !"<<endl;
    
    cout<<flag<<"Lecture des pas de temps                  : "<<flush; 
    deque<DT_IT_> pasDeTemps=fromMED.getFieldIteration (fromFieldName); 
    cout<<"OK !"<<endl;
    
    deque<DT_IT_>::const_iterator currentStep;
    
    INTERPOLATION<3>  * myInter ;
    FIELD<double>   * toField   ;
    int flagNewMapping = 1;

    for (currentStep=pasDeTemps.begin();currentStep!=pasDeTemps.end();currentStep++)
    	{
        cout<<flag<<"Traitement du Step ( "<<flush<<(*currentStep).dt<<" ; "<<(*currentStep).it<<" )  : "<<endl;
	
    	cout<<flag<<"Lecture du FIELD_ "<<flush;
    	FIELD_ * fromField_ = fromMED.getField(fromFieldName,(*currentStep).dt,(*currentStep).it) ; 
    	cout<<"OK !"<<endl;
    
    	cout<<flag<<"Transtypage en FIELD                      : "<<flush; 
    	FIELD<double> * fromField = dynamic_cast<FIELD<double> *>(fromField_); 
    	cout<<"OK !"<<endl;

    	// Utilisation completement débile, on ne devrait pas avoir a faire l'appel suivant
    	fromField->getSupport()->getMesh()->read();
        
    	cout<<flag<<"Lecture des valeurs du FIELD              : "<<flush; fromField->read(); 
    	cout<<"OK !"<<endl;   
	
	if (currentStep==pasDeTemps.begin())
		{
    		cout<<flag<<"Préparation de l'interpolation pour le premier pas de temps  : "<<flush;
    		myInter = new INTERPOLATION<3>(*fromField,toMesh) ; 
		cout<<"OK !"<<endl;	       

    		cout<<flag<<"Interpolation effective du premier pas de temps              : "<<flush;
    		toField = myInter->interpolate(1,1); 
    		cout<<"OK !"<<endl;    
		}
	else
		{
    		cout<<flag<<"Interpolation nextStep              : "<<flush;
    		toField = myInter->interpolateNextStep(*fromField,flagNewMapping); 
    		cout<<"OK !"<<endl;    
		}
    
    	cout<<flag<<"Creation du driver d'écriture Field       : "<<flush;
    	toField->addDriver(MED_DRIVER,resultFileName,toField->getName()); 
    	cout<<"OK !"<<endl;
    
    	cout<<flag<<"Ecriture du Field résultat                : "<<flush;
    	toField->write(); 
    	cout<<"OK !"<<endl;
    
    	if (flagNewMapping==1)
		{
    		cout<<flag<<"Creation du driver d'écriture Mesh        : "<<flush; 
    		handle = toMesh.addDriver(MED_DRIVER,resultFileName,toMesh.getName()) ; 
    		cout<<"OK !"<<endl;
    
    		cout<<flag<<"Ecriture du Mesh résultat                 : "<<flush; 
    		toMesh.write(handle); 
    		cout<<"OK !"<<endl;
		}
	}

  } catch (MEDEXCEPTION& ex){
    MESSAGE(ex.what()) ;
  }
}
