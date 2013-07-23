// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#include "MEDFileFieldOverView.hxx"
#include "MEDFileField.hxx"
#include "MEDFileMesh.hxx"

#include "CellModel.hxx"

using namespace ParaMEDMEM;

const char MEDFileField1TSStructItem2::NEWLY_CREATED_PFL_NAME[]="???";

MEDFileMeshStruct *MEDFileMeshStruct::New(const MEDFileMesh *mesh)
{
  return new MEDFileMeshStruct(mesh);
}

std::size_t MEDFileMeshStruct::getHeapMemorySize() const
{
  std::size_t ret(0);
  for(std::vector< std::vector<int> >::const_iterator it0=_geo_types_distrib.begin();it0!=_geo_types_distrib.end();it0++)
    ret+=(*it0).capacity()*sizeof(int);
  ret+=_geo_types_distrib.capacity()*sizeof(std::vector<int>);
  return ret;
}

MEDFileMeshStruct::MEDFileMeshStruct(const MEDFileMesh *mesh):_mesh(mesh)
{
  std::vector<int> levs=mesh->getNonEmptyLevels();
  _name=mesh->getName();
  _nb_nodes=mesh->getNumberOfNodes();
  _geo_types_distrib.resize(levs.size());
  for(std::vector<int>::const_iterator lev=levs.begin();lev!=levs.end();lev++)
    {
      MEDCouplingAutoRefCountObjectPtr<MEDCouplingMesh> mLev=mesh->getGenMeshAtLevel(*lev);
      _geo_types_distrib[-(*lev)]=mLev->getDistributionOfTypes();
    }
}

int MEDFileMeshStruct::getLevelOfGeoType(INTERP_KERNEL::NormalizedCellType t) const throw(INTERP_KERNEL::Exception)
{
  int j=0;
  for(std::vector< std::vector<int> >::const_iterator it1=_geo_types_distrib.begin();it1!=_geo_types_distrib.end();it1++,j--)
    {
      std::size_t sz=(*it1).size();
      if(sz%3!=0)
        throw INTERP_KERNEL::Exception("MEDFileMeshStruct::getLevelOfGeoType : internal error in code !");
      std::size_t nbGeo=sz/3;
      for(std::size_t i=0;i<nbGeo;i++)
        if((*it1)[3*i]==(int)t)
          return j;
    }
  throw INTERP_KERNEL::Exception("MEDFileMeshStruct::getLevelOfGeoType : The specified geometric type is not present in the mesh structure !");
}

int MEDFileMeshStruct::getNumberOfElemsOfGeoType(INTERP_KERNEL::NormalizedCellType t) const throw(INTERP_KERNEL::Exception)
{
  for(std::vector< std::vector<int> >::const_iterator it1=_geo_types_distrib.begin();it1!=_geo_types_distrib.end();it1++)
    {
      std::size_t sz=(*it1).size();
      if(sz%3!=0)
        throw INTERP_KERNEL::Exception("MEDFileMeshStruct::getNumberOfElemsOfGeoType : internal error in code !");
      std::size_t nbGeo=sz/3;
      for(std::size_t i=0;i<nbGeo;i++)
        if((*it1)[3*i]==(int)t)
          return (*it1)[3*i+1];
    }
  throw INTERP_KERNEL::Exception("The specified geometric type is not present in the mesh structure !");
}

int MEDFileMeshStruct::getNumberOfLevs() const
{
  return (int)_geo_types_distrib.size();
}

int MEDFileMeshStruct::getNumberOfGeoTypesInLev(int relativeLev) const throw(INTERP_KERNEL::Exception)
{
  int pos(-relativeLev);
  if(pos<0 || pos>=_geo_types_distrib.size())
    throw INTERP_KERNEL::Exception("MEDFileMeshStruct::getNumberOfGeoTypesInLev : invalid level specified !");
  std::size_t sz=_geo_types_distrib[pos].size();
  if(sz%3!=0)
    throw INTERP_KERNEL::Exception("MEDFileMeshStruct::getNumberOfGeoTypesInLev : internal error in code !");
  return (int)(sz/3);
}

//=

MEDFileField1TSStructItem2::MEDFileField1TSStructItem2()
{
}

MEDFileField1TSStructItem2::MEDFileField1TSStructItem2(INTERP_KERNEL::NormalizedCellType a, const std::pair<int,int>& b, const std::string& c, const std::string& d):_geo_type(a),_start_end(b),_pfl(DataArrayInt::New()),_loc(d),_nb_of_entity(-1)
{
  _pfl->setName(c.c_str());
}

void MEDFileField1TSStructItem2::checkWithMeshStructForCells(const MEDFileMeshStruct *mst, const MEDFileFieldGlobsReal *globs) throw(INTERP_KERNEL::Exception)
{
  int nbOfEnt=mst->getNumberOfElemsOfGeoType(_geo_type);
  checkInRange(nbOfEnt,1,globs);
}

void MEDFileField1TSStructItem2::checkWithMeshStructForGaussNE(const MEDFileMeshStruct *mst, const MEDFileFieldGlobsReal *globs) throw(INTERP_KERNEL::Exception)
{
  int nbOfEnt=mst->getNumberOfElemsOfGeoType(_geo_type);
  const INTERP_KERNEL::CellModel& cm=INTERP_KERNEL::CellModel::GetCellModel(_geo_type);
  checkInRange(nbOfEnt,(int)cm.getNumberOfNodes(),globs);
}

