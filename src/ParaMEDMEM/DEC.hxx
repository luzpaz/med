#ifndef DEC_HXX_
#define DEC_HXX_

namespace ParaMEDMEM
{
class ProcessorGroup;
class ParaFIELD;
class CommInterface;
class DEC
{
public:
	DEC():_source_field(0),_target_field(0){}
  DEC(ProcessorGroup& local_group, ProcessorGroup& distant_group); 
	void attachTargetField(const ParaFIELD* field);
	void attachSourceField(const ParaFIELD* field) ;
	virtual void prepareSourceDE()=0;
	virtual void prepareTargetDE()=0;
	virtual void recvData()=0;
	virtual void sendData()=0;
	virtual void synchronize()=0;
  virtual ~DEC();
	virtual void computeProcGroup(){};
protected:
	const ParaFIELD* _source_field;
	const ParaFIELD* _target_field;
	//! Processor group representing the union of target and source processors
	ProcessorGroup* _union_group;
  ProcessorGroup* _source_group;
  ProcessorGroup* _target_group;
  
	const CommInterface* _comm_interface;
};

}

#endif /*DEC_HXX_*/
