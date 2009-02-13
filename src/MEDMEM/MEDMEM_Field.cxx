//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

// ---------------------------------
// FIELD_ : Constructors
// ---------------------------------
FIELD_::FIELD_(): 
  _isRead(false),
  _isMinMax(false),
  _name(""), _description(""), _support((SUPPORT *)NULL),
  _numberOfComponents(0), _numberOfValues(0),
  //_componentsTypes((int *)NULL),
  //_componentsNames((string *)NULL), 
  //_componentsDescriptions((string *)NULL),
  //_componentsUnits((UNIT*)NULL),
  //_MEDComponentsUnits((string *)NULL),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1),
  _valueType(MED_EN::MED_UNDEFINED_TYPE),
  _interlacingType(MED_EN::MED_UNDEFINED_INTERLACE)
{
  MESSAGE_MED("Constructeur FIELD_ sans parametre");
}

FIELD_::FIELD_(const SUPPORT * Support, const int NumberOfComponents):
  _isRead(false),
  _isMinMax(false),
  _name(""), _description(""), _support(Support),
  _numberOfComponents(NumberOfComponents),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1),
  _valueType(MED_EN::MED_UNDEFINED_TYPE),
  _interlacingType(MED_EN::MED_UNDEFINED_INTERLACE)
{
  MESSAGE_MED("FIELD_(const SUPPORT * Support, const int NumberOfComponents)");

  _numberOfValues = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  //_componentsTypes = new int[NumberOfComponents] ;
  //_componentsNames = new string[NumberOfComponents];
  //_componentsDescriptions = new string[NumberOfComponents];
  //_componentsUnits = new UNIT[NumberOfComponents];
  //_MEDComponentsUnits = new string[NumberOfComponents];
  _componentsTypes.resize(NumberOfComponents);
  _componentsNames.resize(NumberOfComponents);
  _componentsDescriptions.resize(NumberOfComponents);
  _componentsUnits.resize(NumberOfComponents);
  _MEDComponentsUnits.resize(NumberOfComponents);
  for(int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }
  if(_support)
    _support->addReference();
}

FIELD_& FIELD_::operator=(const FIELD_ &m) {

  if ( this == &m) return *this;

  _isRead             = m._isRead ;
  _isMinMax           = m._isMinMax ;
  _name               = m._name;
  _description        = m._description;
  _support            = m._support;   //Cf Op�rateur de recopie du Support?
  _numberOfComponents = m._numberOfComponents;
  _numberOfValues     = m._numberOfValues;

  //if (m._componentsTypes != NULL) {
  //  _componentsTypes = new int[m._numberOfComponents] ;
  //  memcpy(_componentsTypes,m._componentsTypes,sizeof(int)*m._numberOfComponents);
  //} else 
  //  _componentsTypes = (int *) NULL;

  _componentsTypes.resize(_numberOfComponents);
  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsTypes[i]=m._componentsTypes[i];}

  _componentsNames.resize(_numberOfComponents);
  _componentsDescriptions.resize(_numberOfComponents);
  _componentsUnits.resize(_numberOfComponents);
  _MEDComponentsUnits.resize(_numberOfComponents);

  //_componentsNames = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsNames[i]=m._componentsNames[i];}
  //_componentsDescriptions = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsDescriptions[i]=m._componentsDescriptions[i];}
  //_componentsUnits = new UNIT[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsUnits[i] = m._componentsUnits[i];}
  // L'operateur '=' est defini dans la classe UNIT
  //_MEDComponentsUnits = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    {_MEDComponentsUnits[i] = m._MEDComponentsUnits[i];}

  _iterationNumber = m._iterationNumber;
  _time            = m._time;
  _orderNumber     = m._orderNumber;

  // _valueType et _interlacingType doivent uniquement �tre recopi�s 
  // par l'op�rateur de recopie de FIELD<T,...>

  //_drivers = m._drivers ; // PG : Well, same driver, what about m destructor !

  return *this;
}

FIELD_::FIELD_(const FIELD_ &m)
{
  _isRead = m._isRead ;
  _isMinMax = m._isMinMax ;
  _name = m._name;
  _description = m._description;
  _support = m._support;
  if(_support)
    _support->addReference();
  _numberOfComponents = m._numberOfComponents;
  _numberOfValues = m._numberOfValues;
  copyGlobalInfo(m);
  //_valueType = m._valueType;
  // _valueType et _interlacingType doivent uniquement �tre recopi�s 
  // par l'op�rateur de recopie de FIELD<T,...>
  //_drivers = m._drivers ; // PG : Well, same driver, what about m destructor !
}