void MEDFileField1TSStructItem2::checkWithMeshStructForGaussPT(const MEDFileMeshStruct *mst, const MEDFileFieldGlobsReal *globs) throw(INTERP_KERNEL::Exception)
{
  if(!globs)
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::checkWithMeshStructForGaussPT : no globals specified !");
  if(_loc.empty())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::checkWithMeshStructForGaussPT : no localization specified !");
  const MEDFileFieldLoc& loc=globs->getLocalization(_loc.c_str());
  int nbOfEnt=mst->getNumberOfElemsOfGeoType(_geo_type);
  checkInRange(nbOfEnt,loc.getNumberOfGaussPoints(),globs);
}

std::string MEDFileField1TSStructItem2::getPflName() const
{
  return _pfl->getName();
}

const DataArrayInt *MEDFileField1TSStructItem2::getPfl(const MEDFileFieldGlobsReal *globs) const
{
  if(!_pfl->isAllocated())
    {
      if(_pfl->getName().empty())
        return 0;
      else
        return globs->getProfile(_pfl->getName().c_str());
    }
  else
    return _pfl;
}

/*!
 * \param [in] nbOfEntity - number of entity that can be either cells or nodes. Not other possiblity.
 * \param [in] nip - number of integration points. 1 for ON_CELLS and NO_NODES
 */
void MEDFileField1TSStructItem2::checkInRange(int nbOfEntity, int nip, const MEDFileFieldGlobsReal *globs) throw(INTERP_KERNEL::Exception)
{
  _nb_of_entity=nbOfEntity;
  if(_pfl->getName().empty())
    {
      if(nbOfEntity!=(_start_end.second-_start_end.first)/nip)
        throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::checkInRange : Mismatch between number of entities and size of node field !");
      return ;
    }
  else
    {
      if(!globs)
        throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::checkInRange : Presence of a profile on field whereas no globals found in file !");
      const DataArrayInt *pfl=globs->getProfile(_pfl->getName().c_str());
      if(!pfl)
        throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::checkInRange : Presence of a profile on field whereas no such profile found in file !");
      pfl->checkAllIdsInRange(0,nbOfEntity);
    }
}

bool MEDFileField1TSStructItem2::operator==(const MEDFileField1TSStructItem2& other) const throw(INTERP_KERNEL::Exception)
{
  //_nb_of_entity is not taken into account here. It is not a bug, because no mesh consideration needed here to perform fast compare.
  //idem for _loc. It is not an effective attribute for support comparison.
  return _geo_type==other._geo_type && _start_end==other._start_end && _pfl->getName()==other._pfl->getName();
}

bool MEDFileField1TSStructItem2::isCellSupportEqual(const MEDFileField1TSStructItem2& other, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(_geo_type!=other._geo_type)
    return false;
  if(_nb_of_entity!=other._nb_of_entity)
    return false;
  if((_pfl->getName().empty() && !other._pfl->getName().empty()) || (!_pfl->getName().empty() && other._pfl->getName().empty()))
    return false;
  if(_pfl->getName().empty() && other._pfl->getName().empty())
    return true;
  const DataArrayInt *pfl1(getPfl(globs)),*pfl2(other.getPfl(globs));
  return pfl1->isEqualWithoutConsideringStr(*pfl2);
}

bool MEDFileField1TSStructItem2::isNodeSupportEqual(const MEDFileField1TSStructItem2& other, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  return isCellSupportEqual(other,globs);
}

/*!
 * \a objs must be non empty. \a objs should contain items having same geometric type.
 */
MEDFileField1TSStructItem2 MEDFileField1TSStructItem2::BuildAggregationOf(const std::vector<const MEDFileField1TSStructItem2 *>& objs, const MEDFileFieldGlobsReal *globs) throw(INTERP_KERNEL::Exception)
{
  if(objs.empty())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::BuildAggregationOf : empty input !");
  if(objs.size()==1)
    return MEDFileField1TSStructItem2(*objs[0]);
  INTERP_KERNEL::NormalizedCellType gt(objs[0]->_geo_type);
  int nbEntityRef(objs[0]->_nb_of_entity);
  std::size_t sz(objs.size());
  std::vector<const DataArrayInt *> arrs(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      const MEDFileField1TSStructItem2 *obj(objs[i]);
      if(gt!=obj->_geo_type)
        throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::BuildAggregationOf : invalid situation ! All input must have the same geo type !");
      if(nbEntityRef!=obj->_nb_of_entity)
        throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::BuildAggregationOf : invalid situation ! All input must have the global nb of entity !");
      if(obj->_pfl->getName().empty())
        throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::BuildAggregationOf : invalid situation ! Several same geo type chunk must all lie on profiles !");
      arrs[i]=globs->getProfile(obj->_pfl->getName().c_str());
    }
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> arr(DataArrayInt::Aggregate(arrs));
  arr->sort();
  int oldNbTuples(arr->getNumberOfTuples());
  arr=arr->buildUnique();
  if(oldNbTuples!=arr->getNumberOfTuples())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem2::BuildAggregationOf : some entities are present several times !");
  if(arr->isIdentity() && oldNbTuples==nbEntityRef)
    {
      std::pair<int,int> p(0,nbEntityRef);
      std::string a,b;
      MEDFileField1TSStructItem2 ret(gt,p,a,b);
      ret._nb_of_entity=nbEntityRef;
      return ret;
    }
  else
    {
      arr->setName(NEWLY_CREATED_PFL_NAME);
      std::pair<int,int> p(0,oldNbTuples);
      std::string a,b;
      MEDFileField1TSStructItem2 ret(gt,p,a,b);
      ret._nb_of_entity=nbEntityRef;
      ret._pfl=arr;
      return ret;
    }
}

std::size_t MEDFileField1TSStructItem2::getHeapMemorySize() const
{
  std::size_t ret(0);
  const DataArrayInt *pfl(_pfl);
  if(pfl)
    ret+=pfl->getHeapMemorySize();
  ret+=_loc.capacity();
  return ret;
}

//=

