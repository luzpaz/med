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
//  File   : Field_i.hxx
//  Author : EDF
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Field_i.hxx

# ifndef __FIELD_I_H__
# define __FIELD_I_H__

# include <SALOMEconfig.h>

//# include "SALOME_Component_i.hxx"
# include CORBA_SERVER_HEADER(MED)

# include "Utils_CorbaException.hxx"

class FIELD_i
{

protected :
        FIELD_i();

public :
        FIELD_i( const FIELD_i & x);
        ~FIELD_i();
        virtual char *           getName()
				     throw (SALOME::SALOME_Exception) = 0;
        virtual char *           getDescription()
		 		     throw (SALOME::SALOME_Exception) = 0;
	virtual SALOME_MED::SUPPORT_ptr getSupport()
				     throw (SALOME::SALOME_Exception) = 0;
        virtual CORBA::Long      getNumberOfComponents()
		 	  	     throw (SALOME::SALOME_Exception) = 0;
        virtual char *           getComponentName(CORBA::Long i)
		  		     throw (SALOME::SALOME_Exception) = 0;
        virtual char *           getComponentUnit(CORBA::Long i)
		  		     throw (SALOME::SALOME_Exception) = 0;
        virtual CORBA::Long      getIterationNumber()
			 	     throw (SALOME::SALOME_Exception) = 0;
        virtual CORBA::Long      getOrderNumber()
			 	     throw (SALOME::SALOME_Exception) = 0;
        virtual CORBA::Double    getTime()
			 	     throw (SALOME::SALOME_Exception) = 0;
	virtual CORBA::Long      getCorbaIndex()
			 	     throw (SALOME::SALOME_Exception) = 0;
        virtual Engines::string_array * getComponentsNames()
		  		     throw (SALOME::SALOME_Exception) = 0;
        virtual Engines::string_array * getComponentsUnits()
		 		     throw (SALOME::SALOME_Exception) = 0;
        virtual void             addInStudy(SALOMEDS::Study_ptr myStudy , 
					    SALOME_MED::FIELD_ptr myIor)
		 		     throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection) = 0;
        virtual CORBA::Long      addDriver (SALOME_MED::medDriverTypes driverType, 
					    const char* fileName, 
					    const char* fieldName)
		 		     throw (SALOME::SALOME_Exception) = 0;
	virtual void		 rmDriver  (CORBA::Long i)
		 		     throw (SALOME::SALOME_Exception) = 0;
	virtual void		 read      (CORBA::Long i)
		 		     throw (SALOME::SALOME_Exception) = 0;
	virtual void		 write     (CORBA::Long i,
					    const char* driverFieldName)
		 		     throw (SALOME::SALOME_Exception) = 0;
};
# endif /* ifndef ____FIELD_I_H__ */
