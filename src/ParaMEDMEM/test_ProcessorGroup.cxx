#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <mpi.h>
#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"


using namespace std;
using namespace ParaMEDMEM;

int main(int argc, char** argv)
{
	string testname="ParaMEDMEM - test #1 -";
	MPI_Init(&argc, &argv); 
	int size;
	int rank;
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	set<int> procs;
	procs.insert(0);
	
	ParaMEDMEM::CommInterface interface;
		
	ParaMEDMEM::ProcessorGroup* group=new ParaMEDMEM::MPIProcessorGroup(interface,procs);
	cout << "proc #"<<rank<<" size :"<<group->size()<<endl;
	if (group->size() !=1) return 1;
		
	set<int> empty_proc_group;
	ParaMEDMEM::ProcessorGroup* group_empty=new ParaMEDMEM::MPIProcessorGroup(interface,empty_proc_group);
	cout << "proc #"<<rank<<" size :"<<group_empty->size()<<endl;
	if (group_empty->size() !=0) return 1;
	
	delete group;
	delete group_empty;
	
	MPI_Finalize();
	return 0;
}