MEDFileField1TSStructItem::MEDFileField1TSStructItem(TypeOfField a, const std::vector< MEDFileField1TSStructItem2 >& b):_computed(false),_type(a),_items(b)
{
}

void MEDFileField1TSStructItem::checkWithMeshStruct(const MEDFileMeshStruct *mst, const MEDFileFieldGlobsReal *globs) throw(INTERP_KERNEL::Exception)
{
  switch(_type)
    {
    case ON_NODES:
      {
        int nbOfEnt=mst->getNumberOfNodes();
        if(_items.size()!=1)
          throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::checkWithMeshStruct : for nodes field only one subdivision supported !");
        _items[0].checkInRange(nbOfEnt,1,globs);
        break ;
      }
    case ON_CELLS:
      {
        for(std::vector< MEDFileField1TSStructItem2 >::iterator it=_items.begin();it!=_items.end();it++)
          (*it).checkWithMeshStructForCells(mst,globs);
        break;
      }
    case ON_GAUSS_NE:
      {
        for(std::vector< MEDFileField1TSStructItem2 >::iterator it=_items.begin();it!=_items.end();it++)
          (*it).checkWithMeshStructForGaussNE(mst,globs);
        break;
      }
    case ON_GAUSS_PT:
      {
        for(std::vector< MEDFileField1TSStructItem2 >::iterator it=_items.begin();it!=_items.end();it++)
          (*it).checkWithMeshStructForGaussPT(mst,globs);
        break;
      }
    default:
      throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::checkWithMeshStruct : not managed field type !");
    }
}

bool MEDFileField1TSStructItem::operator==(const MEDFileField1TSStructItem& other) const throw(INTERP_KERNEL::Exception)
{
  if(_type!=other._type)
    return false;
  if(_items.size()!=other._items.size())
    return false;
  for(std::size_t i=0;i<_items.size();i++)
    if(!(_items[i]==other._items[i]))
      return false;
  return true;
}

bool MEDFileField1TSStructItem::isCellSupportEqual(const MEDFileField1TSStructItem& other, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(_type!=other._type)
    return false;
  if(_items.size()!=other._items.size())
    return false;
  for(std::size_t i=0;i<_items.size();i++)
    if(!(_items[i].isCellSupportEqual(other._items[i],globs)))
      return false;
  return true;
}

bool MEDFileField1TSStructItem::isNodeSupportEqual(const MEDFileField1TSStructItem& other, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(_type!=other._type)
    return false;
  if(_items.size()!=other._items.size())
    return false;
  for(std::size_t i=0;i<_items.size();i++)
    if(!(_items[i].isNodeSupportEqual(other._items[i],globs)))
      return false;
  return true;
}

bool MEDFileField1TSStructItem::isEntityCell() const
{
  if(_type==ON_NODES)
    return false;
  else
    return true;
}

class CmpGeo
{
public:
  CmpGeo(INTERP_KERNEL::NormalizedCellType geoTyp):_geo_type(geoTyp) { }
  bool operator()(const std::pair< INTERP_KERNEL::NormalizedCellType, std::vector<std::size_t> > & v) const { return _geo_type==v.first; }
private:
  INTERP_KERNEL::NormalizedCellType _geo_type;
};

MEDFileField1TSStructItem MEDFileField1TSStructItem::simplifyMeOnCellEntity(const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(!isEntityCell())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::simplifyMeOnCellEntity : must be on ON_CELLS, ON_GAUSS_NE or ON_GAUSS_PT !");
  std::vector< std::pair< INTERP_KERNEL::NormalizedCellType, std::vector<std::size_t> > > m;
  std::size_t i=0;
  for(std::vector< MEDFileField1TSStructItem2 >::const_iterator it=_items.begin();it!=_items.end();it++,i++)
    {
      std::vector< std::pair< INTERP_KERNEL::NormalizedCellType, std::vector<std::size_t> > >::iterator it0(std::find_if(m.begin(),m.end(),CmpGeo((*it).getGeo())));
      if(it0==m.end())
        m.push_back(std::pair< INTERP_KERNEL::NormalizedCellType, std::vector<std::size_t> >((*it).getGeo(),std::vector<std::size_t>(1,i)));
      else
        (*it0).second.push_back(i);
    }
  if(m.size()==_items.size())
    {
      MEDFileField1TSStructItem ret(*this);
      ret._type=ON_CELLS;
      return ret;
    }
  std::size_t sz(m.size());
  std::vector< MEDFileField1TSStructItem2 > items(sz);
  for(i=0;i<sz;i++)
    {
      const std::vector<std::size_t>& ids=m[i].second;
      std::vector<const MEDFileField1TSStructItem2 *>objs(ids.size());
      for(std::size_t j=0;j<ids.size();j++)
        objs[j]=&_items[ids[j]];
      items[i]=MEDFileField1TSStructItem2::BuildAggregationOf(objs,globs);
    }
  MEDFileField1TSStructItem ret(ON_CELLS,items);
  ret._computed=true;
  return ret;
}

/*!
 * \a this is expected to be ON_CELLS and simplified.
 */
