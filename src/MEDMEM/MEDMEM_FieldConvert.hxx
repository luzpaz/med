#ifndef FIELD_CONVERT_HXX
#define FIELD_CONVERT_HXX

#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_ArrayConvert.hxx"

namespace MEDMEM {

template <class T> FIELD<T,FullInterlace> *
FieldConvert(const FIELD<T,NoInterlace> & field )
{
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFullNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array ArrayFullGa;

  FIELD<T,FullInterlace> * myField = new FIELD<T,FullInterlace>();
  FIELD_ * myField_ = myField;
  FIELD_ * field_ = &(const_cast< FIELD<T,NoInterlace> &> (field));
  *myField_ = *field_;                        // Opérateur d'affectation de FIELD_ OK
  // *((FIELD_ *) myField) = (FIELD_ ) field; //Contructeur par recopie de FIELD_ Pourquoi?

  if  ( field.getGaussPresence() ) {
    ArrayFullGa * myArray = ArrayConvert( *(field.getArrayGauss()) );
    myField->setArray(myArray);
    return myField;
  } else {
    ArrayFullNo * myArray = ArrayConvert( *(field.getArrayNoGauss()) );
    myField->setArray(myArray);
    return myField;
  }
}

template <class T> FIELD<T,NoInterlace> *
FieldConvert(const FIELD<T,FullInterlace> & field )
{
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array ArrayNoNo;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,Gauss>::Array ArrayNoGa;


  FIELD<T,NoInterlace> * myField = new FIELD<T,NoInterlace>();
  FIELD_ * myField_ = myField;
  FIELD_ * field_ = &(const_cast< FIELD<T,FullInterlace> &> (field));
  *myField_ = *field_;                        // Opérateur d'affectation de FIELD_ OK
  //   *((FIELD_*) myField) = (FIELD_) field; //Contructeur par recopie de FIELD_ Pourquoi?

  if  ( field.getGaussPresence() ) {
    ArrayNoGa * myArray = ArrayConvert( *(field.getArrayGauss()) );
    myField->setArray(myArray);
    return myField;
  } else {
    ArrayNoNo * myArray = ArrayConvert( *(field.getArrayNoGauss()) );
    myField->setArray(myArray);
    return myField;
  }

}

}
#endif
