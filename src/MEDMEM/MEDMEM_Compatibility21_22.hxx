/*
  Include file managing the compatibility
  Med File V 2.1 and V 2.2
*/

#ifndef COMPATIBILITY_21_22_HXX
#define COMPATIBILITY_21_22_HXX

/*
  Med File V 2.1 attributes
*/

#undef MED_H
#undef MED_PROTO_H

# include <med.hxx>
# include <med_proto.hxx>

#undef MED_TAILLE_PNOM
#undef MED_NOPFLi

/*
  Med File V 2.2 attributes
*/

#undef MED_H
#undef MED_PROTO_H

namespace med_2_2 {
  extern "C" { 
#include <med.h>
#include <med_proto.h>
  }
}


#define MED_TAILLE_PNOM21   8
#define MED_TAILLE_PNOM22  16

#endif /* COMPATIBILITY_21_22_HXX */