bool MEDFileField1TSStructItem::isCompatibleWithNodesDiscr(const MEDFileField1TSStructItem& other, const MEDFileMeshStruct *meshSt, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(other._type!=ON_NODES)
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::isCompatibleWithNodesDiscr : other must be on nodes !");
  if(other._items.size()!=1)
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::isCompatibleWithNodesDiscr : other is on nodes but number of subparts !");
  int theFirstLevFull;
  bool ret0=isFullyOnOneLev(meshSt,theFirstLevFull);
  const MEDFileField1TSStructItem2& otherNodeIt(other._items[0]);
  if(otherNodeIt.getPflName().empty())
    {//on all nodes
      if(!ret0)
        return false;
      return theFirstLevFull==0;
    }
  else
    {
      const DataArrayInt *pfl=globs->getProfile(otherNodeIt.getPflName().c_str());
      MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cpyPfl(pfl->deepCpy());
      cpyPfl->sort();
      int nbOfNodes(meshSt->getNumberOfNodes());
      if(cpyPfl->isIdentity() && cpyPfl->getNumberOfTuples()==nbOfNodes)
        {//on all nodes also !
          if(!ret0)
            return false;
          return theFirstLevFull==0;
        }
      std::vector<bool> nodesFetched(nbOfNodes,false);
      meshSt->getTheMesh()->whichAreNodesFetched(*this,globs,nodesFetched);
      return cpyPfl->isFittingWith(nodesFetched);
    }
}

bool MEDFileField1TSStructItem::isFullyOnOneLev(const MEDFileMeshStruct *meshSt, int& theFirstLevFull) const throw(INTERP_KERNEL::Exception)
{
  if(_type!=ON_CELLS)
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::isFullyOnOneLev : works only for ON_CELLS discretization !");
  if(_items.empty())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::isFullyOnOneLev : items vector is empty !");
  int nbOfLevs(meshSt->getNumberOfLevs());
  if(nbOfLevs==0)
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::isFullyOnOneLev : no levels in input mesh structure !");
  std::vector<int> levs(nbOfLevs);
  theFirstLevFull=1;
  int nbOfGT=0;
  std::set<INTERP_KERNEL::NormalizedCellType> gts;
  for(std::vector< MEDFileField1TSStructItem2 >::const_iterator it=_items.begin();it!=_items.end();it++)
    {
      if(!(*it).getPflName().empty())
        return false;
      INTERP_KERNEL::NormalizedCellType gt((*it).getGeo());
      if(gts.find(gt)!=gts.end())
        throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::isFullyOnOneLev : internal error !");
      gts.insert(gt);
      int pos(meshSt->getLevelOfGeoType((*it).getGeo()));
      levs[-pos]++;
    }
  for(int i=0;i<nbOfLevs;i++)
    if(meshSt->getNumberOfGeoTypesInLev(-i)==levs[i])
      { theFirstLevFull=-i; return true; }
  return false;
}

const MEDFileField1TSStructItem2& MEDFileField1TSStructItem::operator[](std::size_t i) const throw(INTERP_KERNEL::Exception)
{
  if(i<0 || i>=_items.size())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStructItem::operator[] : input is not in valid range !");
  return _items[i];
}

std::size_t MEDFileField1TSStructItem::getHeapMemorySize() const
{
  std::size_t ret(0);
  for(std::vector< MEDFileField1TSStructItem2 >::const_iterator it=_items.begin();it!=_items.end();it++)
    ret+=(*it).getHeapMemorySize();
  ret+=_items.size()*sizeof(MEDFileField1TSStructItem2);
  return ret;
}

MEDMeshMultiLev *MEDFileField1TSStructItem::buildFromScratchDataSetSupportOnCells(const MEDFileMeshStruct *mst, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  std::vector<INTERP_KERNEL::NormalizedCellType> a0;
  std::vector<const DataArrayInt *> a1;
  std::vector<int> a2;
  std::size_t i(0);
  for(std::vector< MEDFileField1TSStructItem2 >::const_iterator it=_items.begin();it!=_items.end();it++,i++)
    {
      a0[i]=(*it).getGeo();
      a1[i]=(*it).getPfl(globs);
      a2[i]=mst->getNumberOfElemsOfGeoType((*it).getGeo());
    }
  return MEDMeshMultiLev::New(mst->getTheMesh(),a0,a1,a2);
}

//=

MEDFileField1TSStruct *MEDFileField1TSStruct::New(const MEDFileAnyTypeField1TS *ref, MEDFileMeshStruct *mst) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileField1TSStruct(ref,mst);
}

MEDFileField1TSStruct::MEDFileField1TSStruct(const MEDFileAnyTypeField1TS *ref, MEDFileMeshStruct *mst)
{
  _already_checked.push_back(BuildItemFrom(ref,mst));
}

void MEDFileField1TSStruct::checkWithMeshStruct(MEDFileMeshStruct *mst, const MEDFileFieldGlobsReal *globs) throw(INTERP_KERNEL::Exception)
{
  if(_already_checked.empty())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::checkWithMeshStruct : not correctly initialized !");
  _already_checked.back().checkWithMeshStruct(mst,globs);
}

bool MEDFileField1TSStruct::isEqualConsideringThePast(const MEDFileAnyTypeField1TS *other, const MEDFileMeshStruct *mst) const throw(INTERP_KERNEL::Exception)
{
  MEDFileField1TSStructItem b(BuildItemFrom(other,mst));
  for(std::vector<MEDFileField1TSStructItem>::const_iterator it=_already_checked.begin();it!=_already_checked.end();it++)
    {
      if((*it)==b)
        return true;
    }
  return false;
}

/*!
 * Not const because \a other structure will be added to the \c _already_checked attribute in case of success.
 */
bool MEDFileField1TSStruct::isSupportSameAs(const MEDFileAnyTypeField1TS *other, const MEDFileMeshStruct *meshSt) throw(INTERP_KERNEL::Exception)
{
  if(_already_checked.empty())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::isSupportSameAs : no ref !");
  MEDFileField1TSStructItem b(BuildItemFrom(other,meshSt));
  if(!_already_checked[0].isEntityCell() || !b.isEntityCell())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::isSupportSameAs : only available on cell entities !");
  MEDFileField1TSStructItem other1(b.simplifyMeOnCellEntity(other));
  int found=-1,i=0;
  for(std::vector<MEDFileField1TSStructItem>::const_iterator it=_already_checked.begin();it!=_already_checked.end();it++,i++)
    if((*it).isComputed())
      { found=i; break; }
  bool ret(false);
  if(found==-1)
    {
      MEDFileField1TSStructItem this1(_already_checked[0].simplifyMeOnCellEntity(other));
      ret=this1.isCellSupportEqual(other1,other);
      if(ret)
        _already_checked.push_back(this1);
    }
  else
    ret=_already_checked[found].isCellSupportEqual(other1,other);
  if(ret)
    _already_checked.push_back(b);
  return ret;
}

