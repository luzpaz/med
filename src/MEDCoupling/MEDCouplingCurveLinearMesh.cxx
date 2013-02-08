// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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

#include "MEDCouplingCurveLinearMesh.hxx"
#include "MEDCouplingPointSet.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingFieldDouble.hxx"

#include <functional>
#include <algorithm>
#include <sstream>
#include <numeric>

using namespace ParaMEDMEM;

MEDCouplingCurveLinearMesh::MEDCouplingCurveLinearMesh():_coords(0),_structure(0)
{
}

MEDCouplingCurveLinearMesh::MEDCouplingCurveLinearMesh(const MEDCouplingCurveLinearMesh& other, bool deepCopy):MEDCouplingStructuredMesh(other,deepCopy),_structure(other._structure)
{
  if(deepCopy)
    {
      if((const DataArrayDouble *)other._coords)
        _coords=other._coords->deepCpy();
    }
  else
    _coords=other._coords;
}

MEDCouplingCurveLinearMesh::~MEDCouplingCurveLinearMesh()
{
}

MEDCouplingCurveLinearMesh *MEDCouplingCurveLinearMesh::New()
{
  return new MEDCouplingCurveLinearMesh;
}

MEDCouplingCurveLinearMesh *MEDCouplingCurveLinearMesh::New(const char *meshName)
{
  MEDCouplingCurveLinearMesh *ret=new MEDCouplingCurveLinearMesh;
  ret->setName(meshName);
  return ret;
}

MEDCouplingMesh *MEDCouplingCurveLinearMesh::deepCpy() const
{
  return clone(true);
}

MEDCouplingCurveLinearMesh *MEDCouplingCurveLinearMesh::clone(bool recDeepCpy) const
{
  return new MEDCouplingCurveLinearMesh(*this,recDeepCpy);
}

void MEDCouplingCurveLinearMesh::updateTime() const
{
  if((const DataArrayDouble *)_coords)
    updateTimeWith(*_coords);
}

std::size_t MEDCouplingCurveLinearMesh::getHeapMemorySize() const
{
  std::size_t ret=0;
  ret+=_structure.capacity()*sizeof(int);
  if((const DataArrayDouble *)_coords)
    ret+=_coords->getHeapMemorySize();
  return MEDCouplingStructuredMesh::getHeapMemorySize()+ret;
}

/*!
 * This method copyies all tiny strings from other (name and components name).
 * @throw if other and this have not same mesh type.
 */
void MEDCouplingCurveLinearMesh::copyTinyStringsFrom(const MEDCouplingMesh *other) throw(INTERP_KERNEL::Exception)
{ 
  const MEDCouplingCurveLinearMesh *otherC=dynamic_cast<const MEDCouplingCurveLinearMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::copyTinyStringsFrom : meshes have not same type !");
  MEDCouplingStructuredMesh::copyTinyStringsFrom(other);
  if((DataArrayDouble *)_coords && (const DataArrayDouble *)otherC->_coords)
    _coords->copyStringInfoFrom(*otherC->_coords);
}

bool MEDCouplingCurveLinearMesh::isEqualIfNotWhy(const MEDCouplingMesh *other, double prec, std::string& reason) const throw(INTERP_KERNEL::Exception)
{
  if(!other)
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::isEqualIfNotWhy : input other pointer is null !");
  const MEDCouplingCurveLinearMesh *otherC=dynamic_cast<const MEDCouplingCurveLinearMesh *>(other);
  if(!otherC)
    {
      reason="mesh given in input is not castable in MEDCouplingCurveLinearMesh !";
      return false;
    }
  if(!MEDCouplingStructuredMesh::isEqualIfNotWhy(other,prec,reason))
    return false;
  std::ostringstream oss; oss.precision(15);
  if(((const DataArrayDouble *)_coords && ((const DataArrayDouble *)otherC->_coords)==0) || (((const DataArrayDouble *)_coords)==0 && (const DataArrayDouble *)otherC->_coords))
    {
      oss << "Only one CurveLinearMesh between the two this and other has its coordinates defined !";
      reason=oss.str();
      return false;
    }
  if((const DataArrayDouble *)_coords)
    {
      if(!_coords->isEqualIfNotWhy(*(otherC->_coords),prec,reason))
        {
          oss << "Coordinates DataArrayDouble of differ :";
          reason.insert(0,oss.str());
          return false;
        }
      if(_structure!=otherC->_structure)
        { reason="CurveLinearMesh structures differ !"; return false; }
    }
  return true;
}

