// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#ifndef _MED_PRESENTATION_MANAGER_I_HXX_
#define _MED_PRESENTATION_MANAGER_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDPresentationManager)
#include "SALOME_GenericObj_i.hh"

#include "MEDDataManager_i.hxx"
#include "MEDCALC.hxx"

#include <vector>
#include <string>

typedef ::CORBA::Long TypeID;

class MEDPresentation;

class MEDPresentationManager_i: public POA_MEDCALC::MEDPresentationManager,
                                               public SALOME::GenericObj_i
{
 public:

  static MEDPresentationManager_i* getInstance();

  MEDCALC_EXPORT TypeID makeScalarMap(const MEDCALC::ScalarMapParameters&);
  MEDCALC_EXPORT TypeID makeContour(const MEDCALC::ContourParameters&);
  MEDCALC_EXPORT TypeID makeVectorField(const MEDCALC::VectorFieldParameters&);
  MEDCALC_EXPORT TypeID makeSlices(const MEDCALC::SlicesParameters&);
  MEDCALC_EXPORT TypeID makeDeflectionShape(const MEDCALC::DeflectionShapeParameters&);
  MEDCALC_EXPORT TypeID makePointSprite(const MEDCALC::PointSpriteParameters&);

  MEDCALC_EXPORT void setPresentationProperty(TypeID presentationID, const char * propName, const char * propValue);
  MEDCALC_EXPORT char* getPresentationProperty(TypeID presentationID, const char* propName);

 private:
  MEDPresentationManager_i();
  virtual ~MEDPresentationManager_i();

 private :

  // The MEDPresentationManager is a singleton, whose instance can be obtained
  // using the getInstance static method.
  static MEDPresentationManager_i * _instance;
  static TypeID GenerateID();

  // Owns a list of MEDPresentation objects
  std::map< TypeID, MEDPresentation * > _presentations;

};

#endif // _MED_PRESENTATION_MANAGER_I_HXX_
