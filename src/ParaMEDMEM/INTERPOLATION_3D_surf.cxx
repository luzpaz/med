#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"
#include "TranslationRotationMatrix.hxx"
#include "INTERPOLATION_Utils.hxx"
#include "INTERPOLATION_3D_surf.hxx"
#include "BBTree.H"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;
using namespace ParaMEDMEM_utils;

namespace ParaMEDMEM
{

	INTERPOLATION_3D_surf::INTERPOLATION_3D_surf()
	{
    _Precision=1.0E-12;
    _DimCaracteristic=1;
    _SecondMethodOfLocalisation=1;
    _PrintLevel=0;
		_Surf3DAdjustmentEps=0.1;
	}

	/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
	/*   Options :                                        */
	/*     Precision : for geometric computation          */
	/*     SecondMethodOfLocalisation : 0/1               */
	/*     PrintLevel : between 0 and 3                   */
	/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
	void INTERPOLATION_3D_surf::setOptions(double Precision, int SecondMethodOfLocalisation, int PrintLevel)
	{
    _Precision=Precision;
    _SecondMethodOfLocalisation=SecondMethodOfLocalisation;
    _PrintLevel=PrintLevel;
	}



	/*! method computing the translation/rotation matrix
		necessary to transform a triangle into a triangle located inside the Oxy plane. The method fails only if two of the three points are coincident
		\param P1 pointer to three coordinates defining the first point
		\param P2 pointer to three coordinates defining the second point
		\param P3 pointer to three coordinates defining the third point
		\return TranslationRotationMatrix object containing the transform that must be applied to the triangle for putting it inside the Oxy plane.
	*/
	void INTERPOLATION_3D_surf::rotate3DTriangle(const double* PP1, const double*PP2, const double*PP3, TranslationRotationMatrix& rotation_matrix)
	{
		//initializes
		rotation_matrix.translate(PP1);
  
		double P2w[3];
		double P3w[3];
		P2w[0]=PP2[0]; P2w[1]=PP2[1];P2w[2]=PP2[2];
		P3w[0]=PP3[0]; P3w[1]=PP3[1];P3w[2]=PP3[2];

		// translating to set P1 at the origin
		for (int i=0; i<2; i++)
			{
				P2w[i]-=PP1[i];
				P3w[i]-=PP1[i];
			}
   
		// rotating to set P2 on the Oxy plane
		TranslationRotationMatrix A;
		A.rotate_x(P2w);
		A.rotate_vector(P3w);
		rotation_matrix.multiply(A);

		//rotating to set P2 on the Ox axis
		TranslationRotationMatrix B;
		B.rotate_z(P2w);
		B.rotate_vector(P3w);
		rotation_matrix.multiply(B);
  
		//rotating to set P3 on the Oxy plane
		TranslationRotationMatrix C;
		C.rotate_x(P3w);
		rotation_matrix.multiply(C);
  
	}



	/* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ */
	/*  fonction qui permet de remplir le vecteur donnant la surface d'intersection           */
	/*  de la maille i_P du maillage P (projeté) avec la maille i_S du maillage S (support)  */   
	/* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ _ _  */


	inline void INTERPOLATION_3D_surf::rempli_vect_d_intersect(int i_P,int i_S,const monMaillageP& MaP,const monMaillageS& MaS,	
																														 vector<map<int,double> >& Surface_d_intersect
																														 )
 
