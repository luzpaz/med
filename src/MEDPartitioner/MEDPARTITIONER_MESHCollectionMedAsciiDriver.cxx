//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
#include <vector>
#include <string>
#include <map>
#include <set>

#include <iostream>
#include <fstream>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <sys/time.h>
//Debug macros
#include "MEDMEM_Exception.hxx"

#include "MEDCouplingUMesh.hxx"
#include "MEDLoader.hxx"

//MEDPARTITIONER includes
#include "MEDPARTITIONER_ParallelTopology.hxx"
#include "MEDPARTITIONER_MESHCollectionDriver.hxx"
#include "MEDPARTITIONER_MESHCollection.hxx"
#include "MEDPARTITIONER_MESHCollectionMedAsciiDriver.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"

using namespace MEDPARTITIONER;

//template inclusion
//#include "MEDPARTITIONER_MESHCollectionMedAsciiDriver.H"


MESHCollectionMedAsciiDriver::MESHCollectionMedAsciiDriver(MESHCollection* collection):MESHCollectionDriver(collection)
{
}

/*!reads a MED File v>=2.3
 * and mounts the corresponding meshes in memory
 * the connect zones are created from the joints
 * 
 *\param filename ascii file containing the list of MED v2.3 files
 * */

int MESHCollectionMedAsciiDriver::read(const char* filename, ParaDomainSelector* domainSelector)
{

  //ditributed meshes
  vector<int*> cellglobal;
  vector<int*> nodeglobal;
  vector<int*> faceglobal;

  int nbdomain;

  // reading ascii master file
  try{
    ifstream asciiinput(filename);

    if (!asciiinput)     
      throw INTERP_KERNEL::Exception("MEDPARTITIONER read - Master File does not exist");

    char charbuffer[512];
    asciiinput.getline(charbuffer,512);

    while (charbuffer[0]=='#')
    {
      asciiinput.getline(charbuffer,512);
    }

    //reading number of domains
    nbdomain=atoi(charbuffer);
    cout << "nb domain "<<nbdomain<<endl;
    //    asciiinput>>nbdomain;
    _filename.resize(nbdomain);
    _meshname.resize(nbdomain);
    (_collection->getMesh()).resize(nbdomain);
    cellglobal.resize(nbdomain);
    nodeglobal.resize(nbdomain);
    faceglobal.resize(nbdomain);

    if (nbdomain == 0)
      throw INTERP_KERNEL::Exception("Empty ASCII master file");
    for (int i=0; i<nbdomain;i++)
    {

      //reading information about the domain
      string mesh;
      int idomain;
      string host;
      cellglobal[i]=0;
      faceglobal[i]=0;
      nodeglobal[i]=0;

      asciiinput >> mesh >> idomain >> _meshname[i] >> host >> _filename[i];

      //Setting the name of the global mesh (which is the same
      //for all the subdomains)
      if (i==0)
        _collection->setName(mesh);

      if (idomain!=i+1)
      {
        cerr<<"Error : domain must be written from 1 to N in asciifile descriptor"<<endl;
        return 1;
      }
      if ( !domainSelector || domainSelector->isMyDomain(i))
        readSubdomain(cellglobal,faceglobal,nodeglobal, i);

    }//loop on domains
  }//of try
  catch(...)
  {
    cerr << "I/O error reading parallel MED file"<<endl;
    throw;
  }

  //creation of topology from mesh and connect zones
  ParallelTopology* aPT = new ParallelTopology
    ((_collection->getMesh()), (_collection->getCZ()), cellglobal, nodeglobal, faceglobal);
  _collection->setTopology(aPT);

  for (int i=0; i<nbdomain; i++)
  {
    if (cellglobal[i]!=0) delete[] cellglobal[i];
    if (nodeglobal[i]!=0) delete[] nodeglobal[i];
    if (faceglobal[i]!=0) delete[] faceglobal[i];
  }

  return 0;
}


/*! writes the collection of meshes in a 
 * MED v2.3 file
 * with the connect zones being written as joints
 * \param filename name of the ascii file containing the meshes description
 */
void MESHCollectionMedAsciiDriver::write(const char* filename, ParaDomainSelector* domainSelector)
{


  int nbdomains= _collection->getMesh().size();
  _filename.resize(nbdomains);

  //loop on the domains
  for (int idomain=0; idomain<nbdomains;idomain++)
  {
    string distfilename;

    ostringstream suffix;
    suffix << filename<< idomain+1 <<".med";

    distfilename=suffix.str();

    _filename[idomain]=distfilename;

    if ( !domainSelector || domainSelector->isMyDomain( idomain ) )
    {
      if ( !_collection->getMesh()[idomain]->getNumberOfCells()==0 ) continue;//empty domain

      MEDLoader::WriteUMesh(distfilename.c_str(),(_collection->getMesh())[idomain],true);

      //      writeSubdomain(idomain, nbdomains, distfilename.c_str(), domainSelector);
    }
  }

  // write master file
  if ( !domainSelector || domainSelector->rank() == 0 )
  {
    ofstream file(filename);

    file <<"#MED Fichier V 2.3"<<" "<<endl;
    file <<"#"<<" "<<endl;
    file<<_collection->getMesh().size()<<" "<<endl;

    for (int idomain=0; idomain<nbdomains;idomain++)
      file << _collection->getName() <<" "<< idomain+1 << " "
           << (_collection->getMesh())[idomain]->getName() << " localhost "
           << _filename[idomain] << " "<<endl;
  }

}
