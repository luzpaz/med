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

#include "MEDCoupling1GTUMesh.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"

#include "SplitterTetra.hxx"

using namespace ParaMEDMEM;

MEDCoupling1GTUMesh::MEDCoupling1GTUMesh(const char *name, const INTERP_KERNEL::CellModel& cm):_cm(&cm)
{
  setName(name);
}

MEDCoupling1GTUMesh::MEDCoupling1GTUMesh(const MEDCoupling1GTUMesh& other, bool recDeepCpy):MEDCouplingPointSet(other,recDeepCpy),_cm(other._cm)
{
}

MEDCoupling1GTUMesh *MEDCoupling1GTUMesh::New(const char *name, INTERP_KERNEL::NormalizedCellType type) throw(INTERP_KERNEL::Exception)
{
  if(type==INTERP_KERNEL::NORM_ERROR)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::New : NORM_ERROR is not a valid type to be used as base geometric type for a mesh !");
  const INTERP_KERNEL::CellModel& cm=INTERP_KERNEL::CellModel::GetCellModel(type);
  if(!cm.isDynamic())
    return MEDCoupling1SGTUMesh::New(name,type);
  else
    return MEDCoupling1DGTUMesh::New(name,type);
}

const INTERP_KERNEL::CellModel& MEDCoupling1GTUMesh::getCellModel() const throw(INTERP_KERNEL::Exception)
{
  return *_cm;
}

INTERP_KERNEL::NormalizedCellType MEDCoupling1GTUMesh::getCellModelEnum() const throw(INTERP_KERNEL::Exception)
{
  return _cm->getEnum();
}

int MEDCoupling1GTUMesh::getMeshDimension() const
{
  return (int)_cm->getDimension();
}

/*!
 * This method returns a newly allocated array containing cell ids (ascendingly sorted) whose geometric type are equal to type.
 * This method does not throw exception if geometric type \a type is not in \a this.
 * This method throws an INTERP_KERNEL::Exception if meshdimension of \b this is not equal to those of \b type.
 * The coordinates array is not considered here.
 *
 * \param [in] type the geometric type
 * \return cell ids in this having geometric type \a type.
 */
DataArrayInt *MEDCoupling1GTUMesh::giveCellsWithType(INTERP_KERNEL::NormalizedCellType type) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  if(type==getCellModelEnum())
    ret->alloc(getNumberOfCells(),1);
  else
    ret->alloc(0,1);
  ret->iota();
  return ret.retn();
}

/*!
 * Returns nb of cells having the geometric type \a type. No throw if no cells in \a this has the geometric type \a type.
 */
int MEDCoupling1GTUMesh::getNumberOfCellsWithType(INTERP_KERNEL::NormalizedCellType type) const
{
  return type==getCellModelEnum()?getNumberOfCells():0;
}

/*!
 * Returns a type of a cell by its id.
 *  \param [in] cellId - the id of the cell of interest.
 *  \return INTERP_KERNEL::NormalizedCellType - enumeration item describing the cell type.
 *  \throw If \a cellId is invalid. Valid range is [0, \a this->getNumberOfCells() ).
 */
INTERP_KERNEL::NormalizedCellType MEDCoupling1GTUMesh::getTypeOfCell(int cellId) const
{
  if(cellId>=0 && cellId<getNumberOfCells())
    return getCellModelEnum();
  std::ostringstream oss; oss << "MEDCoupling1GTUMesh::getTypeOfCell : Requesting type of cell #" << cellId << " but it should be in [0," << getNumberOfCells() << ") !";
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

/*!
 * Returns a set of all cell types available in \a this mesh.
 * \return std::set<INTERP_KERNEL::NormalizedCellType> - the set of cell types.
 * \warning this method does not throw any exception even if \a this is not defined.
 */
std::set<INTERP_KERNEL::NormalizedCellType> MEDCoupling1GTUMesh::getAllGeoTypes() const
{
  std::set<INTERP_KERNEL::NormalizedCellType> ret;
  ret.insert(getCellModelEnum());
  return ret;
}

/*!
 * This method expects that \a this is sorted by types. If not an exception will be thrown.
 * This method returns in the same format as code (see MEDCouplingUMesh::checkTypeConsistencyAndContig or MEDCouplingUMesh::splitProfilePerType) how
 * \a this is composed in cell types.
 * The returned array is of size 3*n where n is the number of different types present in \a this. 
 * For every k in [0,n] ret[3*k+2]==0 because it has no sense here. 
 * This parameter is kept only for compatibility with other methode listed above.
 */
std::vector<int> MEDCoupling1GTUMesh::getDistributionOfTypes() const throw(INTERP_KERNEL::Exception)
{
  std::vector<int> ret(3);
  ret[0]=(int)getCellModelEnum(); ret[1]=getNumberOfCells(); ret[2]=0;
  return ret;
}

/*!
 * This method is the opposite of MEDCouplingUMesh::checkTypeConsistencyAndContig method. Given a list of cells in \a profile it returns a list of sub-profiles sorted by geo type.
 * The result is put in the array \a idsPerType. In the returned parameter \a code, foreach i \a code[3*i+2] refers (if different from -1) to a location into the \a idsPerType.
 * This method has 1 input \a profile and 3 outputs \a code \a idsInPflPerType and \a idsPerType.
 * 
 * \param [out] code is a vector of size 3*n where n is the number of different geometric type in \a this \b reduced to the profile \a profile. \a code has exactly the same semantic than in MEDCouplingUMesh::checkTypeConsistencyAndContig method.
 * \param [out] idsInPflPerType is a vector of size of different geometric type in the subpart defined by \a profile of \a this ( equal to \a code.size()/3). For each i,
 *              \a idsInPflPerType[i] stores the tuple ids in \a profile that correspond to the geometric type code[3*i+0]
 * \param [out] idsPerType is a vector of size of different sub profiles needed to be defined to represent the profile \a profile for a given geometric type.
 *              This vector can be empty in case of all geometric type cells are fully covered in ascending in the given input \a profile.
 * 
 * \warning for performance reasons no deep copy will be performed, if \a profile can been used as this in output parameters \a idsInPflPerType and \a idsPerType.
 *
 * \throw if \a profile has not exactly one component. It throws too, if \a profile contains some values not in [0,getNumberOfCells()) or if \a this is not fully defined
 *
 *  \b Example1: <br>
 *          - Before \a this has 3 cells \a profile contains [0,1,2]
 *          - After \a code contains [NORM_...,nbCells,-1], \a idsInPflPerType [[0,1,2]] and \a idsPerType is empty <br>
 * 
 *  \b Example2: <br>
 *          - Before \a this has 3 cells \a profile contains [1,2]
 *          - After \a code contains [NORM_...,nbCells,0], \a idsInPflPerType [[0,1]] and \a idsPerType is [[1,2]] <br>

 */
void MEDCoupling1GTUMesh::splitProfilePerType(const DataArrayInt *profile, std::vector<int>& code, std::vector<DataArrayInt *>& idsInPflPerType, std::vector<DataArrayInt *>& idsPerType) const throw(INTERP_KERNEL::Exception)
{
  if(!profile)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::splitProfilePerType : input profile is NULL !");
  if(profile->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::splitProfilePerType : input profile should have exactly one component !");
  int nbTuples=profile->getNumberOfTuples();
  int nbOfCells=getNumberOfCells();
  code.resize(3); idsInPflPerType.resize(1);
  code[0]=(int)getCellModelEnum(); code[1]=nbTuples;
  idsInPflPerType.resize(1);
  if(profile->isIdentity() && nbTuples==nbOfCells)
    {
      code[2]=-1;
      idsInPflPerType[0]=const_cast<DataArrayInt *>(profile); idsInPflPerType[0]->incrRef();
      idsPerType.clear();
      return ;
    }
  code[2]=0;
  profile->checkAllIdsInRange(0,nbOfCells);
  idsPerType.resize(1);
  idsPerType[0]=const_cast<DataArrayInt *>(profile); idsPerType[0]->incrRef();
  idsInPflPerType[0]=DataArrayInt::Range(0,nbTuples,1);
}

/*!
 * This method tries to minimize at most the number of deep copy.
 * So if \a idsPerType is not empty it can be returned directly (without copy, but with ref count incremented) in return.
 * 
 * \sa MEDCouplingUMesh::checkTypeConsistencyAndContig
 */
DataArrayInt *MEDCoupling1GTUMesh::checkTypeConsistencyAndContig(const std::vector<int>& code, const std::vector<const DataArrayInt *>& idsPerType) const throw(INTERP_KERNEL::Exception)
{
  int nbOfCells=getNumberOfCells();
  if(code.size()!=3)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::checkTypeConsistencyAndContig : invalid input code should be exactly of size 3 !");
  if(code[0]!=(int)getCellModelEnum())
    {
      std::ostringstream oss; oss << "MEDCoupling1GTUMesh::checkTypeConsistencyAndContig : Mismatch of geometric type ! Asking for " << code[0] << " whereas the geometric type is \a this is " << getCellModelEnum() << " (" << _cm->getRepr() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(code[2]==-1)
    {
      if(code[1]==nbOfCells)
        return 0;
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1GTUMesh::checkTypeConsistencyAndContig : mismatch between the number of cells in this (" << nbOfCells << ") and the number of non profile (" << code[1] << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  if(code[2]!=0)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::checkTypeConsistencyAndContig : single geo type mesh ! 0 or -1 is expected at pos #2 of input code !");
  if(idsPerType.size()!=1)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::checkTypeConsistencyAndContig : input code points to DataArrayInt #0 whereas the size of idsPerType is not equal to 1 !");
  const DataArrayInt *pfl=idsPerType[0];
  if(!pfl)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::checkTypeConsistencyAndContig : the input code points to a NULL DataArrayInt at rank 0 !");
  if(pfl->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::checkTypeConsistencyAndContig : input profile should have exactly one component !");
  pfl->checkAllIdsInRange(0,nbOfCells);
  pfl->incrRef();
  return const_cast<DataArrayInt *>(pfl);
}

void MEDCoupling1GTUMesh::writeVTKLL(std::ostream& ofs, const std::string& cellData, const std::string& pointData) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  m->writeVTKLL(ofs,cellData,pointData);
}

std::string MEDCoupling1GTUMesh::getVTKDataSetType() const throw(INTERP_KERNEL::Exception)
{
  return std::string("UnstructuredGrid");
}

std::size_t MEDCoupling1GTUMesh::getHeapMemorySize() const
{
  return MEDCouplingPointSet::getHeapMemorySize();
}

bool MEDCoupling1GTUMesh::isEqualIfNotWhy(const MEDCouplingMesh *other, double prec, std::string& reason) const throw(INTERP_KERNEL::Exception)
{
  if(!MEDCouplingPointSet::isEqualIfNotWhy(other,prec,reason))
    return false;
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::isEqualIfNotWhy : input other pointer is null !");
  const MEDCoupling1GTUMesh *otherC=dynamic_cast<const MEDCoupling1GTUMesh *>(other);
  if(!otherC)
    {
      reason="mesh given in input is not castable in MEDCouplingSGTUMesh !";
      return false;
    }
  if(_cm!=otherC->_cm)
    {
      reason="mismatch in geometric type !";
      return false;
    }
  return true;
}

bool MEDCoupling1GTUMesh::isEqualWithoutConsideringStr(const MEDCouplingMesh *other, double prec) const
{
  if(!MEDCouplingPointSet::isEqualWithoutConsideringStr(other,prec))
    return false;
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1GTUMesh::isEqualWithoutConsideringStr : input other pointer is null !");
  const MEDCoupling1GTUMesh *otherC=dynamic_cast<const MEDCoupling1GTUMesh *>(other);
  if(!otherC)
    return false;
  if(_cm!=otherC->_cm)
    return false;
  return true;
}

void MEDCoupling1GTUMesh::checkCoherency() const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingPointSet::checkCoherency();
}

DataArrayDouble *MEDCoupling1GTUMesh::getBarycenterAndOwner() const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=m->getBarycenterAndOwner();
  return ret.retn();
}

MEDCouplingFieldDouble *MEDCoupling1GTUMesh::getMeasureField(bool isAbs) const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> ret=m->getMeasureField(isAbs);
  ret->setMesh(this);
  return ret.retn();
}

MEDCouplingFieldDouble *MEDCoupling1GTUMesh::getMeasureFieldOnNode(bool isAbs) const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> ret=m->getMeasureFieldOnNode(isAbs);
  ret->setMesh(this);
  return ret.retn();
}

/*!
 * to improve perf !
 */
int MEDCoupling1GTUMesh::getCellContainingPoint(const double *pos, double eps) const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  return m->getCellContainingPoint(pos,eps);
}

MEDCouplingFieldDouble *MEDCoupling1GTUMesh::buildOrthogonalField() const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> ret=m->buildOrthogonalField();
  ret->setMesh(this);
  return ret.retn();
}

DataArrayInt *MEDCoupling1GTUMesh::getCellsInBoundingBox(const double *bbox, double eps) const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  return m->getCellsInBoundingBox(bbox,eps);
}

DataArrayInt *MEDCoupling1GTUMesh::getCellsInBoundingBox(const INTERP_KERNEL::DirectedBoundingBox& bbox, double eps)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  return m->getCellsInBoundingBox(bbox,eps);
}

