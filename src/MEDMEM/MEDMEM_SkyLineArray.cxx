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
