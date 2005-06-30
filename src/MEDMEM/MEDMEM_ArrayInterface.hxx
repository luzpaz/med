#ifndef MEDMEM_ARRAYINTERFACE_HXX
#define MEDMEM_ARRAYINTERFACE_HXX

#include "MEDMEM_nArray.hxx"
#include "MEDMEM_InterlacingTraits.hxx"

// L'astuce d'une classe d'interface consiste en 
// 1) La d�claration d'un type qui est celui de la classe d'impl�mentation
// 2) D'utiliser ce nouveau nom de type comme param�tres de toutes
//     les m�thodes de l'interface.
// L'inconvenient est qu'il faut justement passer en argument une instance de
// le classe d'impl�mentation dans toutes les m�thodes et que la classe
// appelante aura aussi � faire ce travail.
//  Ne surtout pas oublier inline sinon l'interface couterait cher � l'appel
//  des m�thodes !
namespace MEDMEM {

template < class ARRAY_ELEMENT_TYPE,
	   class INTERLACE_TAG,
	   class GAUSS_TAG,
	   class CHECKING_POLICY=IndexCheckPolicy>
	   //NoIndexCheckPolicy>
class MEDMEM_ArrayInterface  {

public:

  // Les type ElementType et Array sont a d�finir aussi dans les classes utilisatrice
  // par une d�claration du type : typedef typename ArrayInterface::Array Array;

  typedef  ARRAY_ELEMENT_TYPE  ElementType;
  typedef  INTERLACE_TAG       Interlacing;
  typedef  GAUSS_TAG           GaussPresence;
  typedef  typename MEDMEM_InterlacingTraits<Interlacing,GaussPresence>::Type InterlacingPolicy;
  typedef  CHECKING_POLICY     CheckingPolicy;
  typedef  MEDMEM_Array<ElementType,InterlacingPolicy,CheckingPolicy> Array;

  static inline int getNbGauss(int i, const Array & array)  {
    return array.getNbGauss(i);
  };

  static inline ElementType * getPtr( Array & array)  {
    return array.getPtr();
  };

  static inline void setPtr( ElementType * arrayptr,  Array & array, 
			     bool shallowCopy=false,
			     bool ownershipOfValues=false )  {
    array.setPtr(arrayptr,shallowCopy,ownershipOfValues);
  };

  static inline const ElementType * getRow(int i, const Array & array ) {
    return array.getRow(i);
  }

  static inline void setRow(int i, const ElementType & value, const Array & array ) {
    return array.setRow(i,value);
  }

  static inline const ElementType * getColumn(int j, const Array & array ) {
    return array.getColumn(j);
  }

  static inline void setColumn(int j, const ElementType & value, const Array & array ) {
    return array.setColumn(j,value);
  }

  static inline const ElementType & getIJ(int i, int j, const Array & array) {
    return array.getIJ(i,j);
  }

  static inline const ElementType & getIJK(int i, int j, int k, const Array & array) {
    return array.getIJK(i,j,k);
  }

  static inline void setIJ(int i, int j, const ElementType & value, Array & array) {
    array.setIJ(i,j,value);
  }

  static inline void setIJK(int i, int j, int k, const ElementType & value, Array & array) {
    array.setIJK(i,j,k,value);
  }

};

} //END NAMESPACE
#endif