bool MEDCouplingCurveLinearMesh::isEqualWithoutConsideringStr(const MEDCouplingMesh *other, double prec) const
{
  const MEDCouplingCurveLinearMesh *otherC=dynamic_cast<const MEDCouplingCurveLinearMesh *>(other);
  if(!otherC)
    return false;
  if(((const DataArrayDouble *)_coords && ((const DataArrayDouble *)otherC->_coords)==0) || (((const DataArrayDouble *)_coords)==0 && (const DataArrayDouble *)otherC->_coords))
    return false;
  if((const DataArrayDouble *)_coords)
    {
      if(!_coords->isEqualWithoutConsideringStr(*(otherC->_coords),prec))
        return false;
      if(_structure!=otherC->_structure)
        return false;
    }
  return true;
}

void MEDCouplingCurveLinearMesh::checkDeepEquivalWith(const MEDCouplingMesh *other, int cellCompPol, double prec,
                                            DataArrayInt *&cellCor, DataArrayInt *&nodeCor) const throw(INTERP_KERNEL::Exception)
{
  if(!isEqualWithoutConsideringStr(other,prec))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::checkDeepEquivalWith : Meshes are not the same !");
}

/*!
 * Nothing is done here (except to check that the other is a ParaMEDMEM::MEDCouplingCurveLinearMesh instance too).
 * The user intend that the nodes are the same, so by construction of ParaMEDMEM::MEDCouplingCurveLinearMesh, 'this' and 'other' are the same !
 */
void MEDCouplingCurveLinearMesh::checkDeepEquivalOnSameNodesWith(const MEDCouplingMesh *other, int cellCompPol, double prec,
                                                       DataArrayInt *&cellCor) const throw(INTERP_KERNEL::Exception)
{
  const MEDCouplingCurveLinearMesh *otherC=dynamic_cast<const MEDCouplingCurveLinearMesh *>(other);
  if(!otherC)
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::checkDeepEquivalOnSameNodesWith : other is NOT a cartesian mesh ! Impossible to check equivalence !");
}

