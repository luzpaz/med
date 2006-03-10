#ifndef MEDMEM_SET_INTERLACING_TYPE
#define MEDMEM_SET_INTERLACING_TYPE

#include "MEDMEM_Tags.hxx"
using namespace MEDMEM;

namespace {

  template < typename T > struct SET_INTERLACING_TYPE {
    static const MED_EN::medModeSwitch _interlacingType = MED_EN::MED_UNDEFINED_INTERLACE; };
  template < > struct SET_INTERLACING_TYPE<FullInterlace>{
    static const MED_EN::medModeSwitch _interlacingType = MED_EN::MED_FULL_INTERLACE; };
  template < > struct SET_INTERLACING_TYPE<NoInterlace> {
    static const MED_EN::medModeSwitch _interlacingType = MED_EN::MED_NO_INTERLACE; };

}
#endif
