#include <mpi.h>
#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "MPIProcessorGroup.hxx"
#include "ExplicitCoincidentDEC.hxx"

namespace ParaMEDMEM
{

ExplicitCoincidentDEC::ExplicitCoincidentDEC():_toposource(0),_topotarget(0)
{	
}

ExplicitCoincidentDEC::~ExplicitCoincidentDEC()
{
}

/*! Synchronization process for exchanging topologies
 */
void ExplicitCoincidentDEC::synchronize()
{
	if (_source_field!=0)
		_toposource = dynamic_cast<ExplicitTopology*>(_source_field->getTopology());
	if (_target_field!=0)
		_topotarget = dynamic_cast<ExplicitTopology*>(_target_field->getTopology());
	
	// Transmitting source topology to target code 
	broadcastTopology(*_toposource,*_topotarget,1000);
	// Transmitting target topology to source code
	//broadcastTopology(_topotarget,2000);
	//checkCompatibility(_toposource,_topotarget);
}

/*! Creates the arrays necessary for the data transfer
 * and fills the send array with the values of the 
 * source field
 *  */
void ExplicitCoincidentDEC::prepareSourceDE()
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
		pair<int,int> target_local =_distant_elems[ielem];
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
	  //int global=_toposource->localToGlobal(ielem);
		int target_local =_topotarget->globalToLocal(global);
		//cout <<"global : "<< global<<" local :"<<target_local.first<<" "<<target_local.second;
		//cout <<"counter[]"<<counter[target_local.first]<<endl;
		_sendbuffer[counter[target_local]++]=value[ielem];
		
	}
	delete[] target_arrays;
	delete[] counter;
}

/*!
 *  Creates the buffers for receiving the fields on the target side
 */
void ExplicitCoincidentDEC::prepareTargetDE()
{
	if (!_topotarget->getProcGroup()->containsMyRank())
		return;
	MPIProcessorGroup* group=new MPIProcessorGroup(_toposource->getProcGroup()->getCommInterface());
	
	//int myranktarget = _topotarget->getProcGroup()->myRank();
	
	vector < vector <int> > source_arrays(_toposource->getProcGroup()->size());
	int nb_local = _topotarget-> getNbLocalElements();
	for (int ielem=0; ielem< nb_local ; ielem++)
	{
		pair<int,int> source_local =_distant_elems[ielem];
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
void ExplicitCoincidentDEC::broadcastTopology(const ExplicitTopology& toposend, ExplicitTopology& toporecv, int tag)
{
	MPI_Status status;
	
	int* serializer=0;
	int size;
	
	MPIProcessorGroup* group=new MPIProcessorGroup(*_comm_interface);
	
	// The send processors serialize the send topology
	// and send the buffers to the recv procs
	if (toposend.getProcGroup()->containsMyRank())
	{
		toposend.serialize(serializer, size);
		for (int iproc=0; iproc< group->size(); iproc++)
		{
			int itarget=(iproc+toposend.getProcGroup()->myRank())%group->size();
			if (!toposend.getProcGroup()->contains(itarget))
			{
				int nbelem = toposend.getNbLocalElements();
				_comm_interface->send(&nbelem,1,MPI_INTEGER, itarget,tag+itarget,*(group->getComm()));
				_comm_interface->send(&serializer, size, MPI_INTEGER, itarget, tag+itarget,*(group->getComm()));					
			}
		}
	}
	else
	{
		vector <int> size (group->size());
		
		for (int iproc=0; iproc<group->size();iproc++)
		{
			int isource = iproc;
			if (!toporecv.getProcGroup()->contains(isource))
			{
				int nbelem;
				_comm_interface->recv(&nbelem, 1, MPI_INTEGER, isource, tag+isource, *(group->getComm()), &status);
				int* buffer = new int[nbelem];
				_comm_interface->recv(buffer, nbelem, MPI_INTEGER, isource,tag+isource, *(group->getComm()), &status);				
			
				ExplicitTopology* topotemp=new ExplicitTopology();
				topotemp->unserialize(buffer, *_comm_interface);
				delete[] buffer;
				
				for (int ielem=0; ielem<toporecv.getNbLocalElements(); ielem++)
				{
					int global=toporecv.localToGlobal(make_pair(iproc,ielem));
					int sendlocal=topotemp->globalToLocal(global);
					if (sendlocal!=-1)
					{
						size[iproc]++;
						_distant_elems.insert(make_pair(ielem, make_pair(iproc,sendlocal)));
					}
				}
				delete topotemp;
			}
		}	
	}	
	MESSAGE (" rank "<<group->myRank()<< " broadcastTopology is over");
}

void ExplicitCoincidentDEC::recvData()
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
		int source_local =_toposource->globalToLocal(global);
		value[ielem]=_recvbuffer[counters[source_local]++];
	}
	
	
	_target_field->getField()->setValue(value);
}

void ExplicitCoincidentDEC::sendData()
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

