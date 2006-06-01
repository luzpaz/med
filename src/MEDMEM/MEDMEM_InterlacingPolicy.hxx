// Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
//           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
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
#ifndef MEDMEM_INTERLACING_HXX
#define MEDMEM_INTERLACING_HXX

#include <iostream>
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {

class InterlacingPolicy {
protected:
  ~InterlacingPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
public :
  InterlacingPolicy(void) : _dim(0), _nbelem(0),
			    _arraySize(0),
			    _interlacing(MED_EN::MED_UNDEFINED_INTERLACE),
			    _gaussPresence(false) {}

  InterlacingPolicy(int nbelem, int dim, int arraySize=0, int interlacing=MED_EN::MED_UNDEFINED_INTERLACE) :
    _dim(dim),
    _nbelem(nbelem),
    _arraySize(arraySize),
    _interlacing(interlacing),
    _gaussPresence(false) {}

  // Constructeur par recopie
  InterlacingPolicy(const InterlacingPolicy & intpol,
		    bool shallowcopy = true) :_dim(intpol._dim),
					      _nbelem(intpol._nbelem),
					      _arraySize(intpol._arraySize),
					      _interlacing(intpol._interlacing),
					      _gaussPresence(intpol._gaussPresence) {}

  InterlacingPolicy & operator=(const InterlacingPolicy & intpol) {
    if ( this == &intpol ) return *this;
    BEGIN_OF("InterlacingPolicy operator =");

    _dim           = intpol._dim;
    _nbelem        = intpol._nbelem; //ne prend pas en compte les points de Gauss
    _arraySize     = intpol._arraySize;
    _interlacing   = intpol._interlacing;
    _gaussPresence = intpol._gaussPresence;

    return *this;
  }

  inline int getDim()       const { return _dim; }
  inline int getNbElem()    const { return _nbelem; }
  inline int getArraySize() const { return _arraySize; }
  inline MED_EN::medModeSwitch getInterlacingType() const {return _interlacing;}
  inline bool getGaussPresence() const { return _gaussPresence;}
  virtual int getNbGauss(int i) const = 0;

  int _dim;
  int _nbelem;
  int _arraySize;
  MED_EN::medModeSwitch _interlacing;
  bool _gaussPresence;
};


class FullInterlaceNoGaussPolicy : public  InterlacingPolicy {

protected:
  ~FullInterlaceNoGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique

public :
  FullInterlaceNoGaussPolicy() : InterlacingPolicy() {}
  FullInterlaceNoGaussPolicy(int nbelem, int dim) :
    InterlacingPolicy(nbelem, dim, dim*nbelem,MED_EN::MED_FULL_INTERLACE) {}

  FullInterlaceNoGaussPolicy(const FullInterlaceNoGaussPolicy & policy,
				bool shallowcopie=true)
    : InterlacingPolicy(policy) {};
 
  inline int getIndex(int i,int j) const {
    return (i-1)*_dim + j-1;
  }

  inline int getIndex(int i,int j,int k) const {
    return (i-1)*_dim + j-1;
  }

  inline int getNbGauss(int i) const { return 1; }

};

class NoInterlaceNoGaussPolicy : public InterlacingPolicy {

protected:
  ~NoInterlaceNoGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique

public :

  NoInterlaceNoGaussPolicy():InterlacingPolicy() {}
  NoInterlaceNoGaussPolicy(int nbelem, int dim) : 
    InterlacingPolicy(nbelem, dim, dim*nbelem,MED_EN::MED_NO_INTERLACE) {}

  NoInterlaceNoGaussPolicy(const NoInterlaceNoGaussPolicy & policy,
			      bool shallowcopie=true)
    : InterlacingPolicy(policy) {}

  inline int getIndex(int i,int j) const {
    return (j-1)*_nbelem + i-1;
  }

  inline int getIndex(int i,int j,int k) const {
    return (j-1)*_nbelem + i-1;
  }

  inline int getNbGauss(int i) const { return 1; }

};

class FullInterlaceGaussPolicy : public InterlacingPolicy {
protected:
  ~FullInterlaceGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
public :

  PointerOf<int> _G;
  PointerOf<int> _S;
  int _nbtypegeo;
  PointerOf<int> _nbelegeoc;
  PointerOf<int> _nbgaussgeo;

  FullInterlaceGaussPolicy():InterlacingPolicy(),_nbtypegeo(-1) {
    InterlacingPolicy::_gaussPresence=true;
  }
  FullInterlaceGaussPolicy(int nbelem, int dim, int nbtypegeo,
		  const int * const nbelgeoc, const int * const nbgaussgeo)
    : InterlacingPolicy(nbelem, dim, -1, MED_EN::MED_FULL_INTERLACE),_nbtypegeo(nbtypegeo) {

    InterlacingPolicy::_gaussPresence=true;

    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _nbgaussgeo.set(_nbtypegeo+1,nbgaussgeo);
    _G.set(nbelem+1);
    // _G[0]       = 1;
    _S.set(nbelem+1);
    _S[0]       = -1;
    int cumul   = 0;
    int elemno  = 0;

    // Construction of _G
    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
	_G[ elemno ] = cumul + i*nbgaussgeo[ntyp]*dim + 1;
	elemno++;
	_S[ elemno ] = nbgaussgeo[ntyp];
      };
      cumul += (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) * nbgaussgeo[ntyp] * dim;
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de cumul " << cumul << std::endl;
#endif
    };

    _G[ elemno ] = cumul+1;
    _arraySize   = _G[ elemno ] -1 ;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _G["<<i<<"] = "<<_G[i] << std::endl;
#endif
  }

  FullInterlaceGaussPolicy(const FullInterlaceGaussPolicy & policy,
			      bool shallowcopie=true)
    : InterlacingPolicy(policy),_nbtypegeo(policy._nbtypegeo) {

    //Seuls les tableaux de grande taille sont recopiés superficiellement
    if(shallowcopie) {
      this->_G.set(policy._G);
      this->_S.set(policy._S);
    } else {
      this->_G.set(_nbelem+1,policy._G);
      this->_S.set(_nbelem+1,policy._S);
    }
    // Tableaux toujours recopiés par recopie profonde
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
  }

  FullInterlaceGaussPolicy & operator=(const FullInterlaceGaussPolicy & policy) {
    BEGIN_OF("FullInterlaceGaussPolicy operator =");

    if ( this == &policy) return *this;

    //Seuls les tableaux de grande taille sont recopiés superficiellement
    InterlacingPolicy::operator=(policy);
    this->_G.set(policy._G);
    this->_S.set(policy._S);

    // Tableaux toujours recopiés par recopie profonde
    this->_nbtypegeo=policy._nbtypegeo;
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);

    return *this;
  }

  inline int getIndex(int i,int j ) const {
    return _G[i-1]-1 + (j-1)*_dim ;
  }

  inline int getIndex(int i,int j, int k ) const {
    //std::cout << "Index : " << _G[i-1]-1 + _dim *(k-1) + (j-1) << std::endl;
    return _G[i-1]-1 +  (k-1)*_dim + (j-1);
  }

  inline int getNbGauss(int i) const { return _S[i]; }

  inline int getNbGeoType() const {return _nbtypegeo;}

  inline const int * const getNbElemGeoC() const {return _nbelegeoc;}

  inline const int * const getNbGaussGeo() const {return _nbgaussgeo;}


};

