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
	   //NoIndexCheckPolicy>
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
    checkMoreThanZero("MEDMEM_Array",nbelem);
    checkMoreThanZero("MEDMEM_Array",dim);
    _array.set(_arraySize);
  };

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...NoGaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array( ElementType * values, int dim, int nbelem,
		       bool shallowCopy=false,
		       bool ownershipOfValues=false) : InterlacingPolicy(nbelem,dim)
  {
    checkMoreThanZero("MEDMEM_Array",nbelem);
    checkMoreThanZero("MEDMEM_Array",dim);
    if(shallowCopy)

      if(ownershipOfValues)
	_array.setShallowAndOwnership((const ElementType *)values);
      else
	_array.set((const ElementType*)values);

    else // Cas par défaut
      _array.set(_arraySize,values);

  }

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...GaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(int dim, int nbelem, int nbtypegeo,
		      const int * const  nbelgeoc, const int * const nbgaussgeo)
    : InterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc, nbgaussgeo)
  {
    checkMoreThanZero("MEDMEM_Array",nbelem);
    checkMoreThanZero("MEDMEM_Array",dim);
    checkMoreThanZero("MEDMEM_Array",nbtypegeo);
    _array.set(_arraySize);
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
    checkMoreThanZero("MEDMEM_Array",nbelem);
    checkMoreThanZero("MEDMEM_Array",dim);
    checkMoreThanZero("MEDMEM_Array",nbtypegeo);

    if(shallowCopy)

      if(ownershipOfValues)
	_array.setShallowAndOwnership((const ElementType *)values);
      else
	_array.set((const ElementType*)values);

    else
      _array.set(_arraySize,values);

  };

  // Constructeur de recopie pour un MEDMEM_Array avec les mêmes 
  // paramètres template qu'à la construction
  inline MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=true)
    :InterlacingPolicy(array,shallowCopy)
  {
    if (shallowCopy)
      this->_array.set(array._array); // Le propriétaire reste le ARRAY initial
    else
      this->_array.set(_arraySize,array._array);
  }


//   inline MEDMEM_Array( const MEDMEM_Array< class ARRAY_ELEMENT_TYPE_OTHER,
// 		       class INTERLACING_POLICY_OTHER,
// 		       class CHECKING_POLICY_OTHER > & array )
//   {
//     //ERREUR
//   }


  inline MEDMEM_Array<ElementType,InterlacingPolicy,CheckingPolicy> &
         operator=( const MEDMEM_Array & array) {
    if ( this == &array) return *this;
    BEGIN_OF("MEDMEM_Array  operator =");
    InterlacingPolicy::operator=(array); //Appel des classes de base ?

    this->_array.set(array._array); // Le propriétaire reste le ARRAY initial
    // verifier l'appel des opérateurs de la policy

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
      _array.set(_arraySize,values);
  }

  inline const ElementType * getRow(int i) const {
    checkInInclusiveRange("MEDMEM_Array",1,_nbelem,i);
    // Empêche l'utilisation de getRow en mode MED_NO_INTERLACE
    // Ne devrait pas dépendre de la politique check
    checkEquality("MEDMEM_Array (Interlace test)",
		  MED_EN::MED_NO_INTERLACE,
		  _interlacing );
    return &(_array[ getIndex(i,1) ]);

  }

  void setRow(int i,const ElementType * const value) {
    checkInInclusiveRange("MEDMEM_Array",1,_nbelem,i);
    // setRow fonctionne
    // dans les deux modes d'entrelacement.

    for (int j =1; j <= getDim(); j++)
      for (int k = 1 ; k <= getNbGauss(i); k++)
	_array[getIndex(i,j,k)] = value[getIndex(1,j,k)];
  }

  inline const ElementType * getColumn(int j) const {
    checkInInclusiveRange("MEDMEM_Array",1,_dim,j);
    checkEquality("MEDMEM_Array (Interlace test)",
		  MED_EN::MED_FULL_INTERLACE, _interlacing );
    return &(_array[ getIndex(1,j) ]);
  }

  void setColumn(int j, const ElementType * const value) {
    checkInInclusiveRange("MEDMEM_Array",1,_dim,j);
    // setColumn fonctionne
    // dans les deux modes d'entrelacement.

    for (int i=1; i <= getNbElem(); i++)
      for (int k = 1 ; k <= getNbGauss(i); k++)
	_array[getIndex(i,j,k)] = value[getIndex(i,1,k)];
  }


  inline const ElementType & getIJ(int i, int j) const  {
    checkInInclusiveRange("MEDMEM_Array",1,_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,_dim,j);
    return _array[ getIndex(i,j) ];
  }

  inline const ElementType & getIJK(int i, int j, int k ) const {
    checkInInclusiveRange("MEDMEM_Array",1,_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,_dim,j);
    checkInInclusiveRange("MEDMEM_Array",1,getNbGauss(i),k);

    return _array[ getIndex(i,j,k) ];
  };

  inline void setIJ(int i, int j, const ElementType & value) {   //autre signature avec
    checkInInclusiveRange("MEDMEM_Array",1,_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,_dim,j);

    _array[ getIndex(i,j) ] = value;                      // retour ElementType & ?
  };

  inline void setIJK(int i, int j, int k, const ElementType & value) {   //autre signature avec
    checkInInclusiveRange("MEDMEM_Array",1,_nbelem,i);
    checkInInclusiveRange("MEDMEM_Array",1,_dim,j);
    checkInInclusiveRange("MEDMEM_Array",1,getNbGauss(i),k);

    _array[ getIndex(i,j,k) ] = value;                      // retour ElementType & ?
  };


private:

  PointerOf<ElementType> _array;
};

} //END NAMESPACE
#endif
