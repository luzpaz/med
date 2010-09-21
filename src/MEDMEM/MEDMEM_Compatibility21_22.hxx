//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

/*#ifdef med_2_3_NAMESPACE

{
#endif //med_2_3_NAMESPACE*/
namespace med_2_3 {
  extern "C" { 
    #include <med.h>
    #include <med_proto.h>
  }
}
//  }
//#ifdef med_2_3_NAMESPACE

//#endif //med_2_3_NAMESPACE

/*
  Core of the management V2.1 V2.2 of the Med File
*/

#undef MED_TAILLE_PNOM
#undef MED_NOPFLi

#define MED_TAILLE_PNOM21   8
#define MED_TAILLE_PNOM22  16

//#include "MEDMEM_define.hxx"
#include "MEDMEM_MedVersion.hxx"

namespace MEDMEM {

  MEDMEM_EXPORT int getMedAccessMode(MED_EN::med_mode_acces mode,
                       MED_EN::medFileVersion medVersion)
    throw (MEDEXCEPTION);

}

#endif /* COMPATIBILITY_21_22_HXX */