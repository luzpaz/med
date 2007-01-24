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
	
	set<int> procs;
	for (int i=0; i< size-1; i++)
	procs.insert(i);
	
	ParaMEDMEM::CommInterface interface;
		
	ParaMEDMEM::ProcessorGroup* group=new ParaMEDMEM::MPIProcessorGroup(interface,procs);
	double xmin=0.0, xmax=1.0, ymin=0.0, ymax=1.0;
	int nx=10, ny=10;
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
	
	grid.setName("grid_10_10");
	grid.setDescription("10 by 10 square grid");
	Topology* topo = new BlockTopology(*group, grid);

	ParaGRID local_grid(&grid, topo);
	StructuredParaSUPPORT support(&local_grid,MED_EN::MED_CELL);
	local_grid.write (MED_DRIVER, "/tmp/toto");
	
	MPI_Finalize();
	return 0;
}




