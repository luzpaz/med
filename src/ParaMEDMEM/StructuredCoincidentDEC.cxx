#include <mpi.h>
#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "MPIProcessorGroup.hxx"
#include "StructuredCoincidentDEC.hxx"

namespace ParaMEDMEM
{

StructuredCoincidentDEC::StructuredCoincidentDEC():_toposource(0),_topotarget(0)
{	
}


StructuredCoincidentDEC::~StructuredCoincidentDEC()
{
}


/*! Synchronization process for exchanging topologies
 */
void StructuredCoincidentDEC::synchronize()
{
	if (_source_field!=0)
		_toposource = dynamic_cast<BlockTopology*>(_source_field->getTopology());
	if (_target_field!=0)
		_topotarget = dynamic_cast<BlockTopology*>(_target_field->getTopology());
	
	// Transmitting source topology to target code 
	broadcastTopology(_toposource,1000);
	// Transmitting target topology to source code
	broadcastTopology(_topotarget,2000);
	//checkCompatibility(_toposource,_topotarget);
}

/*! Creates the arrays necessary for the data transfer
 * and fills the send array with the values of the 
 * source field
 *  */
void StructuredCoincidentDEC::prepareSourceDE()
{
	////////////////////////////////////
	//Step 1 : buffer array creation 
	
	if (!_toposource->getProcGroup()->containsMyRank())
		return;
	MPIProcessorGroup* group=new MPIProcessorGroup(_toposource->getProcGroup()->getCommInterface());
	
	int myranksource = _toposource->getProcGroup()->myRank();
	
	vector <int>* target_arrays=new vector<int>[_topotarget->getProcGroup()->size()];
	
	//cout<<" topotarget size"<<	_topotarget->getProcGroup()->size()<<endl;
	
	int nb_local = _toposource-> getNbLocalElements();
	for (int ielem=0; ielem< nb_local ; ielem++)
	{
	//	cout <<"source local :"<<myranksource<<","<<ielem<<endl; 
		int global = _toposource->localToGlobal(make_pair(myranksource, ielem));
	//	cout << "global "<<global<<endl;
		pair<int,int> target_local =_topotarget->globalToLocal(global);
	//	cout << "target local : "<<target_local.first<<","<<target_local.second<<endl; 
		target_arrays[target_local.first].push_back(target_local.second); 
	}	
	
	int union_size=group->size();
	
	_sendcounts=new int[union_size];
   	_senddispls=new int[union_size];
   	_recvcounts=new int[union_size];
   	_recvdispls=new int[union_size];
   	
	for (int i=0; i< union_size; i++)
	{
		_sendcounts[i]=0;
		_recvcounts[i]=0;
		_recvdispls[i]=0;
	}
	_senddispls[0]=0;
	
	for (int iproc=0; iproc < _topotarget->getProcGroup()->size(); iproc++)
	{
		//converts the rank in target to the rank in union communicator
		int unionrank=group->translateRank(_topotarget->getProcGroup(),iproc);
		_sendcounts[unionrank]=target_arrays[iproc].size();
	}
	
	for (int iproc=1; iproc<group->size();iproc++)
		_senddispls[iproc]=_senddispls[iproc-1]+_sendcounts[iproc-1];
	
	_sendbuffer = new double [nb_local ];

	/////////////////////////////////////////////////////////////
	//Step 2 : filling the buffers with the source field values 

	int* counter=new int [_topotarget->getProcGroup()->size()];
	counter[0]=0;	
	for (int i=1; i<_topotarget->getProcGroup()->size(); i++)
		counter[i]=counter[i-1]+target_arrays[i-1].size();
		
			
	const double* value = _source_field->getField()->getValue();
	//cout << "Nb local " << nb_local<<endl;
	for (int ielem=0; ielem<nb_local ; ielem++)
	{
		int global = _toposource->localToGlobal(make_pair(myranksource, ielem));
		pair<int,int> target_local =_topotarget->globalToLocal(global);
		//cout <<"global : "<< global<<" local :"<<target_local.first<<" "<<target_local.second;
		//cout <<"counter[]"<<counter[target_local.first]<<endl;
		_sendbuffer[counter[target_local.first]++]=value[ielem];
		
	}
	delete[] target_arrays;
	delete[] counter;
}

/*!
 *  Creates the buffers for receiving the fields on the target side
 */
void StructuredCoincidentDEC::prepareTargetDE()
{
	if (!_topotarget->getProcGroup()->containsMyRank())
		return;
	MPIProcessorGroup* group=new MPIProcessorGroup(_toposource->getProcGroup()->getCommInterface());
	
	int myranktarget = _topotarget->getProcGroup()->myRank();
	
	vector < vector <int> > source_arrays(_toposource->getProcGroup()->size());
	int nb_local = _topotarget-> getNbLocalElements();
	for (int ielem=0; ielem< nb_local ; ielem++)
	{
	//	cout <<"TS target local :"<<myranktarget<<","<<ielem<<endl; 
		int global = _topotarget->localToGlobal(make_pair(myranktarget, ielem));
		//cout << "TS global "<<global<<endl;
		pair<int,int> source_local =_toposource->globalToLocal(global);
	//	cout << "TS source local : "<<source_local.first<<","<<source_local.second<<endl; 
		source_arrays[source_local.first].push_back(source_local.second); 
	}	
	int union_size=group->size();
	_recvcounts=new int[union_size];
    _recvdispls=new int[union_size];
    _sendcounts=new int[union_size];
    _senddispls=new int[union_size];
    
	for (int i=0; i< union_size; i++)
		{
			_sendcounts[i]=0;
			_recvcounts[i]=0;
			_recvdispls[i]=0;
		}
	for (int iproc=0; iproc < _toposource->getProcGroup()->size(); iproc++)
	{
		//converts the rank in target to the rank in union communicator
		int unionrank=group->translateRank(_toposource->getProcGroup(),iproc);
		_recvcounts[unionrank]=source_arrays[iproc].size();
	}
	for (int i=1; i<union_size; i++)
		_recvdispls[i]=_recvdispls[i-1]+_recvcounts[i-1];
	_recvbuffer=new double[nb_local];
		
}

 
/*!
 * Synchronizing a topology so that all the 
 * group possesses it.
 * 
 * \param topo Topology that is transmitted. It is read on processes where it already exists, and it is created and filled on others.
 * \param tag Communication tag associated with this operation.
 */
void StructuredCoincidentDEC::broadcastTopology(BlockTopology*& topo, int tag)
{
	MPI_Status status;
	
	int* serializer=0;
	int size;
	
	MPIProcessorGroup* group=new MPIProcessorGroup(*_comm_interface);
	
	// The master proc creates a send buffer containing
	// a serialized topology
	int rank_master;
	
	if (topo!=0 && topo->getProcGroup()->myRank()==0)
	{
		MESSAGE ("Master rank");
		topo->serialize(serializer, size);
		rank_master = group->translateRank(topo->getProcGroup(),0);
		MESSAGE("Master rank world number is "<<rank_master);
		MESSAGE("World Size is "<<group->size());
		for (int i=0; i< group->size(); i++)
		{
			if (i!= rank_master)
				_comm_interface->send(&rank_master,1,MPI_INTEGER, i,tag+i,*(group->getComm()));
		}
	}
	else
	{
		MESSAGE(" rank "<<group->myRank()<< " waiting ...");
		_comm_interface->recv(&rank_master, 1,MPI_INTEGER, MPI_ANY_SOURCE, tag+group->myRank(), *(group->getComm()),&status);
		MESSAGE(" rank "<<group->myRank()<< "received master rank"<<rank_master);
	}
	// The topology is broadcasted to all processsors in the group
	_comm_interface->broadcast(&size, 1,MPI_INTEGER,rank_master,*(group->getComm()));
	
	int* buffer=new int[size];
	if (topo!=0 && topo->getProcGroup()->myRank()==0)
		copy(serializer, serializer+size, buffer); 
	_comm_interface->broadcast(buffer,size,MPI_INTEGER,rank_master,*(group->getComm()));
	
	// Processors which did not possess the source topology 
	// unserialize it
	
	BlockTopology* topotemp=new BlockTopology();
	topotemp->unserialize(buffer, *_comm_interface);
	
	if (topo==0) 
		topo=topotemp;
	else 
		delete topotemp;
	
	// Memory cleaning
	delete[] buffer;
	if (serializer!=0)
		delete[] serializer;
	MESSAGE (" rank "<<group->myRank()<< " unserialize is over");
}



void StructuredCoincidentDEC::recvData()
{
	//MPI_COMM_WORLD is used instead of group because there is no
	//mechanism for creating the union group yet
	MESSAGE("recvData");
	for (int i=0; i< 4; i++)
		cout << _recvcounts[i]<<" ";
	cout <<endl;
	for (int i=0; i< 4; i++)
		cout << _recvdispls[i]<<" ";
	cout <<endl;
	
	cout<<"start AllToAll"<<endl;
	_comm_interface->allToAllV(_sendbuffer, _sendcounts, _senddispls, MPI_DOUBLE, 
			_recvbuffer, _recvcounts, _recvdispls, MPI_DOUBLE,MPI_COMM_WORLD);
	cout<<"end AllToAll"<<endl;

	int nb_local = _topotarget->getNbLocalElements();
	double* value=new double[nb_local];
	int myranktarget=_topotarget->getProcGroup()->myRank();
	vector<int> counters(_toposource->getProcGroup()->size());
	counters[0]=0;
	for (int i=0; i<_toposource->getProcGroup()->size()-1; i++)
		{
			MPIProcessorGroup* group=new MPIProcessorGroup(*_comm_interface);
			int worldrank=group->translateRank(_toposource->getProcGroup(),i);
			counters[i+1]=counters[i]+_recvcounts[worldrank];
		}
	
	for (int ielem=0; ielem<nb_local ; ielem++)
	{
		int global = _topotarget->localToGlobal(make_pair(myranktarget, ielem));
		pair<int,int> source_local =_toposource->globalToLocal(global);
		value[ielem]=_recvbuffer[counters[source_local.first]++];
	}
	
	
	_target_field->getField()->setValue(value);
}

void StructuredCoincidentDEC::sendData()
{
	MESSAGE ("sendData");
	for (int i=0; i< 4; i++)
		cout << _sendcounts[i]<<" ";
	cout <<endl;
	for (int i=0; i< 4; i++)
		cout << _senddispls[i]<<" ";
	cout <<endl;
	//MPI_COMM_WORLD is used instead of group because there is no
	//mechanism for creating the union group yet
	cout <<"start AllToAll"<<endl;
	
	_comm_interface->allToAllV(_sendbuffer, _sendcounts, _senddispls, MPI_DOUBLE, 
			_recvbuffer, _recvcounts, _recvdispls, MPI_DOUBLE,MPI_COMM_WORLD);
	cout<<"end AllToAll"<<endl;
}
	

}

