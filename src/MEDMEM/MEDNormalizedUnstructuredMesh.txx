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
#ifndef __MEDNORMALIZEDUNSTRUCTUREDMESH_TXX__
#define __MEDNORMALIZEDUNSTRUCTUREDMESH_TXX__

#include "MEDNormalizedUnstructuredMesh.hxx"
#include "InterpKernelException.hxx"

#include "MEDMEM_Mesh.hxx"

template<int SPACEDIM,int MESHDIM>
MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::MEDNormalizedUnstructuredMesh(const MEDMEM::MESH *mesh):_meshInMedMode(mesh),
                                                                                                         _conn_for_interp(0),
                                                                                                         _own_conn_for_interp(false),
                                                                                                         _conn_index_for_interp(0),
                                                                                                         _own_conn_index_for_interp(false)
{
  prepare();
}

template<int SPACEDIM,int MESHDIM>
MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::~MEDNormalizedUnstructuredMesh()
{
  releaseTempArrays();
}

template<int SPACEDIM,int MESHDIM>
void MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getBoundingBox(double *boundingBox) const
{
  vector< vector<double> > ret=_meshInMedMode->getBoundingBox();
  //low left corner
  int i;
  for(i=0;i<SPACEDIM;i++)
    boundingBox[i]=ret[0][i];
  for(i=0;i<SPACEDIM;i++)
    boundingBox[i+SPACEDIM]=ret[1][i];
}

template<int SPACEDIM,int MESHDIM>
INTERP_KERNEL::NormalizedCellType MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getTypeOfElement(int eltId) const
{
  MED_EN::medGeometryElement type=_meshInMedMode->getElementTypeWithPoly(MED_EN::MED_CELL,eltId);
  if(type==MED_EN::MED_POLYGON)
    return INTERP_KERNEL::NORM_POLYGON;
  if(type==MED_EN::MED_POLYHEDRA)
    return INTERP_KERNEL::NORM_POLYHED;
  return (INTERP_KERNEL::NormalizedCellType)(((unsigned long)type/100-2)*10+((unsigned long)type%100));
}

template<int SPACEDIM,int MESHDIM>
unsigned char MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getNumberOfNodesOfElement(int eltId) const
{
  if(_meshInMedMode->getElementTypeWithPoly(MED_EN::MED_CELL,eltId)!=MED_EN::MED_POLYHEDRA)
    return (unsigned char) (_conn_index_for_interp[eltId]-_conn_index_for_interp[eltId-1]);
  else
    {
      return (unsigned char)(_conn_index_for_interp[eltId]-_conn_index_for_interp[eltId-1])-
        std::count(_conn_for_interp+_conn_index_for_interp[eltId-1]-1,_conn_for_interp+_conn_index_for_interp[eltId]-1,-1);
    }
}

template<int SPACEDIM,int MESHDIM>
unsigned long MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getNumberOfElements() const
{
  return _meshInMedMode->getNumberOfElementsWithPoly(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
}

template<int SPACEDIM,int MESHDIM>
unsigned long MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getNumberOfNodes() const
{
  return _meshInMedMode->getNumberOfNodes();
}

template<int SPACEDIM,int MESHDIM>
const int *MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getConnectivityPtr() const
{
  return _conn_for_interp;
}

template<int SPACEDIM,int MESHDIM>
const double *MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getCoordinatesPtr() const
{
  return _meshInMedMode->getCoordinates(MED_EN::MED_FULL_INTERLACE);
}

template<int SPACEDIM,int MESHDIM>
const int *MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getConnectivityIndexPtr() const
{
  return _conn_index_for_interp;
}

template<int SPACEDIM,int MESHDIM>
void MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::releaseTempArrays()
{
  if(_own_conn_for_interp)
    delete [] _conn_for_interp;
  if(_own_conn_index_for_interp)
    delete [] _conn_index_for_interp;
  _conn_for_interp=0;
  _conn_index_for_interp=0;
  _own_conn_for_interp=false;
  _own_conn_index_for_interp=false;
}

template<int SPACEDIM,int MESHDIM>
void MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::prepare()
{
  releaseTempArrays();
  int nbElt1=_meshInMedMode->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  int nbElt2=_meshInMedMode->getNumberOfElementsWithPoly(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  if(nbElt1==nbElt2)
    {//no poly
      _conn_for_interp=(int *)_meshInMedMode->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
      _own_conn_for_interp=false;
      _conn_index_for_interp=(int *)_meshInMedMode->getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);
      _own_conn_index_for_interp=false;
    }
  else
    {//poly
      int meshDim=_meshInMedMode->getMeshDimension();
      _own_conn_for_interp=true;
      _own_conn_index_for_interp=true;
      _conn_index_for_interp=new int[nbElt2+1];
      const int indDft[1]={1};
      const int *ind;
      int connClassLgth;
      const int *conn;
      if(_meshInMedMode->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL))
        {
          ind=_meshInMedMode->getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);
          connClassLgth=_meshInMedMode->getConnectivityLength(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
          conn=_meshInMedMode->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,
                                               MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
        }
      else
        {
          ind=indDft;
          connClassLgth=0;
          conn=0;
        }
      int *work=std::copy(ind,ind+nbElt1+1,_conn_index_for_interp);
      int offset=work[-1]-1;
      switch(meshDim)
        {
        case 3:
          {
            int nbOfPolyH=nbElt2-nbElt1;
            std::vector<int> connPolyh,connPolyhInd(nbOfPolyH+1);
            connPolyhInd[0]=1;
            const int *conn1=_meshInMedMode->getPolyhedronIndex(MED_EN::MED_NODAL);
            const int *conn2=_meshInMedMode->getPolyhedronFacesIndex();
            const int *conn3=_meshInMedMode->getPolyhedronConnectivity(MED_EN::MED_NODAL);
            for(int i=0;i<nbOfPolyH;i++)
              {
                int nbOfFaces=conn1[i+1]-conn1[i];
                int size1=connPolyh.size();
                for(int j=0;j<nbOfFaces;j++)
                  {
                    connPolyh.insert(connPolyh.end(),conn3+conn2[conn1[i]-1+j]-1,conn3+conn2[conn1[i]+j]-1);
                    if(j!=nbOfFaces-1)
                      connPolyh.push_back(-1);
                  }
                connPolyhInd[i+1]=connPolyhInd[i]+connPolyh.size()-size1;
              }
            std::vector<int>::iterator iter=connPolyhInd.begin(); iter++;
            std::transform(iter,connPolyhInd.end(),work,std::bind2nd(std::plus<int>(),offset));
            _conn_for_interp=new int[connClassLgth+connPolyh.size()];
            work=std::copy(conn,conn+connClassLgth,_conn_for_interp);
            std::copy(connPolyh.begin(),connPolyh.end(),work);
            break;
          }
        case 2:
          {
            const int *indP=_meshInMedMode->getPolygonsConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
            std::transform(indP+1,indP+nbElt2-nbElt1+1,work,std::bind2nd(std::plus<int>(),offset));
            //
            int connPolyLgth=_meshInMedMode->getPolygonsConnectivityLength(MED_EN::MED_NODAL,MED_EN::MED_CELL);
            _conn_for_interp=new int[connClassLgth+connPolyLgth];
            work=std::copy(conn,conn+connClassLgth,_conn_for_interp);
            indP=_meshInMedMode->getPolygonsConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL);
            std::copy(indP,indP+connPolyLgth,work);
            break;
          }
        default:
          throw INTERP_KERNEL::Exception("Invalid mesh dimension detected !");
        }
    }
}

#endif
