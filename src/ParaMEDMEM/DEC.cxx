#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "DEC.hxx"
#include "./ICoCo/ICoCoField.hxx"
#include "./ICoCo/ICoCoMEDField.hxx"
#include "./ICoCo/ICoCoTrioField.hxx"


/*! \defgroup dec DEC
 * Data Exchange Channel 
 *
 * Interface class for creation of a link between two 
 * processor groups for exhanging mesh or field data.
 * The DEC is defined by attaching a field on the receiving or on the 
 * sending side. 
 * On top of attaching a ParaMEDMEM::FIELD, it is possible to
 * attach a ICoCo::Field. This class is an abstract class that enables 
 * coupling of codes that respect the ICoCo interface \ref icoco. It has two implementations:
 * one for codes that express their fields as MEDMEM fields (ICoCo::MEDField) and one
 * for codes that express their fields as Trio/U fields.
*/

namespace ParaMEDMEM
{

	/*! \addtogroup dec
		@{ 
	*/
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
	/*! Attaches a local field to a DEC.
If the processor is on the receiving end of the DEC, the field
will be updated by a recvData() call.
Reversely, if the processor is on the sending end, the field will be read, possibly transformed, and sent appropriately to the other side.
	*/
	void DEC::attachLocalField(const ParaFIELD* field) 
	{
		_local_field=field;
		_comm_interface=&(field->getTopology()->getProcGroup()->getCommInterface());
	}
	/*! 
		Attaches a local field to a DEC.
		If the processor is on the receiving end of the DEC, the field
		will be updated by a recvData() call.
		Reversely, if the processor is on the sending end, the field will be read, possibly transformed, and sent appropriately to the other side.
		The field type is a generic ICoCo Field, so that the DEC can couple a number of different fields :
		- a ICoCo::MEDField, that is created from a MEDMEM structure
		- a ICOCo::TrioField, that is created from tables extracted from a TRIO-U structure.
		
	*/
	
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
	/*! If the renormalization flag is set, after a call to \a recvData(), 
the field is renormalized so that the domain integral of the field is the same on the 
sending side and on the receiving side.
	*/
	void DEC::setForcedRenormalizationFlag(bool flag)
	{
		_forced_renormalization_flag=flag;
	}
	bool DEC::getForcedRenormalizationFlag()
	{
		return _forced_renormalization_flag;
	}
	/*! @} */
}
