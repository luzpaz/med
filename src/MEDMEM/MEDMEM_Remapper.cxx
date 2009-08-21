//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#include "MEDMEM_Remapper.hxx"
#include "MEDMEM_Exception.hxx"
#include "Interpolation.hxx"
#include "Interpolation2D.txx"
#include "Interpolation3D.txx"
#include "Interpolation3DSurf.txx"

namespace MEDMEM
{
  
  //   int InterpolationOptions::_printLevel=0;
  //   IntersectionType  InterpolationOptions::_intersectionType=Triangulation;
  //   double  InterpolationOptions::_precision=1e-12;;
  //   double  InterpolationOptions::_medianPlane =0.5;
  //   bool  InterpolationOptions::_doRotate =true;
  //   double  InterpolationOptions::_boundingBoxAdjustment =0.1;
  //   int  InterpolationOptions::_orientation =0;
  //   SplittingPolicy  InterpolationOptions::_splittingPolicy =GENERAL_48;

  Remapper::Remapper():_matrix(0)
  {
  }

  Remapper::~Remapper()
  {
    delete _matrix;
  }
  /*! This method computes the intersection matrix between 
   * source \a mesh_source and \a mesh_target. It is a preliminary step 
   * that is necessary before calling the \c transfer() method.
   * The method analyses the dimensions of the meshes and checks for compatibility.
   * 
   */
  bool Remapper::prepare(const MEDMEM::MESH& mesh_source, const MEDMEM::MESH& mesh_target, const char *method)
  {
    const int sm_spacedim = mesh_source.getSpaceDimension();
    const int tm_spacedim = mesh_target.getSpaceDimension();
    int sm_meshdim = mesh_source.getMeshDimension();
    int tm_meshdim = mesh_target.getMeshDimension();

    delete _matrix;
    _matrix= new INTERP_KERNEL::Matrix<double,INTERP_KERNEL::ALL_FORTRAN_MODE>;
		
    if (tm_spacedim!=sm_spacedim || tm_meshdim!=sm_meshdim)
      throw MEDEXCEPTION("incompatible mesh and/or space dimensions in meshes");
    if ((sm_spacedim==2)&&(sm_meshdim==2))
      {
        MEDNormalizedUnstructuredMesh<2,2> source_mesh_wrapper(&mesh_source);
        MEDNormalizedUnstructuredMesh<2,2> target_mesh_wrapper(&mesh_target);
        INTERP_KERNEL::Interpolation2D interpolation(*this);
        _nb_cols = interpolation.interpolateMeshes(source_mesh_wrapper,target_mesh_wrapper,*_matrix,method);
      }
    else if ((sm_spacedim==3)&&(sm_meshdim==3))
      {
        MEDNormalizedUnstructuredMesh<3,3> source_mesh_wrapper(&mesh_source);
        MEDNormalizedUnstructuredMesh<3,3> target_mesh_wrapper(&mesh_target);
        INTERP_KERNEL::Interpolation3D interpolation(*this);
        _nb_cols = interpolation.interpolateMeshes(source_mesh_wrapper,target_mesh_wrapper,*_matrix,method);
      }
    else if ((sm_spacedim==3)&&(sm_meshdim==2))
      {
        MEDNormalizedUnstructuredMesh<3,2> source_mesh_wrapper(&mesh_source);
        MEDNormalizedUnstructuredMesh<3,2> target_mesh_wrapper(&mesh_target);
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
      throw INTERP_KERNEL::Exception("Remapper::prepare: Invalid method specified ! Must be in : \"P0P0\" \"P0P1\" \"P1P0\" or \"P1P1\"");
		
    return true;
  }


  /*
    remaps a vector source field defined on the source mesh onto the target mesh using the intersection matrix
    \param field_source : source field to be remapped
    \param target_field : field resulting from the remapping on the target mesh
  */
  void Remapper::transfer(const MEDMEM::FIELD<double>& field_source, MEDMEM::FIELD<double>& field_target)
  {
    int source_nbcomp=field_source.getNumberOfComponents();
    int nb_source_values= field_source.getNumberOfValues();
    const double* value_source = field_source.getValue();

    int target_nbcomp=field_target.getNumberOfComponents();
    double* value_target = const_cast<double*> (field_target.getValue());

    if (source_nbcomp != target_nbcomp)
      throw MEDMEM::MEDEXCEPTION("Remapper::transfer: incoherent number of components for source and target fields");
    if (_nb_cols != nb_source_values)
      throw MEDMEM::MEDEXCEPTION("Remapper::transfer: incoherent number of field values, cannot cannot multiply by interpolation matrix");

    _matrix->multiply(value_source, value_target,source_nbcomp);

    for (int i=0; i< _nb_rows; i++)
      for(int comp = 0; comp < source_nbcomp; comp++)
        value_target[i*source_nbcomp+comp]/=_deno_multiply[i]; 		
  }

  /*
    reverses the direct transfer remapping: a Vector field supported on the target mesh is remapped onto 
    the source mesh using the transpose of the intersection matrix
    \param field_target : target field to be remapped
    \param source_field : field resulting from the remapping on the source mesh
  */
  void Remapper::reverseTransfer( MEDMEM::FIELD<double>& field_source, const MEDMEM::FIELD<double>& field_target)
  {
    int source_nbcomp=field_source.getNumberOfComponents();
    double* value_source = const_cast<double*> (field_source.getValue());

    int target_nbcomp=field_target.getNumberOfComponents();
    int nb_target_values= field_target.getNumberOfValues();
    const double* value_target = field_target.getValue();

    if (source_nbcomp != target_nbcomp)
      throw MEDMEM::MEDEXCEPTION(" Remapper::reverseTransfer: incoherent number of components for source and target fields");
    if (_nb_rows != nb_target_values)
      throw MEDMEM::MEDEXCEPTION(" Remapper::reverseTransfer: incoherent number of field values, cannot cannot multiply by interpolation matrix");

    _matrix->transposeMultiply(value_target, value_source, _nb_cols,target_nbcomp);//transposeMultiply(input,output, nbcols,nbcomp)

    for (int i=0; i< _nb_cols; i++)
      for(int comp = 0; comp < source_nbcomp; comp++)
        value_source[i*target_nbcomp+comp]/=_deno_reverse_multiply[i];
  }

  /*
    remaps a vector source field defined on the source mesh onto the target mesh using the intersection matrix
    \param field_source : source field to be remapped
    \return : field resulting from the remapping on the target mesh
  */
  MEDMEM::FIELD<double> *  Remapper::transferField(const MEDMEM::FIELD<double>& field_source, MEDMEM::MESH& target_mesh)
  {
    int source_nbcomp=field_source.getNumberOfComponents();
    const double* value_source = field_source.getValue();
    int nb_source_values= field_source.getNumberOfValues();

    if (_nb_cols != nb_source_values)
      throw MEDMEM::MEDEXCEPTION("Remapper::transfer: incoherent number of field values, cannot cannot multiply by interpolation matrix");

    MEDMEM::SUPPORT target_support;
    if(	_targetFieldType == "P0")
      target_support = MEDMEM::SUPPORT(&target_mesh,"on All support",MED_EN::MED_CELL);
    else
      target_support = MEDMEM::SUPPORT(&target_mesh,"on All support",MED_EN::MED_NODE);
		
    MEDMEM::FIELD<double> * target_field = new MEDMEM::FIELD<double>(&target_support,source_nbcomp);
    double* value_target = const_cast<double*> ((*target_field).getValue());
		
    _matrix->multiply(value_source, value_target,source_nbcomp);
		
    for (int i=0; i< _nb_rows; i++)
      for(int comp = 0; comp < source_nbcomp; comp++)
        value_target[i*source_nbcomp+comp]/=_deno_multiply[i]; 	
		
    return target_field;
  }

  /*
    reverses the direct transfer remapping: a Vector field supported on the target mesh is remapped onto 
    the source mesh using the transpose of the intersection matrix
    \param field_target : target field to be remapped
    \return : field resulting from the remapping on the source mesh
  */
  MEDMEM::FIELD<double> *  Remapper::reverseTransferField(const MEDMEM::FIELD<double>& field_target, MEDMEM::MESH& source_mesh)
  {
    int target_nbcomp=field_target.getNumberOfComponents();
    const double* value_target = field_target.getValue();
    int nb_target_values= field_target.getNumberOfValues();

    if (_nb_rows != nb_target_values)
      throw MEDMEM::MEDEXCEPTION(" Remapper::reverseTransfer: incoherent number of field values, cannot cannot transpose-multiply by interpolation matrix");

    MEDMEM::SUPPORT source_support;
    if(	_sourceFieldType == "P0")
      source_support = MEDMEM::SUPPORT(&source_mesh,"on All support",MED_EN::MED_CELL);
    else
      source_support = MEDMEM::SUPPORT(&source_mesh,"on All support",MED_EN::MED_NODE);
		
    MEDMEM::FIELD<double> * source_field = new MEDMEM::FIELD<double>(&source_support,target_nbcomp);
    double* value_source = const_cast<double*> ((*source_field).getValue());
		
    _matrix->transposeMultiply(value_target, value_source, _nb_cols,target_nbcomp);//transposeMultiply(input,output, nbcols,nbcomp)

    for (int i=0; i< _nb_cols; i++)
      for(int comp = 0; comp < target_nbcomp; comp++)
        value_source[i*target_nbcomp+comp]/=_deno_reverse_multiply[i];
		
    return source_field;
  }

  bool Remapper::setOptionDouble(const std::string& key, double value)
  {
    if(key == "Precision") 
      {
        setPrecision(value);
        return true;
      }
    else if(key == "MedianPlane") 
      {
        setMedianPlane(value);
        return true;
      }
    else if(key == "BoundingBoxAdjustment") 
      {
        setBoundingBoxAdjustment(value);
        return true;
      }
    else if(key == "BoundingBoxAdjustmentAbs") 
      {
        setBoundingBoxAdjustmentAbs(value);
        return true;
      }
    else if(key == "MaxDistance3DSurfIntersect") 
      {
        setMaxDistance3DSurfIntersect(value);
        return true;
      }
    else 
      return true;
  }
	
  bool Remapper::setOptionInt(const std::string& key, int value)
  {
    if(key == "PrintLevel") 
      {
        setPrintLevel(value);
        return true;
      }
    else if(key == "DoRotate") 
      {
        setDoRotate(value);
        return true;
      }
    else if(key == "Orientation") 
      {
        setOrientation(value);
        return true;
      }
    else
      return false;
  }

  bool Remapper::setOptionString(const std::string& key, std::string& value)
  {
    if(key == "IntersectionType") 
      {
        if(value == "Triangulation")
          {
            setIntersectionType(INTERP_KERNEL::Triangulation);
            return true;
          }
        else if(value == "Convex")
          {
            setIntersectionType(INTERP_KERNEL::Convex);
            return true;
          }
        else if(value == "Geometric2D")
          {
            setIntersectionType(INTERP_KERNEL::Geometric2D);
            return true;
          }
      }
    else if(key == "SplittingPolicy") 
      {
        if(value == "PLANAR_FACE_5")
          {
            setSplittingPolicy(INTERP_KERNEL::PLANAR_FACE_5);
            return true;
          }
        else if(value == "PLANAR_FACE_6")
          {
            setSplittingPolicy(INTERP_KERNEL::PLANAR_FACE_6);
            return true;
          }
        else if(value == "GENERAL_24")
          {
            setSplittingPolicy(INTERP_KERNEL::GENERAL_24);
            return true;
          }
        else if(value == "GENERAL_48")
          {
            setSplittingPolicy(INTERP_KERNEL::GENERAL_48);
            return true;
          }
        else
          return false;
      }
  }

  /*!
    \brief returns the volumes of the cells underlying the support \a support

    For 2D geometries, the returned field contains the areas.
    For 3D geometries, the returned field contains the volumes.

    \param support : support whose cell volumes are required
    \return field containing the volumes
  */
  MEDMEM::FIELD<double>* Remapper::getSupportVolumes(const MEDMEM::SUPPORT& support)
  {
    const MEDMEM::MESH* mesh=support.getMesh();
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


}