FIELD_::~FIELD_()
{
  MESSAGE_MED("~FIELD_()");
  //if ( _componentsTypes !=NULL)
  //  delete[] _componentsTypes ;
  //if ( _componentsNames !=NULL)
  //  delete[] _componentsNames ;
  //if ( _componentsDescriptions !=NULL)
  //  delete[] _componentsDescriptions ;
  //if ( _componentsUnits !=NULL)
  //  delete[] _componentsUnits ;
  //if ( _MEDComponentsUnits !=NULL)
  //  delete[] _MEDComponentsUnits ;
  // delete driver
//   vector<GENDRIVER *>::const_iterator it ;
//   SCRUTE_MED(_drivers.size());
//   int i=0;
//   for (it=_drivers.begin();it!=_drivers.end();it++) {
//     i++;
//     SCRUTE_MED(i);
//     delete (*it) ;


  MESSAGE_MED("In this object FIELD_ there is(are) " << _drivers.size() << " driver(s)");

  for (unsigned int index=0; index < _drivers.size(); index++ )
    {
      SCRUTE_MED(_drivers[index]);
      if ( _drivers[index] != NULL) delete _drivers[index];
    }
  //CCAR: if _support is a SUPPORTClient remove reference
  // This is correct but it's highlighting other problem
 // if(_support)
 //   _support->removeReference();
}

/*! 
  \if developper
  PROVISOIRE : retourne des volumes, surfaces ou longueurs suivant les cas
  \endif
*/
FIELD<double>* FIELD_::_getFieldSize() const
{
    FIELD<double>* p_field_size;
    switch (getSupport()->getEntity())
    {
	case MED_CELL :
	    switch (getSupport()->getMesh()->getMeshDimension() ) 
	    {
		case 1:
		    p_field_size=getSupport()->getMesh()->getLength(getSupport() );
		    break;
		case 2:
		    p_field_size=getSupport()->getMesh()->getArea(getSupport() );
		    break;
		case 3:
		    p_field_size=getSupport()->getMesh()->getVolume(getSupport() );
		    break;
	    }
	    break;
	    
	case MED_FACE :
	    p_field_size=getSupport()->getMesh()->getArea(getSupport() );
	    break;

	case MED_EDGE :
	    p_field_size=getSupport()->getMesh()->getLength(getSupport() );
	    break;
    }
    return p_field_size;
}


/*! 
  \if developper
  Check up the compatibility of field before computing sobolev norm 
  \endif
*/
void FIELD_::_checkNormCompatibility(const FIELD<double>* support_volume) const throw (MEDEXCEPTION)
{
    string diagnosis;

    if( getSupport()->getEntity() == MED_NODE )
    {
	diagnosis="Cannot compute sobolev norm on a field "+getName()+
	    " : it has support on nodes!";
	throw MEDEXCEPTION(diagnosis.c_str());
    }
	
    if (getNumberOfValues()*getNumberOfComponents()<= 0) // Size of array has to be strictly positive
    {
	diagnosis="Cannot compute the norm of "+getName()+
	    " : it size is non positive!";
	throw MEDEXCEPTION(diagnosis.c_str());
    }

    if( getSupport()->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) != getNumberOfValues() ) {
      diagnosis="Cannot compute Lnorm of "+getName()+
        " : the suppors size not corresponded to number of elements!";
      throw MEDEXCEPTION(diagnosis.c_str());
    }

    if (getGaussPresence() ) {
      diagnosis="Cannot compute Lnorm of "+getName()+
	" : Gauss numbers greater than one are not yet implemented!";
      throw MEDEXCEPTION(diagnosis.c_str());
    }

    if(support_volume) // if the user has supplied the volume
    {
	if(support_volume->getSupport()!=getSupport())
	{
	    diagnosis="Cannot compute Lnorm of "+getName()+
	    " : the volume furnished has not the same support!";
	    throw MEDEXCEPTION(diagnosis.c_str());
	}
	if(support_volume->getNumberOfValues()!=getNumberOfValues())
	{
	    diagnosis="Cannot compute Lnorm of "+getName()+
	    " : the volume furnished has not the same number of values!";
	    throw MEDEXCEPTION(diagnosis.c_str());
	}
        if( getSupport()->getNumberOfElements() != 
            support_volume->getSupport()->getNumberOfElements() ) {
          diagnosis="Cannot compute Lnorm of "+getName()+
            " : the supports have not the same number of elements!";
          throw MEDEXCEPTION(diagnosis.c_str());
        }
    }

}

/*! 
  \if developper
   Check up the compatibility of fields before performing an arithmetic operation
  \endif
*/
void FIELD_::_checkFieldCompatibility(const FIELD_& m, const FIELD_& n, bool checkUnit) throw (MEDEXCEPTION)
{
    string diagnosis;

    // check-up, fill diagnosis if some incompatibility is found.

    // Ne pas v�rifier l'entrelacement
    // Le compilo s'en occupe Rmq from EF

    if(m._support != n._support)
      {
	if(!(*m._support==*n._support))
	  diagnosis+="They don't have the same support!";
      }
    else if(m._numberOfComponents != n._numberOfComponents)
      diagnosis+="They don't have the same number of components!";
    else if (m._valueType != n._valueType)
      diagnosis+="They don't have the same type!";
    else if(m._numberOfValues != n._numberOfValues)
      diagnosis+="They don't have the same number of values!";
    else
      {
	if(checkUnit)
	  {
	    for(int i=0; i<m._numberOfComponents; i++)
	      {
		// Not yet implemented   
		//	    if(m._componentsTypes[i] != n._componentsTypes[i])
		//	    {
		//		diagnosis+="Components don't have the same types!";
		//		break;
		//	    }
		if(m._MEDComponentsUnits[i] != n._MEDComponentsUnits[i])
		  {
		    diagnosis+="Components don't have the same units!";
		    break;
		  }
	      }
	  }
      }

    if(diagnosis.size()) // if fields are not compatible : complete diagnosis and throw exception
    {
	diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
	         + n._name + " are not compatible.\n" + diagnosis;
	throw MEDEXCEPTION(diagnosis.c_str());
    }

    if( m.getNumberOfValues()<=0 || m.getNumberOfComponents()<=0) // check up the size is strictly positive
    {
	diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
	         + n._name + " are empty! (size<=0).\n";
	throw MEDEXCEPTION(diagnosis.c_str());
    }

}

