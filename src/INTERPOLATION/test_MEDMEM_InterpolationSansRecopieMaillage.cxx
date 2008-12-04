//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
//  const char * resultFileName = "ResultatInterpolation.med";

  const char * fromFieldName  = "THERDEP_TEMP____________________";

  const char * fromMeshName   = "MA";
  const char * toMeshName     = "MAILLAGE_IDEAS";

  try {
    
    string flag="================[MAIN MESSAGE_MEDS]================> ";
    
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
    MESSAGE_MED(ex.what()) ;
  }
}
