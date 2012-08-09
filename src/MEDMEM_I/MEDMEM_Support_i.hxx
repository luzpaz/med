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

//=============================================================================
// File      : MEDMEM_Support_i.hxx
// Project   : SALOME
// Author    : EDF 
// $Header: /export/home/PAL/MED/src/MEDMEM_I/MEDMEM_Support_i.hxx
//=============================================================================
//
#ifndef _MED_SUPPORT_I_HXX_
#define _MED_SUPPORT_I_HXX_

#include "MEDMEM_I.hxx"

#include <map>
#include <string>

#include <SALOMEconfig.h>

#include "SALOMEMultiComm.hxx"
#include "SALOME_GenericObj_i.hh"
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOME_Comm)

namespace MEDMEM {
class SUPPORT;

class MEDMEM_I_EXPORT SUPPORT_i: virtual public POA_SALOME_MED::SUPPORT,
                   virtual public SALOMEMultiComm,
                   virtual public SALOME::GenericObj_i
{
public :
    static std::map < int,::MEDMEM::SUPPORT *> supportMap;
private :
    static int supportIndex;

protected :

     // C++ object containing values
     const ::MEDMEM::SUPPORT * const _support;
     const int   _corbaIndex ;
     std::string      _supportId ;

public:

    SUPPORT_i();
    ~SUPPORT_i();

    // Constructors and associated internal methods
    SUPPORT_i(const ::MEDMEM::SUPPORT * const s);
    SUPPORT_i(const SUPPORT_i & s);

    // IDL Methods
    char *               getName()         throw (SALOME::SALOME_Exception);
    char *               getDescription()  throw (SALOME::SALOME_Exception);
    SALOME_MED::GMESH_ptr getMesh()         throw (SALOME::SALOME_Exception);
    CORBA::Boolean       isOnAllElements() throw (SALOME::SALOME_Exception);
    SALOME_MED::medEntityMesh getEntity()  throw (SALOME::SALOME_Exception);
    CORBA::Long          getNumberOfElements(SALOME_MED::medGeometryElement geomElement) 
                                           throw (SALOME::SALOME_Exception);
    CORBA::Long          getNumberOfTypes()
                                           throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfLong* getNumber(SALOME_MED::medGeometryElement geomElement) 
                                           throw (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfLong* getNumberFromFile(SALOME_MED::medGeometryElement geomElement) 
                                           throw (SALOME::SALOME_Exception);

    SALOME::SenderInt_ptr getSenderForNumber(SALOME_MED::medGeometryElement geomElement) 
                                           throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfLong* getNumberIndex() 
                                           throw (SALOME::SALOME_Exception);
    SALOME::SenderInt_ptr getSenderForNumberIndex() 
                                           throw (SALOME::SALOME_Exception);
    CORBA::Long          getNumberOfGaussPoint(SALOME_MED::medGeometryElement geomElement) 
                                           throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfLong* getNumbersOfGaussPoint()
                                           throw (SALOME::SALOME_Exception);

    SALOME_MED::medGeometryElement_array* getTypes() 
                                           throw (SALOME::SALOME_Exception);

    void getBoundaryElements()             throw (SALOME::SALOME_Exception);


  // Others
  void           addInStudy (SALOMEDS::Study_ptr myStudy, 
                          SALOME_MED::SUPPORT_ptr myIor)
    throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);

  static std::string getEntryPath (const MEDMEM::SUPPORT * aSupport);

  void release();
  //                                    Cuisine interne
  CORBA::Long    getCorbaIndex()   throw (SALOME::SALOME_Exception);
  SALOME_MED::SUPPORT::supportInfos * getSupportGlobal()   throw (SALOME::SALOME_Exception);
};
}

#endif /* _MED_SUPPORT_I_HXX_ */
