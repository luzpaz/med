#include "Utils_CorbaException.hxx"
#include "UtilClient.hxx"
#include "SUPPORTClient.hxx"
#include "MESHClient.hxx"
#include "ReceiverFactory.hxx"

using namespace MEDMEM;
using namespace MED_EN;

//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================

SUPPORTClient::SUPPORTClient(const SALOME_MED::SUPPORT_ptr S,
			     MESH * M) : 
  SUPPORT(), 
  IOR_Support(SALOME_MED::SUPPORT::_duplicate(S)),_refCounter(1)
{
  BEGIN_OF("SUPPORTClient::SUPPORTClient(SALOME_MED::SUPPORT_ptr m)");

  SCRUTE(S);
  SCRUTE(M);
  if(M)
    _mesh=M;
  else
    {
      _mesh=new MESHClient(IOR_Support->getMesh());
    }
  blankCopy();

  END_OF("SUPPORTClient::SUPPORTClient(SALOME_MED::SUPPORT_ptr m)");
}
//=============================================================================
/*!
 * Remplit les informations générales
 */
//=============================================================================
void SUPPORTClient::blankCopy()
{
  BEGIN_OF("SUPPORTClient::blankCopy");

 try
  {
        SALOME_MED::SUPPORT::supportInfos_var all = IOR_Support->getSupportGlobal();

        _name = all->name;
        _description = all->description;
	setAll(all->isOnAllElements);
	setEntity(all->entity);
// modifs PN 
        setNumberOfGeometricType(all->numberOfGeometricType);
  	convertCorbaArray(_geometricType, _numberOfGeometricType,
		           &all->types);

        //_name = IOR_Support->getName();
        //_description = IOR_Support->getDescription();
  	//setAll(IOR_Support->isOnAllElements());
  	//setEntity(IOR_Support->getEntity());

        SCRUTE(_name);
        SCRUTE(_description);
  	//convertCorbaArray(_geometricType, _numberOfGeometricType,
	//	    IOR_Support->getTypes());

        int *nE = new int[_numberOfGeometricType];
        int i;
  	for (i=0; i<_numberOfGeometricType; i++)
	{
    		nE[i] = all->nbEltTypes[i];
  	}
  	setNumberOfElements(nE);

	delete [] nE;

  	SCRUTE(_totalNumberOfElements);
  	_complete_support = false;
	//Gauss points settings
	_numberOfGaussPoint = new int[_numberOfGeometricType];
	for (int i=0;i<_numberOfGeometricType;i++)
	  _numberOfGaussPoint[i] = 1 ;
  }
  catch( const CORBA::Exception &ex )
  {
         MESSAGE("Erreur CORBA dans la communication" ) ;
         THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
                                       SALOME::INTERNAL_ERROR);
  }

  END_OF("SUPPORTClient::blankCopy");

}
//=============================================================================
/*!
 *  
 */
//=============================================================================
void SUPPORTClient::fillCopy()
{
  BEGIN_OF("SUPPORTClient::fillCopy");

  if (!_complete_support) {

    const int * index, * value;
    long n_index, n_value;
    
    SALOME::SenderInt_var senderForValue=IOR_Support->getSenderForNumber(MED_ALL_ELEMENTS);
    value=(const int *)ReceiverFactory::getValue(senderForValue,n_value);
    SALOME::SenderInt_var senderForIndex=IOR_Support->getSenderForNumberIndex();
    index=(const int *)ReceiverFactory::getValue(senderForIndex,n_index);
    
    SCRUTE(n_index);
    SCRUTE(n_value);
    setNumber(index, value,true);

    _complete_support = true;
  }

  END_OF("SUPPORTClient::fillCopy");
}
//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================
SUPPORTClient::~SUPPORTClient()
{
  BEGIN_OF("SUPPORTClient::~SUPPORTClient");
  IOR_Support->Destroy();
  if(_mesh)
    _mesh->removeReference();
  END_OF("SUPPORTClient::~SUPPORTClient");
}

//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================
MEDSKYLINEARRAY *  SUPPORTClient::getnumber() const throw (MEDEXCEPTION)
{
  BEGIN_OF("SUPPORTClient::getnumber()");

  if (!_complete_support) (const_cast<SUPPORTClient *>(this))->fillCopy();
  MEDSKYLINEARRAY *m = SUPPORT::getnumber();

  END_OF("SUPPORTClient::getnumber()");
  return m;
}

//=============================================================================
/*!
 * 
 */
//=============================================================================
const int *  SUPPORTClient::getNumber(medGeometryElement GeometricType) 
    const throw (MEDEXCEPTION)
{
  BEGIN_OF("SUPPORTClient::getnumber(medGeometryElement)");

  if (!_complete_support) (const_cast<SUPPORTClient *>(this))->fillCopy();
  const int *n = SUPPORT::getNumber(GeometricType);

  END_OF("SUPPORTClient::getnumber(medGeometryElement)");
  return n;
}

//=============================================================================
/*!
 * 
 */
//=============================================================================
const int *  SUPPORTClient::getNumberIndex() const throw (MEDEXCEPTION) 
{
  BEGIN_OF("SUPPORTClient::getnumberIndex()");

  if (!_complete_support) (const_cast<SUPPORTClient *>(this))->fillCopy();
  const int * n = SUPPORT::getNumberIndex();

  END_OF("SUPPORTClient::getnumberIndex()");
  return n;
}

//=============================================================================
/*!
 * 
 */
//=============================================================================
void SUPPORTClient::addReference() const
{
  _refCounter++;
}

//=============================================================================
/*!
 * 
 */
//=============================================================================
void SUPPORTClient::removeReference() const
{
  if (--_refCounter <= 0)
    {
      delete this;
    }
}
