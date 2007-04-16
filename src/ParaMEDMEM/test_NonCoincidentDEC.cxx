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
	if (argc !=2 || size < 2) 
	{ 
    cout << "usage :"<<endl;
    cout << "mpirun -n <nbprocs> test_NonCoincidentDEC <nbprocsource>"<<endl;
    cout << " (nbprocs >=2)"<<endl;
    return 1;
	}
  int nproc_source = atoi(argv[1]);
	set<int> self_procs;
	set<int> procs_source;
	set<int> procs_target;
	
  for (int i=0; i<nproc_source; i++)
		procs_source.insert(i);
	for (int i=nproc_source; i<rank; i++)
		procs_target.insert(i);
	self_procs.insert(rank);
	
	ParaMEDMEM::CommInterface interface;
		
	ParaMEDMEM::ProcessorGroup* self_group = new ParaMEDMEM::MPIProcessorGroup(interface,self_procs);
	ParaMEDMEM::ProcessorGroup* target_group = new ParaMEDMEM::MPIProcessorGroup(interface,procs_target);
	ParaMEDMEM::ProcessorGroup* source_group = new ParaMEDMEM::MPIProcessorGroup(interface,procs_source);
	
	ParaMEDMEM::ParaMESH* source_mesh=0;
	ParaMEDMEM::ParaMESH* target_mesh=0;
	ParaMEDMEM::ParaMESH* source_mesh_bis = 0;
  ParaMEDMEM::ParaMESH* target_mesh_bis = 0;
  
	Topology* topo_source;
	Topology* topo_target;
  
	//loading the geometry for the source group
  if (source_group->containsMyRank())
	{
    string master = "/home/vb144235/resources/fluid_split";
		source_mesh=new ParaMESH(MED_DRIVER,master,*source_group);
		topo_source=source_mesh->getBlockTopology();
    ostringstream strstream (master);
    strstream << "_"<<rank<<endl;
    MEDMEM::MESH mesh_bis = new MESH(MED_DRIVER,strstream.str(),'fluid');
    source_mesh_bis = new ParaMESH(*mesh_bis,*source_group);
    const vector <MEDMEM::GROUP*> mesh_groups;
    const MEDMEM::GROUP* boundary_group;
    for (int i=0; i<mesh_groups.size(); i++)
      {
        if (mesh_groups[i].getName() == "boundary")
          boundary_group = mesh_groups[i];
      } 
	}
  
  //loading the geometry for the target group
	if (target_group->containsMyRank())
	{
    string master = "/home/vb144235/resources/blade_split";
		target_mesh=new ParaMESH(MED_DRIVER,master,*target_group);
		topo_target=target_mesh->getBlockTopology();
    ostringstream strstream (master);
    strstream << "_"<<(rank-nproc_source)<<endl;
    MEDMEM::MESH mesh_bis = new MESH(MED_DRIVER,strstream.str(),'Fuse_1');
    target_mesh_bis = new ParaMESH(*mesh_bis,*target_group);
    const vector <MEDMEM::GROUP*> mesh_groups;
    const MEDMEM::GROUP* boundary_group;
    for (int i=0; i<mesh_groups.size(); i++)
      {
        if (mesh_groups[i].getName() == "boundary")
          boundary_group = mesh_groups[i];
      } 
  }
		
	UnstructuredParaSUPPORT* target_support;
	UnstructuredParaSUPPORT* source_support;
	ComponentTopology* target_comp;
	ComponentTopology* source_comp;
	ParaFIELD* target_field=0;
	ParaFIELD* source_field=0;
	
  //attaching a DEC to the source group 
	if (source_group->containsMyRank())
	{ 
        
		//UnstructuredParaSUPPORT source_support(boundary_group, source_group);
    UnstructuredParaSUPPORT source_support(source_mesh_bis, boundary_group);
		ComponentTopology source_comp(1);
		ParaFIELD source_field (source_support,*source_comp);
		
		int nb_local = source_field.getTopology()->getNbLocalElements();
		cout << "Source field nb elems on rank : "<<rank<<" : "<<nb_local<<endl;
		double * value= new double[nb_local];
		for(int ielem=0; ielem<nb_local;ielem++)
			value[ielem]=(double)ielem;
		source_field.getField()->setValue(value);
		NonCoincidentDEC dec;
		dec.attachSourceField(&source_field);
		dec.synchronize();
		dec.sendData();
	
		source_mesh.write(MED_DRIVER,"/home/vb144235/tmp/sourceexp");
		source_field.write(MED_DRIVER,"/home/vb144235/tmp/sourceexp","boundary");  
	}
  
  //attaching a DEC to the target group
	if (target_group->containsMyRank())
	{
    UnstructuredParaSUPPORT target_support(target_mesh_bis, boundary_group);
		//UnstructuredParaSUPPORT target_support(boundary_group, target_group);	
		ComponentTopology target_comp(1);
		ParaFIELD target_field(target_support,*target_comp);
		NonCoincidentDEC dec;
		dec.attachTargetField(&target_field);
		dec.synchronize();
		dec.recvData();
		target_mesh.write(MED_DRIVER, "/home/vb144235/tmp/targetexp");
		target_field.write(MED_DRIVER, "/home/vb144235/tmp/targetexp", "boundary");
	}
	
  MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}




