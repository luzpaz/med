// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

#include <MEDMEM.hxx>

#include <iostream>
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {

class MEDMEM_EXPORT InterlacingPolicy {
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
  const char* LOC = "InterlacingPolicy operator =";
  BEGIN_OF_MED(LOC);

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


class MEDMEM_EXPORT FullInterlaceNoGaussPolicy : public  InterlacingPolicy {

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

class MEDMEM_EXPORT NoInterlaceNoGaussPolicy : public InterlacingPolicy {

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

class MEDMEM_EXPORT NoInterlaceByTypeNoGaussPolicy : public InterlacingPolicy {

protected:
  ~NoInterlaceByTypeNoGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique

  PointerOf<int> _T; //!< type of element
  PointerOf<int> _G; //!< where type begin
  int _nbtypegeo;
  PointerOf<int> _nbelegeoc;

public :

  NoInterlaceByTypeNoGaussPolicy():InterlacingPolicy(),_nbtypegeo(-1)
  {
  }

  NoInterlaceByTypeNoGaussPolicy(int nbelem, int dim):InterlacingPolicy()
  {
    // constructor is incoorect for this type of interlace
    throw MEDEXCEPTION(LOCALIZED("Wrong constructor of NoInterlaceByTypeNoGaussPolicy "));
  }

  NoInterlaceByTypeNoGaussPolicy(int nbelem, int dim, int nbtypegeo,
                                 const int * const nbelgeoc) : 
    InterlacingPolicy(nbelem, dim, dim*nbelem,MED_EN::MED_NO_INTERLACE_BY_TYPE),
    _nbtypegeo(nbtypegeo)
  {
    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _G.set(nbtypegeo+1);
    _T.set(nbelem+1);
    int elemno = 1;
    int cumul = 0;

    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      int nbelcurtype = nbelgeoc[ntyp]-nbelgeoc[ntyp-1];
      for (int i=0; i < nbelcurtype; i++ ) {
        _T[ elemno ] = ntyp;
        elemno++;
      };
      _G[ ntyp ] = cumul;
      cumul += nbelcurtype*_dim;
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de cumul " << cumul << std::endl;
#endif
    };

    _arraySize = cumul;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _T["<<i<<"] = "<<_T[i] << std::endl;
#endif
  }

  NoInterlaceByTypeNoGaussPolicy(const NoInterlaceByTypeNoGaussPolicy & policy,
                                 bool shallowcopie=true)
    : InterlacingPolicy(policy),_nbtypegeo(policy._nbtypegeo)
  {
    //Seuls les tableaux de grande taille sont recopiés superficiellement
    if(shallowcopie)
    {
      this->_G.set(policy._G);
      this->_T.set(policy._T);
    }
    else
    {
      this->_G.set(_nbtypegeo+1,policy._G);
      this->_T.set(_nbelem+1,policy._T);
    }
    
    // Tableaux toujours recopiés par recopie profonde
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
  }

  NoInterlaceByTypeNoGaussPolicy & operator=(const NoInterlaceByTypeNoGaussPolicy & policy) {
    if ( this == &policy) return *this;

  const char* LOC = "NoInterlaceNoGaussPolicy operator =";
  BEGIN_OF_MED(LOC);
    InterlacingPolicy::operator=(policy);
    this->_G.set(policy._G);
    this->_T.set(policy._T);

    // Tableaux toujours recopiés par recopie profonde
    this->_nbtypegeo=policy._nbtypegeo;
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);

    return *this;
  }
  inline int getIndex(int t) const {
    return _G[t];
  }

  inline int getIndex(int i,int j) const {
    int t = _T[i];
    return getIndexByType( i-(_nbelegeoc[t-1]-_nbelegeoc[0]), j, t );
  }

  inline int getIndex(int i,int j,int k) const {
    return getIndex(i,j);
  }

  inline int getIndexByType(int i,int j,int t) const {
    return _G[t] + i-1 + (j-1)*(_nbelegeoc[t]-_nbelegeoc[t-1]);
  }

  inline int getIndexByType(int i,int j,int k,int t) const {
    return getIndexByType( i, j, t );
  }

  inline int getLengthOfType(int t) const {
    return (_nbelegeoc[t]-_nbelegeoc[t-1]) * _dim;
  }

  inline int getNbGauss(int i) const { return 1; }

  inline int getNbGeoType() const {return _nbtypegeo;}

  inline const int * const getNbElemGeoC() const {return _nbelegeoc;}

};

class MEDMEM_EXPORT FullInterlaceGaussPolicy : public InterlacingPolicy {
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
  const char* LOC = "FullInterlaceGaussPolicy operator =";
  BEGIN_OF_MED(LOC);

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
    return _G[i-1]-1 + (j-1);
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

class MEDMEM_EXPORT NoInterlaceGaussPolicy : public InterlacingPolicy {

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
    : InterlacingPolicy(policy),_nbtypegeo(policy._nbtypegeo),_cumul(policy._cumul)
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

  const char* LOC = "NoInterlaceGaussPolicy operator =";
  BEGIN_OF_MED(LOC);
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

class MEDMEM_EXPORT NoInterlaceByTypeGaussPolicy : public InterlacingPolicy {

protected:
  ~NoInterlaceByTypeGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique

  PointerOf<int> _T; //!< type of element
  PointerOf<int> _G; //!< where type begin
  int _nbtypegeo;
  PointerOf<int> _nbelegeoc;
  PointerOf<int> _nbgaussgeo;

public :

  NoInterlaceByTypeGaussPolicy():InterlacingPolicy(),_nbtypegeo(-1) {
    InterlacingPolicy::_gaussPresence=true;
  }

  NoInterlaceByTypeGaussPolicy(int nbelem, int dim, int nbtypegeo,
                               const int * const nbelgeoc, const int * const nbgaussgeo)
    : InterlacingPolicy(nbelem, dim, -1, MED_EN::MED_NO_INTERLACE_BY_TYPE),_nbtypegeo(nbtypegeo) {

    InterlacingPolicy::_gaussPresence=true;

    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _nbgaussgeo.set(_nbtypegeo+1,nbgaussgeo);
    _G.set(_nbtypegeo+1);
    _T.set(nbelem+1);
    int elemno = 1;
    int cumul = 0;

    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      int nbelcurtype = nbelgeoc[ntyp]-nbelgeoc[ntyp-1];
      for (int i=0; i < nbelcurtype; i++ ) {
        _T[ elemno ] = ntyp;
        elemno++;
      };
      _G[ ntyp ] = cumul;
      cumul += nbelcurtype * _dim * nbgaussgeo[ntyp];
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de cumul " << cumul << std::endl;
#endif
    };

    _arraySize = cumul;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _T["<<i<<"] = "<<_T[i] << std::endl;
#endif
  }


  NoInterlaceByTypeGaussPolicy(const NoInterlaceByTypeGaussPolicy & policy,
                               bool shallowcopie=true)
    : InterlacingPolicy(policy),_nbtypegeo(policy._nbtypegeo)
  {
    //Seuls les tableaux de grande taille sont recopiés superficiellement
    if(shallowcopie) {
      this->_G.set(policy._G);
      this->_T.set(policy._T);
    } else {
      this->_G.set(_nbtypegeo+1,policy._G);
      this->_T.set(_nbelem+1,policy._T);
    }
    // Tableaux toujours recopiés par recopie profonde
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
  }

  NoInterlaceByTypeGaussPolicy & operator=(const NoInterlaceByTypeGaussPolicy & policy) {
    if ( this == &policy) return *this;

  const char* LOC = "NoInterlaceGaussPolicy operator =";
  BEGIN_OF_MED(LOC);
    InterlacingPolicy::operator=(policy);
    this->_G.set(policy._G);
    this->_T.set(policy._T);

    // Tableaux toujours recopiés par recopie profonde
    this->_nbtypegeo=policy._nbtypegeo;
    this->_nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);

    return *this;
  }

  inline int getIndex(int t) const {
    return _G[t];
  }
  inline int getIndex(int i,int j ) const {
    int t = _T[i];
    return getIndexByType( i-(_nbelegeoc[t-1]-_nbelegeoc[0]), j, t );
  }

  inline int getIndex(int i,int j, int k ) const {
    return getIndex( i, j ) + (k-1);
  }

  inline int getIndexByType(int i,int j,int t) const {
    return _G[t] + (i-1 + (j-1)*(_nbelegeoc[t]-_nbelegeoc[t-1])) * _nbgaussgeo[t];
  }

  inline int getIndexByType(int i,int j,int k,int t) const {
    return getIndexByType( i,j,t ) + (k-1);
  }

  inline int getNbGauss(int i) const { return _nbgaussgeo[ _T[i] ]; }

  inline int getNbGaussByType(int t) const { return _nbgaussgeo[ t ]; }

  inline int getNbGeoType() const {return _nbtypegeo;}

  inline const int * const getNbElemGeoC() const {return _nbelegeoc;}

  inline const int * const getNbGaussGeo() const {return _nbgaussgeo;}

  inline int getLengthOfType(int t) const {
    return (_nbelegeoc[t]-_nbelegeoc[t-1]) * _dim * _nbgaussgeo[t];
  }

};

} //END NAMESPACE

#endif
