#ifndef MEDMEM_ARRAY_HXX
#define MEDMEM_ARRAY_HXX

#include "MEDMEM_InterlacingPolicy.hxx"
#include "MEDMEM_IndexCheckingPolicy.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {

class MEDMEM_Array_ {
public:
  //virtual void dummy() {};
  virtual bool getGaussPresence() const { return false; }
  virtual MED_EN::medModeSwitch getInterlacingType() const {return MED_EN::MED_UNDEFINED_INTERLACE;}
  virtual ~MEDMEM_Array_() {}; //Indispensable pour détruire le vrai objet pointé
};

template < class ARRAY_ELEMENT_TYPE,
	   class INTERLACING_POLICY=FullInterlaceNoGaussPolicy,
	   class CHECKING_POLICY=IndexCheckPolicy >
class MEDMEM_Array : public INTERLACING_POLICY, public CHECKING_POLICY, public MEDMEM_Array_ {

public :

  typedef ARRAY_ELEMENT_TYPE  ElementType;
  typedef INTERLACING_POLICY  InterlacingPolicy;
  typedef CHECKING_POLICY     CheckingPolicy;

public  :
  MEDMEM_Array():_array( ( ElementType *) NULL)  {}; //Interdit le constructeur par défaut, peut pas à  cause du FIELD