class NoInterlaceGaussPolicy : public InterlacingPolicy {

protected:
  ~NoInterlaceGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique

public :

  PointerOf<int> _G;
  PointerOf<int> _S;
  int _nbtypegeo;
  PointerOf<int> _nbelegeoc;
  PointerOf<int> _nbgaussgeo;
  // _cumul is used in getIndex() to access directly to the first value
  // of a given dimension.
  int _cumul;

  NoInterlaceGaussPolicy():InterlacingPolicy(),_nbtypegeo(-1),_cumul(0) {
    InterlacingPolicy::_gaussPresence=true;
  }

  NoInterlaceGaussPolicy(int nbelem, int dim, int nbtypegeo,
			 const int * const nbelgeoc, const int * const nbgaussgeo)
    : InterlacingPolicy(nbelem, dim, -1, MED_EN::MED_NO_INTERLACE),_nbtypegeo(nbtypegeo) {

    InterlacingPolicy::_gaussPresence=true;

    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _nbgaussgeo.set(_nbtypegeo+1,nbgaussgeo);
    _G.set(nbelem+1);
    //_G[0]       = 1;
    _S.set(nbelem+1);
    _S[0] = -1;
    int elemno = 0;

    _cumul = 0;
    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
	_G[ elemno ] = _cumul + i*nbgaussgeo[ntyp] + 1;
	elemno++;
	_S[ elemno ] = nbgaussgeo[ntyp];
      };
      _cumul += (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) * nbgaussgeo[ntyp];
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de _cumul " << _cumul << std::endl;
#endif
    };

    _G[ elemno ] = _cumul+1;
    _arraySize   = ( _G[ elemno ] -1 ) * dim ;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _G["<<i<<"] = "<<_G[i] << std::endl;
#endif
  }


  NoInterlaceGaussPolicy(const NoInterlaceGaussPolicy & policy,
			    bool shallowcopie=true)
    : InterlacingPolicy(policy),_cumul(policy._cumul),_nbtypegeo(policy._nbtypegeo)
  {
    //Seuls les tableaux de grande taille sont recopiés superficiellement
    if(shallowcopie) {
      this->_G.set(policy._G);
      this->_S.set(policy._S);
    } else {
      this->_G.set(_nbelem+1,policy._G);
      this->_S.set(_nbelem+1,policy._S);
    }
    // Tableaux toujours recopiés par recopie profonde
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
  }

  NoInterlaceGaussPolicy & operator=(const NoInterlaceGaussPolicy & policy) {
    if ( this == &policy) return *this;

    BEGIN_OF("NoInterlaceGaussPolicy operator =");
    InterlacingPolicy::operator=(policy);
    this->_G.set(policy._G);
    this->_S.set(policy._S);

    this->_cumul = policy._cumul;

    // Tableaux toujours recopiés par recopie profonde
    this->_nbtypegeo=policy._nbtypegeo;
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);

    return *this;
  }

  inline int getIndex(int i,int j ) const {
    return _G[i-1]-1 + (j-1)*_cumul ;
  }

  inline int getIndex(int i,int j, int k ) const {
    return _G[i-1]-1 + (j-1)*_cumul + (k-1) ;
  }

  inline int getNbGauss(int i) const { return _S[i]; }

  inline int getNbGeoType() const {return _nbtypegeo;}

  inline const int * const getNbElemGeoC() const {return _nbelegeoc;}

  inline const int * const getNbGaussGeo() const {return _nbgaussgeo;}

};

} //END NAMESPACE

#endif
