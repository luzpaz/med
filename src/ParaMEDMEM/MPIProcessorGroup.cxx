#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"
#include "CommInterface.hxx"

#include <iostream>
#include <set>
#include <algorithm>
#include "mpi.h"

using namespace std;

namespace ParaMEDMEM
{
MPIProcessorGroup::MPIProcessorGroup(const CommInterface& interface):
ProcessorGroup(interface)
{
  _comm=MPI_COMM_WORLD;
  _comm_interface.commGroup(MPI_COMM_WORLD, &_group);
  int size;
  _comm_interface.commSize(MPI_COMM_WORLD,&size);
  for (int i=0; i<size; i++)
  	_proc_ids.insert(i);

}
MPIProcessorGroup::MPIProcessorGroup(const CommInterface& interface, set<int> proc_ids):
ProcessorGroup(interface, proc_ids)
{
  //Creation of a communicator 
  MPI_Group group_world;
  
  int size_world;
  _comm_interface.commSize(MPI_COMM_WORLD,&size_world);
  int rank_world;
  _comm_interface.commRank(MPI_COMM_WORLD,&rank_world);
  _comm_interface.commGroup(MPI_COMM_WORLD, &group_world);

  int* ranks=new int[proc_ids.size()];
   
  // copying proc_ids in ranks
  copy<set<int>::const_iterator,int*> (proc_ids.begin(), proc_ids.end(), ranks);

  _comm_interface.groupIncl(group_world, proc_ids.size(), ranks, &_group);
  
  _comm_interface.commCreate(MPI_COMM_WORLD, _group, &_comm);
  
}

MPIProcessorGroup::MPIProcessorGroup (const ProcessorGroup& proc_group, set<int> proc_ids) :
ProcessorGroup(proc_group.getCommInterface())
{
	cout << "MPIProcessorGroup (const ProcessorGroup& proc_group, set<int> proc_ids)" <<endl;
	cout << "Not implemented yet !"<<endl;
	exit(1);
}

MPIProcessorGroup::~MPIProcessorGroup()
{
	_comm_interface.groupFree(&_group);
	if (_comm!=MPI_COMM_WORLD && _comm !=MPI_COMM_NULL)
		_comm_interface.commFree(&_comm);
}

int MPIProcessorGroup::translateRank(const ProcessorGroup* group, int rank) const
{
	const MPIProcessorGroup* targetgroup=dynamic_cast<const MPIProcessorGroup*>(group);
	int local_rank;
	MPI_Group_translate_ranks(targetgroup->_group, 1, &rank, _group, &local_rank);
	return local_rank;
}


ProcessorGroup* MPIProcessorGroup::createComplementProcGroup() const
{
  set <int> procs;
  int world_size=_comm_interface.worldSize();
  for (int i=0; i<world_size; i++)
    procs.insert(i);
  for (set<int>::const_iterator iter=_proc_ids.begin(); iter!= _proc_ids.end(); iter++)
    procs.erase(*iter);
  
  return new MPIProcessorGroup(_comm_interface, procs);

}
ProcessorGroup* MPIProcessorGroup::createProcGroup() const
{
  set <int> procs;
  for (set<int>::const_iterator iter=_proc_ids.begin(); iter!= _proc_ids.end(); iter++)
    procs.insert(*iter);
  
  return new MPIProcessorGroup(_comm_interface, procs);

}
ProcessorGroup*  MPIProcessorGroup::fuse (const ProcessorGroup& group) const
{
  set <int> procs = _proc_ids;
  const set<int>& distant_proc_ids = group.getProcIDs();
  for (set<int>::const_iterator iter=distant_proc_ids.begin(); iter!=distant_proc_ids.end(); iter++)
  {
    procs.insert(*iter);
  }
  return new MPIProcessorGroup(_comm_interface,procs);
}

	
}
