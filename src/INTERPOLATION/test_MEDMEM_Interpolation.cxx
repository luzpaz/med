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

#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Interpolation.hxx"

#include "stdio.h"

int main () {
  const char * fromFileName  = "fromMesh.med";
  const char * toFileName    = "toMesh.med";
  //const char * fieldName     = "fieldcelldoublevector"; 
  const char * fieldName     = "fieldnodedouble";

  const char * fromMeshName  = "fromMesh";
  const char * toMeshName    = "toMesh";

  try {
    
    cout<<"Lecture du Maillage Source : "<<flush; MESH            fromMesh    (MED_DRIVER,fromFileName,fromMeshName);           cout<<"OK !"<<endl;
    //cout<<"Construction du support    : "<<flush; SUPPORT         fromSupport (&fromMesh,"XsupportX",MED_CELL);                 cout<<"OK !"<<endl;    
    cout<<"Construction du support    : "<<flush; SUPPORT         fromSupport (&fromMesh,"XsupportX",MED_NODE);                 cout<<"OK !"<<endl;
    cout<<"Lecture du champ           : "<<flush; FIELD<double>   fromField   (&fromSupport,MED_DRIVER,fromFileName,fieldName); cout<<"OK !"<<endl;
    cout<<"Lecture du Mailllage Cible : "<<flush; MESH            toMesh      (MED_DRIVER,toFileName,toMeshName);               cout<<"OK !"<<endl;
    
    INTERPOLATION<3> myInter (fromField,toMesh);
    
    //FIELD<double>   * toField = myInter.interpolate(0,1);
    FIELD<double>   * toField = myInter.interpolate(1,1);
    
    cout<<"Creation du driver"<<endl;
    
    toField->addDriver(MED_DRIVER,toFileName,toField->getName()) ;

    cout<<"toField->getName()                   = "<<toField->getName()                  <<endl;
    cout<<"toField->getDescription()            = "<<toField->getDescription()           <<endl;
    cout<<"toField->getNumberOfComponents()     = "<<toField->getNumberOfComponents()    <<endl;
    cout<<"toField->getNumberOfValues()         = "<<toField->getNumberOfValues()        <<endl;
    cout<<"toField->getComponentsNames()        = "<<toField->getComponentsNames()       <<endl;
    cout<<"toField->getComponentsDescriptions() = "<<toField->getComponentsDescriptions()<<endl;
    cout<<"toField->getMEDComponentsUnits()     = "<<toField->getMEDComponentsUnits()    <<endl;
    cout<<"toField->getIterationNumber()        = "<<toField->getIterationNumber()       <<endl;
    cout<<"toField->getTime()                   = "<<toField->getTime()                  <<endl;
    cout<<"toField->getOrderNumber()            = "<<toField->getOrderNumber()           <<endl;
    cout<<"toField->getValueType()              = "<<toField->getValueType()             <<endl;

    toField->write();

    cout<<"Fin"<<endl;

  } catch (MEDEXCEPTION& ex){
    MESSAGE_MED(ex.what()) ;
  }
}
