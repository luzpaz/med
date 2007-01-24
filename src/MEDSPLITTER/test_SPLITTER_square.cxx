/*
 * test_SPLITTER_square
 * 
 * computes a partitioning for the following geometry
 * 
 * 
 * 
 * 7------------8------------9
 * |            |            |
 * |            |            |
 * |     3      |     4      |
 * |            |            |
 * |            |            |
 * 4------------5------------6
 * |            |            |
 * |            |            |
 * |     1      |     2      |
 * |            |            |
 * |            |            |
 * 1------------2------------3 
 *
 * Result of the 2 domain split :
 *  
 * 5------------6 5------------6
 * |            | |            |
 * |            | |            |
 * |     2      | |     2      |
 * |            | |            |
 * |            | |            |
 * 1------------2 1------------2
 * |            | |            |
 * |            | |            |
 * |     1      | |     1      |
 * |            | |            |
 * |            | |            |
 * 4------------3 4------------3 
*/

#include <string>
#include <vector>
#include <map>
#include <iostream>

 
#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_GaussLocalization.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Group.hxx"

#include "MEDSPLITTER_Graph.hxx"

#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"

#include "MEDSPLITTER_METISGraph.hxx"
#include "MEDSPLITTER_SCOTCHGraph.hxx"

#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"

using namespace MEDSPLITTER;
using namespace std;