	{


    //on récupere le n° des noeuds.

    //debug cout << "\nintersect maille " << i_P << endl;
    int NP_1=MaP._Connect[3*(i_P-1)];
    int NP_2=MaP._Connect[3*(i_P-1)+1];
    int NP_3=MaP._Connect[3*(i_P-1)+2];

    
    TranslationRotationMatrix rotation;
		rotate3DTriangle(MaP._Coord+3*(NP_1-1),MaP._Coord+3*(NP_2-1), MaP._Coord+3*(NP_3-1), rotation);
    
    //on calcule la surface de la maille i_P

		//    double Surf_i_P =ParaMEDMEM_utils::Surf_Tri(MaP._Coord+3*(NP_1-1),MaP._Coord+3*(NP_2-1),
		//																						MaP._Coord+3*(NP_3-1));

		// double Surface = 0 ;
 
    
    
    int NS_1=MaS._Connect[3*(i_S-1)];
    int NS_2=MaS._Connect[3*(i_S-1)+1];
    int NS_3=MaS._Connect[3*(i_S-1)+2];
    
    double PS1[3];
    double PS2[3];
    double PS3[3];
		double PP1[3];
		double PP2[3];
		double PP3[3];
		
    for (int i=0; i<3; i++)
      {
				PS1[i] = *(MaS._Coord+3*(NS_1-1)+i);
				PS2[i] = *(MaS._Coord+3*(NS_2-1)+i);
				PS3[i] = *(MaS._Coord+3*(NS_3-1)+i);
				PP1[i] = *(MaP._Coord+3*(NP_1-1)+i);
				PP2[i] = *(MaP._Coord+3*(NP_2-1)+i);
				PP3[i] = *(MaP._Coord+3*(NP_3-1)+i);	
      }

    rotation.transform_vector(PS1);
    rotation.transform_vector(PS2);
    rotation.transform_vector(PS3);
    rotation.transform_vector(PP1);
    rotation.transform_vector(PP2);
    rotation.transform_vector(PP3);



    //intersects 3D triangle only considering the two first coordinates
    // 
    //> MaP is located in the Oxy plane
    //> PS is not and is therefore projected on MaP orthogonally along the z
    //   direction 
    vector<double> Inter=ParaMEDMEM_utils::
			intersec_de_triangle(PS1,PS2,PS3,
													 PP1,PP2,PP3,
													 _DimCaracteristic,
													 _Precision);
    
    
    //on teste l'intersection.
    int taille=Inter.size()/2;
    //debug cout << "  -> maille source : " << i_S << " , nb intersection=" <<  taille << endl;
    
    
    /* CAS 3  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/ 
    if(taille==3)
      {
				double Surf_inter = Surf_Poly(Inter) ;
	
				//on remplit le vecteur des correspondances n° maille-surfaces d'intersections.
				Surface_d_intersect[i_P-1][i_S]=Surf_inter;
	
	
				//	Surface = Surface + Surf_inter ; 
      }
    
    /* CAS 4  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    
    else if (taille>3) //taille>3
      {
				vector<double> Poly_Inter=reconstruct_polygon(Inter);
	
				double Surf_inter =Surf_Poly(Poly_Inter) ;
	
				//on remplit le vecteur des correspondances n° maille-surfaces d'intersection
				(Surface_d_intersect[i_P-1])[i_S]=Surf_inter;
	
				//	Surface = Surface + Surf_inter ; 
      }

		//debug cout << "     surface = " << Surface << endl << flush;
    

    /* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on rentre la fraction totale de la maille i_P qui a été considérer lors de l'interpolation.
    //  double Fract=Surface/Surf_i_P;
    //  myField_P.setValueIJ(i_P,1,Fract);

	}


	/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
	//fonction principale pour interpoler deux maillages triangulaires.
	vector<map<int,double> > INTERPOLATION_3D_surf::interpol_maillages(const MEDMEM::MESH& myMesh_S,
																																		 const MEDMEM::MESH& myMesh_P)
	{


    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    // on vérifie que les deux maillages sont formés de triangles.
    int NumberOfCellsTypes_S = myMesh_S.getNumberOfTypes(MED_CELL);
    int NumberOfCellsTypes_P = myMesh_P.getNumberOfTypes(MED_CELL);
    if  ( NumberOfCellsTypes_S != 1  || NumberOfCellsTypes_P != 1)
			{ cout<<"l'un au moins des deux maillages n'est pas triangulaires"<<endl;}
    string* Type_S = myMesh_S.getCellTypeNames(MED_CELL);
    string* Type_P = myMesh_P.getCellTypeNames(MED_CELL);
    if ( Type_S[0] != "MED_TRIA3" || Type_P[0] != "MED_TRIA3")
			{ cout<<"l'un au moins des deux maillages n'est pas triangulaires"<<endl;}
    //VB
    delete[] Type_S;
    delete[] Type_P;

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
		ParaMEDMEM_utils::monMaillageS MaS;
		ParaMEDMEM_utils::monMaillageP MaP;

    MaS._NbNoeud = myMesh_S.getNumberOfNodes() ;
    MaP._NbNoeud = myMesh_P.getNumberOfNodes() ;
    MaS._NbMaille =myMesh_S.getNumberOfElements(MED_CELL,MED_TRIA3);
    MaP._NbMaille =myMesh_P.getNumberOfElements(MED_CELL,MED_TRIA3);

    //on charge les connectivités des deux maillages.
    MaS._Connect =myMesh_S.getConnectivity(MED_FULL_INTERLACE, 
																					 MED_NODAL, MED_CELL, MED_TRIA3) ;
    MaP._Connect =myMesh_P.getConnectivity(MED_FULL_INTERLACE, 
																					 MED_NODAL, MED_CELL, MED_TRIA3) ;

    //on charge les coordonnées des noeuds.
    MaS._Coord = myMesh_S.getCoordinates(MED_FULL_INTERLACE);
    MaP._Coord = myMesh_P.getCoordinates(MED_FULL_INTERLACE);

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on cherche la dimension caractéristique des maillages
		/*______________________________________________________________*/

    vector<vector<double> > BoxS=myMesh_S.getBoundingBox();
    vector<vector<double> > BoxP=myMesh_P.getBoundingBox();
    double VolumeS=sqrt((BoxS[1][0]-BoxS[0][0])*(BoxS[1][0]-BoxS[0][0])+(BoxS[1][1]-BoxS[0][1])*(BoxS[1][1]-BoxS[0][1]))+(BoxS[1][2]-BoxS[0][2])*(BoxS[1][2]-BoxS[0][2]);
    MaS._DimCaracteristic=sqrt(VolumeS/MaS._NbMaille);
    double VolumeP=sqrt((BoxP[1][0]-BoxP[0][0])*(BoxP[1][0]-BoxP[0][0])+(BoxP[1][1]-BoxP[0][1])*(BoxP[1][1]-BoxP[0][1]))+(BoxP[1][2]-BoxP[0][2])*(BoxP[1][2]-BoxP[0][2]);
    MaP._DimCaracteristic=sqrt(VolumeP/MaP._NbMaille);
    
    _DimCaracteristic=min(MaS._DimCaracteristic,MaP._DimCaracteristic);
    if (_PrintLevel)
			{
				cout<<"recherche de la dimension caractéristique des maillages 3D_surf :"<<endl;
				cout<<"  - dimension caractéristique du maillage source : "<<MaS._DimCaracteristic<<endl;
				cout<<"  - dimension caractéristique du maillage projeté: "<<MaS._DimCaracteristic<<endl;
				cout<<"  - d'où la dimension caractéristique: "<<_DimCaracteristic<<endl;
			}

		//creating a search structure based on the bounding boxes 
		//of the elements to know 

		vector<double> bbox;
		createBoundingBoxes(MaS,bbox); // create the bounding boxes
		adjustBoundingBoxes(bbox); // expand them so that no intersection is missed
 
		BBTree<3> my_tree(&bbox[0], 0, 0,MaS._NbMaille );//creating the search structure 

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on crée un tableau où l'on rentrera la valeurs des intersections.
    cout << "INTERPOLATION_3D_surf::calcul intersec"<<endl;
    map<int,double> MAP_init;
    vector<map<int,double> > Surface_d_intersect(MaP._NbMaille,MAP_init);//on initialise.


    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //boucle sur les mailles de P.
    //Coeur de l'algorithme

    for(int i_P=0; i_P<MaP._NbMaille ; i_P++)
			{
				vector<int> intersecting_elems;
				double bb[6];
				ParaMEDMEM_utils::getElemBB(bb,MaP,i_P);
				my_tree.getIntersectingElems(bb, intersecting_elems);
				
				//browsing all the i_S (from mesh S) elems that can 
				//intersect elem i_P (from mesh P)
				for (int ielem=0; ielem<intersecting_elems.size();ielem++)
					{
						//BBTree structure returns numbers between 0 and n-1
						int i_S=intersecting_elems[ielem]+1; 
	
						rempli_vect_d_intersect(i_P+1,i_S,MaP,MaS,Surface_d_intersect);
					}
				intersecting_elems.clear();
			}


		//DEBUG prints
    if (_PrintLevel >= 2)
			{
				cout<<endl<<"Impression des surfaces d'intersection:"<<endl<<endl;
				cout<<"(maille source, maille cible):surface d'intersection"<<endl;
				double total=0.0;
				for(int i=0; i< Surface_d_intersect.size();i++)
					{ 
						if(Surface_d_intersect[i].size()!=0)
							{
								map<int,double>::iterator surface;
								for( surface=Surface_d_intersect[i].begin();surface!=Surface_d_intersect[i].end();surface++)
									{
										cout<<"    ("<<i+1<<" , "<<(*surface).first<<")"<<" : "<<(*surface).second<<endl;
										total+=(*surface).second;
									}
							}
					}  
				cout << "surface totale "<<total<<endl;
			}

    return Surface_d_intersect;

	}





