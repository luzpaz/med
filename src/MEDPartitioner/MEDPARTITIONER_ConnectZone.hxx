// Copyright (C) 2007-2015  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#ifndef __MEDPARTITIONER_CONNECTZONE_HXX__
#define __MEDPARTITIONER_CONNECTZONE_HXX__

#include "MEDPARTITIONER.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingSkyLineArray.hxx"

#include <map>
#include <vector>
#include <string>

namespace MEDPARTITIONER
{
  class MEDPARTITIONER_EXPORT ConnectZone
  {
  public :
    ConnectZone();
    ~ConnectZone();
    ConnectZone(const ConnectZone & myConnectZone);

    std::string getName() const ;
    std::string getDescription() const ;
    int getDistantDomainNumber() const ;
    int getLocalDomainNumber() const ;
    ParaMEDMEM::MEDCouplingUMesh *getLocalMesh() const ;
    ParaMEDMEM::MEDCouplingUMesh *getDistantMesh() const ;

    bool isEntityCorrespPresent(int localEntity,int distantEntity) const;
    const int *getNodeCorrespIndex() const;
    const int *getNodeCorrespValue() const;
    int getNodeNumber() const;
    const ParaMEDMEM::MEDCouplingSkyLineArray * getNodeCorresp() const;
    const int *getFaceCorrespIndex() const;
    const int *getFaceCorrespValue() const;
    int getFaceNumber() const;
    const ParaMEDMEM::MEDCouplingSkyLineArray * getFaceCorresp() const;
    const int *getEntityCorrespIndex(int localEntity,
                                     int distantEntity) const;
    const int *getEntityCorrespValue(int localEntity,
                                     int distantEntity) const;
    int getEntityCorrespNumber(int localEntity,
                               int distantEntity) const;
    int getEntityCorrespLength(int localEntity,
                               int distantEntity) const;
    const ParaMEDMEM::MEDCouplingSkyLineArray * getEntityCorresp(int localEntity,
                                                                 int distantEntity) const;
    std::vector< std::pair< int,int > > getEntities() const;

    void setName(const std::string& name) ;
    void setDescription(const std::string& description) ;
    void setDistantDomainNumber(int distantDomainNumber) ;
    void setLocalDomainNumber(int distantDomainNumber) ;
    void setLocalMesh(ParaMEDMEM::MEDCouplingUMesh * localMesh) ;
    void setDistantMesh(ParaMEDMEM::MEDCouplingUMesh * distantMesh) ;

    void setNodeCorresp(const int * nodeCorresp, int nbnode);
    void setNodeCorresp(ParaMEDMEM::MEDCouplingSkyLineArray* array);
    void setFaceCorresp(const int * faceCorresp, int nbface);
    void setFaceCorresp(ParaMEDMEM::MEDCouplingSkyLineArray* array);
    void setEntityCorresp(int localEntity, int distantEntity,
                          const int * entityCorresp, int nbentity);
    void setEntityCorresp(int localEntity, int distantEntity,
                          ParaMEDMEM::MEDCouplingSkyLineArray *array);
  private :
    std::string _name;
    std::string _description;
    int _local_domain_number;
    int _distant_domain_number;

    ParaMEDMEM::MEDCouplingUMesh * _local_mesh;
    ParaMEDMEM::MEDCouplingUMesh * _distant_mesh;

    ParaMEDMEM::MEDCouplingSkyLineArray * _node_corresp;
    ParaMEDMEM::MEDCouplingSkyLineArray * _face_corresp;
  
    std::map < std::pair <int,int>, ParaMEDMEM::MEDCouplingSkyLineArray * > _entity_corresp;
  };
}
# endif
