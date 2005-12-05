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

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Interpolation.hxx"

#include <deque>

#include "stdio.h"

using namespace MEDMEM;

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

double ABS(Valeur<double> v)
	{
	double tmp=0;
	int i;
	for (i=0;i<v.SIZE();i++) tmp+=fabs(v[i]);
	return tmp;
	}

double ABS(Valeur<double> v1,Valeur<double> v2)
	{
	double tmp=0;
	int i;
	for (i=0;i<v1.SIZE();i++) tmp+=fabs(v1[i]-v2[i]);
	return tmp;
	}



// effectue la soustraction du premier moins le second et stocke le résultat dans le premier
void Flipback(FIELD<double> * firstField, FIELD<double> * secondField)
	{
	Wrapper_MED_Field first  ( firstField);
	Wrapper_MED_Field second (secondField);
	int nbr_valeurs_first  =  first.Get_Nbr_Valeurs();
	int nbr_valeurs_second = second.Get_Nbr_Valeurs();
	
	double max1    = 0;
	double max2    = 0;
	
	double min1    = ABS(first[0]);
	double min2    = ABS(second[0]);
	
	int imax1,imax2;
	
	double tmp;
	
	int i;
	
	//cout<<first<<endl;
	//int tyty;cin>>tyty;

	if (nbr_valeurs_first!=nbr_valeurs_second) 
		{
		cerr<<"Les champs à soustraire n'ont pas le meme nombre de valeurs"<<endl;
		exit(-1);
		}
	
	imax1=0;
	for (i=0;i<nbr_valeurs_first;i++) 
		{
		tmp=ABS(first[i]);
		//cout<<"tmp 1 ["<<i<<"] = "<<tmp<<endl;
		if (tmp>max1) 
			{
			imax1=i;
			max1=tmp;
			}
		if (tmp<min1) min1=tmp;
		}

	imax2=0;
	for (i=0;i<nbr_valeurs_first;i++) 
		{
		tmp=ABS(second[i]);
		if (tmp>max2) 
			{
			imax2=i;
			max2=tmp;
			}
		if (tmp<min2) min2=tmp;
		}
		
	for (i=0;i<nbr_valeurs_first;i++) 
		{
		first[i]=ABS(first[i],second[i]);
		}
	
	double maxdiff=ABS(first[0]);
	double mindiff=ABS(first[0]);
		
	for (i=0;i<nbr_valeurs_first;i++) 
		{
		tmp=ABS(first[i]);
		if (tmp>maxdiff) maxdiff=tmp;
		if (tmp<mindiff) mindiff=tmp;
		}
	
	cout<<endl;	
	cout<<"/////////////////////////////////////////////////////////////////////////"<<endl;
	cout<<"/////// max1    = "<<max1<<endl;
	cout<<"/////// min1    = "<<min1<<endl;
	cout<<"/////// Maximum First  atteint pour i = "<<imax1<<endl;
	cout<<"/////// max2    = "<<max2<<endl;
	cout<<"/////// min2    = "<<min2<<endl;	
	cout<<"/////// Maximum Second atteint pour i = "<<imax2<<endl;
	cout<<"/////// maxdiff = "<<maxdiff<<endl;
	cout<<"/////// mindiff = "<<mindiff<<endl;	
	cout<<"/////////////////////////////////////////////////////////////////////////"<<endl;
		
	}


#include "MEDMEM_WrapperCells.hxx"

main () {
  const char * fromFileName   = "ResultatSyrthes.med";
  const char * toFileName     = "MaillageAster.med";
  const char * resultFileName = "ResultatFlipback.med";

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
    
    INTERPOLATION<3>  * interFromTo ;
    INTERPOLATION<3>  * interToFrom ;
    FIELD<double>   * toField   ;
    FIELD<double>   * toToField ;
    int flagNewMappingFromTo = 0;
    int flagNewMappingToFrom = 0;

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
		
	MESH * fromMesh = fromField->getSupport()->getMesh();		
	        
    	cout<<flag<<"Lecture des valeurs du FIELD              : "<<flush; 
	RUN(fromField->read()); 
    	cout<<"OK !"<<endl;   
	
	if (currentStep==pasDeTemps.begin())
		{
    		cout<<flag<<"Préparation de l'interpolation DIRECTE pour le premier pas de temps  : "<<flush;
    		RUN(interFromTo = new INTERPOLATION<3>(*fromField,toMesh));
		cout<<"OK !"<<endl;
    		cout<<flag<<"Interpolation effective DIRECTE du premier pas de temps              : "<<flush;
    		RUN(toField = interFromTo->interpolate(1,1));
    		cout<<"OK !"<<endl;    
    		cout<<flag<<"Préparation de l'interpolation INVERSE pour le premier pas de temps  : "<<flush;
    		RUN(interToFrom = new INTERPOLATION<3>(*toField,*fromMesh));
		cout<<"OK !"<<endl;
    		cout<<flag<<"Interpolation effective INVERSE du premier pas de temps              : "<<flush;
    		RUN(toToField = interToFrom->interpolate(1,1));
    		cout<<"OK !"<<endl;    
		}
	else
		{
    		cout<<flag<<"Interpolation nextStep DIRECTE             : "<<flush;
    		RUN(toField = interFromTo->interpolateNextStep(*fromField,flagNewMappingFromTo));
    		cout<<"OK !"<<endl;    
    		cout<<flag<<"Interpolation nextStep INVERSE             : "<<flush;
    		RUN(toToField = interToFrom->interpolateNextStep(*toField,flagNewMappingToFrom));
    		cout<<"OK !"<<endl;    
		}
    
	cout<<flag<<"Calcul du flip back : "<<flush;
	Flipback(toToField,fromField);
	cout<<"OK !"<<endl;
    
    	cout<<flag<<"Creation du driver d'écriture Field       : "<<flush;
    	toToField->addDriver(MED_DRIVER,resultFileName,toToField->getName()); 
    	cout<<"OK !"<<endl;
	    
    	cout<<flag<<"Ecriture du Field résultat                : "<<flush;
    	toToField->write(); 
    	cout<<"OK !"<<endl;
    
    	if (flagNewMappingToFrom==1)
		{
    		cout<<flag<<"Creation du driver d'écriture Mesh        : "<<flush; 
    		handle = fromMesh->addDriver(MED_DRIVER,resultFileName,fromMesh->getName()) ; 
    		cout<<"OK !"<<endl;
    
    		cout<<flag<<"Ecriture du Mesh résultat                 : "<<flush; 
    		fromMesh->write(handle); 
    		cout<<"OK !"<<endl;
		}
	}

  } catch (MEDEXCEPTION& ex){
    MESSAGE(ex.what()) ;
  }
}
