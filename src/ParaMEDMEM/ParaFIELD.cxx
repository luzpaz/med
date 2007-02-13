#include "MEDMEM_Exception.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaSUPPORT.hxx"
#include "StructuredParaSUPPORT.hxx"
#include "ExplicitCoincidentDEC.hxx"
#include "StructuredCoincidentDEC.hxx"
#include "ParaFIELD.hxx"
#include "ParaMESH.hxx"

using namespace MEDMEM;

namespace ParaMEDMEM
{

ParaFIELD::ParaFIELD(const ParaSUPPORT* para_support, const ComponentTopology& component_topology)
:_support(para_support),
_component_topology(component_topology) 
{
	if (dynamic_cast<const StructuredParaSUPPORT*>(para_support)!=0)
	{const BlockTopology* source_topo = dynamic_cast<const BlockTopology*>(para_support->getTopology());
		_topology=new BlockTopology(*source_topo,component_topology);
	}
	else
	{
	  if (component_topology.nbBlocks()!=1)
	    throw MEDEXCEPTION(LOCALIZED(
		"ParaFIELD constructor : Unstructured Support not taken into account with component topology yet"));
	  else 
	    {
	      const ExplicitTopology* source_topo=
		dynamic_cast<const ExplicitTopology*> (para_support->getTopology());
	      _topology=new ExplicitTopology(*source_topo,component_topology.nbLocalComponents());
					     
	    }
	}
//	int nb_components=0;
//	if (component_topology.getProcGroup()!=0)
	int nb_components = component_topology.nbLocalComponents();
	if (nb_components!=0)
		{
			_field=new FIELD<double> (para_support->getSupport(), nb_components);
		}
	else return;
	
	_field->setName("toto");
	_field->setDescription("titi");
	_field->setNumberOfValues(para_support->getSupport()->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
	string* compnames=new string[nb_components];
	string* compdesc=new string[nb_components];
	for (int i=0; i<nb_components; i++)
	{
		ostringstream stream(compnames[i]);
		ostringstream stream2(compdesc[i]);
		stream<<"component "<<i;
		stream2<<"component description "<<i;
	}
	_field->setComponentsNames(compnames);
	_field->setComponentsDescriptions(compdesc);
	_field->setIterationNumber(0);
	_field->setOrderNumber(0);
	_field->setTime(0.0);
} 

ParaFIELD::ParaFIELD(MEDMEM::driverTypes driver_type, const string& file_name, 
	const string& driver_name, const ComponentTopology& component_topology) 
	throw (MEDEXCEPTION):_component_topology(component_topology){}
ParaFIELD::~ParaFIELD(){}

void ParaFIELD::write(MEDMEM::driverTypes driverType, const string& fileName, const string& meshName){
  //	Topology* topo = dynamic_cast<BlockTopology*> (_topology);
	int myrank = _topology->getProcGroup()->myRank();
	ostringstream name;
	name <<fileName<<myrank+1<<".med";
	cout << name <<endl;
	int driver = _field->addDriver(driverType, name.str().c_str(), meshName);
	_field->write(driver);
}

void ParaFIELD::synchronizeTarget(ParaFIELD* source_field){
	DEC* data_channel;
	if (dynamic_cast<BlockTopology*>(_topology)!=0)
	{
		data_channel=new StructuredCoincidentDEC();
	}
	else
	{
		data_channel=new ExplicitCoincidentDEC();
	}
	data_channel->attachTargetField(this);
	data_channel->synchronize();
	data_channel->prepareTargetDE();
	data_channel->recvData();
	
	delete data_channel;
}

void ParaFIELD::synchronizeSource(ParaFIELD* target_field){
	DEC* data_channel;
	if (dynamic_cast<BlockTopology*>(_topology)!=0)
	{
		data_channel=new StructuredCoincidentDEC();
	}
	else
	{
		data_channel=new ExplicitCoincidentDEC();
	}
	data_channel->attachSourceField(this);
	data_channel->synchronize();
	data_channel->prepareSourceDE();
	data_channel->sendData();
	
	delete data_channel;
}

}