  ~MEDMEM_Array() {
    // PointerOf s'occupe de la desallocation.
  };

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...NoGaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(int dim, int nbelem) : InterlacingPolicy(nbelem,dim)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    _array.set(InterlacingPolicy::_arraySize);
  };

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...NoGaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array( ElementType * values, int dim, int nbelem,
		       bool shallowCopy=false,
		       bool ownershipOfValues=false) : InterlacingPolicy(nbelem,dim)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    if(shallowCopy)

      if(ownershipOfValues)
	_array.setShallowAndOwnership((const ElementType *)values);
      else
	_array.set((const ElementType*)values);

    else // Cas par défaut
      _array.set(InterlacingPolicy::_arraySize,values);

  }

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...GaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(int dim, int nbelem, int nbtypegeo,
		      const int * const  nbelgeoc, const int * const nbgaussgeo)
    : InterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc, nbgaussgeo)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbtypegeo);
    _array.set(InterlacingPolicy::_arraySize);
  };


  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...GaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(ElementType * values, int dim, int nbelem, int nbtypegeo,
		      const int * const  nbelgeoc, const int * const  nbgaussgeo,
		      bool shallowCopy=false,
		      bool ownershipOfValues=false)
    : InterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc, nbgaussgeo)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbtypegeo);

    if(shallowCopy)

      if(ownershipOfValues)
	_array.setShallowAndOwnership((const ElementType *)values);
      else
	_array.set((const ElementType*)values);

    else
      _array.set(InterlacingPolicy::_arraySize,values);

  };

  // Constructeur de recopie pour un MEDMEM_Array avec les mêmes
  // paramètres template qu'à la construction
  inline MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false)
    :InterlacingPolicy(array,shallowCopy)
  {
    if (shallowCopy)
      this->_array.set(array._array); // Le propriétaire reste le ARRAY initial
    else
      this->_array.set(InterlacingPolicy::_arraySize,array._array);
  }


  // L'utilisation d'une copie superficielle pour l'opérateur d'affectation
  // ne me parait pas être une bonne ideé : Compatibilité ancienne version MEDARRAY?
  inline MEDMEM_Array<ElementType,InterlacingPolicy,CheckingPolicy> &
         operator=( const MEDMEM_Array & array) {
    if ( this == &array) return *this;
    BEGIN_OF("MEDMEM_Array  operator =");
    InterlacingPolicy::operator=(array); //Appel des classes de base ?

    this->_array.set(array._array); // Le propriétaire reste le ARRAY initial

    return *this;
  }

  MED_EN::medModeSwitch getInterlacingType() const {
    return InterlacingPolicy::getInterlacingType();
  }

  bool getGaussPresence() const {
    return InterlacingPolicy::getGaussPresence();
  }

  ElementType * getPtr() {
    return  _array;
  }

  void setPtr(ElementType * values, bool shallowCopy=false,
	      bool ownershipOfValues=false) {

    if(shallowCopy)

      if(ownershipOfValues)
	_array.setShallowAndOwnership((const ElementType *)values);
      else
	_array.set((const ElementType*)values);

    else
      _array.set(InterlacingPolicy::_arraySize,values);
  }

  inline const ElementType * getRow(int i) const {
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    // Empêche l'utilisation de getRow en mode MED_NO_INTERLACE
    // Ne devrait pas dépendre de la politique check
    checkEquality("MEDMEM_Array (Interlace test)",
		  MED_EN::MED_NO_INTERLACE,
		  InterlacingPolicy::_interlacing );
    return &(_array[ InterlacingPolicy::getIndex(i,1) ]);

  }

  void setRow(int i,const ElementType * const value) {
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    // setRow fonctionne
    // dans les deux modes d'entrelacement.

    for (int j =1; j <= InterlacingPolicy::getDim(); j++)
      for (int k = 1 ; k <= InterlacingPolicy::getNbGauss(i); k++)
	_array[InterlacingPolicy::getIndex(i,j,k)] = value[InterlacingPolicy::getIndex(1,j,k)];
  }

  inline const ElementType * getColumn(int j) const {
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    checkEquality("MEDMEM_Array (Interlace test)",
		  MED_EN::MED_FULL_INTERLACE, InterlacingPolicy::_interlacing );
    return &(_array[ InterlacingPolicy::getIndex(1,j) ]);
  }

  void setColumn(int j, const ElementType * const value) {
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    // setColumn fonctionne
    // dans les deux modes d'entrelacement.

    for (int i=1; i <= InterlacingPolicy::getNbElem(); i++)
      for (int k = 1 ; k <= InterlacingPolicy::getNbGauss(i); k++)
	_array[InterlacingPolicy::getIndex(i,j,k)] = value[InterlacingPolicy::getIndex(i,1,k)];
  }


  inline const ElementType & getIJ(int i, int j) const  {
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    return _array[ InterlacingPolicy::getIndex(i,j) ];
  }

  inline const ElementType & getIJK(int i, int j, int k ) const {
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGauss(i),k);

    return _array[ InterlacingPolicy::getIndex(i,j,k) ];
  };

  inline void setIJ(int i, int j, const ElementType & value) {   //autre signature avec
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);

    _array[ InterlacingPolicy::getIndex(i,j) ] = value;                      // retour ElementType & ?
  };

  inline void setIJK(int i, int j, int k, const ElementType & value) {   //autre signature avec
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGauss(i),k);

    _array[ InterlacingPolicy::getIndex(i,j,k) ] = value;                      // retour ElementType & ?
  };

  bool operator == (const MEDMEM_Array & array ) const {

    if ( this == &array ) return true;

    int size = array.getArraySize();
    if ( size != getArraySize() ) return false;

    ARRAY_ELEMENT_TYPE * arrayPtr =
      const_cast<MEDMEM_Array &>(array).getPtr();
    for (int i=0; i < size; ++i)
      if (_array[i] != arrayPtr[i]) return false;

    return true;
  }

  friend ostream & operator<<(ostream & os, const MEDMEM_Array & array) {

    for (int i=1;i<=array.getNbElem();++i) {
      for (int j=1; j<=array.getDim();++j)
	for (int k=1;k<=array.getNbGauss(i);++k)
	  os << "Value [" << i << "," << j << "," << k << "] = " << array.getIJK(i,j,k) << ", ";
      os << endl;
    }
    return os;
  }

private:

  PointerOf<ElementType> _array;
};

} //END NAMESPACE
#endif
