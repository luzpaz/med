#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <mpi.h>

#include "ProcessorGroup.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "CommInterface.hxx"


#include "MPIProcessorGroup.hxx"
#include "MEDMEM_Mesh.hxx"
#include "ParaMESH.hxx"
#include "UnstructuredParaSUPPORT.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"


using namespace std;
using namespace ParaMEDMEM;
using namespace MEDMEM;
int main(int argc, char** argv)
{
	string testname="ParaMEDMEM - test #1 -";
	MPI_Init(&argc, &argv); 
	int size;
	int rank;
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if (size<3) 
	{ 
		cout << " test_DEC test program is not meant to run "<<endl;
		cout << " with less than 3 processes"<<endl;
		return 1;
	}
	set<int> self_procs;
	set<int> procs_source;
	set<int> procs_target;
	for (int i=0; i<size-2; i++)
		procs_source.insert(i);
	for (int i=size-2; i<size; i++)
		procs_target.insert(i);
	self_procs.insert(rank);
	
	ParaMEDMEM::CommInterface interface;
		
	ParaMEDMEM::ProcessorGroup* self_group = new ParaMEDMEM::MPIProcessorGroup(interface,self_procs);
	ParaMEDMEM::ProcessorGroup* target_group = new ParaMEDMEM::MPIProcessorGroup(interface,procs_target);
	ParaMEDMEM::ProcessorGroup* source_group = new ParaMEDMEM::MPIProcessorGroup(interface,procs_source);
	
	ParaMEDMEM::ParaMESH* source_mesh=0;
	ParaMEDMEM::ParaMESH* target_mesh=0;
	
	
	Topology* topo_source;
	Topology* topo_target;
	if (source_group->containsMyRank())
	{
		source_mesh=new ParaMESH(MED_DRIVER,"../../share/salome/resources/med/pointe_nosplit",*source_group);
		topo_source=source_mesh->getBlockTopology();
	}
	if (target_group->containsMyRank())
	{
		target_mesh=new ParaMESH(MED_DRIVER,"../../share/salome/resources/med/pointe_split",*target_group);
		topo_target=target_mesh->getBlockTopology();
	}
		
	UnstructuredParaSUPPORT* target_support;
	UnstructuredParaSUPPORT* source_support;
	ComponentTopology* target_comp;
	ComponentTopology* source_comp;
	ParaFIELD* target_field=0;
	ParaFIELD* source_field=0;
	
	if (source_group->containsMyRank())
	{
		source_support=new UnstructuredParaSUPPORT(source_mesh,MED_EN::MED_CELL);
		source_comp=new ComponentTopology (1);
		source_field = new ParaFIELD(source_support,*source_comp);
		int nb_local = source_field->getTopology()->getNbLocalElements();
		cout << "Source field nb elems on rank : "<<rank<<" : "<<nb_local<<endl;
		double * value= new double[nb_local];
		for(int ielem=0; ielem<nb_local;ielem++)
			value[ielem]=(double)ielem;
		source_field->getField()->setValue(value);
		source_field->synchronizeSource(target_field);
		if (source_group->myRank()==0)
		{
			source_mesh->write(MED_DRIVER,"/home/vb144235/tmp/sourceexp");
			source_field->write(MED_DRIVER,"/home/vb144235/tmp/sourceexp","maa1");
		}
	}
	if (target_group->containsMyRank())
	{
		target_support=new UnstructuredParaSUPPORT(target_mesh,MED_EN::MED_CELL);	
		target_comp=new ComponentTopology(1);
		target_field = new ParaFIELD(target_support,*target_comp);
		target_field->synchronizeTarget(source_field);
		target_mesh->write(MED_DRIVER, "/home/vb144235/tmp/targetexp");
		target_field->write(MED_DRIVER, "/home/vb144235/tmp/targetexp", "maa1_1");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}




