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
