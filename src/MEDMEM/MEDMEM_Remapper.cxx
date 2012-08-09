// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_Remapper.hxx"
#include "MEDMEM_Exception.hxx"
#include "Interpolation.hxx"
#include "Interpolation2D.txx"
#include "Interpolation3D.txx"
#include "Interpolation3DSurf.hxx"


  
//   int InterpolationOptions::_printLevel=0;
//   IntersectionType  InterpolationOptions::_intersectionType=Triangulation;
//   double  InterpolationOptions::_precision=1e-12;;
//   double  InterpolationOptions::_medianPlane =0.5;
//   bool  InterpolationOptions::_doRotate =true;
//   double  InterpolationOptions::_boundingBoxAdjustment =0.1;
//   int  InterpolationOptions::_orientation =0;
//   SplittingPolicy  InterpolationOptions::_splittingPolicy =GENERAL_48;

/*!
    \defgroup medmemremapper MEDMEM::MEDMEM_REMAPPER

    \section overview Overview

    \c MEDMEM_REMAPPER enables conservative remapping of fields 
    between two sequential codes. 
    The computation is possible for 3D meshes and 2D meshes or 3D surfaces. It enables fast sequential localization, based on a bounding box tree structure. It is based on cell-cell intersection or on a point in element search, depending on the field type. Fields can either lie on cells (P0) or nodes (P1).

    A typical use of \c MEDMEM_REMAPPER encompasses two distinct phases :
    - A setup phase during which the intersection volumes are computed and the interpolation matrix is stored. This corresponds to calling the \c MEDMEM_REMAPPER::prepare() method.
    - A use phase during which the remappings are actually performed. This corresponds to the calls to transfer() and reverseTransfer() which actually performs the interpolation.

    The following code excerpt illutrates a typical use of the \c MEDMEM_REMAPPER class.
 
    \code
    ...
    std::string sourceFileName("source.med");
    MEDMEM::MESH med_source_mesh(MED_DRIVER,sourceFileName,"Source_Mesh");
    std::string targetFileName("target.med");
    MEDMEM::MESH med_target_mesh(MED_DRIVER,targetFileName,"Target_Mesh");
    FIELD<double> sourceField(MED_DRIVER,sourceFileName,"Density",0,0);
    FIELD<double> targetField;
    MEDMEM_Remapper mapper;
    mapper.setOptionsDouble("Precision",1e-7);
    mapper.setOptionsString("Intersection_type",Geometric2D);
    mapper.prepare(med_source_mesh,med_target_mesh,"P0P1");
    mapper.transfer(sourceField,targetField);
    //use targetField
    ...
    \endcode

    @{
 */

MEDMEM_REMAPPER::MEDMEM_REMAPPER():_matrix(0),_sourceMesh(0), _targetMesh(0), _sourceSupport(0), _targetSupport(0)
{
}

MEDMEM_REMAPPER::~MEDMEM_REMAPPER()
{
  delete _matrix;
  if(_sourceMesh)
    _sourceMesh->removeReference();
  if(_targetMesh)
    _targetMesh->removeReference();
//   if(_sourceSupport)
//     _sourceSupport->removeReference();
//   if(_targetSupport)
//     _targetSupport->removeReference();
}
/*! This method computes the intersection matrix between 
 * source \a mesh_source and \a mesh_target. It is a preliminary step 
 * that is necessary before calling the \c transfer() method.
 * The method analyses the dimensions of the meshes and checks for compatibility.
 * 
 */