void FIELD_::_deepCheckFieldCompatibility(const FIELD_& m, const FIELD_& n , bool checkUnit ) throw (MEDEXCEPTION)
{
  string diagnosis;

  // check-up, fill diagnosis if some incompatibility is found.

  // Ne pas v�rifier l'entrelacement
  // Le compilo s'en occupe Rmq from EF

    if(m._support != n._support)
      {
	if(!(m._support->deepCompare(*n._support)))
	  diagnosis+="They don't have the same support!";
      }
    else if (m._valueType != n._valueType)
      diagnosis+="They don't have the same type!";
    else if(m._numberOfComponents != n._numberOfComponents)
      diagnosis+="They don't have the same number of components!";
    else if(m._numberOfValues != n._numberOfValues)
      diagnosis+="They don't have the same number of values!";
    else
      {
	if(checkUnit)
	  {
	    for(int i=0; i<m._numberOfComponents; i++)
	      {
		if(m._MEDComponentsUnits[i] != n._MEDComponentsUnits[i])
		  {
		    diagnosis+="Components don't have the same units!";
		    break;
		  }
	      }
	  }
      }

    if(diagnosis.size()) // if fields are not compatible : complete diagnosis and throw exception
    {
	diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
	         + n._name + " are not compatible.\n" + diagnosis;
	throw MEDEXCEPTION(diagnosis.c_str());
    }

    if( m.getNumberOfValues()<=0 || m.getNumberOfComponents()<=0) // check up the size is strictly positive
    {
	diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
	         + n._name + " are empty! (size<=0).\n";
	throw MEDEXCEPTION(diagnosis.c_str());
    }
} 


void     FIELD_::rmDriver      (int index)
{
  MESSAGE_MED("void FIELD_::rmDriver(int index) : removing the driver " << index);
}

int      FIELD_::addDriver     (driverTypes driverType, 
                                const string & fileName,
				const string & driverFieldName,
				MED_EN::med_mode_acces access)
{
  MESSAGE_MED("int FIELD_::addDriver(driverTypes driverType, const string & fileName, const string & driverFieldName) : adding the driver " << driverType << " fileName = " << fileName.c_str() << " driverFieldName = " << driverFieldName.c_str());
  return 0;
}

int      FIELD_::addDriver     (GENDRIVER & driver)
{
  MESSAGE_MED("int FIELD_::addDriver(GENDRIVER & driver) : driver " << driver);
  return 0;
}

void     FIELD_::openAppend    ( void )                               {}
void     FIELD_::write         (const GENDRIVER &)                    {}
void     FIELD_::writeAppend   (const GENDRIVER &)                    {}
void     FIELD_::read          (const GENDRIVER &)                    {}
void     FIELD_::write         (int , const string & ) {}
void     FIELD_::writeAppend   (int , const string & ) {}
void     FIELD_::read          (int )                                  {}
void     FIELD_::copyGlobalInfo(const FIELD_& m)
{  

  _componentsTypes.resize(_numberOfComponents);
  _componentsNames.resize(_numberOfComponents);
  _componentsDescriptions.resize(_numberOfComponents);
  _componentsUnits.resize(_numberOfComponents);
  _MEDComponentsUnits.resize(_numberOfComponents);

  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsTypes[i]=m._componentsTypes[i];}

  //if (m._componentsTypes != NULL)
  //  {
  //    _componentsTypes = new int[m._numberOfComponents] ;
  //    memcpy(_componentsTypes,m._componentsTypes,sizeof(int)*m._numberOfComponents);
  //  }
  //else
  //  _componentsTypes = (int *) NULL;

  //_componentsNames = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    _componentsNames[i]=m._componentsNames[i];
  //_componentsDescriptions = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    _componentsDescriptions[i]=m._componentsDescriptions[i];

  //if (m._componentsUnits != NULL)
  //  {
  //    _componentsUnits = new UNIT[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    _componentsUnits[i] = m._componentsUnits[i];
  //  }
  //else
  //  _componentsUnits=(UNIT*)NULL;
  
  // L'operateur '=' est defini dans la classe UNIT
  //_MEDComponentsUnits = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    {_MEDComponentsUnits[i] = m._MEDComponentsUnits[i];}

  _iterationNumber = m._iterationNumber;
  _time = m._time;
  _orderNumber = m._orderNumber;
}
