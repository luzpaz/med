#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Interpolation.hxx"

#include <deque>

#include "stdio.h"

#ifdef _DEBUG_
#include "LocalTraceCollector.hxx"
#endif /* ifdef _DEBUG_*/

using namespace MEDMEM;
using namespace MED_EN;

// pour gestion timings
#include "time.h"

#define RUN(procedure) {double t0,t1;cout<<"# =============> TEMPS D'EXECUTION A PARTIR D'ICI "<<endl<<#procedure<<endl;t0=CPUtime();procedure;t1=CPUtime();cout<<"# ================> TEMPS D'EXECUTION : "<<t1-t0<<endl;}
#define TIMORIZE(procedure,t) {double t0,t1;t0=CPUtime();procedure;t1=CPUtime();t=t1-t0;}

double CPUtime()
	{
	#ifdef SYSTIMES
	struct tms buf;
  	if (times(&buf)!=-1)
    		return ((double)buf.tms_utime+(double)buf.tms_stime)/(long) sysconf(_SC_CLK_TCK);
 	else
	#endif
    	return ((double) clock())/CLOCKS_PER_SEC;
	}



#include "MEDMEM_WrapperCells.hxx"

main () {
#ifdef _DEBUG_
  LocalTraceCollector::instance();
#endif /* ifdef _DEBUG_*/

/*
int taille=100;
int * liste = new int [taille] ;
int i,j;
int nbr_faces;
for (i=0;i<taille;i++) liste[i]=i;

Wrapper_Maille_Tria3   Tria3  ;Tria3  .positionne(liste);cout<<"Tria3  "<<endl;Tria3  .export_connectivite();
Wrapper_Maille_Tria6   Tria6  ;Tria6  .positionne(liste);cout<<"Tria6  "<<endl;Tria6  .export_connectivite();
Wrapper_Maille_Quad4   Quad4  ;Quad4  .positionne(liste);cout<<"Quad4  "<<endl;Quad4  .export_connectivite();
Wrapper_Maille_Quad8   Quad8  ;Quad8  .positionne(liste);cout<<"Quad8  "<<endl;Quad8  .export_connectivite();
Wrapper_Maille_Tetra4  Tetra4 ;Tetra4 .positionne(liste);cout<<"Tetra4 "<<endl;Tetra4 .export_connectivite();
Wrapper_Maille_Tetra10 Tetra10;Tetra10.positionne(liste);cout<<"Tetra10"<<endl;Tetra10.export_connectivite();
Wrapper_Maille_Hexa8   Hexa8  ;Hexa8  .positionne(liste);cout<<"Hexa8  "<<endl;Hexa8  .export_connectivite();
Wrapper_Maille_Hexa20  Hexa20 ;Hexa20 .positionne(liste);cout<<"Hexa20 "<<endl;Hexa20 .export_connectivite();
Wrapper_Maille_Pyra5   Pyra5  ;Pyra5  .positionne(liste);cout<<"Pyra5  "<<endl;Pyra5  .export_connectivite();
Wrapper_Maille_Pyra13  Pyra13 ;Pyra13 .positionne(liste);cout<<"Pyra13 "<<endl;Pyra13 .export_connectivite();
Wrapper_Maille_Penta6  Penta6 ;Penta6 .positionne(liste);cout<<"Penta6 "<<endl;Penta6 .export_connectivite();
Wrapper_Maille_Penta15 Penta15;Penta15.positionne(liste);cout<<"Penta15"<<endl;Penta15.export_connectivite();
//*/

//*    
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
    	FIELD_ * fromField_ = fromMED.getField(fromFieldName,(*currentStep).dt,(*currentStep).it);
    	cout<<"OK !"<<endl;
    
    	cout<<flag<<"Transtypage en FIELD                      : "<<flush; 
    	FIELD<double> * fromField = dynamic_cast<FIELD<double> *>(fromField_);
    	cout<<"OK !"<<endl;
	
	if (currentStep==pasDeTemps.begin())
		{
    		//Utilisation completement débile, on ne devrait pas avoir a faire l'appel suivant
    		RUN(fromField->getSupport()->getMesh()->read());
		}
	        
    	cout<<flag<<"Lecture des valeurs du FIELD              : "<<flush; 
	RUN(fromField->read()); 
    	cout<<"OK !"<<endl;   
	
	if (currentStep==pasDeTemps.begin())
		{
    		cout<<flag<<"Préparation de l'interpolation pour le premier pas de temps  : "<<flush;
    		RUN(myInter = new INTERPOLATION<3>(*fromField,toMesh));
		cout<<"OK !"<<endl;	       
    		cout<<flag<<"Interpolation effective du premier pas de temps              : "<<flush;
    		RUN(toField = myInter->interpolate(1,1));
    		cout<<"OK !"<<endl;    
		}
	else
		{
    		cout<<flag<<"Interpolation nextStep              : "<<flush;
    		RUN(toField = myInter->interpolateNextStep(*fromField,flagNewMapping));
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
//*/
}
