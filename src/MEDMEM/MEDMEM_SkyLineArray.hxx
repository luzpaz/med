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
# ifndef __MEDSKYLINEARRAY_H__
# define __MEDSKYLINEARRAY_H__

#include "MEDMEM_Exception.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {
class MEDSKYLINEARRAY
{
private :
  int   _count ;
  int   _length ;
  PointerOf <MED_EN::med_int/*int*/> _index ; // array of size _count+1 : _index[0]=1 and
                           // _index[_count]=length+1
  PointerOf <MED_EN::med_int/*int*/> _value ; // array of size _length

public :
  // Attention, avec ce constructeur, il n'est possible de remplir le MEDSKYLINEARRAY 
  MEDSKYLINEARRAY();

  // Constructeur par recopie
  MEDSKYLINEARRAY( const MEDSKYLINEARRAY &myArray );

  // Avec ce constructeur la mémoire pour le tableau  de valeur et le
  // tableau d'index est réservée. Il suffit d'effectuer les séquences
  // d'appels suivantes pour initialiser le MEDSKYLINEARRAY
  // 1) setIndex(index) puis <count> fois setI(i,&listValeurN°I) avec i dans 1..count
  //    rem :   listValeurN°I est dupliquée
  // 2) appeler <length> fois setIJ(i,j,valeur) avec i dans 1..count et avec j dans 1..count
  MEDSKYLINEARRAY( const int count, const int length );

  // Avec ce constructeur le MEDSKYLINEARRAY est complètement initialisé
  // Si shallowCopy=false (par défaut) les tableaux d'index et de valeurs
  // sont dupliqués
  // Sinon le MEDSKYLINEARRAY prend directement les pointeurs et en devient 
  // propriétaire
  MEDSKYLINEARRAY( const int count, const int length,
		   const MED_EN::med_int* /* const int* */ index, 
		   const MED_EN::med_int* /* const int* */ value, 
		   bool shallowCopy=false );

  ~MEDSKYLINEARRAY();
  //void setMEDSKYLINEARRAY( const int count, const int length, int* index , int* value ) ;

  inline int  getNumberOf()       const;
  inline int  getLength()         const;
  inline const MED_EN::med_int* /* const int* */  getIndex()   const;
  inline const MED_EN::med_int* /* const int* */  getValue()   const;
  inline MED_EN::med_int /*int*/  getNumberOfI(int i) const throw (MEDEXCEPTION) ;
  inline const MED_EN::med_int* /* const int* */  getI(int i)  const throw (MEDEXCEPTION) ;
  inline MED_EN::med_int /*int*/  getIJ(int i, int j) const throw (MEDEXCEPTION) ;
  inline MED_EN::med_int /*int*/  getIndexValue(int i) const throw (MEDEXCEPTION) ;

  inline void setIndex(const MED_EN::med_int* /* const int* */ index) ;
  inline void setI(const int i, const MED_EN::med_int* /* const int* */ values) throw (MEDEXCEPTION) ;
  inline void setIJ(int i, int j, MED_EN::med_int /*int*/ value) throw (MEDEXCEPTION) ;
  inline void setIndexValue(int i, MED_EN::med_int /*int*/ value) throw (MEDEXCEPTION) ;

  friend ostream& operator<<(ostream &os, const MEDSKYLINEARRAY &sky);
};

// ---------------------------------------
//              Methodes Inline
// ---------------------------------------
inline int MEDSKYLINEARRAY::getNumberOf() const
{
  return _count ;
};
inline int MEDSKYLINEARRAY::getLength() const
{
  return _length ;
};
inline const MED_EN::med_int* /* const int* */  MEDSKYLINEARRAY::getIndex() const
{
  return (const MED_EN::med_int* /* const int* */)_index ;
} ;
inline const MED_EN::med_int* /* const int* */  MEDSKYLINEARRAY::getValue() const
{
  return (const MED_EN::med_int* /* const int* */)_value ;
} ;
inline MED_EN::med_int /*int*/ MEDSKYLINEARRAY::getNumberOfI(int i) const throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument must be >= 1");
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getNumberOfI : argument is out of range");
  return _index[i]-_index[i-1] ;
} ;
inline const MED_EN::med_int* /* const int* */ MEDSKYLINEARRAY::getI(int i) const throw (MEDEXCEPTION)
{
    if (i<1)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getI : argument must be >= 1");
    if (i>_count)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getI : argument is out of range");
    return _value+_index[i-1]-1 ;
}
inline MED_EN::med_int /*int*/ MEDSKYLINEARRAY::getIJ(int i, int j) const throw (MEDEXCEPTION)
{
    if (i<1)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : first argument must be >= 1");
    if (j<1)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : second argument must be >= 1");
    if (i>_count)
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : first argument is out of range") ;
    if (j>_index[i])
      throw MEDEXCEPTION("MEDSKYLINEARRAY::getIJ : second argument is out of range") ;
    return _value[_index[i-1]+j-2] ;
}

inline MED_EN::med_int /*int*/  MEDSKYLINEARRAY::getIndexValue(int i) const throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getIndexValue : argument must be >= 1");
  if (i>_index[_count])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::getIndexValue : argument is out of range") ;
  return _value[i-1] ;
}

inline void MEDSKYLINEARRAY::setIndex(const MED_EN::med_int* /* const int* */ index)
{
  memcpy((MED_EN::med_int* /*int**/)_index,index,(_count+1)*sizeof(MED_EN::med_int/*int*/));
}


inline void MEDSKYLINEARRAY::setIJ(int i, int j, MED_EN::med_int /*int*/ value) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : first argument must be >= 1");
  if (j<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : second argument must be >= 1");
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : first argument is out of range") ;
  if (j>_index[i])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIJ : second argument is out of range") ;
  
  _value[_index[i-1]+j-2]=value ;

}

inline void MEDSKYLINEARRAY::setI(const int i, const MED_EN::med_int* /* const int * */ values) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setI : index must be >= 1");
;
  if (i>_count)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setI : index is out of range") ;

  memcpy(_value+_index[i-1]-1,values,(_index[i]-_index[i-1])*sizeof(MED_EN::med_int/*int*/)) ;
}

inline void MEDSKYLINEARRAY::setIndexValue(int i, MED_EN::med_int /*int*/ value) throw (MEDEXCEPTION)
{
  if (i<1)
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIndexValue : argument must be >= 1");
  if (i>_index[_count])
    throw MEDEXCEPTION("MEDSKYLINEARRAY::setIndexValue : argument is out of range") ;
  _value[i-1]=value ;
}
}
# endif