int MEDMEM_REMAPPER::prepare(const MEDMEM::MESH& mesh_source, const MEDMEM::MESH& mesh_target, const char *method)
{
  const int sm_spacedim = mesh_source.getSpaceDimension();
  const int tm_spacedim = mesh_target.getSpaceDimension();
  int sm_meshdim = mesh_source.getMeshDimension();
  int tm_meshdim = mesh_target.getMeshDimension();

  delete _matrix;
  _matrix= new INTERP_KERNEL::Matrix<double,INTERP_KERNEL::ALL_FORTRAN_MODE>;

  if(_sourceMesh)
    _sourceMesh->removeReference();
  _sourceMesh= new MEDMEM::MESH((MEDMEM::MESH&)mesh_source);
  if(_targetMesh)
    _targetMesh->removeReference();
  _targetMesh= new MEDMEM::MESH((MEDMEM::MESH&)mesh_target);

  std::string methodC=method;
  if(methodC == "P0P0"){
    _sourceFieldType = "P0";
    _targetFieldType = "P0";
  }
  else if(methodC =="P0P1"){
    _sourceFieldType = "P0";
    _targetFieldType = "P1";
  }
  else if(methodC == "P1P0"){
    _sourceFieldType = "P1";
    _targetFieldType = "P0";
  }
  else if(methodC == "P1P1"){
    _sourceFieldType = "P1";
    _targetFieldType = "P1";
  }
  else
    throw INTERP_KERNEL::Exception("MEDMEM_REMAPPER::prepare: Invalid method specified ! Must be in : \"P0P0\" \"P0P1\" \"P1P0\" or \"P1P1\"");
                

//   if(_sourceSupport)
//     _sourceSupport->removeReference();
//   if(_targetSupport)
//     _targetSupport->removeReference();
  if(   _sourceFieldType == "P0")
    _sourceSupport = ((MEDMEM::MESH *)_sourceMesh)->getSupportOnAll(MED_EN::MED_CELL);
  else
    _sourceSupport = ((MEDMEM::MESH *)_sourceMesh)->getSupportOnAll(MED_EN::MED_NODE);
  if(   _targetFieldType == "P0")
    _targetSupport = ((MEDMEM::MESH *)_targetMesh)->getSupportOnAll(MED_EN::MED_CELL);
  else
    _targetSupport = ((MEDMEM::MESH *)_targetMesh)->getSupportOnAll(MED_EN::MED_NODE);
        
  if (tm_spacedim!=sm_spacedim || tm_meshdim!=sm_meshdim)
    throw MEDEXCEPTION("incompatible mesh and/or space dimensions in meshes");
  if ((sm_spacedim==2)&&(sm_meshdim==2))
    {
      MEDNormalizedUnstructuredMesh<2,2> source_mesh_wrapper(_sourceMesh);
      MEDNormalizedUnstructuredMesh<2,2> target_mesh_wrapper(_targetMesh);
      INTERP_KERNEL::Interpolation2D interpolation(*this);
      _nb_cols = interpolation.interpolateMeshes(source_mesh_wrapper,target_mesh_wrapper,*_matrix,method);
    }
  else if ((sm_spacedim==3)&&(sm_meshdim==3))
    {
      MEDNormalizedUnstructuredMesh<3,3> source_mesh_wrapper(_sourceMesh);
      MEDNormalizedUnstructuredMesh<3,3> target_mesh_wrapper(_targetMesh);
      INTERP_KERNEL::Interpolation3D interpolation(*this);
      _nb_cols = interpolation.interpolateMeshes(source_mesh_wrapper,target_mesh_wrapper,*_matrix,method);
    }
  else if ((sm_spacedim==3)&&(sm_meshdim==2))
    {
      MEDNormalizedUnstructuredMesh<3,2> source_mesh_wrapper(_sourceMesh);
      MEDNormalizedUnstructuredMesh<3,2> target_mesh_wrapper(_targetMesh);
      INTERP_KERNEL::Interpolation3DSurf interpolation(*this);
      _nb_cols =  interpolation.interpolateMeshes(source_mesh_wrapper,target_mesh_wrapper,*_matrix,method);
    }
  else
    throw MEDEXCEPTION("no Interpolation exists for the given mesh and space dimensions");

  _nb_rows=(*_matrix).getNbRows();

  _deno_multiply.resize(_nb_rows);
  _matrix->rowSum(_deno_multiply);
  _deno_reverse_multiply.resize(_nb_cols);
  _matrix->colSum(_deno_reverse_multiply, _nb_cols);

  return 1;
}


