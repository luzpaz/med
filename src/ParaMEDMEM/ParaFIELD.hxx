#ifndef PARAFIELD_HXX_
#define PARAFIELD_HXX_

#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Field.hxx"

namespace MEDMEM{
	class MEDEXCEPTION;
}


namespace ParaMEDMEM
{
class ComponentTopology;
class ParaSUPPORT;

class ParaFIELD
{
public:

	ParaFIELD(const ParaSUPPORT* support, const ComponentTopology& component_topology); 

	ParaFIELD(MEDMEM::driverTypes driver_type, const string& file_name, 
		const string& driver_name, const ComponentTopology& component_topology) 
		throw (MEDMEM::MEDEXCEPTION);
	virtual ~ParaFIELD();
	void write(MEDMEM::driverTypes driverType, const string& fileName="", const string& meshName="");
	void synchronizeTarget(ParaFIELD* source_field);
	void synchronizeSource(ParaFIELD* target_field);
	MEDMEM::FIELD<double>* getField() const {return _field;}
	Topology* getTopology() const {return _topology;}
	int nbComponents() const {return _component_topology.nbComponents();}
private:
	const ComponentTopology& _component_topology;
	Topology* _topology; 
	MEDMEM::FIELD<double>* _field;
	const ParaSUPPORT* _support;
};

}

#endif /*PARAFIELD_HXX_*/