MEDCouplingPointSet *MEDCoupling1GTUMesh::buildFacePartOfMySelfNode(const int *start, const int *end, bool fullyIn) const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  return m->buildFacePartOfMySelfNode(start,end,fullyIn);
}

DataArrayInt *MEDCoupling1GTUMesh::findBoundaryNodes() const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  return m->findBoundaryNodes();
}

MEDCouplingPointSet *MEDCoupling1GTUMesh::buildBoundaryMesh(bool keepCoords) const
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  return m->buildBoundaryMesh(keepCoords);
}

void MEDCoupling1GTUMesh::findCommonCells(int compType, int startCellId, DataArrayInt *& commonCellsArr, DataArrayInt *& commonCellsIArr) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> m=buildUnstructured();
  m->findCommonCells(compType,startCellId,commonCellsArr,commonCellsIArr);
}

//==

MEDCoupling1SGTUMesh::MEDCoupling1SGTUMesh(const MEDCoupling1SGTUMesh& other, bool recDeepCpy):MEDCoupling1GTUMesh(other,recDeepCpy),_conn(other._conn)
{
  if(recDeepCpy)
    {
      const DataArrayInt *c(other._conn);
      if(c)
        _conn=c->deepCpy();
    }
}

MEDCoupling1SGTUMesh::MEDCoupling1SGTUMesh(const char *name, const INTERP_KERNEL::CellModel& cm):MEDCoupling1GTUMesh(name,cm)
{
}

MEDCoupling1SGTUMesh *MEDCoupling1SGTUMesh::New(const char *name, INTERP_KERNEL::NormalizedCellType type) throw(INTERP_KERNEL::Exception)
{
  if(type==INTERP_KERNEL::NORM_ERROR)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::New : NORM_ERROR is not a valid type to be used as base geometric type for a mesh !");
  const INTERP_KERNEL::CellModel& cm=INTERP_KERNEL::CellModel::GetCellModel(type);
  if(cm.isDynamic())
    {
      std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::New : the input geometric type " << cm.getRepr() << " is dynamic ! Only static types are allowed here !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  return new MEDCoupling1SGTUMesh(name,cm);
}

MEDCoupling1SGTUMesh *MEDCoupling1SGTUMesh::clone(bool recDeepCpy) const
{
  return new MEDCoupling1SGTUMesh(*this,recDeepCpy);
}

void MEDCoupling1SGTUMesh::shallowCopyConnectivityFrom(const MEDCouplingPointSet *other) throw(INTERP_KERNEL::Exception)
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::shallowCopyConnectivityFrom : input pointer is null !");
  const MEDCoupling1SGTUMesh *otherC=dynamic_cast<const MEDCoupling1SGTUMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::shallowCopyConnectivityFrom : input pointer is not an MEDCoupling1SGTUMesh instance !");
  setNodalConnectivity(otherC->getNodalConnectivity());
}

void MEDCoupling1SGTUMesh::updateTime() const
{
  MEDCoupling1GTUMesh::updateTime();
  const DataArrayInt *c(_conn);
  if(c)
    updateTimeWith(*c);
}

std::size_t MEDCoupling1SGTUMesh::getHeapMemorySize() const
{
  std::size_t ret=0;
  const DataArrayInt *c(_conn);
  if(c)
    ret+=c->getHeapMemorySize();
  return MEDCoupling1GTUMesh::getHeapMemorySize()+ret;
}

MEDCouplingMesh *MEDCoupling1SGTUMesh::deepCpy() const
{
  return clone(true);
}

bool MEDCoupling1SGTUMesh::isEqualIfNotWhy(const MEDCouplingMesh *other, double prec, std::string& reason) const throw(INTERP_KERNEL::Exception)
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::isEqualIfNotWhy : input other pointer is null !");
  std::ostringstream oss; oss.precision(15);
  const MEDCoupling1SGTUMesh *otherC=dynamic_cast<const MEDCoupling1SGTUMesh *>(other);
  if(!otherC)
    {
      reason="mesh given in input is not castable in MEDCoupling1SGTUMesh !";
      return false;
    }
  if(!MEDCoupling1GTUMesh::isEqualIfNotWhy(other,prec,reason))
    return false;
  const DataArrayInt *c1(_conn),*c2(otherC->_conn);
  if(c1==c2)
    return true;
  if(!c1 || !c2)
    {
      reason="in connectivity of single static geometric type exactly one among this and other is null !";
      return false;
    }
  if(!c1->isEqualIfNotWhy(*c2,reason))
    {
      reason.insert(0,"Nodal connectivity DataArrayInt differ : ");
      return false;
    }
  return true;
}

bool MEDCoupling1SGTUMesh::isEqualWithoutConsideringStr(const MEDCouplingMesh *other, double prec) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::isEqualWithoutConsideringStr : input other pointer is null !");
  const MEDCoupling1SGTUMesh *otherC=dynamic_cast<const MEDCoupling1SGTUMesh *>(other);
  if(!otherC)
    return false;
  if(!MEDCoupling1GTUMesh::isEqualWithoutConsideringStr(other,prec))
    return false;
  const DataArrayInt *c1(_conn),*c2(otherC->_conn);
  if(c1==c2)
    return true;
  if(!c1 || !c2)
    return false;
  if(!c1->isEqualWithoutConsideringStr(*c2))
    return false;
  return true;
}

void MEDCoupling1SGTUMesh::checkCoherency() const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingPointSet::checkCoherency();
  const DataArrayInt *c1(_conn);
  if(c1)
    {
      if(c1->getNumberOfComponents()!=1)
        throw INTERP_KERNEL::Exception("Nodal connectivity array is expected to be with number of components set to one !");
      if(c1->getInfoOnComponent(0)!="")
        throw INTERP_KERNEL::Exception("Nodal connectivity array is expected to have no info on its single component !");
      c1->checkAllocated();
    }
  else
    throw INTERP_KERNEL::Exception("Nodal connectivity array not defined !");
}

