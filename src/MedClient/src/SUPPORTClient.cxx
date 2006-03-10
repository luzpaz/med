// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
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
  	convertCorbaArray2(_geometricType, _numberOfGeometricType, all->types);

        SCRUTE(_name);
        SCRUTE(_description);

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
    if(!_isOnAllElts) {
    const int * index, * value;
    long n_index, n_value;
    
    SALOME::SenderInt_var senderForValue=IOR_Support->getSenderForNumber(MED_ALL_ELEMENTS);
    value=(const int *)ReceiverFactory::getValue(senderForValue,n_value);
    SALOME::SenderInt_var senderForIndex=IOR_Support->getSenderForNumberIndex();
    index=(const int *)ReceiverFactory::getValue(senderForIndex,n_index);
    
    SCRUTE(n_index);
    SCRUTE(n_value);
    setNumber(index, value,true);
    }
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
int SUPPORTClient::getValIndFromGlobalNumber(const int number) const throw (MEDEXCEPTION)
{
  BEGIN_OF("SUPPORTClient::getValIndFromGlobalNumber()");
  
  if (!_complete_support) (const_cast<SUPPORTClient *>(this))->fillCopy();
  const int n = SUPPORT::getValIndFromGlobalNumber(number);
  
  END_OF("SUPPORTClient::getValIndFromGlobalNumber()");
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
