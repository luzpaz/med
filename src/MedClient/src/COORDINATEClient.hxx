#ifndef _COORDINATE_CLIENT_HXX
#define _COORDINATE_CLIENT_HXX

#include <SALOMEconfig.h>
#include "MEDMEM_Coordinate.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM {
class COORDINATEClient : public COORDINATE {

protected:

  mutable bool _complete;
  SALOME_MED::MESH_var IOR_Mesh ;

public:

  /*!
  COORDINATEClient constructor
  
  Build an "empty" coordinate object (without the actual coordinate values).
  The object hold a Corba IOR and get data from it on user demand.

  Parameters in : m (Mesh Corba IOR)
                  mode (FULL_ or NO_ interlace)
 */
  COORDINATEClient(const SALOME_MED::MESH_ptr m, medModeSwitch Mode);


  /*!
  COORDINATEClient::blankCopy()
  
  "(Re)Blank" the coordinate object.
  
  Update fixed sized data from the Corba IOR (dimensions, strings).
  Get rid of variable sized data (coordinate values).
  Next call of getCoordinates (for instance) will update the
  object.
  */
  void blankCopy();


  /*!
  COORDINATEClient::fillCopy()
  
  (Re)fill the coordinate object from the Corba IOR 
  */
  void fillCopy();


  virtual ~COORDINATEClient() {};

  virtual const double *  getCoordinates(medModeSwitch Mode);
  virtual double          getCoordinate(int Number,int Axis);
  virtual const double *  getCoordinateAxis(int Axis);
  virtual const int*      getNodesNumbers() const;

};
};

#endif
