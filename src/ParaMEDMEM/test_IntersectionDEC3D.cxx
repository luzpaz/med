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
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Support.hxx"
#include "ParaMESH.hxx"
#include "UnstructuredParaSUPPORT.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "IntersectionDEC.cxx"

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
      cout << "mpirun -n <nbprocs> test_IntersectionDEC3D <nbprocsource>"<<endl;
      cout << " (nbprocs >=2)"<<endl;
      return 1;
    }
  int nproc_source = atoi(argv[1]);
  set<int> self_procs;
  set<int> procs_source;
  set<int> procs_target;
	
  for (int i=0; i<nproc_source; i++)
    procs_source.insert(i);
  for (int i=nproc_source; i<size; i++)
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
  
  MEDMEM::SUPPORT* boundary_group;
  //loading the geometry for the source group
  if (source_group->containsMyRank())
    {
			//      string master = "/home/vb144235/resources/square128000_split";
      string master = "/home/vb144235/resources/blade_3000_split1";
      
      cout <<"loading source"<<endl;
      source_mesh=new ParaMESH(MED_DRIVER,master,*source_group);
      cout <<"end of load"<<endl;
      topo_source=source_mesh->getBlockTopology();
      boundary_group = new SUPPORT(source_mesh->getMesh(), "support on all cells", MED_EN::MED_CELL);     
    }
  
  //loading the geometry for the target group
  if (target_group->containsMyRank())
    {
			//      string master = "/home/vb144235/resources/square30000_split";
      string master= "/home/vb144235/resources/blade_12000_split1";
      target_mesh=new ParaMESH(MED_DRIVER,master,*target_group);
      topo_target=target_mesh->getBlockTopology();
   
      boundary_group = new SUPPORT(target_mesh->getMesh(), "support on all cells", MED_EN::MED_CELL);     
  
    }
		
	
  //attaching a DEC to the source group 
  if (source_group->containsMyRank())
    { 
        
      //UnstructuredParaSUPPORT source_support(boundary_group, source_group);
      UnstructuredParaSUPPORT source_support(source_mesh, boundary_group);
      ComponentTopology source_comp(1);
      cout << "setting up field"<<endl;
      ParaFIELD source_field (&source_support,source_comp);
		
      int nb_local = source_field.getTopology()->getNbLocalElements();
      cout << "Source field nb elems on rank : "<<rank<<" : "<<nb_local<<endl;
      double * value= new double[nb_local];
      for(int ielem=0; ielem<nb_local;ielem++)
	value[ielem]=1.0;
      source_field.getField()->setValue(value);
      cout <<"creating intersectionDEC"<<endl;
      IntersectionDEC dec(*source_group,*target_group);
      dec.attachLocalField(&source_field);
      dec.synchronize();
      cout<<"DEC usage"<<endl;
      
      dec.sendData();
      source_mesh->write(MED_DRIVER,"/home/vb144235/tmp/sourcesquareb");
      source_field.write(MED_DRIVER,"/home/vb144235/tmp/sourcesquareb","boundary");  
      
      dec.recvData();
      cout <<"writing"<<endl;
      source_mesh->write(MED_DRIVER,"/home/vb144235/tmp/sourcesquare");
      source_field.write(MED_DRIVER,"/home/vb144235/tmp/sourcesquare","boundary");  
      delete[] value;
    }
  
  //attaching a DEC to the target group
  if (target_group->containsMyRank())
    {

      UnstructuredParaSUPPORT target_support(target_mesh, boundary_group);
      //UnstructuredParaSUPPORT target_support(boundary_group, target_group);	
      ComponentTopology target_comp(1);
      ParaFIELD target_field(&target_support,target_comp);
      int nb_local = target_field.getTopology()->getNbLocalElements();
      double * value= new double[nb_local];
      for(int ielem=0; ielem<nb_local;ielem++)
	value[ielem]=0.0;
      target_field.getField()->setValue(value);
      IntersectionDEC dec(*source_group,*target_group);
      dec.attachLocalField(&target_field);
      dec.synchronize();
      dec.recvData();
      target_mesh->write(MED_DRIVER, "/home/vb144235/tmp/targetsquareb");
      target_field.write(MED_DRIVER, "/home/vb144235/tmp/targetsquareb", "boundary");
      dec.sendData();
      target_mesh->write(MED_DRIVER, "/home/vb144235/tmp/targetsquare");
      target_field.write(MED_DRIVER, "/home/vb144235/tmp/targetsquare", "boundary");
      delete[] value;
    }
	
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}


