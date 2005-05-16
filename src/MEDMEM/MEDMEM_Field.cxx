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
  _name(""), _description(""), _support((SUPPORT *)NULL),
  _numberOfComponents(0), _numberOfValues(0),_componentsTypes((int *)NULL),
  _componentsNames((string *)NULL), 
  _componentsDescriptions((string *)NULL),
  _componentsUnits((UNIT*)NULL),
  _MEDComponentsUnits((string *)NULL),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1)
{
  MESSAGE("Constructeur FIELD_ sans parametre");
}

FIELD_::FIELD_(const SUPPORT * Support, const int NumberOfComponents):
  _isRead(false),
  _name(""), _description(""), _support(Support),
  _numberOfComponents(NumberOfComponents),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1)
{
  MESSAGE("FIELD_(const SUPPORT * Support, const int NumberOfComponents)");

  _numberOfValues = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  _componentsTypes = new int[NumberOfComponents] ;
  _componentsNames = new string[NumberOfComponents];
  _componentsDescriptions = new string[NumberOfComponents];
  _componentsUnits = new UNIT[NumberOfComponents];
  _MEDComponentsUnits = new string[NumberOfComponents];
  for(int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }
  if(_support)
    _support->addReference();
}

FIELD_::FIELD_(const FIELD_ &m)
{
  _isRead = m._isRead ;
  _name = m._name;
  _description = m._description;
  _support = m._support;
  if(_support)
    _support->addReference();
  _numberOfComponents = m._numberOfComponents;
  _numberOfValues = m._numberOfValues;
  copyGlobalInfo(m);
  _valueType = m._valueType;
  //_drivers = m._drivers ; // PG : Well, same driver, what about m destructor !
}

FIELD_::~FIELD_()
{   
  MESSAGE("~FIELD_()");
  if ( _componentsTypes !=NULL)
    delete[] _componentsTypes ;
  if ( _componentsNames !=NULL)
    delete[] _componentsNames ;
  if ( _componentsDescriptions !=NULL)
    delete[] _componentsDescriptions ;
  if ( _componentsUnits !=NULL)
    delete[] _componentsUnits ;
  if ( _MEDComponentsUnits !=NULL)
    delete[] _MEDComponentsUnits ;
  // delete driver
//   vector<GENDRIVER *>::const_iterator it ;
//   SCRUTE(_drivers.size());
//   int i=0;
//   for (it=_drivers.begin();it!=_drivers.end();it++) {
//     i++;
//     SCRUTE(i);
//     delete (*it) ;


  MESSAGE("In this object FIELD_ there is(are) " << _drivers.size() << " driver(s)");

  for (unsigned int index=0; index < _drivers.size(); index++ )
    {
      SCRUTE(_drivers[index]);
      if ( _drivers[index] != NULL) delete _drivers[index];
    }
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
	    switch (getSupport()->getMesh()->getSpaceDimension() ) 
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

    const int* nbGauss=getSupport()->getNumberOfGaussPoint();
    for (int i=0; i<getSupport()->getNumberOfTypes(); ++i)
	if(nbGauss[i]!=1)
	{
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
    }

}

/*! 
  \if developper
   Check up the compatibility of fields before performing an arithmetic operation
  \endif
*/
void FIELD_::_checkFieldCompatibility(const FIELD_& m, const FIELD_& n , bool checkUnit ) throw (MEDEXCEPTION)
{
    string diagnosis;

    // check-up, fill diagnosis if some incompatibility is found.
    if(m._support != n._support)
      {
	if(!(*m._support==*n._support))
	  diagnosis+="They don't have the same support!";
      }
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
    if(m._support != n._support)
      {
	if(!(m._support->deepCompare(*n._support)))
	  diagnosis+="They don't have the same support!";
      }
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

//  void     FIELD_::setIterationNumber (int IterationNumber)           {};
//  void     FIELD_::setOrderNumber     (int OrderNumber)               {}; 
//  void     FIELD_::setFieldName       (string& fieldName)             {}; 
         
void     FIELD_::rmDriver      (int index)
{
  MESSAGE("void FIELD_::rmDriver(int index) : removing the driver " << index);
};
int      FIELD_::addDriver     (driverTypes driverType, 
                                const string & fileName,
				const string & driverFieldName,
				med_mode_acces access)
{
  MESSAGE("int FIELD_::addDriver(driverTypes driverType, const string & fileName, const string & driverFieldName) : adding the driver " << driverType << " fileName = " << fileName.c_str() << " driverFieldName = " << driverFieldName.c_str());
  return 0;
};
int      FIELD_::addDriver     (GENDRIVER & driver)
{
  MESSAGE("int FIELD_::addDriver(GENDRIVER & driver) : driver " << driver);
  return 0;
};
void     FIELD_::openAppend    ( void )                               {};
void     FIELD_::write         (const GENDRIVER &)                    {};
void     FIELD_::writeAppend   (const GENDRIVER &)                    {};
void     FIELD_::read          (const GENDRIVER &)                    {};
void     FIELD_::write         (int , const string & ) {};
void     FIELD_::writeAppend   (int , const string & ) {};
void     FIELD_::read          (int )                                  {};

void     FIELD_::copyGlobalInfo(const FIELD_& m)
{  
  if (m._componentsTypes != NULL)
    {
      _componentsTypes = new int[m._numberOfComponents] ;
      memcpy(_componentsTypes,m._componentsTypes,sizeof(int)*m._numberOfComponents);
    }
  else
    _componentsTypes = (int *) NULL;

  _componentsNames = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    _componentsNames[i]=m._componentsNames[i];
  _componentsDescriptions = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    _componentsDescriptions[i]=m._componentsDescriptions[i];

  if (m._componentsUnits != NULL)
    {
      _componentsUnits = new UNIT[m._numberOfComponents];
      for (int i=0; i<m._numberOfComponents; i++)
	_componentsUnits[i] = m._componentsUnits[i];
    }
  else
    _componentsUnits=(UNIT*)NULL;
  
  // L'operateur '=' est defini dans la classe UNIT
  _MEDComponentsUnits = new string[m._numberOfComponents];
  for (int i=0; i<m._numberOfComponents; i++)
    {_MEDComponentsUnits[i] = m._MEDComponentsUnits[i];}
  _iterationNumber = m._iterationNumber;
  _time = m._time;
  _orderNumber = m._orderNumber;
}

//  void                     FIELD_::setValueType(med_type_champ ValueType) {};
//  med_type_champ FIELD_::getValueType() {};
