//=============================================================================
// File      : MEDMEM_convert.hxx
// Created   : mer fév 20 15:47:57 CET 2002
// Author    : EDF
// Project   : SALOME
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_convert.hxx
//=============================================================================
# if ! defined ( __CONVERT_H__ )
# define __CONVERT_H__

#include <SALOMEconfig.h> 
#include CORBA_SERVER_HEADER(MED)
#include "MEDMEM_define.hxx"

using namespace MED_EN;

#include "MEDMEM_GenDriver.hxx"

SALOME_MED::medGeometryElement convertMedEltToIdlElt(medGeometryElement element) 
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medEntityMesh      convertMedEntToIdlEnt(medEntityMesh entity) 
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medModeSwitch      convertMedModeToIdlMode(medModeSwitch mode) 
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medDriverTypes     convertMedDriverToIdlDriver(MEDMEM::driverTypes driverType)
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medConnectivity    convertMedConnToIdlConn(medConnectivity connectivite)
			       throw (SALOME::SALOME_Exception);

medGeometryElement convertIdlEltToMedElt (SALOME_MED::medGeometryElement element) 
		      throw (SALOME::SALOME_Exception);
medEntityMesh   convertIdlEntToMedEnt(SALOME_MED::medEntityMesh entity) 
		      throw (SALOME::SALOME_Exception);
medModeSwitch       convertIdlModeToMedMode(SALOME_MED::medModeSwitch mode) 
		      throw (SALOME::SALOME_Exception);
MEDMEM::driverTypes 	      convertIdlDriverToMedDriver(SALOME_MED::medDriverTypes driverType)
		      throw (SALOME::SALOME_Exception);
medConnectivity      convertIdlConnToMedConn(SALOME_MED::medConnectivity connectivite)
		      throw (SALOME::SALOME_Exception);


bool verifieParam (SALOME_MED::medEntityMesh entity, SALOME_MED::medGeometryElement geomElement)
     throw (SALOME::SALOME_Exception);

# endif 	/* # if ! defined ( __CONVERT_H__ ) */