void MEDCoupling1SGTUMesh::checkCoherency1(double eps) const throw(INTERP_KERNEL::Exception)
{
  checkCoherency();
  const DataArrayInt *c1(_conn);
  int nbOfTuples=c1->getNumberOfTuples();
  int nbOfNodesPerCell=(int)_cm->getNumberOfNodes();
  if(nbOfTuples%nbOfNodesPerCell!=0)
    {
      std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::checkCoherency1 : the nb of tuples in conn is " << nbOfTuples << " and number of nodes per cell is " << nbOfNodesPerCell << ". But " << nbOfTuples << "%" << nbOfNodesPerCell << " !=0 !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  int nbOfNodes=getNumberOfNodes();
  int nbOfCells=nbOfTuples/nbOfNodesPerCell;
  const int *w(c1->begin());
  for(int i=0;i<nbOfCells;i++)
    for(int j=0;j<nbOfNodesPerCell;j++,w++)
      {
        if(*w<0 || *w>=nbOfNodes)
          {
            std::ostringstream oss; oss << "At node #" << j << " of  cell #" << i << ", is equal to " << *w << " must be in [0," << nbOfNodes << ") !";
            throw INTERP_KERNEL::Exception(oss.str().c_str());
          }
      }
}

void MEDCoupling1SGTUMesh::checkCoherency2(double eps) const throw(INTERP_KERNEL::Exception)
{
  checkCoherency1(eps);
}

int MEDCoupling1SGTUMesh::getNumberOfCells() const
{
  int nbOfTuples=getNodalConnectivityLength();
  int nbOfNodesPerCell=getNumberOfNodesPerCell();
  if(nbOfTuples%nbOfNodesPerCell!=0)
    {
      std::ostringstream oss; oss << "MEDCoupling1SGTUMesh:getNumberOfCells: : the nb of tuples in conn is " << nbOfTuples << " and number of nodes per cell is " << nbOfNodesPerCell << ". But " << nbOfTuples << "%" << nbOfNodesPerCell << " !=0 !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  return nbOfTuples/nbOfNodesPerCell;
}

int MEDCoupling1SGTUMesh::getNumberOfNodesPerCell() const throw(INTERP_KERNEL::Exception)
{
  checkNonDynamicGeoType();
  return (int)_cm->getNumberOfNodes();
}

int MEDCoupling1SGTUMesh::getNodalConnectivityLength() const throw(INTERP_KERNEL::Exception)
{
  const DataArrayInt *c1(_conn);
  if(!c1)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::getNodalConnectivityLength : no connectivity set !");
  if(c1->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::getNodalConnectivityLength : Nodal connectivity array set must have exactly one component !");
  if(!c1->isAllocated())
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::getNodalConnectivityLength : Nodal connectivity array must be allocated !");
  return c1->getNumberOfTuples();
}

DataArrayInt *MEDCoupling1SGTUMesh::computeNbOfNodesPerCell() const throw(INTERP_KERNEL::Exception)
{
  checkNonDynamicGeoType();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(getNumberOfCells(),1);
  ret->fillWithValue((int)_cm->getNumberOfNodes());
  return ret.retn();
}

DataArrayInt *MEDCoupling1SGTUMesh::computeNbOfFacesPerCell() const throw(INTERP_KERNEL::Exception)
{
  checkNonDynamicGeoType();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(getNumberOfCells(),1);
  ret->fillWithValue((int)_cm->getNumberOfSons());
  return ret.retn();
}

void MEDCoupling1SGTUMesh::getNodeIdsOfCell(int cellId, std::vector<int>& conn) const
{
  int sz=getNumberOfNodesPerCell();
  conn.resize(sz);
  if(cellId>=0 && cellId<getNumberOfCells())
    std::copy(_conn->begin()+cellId*sz,_conn->begin()+(cellId+1)*sz,conn.begin());
  else
    {
      std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::getNodeIdsOfCell : request for cellId #" << cellId << " must be in [0," << getNumberOfCells() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void MEDCoupling1SGTUMesh::checkNonDynamicGeoType() const throw(INTERP_KERNEL::Exception)
{
  if(_cm->isDynamic())
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::checkNonDynamicGeoType : internal error ! the internal geo type is dynamic ! should be static !");
}

std::string MEDCoupling1SGTUMesh::simpleRepr() const
{
  static const char msg0[]="No coordinates specified !";
  std::ostringstream ret;
  ret << "Single static geometic type (" << _cm->getRepr() << ") unstructured mesh with name : \"" << getName() << "\"\n";
  ret << "Description of mesh : \"" << getDescription() << "\"\n";
  int tmpp1,tmpp2;
  double tt=getTime(tmpp1,tmpp2);
  ret << "Time attached to the mesh [unit] : " << tt << " [" << getTimeUnit() << "]\n";
  ret << "Iteration : " << tmpp1  << " Order : " << tmpp2 << "\n";
  ret << "Mesh dimension : " << getMeshDimension() << "\nSpace dimension : ";
  if(_coords!=0)
    {
      const int spaceDim=getSpaceDimension();
      ret << spaceDim << "\nInfo attached on space dimension : ";
      for(int i=0;i<spaceDim;i++)
        ret << "\"" << _coords->getInfoOnComponent(i) << "\" ";
      ret << "\n";
    }
  else
    ret << msg0 << "\n";
  ret << "Number of nodes : ";
  if(_coords!=0)
    ret << getNumberOfNodes() << "\n";
  else
    ret << msg0 << "\n";
  ret << "Number of cells : ";
  if((const DataArrayInt *)_conn)
    {
      if(_conn->isAllocated())
        {
          if(_conn->getNumberOfComponents()==1)
            ret << getNumberOfCells() << "\n";
          else
            ret << "Nodal connectivity array specified and allocated but with not exactly one component !" << "\n";
        }
      else
        ret << "Nodal connectivity array specified but not allocated !" << "\n";
    }
  else
    ret << "No connectivity specified !" << "\n";
  ret << "Cell type : " << _cm->getRepr() << "\n";
  return ret.str();
}

std::string MEDCoupling1SGTUMesh::advancedRepr() const
{
  std::ostringstream ret;
  ret << simpleRepr();
  ret << "\nCoordinates array : \n___________________\n\n";
  if(_coords)
    _coords->reprWithoutNameStream(ret);
  else
    ret << "No array set !\n";
  ret << "\n\nConnectivity array : \n____________________\n\n";
  //
  if((const DataArrayInt *)_conn)
    {
      if(_conn->isAllocated())
        {
          if(_conn->getNumberOfComponents()==1)
            {
             int nbOfCells=getNumberOfCells();
             int sz=getNumberOfNodesPerCell();
             const int *connPtr=_conn->begin();
             for(int i=0;i<nbOfCells;i++,connPtr+=sz)
               {
                 ret << "Cell #" << i << " : ";
                 std::copy(connPtr,connPtr+sz,std::ostream_iterator<int>(ret," "));
                 ret << "\n";
               }
            }
          else
            ret << "Nodal connectivity array specified and allocated but with not exactly one component !" << "\n";
        }
      else
        ret << "Nodal connectivity array specified but not allocated !" << "\n";
    }
  else
    ret << "No connectivity specified !" << "\n";
  return ret.str();
}

DataArrayDouble *MEDCoupling1SGTUMesh::computeIsoBarycenterOfNodesPerCell() const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=DataArrayDouble::New();
  int spaceDim=getSpaceDimension();
  int nbOfCells=getNumberOfCells();//checkCoherency()
  int nbOfNodes=getNumberOfNodes();
  ret->alloc(nbOfCells,spaceDim);
  double *ptToFill=ret->getPointer();
  const double *coor=_coords->begin();
  const int *nodal=_conn->begin();
  int sz=getNumberOfNodesPerCell();
  double coeff=1./(double)sz;
  for(int i=0;i<nbOfCells;i++,ptToFill+=spaceDim)
    {
      std::fill(ptToFill,ptToFill+spaceDim,0.);
      for(int j=0;j<sz;j++,nodal++)
        if(*nodal>=0 && *nodal<nbOfNodes)
          std::transform(coor+spaceDim*nodal[0],coor+spaceDim*(nodal[0]+1),ptToFill,ptToFill,std::plus<double>());
        else
          {
            std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::computeIsoBarycenterOfNodesPerCell : on cell #" << i << " presence of nodeId #" << *nodal << " should be in [0," <<   nbOfNodes << ") !";
            throw INTERP_KERNEL::Exception(oss.str().c_str());
          }
      std::transform(ptToFill,ptToFill+spaceDim,ptToFill,std::bind2nd(std::multiplies<double>(),coeff));
    }
  return ret.retn();
}

void MEDCoupling1SGTUMesh::renumberCells(const int *old2NewBg, bool check) throw(INTERP_KERNEL::Exception)
{
  int nbCells=getNumberOfCells();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> o2n=DataArrayInt::New();
  o2n->useArray(old2NewBg,false,C_DEALLOC,nbCells,1);
  if(check)
    o2n=o2n->checkAndPreparePermutation();
  //
  const int *conn=_conn->begin();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> n2o=o2n->invertArrayO2N2N2O(nbCells);
  const int *n2oPtr=n2o->begin();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> newConn=DataArrayInt::New();
  newConn->alloc(_conn->getNumberOfTuples(),1);
  newConn->copyStringInfoFrom(*_conn);
  int sz=getNumberOfNodesPerCell();
  //
  int *newC=newConn->getPointer();
  for(int i=0;i<nbCells;i++,newC+=sz)
    {
      int pos=n2oPtr[i];
      std::copy(conn+pos*sz,conn+(pos+1)*sz,newC);
    }
  _conn=newConn;
}

/*!
 * Keeps from \a this only cells which constituing point id are in the ids specified by [\a begin,\a end).
 * The resulting cell ids are stored at the end of the 'cellIdsKept' parameter.
 * Parameter \a fullyIn specifies if a cell that has part of its nodes in ids array is kept or not.
 * If \a fullyIn is true only cells whose ids are \b fully contained in [\a begin,\a end) tab will be kept.
 *
 * \param [in] begin input start of array of node ids.
 * \param [in] end input end of array of node ids.
 * \param [in] fullyIn input that specifies if all node ids must be in [\a begin,\a end) array to consider cell to be in.
 * \param [in,out] cellIdsKeptArr array where all candidate cell ids are put at the end.
 */
void MEDCoupling1SGTUMesh::fillCellIdsToKeepFromNodeIds(const int *begin, const int *end, bool fullyIn, DataArrayInt *&cellIdsKeptArr) const
{
  int nbOfCells=getNumberOfCells();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cellIdsKept=DataArrayInt::New(); cellIdsKept->alloc(0,1);
  int tmp=-1;
  int sz=_conn->getMaxValue(tmp); sz=std::max(sz,0)+1;
  std::vector<bool> fastFinder(sz,false);
  for(const int *work=begin;work!=end;work++)
    if(*work>=0 && *work<sz)
      fastFinder[*work]=true;
  const int *conn=_conn->begin();
  int nbNodesPerCell=getNumberOfNodesPerCell();
  for(int i=0;i<nbOfCells;i++,conn+=nbNodesPerCell)
    {
      int ref=0,nbOfHit=0;
      for(int j=0;j<nbNodesPerCell;j++)
        if(conn[j]>=0)
          {
            ref++;
            if(fastFinder[conn[j]])
              nbOfHit++;
          }
      if((ref==nbOfHit && fullyIn) || (nbOfHit!=0 && !fullyIn))
        cellIdsKept->pushBackSilent(i);
    }
  cellIdsKeptArr=cellIdsKept.retn();
}

MEDCouplingMesh *MEDCoupling1SGTUMesh::mergeMyselfWith(const MEDCouplingMesh *other) const
{
  if(other->getType()!=SINGLE_STATIC_GEO_TYPE_UNSTRUCTURED)
    throw INTERP_KERNEL::Exception("Merge of umesh only available with umesh single static geo type each other !");
  const MEDCoupling1SGTUMesh *otherC=static_cast<const MEDCoupling1SGTUMesh *>(other);
  return Merge1SGTUMeshes(this,otherC);
}

MEDCouplingUMesh *MEDCoupling1SGTUMesh::buildUnstructured() const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> ret=MEDCouplingUMesh::New(getName(),getMeshDimension());
  ret->setCoords(getCoords());
  const int *nodalConn=_conn->begin();
  int nbCells=getNumberOfCells();
  int nbNodesPerCell=getNumberOfNodesPerCell();
  int geoType=(int)getCellModelEnum();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> c=DataArrayInt::New(); c->alloc(nbCells*(nbNodesPerCell+1),1);
  int *cPtr=c->getPointer();
  for(int i=0;i<nbCells;i++,nodalConn+=nbNodesPerCell)
    {
      *cPtr++=geoType;
      cPtr=std::copy(nodalConn,nodalConn+nbNodesPerCell,cPtr);
    }
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cI=DataArrayInt::Range(0,(nbCells+1)*(nbNodesPerCell+1),nbNodesPerCell+1);
  ret->setConnectivity(c,cI,true);
  return ret.retn();
}

DataArrayInt *MEDCoupling1SGTUMesh::simplexize(int policy) throw(INTERP_KERNEL::Exception)
{
  switch(policy)
    {
    case 0:
      return simplexizePol0();
    case 1:
      return simplexizePol1();
    case (int) INTERP_KERNEL::PLANAR_FACE_5:
      return simplexizePlanarFace5();
    case (int) INTERP_KERNEL::PLANAR_FACE_6:
      return simplexizePlanarFace6();
    default:
      throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::simplexize : unrecognized policy ! Must be :\n  - 0 or 1 (only available for meshdim=2) \n  - PLANAR_FACE_5, PLANAR_FACE_6  (only for meshdim=3)");
    }
}

/// @cond INTERNAL

struct MEDCouplingAccVisit
{
  MEDCouplingAccVisit():_new_nb_of_nodes(0) { }
  int operator()(int val) { if(val!=-1) return _new_nb_of_nodes++; else return -1; }
  int _new_nb_of_nodes;
};

/// @endcond

/*!
 * Finds nodes not used in any cell and returns an array giving a new id to every node
 * by excluding the unused nodes, for which the array holds -1. The result array is
 * a mapping in "Old to New" mode. 
 *  \param [out] nbrOfNodesInUse - number of node ids present in the nodal connectivity.
 *  \return DataArrayInt * - a new instance of DataArrayInt. Its length is \a
 *          this->getNumberOfNodes(). It holds for each node of \a this mesh either -1
 *          if the node is unused or a new id else. The caller is to delete this
 *          array using decrRef() as it is no more needed.  
 *  \throw If the coordinates array is not set.
 *  \throw If the nodal connectivity of cells is not defined.
 *  \throw If the nodal connectivity includes an invalid id.
 */
DataArrayInt *MEDCoupling1SGTUMesh::getNodeIdsInUse(int& nbrOfNodesInUse) const throw(INTERP_KERNEL::Exception)
{
  nbrOfNodesInUse=-1;
  int nbOfNodes=getNumberOfNodes();
  int nbOfCells=getNumberOfCells();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfNodes,1);
  int *traducer=ret->getPointer();
  std::fill(traducer,traducer+nbOfNodes,-1);
  const int *conn=_conn->begin();
  int nbNodesPerCell=getNumberOfNodesPerCell();
  for(int i=0;i<nbOfCells;i++)
    for(int j=0;j<nbNodesPerCell;j++,conn++)
      if(*conn>=0 && *conn<nbOfNodes)
        traducer[*conn]=1;
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::getNodeIdsInUse : In cell #" << i  << " presence of node id " <<  conn[j] << " not in [0," << nbOfNodes << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
  nbrOfNodesInUse=(int)std::count(traducer,traducer+nbOfNodes,1);
  std::transform(traducer,traducer+nbOfNodes,traducer,MEDCouplingAccVisit());
  return ret.retn();
}

/*!
 * Changes ids of nodes within the nodal connectivity arrays according to a permutation
 * array in "Old to New" mode. The node coordinates array is \b not changed by this method.
 * This method is a generalization of shiftNodeNumbersInConn().
 *  \warning This method performs no check of validity of new ids. **Use it with care !**
 *  \param [in] newNodeNumbersO2N - a permutation array, of length \a
 *         this->getNumberOfNodes(), in "Old to New" mode. 
 *         See \ref MEDCouplingArrayRenumbering for more info on renumbering modes.
 *  \throw If the nodal connectivity of cells is not defined.
 */
void MEDCoupling1SGTUMesh::renumberNodesInConn(const int *newNodeNumbersO2N)
{
  getNumberOfCells();//only to check that all is well defined.
  _conn->transformWithIndArr(newNodeNumbersO2N,newNodeNumbersO2N+getNumberOfNodes());
  updateTime();
}

MEDCoupling1SGTUMesh *MEDCoupling1SGTUMesh::Merge1SGTUMeshes(const MEDCoupling1SGTUMesh *mesh1, const MEDCoupling1SGTUMesh *mesh2) throw(INTERP_KERNEL::Exception)
{
  std::vector<const MEDCoupling1SGTUMesh *> tmp(2);
  tmp[0]=const_cast<MEDCoupling1SGTUMesh *>(mesh1); tmp[1]=const_cast<MEDCoupling1SGTUMesh *>(mesh2);
  return Merge1SGTUMeshes(tmp);
}

MEDCoupling1SGTUMesh *MEDCoupling1SGTUMesh::Merge1SGTUMeshes(std::vector<const MEDCoupling1SGTUMesh *>& a) throw(INTERP_KERNEL::Exception)
{
  std::size_t sz=a.size();
  if(sz==0)
    return Merge1SGTUMeshesLL(a);
  for(std::size_t ii=0;ii<sz;ii++)
    if(!a[ii])
      {
        std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::Merge1SGTUMeshes : item #" << ii << " in input array of size "<< sz << " is empty !";
        throw INTERP_KERNEL::Exception(oss.str().c_str());
      }
  const INTERP_KERNEL::CellModel *cm=&(a[0]->getCellModel());
  for(std::size_t ii=0;ii<sz;ii++)
    if(&(a[ii]->getCellModel())!=cm)
      throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::Merge1SGTUMeshes : all items must have the same geo type !");
  std::vector< MEDCouplingAutoRefCountObjectPtr<MEDCoupling1SGTUMesh> > bb(sz);
  std::vector< const MEDCoupling1SGTUMesh * > aa(sz);
  int spaceDim=-3;
  for(std::size_t i=0;i<sz && spaceDim==-3;i++)
    {
      const MEDCoupling1SGTUMesh *cur=a[i];
      const DataArrayDouble *coo=cur->getCoords();
      if(coo)
        spaceDim=coo->getNumberOfComponents();
    }
  if(spaceDim==-3)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::Merge1SGTUMeshes : no spaceDim specified ! unable to perform merge !");
  for(std::size_t i=0;i<sz;i++)
    {
      bb[i]=a[i]->buildSetInstanceFromThis(spaceDim);
      aa[i]=bb[i];
    }
  return Merge1SGTUMeshesLL(aa);
}

MEDCoupling1SGTUMesh *MEDCoupling1SGTUMesh::Merge1SGTUMeshesOnSameCoords(std::vector<const MEDCoupling1SGTUMesh *>& a) throw(INTERP_KERNEL::Exception)
{
  if(a.empty())
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::Merge1SGTUMeshesOnSameCoords : input array must be NON EMPTY !");
  std::vector<const MEDCoupling1SGTUMesh *>::const_iterator it=a.begin();
  if(!(*it))
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::Merge1SGTUMeshesOnSameCoords : presence of null instance !");
  std::vector<const DataArrayInt *> ncs(a.size());
  int nbOfCells=(*it)->getNumberOfCells();
  const DataArrayDouble *coords=(*it)->getCoords();
  const INTERP_KERNEL::CellModel *cm=&((*it)->getCellModel());
  int nbNodesPerCell=(*it)->getNumberOfNodesPerCell();
  ncs[0]=(*it)->getNodalConnectivity();
  it++;
  for(int i=1;it!=a.end();i++,it++)
    {
      if(cm!=&((*it)->getCellModel()))
        throw INTERP_KERNEL::Exception("Geometric types mismatches, Merge1SGTUMeshes impossible !");
      (*it)->getNumberOfCells();//to check that all is OK
      ncs[i]=(*it)->getNodalConnectivity();
      if(coords!=(*it)->getCoords())
        throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::Merge1SGTUMeshesOnSameCoords : not lying on same coords !");
    }
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1SGTUMesh> ret(new MEDCoupling1SGTUMesh("merge",*cm));
  ret->setCoords(coords);
  ret->_conn=DataArrayInt::Aggregate(ncs);
  return ret.retn();
}

MEDCoupling1SGTUMesh *MEDCoupling1SGTUMesh::Merge1SGTUMeshesLL(std::vector<const MEDCoupling1SGTUMesh *>& a) throw(INTERP_KERNEL::Exception)
{
  if(a.empty())
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::Merge1SGTUMeshes : input array must be NON EMPTY !");
  std::vector<const MEDCoupling1SGTUMesh *>::const_iterator it=a.begin();
  int nbOfCells=(*it)->getNumberOfCells();
  const INTERP_KERNEL::CellModel *cm=&((*it)->getCellModel());
  int nbNodesPerCell=(*it)->getNumberOfNodesPerCell();
  it++;
  for(;it!=a.end();it++)
    {
      if(cm!=&((*it)->getCellModel()))
        throw INTERP_KERNEL::Exception("Geometric types mismatches, Merge1SGTUMeshes impossible !");
      nbOfCells+=(*it)->getNumberOfCells();
    }
  std::vector<const MEDCouplingPointSet *> aps(a.size());
  std::copy(a.begin(),a.end(),aps.begin());
  MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> pts=MergeNodesArray(aps);
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1SGTUMesh> ret(new MEDCoupling1SGTUMesh("merge",*cm));
  ret->setCoords(pts);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> c=DataArrayInt::New();
  c->alloc(nbOfCells*nbNodesPerCell,1);
  int *cPtr=c->getPointer();
  int offset=0;
  for(it=a.begin();it!=a.end();it++)
    {
      int curConnLgth=(*it)->getNodalConnectivityLength();
      const int *curC=(*it)->_conn->begin();
      cPtr=std::transform(curC,curC+curConnLgth,cPtr,std::bind2nd(std::plus<int>(),offset));
      offset+=(*it)->getNumberOfNodes();
    }
  //
  ret->setNodalConnectivity(c);
  return ret.retn();
}

MEDCouplingPointSet *MEDCoupling1SGTUMesh::buildPartOfMySelfKeepCoords(const int *begin, const int *end) const
{
  int ncell=getNumberOfCells();
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1SGTUMesh> ret(new MEDCoupling1SGTUMesh(getName(),*_cm));
  ret->setCoords(_coords);
  std::size_t nbOfElemsRet=std::distance(begin,end);
  const int *inConn=_conn->getConstPointer();
  int sz=getNumberOfNodesPerCell();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> connRet=DataArrayInt::New(); connRet->alloc((int)nbOfElemsRet*sz,1);
  int *connPtr=connRet->getPointer();
  for(const int *work=begin;work!=end;work++,connPtr+=sz)
    {
      if(*work>=0 && *work<ncell)
        std::copy(inConn+(work[0])*sz,inConn+(work[0]+1)*sz,connPtr);
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::buildPartOfMySelfKeepCoords : On pos #" << std::distance(begin,work) << " input cell id =" << *work << " should be in [0," << ncell << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  ret->_conn=connRet;
  ret->copyTinyInfoFrom(this);
  return ret.retn();
}

MEDCouplingPointSet *MEDCoupling1SGTUMesh::buildPartOfMySelfKeepCoords2(int start, int end, int step) const
{
  int ncell=getNumberOfCells();
  int nbOfElemsRet=DataArray::GetNumberOfItemGivenBESRelative(start,end,step,"MEDCoupling1SGTUMesh::buildPartOfMySelfKeepCoords2 : ");
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1SGTUMesh> ret(new MEDCoupling1SGTUMesh(getName(),*_cm));
  ret->setCoords(_coords);
  const int *inConn=_conn->getConstPointer();
  int sz=getNumberOfNodesPerCell();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> connRet=DataArrayInt::New(); connRet->alloc((int)nbOfElemsRet*sz,1);
  int *connPtr=connRet->getPointer();
  int curId=start;
  for(int i=0;i<nbOfElemsRet;i++,connPtr+=sz,curId+=step)
    {
      if(curId>=0 && curId<ncell)
        std::copy(inConn+curId*sz,inConn+(curId+1)*sz,connPtr);
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::buildPartOfMySelfKeepCoords2 : On pos #" << i << " input cell id =" << curId  << " should be in [0," << ncell << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  ret->_conn=connRet;
  ret->copyTinyInfoFrom(this);
  return ret.retn();
}

MEDCoupling1SGTUMesh *MEDCoupling1SGTUMesh::buildSetInstanceFromThis(int spaceDim) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1SGTUMesh> ret(new MEDCoupling1SGTUMesh(getName(),*_cm));
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> tmp1;
  const DataArrayInt *nodalConn(_conn);
  if(!nodalConn)
    {
      tmp1=DataArrayInt::New(); tmp1->alloc(0,1);
    }
  else
    tmp1=_conn;
  ret->_conn=tmp1;
  if(!_coords)
    {
      MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> coords=DataArrayDouble::New(); coords->alloc(0,spaceDim);
      ret->setCoords(coords);
    }
  else
    ret->setCoords(_coords);
  return ret.retn();
}

DataArrayInt *MEDCoupling1SGTUMesh::simplexizePol0() throw(INTERP_KERNEL::Exception)
{
  int nbOfCells=getNumberOfCells();
  if(getCellModelEnum()!=INTERP_KERNEL::NORM_QUAD4)
    return DataArrayInt::Range(0,nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> newConn=DataArrayInt::New(); newConn->alloc(2*3*nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(2*nbOfCells,1);
  const int *c(_conn->begin());
  int *retPtr(ret->getPointer()),*newConnPtr(newConn->getPointer());
  for(int i=0;i<nbOfCells;i++,c+=4,newConnPtr+=6,retPtr+=2)
    {
      newConnPtr[0]=c[0]; newConnPtr[1]=c[1]; newConnPtr[2]=c[2];
      newConnPtr[3]=c[0]; newConnPtr[4]=c[2]; newConnPtr[5]=c[3];
      retPtr[0]=i; retPtr[1]=i;
    }
  _conn=newConn;
  _cm=&INTERP_KERNEL::CellModel::GetCellModel(INTERP_KERNEL::NORM_TRI3);
  updateTime();
  return ret.retn();
}

DataArrayInt *MEDCoupling1SGTUMesh::simplexizePol1() throw(INTERP_KERNEL::Exception)
{
  int nbOfCells=getNumberOfCells();
  if(getCellModelEnum()!=INTERP_KERNEL::NORM_QUAD4)
    return DataArrayInt::Range(0,nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> newConn=DataArrayInt::New(); newConn->alloc(2*3*nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(2*nbOfCells,1);
  const int *c(_conn->begin());
  int *retPtr(ret->getPointer()),*newConnPtr(newConn->getPointer());
  for(int i=0;i<nbOfCells;i++,c+=4,newConnPtr+=6,retPtr+=2)
    {
      newConnPtr[0]=c[0]; newConnPtr[1]=c[1]; newConnPtr[2]=c[3];
      newConnPtr[3]=c[1]; newConnPtr[4]=c[2]; newConnPtr[5]=c[3];
      retPtr[0]=i; retPtr[1]=i;
    }
  _conn=newConn;
  _cm=&INTERP_KERNEL::CellModel::GetCellModel(INTERP_KERNEL::NORM_TRI3);
  updateTime();
  return ret.retn();
}

DataArrayInt *MEDCoupling1SGTUMesh::simplexizePlanarFace5() throw(INTERP_KERNEL::Exception)
{
  int nbOfCells=getNumberOfCells();
  if(getCellModelEnum()!=INTERP_KERNEL::NORM_HEXA8)
    return DataArrayInt::Range(0,nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> newConn=DataArrayInt::New(); newConn->alloc(5*4*nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(5*nbOfCells,1);
  const int *c(_conn->begin());
  int *retPtr(ret->getPointer()),*newConnPtr(newConn->getPointer());
  for(int i=0;i<nbOfCells;i++,c+=8,newConnPtr+=20,retPtr+=5)
    {
      for(int j=0;j<20;j++)
        newConnPtr[j]=c[INTERP_KERNEL::SPLIT_NODES_5_WO[j]];
      retPtr[0]=i; retPtr[1]=i; retPtr[2]=i; retPtr[3]=i; retPtr[4]=i;
    }
  _conn=newConn;
  _cm=&INTERP_KERNEL::CellModel::GetCellModel(INTERP_KERNEL::NORM_TETRA4);
  updateTime();
  return ret.retn();
}

DataArrayInt *MEDCoupling1SGTUMesh::simplexizePlanarFace6() throw(INTERP_KERNEL::Exception)
{
  int nbOfCells=getNumberOfCells();
  if(getCellModelEnum()!=INTERP_KERNEL::NORM_HEXA8)
    return DataArrayInt::Range(0,nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> newConn=DataArrayInt::New(); newConn->alloc(6*4*nbOfCells,1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(6*nbOfCells,1);
  const int *c(_conn->begin());
  int *retPtr(ret->getPointer()),*newConnPtr(newConn->getPointer());
  for(int i=0;i<nbOfCells;i++,c+=8,newConnPtr+=24,retPtr+=6)
    {
      for(int j=0;j<24;j++)
        newConnPtr[j]=c[INTERP_KERNEL::SPLIT_NODES_6_WO[j]];
      retPtr[0]=i; retPtr[1]=i; retPtr[2]=i; retPtr[3]=i; retPtr[4]=i; retPtr[5]=i;
    }
  _conn=newConn;
  _cm=&INTERP_KERNEL::CellModel::GetCellModel(INTERP_KERNEL::NORM_TETRA4);
  updateTime();
  return ret.retn();
}

void MEDCoupling1SGTUMesh::reprQuickOverview(std::ostream& stream) const throw(INTERP_KERNEL::Exception)
{
  stream << "MEDCoupling1SGTUMesh C++ instance at " << this << ". Type=" << _cm->getRepr() << ". Name : \"" << getName() << "\".";
  stream << " Mesh dimension : " << getMeshDimension() << ".";
  if(!_coords)
    { stream << " No coordinates set !"; return ; }
  if(!_coords->isAllocated())
    { stream << " Coordinates set but not allocated !"; return ; }
  stream << " Space dimension : " << _coords->getNumberOfComponents() << "." << std::endl;
  stream << "Number of nodes : " << _coords->getNumberOfTuples() << ".";
  if(!(const DataArrayInt *)_conn)
    { stream << std::endl << "Nodal connectivity NOT set !"; return ; }
  if(_conn->isAllocated())
    {
      if(_conn->getNumberOfComponents()==1)
        stream << std::endl << "Number of cells : " << getNumberOfCells() << ".";
    }
}

void MEDCoupling1SGTUMesh::checkFullyDefined() const throw(INTERP_KERNEL::Exception)
{
  if(!((const DataArrayInt *)_conn) || !((const DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::checkFullyDefined : part of this is not fully defined.");
}

/*!
 * First step of unserialization process.
 */
bool MEDCoupling1SGTUMesh::isEmptyMesh(const std::vector<int>& tinyInfo) const
{
  throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::isEmptyMesh : not implemented yet !");
}

/*!
 * Checks if \a this and \a other meshes are geometrically equivalent with high
 * probability, else an exception is thrown. The meshes are considered equivalent if
 * (1) meshes contain the same number of nodes and the same number of elements of the
 * same types (2) three cells of the two meshes (first, last and middle) are based
 * on coincident nodes (with a specified precision).
 *  \param [in] other - the mesh to compare with.
 *  \param [in] prec - the precision used to compare nodes of the two meshes.
 *  \throw If the two meshes do not match.
 */
void MEDCoupling1SGTUMesh::checkFastEquivalWith(const MEDCouplingMesh *other, double prec) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingPointSet::checkFastEquivalWith(other,prec);
  const MEDCoupling1SGTUMesh *otherC=dynamic_cast<const MEDCoupling1SGTUMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::checkFastEquivalWith : Two meshes are not not unstructured with single static geometric type !");
  const DataArrayInt *c1(_conn),*c2(otherC->_conn);
  if(c1==c2)
    return;
  if(!c1 || !c2)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::checkFastEquivalWith : presence of nodal connectivity only in one of the 2 meshes !");
  if((c1->isAllocated() && !c2->isAllocated()) || (!c1->isAllocated() && c2->isAllocated()))
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::checkFastEquivalWith : in nodal connectivity, only one is allocated !");
  if(c1->getNumberOfComponents()!=1 || c1->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::checkFastEquivalWith : in nodal connectivity, must have 1 and only 1 component !");
  if(c1->getHashCode()!=c2->getHashCode())
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::checkFastEquivalWith : nodal connectivity differs");
}

MEDCouplingPointSet *MEDCoupling1SGTUMesh::mergeMyselfWithOnSameCoords(const MEDCouplingPointSet *other) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::mergeMyselfWithOnSameCoords : input other is null !");
  const MEDCoupling1SGTUMesh *otherC=dynamic_cast<const MEDCoupling1SGTUMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::mergeMyselfWithOnSameCoords : the input other mesh is not of type single statuc geo type unstructured !");
  std::vector<const MEDCoupling1SGTUMesh *> ms(2);
  ms[0]=this;
  ms[1]=otherC;
  return Merge1SGTUMeshesOnSameCoords(ms);
}

void MEDCoupling1SGTUMesh::getReverseNodalConnectivity(DataArrayInt *revNodal, DataArrayInt *revNodalIndx) const throw(INTERP_KERNEL::Exception)
{
  checkFullyDefined();
  int nbOfNodes=getNumberOfNodes();
  int *revNodalIndxPtr=(int *)malloc((nbOfNodes+1)*sizeof(int));
  revNodalIndx->useArray(revNodalIndxPtr,true,C_DEALLOC,nbOfNodes+1,1);
  std::fill(revNodalIndxPtr,revNodalIndxPtr+nbOfNodes+1,0);
  const int *conn=_conn->begin();
  int nbOfCells=getNumberOfCells();
  int nbOfEltsInRevNodal=0;
  int nbOfNodesPerCell=getNumberOfNodesPerCell();
  for(int eltId=0;eltId<nbOfCells;eltId++)
    {
      for(int j=0;j<nbOfNodesPerCell;j++,conn++)
        {
          if(conn[0]>=0 && conn[0]<nbOfNodes)
            {
              nbOfEltsInRevNodal++;
              revNodalIndxPtr[conn[0]+1]++;
            }
          else
            {
              std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::getReverseNodalConnectivity : At cell #" << eltId << " presence of nodeId #" << conn[0] << " should be in [0," << nbOfNodes << ") !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
    }
  std::transform(revNodalIndxPtr+1,revNodalIndxPtr+nbOfNodes+1,revNodalIndxPtr,revNodalIndxPtr+1,std::plus<int>());
  conn=_conn->begin();
  int *revNodalPtr=(int *)malloc((nbOfEltsInRevNodal)*sizeof(int));
  revNodal->useArray(revNodalPtr,true,C_DEALLOC,nbOfEltsInRevNodal,1);
  std::fill(revNodalPtr,revNodalPtr+nbOfEltsInRevNodal,-1);
  for(int eltId=0;eltId<nbOfCells;eltId++)
    {
      for(int j=0;j<nbOfNodesPerCell;j++,conn++)
        {
          *std::find_if(revNodalPtr+revNodalIndxPtr[*conn],revNodalPtr+revNodalIndxPtr[*conn+1],std::bind2nd(std::equal_to<int>(),-1))=eltId;
        }
    }
}

/*!
 * Use \a nodalConn array as nodal connectivity of \a this. The input \a nodalConn pointer can be null.
 */
void MEDCoupling1SGTUMesh::setNodalConnectivity(DataArrayInt *nodalConn) throw(INTERP_KERNEL::Exception)
{
  if(nodalConn)
    nodalConn->incrRef();
  _conn=nodalConn;
  declareAsNew();
}

/*!
 * \return DataArrayInt * - the internal reference to the nodal connectivity. The caller is not reponsible to deallocate it.
 */
DataArrayInt *MEDCoupling1SGTUMesh::getNodalConnectivity() const throw(INTERP_KERNEL::Exception)
{
  const DataArrayInt *ret(_conn);
  return const_cast<DataArrayInt *>(ret);
}

/*!
 * Allocates memory to store an estimation of the given number of cells. Closer is the estimation to the number of cells effectively inserted,
 * less will be the needs to realloc. If the number of cells to be inserted is not known simply put 0 to this parameter.
 * If a nodal connectivity previouly existed before the call of this method, it will be reset.
 *
 *  \param [in] nbOfCells - estimation of the number of cell \a this mesh will contain.
 */
void MEDCoupling1SGTUMesh::allocateCells(int nbOfCells) throw(INTERP_KERNEL::Exception)
{
  if(nbOfCells<0)
    throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::allocateCells : the input number of cells should be >= 0 !");
  _conn=DataArrayInt::New();
  _conn->reserve(getNumberOfNodesPerCell()*nbOfCells);
  declareAsNew();
}

/*!
 * Appends at the end of \a this a cell having nodal connectivity array defined in [ \a nodalConnOfCellBg, \a nodalConnOfCellEnd ).
 *
 * \param [in] nodalConnOfCellBg - the begin (included) of nodal connectivity of the cell to add.
 * \param [in] nodalConnOfCellEnd - the end (excluded) of nodal connectivity of the cell to add.
 * \throw If the length of the input nodal connectivity array of the cell to add is not equal to number of nodes per cell relative to the unique geometric type
 *        attached to \a this.
 * \thow If the nodal connectivity array in \a this is null (call MEDCoupling1SGTUMesh::allocateCells before).
 */
void MEDCoupling1SGTUMesh::insertNextCell(const int *nodalConnOfCellBg, const int *nodalConnOfCellEnd) throw(INTERP_KERNEL::Exception)
{
  int sz=(int)std::distance(nodalConnOfCellBg,nodalConnOfCellEnd);
  int ref=getNumberOfNodesPerCell();
  if(sz==ref)
    {
      DataArrayInt *c(_conn);
      if(c)
        c->pushBackValsSilent(nodalConnOfCellBg,nodalConnOfCellEnd);
      else
        throw INTERP_KERNEL::Exception("MEDCoupling1SGTUMesh::insertNextCell : nodal connectivity array is null ! Call MEDCoupling1SGTUMesh::allocateCells before !");
    }
  else
    {
      std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::insertNextCell : input nodal size (" << sz << ") does not match number of nodes per cell of this (";
      oss << ref << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

//== find static tony

MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::New(const char *name, INTERP_KERNEL::NormalizedCellType type) throw(INTERP_KERNEL::Exception)
{
  if(type==INTERP_KERNEL::NORM_ERROR)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::New : NORM_ERROR is not a valid type to be used as base geometric type for a mesh !");
  const INTERP_KERNEL::CellModel& cm=INTERP_KERNEL::CellModel::GetCellModel(type);
  if(!cm.isDynamic())
    {
      std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::New : the input geometric type " << cm.getRepr() << " is static ! Only dynamic types are allowed here !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  return new MEDCoupling1DGTUMesh(name,cm);
}

MEDCoupling1DGTUMesh::MEDCoupling1DGTUMesh(const char *name, const INTERP_KERNEL::CellModel& cm):MEDCoupling1GTUMesh(name,cm)
{
}

MEDCoupling1DGTUMesh::MEDCoupling1DGTUMesh(const MEDCoupling1DGTUMesh& other, bool recDeepCpy):MEDCoupling1GTUMesh(other,recDeepCpy),_conn(other._conn)
{
  if(recDeepCpy)
    {
      const DataArrayInt *c(other._conn);
      if(c)
        _conn=c->deepCpy();
      c=other._conn_indx;
      if(c)
        _conn_indx=c->deepCpy();
    }
}

MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::clone(bool recDeepCpy) const
{
  return new MEDCoupling1DGTUMesh(*this,recDeepCpy);
}

void MEDCoupling1DGTUMesh::updateTime() const
{
  MEDCoupling1GTUMesh::updateTime();
  const DataArrayInt *c(_conn);
  if(c)
    updateTimeWith(*c);
  c=_conn_indx;
  if(c)
    updateTimeWith(*c);
}

std::size_t MEDCoupling1DGTUMesh::getHeapMemorySize() const
{
  std::size_t ret=0;
  const DataArrayInt *c(_conn);
  if(c)
    ret+=c->getHeapMemorySize();
  c=_conn_indx;
  if(c)
    ret+=c->getHeapMemorySize();
  return MEDCoupling1GTUMesh::getHeapMemorySize()+ret;
}

MEDCouplingMesh *MEDCoupling1DGTUMesh::deepCpy() const
{
  return clone(true);
}

bool MEDCoupling1DGTUMesh::isEqualIfNotWhy(const MEDCouplingMesh *other, double prec, std::string& reason) const throw(INTERP_KERNEL::Exception)
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::isEqualIfNotWhy : input other pointer is null !");
  std::ostringstream oss; oss.precision(15);
  const MEDCoupling1DGTUMesh *otherC=dynamic_cast<const MEDCoupling1DGTUMesh *>(other);
  if(!otherC)
    {
      reason="mesh given in input is not castable in MEDCoupling1DGTUMesh !";
      return false;
    }
  if(!MEDCoupling1GTUMesh::isEqualIfNotWhy(other,prec,reason))
    return false;
  const DataArrayInt *c1(_conn),*c2(otherC->_conn);
  if(c1==c2)
    return true;
  if(!c1 || !c2)
    {
      reason="in connectivity of single dynamic geometric type exactly one among this and other is null !";
      return false;
    }
  if(!c1->isEqualIfNotWhy(*c2,reason))
    {
      reason.insert(0,"Nodal connectivity DataArrayInt differs : ");
      return false;
    }
  c1=_conn_indx; c2=otherC->_conn_indx;
  if(c1==c2)
    return true;
  if(!c1 || !c2)
    {
      reason="in connectivity index of single dynamic geometric type exactly one among this and other is null !";
      return false;
    }
  if(!c1->isEqualIfNotWhy(*c2,reason))
    {
      reason.insert(0,"Nodal connectivity index DataArrayInt differs : ");
      return false;
    }
  return true;
}

bool MEDCoupling1DGTUMesh::isEqualWithoutConsideringStr(const MEDCouplingMesh *other, double prec) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::isEqualWithoutConsideringStr : input other pointer is null !");
  const MEDCoupling1DGTUMesh *otherC=dynamic_cast<const MEDCoupling1DGTUMesh *>(other);
  if(!otherC)
    return false;
  if(!MEDCoupling1GTUMesh::isEqualWithoutConsideringStr(other,prec))
    return false;
  const DataArrayInt *c1(_conn),*c2(otherC->_conn);
  if(c1==c2)
    return true;
  if(!c1 || !c2)
    return false;
  if(!c1->isEqualWithoutConsideringStr(*c2))
    return false;
  return true;
  c1=_conn_indx; c2=otherC->_conn_indx;
  if(c1==c2)
    return true;
  if(!c1 || !c2)
    return false;
  if(!c1->isEqualWithoutConsideringStr(*c2))
    return false;
  return true;
}

/*!
 * Checks if \a this and \a other meshes are geometrically equivalent with high
 * probability, else an exception is thrown. The meshes are considered equivalent if
 * (1) meshes contain the same number of nodes and the same number of elements of the
 * same types (2) three cells of the two meshes (first, last and middle) are based
 * on coincident nodes (with a specified precision).
 *  \param [in] other - the mesh to compare with.
 *  \param [in] prec - the precision used to compare nodes of the two meshes.
 *  \throw If the two meshes do not match.
 */
void MEDCoupling1DGTUMesh::checkFastEquivalWith(const MEDCouplingMesh *other, double prec) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingPointSet::checkFastEquivalWith(other,prec);
  const MEDCoupling1DGTUMesh *otherC=dynamic_cast<const MEDCoupling1DGTUMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : Two meshes are not not unstructured with single static geometric type !");
  const DataArrayInt *c1(_conn),*c2(otherC->_conn);
  if(c1!=c2)
    {
      if(!c1 || !c2)
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : presence of nodal connectivity only in one of the 2 meshes !");
      if((c1->isAllocated() && !c2->isAllocated()) || (!c1->isAllocated() && c2->isAllocated()))
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : in nodal connectivity, only one is allocated !");
      if(c1->getNumberOfComponents()!=1 || c1->getNumberOfComponents()!=1)
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : in nodal connectivity, must have 1 and only 1 component !");
      if(c1->getHashCode()!=c2->getHashCode())
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : nodal connectivity differs");
    }
  c1=_conn_indx; c2=otherC->_conn_indx;
  if(c1!=c2)
    {
      if(!c1 || !c2)
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : presence of nodal connectivity index only in one of the 2 meshes !");
      if((c1->isAllocated() && !c2->isAllocated()) || (!c1->isAllocated() && c2->isAllocated()))
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : in nodal connectivity index, only one is allocated !");
      if(c1->getNumberOfComponents()!=1 || c1->getNumberOfComponents()!=1)
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : in nodal connectivity index, must have 1 and only 1 component !");
      if(c1->getHashCode()!=c2->getHashCode())
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFastEquivalWith : nodal connectivity index differs");
    }
}

/*!
 * If \a this pass this method, you are sure that connectivity arrays are not null, with exactly one component, no name, no component name, allocated.
 * In addition you are sure that the length of nodal connectivity index array is bigger than or equal to one.
 * In addition you are also sure that length of nodal connectivity is coherent with the content of the last value in the index array.
 */
void MEDCoupling1DGTUMesh::checkCoherency() const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingPointSet::checkCoherency();
  const DataArrayInt *c1(_conn);
  if(c1)
    {
      if(c1->getNumberOfComponents()!=1)
        throw INTERP_KERNEL::Exception("Nodal connectivity array is expected to be with number of components set to one !");
      if(c1->getInfoOnComponent(0)!="")
        throw INTERP_KERNEL::Exception("Nodal connectivity array is expected to have no info on its single component !");
      c1->checkAllocated();
    }
  else
    throw INTERP_KERNEL::Exception("Nodal connectivity array not defined !");
  //
  int sz2=_conn->getNumberOfTuples();
  c1=_conn_indx;
  if(c1)
    {
      if(c1->getNumberOfComponents()!=1)
        throw INTERP_KERNEL::Exception("Nodal connectivity index array is expected to be with number of components set to one !");
      c1->checkAllocated();
      if(c1->getNumberOfTuples()<1)
        throw INTERP_KERNEL::Exception("Nodal connectivity index array is expected to have a a size of 1 at least !");
      if(c1->getInfoOnComponent(0)!="")
        throw INTERP_KERNEL::Exception("Nodal connectivity index array is expected to have no info on its single component !");
      int f=c1->front(),ll=c1->back();
      if(f<0 || f>=sz2)
        {
          std::ostringstream oss; oss << "Nodal connectivity index array first value (" << f << ") is expected to be exactly in [0," << sz2 << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
      if(ll<0 || ll>sz2)
        {
          std::ostringstream oss; oss << "Nodal connectivity index array last value (" << ll << ") is expected to be exactly in [0," << sz2 << "] !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
      if(f>ll)
        {
          std::ostringstream oss; oss << "Nodal connectivity index array looks very bad (not increasing monotonic) because front (" << f << ") is greater that back (" << ll << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  else
    throw INTERP_KERNEL::Exception("Nodal connectivity index array not defined !");
  int szOfC1Exp=_conn_indx->back();
  if(sz2<szOfC1Exp)
    {
      std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::checkCoherency : The expected length of nodal connectivity array regarding index is " << szOfC1Exp << " but the actual size of it is " << c1->getNumberOfTuples() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void MEDCoupling1DGTUMesh::checkCoherency1(double eps) const throw(INTERP_KERNEL::Exception)
{
  checkCoherency();
  const DataArrayInt *c1(_conn),*c2(_conn_indx);
  if(!c2->isMonotonic(true))
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkCoherency1 : the nodal connectivity index is expected to be increasing monotinic !");
  //
  int nbOfTuples=c1->getNumberOfTuples();
  int nbOfNodes=getNumberOfNodes();
  const int *w(c1->begin());
  for(int i=0;i<nbOfTuples;i++,w++)
    {
      if(*w==-1) continue;
      if(*w<0 || *w>=nbOfNodes)
        {
          std::ostringstream oss; oss << "At pos #" << i << " of nodal connectivity array references to node id #" << *w << " must be in [0," << nbOfNodes << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
}

void MEDCoupling1DGTUMesh::checkCoherency2(double eps) const throw(INTERP_KERNEL::Exception)
{
  checkCoherency1(eps);
}

int MEDCoupling1DGTUMesh::getNumberOfCells() const
{
  checkCoherency();//do not remove
  return _conn_indx->getNumberOfTuples()-1;
}

/*!
 * This method returns a newly allocated array containing this->getNumberOfCells() tuples and 1 component.
 * For each cell in \b this the number of nodes constituting cell is computed.
 * For each polyhedron cell, the sum of the number of nodes of each face constituting polyhedron cell is returned.
 * So for pohyhedrons some nodes can be counted several times in the returned result.
 * 
 * \return a newly allocated array
 */
DataArrayInt *MEDCoupling1DGTUMesh::computeNbOfNodesPerCell() const throw(INTERP_KERNEL::Exception)
{
  checkCoherency();
  _conn_indx->checkMonotonic(true);
  if(getCellModelEnum()!=INTERP_KERNEL::NORM_POLYHED)
    return _conn_indx->deltaShiftIndex();
  // for polyhedrons
  int nbOfCells=_conn_indx->getNumberOfTuples()-1;
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfCells,1);
  int *retPtr=ret->getPointer();
  const int *ci=_conn_indx->begin(),*c=_conn->begin();
  for(int i=0;i<nbOfCells;i++,retPtr++,ci++)
    *retPtr=ci[1]-ci[0]-std::count(c+ci[0],c+ci[1],-1);
  return ret.retn();
}

/*!
 * This method returns a newly allocated array containing this->getNumberOfCells() tuples and 1 component.
 * For each cell in \b this the number of faces constituting (entity of dimension this->getMeshDimension()-1) cell is computed.
 * 
 * \return a newly allocated array
 */
DataArrayInt *MEDCoupling1DGTUMesh::computeNbOfFacesPerCell() const throw(INTERP_KERNEL::Exception)
{
  checkCoherency();
  _conn_indx->checkMonotonic(true);
  if(getCellModelEnum()!=INTERP_KERNEL::NORM_POLYHED && getCellModelEnum()!=INTERP_KERNEL::NORM_QPOLYG)
    return _conn_indx->deltaShiftIndex();
  if(getCellModelEnum()==INTERP_KERNEL::NORM_QPOLYG)
    {
      MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=_conn_indx->deltaShiftIndex();
      ret->applyDivideBy(2);
      return ret.retn();
    }
  // for polyhedrons
  int nbOfCells=_conn_indx->getNumberOfTuples()-1;
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfCells,1);
  int *retPtr=ret->getPointer();
  const int *ci=_conn_indx->begin(),*c=_conn->begin();
  for(int i=0;i<nbOfCells;i++,retPtr++,ci++)
    *retPtr=std::count(c+ci[0],c+ci[1],-1)+1;
  return ret.retn();
}

void MEDCoupling1DGTUMesh::getNodeIdsOfCell(int cellId, std::vector<int>& conn) const
{
  int nbOfCells=getNumberOfCells();//performs checks
  if(cellId>=0 && cellId<nbOfCells)
    {
      int strt=_conn_indx->getIJ(cellId,0),stp=_conn_indx->getIJ(cellId+1,0);
      int nbOfNodes=stp-strt;
      if(nbOfNodes<0)
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::getNodeIdsOfCell : the index array is invalid ! Should be increasing monotonic !");
      conn.resize(nbOfNodes);
      std::copy(_conn->begin()+strt,_conn->begin()+stp,conn.begin());
    }
  else
    {
      std::ostringstream oss; oss << "MEDCoupling1SGTUMesh::getNodeIdsOfCell : request for cellId #" << cellId << " must be in [0," << nbOfCells << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

std::string MEDCoupling1DGTUMesh::simpleRepr() const
{
  static const char msg0[]="No coordinates specified !";
  std::ostringstream ret;
  ret << "Single dynamic geometic type (" << _cm->getRepr() << ") unstructured mesh with name : \"" << getName() << "\"\n";
  ret << "Description of mesh : \"" << getDescription() << "\"\n";
  int tmpp1,tmpp2;
  double tt=getTime(tmpp1,tmpp2);
  ret << "Time attached to the mesh [unit] : " << tt << " [" << getTimeUnit() << "]\n";
  ret << "Iteration : " << tmpp1  << " Order : " << tmpp2 << "\n";
  ret << "Mesh dimension : " << getMeshDimension() << "\nSpace dimension : ";
  if(_coords!=0)
    {
      const int spaceDim=getSpaceDimension();
      ret << spaceDim << "\nInfo attached on space dimension : ";
      for(int i=0;i<spaceDim;i++)
        ret << "\"" << _coords->getInfoOnComponent(i) << "\" ";
      ret << "\n";
    }
  else
    ret << msg0 << "\n";
  ret << "Number of nodes : ";
  if(_coords!=0)
    ret << getNumberOfNodes() << "\n";
  else
    ret << msg0 << "\n";
  ret << "Number of cells : ";
  bool isOK=true;
  try { checkCoherency(); } catch(INTERP_KERNEL::Exception& e)
    {
      ret << "Nodal connectivity arrays are not set or badly set !\n";
      isOK=false;
    }
  if(isOK)
    ret << getNumberOfCells() << "\n";
  ret << "Cell type : " << _cm->getRepr() << "\n";
  return ret.str();
}

std::string MEDCoupling1DGTUMesh::advancedRepr() const
{
  std::ostringstream ret;
  ret << simpleRepr();
  ret << "\nCoordinates array : \n___________________\n\n";
  if(_coords)
    _coords->reprWithoutNameStream(ret);
  else
    ret << "No array set !\n";
  ret << "\n\nNodal Connectivity : \n____________________\n\n";
  //
  bool isOK=true;
  try { checkCoherency1(); } catch(INTERP_KERNEL::Exception& e)
    {
      ret << "Nodal connectivity arrays are not set or badly set !\n";
      isOK=false;
    }
  if(!isOK)
    return ret.str();
  int nbOfCells=getNumberOfCells();
  const int *ci=_conn_indx->begin(),*c=_conn->begin();
  for(int i=0;i<nbOfCells;i++,ci++)
    {
      ret << "Cell #" << i << " : ";
      std::copy(c+ci[0],c+ci[1],std::ostream_iterator<int>(ret," "));
      ret << "\n";
    }
  return ret.str();
}

DataArrayDouble *MEDCoupling1DGTUMesh::computeIsoBarycenterOfNodesPerCell() const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=DataArrayDouble::New();
  int spaceDim=getSpaceDimension();
  int nbOfCells=getNumberOfCells();//checkCoherency()
  int nbOfNodes=getNumberOfNodes();
  ret->alloc(nbOfCells,spaceDim);
  double *ptToFill=ret->getPointer();
  const double *coor=_coords->begin();
  const int *nodal=_conn->begin(),*nodali=_conn_indx->begin();
  nodal+=nodali[0];
  if(getCellModelEnum()!=INTERP_KERNEL::NORM_POLYHED)
    {
      for(int i=0;i<nbOfCells;i++,ptToFill+=spaceDim,nodali++)
        {
          std::fill(ptToFill,ptToFill+spaceDim,0.);
          if(nodali[0]>=nodali[1])// >= to avoid division by 0.
            {
              for(int j=nodali[0];j<nodali[1];j++,nodal++)
                {
                  if(*nodal>=0 && *nodal<nbOfNodes)
                    std::transform(coor+spaceDim*nodal[0],coor+spaceDim*(nodal[0]+1),ptToFill,ptToFill,std::plus<double>());
                  else
                    {
                      std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::computeIsoBarycenterOfNodesPerCell : on cell #" << i << " presence of nodeId #" << *nodal << " should be in [0," <<   nbOfNodes << ") !";
                      throw INTERP_KERNEL::Exception(oss.str().c_str());
                    }
                  std::transform(ptToFill,ptToFill+spaceDim,ptToFill,std::bind2nd(std::multiplies<double>(),1./(nodali[1]-nodali[0])));
                }
            }
          else
            {
              std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::computeIsoBarycenterOfNodesPerCell : at cell #" << i << " the nodal index array is invalid !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
    }
  else
    {
      for(int i=0;i<nbOfCells;i++,ptToFill+=spaceDim,nodali++)
        {
          std::fill(ptToFill,ptToFill+spaceDim,0.);
          if(nodali[0]>=nodali[1])// >= to avoid division by 0.
            {
              int nbOfNod=0;
              for(int j=nodali[0];j<nodali[1];j++,nodal++)
                {
                  if(*nodal==-1) continue;
                  if(*nodal>=0 && *nodal<nbOfNodes)
                    {
                      std::transform(coor+spaceDim*nodal[0],coor+spaceDim*(nodal[0]+1),ptToFill,ptToFill,std::plus<double>());
                      nbOfNod++;
                    }
                  else
                    {
                      std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::computeIsoBarycenterOfNodesPerCell (polyhedron) : on cell #" << i << " presence of nodeId #" << *nodal << " should be in [0," <<   nbOfNodes << ") !";
                      throw INTERP_KERNEL::Exception(oss.str().c_str());
                    }
                }
              if(nbOfNod!=0)
                std::transform(ptToFill,ptToFill+spaceDim,ptToFill,std::bind2nd(std::multiplies<double>(),1./nbOfNod));
              else
                {
                  std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::computeIsoBarycenterOfNodesPerCell (polyhedron) : no nodes in cell #" << i << " !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
            }
          else
            {
              std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::computeIsoBarycenterOfNodesPerCell (polyhedron)  : at cell #" << i << " the nodal index array is invalid !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
    }
  return ret.retn();
}

void MEDCoupling1DGTUMesh::renumberCells(const int *old2NewBg, bool check) throw(INTERP_KERNEL::Exception)
{
  int nbCells=getNumberOfCells();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> o2n=DataArrayInt::New();
  o2n->useArray(old2NewBg,false,C_DEALLOC,nbCells,1);
  if(check)
    o2n=o2n->checkAndPreparePermutation();
  //
  const int *conn=_conn->begin(),*conni=_conn_indx->begin();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> n2o=o2n->invertArrayO2N2N2O(nbCells);
  const int *n2oPtr=n2o->begin();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> newConn=DataArrayInt::New();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> newConnI=DataArrayInt::New();
  newConn->alloc(_conn->getNumberOfTuples(),1); newConnI->alloc(nbCells,1);
  newConn->copyStringInfoFrom(*_conn); newConnI->copyStringInfoFrom(*_conn_indx);
  //
  int *newC=newConn->getPointer(),*newCI=newConnI->getPointer();
  for(int i=0;i<nbCells;i++)
    {
      int sz=conni[i+1]-conni[i];
      if(sz>=0)
        newCI[n2oPtr[i]]=sz;
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::renumberCells : the index nodal array is invalid for cell #" << i << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  newConnI->computeOffsets2(); newCI=newConnI->getPointer();
  //
  for(int i=0;i<nbCells;i++,conni++)
    {
      int sz=conni[1]-conni[0];
      int newp=n2oPtr[i];
      std::copy(conn+conni[0],conn+conni[1],newC+newCI[newp]);
    }
  _conn=newConn;
}

MEDCouplingMesh *MEDCoupling1DGTUMesh::mergeMyselfWith(const MEDCouplingMesh *other) const
{
  if(other->getType()!=SINGLE_DYNAMIC_GEO_TYPE_UNSTRUCTURED)
    throw INTERP_KERNEL::Exception("Merge of umesh only available with umesh single dynamic geo type each other !");
  const MEDCoupling1DGTUMesh *otherC=static_cast<const MEDCoupling1DGTUMesh *>(other);
  return Merge1DGTUMeshes(this,otherC);
}

MEDCouplingUMesh *MEDCoupling1DGTUMesh::buildUnstructured() const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> ret=MEDCouplingUMesh::New(getName(),getMeshDimension());
  ret->setCoords(getCoords());
  const int *nodalConn=_conn->begin(),*nodalConnI=_conn_indx->begin();
  int nbCells=getNumberOfCells();//checkCoherency
  int geoType=(int)getCellModelEnum();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> c=DataArrayInt::New(); c->alloc(nbCells+_conn->getNumberOfTuples(),1);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cI=DataArrayInt::New(); cI->alloc(nbCells+1);
  int *cPtr=c->getPointer(),*ciPtr=cI->getPointer();
  ciPtr[0]=0;
  for(int i=0;i<nbCells;i++,ciPtr++)
    {
      int sz=nodalConnI[i+1]-nodalConnI[i];
      if(sz>=0)
        {
          *cPtr++=geoType;
          cPtr=std::copy(nodalConn+nodalConnI[i],nodalConn+nodalConnI[i+1],cPtr);
          ciPtr[1]=ciPtr[0]+sz+1;
        }
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::buildUnstructured : Invalid for nodal index for cell #" << i << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  ret->setConnectivity(c,cI,true);
  return ret.retn();
}

/*!
 * Do nothing for the moment, because there is no policy that allows to split polygons, polyhedrons ... into simplexes
 */
DataArrayInt *MEDCoupling1DGTUMesh::simplexize(int policy) throw(INTERP_KERNEL::Exception)
{
  int nbOfCells=getNumberOfCells();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfCells,1);
  ret->iota(0);
  return ret.retn();
}

void MEDCoupling1DGTUMesh::reprQuickOverview(std::ostream& stream) const throw(INTERP_KERNEL::Exception)
{
  stream << "MEDCoupling1DGTUMesh C++ instance at " << this << ". Type=" << _cm->getRepr() << ". Name : \"" << getName() << "\".";
  stream << " Mesh dimension : " << getMeshDimension() << ".";
  if(!_coords)
    { stream << " No coordinates set !"; return ; }
  if(!_coords->isAllocated())
    { stream << " Coordinates set but not allocated !"; return ; }
  stream << " Space dimension : " << _coords->getNumberOfComponents() << "." << std::endl;
  stream << "Number of nodes : " << _coords->getNumberOfTuples() << ".";
  bool isOK=true;
  try { checkCoherency(); } catch(INTERP_KERNEL::Exception& e)
    {
      stream << std::endl << "Nodal connectivity NOT set properly !\n";
      isOK=false;
    }
  if(isOK)
    stream << std::endl << "Number of cells : " << getNumberOfCells() << ".";
}

void MEDCoupling1DGTUMesh::shallowCopyConnectivityFrom(const MEDCouplingPointSet *other) throw(INTERP_KERNEL::Exception)
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::shallowCopyConnectivityFrom : input pointer is null !");
  const MEDCoupling1DGTUMesh *otherC=dynamic_cast<const MEDCoupling1DGTUMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::shallowCopyConnectivityFrom : input pointer is not an MEDCoupling1DGTUMesh instance !");
  setNodalConnectivity(otherC->getNodalConnectivity(),otherC->getNodalConnectivityIndex());
}

MEDCouplingPointSet *MEDCoupling1DGTUMesh::mergeMyselfWithOnSameCoords(const MEDCouplingPointSet *other) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::mergeMyselfWithOnSameCoords : input other is null !");
  const MEDCoupling1DGTUMesh *otherC=dynamic_cast<const MEDCoupling1DGTUMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::mergeMyselfWithOnSameCoords : the input other mesh is not of type single statuc geo type unstructured !");
  std::vector<const MEDCoupling1DGTUMesh *> ms(2);
  ms[0]=this;
  ms[1]=otherC;
  return Merge1DGTUMeshesOnSameCoords(ms);
}

MEDCouplingPointSet *MEDCoupling1DGTUMesh::buildPartOfMySelfKeepCoords(const int *begin, const int *end) const
{
  checkCoherency();
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1DGTUMesh> ret(new MEDCoupling1DGTUMesh(getName(),*_cm));
  ret->setCoords(_coords);
  DataArrayInt *c=0,*ci=0;
  MEDCouplingUMesh::ExtractFromIndexedArrays(begin,end,_conn,_conn_indx,c,ci);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cSafe(c),ciSafe(ci);
  ret->setNodalConnectivity(c,ci);
  return ret.retn();
}

MEDCouplingPointSet *MEDCoupling1DGTUMesh::buildPartOfMySelfKeepCoords2(int start, int end, int step) const
{
  checkCoherency();
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1DGTUMesh> ret(new MEDCoupling1DGTUMesh(getName(),*_cm));
  ret->setCoords(_coords);
  DataArrayInt *c=0,*ci=0;
  MEDCouplingUMesh::ExtractFromIndexedArrays2(start,end,step,_conn,_conn_indx,c,ci);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cSafe(c),ciSafe(ci);
  ret->setNodalConnectivity(c,ci);
  return ret.retn();
}

void MEDCoupling1DGTUMesh::getReverseNodalConnectivity(DataArrayInt *revNodal, DataArrayInt *revNodalIndx) const throw(INTERP_KERNEL::Exception)
{
  checkFullyDefined();
  int nbOfNodes=getNumberOfNodes();
  int *revNodalIndxPtr=(int *)malloc((nbOfNodes+1)*sizeof(int));
  revNodalIndx->useArray(revNodalIndxPtr,true,C_DEALLOC,nbOfNodes+1,1);
  std::fill(revNodalIndxPtr,revNodalIndxPtr+nbOfNodes+1,0);
  const int *conn=_conn->begin(),*conni=_conn_indx->begin();
  int nbOfCells=getNumberOfCells();
  int nbOfEltsInRevNodal=0;
  for(int eltId=0;eltId<nbOfCells;eltId++)
    {
      int nbOfNodesPerCell=conni[eltId+1]-conni[eltId];
      if(nbOfNodesPerCell>=0)
        {
          for(int j=0;j<nbOfNodesPerCell;j++)
            {
              int nodeId=conn[conni[eltId]+j];
              if(nodeId==-1) continue;            
              if(nodeId>=0 && nodeId<nbOfNodes)
                {
                  nbOfEltsInRevNodal++;
                  revNodalIndxPtr[nodeId+1]++;
                }
              else
                {
                  std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::getReverseNodalConnectivity : At cell #" << eltId << " presence of nodeId #" << conn[0] << " should be in [0," << nbOfNodes << ") !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
            }
        }
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::getReverseNodalConnectivity : At cell #" << eltId << "nodal connectivity is invalid !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  std::transform(revNodalIndxPtr+1,revNodalIndxPtr+nbOfNodes+1,revNodalIndxPtr,revNodalIndxPtr+1,std::plus<int>());
  conn=_conn->begin();
  int *revNodalPtr=(int *)malloc((nbOfEltsInRevNodal)*sizeof(int));
  revNodal->useArray(revNodalPtr,true,C_DEALLOC,nbOfEltsInRevNodal,1);
  std::fill(revNodalPtr,revNodalPtr+nbOfEltsInRevNodal,-1);
  for(int eltId=0;eltId<nbOfCells;eltId++)
    {
      int nbOfNodesPerCell=conni[eltId+1]-conni[eltId];
      for(int j=0;j<nbOfNodesPerCell;j++)
        {
          int nodeId=conn[conni[eltId]+j];
          if(nodeId!=-1)
            *std::find_if(revNodalPtr+revNodalIndxPtr[nodeId],revNodalPtr+revNodalIndxPtr[nodeId+1],std::bind2nd(std::equal_to<int>(),-1))=eltId;
        }
    }
}

void MEDCoupling1DGTUMesh::checkFullyDefined() const throw(INTERP_KERNEL::Exception)
{
  if(!((const DataArrayInt *)_conn) || !((const DataArrayInt *)_conn_indx) || !((const DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::checkFullyDefined : part of this is not fully defined.");
}

bool MEDCoupling1DGTUMesh::isEmptyMesh(const std::vector<int>& tinyInfo) const
{
  throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::isEmptyMesh : not implemented yet !");
}

/*!
 * Finds nodes not used in any cell and returns an array giving a new id to every node
 * by excluding the unused nodes, for which the array holds -1. The result array is
 * a mapping in "Old to New" mode. 
 *  \param [out] nbrOfNodesInUse - number of node ids present in the nodal connectivity.
 *  \return DataArrayInt * - a new instance of DataArrayInt. Its length is \a
 *          this->getNumberOfNodes(). It holds for each node of \a this mesh either -1
 *          if the node is unused or a new id else. The caller is to delete this
 *          array using decrRef() as it is no more needed.  
 *  \throw If the coordinates array is not set.
 *  \throw If the nodal connectivity of cells is not defined.
 *  \throw If the nodal connectivity includes an invalid id.
 */
DataArrayInt *MEDCoupling1DGTUMesh::getNodeIdsInUse(int& nbrOfNodesInUse) const throw(INTERP_KERNEL::Exception)
{
  nbrOfNodesInUse=-1;
  int nbOfNodes=getNumberOfNodes();
  int nbOfCells=getNumberOfCells();//checkCoherency
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfNodes,1);
  int *traducer=ret->getPointer();
  std::fill(traducer,traducer+nbOfNodes,-1);
  const int *conn=_conn->begin(),*conni(_conn_indx->begin());
  for(int i=0;i<nbOfCells;i++,conni++)
    {
      int nbNodesPerCell=conni[1]-conni[0];
      for(int j=0;j<nbNodesPerCell;j++)
        {
          int nodeId=conn[conni[0]+j];
          if(nodeId==-1) continue;
          if(nodeId>=0 && nodeId<nbOfNodes)
            traducer[*conn]=1;
          else
            {
              std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::getNodeIdsInUse : In cell #" << i  << " presence of node id " <<  nodeId << " not in [0," << nbOfNodes << ") !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
    }
  nbrOfNodesInUse=(int)std::count(traducer,traducer+nbOfNodes,1);
  std::transform(traducer,traducer+nbOfNodes,traducer,MEDCouplingAccVisit());
  return ret.retn();
}

/*!
 * Changes ids of nodes within the nodal connectivity arrays according to a permutation
 * array in "Old to New" mode. The node coordinates array is \b not changed by this method.
 * This method is a generalization of shiftNodeNumbersInConn().
 *  \warning This method performs no check of validity of new ids. **Use it with care !**
 *  \param [in] newNodeNumbersO2N - a permutation array, of length \a
 *         this->getNumberOfNodes(), in "Old to New" mode. 
 *         See \ref MEDCouplingArrayRenumbering for more info on renumbering modes.
 *  \throw If the nodal connectivity of cells is not defined.
 */
void MEDCoupling1DGTUMesh::renumberNodesInConn(const int *newNodeNumbersO2N)
{
  getNumberOfCells();//only to check that all is well defined.
  //
  int nbElemsIn=getNumberOfNodes();
  int nbOfTuples=_conn->getNumberOfTuples();
  int *pt=_conn->getPointer();
  for(int i=0;i<nbOfTuples;i++,pt++)
    {
      if(*pt==-1) continue;
      if(*pt>=0 && *pt<nbElemsIn)
        *pt=newNodeNumbersO2N[*pt];
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::renumberNodesInConn : error on tuple #" << i << " value is " << *pt << " and indirectionnal array as a size equal to " << nbElemsIn;
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  _conn->declareAsNew();
  //
  updateTime();
}

/*!
 * Keeps from \a this only cells which constituing point id are in the ids specified by [\a begin,\a end).
 * The resulting cell ids are stored at the end of the 'cellIdsKept' parameter.
 * Parameter \a fullyIn specifies if a cell that has part of its nodes in ids array is kept or not.
 * If \a fullyIn is true only cells whose ids are \b fully contained in [\a begin,\a end) tab will be kept.
 *
 * \param [in] begin input start of array of node ids.
 * \param [in] end input end of array of node ids.
 * \param [in] fullyIn input that specifies if all node ids must be in [\a begin,\a end) array to consider cell to be in.
 * \param [in,out] cellIdsKeptArr array where all candidate cell ids are put at the end.
 */
void MEDCoupling1DGTUMesh::fillCellIdsToKeepFromNodeIds(const int *begin, const int *end, bool fullyIn, DataArrayInt *&cellIdsKeptArr) const
{
  int nbOfCells=getNumberOfCells();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cellIdsKept=DataArrayInt::New(); cellIdsKept->alloc(0,1);
  int tmp=-1;
  int sz=_conn->getMaxValue(tmp); sz=std::max(sz,0)+1;
  std::vector<bool> fastFinder(sz,false);
  for(const int *work=begin;work!=end;work++)
    if(*work>=0 && *work<sz)
      fastFinder[*work]=true;
  const int *conn=_conn->begin(),*conni=_conn_indx->begin();
  for(int i=0;i<nbOfCells;i++,conni++)
    {
      int ref=0,nbOfHit=0;
      int nbNodesPerCell=conni[1]-conni[0];
      if(nbNodesPerCell>=0)
        {
          for(int j=0;j<nbNodesPerCell;j++)
            {
              int nodeId=conn[conni[0]+j];
              if(nodeId>=0)
                {
                  ref++;
                  if(fastFinder[nodeId])
                    nbOfHit++;
                }
            }
        }
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::fillCellIdsToKeepFromNodeIds : invalid index array for cell #" << i << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
      if((ref==nbOfHit && fullyIn) || (nbOfHit!=0 && !fullyIn))
        cellIdsKept->pushBackSilent(i);
    }
  cellIdsKeptArr=cellIdsKept.retn();
}

void MEDCoupling1DGTUMesh::allocateCells(int nbOfCells) throw(INTERP_KERNEL::Exception)
{
  if(nbOfCells<0)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::allocateCells : the input number of cells should be >= 0 !");
  _conn=DataArrayInt::New();
  _conn->reserve(nbOfCells*3);
  _conn_indx=DataArrayInt::New();
  _conn_indx->reserve(nbOfCells+1); _conn_indx->pushBackSilent(0);
  declareAsNew();
}

/*!
 * Appends at the end of \a this a cell having nodal connectivity array defined in [ \a nodalConnOfCellBg, \a nodalConnOfCellEnd ).
 *
 * \param [in] nodalConnOfCellBg - the begin (included) of nodal connectivity of the cell to add.
 * \param [in] nodalConnOfCellEnd - the end (excluded) of nodal connectivity of the cell to add.
 * \throw If the length of the input nodal connectivity array of the cell to add is not equal to number of nodes per cell relative to the unique geometric type
 *        attached to \a this.
 * \thow If the nodal connectivity array in \a this is null (call MEDCoupling1SGTUMesh::allocateCells before).
 */
void MEDCoupling1DGTUMesh::insertNextCell(const int *nodalConnOfCellBg, const int *nodalConnOfCellEnd) throw(INTERP_KERNEL::Exception)
{
  int sz=(int)std::distance(nodalConnOfCellBg,nodalConnOfCellEnd);
  DataArrayInt *c(_conn),*c2(_conn_indx);
  if(c && c2)
    {
      int pos=c2->back();
      if(pos==c->getNumberOfTuples())
        {
          c->pushBackValsSilent(nodalConnOfCellBg,nodalConnOfCellEnd);
          c2->pushBackSilent(pos+sz);
        }
      else
        {
          std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::insertNextCell : The nodal index array (end=" << pos << ") mismatches with nodal array (length=" << c->getNumberOfTuples() << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  else
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::insertNextCell : nodal connectivity array is null ! Call MEDCoupling1DGTUMesh::allocateCells before !");
}

void MEDCoupling1DGTUMesh::setNodalConnectivity(DataArrayInt *nodalConn, DataArrayInt *nodalConnIndex) throw(INTERP_KERNEL::Exception)
{
  if(nodalConn)
    nodalConn->incrRef();
  _conn=nodalConn;
  if(nodalConnIndex)
    nodalConnIndex->incrRef();
  _conn_indx=nodalConnIndex;
  declareAsNew();
}

/*!
 * \return DataArrayInt * - the internal reference to the nodal connectivity. The caller is not reponsible to deallocate it.
 */
DataArrayInt *MEDCoupling1DGTUMesh::getNodalConnectivity() const throw(INTERP_KERNEL::Exception)
{
  const DataArrayInt *ret(_conn);
  return const_cast<DataArrayInt *>(ret);
}

/*!
 * \return DataArrayInt * - the internal reference to the nodal connectivity index. The caller is not reponsible to deallocate it.
 */
DataArrayInt *MEDCoupling1DGTUMesh::getNodalConnectivityIndex() const throw(INTERP_KERNEL::Exception)
{
  const DataArrayInt *ret(_conn_indx);
  return const_cast<DataArrayInt *>(ret);
}

/*!
 * See the definition of the nodal connectivity pack \ref MEDCoupling1DGTUMesh::isPacked "here".
 * This method tries to build a new instance geometrically equivalent to \a this, by limiting at most the number of new object (nodal connectivity).
 * Geometrically the returned mesh is equal to \a this. So if \a this is already packed, the return value is a shallow copy of \a this.
 *
 * Whatever the status of pack of \a this, the coordinates array of the returned newly created instance is the same than those in \a this.
 * 
 * \param [out] isShallowCpyOfNodalConnn - tells if the returned instance share the same pair of nodal connectivity arrays (true) or if nodal
 *              connectivity arrays are different (false)
 * \return a new object to be managed by the caller.
 * 
 * \sa MEDCoupling1DGTUMesh::retrievePackedNodalConnectivity, MEDCoupling1DGTUMesh::isPacked
 */
MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::copyWithNodalConnectivityPacked(bool& isShallowCpyOfNodalConnn) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1DGTUMesh> ret(new MEDCoupling1DGTUMesh(getName(),*_cm));
  DataArrayInt *nc=0,*nci=0;
  isShallowCpyOfNodalConnn=retrievePackedNodalConnectivity(nc,nci);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ncs(nc),ncis(nci);
  ret->_conn=ncs; ret->_conn_indx=ncis;
  ret->setCoords(getCoords());
  return ret.retn();
}

/*!
 * This method allows to compute, if needed, the packed nodal connectivity pair.
 * Indeed, it is possible to store in \a this a nodal connectivity array bigger than ranges convered by nodal connectivity index array.
 * It is typically the case when nodalConnIndx starts with an id greater than 0, and finishes with id less than number of tuples in \c this->_conn.
 * 
 * If \a this looks packed (the front of nodal connectivity index equal to 0 and back of connectivity index equal to number of tuple of nodal connectivity array)
 * true will be returned and respectively \a this->_conn and \a this->_conn_indx (with ref counter incremented). This is the classical case.
 *
 * If nodal connectivity index points to a subpart of nodal connectivity index the packed pair of arrays will be computed (new objects) and returned and false
 * will be returned.
 * 
 * This method return 3 elements.
 * \param [out] nodalConn - a pointer that can be equal to \a this->_conn if true is returned (general case). Whatever the value of return parameter
 *                          this pointer can be seen as a new object, that is to managed by the caller.
 * \param [out] nodalConnIndx - a pointer that can be equal to \a this->_conn_indx if true is returned (general case). Whatever the value of return parameter
 *                              this pointer can be seen as a new object, that is to managed by the caller.
 * \return bool - an indication of the content of the 2 output parameters. If true, \a this looks packed (general case), if true, \a this is not packed then
 * output parameters are newly created objects.
 *
 * \throw if \a this does not pass MEDCoupling1DGTUMesh::checkCoherency test
 */
bool MEDCoupling1DGTUMesh::retrievePackedNodalConnectivity(DataArrayInt *&nodalConn, DataArrayInt *&nodalConnIndx) const throw(INTERP_KERNEL::Exception)
{
  if(isPacked())//performs the checkCoherency
    {
      const DataArrayInt *c0(_conn),*c1(_conn_indx);
      nodalConn=const_cast<DataArrayInt *>(c0); nodalConnIndx=const_cast<DataArrayInt *>(c1);
      nodalConn->incrRef(); nodalConnIndx->incrRef();
      return true;
    }
  int bg=_conn_indx->front(),end=_conn_indx->back();
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> nc(_conn->selectByTupleId2(bg,end,1));
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> nci(_conn_indx->deepCpy());
  nci->applyLin(1,-bg);
  nodalConn=nc.retn(); nodalConnIndx=nci.retn();
  return false;
}

/*
 * If \a this looks packed (the front of nodal connectivity index equal to 0 and back of connectivity index equal to number of tuple of nodal connectivity array)
 * true will be returned and respectively \a this->_conn and \a this->_conn_indx (with ref counter incremented). This is the classical case.
 * If nodal connectivity index points to a subpart of nodal connectivity index false will be returned.
 * \return bool - true if \a this looks packed, false is not.
 *
 * \throw if \a this does not pass MEDCoupling1DGTUMesh::checkCoherency test
 */
bool MEDCoupling1DGTUMesh::isPacked() const throw(INTERP_KERNEL::Exception)
{
  checkCoherency();
  return _conn_indx->front()==0 && _conn_indx->back()==_conn->getNumberOfTuples();
}

MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::Merge1DGTUMeshes(const MEDCoupling1DGTUMesh *mesh1, const MEDCoupling1DGTUMesh *mesh2) throw(INTERP_KERNEL::Exception)
{
  std::vector<const MEDCoupling1DGTUMesh *> tmp(2);
  tmp[0]=const_cast<MEDCoupling1DGTUMesh *>(mesh1); tmp[1]=const_cast<MEDCoupling1DGTUMesh *>(mesh2);
  return Merge1DGTUMeshes(tmp);
}

MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::Merge1DGTUMeshes(std::vector<const MEDCoupling1DGTUMesh *>& a) throw(INTERP_KERNEL::Exception)
{
  std::size_t sz=a.size();
  if(sz==0)
    return Merge1DGTUMeshesLL(a);
  for(std::size_t ii=0;ii<sz;ii++)
    if(!a[ii])
      {
        std::ostringstream oss; oss << "MEDCoupling1DGTUMesh::Merge1DGTUMeshes : item #" << ii << " in input array of size "<< sz << " is empty !";
        throw INTERP_KERNEL::Exception(oss.str().c_str());
      }
  const INTERP_KERNEL::CellModel *cm=&(a[0]->getCellModel());
  for(std::size_t ii=0;ii<sz;ii++)
    if(&(a[ii]->getCellModel())!=cm)
      throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::Merge1DGTUMeshes : all items must have the same geo type !");
  std::vector< MEDCouplingAutoRefCountObjectPtr<MEDCoupling1DGTUMesh> > bb(sz);
  std::vector< const MEDCoupling1DGTUMesh * > aa(sz);
  int spaceDim=-3;
  for(std::size_t i=0;i<sz && spaceDim==-3;i++)
    {
      const MEDCoupling1DGTUMesh *cur=a[i];
      const DataArrayDouble *coo=cur->getCoords();
      if(coo)
        spaceDim=coo->getNumberOfComponents();
    }
  if(spaceDim==-3)
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::Merge1DGTUMeshes : no spaceDim specified ! unable to perform merge !");
  for(std::size_t i=0;i<sz;i++)
    {
      bb[i]=a[i]->buildSetInstanceFromThis(spaceDim);
      aa[i]=bb[i];
    }
  return Merge1DGTUMeshesLL(aa);
}

MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::Merge1DGTUMeshesOnSameCoords(std::vector<const MEDCoupling1DGTUMesh *>& a) throw(INTERP_KERNEL::Exception)
{
  if(a.empty())
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::Merge1DGTUMeshesOnSameCoords : input array must be NON EMPTY !");
  std::vector<const MEDCoupling1DGTUMesh *>::const_iterator it=a.begin();
  if(!(*it))
    throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::Merge1DGTUMeshesOnSameCoords : presence of null instance !");
  std::vector< MEDCouplingAutoRefCountObjectPtr<MEDCoupling1DGTUMesh> > objs(a.size());
  std::vector<const DataArrayInt *> ncs(a.size()),ncis(a.size());
  int nbOfCells=(*it)->getNumberOfCells();
  const DataArrayDouble *coords=(*it)->getCoords();
  const INTERP_KERNEL::CellModel *cm=&((*it)->getCellModel());
  bool tmp;
  objs[0]=(*it)->copyWithNodalConnectivityPacked(tmp);
  ncs[0]=objs[0]->getNodalConnectivity(); ncis[0]=objs[0]->getNodalConnectivityIndex();
  it++;
  for(int i=1;it!=a.end();i++,it++)
    {
      if(cm!=&((*it)->getCellModel()))
        throw INTERP_KERNEL::Exception("Geometric types mismatches, Merge1DGTUMeshes impossible !");
      (*it)->getNumberOfCells();//to check that all is OK
      objs[i]=(*it)->copyWithNodalConnectivityPacked(tmp);
      ncs[i]=objs[i]->getNodalConnectivity(); ncis[i]=objs[i]->getNodalConnectivityIndex();
      if(coords!=(*it)->getCoords())
        throw INTERP_KERNEL::Exception("MEDCoupling1DGTUMesh::Merge1DGTUMeshesOnSameCoords : not lying on same coords !");
    }
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1DGTUMesh> ret(new MEDCoupling1DGTUMesh("merge",*cm));
  ret->setCoords(coords);
  ret->_conn=DataArrayInt::Aggregate(ncs);
  ret->_conn_indx=DataArrayInt::AggregateIndexes(ncis);
  return ret.retn();
}

MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::Merge1DGTUMeshesLL(std::vector<const MEDCoupling1DGTUMesh *>& a) throw(INTERP_KERNEL::Exception)
{
  //tony
}

MEDCoupling1DGTUMesh *MEDCoupling1DGTUMesh::buildSetInstanceFromThis(int spaceDim) const throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDCoupling1DGTUMesh> ret(new MEDCoupling1DGTUMesh(getName(),*_cm));
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> tmp1,tmp2;
  const DataArrayInt *nodalConn(_conn),*nodalConnI(_conn_indx);
  if(!nodalConn)
    {
      tmp1=DataArrayInt::New(); tmp1->alloc(0,1);
    }
  else
    tmp1=_conn;
  ret->_conn=tmp1;
  //
  if(!nodalConnI)
    {
      tmp2=DataArrayInt::New(); tmp2->alloc(1,1); tmp2->setIJ(0,0,0);
    }
  else
    tmp2=_conn_indx;
  ret->_conn_indx=tmp2;
  //
  if(!_coords)
    {
      MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> coords=DataArrayDouble::New(); coords->alloc(0,spaceDim);
      ret->setCoords(coords);
    }
  else
    ret->setCoords(_coords);
  return ret.retn();
}