void MEDCouplingCurveLinearMesh::checkCoherency() const throw(INTERP_KERNEL::Exception)
{
  std::size_t sz=_structure.size(),i=0,nbOfNodes=1;
  if(sz<1)
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::checkCoherency : structure should have a lgth of size 1 at least !");
  for(std::vector<int>::const_iterator it=_structure.begin();it!=_structure.end();it++,i++)
    {
      if((*it)<1)
        { std::ostringstream oss; oss << "MEDCouplingCurveLinearMesh::checkCoherency : At pos #" << i << " of structure value is " << *it << "should be >= 1 !"; throw INTERP_KERNEL::Exception(oss.str().c_str()); }
      nbOfNodes*=*it;
    }
  if(!((const DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::checkCoherency : the array is not set !");
  if(!_coords->isAllocated())
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::checkCoherency : the array is not allocated !");
  if(_coords->getNumberOfComponents()<1)
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::checkCoherency : the array should have >= 1 components !");
  if(_coords->getNumberOfTuples()!=(int)nbOfNodes)
    {
      std::ostringstream oss; oss << "MEDCouplingCurveLinearMesh::checkCoherency : structure said that number of nodes should be equal to " << nbOfNodes << " but number of tuples in array is equal to " << _coords->getNumberOfTuples() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void MEDCouplingCurveLinearMesh::checkCoherency1(double eps) const throw(INTERP_KERNEL::Exception)
{
  checkCoherency();
}

void MEDCouplingCurveLinearMesh::checkCoherency2(double eps) const throw(INTERP_KERNEL::Exception)
{
  checkCoherency1(eps);
}

int MEDCouplingCurveLinearMesh::getNumberOfCells() const
{
  checkCoherency();
  std::size_t nbOfCells=1,i=0;
  for(std::vector<int>::const_iterator it=_structure.begin();it!=_structure.end();it++,i++)
    nbOfCells*=(*it)-1;
  return (int)nbOfCells;
}

int MEDCouplingCurveLinearMesh::getNumberOfNodes() const
{
  checkCoherency();
  std::size_t nbOfNodes=1;
  for(std::vector<int>::const_iterator it=_structure.begin();it!=_structure.end();it++)
    nbOfNodes*=(*it);
  return (int)nbOfNodes;
}

void MEDCouplingCurveLinearMesh::getSplitCellValues(int *res) const
{
  int meshDim=getMeshDimension();
  for(int l=0;l<meshDim;l++)
    {
      int val=1;
      for(int p=0;p<meshDim-l-1;p++)
        val*=_structure[p]-1;
      res[meshDim-l-1]=val;
    }
}

void MEDCouplingCurveLinearMesh::getSplitNodeValues(int *res) const
{
  int meshDim=getMeshDimension();
  for(int l=0;l<meshDim;l++)
    {
      int val=1;
      for(int p=0;p<meshDim-l-1;p++)
        val*=_structure[p];
      res[meshDim-l-1]=val;
    }
}

void MEDCouplingCurveLinearMesh::getNodeGridStructure(int *res) const
{
  std::copy(_structure.begin(),_structure.end(),res);
}

int MEDCouplingCurveLinearMesh::getSpaceDimension() const
{
  if(!((const DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::getSpaceDimension : no array set ! impossible to deduce a space dimension !");
  return _coords->getNumberOfComponents();
}

int MEDCouplingCurveLinearMesh::getMeshDimension() const
{
  return (int)_structure.size();
}

void MEDCouplingCurveLinearMesh::getCoordinatesOfNode(int nodeId, std::vector<double>& coo) const throw(INTERP_KERNEL::Exception)
{
  if(!((const DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::getCoordinatesOfNode : Coordinates not set !");
  int nbOfCompo=_coords->getNumberOfComponents();
  if(nodeId>=0 && nodeId<_coords->getNumberOfTuples())
    coo.insert(coo.end(),_coords->begin()+nodeId*nbOfCompo,_coords->begin()+(nodeId+1)*nbOfCompo);
  else
    { std::ostringstream oss; oss << "MEDCouplingCurveLinearMesh::getCoordinatesOfNode : nodeId has to be in [0," << _coords->getNumberOfTuples() << ") !"; throw INTERP_KERNEL::Exception(oss.str().c_str()); }
}

std::string MEDCouplingCurveLinearMesh::simpleRepr() const
{
  std::ostringstream ret;
  ret << "Curve linear mesh with name : \"" << getName() << "\"\n";
  ret << "Description of mesh : \"" << getDescription() << "\"\n";
  int tmpp1,tmpp2;
  double tt=getTime(tmpp1,tmpp2);
  ret << "Time attached to the mesh [unit] : " << tt << " [" << getTimeUnit() << "]\n";
  ret << "Iteration : " << tmpp1  << " Order : " << tmpp2 << "\n";
  ret << "The nodal stucture of curve linear mesh is : [";
  std::copy(_structure.begin(),_structure.end(),std::ostream_iterator<int>(ret,",")); ret << "]\n";
  ret << "The coords array is this : ";
  if((const DataArrayDouble *)_coords)
    _coords->reprZipWithoutNameStream(ret);
  else
    ret << "no array specified !";
  return ret.str();
}

std::string MEDCouplingCurveLinearMesh::advancedRepr() const
{
  return simpleRepr();
}

DataArrayDouble *MEDCouplingCurveLinearMesh::getCoords() throw(INTERP_KERNEL::Exception)
{
  return _coords;
}

const DataArrayDouble *MEDCouplingCurveLinearMesh::getCoords() const throw(INTERP_KERNEL::Exception)
{
  return _coords;
}

void MEDCouplingCurveLinearMesh::setCoords(const DataArrayDouble *coords) throw(INTERP_KERNEL::Exception)
{
  if(coords!=(const DataArrayDouble *)_coords)
    {
      _coords=const_cast<DataArrayDouble *>(coords);
      if(coords)
        coords->incrRef();
      declareAsNew();
    }
}

void MEDCouplingCurveLinearMesh::setNodeGridStructure(const int *gridStructBg, const int *gridStructEnd) throw(INTERP_KERNEL::Exception)
{
  _structure.resize(0);
  _structure.insert(_structure.end(),gridStructBg,gridStructEnd);
}

std::vector<int> MEDCouplingCurveLinearMesh::getNodeGridStructure() const throw(INTERP_KERNEL::Exception)
{
  return _structure;
}

void MEDCouplingCurveLinearMesh::getBoundingBox(double *bbox) const
{
  if(!((const DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::getBoundingBox : Coordinates not set !");
  _coords->getMinMaxPerComponent(bbox);
}

MEDCouplingFieldDouble *MEDCouplingCurveLinearMesh::getMeasureField(bool isAbs) const
{
  throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::getMeasureField : not implemented yet !");
}

/*!
 * not implemented yet !
 */
MEDCouplingFieldDouble *MEDCouplingCurveLinearMesh::getMeasureFieldOnNode(bool isAbs) const
{
  throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::getMeasureFieldOnNode : not implemented yet !");
}

MEDCouplingFieldDouble *MEDCouplingCurveLinearMesh::buildOrthogonalField() const
{
  if(getMeshDimension()!=2)
    throw INTERP_KERNEL::Exception("Expected a cmesh with meshDim == 2 !");
  MEDCouplingFieldDouble *ret=MEDCouplingFieldDouble::New(ON_CELLS,NO_TIME);
  DataArrayDouble *array=DataArrayDouble::New();
  int nbOfCells=getNumberOfCells();
  array->alloc(nbOfCells,3);
  double *vals=array->getPointer();
  for(int i=0;i<nbOfCells;i++)
    { vals[3*i]=0.; vals[3*i+1]=0.; vals[3*i+2]=1.; }
  ret->setArray(array);
  array->decrRef();
  ret->setMesh(this);
  return ret;
}

int MEDCouplingCurveLinearMesh::getCellContainingPoint(const double *pos, double eps) const
{
  throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::getCellContainingPoint : not implemented yet !");
}

void MEDCouplingCurveLinearMesh::rotate(const double *center, const double *vector, double angle)
{
  if(!((DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::rotate : no coordinates set !");
  int spaceDim=getSpaceDimension();
  int nbNodes=_coords->getNumberOfTuples();
  double *coords=_coords->getPointer();
  if(spaceDim==3)
    MEDCouplingPointSet::Rotate3DAlg(center,vector,angle,nbNodes,coords);
  else if(spaceDim==2)
    MEDCouplingPointSet::Rotate2DAlg(center,angle,nbNodes,coords);
  else
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::rotate : invalid space dim for rotation must be 2 or 3");
  _coords->declareAsNew();
  updateTime();
}

void MEDCouplingCurveLinearMesh::translate(const double *vector)
{
  if(!((DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::translate : no coordinates set !");
  double *coords=_coords->getPointer();
  int nbNodes=getNumberOfNodes();
  int dim=getSpaceDimension();
  for(int i=0; i<nbNodes; i++)
    for(int idim=0; idim<dim;idim++)
      coords[i*dim+idim]+=vector[idim];
  _coords->declareAsNew();
  updateTime();
}

void MEDCouplingCurveLinearMesh::scale(const double *point, double factor)
{
  if(!((DataArrayDouble *)_coords))
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::scale : no coordinates set !");
  double *coords=_coords->getPointer();
  int nbNodes=_coords->getNumberOfTuples();
  int dim=_coords->getNumberOfComponents();
  for(int i=0;i<nbNodes;i++)
    {
      std::transform(coords+i*dim,coords+(i+1)*dim,point,coords+i*dim,std::minus<double>());
      std::transform(coords+i*dim,coords+(i+1)*dim,coords+i*dim,std::bind2nd(std::multiplies<double>(),factor));
      std::transform(coords+i*dim,coords+(i+1)*dim,point,coords+i*dim,std::plus<double>());
    }
  _coords->declareAsNew();
  updateTime();
}

MEDCouplingMesh *MEDCouplingCurveLinearMesh::mergeMyselfWith(const MEDCouplingMesh *other) const
{
  //not implemented yet !
  return 0;
}

DataArrayDouble *MEDCouplingCurveLinearMesh::getCoordinatesAndOwner() const
{
  DataArrayDouble *ret=const_cast<DataArrayDouble *>((const DataArrayDouble *)_coords);
  if(ret)
    ret->incrRef();
  return ret;
}

DataArrayDouble *MEDCouplingCurveLinearMesh::getBarycenterAndOwner() const
{
  throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::getBarycenterAndOwner : not implemented yet !");
}

void MEDCouplingCurveLinearMesh::renumberCells(const int *old2NewBg, bool check) throw(INTERP_KERNEL::Exception)
{
  throw INTERP_KERNEL::Exception("Functionnality of renumbering cell not available for CurveLinear Mesh !");
}

void MEDCouplingCurveLinearMesh::getTinySerializationInformation(std::vector<double>& tinyInfoD, std::vector<int>& tinyInfo, std::vector<std::string>& littleStrings) const
{
  int it,order;
  double time=getTime(it,order);
  tinyInfo.clear();
  tinyInfoD.clear();
  littleStrings.clear();
  littleStrings.push_back(getName());
  littleStrings.push_back(getDescription());
  littleStrings.push_back(getTimeUnit());
  //
  std::vector<std::string> littleStrings2;
  if((const DataArrayDouble *)_coords)
    _coords->getTinySerializationStrInformation(littleStrings2);
  littleStrings.insert(littleStrings.end(),littleStrings2.begin(),littleStrings2.end());
  //
  tinyInfo.push_back(it);
  tinyInfo.push_back(order);
  tinyInfo.push_back((int)_structure.size());
  for(std::vector<int>::const_iterator itt=_structure.begin();itt!=_structure.end();itt++)
    tinyInfo.push_back(*itt);
  std::vector<int> tinyInfo2;
  if((const DataArrayDouble *)_coords)
    _coords->getTinySerializationIntInformation(tinyInfo2);
  tinyInfo.insert(tinyInfo.end(),tinyInfo2.begin(),tinyInfo2.end());
  //
  tinyInfoD.push_back(time);
}

void MEDCouplingCurveLinearMesh::resizeForUnserialization(const std::vector<int>& tinyInfo, DataArrayInt *a1, DataArrayDouble *a2, std::vector<std::string>& littleStrings) const
{
  a1->alloc(tinyInfo[2],1);
  std::vector<int> tinyInfo2(tinyInfo.begin()+3+tinyInfo[2],tinyInfo.end());
  a2->resizeForUnserialization(tinyInfo2);
}

void MEDCouplingCurveLinearMesh::serialize(DataArrayInt *&a1, DataArrayDouble *&a2) const
{
  a1=DataArrayInt::New();
  a1->alloc((int)_structure.size(),1);
  int *ptr=a1->getPointer();
  for(std::vector<int>::const_iterator it=_structure.begin();it!=_structure.end();it++,ptr++)
    *ptr=(*it);
  int sz=0;
  if((const DataArrayDouble *)_coords)
    if(_coords->isAllocated())
      sz=_coords->getNbOfElems();
  a2=DataArrayDouble::New();
  a2->alloc(sz,1);
  if(sz!=0 && (const DataArrayDouble *)_coords)
    std::copy(_coords->begin(),_coords->end(),a2->getPointer());
}

void MEDCouplingCurveLinearMesh::unserialization(const std::vector<double>& tinyInfoD, const std::vector<int>& tinyInfo, const DataArrayInt *a1, DataArrayDouble *a2,
                                                 const std::vector<std::string>& littleStrings)
{
  setName(littleStrings[0].c_str());
  setDescription(littleStrings[1].c_str());
  setTimeUnit(littleStrings[2].c_str());
  setTime(tinyInfoD[0],tinyInfo[0],tinyInfo[1]);
  int sz=tinyInfo[2];
  _structure.resize(sz);
  for(int i=0;i<sz;i++)
    _structure[i]=tinyInfo[3+i];
  if((int)tinyInfo.size()>sz+3)
    {
      _coords=DataArrayDouble::New();
      std::vector<int> tinyInfo2(tinyInfo.begin()+3+sz,tinyInfo.end());
      _coords->resizeForUnserialization(tinyInfo2);
      std::copy(a2->begin(),a2->end(),_coords->getPointer());
      std::vector<std::string> littleStrings2(littleStrings.begin()+3,littleStrings.end());
      _coords->finishUnserialization(tinyInfo2,littleStrings2);
    }
}

void MEDCouplingCurveLinearMesh::writeVTKLL(std::ostream& ofs, const std::string& cellData, const std::string& pointData) const throw(INTERP_KERNEL::Exception)
{
  std::ostringstream extent;
  int meshDim=(int)_structure.size();
  if(meshDim<=0 || meshDim>3)
    throw INTERP_KERNEL::Exception("MEDCouplingCurveLinearMesh::writeVTKLL : meshDim invalid ! must be in [1,2,3] !");
  for(int i=0;i<3;i++)
    { int val=i<meshDim?_structure[i]-1:0; extent << "0 " <<  val << " "; }
  ofs << "  <" << getVTKDataSetType() << " WholeExtent=\"" << extent.str() << "\">\n";
  ofs << "    <Piece Extent=\"" << extent.str() << "\">\n";
  ofs << "      <PointData>\n" << pointData << std::endl;
  ofs << "      </PointData>\n";
  ofs << "      <CellData>\n" << cellData << std::endl;
  ofs << "      </CellData>\n";
  ofs << "      <Points>\n";
  if(getSpaceDimension()==3)
    _coords->writeVTK(ofs,8,"Points");
  else
    {
      MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> coo=_coords->changeNbOfComponents(3,0.);
      coo->writeVTK(ofs,8,"Points");
    }
  ofs << "      </Points>\n";
  ofs << "    </Piece>\n";
  ofs << "  </" << getVTKDataSetType() << ">\n";
}

std::string MEDCouplingCurveLinearMesh::getVTKDataSetType() const throw(INTERP_KERNEL::Exception)
{
  return std::string("StructuredGrid");
}
