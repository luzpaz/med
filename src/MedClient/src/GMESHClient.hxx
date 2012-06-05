// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

#ifndef _GMESHCLIENT_HXX
#define _GMESHCLIENT_HXX

#include "MEDClient.hxx"

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_GMesh.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM {
class MESHClient;

class MEDCLIENT_EXPORT GMESHClient : public GMESH
{
private :

  const SALOME_MED::GMESH_var IOR_Mesh;

  mutable bool _complete;
  
  //mutable int _refCounter;
  MESHClient* _uMesh;

public :

  GMESHClient(const SALOME_MED::GMESH_ptr m);
  virtual bool operator==(const GMESH& other) const;

  void blankCopy();
  void fillCopy();
//   virtual void addReference() const;
//   virtual void removeReference() const;
  virtual void write(int index=0) const;
  virtual void write(const GENDRIVER & genDriver,
                     MED_EN::med_mode_acces medMode=MED_EN::WRONLY) const;
  virtual void write(driverTypes        driverType,
                     const std::string& filename,
                     const std::string& meshname="",
                     MED_EN::med_mode_acces medMode=MED_EN::WRONLY) const;
  virtual void printMySelf(ostream &os) const;

  virtual bool isEmpty() const;
  virtual bool deepCompare(const GMESH&) const;
  virtual int getMeshDimension() const;
  virtual bool getIsAGrid() const;
  virtual std::string getCoordinatesSystem() const;
  virtual const std::string* getCoordinatesNames() const;
  virtual const std::string* getCoordinatesUnits() const;
  virtual int getNumberOfNodes() const;
  virtual int getNumberOfTypes(MED_EN::medEntityMesh) const;
  virtual int getNumberOfElements(MED_EN::medEntityMesh, MED_EN::medGeometryElement) const;
  virtual const MED_EN::medGeometryElement* getTypes(MED_EN::medEntityMesh) const;
  virtual MED_EN::medGeometryElement getElementType(MED_EN::medEntityMesh, int) const;
  virtual const MESH* convertInMESH() const;
  virtual SUPPORT* getBoundaryElements(MED_EN::medEntityMesh) const throw (MEDEXCEPTION);
  virtual SUPPORT* getSkin(const SUPPORT*) throw (MEDEXCEPTION);
  virtual SUPPORT* buildSupportOnNodeFromElementList(const std::list<int>&, MED_EN::medEntityMesh) const throw (MEDEXCEPTION);
  virtual void fillSupportOnNodeFromElementList(const std::list<int>&, SUPPORT*) const throw (MEDEXCEPTION);
  virtual FIELD<double>* getVolume(const SUPPORT*, bool) const throw (MEDEXCEPTION);
  virtual FIELD<double>* getArea(const SUPPORT*) const throw (MEDEXCEPTION);
  virtual FIELD<double>* getLength(const SUPPORT*) const throw (MEDEXCEPTION);
  virtual FIELD<double>* getNormal(const SUPPORT*) const throw (MEDEXCEPTION);
  virtual FIELD<double>* getBarycenter(const SUPPORT*) const throw (MEDEXCEPTION);
  virtual std::vector<std::vector<double> > getBoundingBox() const;

private:
  virtual ~GMESHClient();
};
}

#endif

