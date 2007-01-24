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
#include "MEDMEM_Grid.hxx"
#include "ParaGRID.hxx"
#include "StructuredParaSUPPORT.hxx"
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
	
	
	double xmin=0.0, xmax=1.0, ymin=0.0, ymax=1.0;
	int nx=500, ny=500;
	std::vector<std::vector<double> > axes(2);
	axes[0].resize(nx);
	axes[1].resize(ny);
	for (int i=0; i<nx; i++)
		axes[0][i]=xmin+((xmax-xmin)*i)/(nx-1);
	for (int i=0; i<ny; i++)
		axes[1][i]=ymin+((ymax-ymin)*i)/(ny-1);
	vector <string> coord_name;
	vector <string> coord_unit;
	coord_name.push_back("x");coord_name.push_back("y");
	coord_unit.push_back("m");coord_unit.push_back("m");
	
	MEDMEM::GRID grid(axes, coord_name,coord_unit);
	
	grid.setName("grid_5_5");
	grid.setDescription("5 by 5 square grid");
	Topology* topo_source = new BlockTopology(*self_group, grid);
	Topology* topo_target = new BlockTopology(*target_group,grid);
	ParaGRID* source_grid;
	ParaGRID* target_grid;
	StructuredParaSUPPORT* target_support;
	StructuredParaSUPPORT* source_support;
	ComponentTopology* target_comp;
	ComponentTopology* source_comp;
	ParaFIELD* target_field=0;
	ParaFIELD* source_field=0;
	
	if (source_group->containsMyRank())
	{
		source_grid=new ParaGRID(&grid, topo_source);
		source_support=new StructuredParaSUPPORT(source_grid,MED_EN::MED_CELL);
		source_comp=new ComponentTopology (6, source_group);
		source_field = new ParaFIELD(source_support, *source_comp);
		cout << "Source field nb elems on rank : "<<rank<<" : "<<source_field->getTopology()->getNbLocalElements()<<endl;
		double * value= new double[source_field->getTopology()->getNbLocalElements()];
		for(int ielem=0; ielem<source_field->getTopology()->getNbLocalElements();ielem++)
			value[ielem]=(double)ielem;
		source_field->getField()->setValue(value);
		source_field->synchronizeSource(target_field);
	}
	if (target_group->containsMyRank())
	{
		target_grid=new ParaGRID(&grid, topo_target);
		target_support=new StructuredParaSUPPORT(target_grid,MED_EN::MED_CELL);	
		target_comp= new ComponentTopology (6);
		target_field = new ParaFIELD(target_support, *target_comp);
		target_field->synchronizeTarget(source_field);
		//target_grid->write(MED_DRIVER, "/tmp/target");
		//target_field->write(MED_DRIVER, "/tmp/target");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}




