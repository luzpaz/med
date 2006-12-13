#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"

using namespace MEDSPLITTER;

int main()
{
	string testname="MEDSPLITTER - test #1 -";
	
	char filename[100] = "../../../share/salome/resources/med/carre_en_quad4_import22.med";
	char meshname[20]  = "carre_en_quad4";
	
	MESHCollection collection(filename,meshname);
	MESHCollection new_collection(collection, collection.createGraph(2));
	
	//collection.write("/export/home/test_splitter");
	new_collection.write("./tests/carre_split");
	new_collection.castAllFields(collection);
	
	char filename1[100]= "./tests/carre_split0.med";
	char filename2[100]= "./tests/carre_split1.med";
	
	MEDMEM::MESH mesh1(MED_EN::MED_DRIVER, filename1, meshname);
	MEDMEM::MESH mesh2(MED_EN::MED_DRIVER, filename2, meshname);
	
	
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
    if (nj1!=2)
	{
		cerr<< testname<<" wrong number of joints for mesh 1"<<endl;
		return 1;
	}
	if (nj2!=2)
	{
		cerr<< testname<<" wrong number of joints for mesh 2"<<endl;
		return 1;
	}
	
	
	//testing global numbering
	char desc1[120], dom1[120], maa_dist1[120], jn1[120];
	
	med_2_2::MEDJointInfo(fid1, meshname, 1, jn1, desc1, dom1, maa_dist1);
	med_2_2::med_entite_maillage typ_ent_loc=med_2_2::MED_NOEUD;
	med_2_2::med_entite_maillage typ_ent_dist=med_2_2::MED_NOEUD;
	med_2_2::med_geometrie_element typ_geo_loc= med_2_2::MED_POINT0;
	med_2_2::med_geometrie_element typ_geo_dist= med_2_2::MED_POINT0;
	char cn1[120]="corr1";
	med_2_2::MEDJointLire(fid1,meshname,jn1,cn1,1,&typ_ent_loc, &typ_geo_loc,&typ_ent_dist, &typ_geo_dist);
	
	return 0;
}


	
