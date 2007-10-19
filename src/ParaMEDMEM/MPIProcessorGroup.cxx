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
	/*! Creates a processor group that is based on all the
MPI_COMM_WORLD processor.This routine must be called by all processors in MPI_COMM_WORLD.
\param interface CommInterface object giving access to the MPI
communication layer
	*/
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

	/*! Creates a processor group that is based on the processors included in \a proc_ids.
This routine must be called by all processors in MPI_COMM_WORLD.

\param interface CommInterface object giving access to the MPI
communication layer
\param proc_ids set of ids that are to be integrated in the group. The ids number are 
to be understood in terms of MPI_COMM_WORLD ranks.
	*/

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
  delete[] ranks;
}
	/*! Creates a processor group that is based on the processors between \a pstart and \pend.
This routine must be called by all processors in MPI_COMM_WORLD.

\param interface CommInterface object giving access to the MPI
communication layer
\param pstart id in MPI_COMM_WORLD of the first processor in the group
\param pend id in MPI_COMM_WORLD of the last processor in the group
	*/
MPIProcessorGroup::MPIProcessorGroup (const CommInterface& comm_interface, int pstart, int pend): ProcessorGroup(comm_interface,pstart,pend)
{
 //Creation of a communicator 
  MPI_Group group_world;
  
  int size_world;
  _comm_interface.commSize(MPI_COMM_WORLD,&size_world);
  int rank_world;
  _comm_interface.commRank(MPI_COMM_WORLD,&rank_world);
  _comm_interface.commGroup(MPI_COMM_WORLD, &group_world);

  int nprocs=pend-pstart+1;
  int* ranks=new int[nprocs];
  for (int i=pstart; i<=pend;i++)
    {
      ranks[i-pstart]=i;
    }

  _comm_interface.groupIncl(group_world, nprocs, ranks, &_group);
  
  _comm_interface.commCreate(MPI_COMM_WORLD, _group, &_comm);
  //  delete[] ranks;
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
	if (_comm!=MPI_COMM_WORLD && _comm !=MPI_COMM_NULL)
		_comm_interface.commFree(&_comm);
	_comm_interface.groupFree(&_group);
}

	/*! Translation of the rank id between two processor groups. This method translates rank \a rank
on the current processor group to the rank on group pointed by \group.
\param group group on which the rank is expected
\param rank local rank of the processor which is to be translated
\return rank on group \a group
	*/
int MPIProcessorGroup::translateRank(const ProcessorGroup* group, int rank) const
{
	const MPIProcessorGroup* targetgroup=dynamic_cast<const MPIProcessorGroup*>(group);
	int local_rank;
	MPI_Group_translate_ranks(targetgroup->_group, 1, &rank, _group, &local_rank);
	return local_rank;
}
	
	/*!Creates a processor group that is the complement of the current group 
		inside MPI_COMM_WORLD
		\return pointer to the new ProcessorGroup structure.
	*/
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
	/*!Adding processors of group \a group to local group.
		\param group group that is to be fused with current group
		\return new group formed by the fusion of local group and \a group.
	 */
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
