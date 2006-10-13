/*
 * test_SPLITTER_indivisible
 * 
 * computes a partitioning for the a geometry
 * with 8 TRIA3 and 4 QUAD4. A group containing the quads
 * is set to be indivisible. 
 * It is checked that on output, there is only one group 
 * containing quads.
 * SCOTCH is used for this partitioning.
 * */

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_ConnectZone.hxx"

#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"


using namespace MEDSPLITTER;
using namespace std;

int main() 
{
	string testname="MEDSPLITTER - test #2 -";
	
	char filename[100] = "../../share/salome/resources/maill.00.med";
	char meshname[20]  = "MAILTRQU";
	char family[30]="QUAD";
	MESHCollection collection(filename,meshname);
	collection.setIndivisibleGroup(family);
	MESHCollection new_collection(collection, collection.createPartition(4,Graph::SCOTCH));
	
	//collection.write("/export/home/test_splitter");
	new_collection.write("./tests/carre_indivisible");
	
	char filename1[100]= "./tests/carre_indivisible1.med";
	char filename2[100]= "./tests/carre_indivisible2.med";
	char filename3[100]= "./tests/carre_indivisible3.med";
	char filename4[100]= "./tests/carre_indivisible4.med";
	
	MEDMEM::MESH mesh1(MEDMEM::MED_DRIVER, filename1, meshname);
	MEDMEM::MESH mesh2(MEDMEM::MED_DRIVER, filename2, meshname);
	MEDMEM::MESH mesh3(MEDMEM::MED_DRIVER, filename3, meshname);
	MEDMEM::MESH mesh4(MEDMEM::MED_DRIVER, filename4, meshname);
		
	// testing number of quads for each partition
	int nbquad1=	mesh1.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);
	int nbquad2=	mesh2.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);
	int nbquad3=	mesh3.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);
	int nbquad4=	mesh4.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);

	int nb_domain_with_quad=(nbquad1?1:0)+(nbquad2?1:0)+(nbquad3?1:0)+(nbquad4?1:0);
//	cout << nbquad1<<nbquad2<<nbquad3<<nbquad4;
	if (nb_domain_with_quad!=1)
	{
		cerr<<"Wrong partition for indivisible test"<<endl; 
		return 1;
	}
	
	return 0;
}



	
