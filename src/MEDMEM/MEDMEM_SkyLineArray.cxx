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
//  File   : MEDMEM_SkyLineArray.cxx
//  Module : MED

using namespace std;
#include "MEDMEM_SkyLineArray.hxx"
#include "utilities.h"

MEDSKYLINEARRAY::MEDSKYLINEARRAY(): _count(0), _length(0),
				_index((med_int*)NULL),_value((med_int*)NULL)
{
  MESSAGE("Constructeur MEDSKYLINEARRAY sans parametre");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const MEDSKYLINEARRAY &myArray):
			        _count(myArray._count),_length(myArray._length),
				_index(_count+1),_value(_length)
{
	BEGIN_OF("MEDSKYLINEARRAY(const MEDSKYLINEARRAY &)");
	memcpy(_index,myArray._index,sizeof(med_int)*(_count+1));
	memcpy(_value,myArray._value,sizeof(med_int)*_length);
	END_OF("MEDSKYLINEARRAY(const MEDSKYLINEARRAY &)");
}

MEDSKYLINEARRAY::~MEDSKYLINEARRAY()
{
  MESSAGE("Destructeur ~MEDSKYLINEARRAY");

  //if (_index != NULL) delete [] _index;
  //if (_value != NULL) delete [] _value;
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const med_int count, const med_int length):
                                _count(count), _length(length),
                                _index(_count+1),_value(_length)
{
	MESSAGE("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const med_int count, const med_int length,
				 const med_int* index, const med_int* value):
                                _count(count), _length(length),
                                _index(_count+1),_value(_length)
{
	MESSAGE("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
	memcpy((med_int*)_index,index,sizeof(med_int)*(_count+1));
	memcpy((med_int*)_value,value,sizeof(med_int)*_length);
}

//  void MEDSKYLINEARRAY::setMEDSKYLINEARRAY( const med_int count , const med_int length, med_int* index , med_int* value )
//  {
//    MESSAGE("void MEDSKYLINEARRAY::setMEDSKYLINEARRAY(count, length, index, value)");
//    _count  = count  ;
//    _length = length ;

//    //if (_index != NULL) delete [] _index;
//    //if (_value != NULL) delete [] _value;

//  	  _index.set(index);
//  	  _value.set(value);
//  }
