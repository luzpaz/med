//=============================================================================
// File      : Field_i.hxx
// Created   : mer fév 20 15:47:57 CET 2002
// Author    : EDF
// Project   : SALOME
// Copyright : EDF 2002
// $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Field_i.hxx
//=============================================================================
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
		 		     throw (SALOME::SALOME_Exception) = 0;
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
