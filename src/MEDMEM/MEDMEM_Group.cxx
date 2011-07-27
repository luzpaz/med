//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

/*
 File Group.cxx
 $Header$
*/
#include <list>

#include "MEDMEM_Group.hxx"
#include "MEDMEM_Family.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

GROUP::GROUP():SUPPORT(),_numberOfFamilies(0),_family() 
{
  MESSAGE_MED("GROUP()");
}

GROUP::~GROUP() 
{
  MESSAGE_MED("~GROUP()");
}
  
GROUP & GROUP::operator=(const GROUP &group) 
{
  MESSAGE_MED("GROUP::operator=");
  if ( &group == this ) return *this;
  SUPPORT::operator=(group);
  _numberOfFamilies = group._numberOfFamilies ;
  _family           = group._family ;
  return *this;
}

ostream & MEDMEM::operator<<(ostream &os, GROUP &myGroup)
{
  os << (SUPPORT) myGroup;

  int numberoffamilies = myGroup.getNumberOfFamilies();
  os << "  - Families ("<<numberoffamilies<<") :"<<endl;
  for (int j=1;j<numberoffamilies+1;j++)
    os << "    * "<<myGroup.getFamily(j)->getName().c_str()<<endl ;

  return os;
}

GROUP::GROUP(const string & name, const list<FAMILY*> & families) throw (MEDEXCEPTION)
{
  const char * LOC = "GROUP( const string & , const list<FAMILY*> & ) : " ;
  
  BEGIN_OF_MED(LOC);

  MESSAGE_MED(LOC<<name);

  int numberOfFamilies = families.size();
  _name = name ;
  _description = "GROUP";
  // description : none !
  // first FAMILY to set all !
  FAMILY * myFamily = families.front() ;
  _mesh =  myFamily->getMesh() ;
  _entity = myFamily->getEntity() ;
  bool isOnAllElts = myFamily->isOnAllElements() ;

  SCRUTE_MED(isOnAllElts);
  SCRUTE_MED(numberOfFamilies);


  if ((numberOfFamilies==1) && (isOnAllElts))
    {
      _numberOfFamilies = numberOfFamilies;
      _isOnAllElts = isOnAllElts ;
      _family.resize(_numberOfFamilies) ;
      _family[0] = myFamily;
      update();
      return;
    }
  else if ((!(numberOfFamilies==1)) && (isOnAllElts))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "building of a GROUP object from several FAMILY, and one of them is on all entities"  )) ;

  _numberOfGeometricType = myFamily->getNumberOfTypes() ;

  _geometricType.set(_numberOfGeometricType);
  _numberOfElements.set(_numberOfGeometricType) ;

  const medGeometryElement * geometricType = myFamily->getTypes() ;
  //int * geometricTypeNumber = myFamily->getGeometricTypeNumber() ;

  SCRUTE_MED(_numberOfGeometricType);

  for (int i=0 ; i<_numberOfGeometricType; i++) {
    _geometricType[i]= geometricType[i] ;
    // _geometricTypeNumber[i] = geometricTypeNumber[i] ;
    _numberOfElements[i]=myFamily->getNumberOfElements(geometricType[i]);
    MESSAGE_MED(LOC << " Type : " << _geometricType[i] << " number of element(s) " << _numberOfElements[i]);
  }
  _isOnAllElts = false ;
  //_totalNumberOfEntities = myFamily->getNumberOfElements(MED_ALL_ELEMENTS) ;


  MEDSKYLINEARRAY * famNumber = myFamily->getnumber();

  const int * famNumberValue = myFamily->getNumber(MED_ALL_ELEMENTS);

  const int * famNumberIndex = myFamily->getNumberIndex();

  int famNumberCount = famNumber->getNumberOf();
  int famNumberLength = famNumber->getLength();

  SCRUTE_MED(famNumber);
  SCRUTE_MED(famNumberCount);
  SCRUTE_MED(famNumberLength);
  SCRUTE_MED(famNumberValue);
  SCRUTE_MED(famNumberIndex);

//   _number = new MEDSKYLINEARRAY(*famNumber) ;
  _number = new MEDSKYLINEARRAY(famNumberCount,famNumberLength,
                                famNumberIndex,famNumberValue) ;

  SCRUTE_MED(_number);

  _numberOfFamilies = families.size();

  SCRUTE_MED(numberOfFamilies);

  //SCRUTE_MED(_numberOfFamilies);

  _family.resize(_numberOfFamilies) ;
  list<FAMILY*>::const_iterator li ;

  // MESSAGE_MED(LOC<<"Printing of the object GROUP built right before the blending"<< (SUPPORT) *this);


  int it = 0 ;
  for (li=families.begin();li!=families.end();li++) {
    blending(*li);
    _family[it] = (*li) ;
    it++ ;
  }

  //MESSAGE_MED(LOC<<"Printing of the object GROUP built "<< (GROUP)*this);

  END_OF_MED(LOC);
}

GROUP::GROUP(const GROUP & m):SUPPORT(m)
{
  _numberOfFamilies = m._numberOfFamilies;
  _family = m._family; //Copie profonde dans FAMILY Rmq from EF
}

// void GROUP::init(const list<FAMILY*> & families)
// {
  
//   BEGIN_OF_MED(LOC);
  
//   FAMILY * myFamily = families.front() ;
//   _mesh =  myFamily->getMesh() ;

//   _isOnAllElts = myFamily->isOnAllElements() ;

//   SCRUTE_MED(_mesh);

//   SCRUTE_MED(_isOnAllElts);

//   _entity = myFamily->getEntity() ;

//   SCRUTE_MED(_mesh->getNumberOfTypes(_entity));

//   _numberOfGeometricType = myFamily->getNumberOfTypes() ;
//   _geometricType = new medGeometryElement[_numberOfGeometricType];
//   //_geometricTypeNumber = new int[_numberOfGeometricType] ;
//   _numberOfGaussPoints = new int[_numberOfGeometricType] ;
//   _numberOfElements = new int[_numberOfGeometricType] ;
//   medGeometryElement * geometricType = myFamily->getTypes() ;
//   //int * geometricTypeNumber = myFamily->getGeometricTypeNumber() ;
//   int * numberOfGaussPoints = myFamily->getNumberOfGaussPoints() ;
//   for (int i=0 ; i<_numberOfGeometricType; i++) {
//     _geometricType[i]= geometricType[i] ;
//     // _geometricTypeNumber[i] = geometricTypeNumber[i] ;
//     _numberOfGaussPoints[i] = numberOfGaussPoints[i] ;
//     _numberOfElements[i]=myFamily->getNumberOfElements(geometricType[i]);
//   }
//   _isOnAllElts = false ;
//   _totalNumberOfEntities = myFamily->getNumberOfElements(MED_ALL_ELEMENTS) ;
//   _number = new MEDSKYLINEARRAY(*myFamily->getnumber()) ;
  
//   _numberOfFamilies = families.size();
//   _family.resize(_numberOfFamilies) ;
//   list<FAMILY*>::const_iterator liIni = families.begin() ;
//   _family[0]=(*liIni);
//   liIni++;
//   list<FAMILY*>::const_iterator li ;
//   int it = 1 ;
//   for (li=liIni;li!=families.end();li++) {
//     blending(*li);
//     _family[it] = (*li) ;
//     it++ ;
//   }
  
//   END_OF_MED();
// };

