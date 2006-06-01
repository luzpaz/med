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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _CONNECTIVITY_CLIENT_HXX
#define _CONNECTIVITY_CLIENT_HXX

#include <SALOMEconfig.h>
#include "MEDMEM_Connectivity.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM {
class CONNECTIVITYClient : public CONNECTIVITY {

protected:
 
  long *_numberOfElements_client;
  long _totalNumberOfElements_client;
  medGeometryElement _polyType_client;

  mutable bool _complete;
  SALOME_MED::MESH_var IOR_Mesh ;

public:

  CONNECTIVITYClient(const SALOME_MED::MESH_ptr m, 
		     medEntityMesh Entity=MED_CELL);

  virtual ~CONNECTIVITYClient();

  void fillCopy();
  void blankCopy();
 
  int getNumberOf(medEntityMesh Entity, medGeometryElement Type) const;

  const int * getConnectivity      (medConnectivity ConnectivityType, 
					medEntityMesh Entity,
					medGeometryElement Type);
  const int * getConnectivityIndex (medConnectivity ConnectivityType,
					medEntityMesh Entity);
  
  void  calculateConnectivity (medConnectivity connectivityType, 
			       medEntityMesh Entity);

  void  updateFamily (vector<FAMILY*> myFamilies);

  const int * getGlobalNumberingIndex (medEntityMesh Entity) const throw (MEDEXCEPTION);

  bool existConnectivity(medConnectivity ConnectivityType, 
			 medEntityMesh Entity) const;

  const int* getReverseConnectivity (medConnectivity ConnectivityType, 
					 medEntityMesh Entity=MED_CELL)
    throw (MEDEXCEPTION);

  const int* getReverseConnectivityIndex (medConnectivity ConnectivityType,
					      medEntityMesh Entity=MED_CELL)
    throw (MEDEXCEPTION);

  const int* getValue (medConnectivity TypeConnectivity, 
			   medGeometryElement Type);
 
  const int* getValueIndex        (medConnectivity TypeConnectivity);
  const int* getNeighbourhood() const;

  

  bool       existPolygonsConnectivity(medConnectivity connectivityType,
                                       medEntityMesh   Entity) const;
  bool       existPolyhedronConnectivity(medConnectivity connectivityType,
                                         medEntityMesh   Entity) const;
  const int* getPolygonsConnectivity(medConnectivity ConnectivityType,
				     medEntityMesh   Entity);
  const int* getPolygonsConnectivityIndex(medConnectivity ConnectivityType,
					  medEntityMesh   Entity);
  const int* getPolyhedronConnectivity(medConnectivity ConnectivityType) const;
  const int* getPolyhedronIndex(medConnectivity ConnectivityType) const;
  const int* getPolyhedronFacesIndex() const;
  int getNumberOfPolygons() const;
  int getNumberOfPolyhedronFaces() const;
  int getNumberOfPolyhedron() const;
};
};


#endif
