#include "ParaMESH.hxx"
#include "ProcessorGroup.hxx"
#include "MxN_Mapping.hxx"
#include "InterpolationMatrix.hxx"
#include "INTERPOLATION_2D.hxx"

/*! \class InterpolationMatrix
This class enables the storage of an interpolation matrix Wij mapping 
source field Sj to target field Ti via Ti=Wij.Sj.
The matrix is built and stored on the processors belonging to the source
group. 
*/

namespace ParaMEDMEM
{

  /*!
    Creates an empty matrix structure linking two distributed supports.
    The method must be called by all processors belonging to source and target groups.
    \param source_support local support
    \param local_group processor group containing the local processors
    \param distant_group processor group containing the distant processors
    \param method interpolation method
   */
InterpolationMatrix::InterpolationMatrix(
const ParaMEDMEM::ParaMESH& source_support, 
const ProcessorGroup& local_group,
const ProcessorGroup& distant_group, 
const string& method):
_source_support(*source_support.getMesh()),
_local_group(local_group),
_distant_group(distant_group),
_mapping(local_group, distant_group),
_method(method)
{
  int nbelems=  _source_support.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  _row_offsets.resize(nbelems+1,0);
  
  
}

InterpolationMatrix::~InterpolationMatrix()
{
}

/*!
adds the contribution of a distant subdomain to the interpolation matrix
\param distant_support local representation of the distant subdomain
\param iproc_distant id of the distant subdomain (in the distant group)
\param distant_elems mapping between the local representation of the subdomain and the actual elem ids on the distant subdomain
 */
void InterpolationMatrix::addContribution(MEDMEM::MESH& distant_support, int iproc_distant, int* distant_elems)
{
  INTERPOLATION_2D interpolator;
  int nbelems=  _source_support.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  //_row_offsets.resize(nbelems);
  vector<map<int,double> > surfaces = interpolator.interpol_maillages(distant_support,_source_support);
  if (surfaces.size() != nbelems)
    throw MEDEXCEPTION("uncoherent number of rows in interpolation matrix");

  MEDMEM::SUPPORT support(&distant_support,"all cells", MED_EN::MED_CELL);
  MEDMEM::FIELD<double>* triangle_surf = distant_support.getArea(&support);

  for (int ielem=0; ielem < surfaces.size(); ielem++) 
  {
    _row_offsets[ielem+1]+=surfaces[ielem].size();
    //    _source_indices.push_back(make_pair(iproc_distant, ielem));
    for (map<int,double>::const_iterator iter=surfaces[ielem].begin();
         iter!=surfaces[ielem].end();
         iter++)
         {
	   double surf = triangle_surf->getValueIJ(iter->first,1);
	   surf = (surf>0)?surf:-surf;
           vector<pair<int,int> >::iterator iter2 = find (_col_offsets.begin(), _col_offsets.end(),make_pair(iproc_distant,iter->first));
           int col_id;
           if (iter2==_col_offsets.end())
           {
             _col_offsets.push_back(make_pair(iproc_distant,iter->first));
             col_id =_col_offsets.size();
	     _mapping.addElementFromSource(col_id,iproc_distant,distant_elems[iter->first-1]);
           } 
           else
           {
            col_id=iter2-_col_offsets.begin()+1;
           }
           _col_numbers.push_back(col_id);
           _coeffs.push_back(iter->second/surf);
         
         }
  }
  delete triangle_surf;
}
    
//  intersect (_local_support,_distant_support);
//  const int* conn_source_index = _local_support->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_ALL_ELEMENTS);
//  const int* conn_source = distant_support->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_ALL_ELEMENTS);
//  const double* coords_source = _local_support->getCoords(MED_EN::MED_FULL_INTERLACE);
//  for (int ielem=0; ielem< _local_support->getNumberOfElements(MED_EN::MED_CELL); ielem++)
//    {
//      MED_EN::medGeometryElementType type = distant_support->getElementType(ielem);
//      switch (type)
//      {
//
//        case MED_EN::TRIA3 :
//          double* coords=new int[3*dim];
//          for (ielem_distant=0; ielem_distant<distant_support->getNumberOfElements(MED_EN::MED_CELL);ielem_distant++)
//          {
//            int ielem_target=*iter;
//                    
//            for (int i=0; i<3; i++)
//              {
//                int node = conn[conn_source_index[ielem]+i];
//                for  (int idim=0; i<dim; idim++)
//                   coords[idim+dim*i]=coords_source[node*dim+idim];
//              }
//            double* moments = new double[dim+1];
//            intersection (target_support, ielem_target, dim, coords,intersection_moments); 
//          }
//          
//          //intersection_moments :
//          // 0 -> int(1.dV)
//          // 1 -> int(x.dV)
//          // 2 -> int(y.dV)
//          // 3 -> int(z.dV)
//          if (intersection_moments[0]>0)
//          {
//           _source_indices.push_back(make_pair(iproc_distant, ielem);
//           _col_numbers.push_back(ielem_target);
//           _row_offsets[irow]++;
//           _coeffs.push_back(intersection_moments[0]);
//           for (int i=0; i<dim; i++)
//              _coeffs.push_back(intersection_moments[i+1]);
//           _mapping.addElementFromSource(ielem,iproc_distant,ielem_target);
//           }        
//      }
//      irow++;
//    }  
//}

void InterpolationMatrix::prepare()
{
  int nbelems = _source_support.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  for (int ielem=0; ielem < nbelems; ielem++)
  {
    _row_offsets[ielem+1]+=_row_offsets[ielem];
  }  
  _mapping.prepareSendRecv();
}

void InterpolationMatrix::multiply(MEDMEM::FIELD<double>& field) const
{
  double* target_value = new double[_col_offsets.size()* field.getNumberOfComponents()];
  for (int i=0; i< _col_offsets.size()* field.getNumberOfComponents();i++)
    {
      target_value[i]=0.0;
    }
  // int nbrows = _source_indices.size();
  int nbrows=  _source_support.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  for (int irow=0; irow<nbrows; irow++)
  {
    for (int icol=_row_offsets[irow]; icol< _row_offsets[irow+1];icol++)
    {
      for (int icomp=0; icomp<field.getNumberOfComponents(); icomp++)
	{
	  double coeff_row = field.getValueIJ(irow+1,icomp+1);
	  target_value[_col_numbers[icol]-1]+=_coeffs[icol]*coeff_row;
	}
    } 
  }
  _mapping.sendRecv(target_value,field);
  
  if (_col_offsets.size()>0)
       delete[] target_value;
}
  

}
