#include "MEDMEM_Exception.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaSUPPORT.hxx"
#include "StructuredParaSUPPORT.hxx"
#include "UnstructuredParaSUPPORT.hxx"
#include "ExplicitCoincidentDEC.hxx"
#include "StructuredCoincidentDEC.hxx"
#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"
#include "ParaFIELD.hxx"
#include "ParaMESH.hxx"

using namespace MEDMEM;

namespace ParaMEDMEM
{

ParaFIELD::ParaFIELD(const ParaSUPPORT* para_support, const ComponentTopology& component_topology)
:_support(para_support),
 _component_topology(component_topology),
 _field(0)
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

	int nb_components = component_topology.nbLocalComponents();
	if (nb_components!=0)
		{
			_field=new FIELD<double> (para_support->getSupport(), nb_components);
		}
	else return;
	
	_field->setName("Default ParaFIELD name");
	_field->setDescription("Default ParaFIELD description");
	_field->setNumberOfValues(para_support->getSupport()->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
	string* compnames=new string[nb_components];
	string* compdesc=new string[nb_components];
		string* compunit=new string[nb_components];
	for (int i=0; i<nb_components; i++)
	{
		ostringstream stream(compnames[i]);
		ostringstream stream2(compdesc[i]);
		ostringstream stream3(compunit[i]);
		stream<<"component "<<i;
		stream2<<"component description "<<i;
				stream3<<"component unit "<<i;

		compnames[i]=stream.str();
		compdesc[i]=stream2.str();
				compunit[i]=stream3.str();

	}
	_field->setComponentsNames(compnames);
	_field->setComponentsDescriptions(compdesc);
	_field->setMEDComponentsUnits(compunit);
	_field->setIterationNumber(0);
	_field->setOrderNumber(0);
	_field->setTime(0.0);
	delete[] compnames;
	delete[] compdesc;
} 

/*! Constructor creating the ParaFIELD
 * from a given FIELD
 */
ParaFIELD::ParaFIELD(MEDMEM::FIELD<double>* subdomain_field, const ProcessorGroup& proc_group):
_field(subdomain_field),
_support(new UnstructuredParaSUPPORT(subdomain_field->getSupport(), proc_group)),
_component_topology(ComponentTopology(_field->getNumberOfComponents()))
{
  const ExplicitTopology* source_topo=
    dynamic_cast<const ExplicitTopology*> (_support->getTopology());
    _topology=new ExplicitTopology(*source_topo,_component_topology.nbLocalComponents());
}

ParaFIELD::ParaFIELD(MEDMEM::driverTypes driver_type, const string& file_name, 
	const string& driver_name, const ComponentTopology& component_topology) 
	throw (MEDEXCEPTION):_component_topology(component_topology),_field(0){}

ParaFIELD::~ParaFIELD()
{
  if (_field!=0)
    delete _field;
  if (_topology!=0)
    delete _topology;
}

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
	data_channel->attachLocalField(this);
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
	data_channel->attachLocalField(this);
	data_channel->synchronize();
	data_channel->prepareSourceDE();
	data_channel->sendData();
	
	delete data_channel;
}

double ParaFIELD::getVolumeIntegral(int icomp) const
{
	CommInterface comm_interface = _topology->getProcGroup()->getCommInterface();
	const MEDMEM::SUPPORT* support = _field->getSupport();
	FIELD<double>* volume= getSupportVolumes(*support);
	double integral=0;
	for (int i=0; i<support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS); i++)
		integral+=_field->getValueIJ(i+1,icomp)*volume->getValueIJ(i+1,1);
	
	double total=0.0;
	comm_interface.allReduce(&integral, &total, 1, MPI_DOUBLE, MPI_SUM, * dynamic_cast<const MPIProcessorGroup*>(_topology->getProcGroup())->getComm());
	return total;
}

/*!
\brief returns the volumes of the cells underlying the field \a field

For 2D geometries, the returned field contains the areas.
For 3D geometries, the returned field contains the volumes.

\param field field on which cells the volumes are required
\return field containing the volumes
*/
 MEDMEM::FIELD<double>* ParaFIELD::getSupportVolumes(const MEDMEM::SUPPORT& support) const
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
