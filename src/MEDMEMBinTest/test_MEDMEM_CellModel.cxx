// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_DriversDef.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

int main (int argc, char ** argv) {
  list<medGeometryElement> geomList = meshEntities[MED_CELL] ;
  list<medGeometryElement>::iterator itGeomList ;
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
