
/*! Data Exchange Channel 
 * Interface class for creation of a link between two 
 * MPI groups for exhanging mesh or field data*/

#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "DEC.hxx"
#include "./ICoCo/ICoCoField.hxx"
#include "./ICoCo/ICoCoMEDField.hxx"
#include "./ICoCo/ICoCoTrioField.hxx"

namespace ParaMEDMEM
{
  
  DEC::DEC(ProcessorGroup& source_group, ProcessorGroup& target_group):_local_field(0), 
																																			 _source_group(&source_group), _target_group(&target_group), _owns_field(false)
  {
    _union_group = source_group.fuse(target_group);  
  }

  DEC::~DEC()
  {
    delete _union_group;
		if (_owns_field)
			delete _local_field;
  }  

	void DEC::attachLocalField(const ParaFIELD* field) 
	{
		_local_field=field;
		_comm_interface=&(field->getTopology()->getProcGroup()->getCommInterface());
	}
	
	void DEC::attachLocalField(const ICoCo::Field* field)
	{
		const ICoCo::MEDField* medfield =dynamic_cast<const ICoCo::MEDField*> (field);
		if(medfield !=0)
			{
				attachLocalField(medfield->getField());
				return;
			}
		const ICoCo::TrioField* triofield=dynamic_cast<const ICoCo::TrioField*> (field);
		if (triofield !=0)
			{
				ProcessorGroup* localgroup;
				if (_source_group->containsMyRank()) localgroup=_source_group;
				else localgroup=_target_group;

				ICoCo::Field* parafield=new ICoCo::MEDField(*triofield, *localgroup);
				_owns_field = true;
				attachLocalField(parafield);
			}
		throw MEDMEM::MEDEXCEPTION("incompatible field type");
	}
	void DEC::setForcedRenormalizationFlag(bool flag)
	{
		_forced_renormalization_flag=flag;
	}
	bool DEC::getForcedRenormalizationFlag()
	{
		return _forced_renormalization_flag;
	}

}
