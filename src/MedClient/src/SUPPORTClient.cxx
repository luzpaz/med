#include "Utils_CorbaException.hxx"
#include "UtilClient.hxx"
#include "SUPPORTClient.hxx"
#include "MESHClient.hxx"

using namespace MEDMEM;


//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================
SUPPORTClient::SUPPORTClient(const SALOME_MED::SUPPORT_ptr theSupport, MESH *theMesh):
  IOR_Support(SALOME_MED::SUPPORT::_duplicate(theSupport)),
  myMeshHolder(theMesh)
{
  INFOS("SUPPORTClient:::SUPPORTClient = "<<this);
  if(!theMesh){
    SALOME_MED::MESH_var aMesh = theSupport->getMesh();
    myMeshHolder.reset(new MESHClient(aMesh));
  }
  setMesh(myMeshHolder.get());

  blankCopy();
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
        SALOME_MED::SUPPORT::supportInfos *all = new SALOME_MED::SUPPORT::supportInfos;
        all= IOR_Support->getSupportGlobal();

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
  	SCRUTE(_totalNumberOfElements);
  	_complete_support = false;
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

    int * index, * value;
    long n_index, n_value;

    convertCorbaArray(value, n_value, 
		      IOR_Support->getNumber(MED_ALL_ELEMENTS));
    convertCorbaArray(index, n_index, 
		      IOR_Support->getNumberIndex());
    
    SCRUTE(n_index);
    SCRUTE(n_value);
    setNumber(index, value);
      
    delete [] index;
    delete [] value;

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
  INFOS("SUPPORTClient:::~SUPPORTClient = "<<this);
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

