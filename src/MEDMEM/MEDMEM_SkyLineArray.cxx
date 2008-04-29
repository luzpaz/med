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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_Utilities.hxx"

using namespace std;
using namespace MEDMEM;

MEDSKYLINEARRAY::MEDSKYLINEARRAY(): _count(0), _length(0),
				_index((int*)NULL),_value((int*)NULL)
{
  MESSAGE("Constructeur MEDSKYLINEARRAY sans parametre");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const MEDSKYLINEARRAY &myArray):
			        _count(myArray._count),_length(myArray._length),
				_index(_count+1),_value(_length)
{
  BEGIN_OF("MEDSKYLINEARRAY(const MEDSKYLINEARRAY &)");
  memcpy(_index,myArray._index,sizeof(int)*(_count+1));
  memcpy(_value,myArray._value,sizeof(int)*_length);
  END_OF();
}

MEDSKYLINEARRAY::~MEDSKYLINEARRAY()
{
  MESSAGE("Destructeur ~MEDSKYLINEARRAY");

  //if (_index != NULL) delete [] _index;
  //if (_value != NULL) delete [] _value;
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const int count, const int length):
                                _count(count), _length(length),
                                _index(_count+1),_value(_length)
{
	MESSAGE("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const int count, const int length,
				 const int* index, const int* value,bool shallowCopy):
                                _count(count), _length(length)
{
// 	MESSAGE("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
		if(shallowCopy)
	  {
	    _index.setShallowAndOwnership(index);
	    _value.setShallowAndOwnership(value);
	  }
	else
	  {
	    _index.set(_count+1,index);
	    _value.set(_length,value);
	  }
}

ostream& MEDMEM::operator<<(ostream &os, const MEDSKYLINEARRAY &sky) {
  os << "_count : " << sky._count << " ,_length : " << sky._length;
  for (int i = 0; i < sky._count ; i++) {
    os << endl << "Values of type n�" << i+1 << " (index["<<i<<"]="<< sky._index[i]<<") :" << endl;
    for (int j=sky._index[i]-1;j < sky._index[i+1]-1;j++)
      os << sky._value[j] << " " ;
  }
  return os;
}

//  void MEDSKYLINEARRAY::setMEDSKYLINEARRAY( const int count , const int length, int* index , int* value )
//  {
//    MESSAGE("void MEDSKYLINEARRAY::setMEDSKYLINEARRAY(count, length, index, value)");
//    _count  = count  ;
//    _length = length ;

//    //if (_index != NULL) delete [] _index;
//    //if (_value != NULL) delete [] _value;

//  	  _index.set(index);
//  	  _value.set(value);
//  }
