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
};
};


#endif