int main()
{
	string testname="MEDSPLITTER - test #1 -";
	
	char filename[100] = "../../share/salome/resources/carre_en_quad4_import22.med";
	char meshname[20]  = "carre_en_quad4";
	
	MESHCollection collection(filename,meshname);
	MESHCollection new_collection(collection, collection.createPartition(2,Graph::METIS));
	
	//collection.write("/export/home/test_splitter");
	new_collection.write("./tests/carre_split");
	new_collection.castAllFields(collection);
	
	char filename1[100]= "./tests/carre_split1.med";
	char filename2[100]= "./tests/carre_split2.med";
	
	MEDMEM::MESH mesh1(MEDMEM::MED_DRIVER, filename1, meshname);
	MEDMEM::MESH mesh2(MEDMEM::MED_DRIVER, filename2, meshname);
	
	
	// testing number of elements for each partition
	
	int nbelem1=mesh1.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
	int nbelem2=mesh2.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
	
	if (nbelem1!=2)
	{
		cerr<< testname<<" wrong number of elements for mesh 1"<<endl;
		return 1;
	}
	if (nbelem2!=2)
	{
		cerr<< testname<<" wrong number of elements for mesh 2"<<endl;
		return 1;
	}
	
	//testing number of joints
	med_2_2::med_idt fid1 = med_2_2::MEDouvrir(filename1,med_2_2::MED_LECTURE);
    med_2_2::med_idt fid2 = med_2_2::MEDouvrir(filename2,med_2_2::MED_LECTURE);
   
    int nj1= med_2_2::MEDnJoint(fid1, meshname);
    int nj2= med_2_2::MEDnJoint(fid2, meshname);
    if (nj1!=1)
	{
		cerr<< testname<<" wrong number of joints for mesh 1"<<endl;
		return 1;
	}
	if (nj2!=1)
	{
		cerr<< testname<<" wrong number of joints for mesh 2"<<endl;
		return 1;
	}
	
	
	//testing distant domains
	
	char desc1[MED_TAILLE_DESC];
	char maa_dist1[MED_TAILLE_NOM], jn1[MED_TAILLE_NOM];
	char desc2[MED_TAILLE_DESC], maa_dist2[MED_TAILLE_NOM], jn2[MED_TAILLE_NOM];
	int dom1, dom2;
	med_2_2::MEDjointInfo(fid1, meshname, 1, jn1, desc1, &dom1, maa_dist1);
	med_2_2::MEDjointInfo(fid2, meshname, 1, jn2, desc2, &dom2, maa_dist2);

	if (dom1!=1)
	{
		cerr<< testname<<" wrong distant domain "<< dom1<<"for mesh 1"<<endl;
		return 1;
	}
	if (dom2!=0)
	{
		cerr<< testname<<" wrong distant domain "<< dom2<<"for mesh 2"<<endl;
		return 1;
	}
	// testing node-node correspondency
	med_2_2::med_entite_maillage typ_ent_loc=med_2_2::MED_NOEUD;
	med_2_2::med_entite_maillage typ_ent_dist=med_2_2::MED_NOEUD;
	med_2_2::med_geometrie_element typ_geo_loc= med_2_2::MED_POINT1;
	med_2_2::med_geometrie_element typ_geo_dist= med_2_2::MED_POINT1;
	
	int n1 =med_2_2::MEDjointnCorres(fid1,meshname,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	int* tab=new int[2*n1];
	
	med_2_2::MEDjointLire(fid1,meshname,jn1,tab,n1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	
	int tabreference1[6]={2,1,3,4,6,5};
	for (int i=0; i<2*n1; i++)
		if (tab[i]!=tabreference1[i])
			{cerr<<testname<<"wrong node-node correspondency in joint 1"<<endl;return 1;}
	delete[] tab;

	int n2 =med_2_2::MEDjointnCorres(fid2,meshname,jn2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	tab=new int[2*n2];
	
	med_2_2::MEDjointLire(fid2,meshname,jn2,tab,n2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	
	int tabreference2[6]={1,2,4,3,5,6};
	for (int i=0; i<2*n1; i++)
		if (tab[i]!=tabreference2[i])
			{cerr<<testname<<"wrong node-node correspondency in joint 2"<<endl;return 1;}
	delete[] tab;

	//testing nodes global numbering
	int* num=new int[6];
	cout <<"Reading global "<<
		MEDglobalNumLire(fid1, meshname, num, 6, typ_ent_loc, typ_geo_loc)<<endl;
	int globnoderef1[6]={4,5,2,1,7,8};
	
	for (int i=0; i<6; i++)
	{
		if (num[i]!=globnoderef1[i])
			{cerr<<testname<<"wrong node global numbering in mesh 1"<<endl;return 1;}
	}
	delete[] num;
	
	//testing nodes global numbering
	num=new int[6];
	MEDglobalNumLire(fid2, meshname, num, 6, typ_ent_loc, typ_geo_loc);
	int globnoderef2[6]={5,6,3,2,8,9};
	for (int i=0; i<6; i++)
		if (num[i]!=globnoderef2[i])
			{cerr<<testname<<"wrong node global numbering in mesh 2"<<endl;return 1;}
	

	//testing cell-cell correspondency
	typ_ent_loc=med_2_2::MED_MAILLE;
	typ_ent_dist=med_2_2::MED_MAILLE;
	typ_geo_loc= med_2_2::MED_QUAD4;
	typ_geo_dist= med_2_2::MED_QUAD4;
	//joint1
	n1 =med_2_2::MEDjointnCorres(fid1,meshname,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	tab=new int[2*n1];
	med_2_2::MEDjointLire(fid1,meshname,jn1,tab,n1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	
	int tabreferencecell1[4]={1,1,2,2};
	for (int i=0; i<2*n1; i++)
		if (tab[i]!=tabreferencecell1[i])
			{cerr<<testname<<"wrong cell-cell correspondency in joint 1"<<endl;return 1;}
			
	n2 =med_2_2::MEDjointnCorres(fid1,meshname,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	delete[] tab;
	//joint2
	tab=new int[2*n2];
	med_2_2::MEDjointLire(fid2,meshname,jn2,tab,n2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
	
	int tabreferencecell2[4]={1,1,2,2};
	for (int i=0; i<n2; i++)
		if (num[i]!=tabreferencecell2[i])
			{cerr<<testname<<"wrong cell-cell correspondency in joint 2"<<endl;return 1;}
			
	delete[]tab;
		
	//testing cells global numbering
	num=new int[2];
	MEDglobalNumLire(fid1, meshname, num, 2, typ_ent_loc, typ_geo_loc);
	int globcellref1[2]={1,3};
	for (int i=0; i<2; i++)
		if (tab[i]!=globcellref1[i])
			{cerr<<testname<<"wrong cell global numbering in mesh 1"<<endl;return 1;}
	delete[] num;
	
	//testing cells global numbering
	num=new int[2];
	MEDglobalNumLire(fid2, meshname, num, 2, typ_ent_loc, typ_geo_loc);
	int globcellref2[2]={2,4};
	for (int i=0; i<2; i++)
		if (tab[i]!=globcellref2[i])
			{cerr<<testname<<"wrong cell global numbering in mesh 2"<<endl;}
	delete[] num;	
	
		
	return 0;
}


<<<<<<< test_SPLITTER_square.cxx

	=======

	
>>>>>>> 1.1.4.1