bool MEDFileField1TSStruct::isCompatibleWithNodesDiscr(const MEDFileAnyTypeField1TS *other, const MEDFileMeshStruct *meshSt) throw(INTERP_KERNEL::Exception)
{
  if(_already_checked.empty())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::isCompatibleWithNodesDiscr : no ref !");
  if(!_already_checked[0].isEntityCell())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::isCompatibleWithNodesDiscr : only available on cell entities !");
  MEDFileField1TSStructItem other1(BuildItemFrom(other,meshSt));
  //
  int found=-1,i=0;
  for(std::vector<MEDFileField1TSStructItem>::const_iterator it=_already_checked.begin();it!=_already_checked.end();it++,i++)
    if((*it).isComputed())
      { found=i; break; }
  bool ret(false);
  if(found==-1)
    {
      MEDFileField1TSStructItem this1(_already_checked[0].simplifyMeOnCellEntity(other));
      ret=this1.isCompatibleWithNodesDiscr(other1,meshSt,other);
      if(ret)
        _already_checked.push_back(this1);
    }
  else
    ret=_already_checked[found].isCompatibleWithNodesDiscr(other1,meshSt,other);
  if(ret)
    _already_checked.push_back(other1);
  return ret;
}

std::size_t MEDFileField1TSStruct::getHeapMemorySize() const
{
  std::size_t ret(0);
  for(std::vector<MEDFileField1TSStructItem>::const_iterator it=_already_checked.begin();it!=_already_checked.end();it++)
    ret+=(*it).getHeapMemorySize();
  ret+=_already_checked.capacity()*sizeof(MEDFileField1TSStructItem);
  return ret;
}

MEDFileField1TSStructItem MEDFileField1TSStruct::BuildItemFrom(const MEDFileAnyTypeField1TS *ref, const MEDFileMeshStruct *meshSt)
{
  TypeOfField atype;
  std::vector< MEDFileField1TSStructItem2 > anItems;
  //
  std::vector< std::vector<std::string> > pfls,locs;
  std::vector< std::vector<TypeOfField> > typesF;
  std::vector<INTERP_KERNEL::NormalizedCellType> geoTypes;
  std::vector< std::vector<std::pair<int,int> > > strtEnds=ref->getFieldSplitedByType(0,geoTypes,typesF,pfls,locs);
  std::size_t nbOfGeoTypes(geoTypes.size());
  if(nbOfGeoTypes==0)
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct : not null by empty ref  !");
  bool isFirst=true;
  for(std::size_t i=0;i<nbOfGeoTypes;i++)
    {
      std::size_t sz=typesF[i].size();
      if(strtEnds[i].size()<1 || sz<1 || pfls[i].size()<1)
        throw INTERP_KERNEL::Exception("MEDFileField1TSStruct : internal error #1 !");
      //
      if(isFirst)
        atype=typesF[i][0];
      isFirst=false;
      //
      for(std::size_t j=0;j<sz;j++)
        {
          if(atype==typesF[i][j])
            anItems.push_back(MEDFileField1TSStructItem2(geoTypes[i],strtEnds[i][j],pfls[i][j],locs[i][j]));
          else
            throw INTERP_KERNEL::Exception("MEDFileField1TSStruct : can be applied only on single spatial discretization fields ! Call SplitPerDiscretization method !");
        }
    }
  MEDFileField1TSStructItem ret(atype,anItems);
  ret.checkWithMeshStruct(meshSt,ref);
  return ret;
}

MEDMeshMultiLev *MEDFileField1TSStruct::buildFromScratchDataSetSupport(const MEDFileMeshStruct *mst, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(_already_checked.empty())
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::buildFromScratchDataSetSupport : No outline structure in this !");
  int pos0(-1),pos1(-1);
  if(presenceOfCellDiscr(pos0))
    {
      MEDCouplingAutoRefCountObjectPtr<MEDMeshMultiLev> ret(_already_checked[pos0].buildFromScratchDataSetSupportOnCells(mst,globs));
      if(presenceOfPartialNodeDiscr(pos1))
        ret->setNodeReduction(_already_checked[pos1][0].getPfl(globs));
      return ret.retn();
    }
  else
    {
      if(!presenceOfPartialNodeDiscr(pos1))
        {//we have only all nodes, no cell definition info -> level 0;
          std::vector<int> levs(1,0);
          return MEDMeshMultiLev::New(mst->getTheMesh(),levs);
        }
    }
}

bool MEDFileField1TSStruct::isDataSetSupportFastlyEqualTo(const MEDFileField1TSStruct& other, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  int b0,b1;
  bool a0(presenceOfCellDiscr(b0)),a1(presenceOfPartialNodeDiscr(b1));
  int d0,d1;
  bool c0(other.presenceOfCellDiscr(d0)),c1(other.presenceOfPartialNodeDiscr(d1)); 
  if(a0!=c0 || a1!=c1)
    return false;
  if(a0)
    if(!_already_checked[b0].isCellSupportEqual(other._already_checked[d0],globs))
      return false;
  if(a1)
    if(!_already_checked[b1].isNodeSupportEqual(other._already_checked[d1],globs))
      return false;
  return true;
}