	/*! Creates a set of bounding boxes that correspond to mesh \a mesh. The vector returned is of dimension 6*nb_elems with bounding boxes stored as xmin1, xmax1, ymin1, ymax1, zmin1, zmax1, xmin2, xmax2, ymin2,... 
		The returned pointer must be deleted by the calling code.

		\param mesh structure pointing to the mesh
		\param bbox vector containing the bounding boxes
	*/

	void INTERPOLATION_3D_surf::createBoundingBoxes(const struct monMaillageS Ma_S, vector<double>& bbox)
	{
		/* We build the segment tree for locating possible matching intersections*/
		bbox.resize(6* Ma_S._NbMaille);
		for (int i=0; i< Ma_S._NbMaille; i++)
			{
				bbox[i*6]=HUGE;
				bbox[i*6+1]=-HUGE;
				bbox[i*6+2]=HUGE;
				bbox[i*6+3]=-HUGE;
				bbox[i*6+4]=HUGE;
				bbox[i*6+5]=-HUGE;
			
				for (int j=0; j<3; j++)
					{
						double x=Ma_S._Coord[(Ma_S._Connect[i*3+j]-1)*3];
						bbox[i*6]=(bbox[i*6]<x)?bbox[i*6]:x;
						bbox[i*6+1]=(bbox[i*6+1]>x)?bbox[i*6+1]:x;
						double y=Ma_S._Coord[(Ma_S._Connect[i*3+j]-1)*3+1];
						bbox[i*6+2]=(bbox[i*6+2]<y)?bbox[i*6+2]:y;
						bbox[i*6+3]=(bbox[i*6+3]>y)?bbox[i*6+3]:y;
						double z=Ma_S._Coord[(Ma_S._Connect[i*3+j]-1)*3+2];
						bbox[i*6+4]=(bbox[i*6+4]<z)?bbox[i*6+4]:z;
						bbox[i*6+5]=(bbox[i*6+5]>z)?bbox[i*6+5]:z;
					}
			}
	}


	/*! Readjusts a set of bounding boxes so that they are extended
		in all dimensions for avoiding missing interesting intersections

		\param bbox vector containing the bounding boxes
	*/

	void INTERPOLATION_3D_surf::adjustBoundingBoxes(vector<double>& bbox)
	{
		/* We build the segment tree for locating possible matching intersections*/

		int size = bbox.size()/6;
		for (int i=0; i<size; i++)
			{
				double Dx=bbox[i*6+1]-bbox[i*6];
				double Dy=bbox[i*6+3]-bbox[i*6+2];
				double Dz=bbox[i*6+5]-bbox[i*6+4];
				double max=(Dx<Dy)?Dy:Dx;
				max=(max<Dz)?Dz:max;
				bbox[i*6]-=_Surf3DAdjustmentEps*max;
				bbox[i*6+1]+=_Surf3DAdjustmentEps*max;
				bbox[i*6+2]-=_Surf3DAdjustmentEps*max;
				bbox[i*6+3]+=_Surf3DAdjustmentEps*max;
				bbox[i*6+4]-=_Surf3DAdjustmentEps*max;
				bbox[i*6+5]+=_Surf3DAdjustmentEps*max;

			}
	}

};
