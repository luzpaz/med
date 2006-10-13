#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_UserGraph.hxx"


using namespace MEDSPLITTER;

/*! constructor that allows to specify the desired partition
 * \param partition as table giving the domain number for each cell 
 *        (domain numbers range from 0 to ndomain-1
 * \param n number of cells in the mesh
*/
UserGraph::UserGraph(MEDMEM::MEDSKYLINEARRAY* array, const int* partition, int n):Graph(array,0)
{
									 
	int* index=new int [n+1];
	index[0]=1;
	for (int i=0; i<n; i++)
	{
		index[i+1]=index[i]+1;
	}
	
	//creating a skylinearray with no copy of the index and partition array
	// the fifth argument true specifies that only the pointers are passed 
	//to the object
	m_partition = new MEDMEM::MEDSKYLINEARRAY(n,n, index, partition, true);
	
} 

UserGraph::~UserGraph()
{
}

void UserGraph::partGraph(int ndomain, const string& options)
{
	cerr<<"MEDSPLITTER::UserGraph::partGraph() should not have to be used"<<endl;
}

