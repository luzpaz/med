//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : test_MEDMEM_CellModel.cxx
//  Module : MED

using namespace std;
#include "utilities.h"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_DriversDef.hxx"

int main (int argc, char ** argv) {
  list<MED_FR::med_geometrie_element> geomList = MED_FR::meshEntities[MED_FR::MED_MAILLE] ;
  list<MED_FR::med_geometrie_element>::iterator itGeomList ;
  for(itGeomList=geomList.begin();itGeomList!=geomList.end();itGeomList++) {
    CELLMODEL myCellModel((MED_EN::medGeometryElement)(*itGeomList)) ;

    cout << "First CellModel :" << endl << endl ;

    cout << myCellModel << endl ;

    cout << "Number of constituents type : "<<myCellModel.getNumberOfConstituentsType() << endl ;

    set <medGeometryElement> geomType = myCellModel.getAllConstituentsType() ;
    set <medGeometryElement>::iterator geomTypeIt ;
    cout << "Types list : " ;
    for(geomTypeIt=geomType.begin();geomTypeIt!=geomType.end();geomTypeIt++)
      cout << (*geomTypeIt) << " " ;
    cout << endl;
    
    map <medGeometryElement,int> myMap = myCellModel.getNumberOfConstituentsForeachType() ; 
    map <medGeometryElement,int>::iterator myMapIt ;
    for(myMapIt=myMap.begin();myMapIt!=myMap.end();myMapIt++)
      cout << " - " << (*myMapIt).second << " constituent(s) of type " << (*myMapIt).first << endl ;
    cout << endl;

    cout << "Second CellModel :" << endl << endl ;
    CELLMODEL myCellModel2=myCellModel ;
    cout << myCellModel2 << endl ;

    cout << "Third CellModel :" << endl << endl ;
    CELLMODEL myCellModel3 ;
    myCellModel3=myCellModel ;
    cout << myCellModel3 << endl ;
  }
}