/*!
 * Returns true if presence in \a this of discretization ON_CELLS, ON_GAUSS_PT, ON_GAUSS_NE.
 * If true is returned the pos of the easiest is returned. The easiest is the first element in \a this having the less splitted subparts.
 */
bool MEDFileField1TSStruct::presenceOfCellDiscr(int& pos) const throw(INTERP_KERNEL::Exception)
{
  std::size_t refSz(std::numeric_limits<std::size_t>::max());
  bool ret(false);
  int i(0);
  for(std::vector<MEDFileField1TSStructItem>::const_iterator it=_already_checked.begin();it!=_already_checked.end();it++,i++)
    {
      if((*it).getType()!=ON_NODES)
        {
          ret=true;
          std::size_t sz((*it).getNumberOfItems());
          if(refSz>sz)
            { pos=i; refSz=sz; }
        }
    }
  if(refSz==0)
    throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::presenceOfCellDiscr : an element in this on entity CELL is empty !");
  return ret;
}

/*!
 * Returns true if presence in \a this of discretization ON_NODES.
 * If true is returned the pos of the first element containing the single subpart.
 */
bool MEDFileField1TSStruct::presenceOfPartialNodeDiscr(int& pos) const throw(INTERP_KERNEL::Exception)
{
  int i(0);
  for(std::vector<MEDFileField1TSStructItem>::const_iterator it=_already_checked.begin();it!=_already_checked.end();it++,i++)
    {
      if((*it).getType()==ON_NODES)
        {
          std::size_t sz((*it).getNumberOfItems());
          if(sz==1)
            {
              if(!(*it)[0].getPflName().empty())
                { pos=i; return true; }
            }
          else
            throw INTERP_KERNEL::Exception("MEDFileField1TSStruct::presenceOfPartialNodeDiscr : an element in this on entity NODE is split into several parts !");
        }
    }
  return false;
}

//=

MEDFileFastCellSupportComparator *MEDFileFastCellSupportComparator::New(const MEDFileMeshStruct *m, const MEDFileAnyTypeFieldMultiTS *ref) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileFastCellSupportComparator(m,ref);
}

MEDFileFastCellSupportComparator::MEDFileFastCellSupportComparator(const MEDFileMeshStruct *m, const MEDFileAnyTypeFieldMultiTS *ref)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDFileFastCellSupportComparator constructor : null input mesh struct !");
  _mesh_comp=const_cast<MEDFileMeshStruct *>(m); _mesh_comp->incrRef();
  int nbPts=ref->getNumberOfTS();
  _f1ts_cmps.resize(nbPts);
  for(int i=0;i<nbPts;i++)
    {
      MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeField1TS> elt=ref->getTimeStepAtPos(i);
      _f1ts_cmps[i]=MEDFileField1TSStruct::New(elt,_mesh_comp);
      _f1ts_cmps[i]->checkWithMeshStruct(_mesh_comp,elt);
    }
}

std::size_t MEDFileFastCellSupportComparator::getHeapMemorySize() const
{
  std::size_t ret(0);
  const MEDFileMeshStruct *mst(_mesh_comp);
  if(mst)
    ret+=mst->getHeapMemorySize();
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileField1TSStruct> >::const_iterator it=_f1ts_cmps.begin();it!=_f1ts_cmps.end();it++)
    {
      const MEDFileField1TSStruct *cur(*it);
      if(cur)
        ret+=cur->getHeapMemorySize()+sizeof(MEDFileField1TSStruct);
    }
  ret+=_f1ts_cmps.capacity()*sizeof(MEDCouplingAutoRefCountObjectPtr<MEDFileField1TSStruct>);
  return ret;
}

