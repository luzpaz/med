#ifndef MXN_MAPPING_HXX_
#define MXN_MAPPING_HXX_

#include <vector>

#include "MEDMEM_Field.hxx"


namespace ParaMEDMEM
{
class ProcessorGroup;

class MxN_Mapping
{
public:
	MxN_Mapping();
  MxN_Mapping(const ProcessorGroup& local_group, const ProcessorGroup& distant_group);
	virtual ~MxN_Mapping();
  void addElementFromSource(int distant_proc, int distant_elem);
  void prepareSendRecv();
  void sendRecv(MEDMEM::FIELD<double>& field);
  void sendRecv(double* field, MEDMEM::FIELD<double>& field) const ;
  
private :
//  ProcessorGroup& _local_group;
//  ProcessorGroup& _distant_group;
  ProcessorGroup* _union_group;
  int _nb_comps;
  std::vector<pair<int,int> > _sending_ids;
  std::vector<int> _recv_ids;
  std::vector<int> _send_proc_offsets;
  std::vector<int> _recv_proc_offsets;
  
};

}

#endif /*MXN_MAPPING_HXX_*/
