#include "MEDMEM_SkyLineArray.hxx"
#include "utilities.h"

MEDSKYLINEARRAY::MEDSKYLINEARRAY(): _count(0), _length(0),
				_index((med_int*)NULL),_value((med_int*)NULL)
{
  MESSAGE("Constructeur MEDSKYLINEARRAY sans parametre");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const MEDSKYLINEARRAY &myArray)
{
  MESSAGE("MEDSKYLINEARRAY(const MEDSKYLINEARRAY &)");
  _count = myArray._count ;
  _length = myArray._length ;
  _index = new med_int[_count+1] ;
  _value = new med_int[_length] ;
  memcpy(_index,myArray._index,sizeof(med_int)*(_count+1));
  memcpy(_value,myArray._value,sizeof(med_int)*_length);
}

MEDSKYLINEARRAY::~MEDSKYLINEARRAY()
{
  MESSAGE("Destructeur ~MEDSKYLINEARRAY");

  if (_index != NULL) delete [] _index;
  if (_value != NULL) delete [] _value;
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const med_int count , const med_int length ) :
    		             _count(count), _length(length)
{
  MESSAGE("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");

  _index = new med_int[count+1] ;
  _value = new med_int[length] ;
}

void MEDSKYLINEARRAY::setMEDSKYLINEARRAY( const med_int count , const med_int length, med_int* index , med_int* value ) 
{
  MESSAGE("void MEDSKYLINEARRAY::setMEDSKYLINEARRAY(count, length, index, value)");
  _count  = count  ;
  _length = length ;

  if (_index != NULL) delete [] _index;
  if (_value != NULL) delete [] _value;

  _index = index;
  _value = value;
}
