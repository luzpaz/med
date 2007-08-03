#ifndef COMMINTERFACE_HXX_
#define COMMINTERFACE_HXX_

#include <mpi.h>
namespace ParaMEDMEM
{

class CommInterface
{
public:
	CommInterface(){}
	virtual ~CommInterface(){}
	int commSize(MPI_Comm comm, int* size) const { return MPI_Comm_size(comm,size);}
	int commRank(MPI_Comm comm, int* rank) const { return MPI_Comm_rank(comm,rank);}
	int commGroup(MPI_Comm comm, MPI_Group* group) const  
		{return MPI_Comm_group(comm, group);}
	int groupIncl(MPI_Group group, int size, int* ranks, MPI_Group* group_output) const 
		{return MPI_Group_incl(group, size, ranks, group_output);}
	int commCreate(MPI_Comm comm, MPI_Group group, MPI_Comm* comm_output) const 
	 	{return MPI_Comm_create(comm,group,comm_output);}
	int groupFree(MPI_Group* group) const {return MPI_Group_free(group);}
	int commFree(MPI_Comm* comm) const {return MPI_Comm_free(comm);}
	int broadcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)const
	{return MPI_Bcast(buffer, count,  datatype, root, comm);}
	int send(void* buffer, int count, MPI_Datatype datatype, int target, int tag, MPI_Comm comm) const
	{return MPI_Send(buffer,count, datatype, target, tag, comm);}
	int recv(void* buffer, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status* status) const
	{return MPI_Recv(buffer,count, datatype, source, tag, comm, status);}
	int allToAllV(void* sendbuf, int* sendcounts, int* senddispls, MPI_Datatype sendtype,
				  void* recvbuf, int* recvcounts, int* recvdispls, MPI_Datatype recvtype, 
				  MPI_Comm comm) const
	{return MPI_Alltoallv(sendbuf, sendcounts, senddispls, sendtype,
						  recvbuf, recvcounts, recvdispls, recvtype,
						  comm);}
  int allToAll(void* sendbuf, int sendcount, MPI_Datatype sendtype,
	       void* recvbuf, int recvcount, MPI_Datatype recvtype,
	       MPI_Comm comm) const
  {
    return MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
  }
	int allGather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
				  void* recvbuf, int recvcount, MPI_Datatype recvtype,
				  	MPI_Comm comm) const
	{return MPI_Allgather(sendbuf,sendcount, sendtype, recvbuf, recvcount, recvtype, comm);  
	}
  int sendRecv(void* sendbuf, int sendcount, MPI_Datatype sendtype,
               int dest, int sendtag, void* recvbuf, int recvcount, 
               MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
               MPI_Status* status)
               {
                return 
                MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf,
                recvcount, recvtype, source, recvtag, comm,status);
               }
  int worldSize() const {int size; MPI_Comm_size(MPI_COMM_WORLD, &size); return size;}
	int reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, 
						 int root, MPI_Comm comm) const
	{return MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);}
	int allReduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, 
						 MPI_Comm comm) const
	{return MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);}
};

}

#endif /*COMMINTERFACE_HXX_*/