bool MEDFileFastCellSupportComparator::isEqual(const MEDFileAnyTypeFieldMultiTS *other) throw(INTERP_KERNEL::Exception)
{
  int nbPts=other->getNumberOfTS();
  if(nbPts!=(int)_f1ts_cmps.size())
    {
      std::ostringstream oss; oss << "MEDFileFastCellSupportComparator::isEqual : unexpected nb of time steps in  input ! Should be " << _f1ts_cmps.size() << " it is in reality " << nbPts << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  for(int i=0;i<nbPts;i++)
    {
      MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeField1TS> elt=other->getTimeStepAtPos(i);
      if(!_f1ts_cmps[i]->isEqualConsideringThePast(elt,_mesh_comp))
        if(!_f1ts_cmps[i]->isSupportSameAs(elt,_mesh_comp))
          return false;
    }
  return true;
}

bool MEDFileFastCellSupportComparator::isCompatibleWithNodesDiscr(const MEDFileAnyTypeFieldMultiTS *other) throw(INTERP_KERNEL::Exception)
{
  int nbPts=other->getNumberOfTS();
  if(nbPts!=(int)_f1ts_cmps.size())
    {
      std::ostringstream oss; oss << "MEDFileFastCellSupportComparator::isCompatibleWithNodesDiscr : unexpected nb of time steps in  input ! Should be " << _f1ts_cmps.size() << " it is in reality " << nbPts << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  for(int i=0;i<nbPts;i++)
    {
      MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeField1TS> elt=other->getTimeStepAtPos(i);
      if(!_f1ts_cmps[i]->isCompatibleWithNodesDiscr(elt,_mesh_comp))
        return false;
    }
  return true;
}

MEDMeshMultiLev *MEDFileFastCellSupportComparator::buildFromScratchDataSetSupport(int timeStepId, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(timeStepId<0 || timeStepId>=(int)_f1ts_cmps.size())
    {
      std::ostringstream oss; oss << "MEDFileFastCellSupportComparator::buildFromScratchDataSetSupport : requested time step id #" << timeStepId << " is not in [0," << _f1ts_cmps.size() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  const MEDFileField1TSStruct *obj(_f1ts_cmps[timeStepId]);
  if(!obj)
    {
      std::ostringstream oss; oss << "MEDFileFastCellSupportComparator::buildFromScratchDataSetSupport : at time step id #" << timeStepId << " no field structure overview defined !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  return obj->buildFromScratchDataSetSupport(_mesh_comp,globs);
}

bool MEDFileFastCellSupportComparator::isDataSetSupportEqualToThePreviousOne(int timeStepId, const MEDFileFieldGlobsReal *globs) const throw(INTERP_KERNEL::Exception)
{
  if(timeStepId<=0 || timeStepId>=(int)_f1ts_cmps.size())
    {
      std::ostringstream oss; oss << "MEDFileFastCellSupportComparator::isDataSetSupportEqualToThePreviousOne : requested time step id #" << timeStepId << " is not in [1," << _f1ts_cmps.size() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  const MEDFileField1TSStruct *obj(_f1ts_cmps[timeStepId]);
  const MEDFileField1TSStruct *objRef(_f1ts_cmps[timeStepId-1]);
  return objRef->isDataSetSupportFastlyEqualTo(*obj,globs);
}

//=

std::size_t MEDMeshMultiLev::getHeapMemorySize() const
{
  return 0;
}

MEDMeshMultiLev *MEDMeshMultiLev::New(const MEDFileMesh *m, const std::vector<int>& levs) throw(INTERP_KERNEL::Exception)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDMeshMultiLev::New : null input pointer !");
  const MEDFileUMesh *um(dynamic_cast<const MEDFileUMesh *>(m));
  if(um)
    return MEDUMeshMultiLev::New(um,levs);
  const MEDFileCMesh *cm(dynamic_cast<const MEDFileCMesh *>(m));
  if(cm)
    return MEDCMeshMultiLev::New(cm,levs);
  const MEDFileCurveLinearMesh *clm(dynamic_cast<const MEDFileCurveLinearMesh *>(m));
  if(clm)
    return MEDCurveLinearMeshMultiLev::New(clm,levs);
  throw INTERP_KERNEL::Exception("MEDMeshMultiLev::New : unrecognized type of mesh ! Must be in [MEDFileUMesh,MEDFileCMesh,MEDFileCurveLinearMesh] !");
}

MEDMeshMultiLev *MEDMeshMultiLev::New(const MEDFileMesh *m, const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities) throw(INTERP_KERNEL::Exception)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDMeshMultiLev::New 2 : null input pointer !");
  const MEDFileUMesh *um(dynamic_cast<const MEDFileUMesh *>(m));
  if(um)
    return MEDUMeshMultiLev::New(um,gts,pfls,nbEntities);
  const MEDFileCMesh *cm(dynamic_cast<const MEDFileCMesh *>(m));
  if(cm)
    return MEDCMeshMultiLev::New(cm,gts,pfls,nbEntities);
  const MEDFileCurveLinearMesh *clm(dynamic_cast<const MEDFileCurveLinearMesh *>(m));
  if(clm)
    return MEDCurveLinearMeshMultiLev::New(clm,gts,pfls,nbEntities);
  throw INTERP_KERNEL::Exception("MEDMeshMultiLev::New 2 : unrecognized type of mesh ! Must be in [MEDFileUMesh,MEDFileCMesh,MEDFileCurveLinearMesh] !");
}

void MEDMeshMultiLev::setNodeReduction(const DataArrayInt *nr)
{
  if(nr)
    nr->incrRef();
  _node_reduction=const_cast<DataArrayInt*>(nr);
}

MEDMeshMultiLev::MEDMeshMultiLev()
{
}

MEDMeshMultiLev::MEDMeshMultiLev(const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities):_geo_types(gts),_nb_entities(nbEntities)
{
  std::size_t sz(_geo_types.size());
  if(sz!=pfls.size() || sz!=nbEntities.size())
    throw INTERP_KERNEL::Exception("MEDMeshMultiLev::MEDMeshMultiLev : input vector must have the same size !");
  _pfls.resize(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      if(pfls[i])
        pfls[i]->incrRef();
      _pfls[i]=const_cast<DataArrayInt *>(pfls[i]);
    }
}

MEDUMeshMultiLev *MEDUMeshMultiLev::New(const MEDFileUMesh *m, const std::vector<int>& levs) throw(INTERP_KERNEL::Exception)
{
  return new MEDUMeshMultiLev(m,levs);
}

MEDUMeshMultiLev::MEDUMeshMultiLev(const MEDFileUMesh *m, const std::vector<int>& levs)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDUMeshMultiLev constructor : null input pointer !");
  std::vector<MEDCoupling1GTUMesh *> v;
  for(std::vector<int>::const_iterator it=levs.begin();it!=levs.end();it++)
    {
      std::vector<MEDCoupling1GTUMesh *> vTmp(m->getDirectUndergroundSingleGeoTypeMeshes(*it));
      v.insert(v.end(),vTmp.begin(),vTmp.end());
    }
  std::size_t sz(v.size());
  _parts.resize(sz);
  _pfls.resize(sz);
  _geo_types.resize(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      MEDCoupling1GTUMesh *obj(v[i]);
      if(obj)
        obj->incrRef();
      else
        throw INTERP_KERNEL::Exception("MEDUMeshMultiLev constructor : presence of a null pointer !");
      _parts[i]=obj;
      _geo_types[i]=obj->getCellModelEnum();
    }
}

MEDUMeshMultiLev *MEDUMeshMultiLev::New(const MEDFileUMesh *m, const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities) throw(INTERP_KERNEL::Exception)
{
  return new MEDUMeshMultiLev(m,gts,pfls,nbEntities);
}

MEDUMeshMultiLev::MEDUMeshMultiLev(const MEDFileUMesh *m, const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities):MEDMeshMultiLev(gts,pfls,nbEntities)
{
  std::size_t sz(gts.size());
  _parts.resize(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      MEDCoupling1GTUMesh *elt(m->getDirectUndergroundSingleGeoTypeMesh(gts[i]));
      if(elt)
        elt->incrRef();
      _parts[i]=elt;
    }
}

MEDCMeshMultiLev *MEDCMeshMultiLev::New(const MEDFileCMesh *m, const std::vector<int>& levs) throw(INTERP_KERNEL::Exception)
{
  return new MEDCMeshMultiLev(m,levs);
}

MEDCMeshMultiLev *MEDCMeshMultiLev::New(const MEDFileCMesh *m, const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities) throw(INTERP_KERNEL::Exception)
{
  return new MEDCMeshMultiLev(m,gts,pfls,nbEntities);
}

MEDCMeshMultiLev::MEDCMeshMultiLev(const MEDFileCMesh *m, const std::vector<int>& levs)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDCMeshMultiLev constructor : null input pointer !");
  if(levs.size()!=1 || levs[0]!=0)
    throw INTERP_KERNEL::Exception("MEDCMeshMultiLev constructor : levels supported is 0 only !");
  int mdim(MEDCouplingStructuredMesh::GetGeoTypeGivenMeshDimension(m->getMeshDimension()));
  _coords.resize(mdim);
  for(int i=0;i<mdim;i++)
    {
      DataArrayDouble *elt(const_cast<DataArrayDouble *>(m->getMesh()->getCoordsAt(i)));
      if(!elt)
        throw INTERP_KERNEL::Exception("MEDCMeshMultiLev constructor 2 : presence of null pointer for an vector of double along an axis !");
      _coords[i]=elt;
    }
}

MEDCMeshMultiLev::MEDCMeshMultiLev(const MEDFileCMesh *m, const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities):MEDMeshMultiLev(gts,pfls,nbEntities)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDCMeshMultiLev constructor 2 : null input pointer !");
  if(gts.size()!=1 || pfls.size()!=1)
    throw INTERP_KERNEL::Exception("MEDCMeshMultiLev constructor 2 : lengthes of gts and pfls must be equal to one !");
  int mdim(MEDCouplingStructuredMesh::GetGeoTypeGivenMeshDimension(m->getMeshDimension()));
  if(mdim!=gts[0])
    throw INTERP_KERNEL::Exception("MEDCMeshMultiLev constructor 2 : the unique geo type is invalid regarding meshdim !");
  _coords.resize(mdim);
  for(int i=0;i<mdim;i++)
    {
      DataArrayDouble *elt(const_cast<DataArrayDouble *>(m->getMesh()->getCoordsAt(i)));
      if(!elt)
        throw INTERP_KERNEL::Exception("MEDCMeshMultiLev constructor 2 : presence of null pointer for an vector of double along an axis !");
      _coords[i]=elt;
    }
}

