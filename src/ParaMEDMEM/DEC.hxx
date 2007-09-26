#ifndef DEC_HXX_
#define DEC_HXX_
namespace ICoCo
{
	class Field;
}
namespace ParaMEDMEM
{
class ProcessorGroup;
class ParaFIELD;
class CommInterface;
class DEC
{
public:
	DEC():_local_field(0){}
  DEC(ProcessorGroup& local_group, ProcessorGroup& distant_group); 
	void attachLocalField(const ParaFIELD* field);
  void attachLocalField(const ICoCo::Field* field);
	virtual void prepareSourceDE()=0;
	virtual void prepareTargetDE()=0;
	virtual void recvData()=0;
	virtual void sendData()=0;
	virtual void synchronize()=0;
  virtual ~DEC();
	virtual void computeProcGroup(){};
	void setForcedRenormalizationFlag(bool flag);
	bool getForcedRenormalizationFlag();
protected:
	const ParaFIELD* _local_field;
	//! Processor group representing the union of target and source processors
	ProcessorGroup* _union_group;
  ProcessorGroup* _source_group;
  ProcessorGroup* _target_group;
  
	const CommInterface* _comm_interface;
	bool _forced_renormalization_flag;
	bool _owns_field;
};

}

#endif /*DEC_HXX_*/
