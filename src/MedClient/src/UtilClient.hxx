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

#ifndef UTILCLIENT_HXX_
#define UTILCLIENT_HXX_

#include <vector>
#include <utilities.h>
#include "MEDMEM_PointerOf.hxx"

namespace MEDMEM {
template <typename TLocal, 
          typename TCorbaSeq,
          typename Tint>
inline void convertCorbaArray (TLocal * & T, Tint &nT, const TCorbaSeq & S)
{
  Tint i, n = S->length();

  nT = n;
  T = n > 0 ? new TLocal[n] : NULL;

  for (i=0; i<n; i++) {
    T[i] = (*S)[i];
  }
}

template <typename TLocal, 
          typename TCorbaSeq,
          typename Tint>
inline void convertCorbaArray2 (TLocal& tab, Tint &nT, const TCorbaSeq s)
{
  Tint i, n = s.length();

  nT = n;
  tab.set(n);
  for (i=0; i<n; i++) {
    tab[i] = s[i];
  }
}

template <typename TCorba,
          typename TLocal,
          typename TCorbaSeq>
inline void convertCorbaArray (TLocal * & T, long &nT, const TCorbaSeq & S,
                               void *f)
{
  int i, n = S->length();
  SCRUTE(n);

  nT = n;
  T = n > 0 ? new TLocal[n] : NULL;

  typedef TLocal (*pfn) (const TCorba &T);

  pfn convert = pfn(f);
  for (i=0; i<n; i++) {
    SCRUTE((*S)[i]);
    T[i] = convert((*S)[i]);
    SCRUTE(T[i]);
  }
}

template <typename TLocal,
          typename TCorbaSeq>
inline void convertCorbaArray (std::vector<TLocal> & T, int &nT, 
                               const TCorbaSeq & S)
{
  int i, n = S->length();
  SCRUTE(n);

  nT = n;
  T.resize(nT);

  for (i=0; i<nT; i++) {
    SCRUTE((*S)[i]);
    T[i] = convert((*S)[i]);
    SCRUTE(T[i]);
  }
}


template <typename TCorba,
          typename TLocal,
          typename TCorbaSeq>
inline void convertCorbaArray (std::vector<TLocal> & T, int &nT, 
                               const TCorbaSeq & S,
                               void *f)
{
  int i, n = S->length();
  SCRUTE(n);

  nT = n;
  T.resize(nT);

  typedef TLocal (*pfn) (const TCorba &T);

  pfn convert = pfn(f);
  for (i=0; i<nT; i++) {
    SCRUTE((*S)[i]);
    T[i] = convert((*S)[i]);
    SCRUTE(T[i]);
  }
 
}


template <typename TCorba,
          typename TLocal,
          typename TCorbaSeq,
          typename TInfo>
inline void convertCorbaArray (std::vector<TLocal> & T, 
                               const TCorbaSeq & S,
                               void *f, TInfo M)
{
  int i, n = S->length();
  SCRUTE(n);

  int nT = n;
  T.resize(nT);

  typedef TLocal (*pfn) (const TCorba & , TInfo);

  pfn convert = pfn(f);
  for (i=0; i<nT; i++) {
    SCRUTE((*S)[i]);
    T[i] = convert((*S)[i], M);
    SCRUTE(T[i]);
  }
 
}
}

#endif
