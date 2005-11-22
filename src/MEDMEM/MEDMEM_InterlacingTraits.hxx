#ifndef MEDMEM_INTERLACING_TRAITS_HXX
#define MEDMEM_INTERLACING_TRAITS_HXX

#include "MEDMEM_Tags.hxx"

namespace MEDMEM {

struct __ERROR_INTERLACING_GAUSS_TRAITS_POLICY_INSTANCIATED_WITH_INCORRECT_TYPE {};

template <  class INTERLACE_TAG, class GAUSS_TAG>
struct MEDMEM_InterlacingTraits
{
  typedef  __ERROR_INTERLACING_GAUSS_TRAITS_POLICY_INSTANCIATED_WITH_INCORRECT_TYPE Type;
};

template <>
struct MEDMEM_InterlacingTraits< FullInterlace, Gauss >
{
  typedef FullInterlaceGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< FullInterlace, NoGauss >
{
  typedef FullInterlaceNoGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< NoInterlace, Gauss >
{
  typedef NoInterlaceGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< NoInterlace, NoGauss >
{
  typedef NoInterlaceNoGaussPolicy Type;
};

} // END NAMESPACE

#endif
