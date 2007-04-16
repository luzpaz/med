#ifndef INTERPOLATIONMATRIX_HXX_
#define INTERPOLATIONMATRIX_HXX_

#include "MEDMEM_Field.hxx"
#include "MxN_Mapping.hxx"

namespace ParaMEDMEM
{
  class InterpolationMatrix
  {
  public:
    //InterpolationMatrix();
    InterpolationMatrix(const ParaMEDMEM::ParaMESH& source_support, 
			const ProcessorGroup& local_group,
			const ProcessorGroup& distant_group, 
			const string& method);
    
    //InterpolationMatrix(const MEDMEM::MESH& source_support, const string& method);
    //InterpolationMatrix(const MEDMEM::MESH& target_support, const MEDMEM::MESH& source_support);
    virtual ~InterpolationMatrix();
    void addContribution(MEDMEM::MESH& distant_support, int iproc_distant, int* distant_elems);
    void multiply(MEDMEM::FIELD<double>&) const;
    void prepare();
    int getNbRows() const {return _row_offsets.size();}
    
  private:
    //  vector<pair <int,int> > _source_indices;
    vector<int> _row_offsets;
    vector<int> _col_numbers;
    vector<pair<int,int> > _col_offsets;
    vector<double> _coeffs;
    const MEDMEM::MESH& _source_support; 
    MxN_Mapping _mapping;
    string _method;
    const ProcessorGroup& _local_group;
    const ProcessorGroup& _distant_group;
    
    
  };
  
}

#endif /*INTERPOLATIONMATRIX_HXX_*/