/*
  remaps a vector source field defined on the source mesh onto the target mesh using the intersection matrix
  \param field_source : source field to be remapped
  \param target_field : field resulting from the remapping on the target mesh
*/
void MEDMEM_REMAPPER::transfer(const MEDMEM::FIELD<double>& field_source, MEDMEM::FIELD<double>& field_target)
{
  int source_nbcomp=field_source.getNumberOfComponents();
  int nb_source_values= field_source.getNumberOfValues();
  const double* value_source = field_source.getValue();

  int target_nbcomp=field_target.getNumberOfComponents();
  double* value_target = const_cast<double*> (field_target.getValue());

  double precision = getPrecision();

  if (source_nbcomp != target_nbcomp)
    throw MEDMEM::MEDEXCEPTION("MEDMEM_REMAPPER::transfer: incoherent number of components for source and target fields");
  if (_nb_cols != nb_source_values)
    throw MEDMEM::MEDEXCEPTION("MEDMEM_REMAPPER::transfer: incoherent number of field values, cannot cannot multiply by interpolation matrix");

  _matrix->multiply(value_source, value_target,source_nbcomp);

  for (int i=0; i< _nb_rows; i++)
    if(fabs(_deno_multiply[i]) > precision)
      for(int comp = 0; comp < source_nbcomp; comp++)
        value_target[i*source_nbcomp+comp]/=_deno_multiply[i];
    else
      for(int comp = 0; comp < source_nbcomp; comp++)
        value_target[i*source_nbcomp+comp]=0.;            
}

/*
  reverses the direct transfer remapping: a Vector field supported on the target mesh is remapped onto 
  the source mesh using the transpose of the intersection matrix
  \param field_target : target field to be remapped
  \param source_field : field resulting from the remapping on the source mesh
*/
void MEDMEM_REMAPPER::reverseTransfer( MEDMEM::FIELD<double>& field_source, const MEDMEM::FIELD<double>& field_target)
{
  int source_nbcomp=field_source.getNumberOfComponents();
  double* value_source = const_cast<double*> (field_source.getValue());

  int target_nbcomp=field_target.getNumberOfComponents();
  int nb_target_values= field_target.getNumberOfValues();
  const double* value_target = field_target.getValue();

  double precision = getPrecision();

  if (source_nbcomp != target_nbcomp)
    throw MEDMEM::MEDEXCEPTION(" MEDMEM_REMAPPER::reverseTransfer: incoherent number of components for source and target fields");
  if (_nb_rows != nb_target_values)
    throw MEDMEM::MEDEXCEPTION(" MEDMEM_REMAPPER::reverseTransfer: incoherent number of field values, cannot cannot multiply by interpolation matrix");

  _matrix->transposeMultiply(value_target, value_source, _nb_cols,target_nbcomp);//transposeMultiply(input,output, nbcols,nbcomp)

  for (int i=0; i< _nb_cols; i++)
    if(fabs(_deno_reverse_multiply[i]) > precision)
      for(int comp = 0; comp < target_nbcomp; comp++)
        value_source[i*target_nbcomp+comp]/=_deno_reverse_multiply[i];
    else
      for(int comp = 0; comp < target_nbcomp; comp++)
        value_source[i*target_nbcomp+comp]=0.;
}

