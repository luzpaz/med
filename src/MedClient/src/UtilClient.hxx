#ifndef UTILCLIENT_HXX_
#define UTILCLIENT_HXX_

#include <vector>
#include <utilities.h>

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
inline void convertCorbaArray (vector<TLocal> & T, int &nT, 
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
inline void convertCorbaArray (vector<TLocal> & T, int &nT, 
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
inline void convertCorbaArray (vector<TLocal> & T, int &nT, 
			       const TCorbaSeq & S,
			       void *f, TInfo M)
{
  int i, n = S->length();
  SCRUTE(n);

  nT = n;
  T.resize(nT);

  typedef TLocal (*pfn) (const TCorba & , TInfo);

  pfn convert = pfn(f);
  for (i=0; i<nT; i++) {
    SCRUTE((*S)[i]);
    T[i] = convert((*S)[i], M);
    SCRUTE(T[i]);
  }
 
}

#endif
