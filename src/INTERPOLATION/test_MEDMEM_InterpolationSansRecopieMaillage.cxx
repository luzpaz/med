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

main () {
#ifdef _DEBUG_
  LocalTraceCollector::instance();
#endif /* ifdef _DEBUG_*/

  const char * fromFileName   = "ResultatSyrthes.med";
  const char * toFileName     = "MaillageAster.med";
//  const char * resultFileName = "ResultatInterpolation.med";

  const char * fromFieldName  = "THERDEP_TEMP____________________";

  const char * fromMeshName   = "MA";
  const char * toMeshName     = "MAILLAGE_IDEAS";

  try {
    
    string flag="================[MAIN MESSAGES]================> ";
    
    cout<<flag<<"Lecture de la structure MED               : "<<flush; MED             fromMED     (MED_DRIVER,fromFileName)                                   ; cout<<"OK !"<<endl;
    
    // Utilisation completement débile, on ne devrait pas avoir a faire l'appel suivant
    fromMED.updateSupport();

    cout<<flag<<"Lecture du Maillage Source                : "<<flush; MESH            fromMesh    (MED_DRIVER,fromFileName,fromMeshName)                      ; cout<<"OK !"<<endl;
    cout<<flag<<"Lecture du Mailllage Cible                : "<<flush; MESH            toMesh      (MED_DRIVER,toFileName,toMeshName)                          ; cout<<"OK !"<<endl;
    cout<<flag<<"Lecture des pas de temps                  : "<<flush; deque<DT_IT_> pasDeTemps=fromMED.getFieldIteration (fromFieldName)                      ; cout<<"OK !"<<endl;
    cout<<flag<<"Lecture du FIELD_ au premier pas de temps : "<<flush; FIELD_ * fromField_ = fromMED.getField(fromFieldName,pasDeTemps[0].dt,pasDeTemps[0].it) ; cout<<"OK !"<<endl;
    cout<<flag<<"Transtypage en FIELD                      : "<<flush; FIELD<double> * fromField = dynamic_cast<FIELD<double> *>(fromField_)                   ; cout<<"OK !"<<endl;

    // Utilisation completement débile, on ne devrait pas avoir a faire l'appel suivant
    fromField->getSupport()->getMesh()->read();
        
    cout<<flag<<"Lecture des valeurs du FIELD              : "<<flush; fromField->read()                                                                       ; cout<<"OK !"<<endl;   
    cout<<flag<<"Préparation de l'interpolation            : "<<flush; INTERPOLATION<3> myInter (*fromField,toMesh)                                            ; cout<<"OK !"<<endl;            
    cout<<flag<<"Interpolation effective                   : "<<flush; FIELD<double>   * toField = myInter.interpolate(1,1)                                    ; cout<<"OK !"<<endl;    
    cout<<flag<<"Creation du driver d'écriture             : "<<flush; toField->addDriver(MED_DRIVER,toFileName,toField->getName())                            ; cout<<"OK !"<<endl;
//    cout<<flag<<"Creation du driver d'écriture Field       : "<<flush; toField->addDriver(MED_DRIVER,resultFileName,toField->getName())                        ; cout<<"OK !"<<endl;
    cout<<flag<<"Ecriture du Field résultat                : "<<flush; toField->write()                                                                        ; cout<<"OK !"<<endl;
//    cout<<flag<<"Creation du driver d'écriture Mesh        : "<<flush; toMesh.addDriver(MED_DRIVER,resultFileName,toMesh.getName())                            ; cout<<"OK !"<<endl;
//    cout<<flag<<"Ecriture du Mesh résultat                 : "<<flush; toMesh.write(1)                                                                         ; cout<<"OK !"<<endl;

  } catch (MEDEXCEPTION& ex){
    MESSAGE(ex.what()) ;
  }
}