MEDCurveLinearMeshMultiLev *MEDCurveLinearMeshMultiLev::New(const MEDFileCurveLinearMesh *m, const std::vector<int>& levs) throw(INTERP_KERNEL::Exception)
{
  return new MEDCurveLinearMeshMultiLev(m,levs);
}

MEDCurveLinearMeshMultiLev *MEDCurveLinearMeshMultiLev::New(const MEDFileCurveLinearMesh *m, const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities) throw(INTERP_KERNEL::Exception)
{
  return new MEDCurveLinearMeshMultiLev(m,gts,pfls,nbEntities);
}

MEDCurveLinearMeshMultiLev::MEDCurveLinearMeshMultiLev(const MEDFileCurveLinearMesh *m, const std::vector<int>& levs)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDCurveLinearMeshMultiLev constructor : null input pointer !");
  if(levs.size()!=1 || levs[0]!=0)
    throw INTERP_KERNEL::Exception("MEDCurveLinearMeshMultiLev constructor : levels supported is 0 only !");
  DataArrayDouble *coords(const_cast<DataArrayDouble *>(m->getMesh()->getCoords()));
  if(!coords)
    throw INTERP_KERNEL::Exception("MEDCurveLinearMeshMultiLev constructor 2 : no coords set !");
  coords->incrRef();
  _coords=coords;
  _structure=m->getMesh()->getNodeGridStructure();
}

MEDCurveLinearMeshMultiLev::MEDCurveLinearMeshMultiLev(const MEDFileCurveLinearMesh *m, const std::vector<INTERP_KERNEL::NormalizedCellType>& gts, const std::vector<const DataArrayInt *>& pfls, const std::vector<int>& nbEntities):MEDMeshMultiLev(gts,pfls,nbEntities)
{
  if(!m)
    throw INTERP_KERNEL::Exception("MEDCurveLinearMeshMultiLev constructor 2 : null input pointer !");
  if(gts.size()!=1 || pfls.size()!=1)
    throw INTERP_KERNEL::Exception("MEDCurveLinearMeshMultiLev constructor 2 : lengthes of gts and pfls must be equal to one !");
  int mdim(MEDCouplingStructuredMesh::GetGeoTypeGivenMeshDimension(m->getMeshDimension()));
  if(mdim!=gts[0])
    throw INTERP_KERNEL::Exception("MEDCurveLinearMeshMultiLev constructor 2 : the unique geo type is invalid regarding meshdim !");
  DataArrayDouble *coords(const_cast<DataArrayDouble *>(m->getMesh()->getCoords()));
  if(!coords)
    throw INTERP_KERNEL::Exception("MEDCurveLinearMeshMultiLev constructor 2 : no coords set !");
  coords->incrRef();
  _coords=coords;
  _structure=m->getMesh()->getNodeGridStructure();
}