/*
  remaps a vector source field defined on the source mesh onto the target mesh using the intersection matrix
  \param field_source : source field to be remapped
  \return : field resulting from the remapping on the target mesh
*/
MEDMEM::FIELD<double> *  MEDMEM_REMAPPER::transferField(const MEDMEM::FIELD<double>& field_source)
{
  int source_nbcomp=field_source.getNumberOfComponents();
  const double* value_source = field_source.getValue();
  int nb_source_values= field_source.getNumberOfValues();

  double precision = getPrecision();

  if (_nb_cols != nb_source_values)
    throw MEDMEM::MEDEXCEPTION("MEDMEM_REMAPPER::transfer: incoherent number of field values, cannot cannot multiply by interpolation matrix");
        
  MEDMEM::FIELD<double> * target_field = new MEDMEM::FIELD<double>(_targetSupport,source_nbcomp);
  double* value_target = const_cast<double*> ((*target_field).getValue());
                
  _matrix->multiply(value_source, value_target,source_nbcomp);
                
  for (int i=0; i< _nb_rows; i++)
    if(fabs(_deno_multiply[i]) > precision)
      for(int comp = 0; comp < source_nbcomp; comp++)
        value_target[i*source_nbcomp+comp]/=_deno_multiply[i];    
    else
      for(int comp = 0; comp < source_nbcomp; comp++)
        value_target[i*source_nbcomp+comp]=0.;
        
  return target_field;
}

/*
  reverses the direct transfer remapping: a Vector field supported on the target mesh is remapped onto 
  the source mesh using the transpose of the intersection matrix
  \param field_target : target field to be remapped
  \return : field resulting from the remapping on the source mesh
*/
MEDMEM::FIELD<double> *  MEDMEM_REMAPPER::reverseTransferField(const MEDMEM::FIELD<double>& field_target)
{
  int target_nbcomp=field_target.getNumberOfComponents();
  const double* value_target = field_target.getValue();
  int nb_target_values= field_target.getNumberOfValues();

  double precision = getPrecision();

  if (_nb_rows != nb_target_values)
    throw MEDMEM::MEDEXCEPTION(" MEDMEM_REMAPPER::reverseTransfer: incoherent number of field values, cannot cannot transpose-multiply by interpolation matrix");
        
  MEDMEM::FIELD<double> * source_field = new MEDMEM::FIELD<double>(_sourceSupport,target_nbcomp);
  double* value_source = const_cast<double*> ((*source_field).getValue());
        
  _matrix->transposeMultiply(value_target, value_source, _nb_cols,target_nbcomp);//transposeMultiply(input,output, nbcols,nbcomp)
        
  for (int i=0; i< _nb_cols; i++)
    if(fabs(_deno_reverse_multiply[i]) > precision)
      for(int comp = 0; comp < target_nbcomp; comp++)
        value_source[i*target_nbcomp+comp]/=_deno_reverse_multiply[i];
    else
      for(int comp = 0; comp < target_nbcomp; comp++)
        value_source[i*target_nbcomp+comp]=0.;
        
  return source_field;
}

int MEDMEM_REMAPPER::setOptionDouble(const std::string& key, double value)
{
  bool result = INTERP_KERNEL::InterpolationOptions::setOptionDouble(key,value);
        
  if(result)
    return 1;
  else
    return 0;
}

int MEDMEM_REMAPPER::setOptionInt(const std::string& key, int value)
{
  bool result = INTERP_KERNEL::InterpolationOptions::setOptionInt(key,value);
        
  if(result)
    return 1;
  else
    return 0;
}

int MEDMEM_REMAPPER::setOptionString(const std::string& key, std::string& value)
{
  bool result = INTERP_KERNEL::InterpolationOptions::setOptionString(key,value);
        
  if(result)
    return 1;
  else
    return 0;
}

/*!
  \brief returns the volumes of the cells underlying the support \a support

  For 2D geometries, the returned field contains the areas.
  For 3D geometries, the returned field contains the volumes.

  \param support : support whose cell volumes are required
  \return field containing the volumes
*/
MEDMEM::FIELD<double>* MEDMEM_REMAPPER::getSupportVolumes(const MEDMEM::SUPPORT& support)
{
  const MEDMEM::GMESH* mesh=support.getMesh();
  int dim = mesh->getMeshDimension();
  switch (dim)
    {
    case 2:
      return mesh->getArea(&support);
    case 3:
      return mesh->getVolume(&support);
    default:
      throw MEDMEM::MEDEXCEPTION("interpolation is not available for this dimension");
    }
}

void MEDMEM_REMAPPER::printMatrixInfo()
{
        std::cout << *_matrix << std::endl;
}

/*!
@}
*/
