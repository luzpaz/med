//  MED MedMem : MED idl descriptions implementation based on the classes of MEDMEM
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : Support_i.hxx
//  Author : EDF 
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Support_i.hxx

#ifndef _MED_SUPPORT_I_HXX_
#define _MED_SUPPORT_I_HXX_

#include <map>
#include <string>

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(MED)

class SUPPORT;
class SALOME_MED::MESH;

class SUPPORT_i: 
		public POA_SALOME_MED::SUPPORT,
                public PortableServer::RefCountServantBase 
{
public :
    static map < int,::SUPPORT *> supportMap;
private :
    static int supportIndex;

protected :

     // C++ object containing values
     const ::SUPPORT * const _support;
     const int   _corbaIndex ;
     string      _supportId ;

public:

    SUPPORT_i();
    ~SUPPORT_i();

    // Constructors and associated internal methods
    SUPPORT_i(const ::SUPPORT * const s);
    SUPPORT_i(const SUPPORT_i & s);

    // IDL Methods
    char *               getName() 	   throw (SALOME::SALOME_Exception);
    char *               getDescription()  throw (SALOME::SALOME_Exception);
    SALOME_MED::MESH_ptr getMesh() 	   throw (SALOME::SALOME_Exception);
    CORBA::Boolean       isOnAllElements() throw (SALOME::SALOME_Exception);
    SALOME_MED::medEntityMesh getEntity()  throw (SALOME::SALOME_Exception);
    CORBA::Long    	 getNumberOfElements(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    Engines::long_array* getNumber(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    Engines::long_array* getNumberIndex() 
					   throw (SALOME::SALOME_Exception);
    CORBA::Long          getNumberOfGaussPoints(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    SALOME_MED::medGeometryElement_array* getTypes() 
					   throw (SALOME::SALOME_Exception);

  // Others
  void           addInStudy (SALOMEDS::Study_ptr myStudy, 
			  SALOME_MED::SUPPORT_ptr myIor)
    throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);
  
  //					Cuisine interne
  CORBA::Long 	 getCorbaIndex()   throw (SALOME::SALOME_Exception);
};

#endif /* _MED_SUPPORT_I_HXX_ */
