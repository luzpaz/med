#include "InterpolationUtils.hxx"
#include "TranslationRotationMatrix.hxx"
#include "Interpolation.hxx"
#include "Interpolation2D.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"

#include "MEDMEM_InterpolationHighLevelObjects.hxx"
#include "MEDMEM_Interpolation.hxx"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;


namespace MEDMEM 
{

  Interpolation2D::Interpolation2D():_counter(0)
  {
    _Precision=1.0E-12;
    _DimCaracteristic=1;
    _SecondMethodOfLocalisation=1;
    _PrintLevel=0;
  }

  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*   Options :                                        */
  /*     Precision : for geometric computation          */
  /*     SecondMethodOfLocalisation : 0/1               */
  /*     PrintLevel : between 0 and 3                   */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  void Interpolation2D::setOptions(double Precision, int SecondMethodOfLocalisation, int PrintLevel)
  {
    _Precision=Precision;
    _SecondMethodOfLocalisation=SecondMethodOfLocalisation;
    _PrintLevel=PrintLevel;
  }

  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*localise le barycentre des mailles de P dans le maillage S*/
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */ 

  void Interpolation2D::local_iP_dans_S(MESH& myMesh_S,MESH& myMesh_P,INTERP_UTILS::monMaillageP& MaP,
					INTERP_UTILS::monMaillageS& MaS,vector<int>& localis )
  {

    //calcul des coordonnées des barycentre des mailles de P


    //calcule des coordonnees du barycentre d'une cellule
    std::auto_ptr<SUPPORT::SUPPORT>  Support ( new SUPPORT(&myMesh_P,"monSupport",MED_CELL) );
    std::auto_ptr<FIELD<double, FullInterlace> > Barycentre (myMesh_P.getBarycenter(Support.get() ));
    const  double* Bary=Barycentre->getValue();  


    //localisation des barycentres des mailles de P dans les mailles de S.
    Meta_Wrapper<2> fromWrapper(MaS._NbNoeud,(double *)MaS._Coord,(MEDMEM::CONNECTIVITY*)myMesh_S.getConnectivityptr());
    Meta_Wrapper<2> toWrapper(MaP._NbMaille,(double *)Bary);
    Meta_Mapping<2> mapping(&fromWrapper,&toWrapper);
    mapping.Cree_Mapping(0);
    for(int i=0;i<MaP._NbMaille;i++)
      localis.push_back(mapping.Get_Mapping()[i]+1);


    if (_SecondMethodOfLocalisation)
      {
	if(_PrintLevel)
	  cout << endl << "Option de localisations complémentaires" << endl;
	/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
	//on vérifie que tout les barycentres ont obtenu un n° de maille 
	//non nul.
	MEDMEM::INTERPOLATION<2> monInterpol_S(myMesh_S);
	for(int i_P=0;i_P<MaP._NbMaille;i_P++)
	  {
	    if(localis[i_P]<=0)
	      {
		//debug cout << endl << "------------------------------------------------" << endl << "Localise : barycentre maille " << i_P << " en dehors" << endl; 
		int Test=0;

		int NP_1=MaP._Connect[3*i_P];
		int NP_2=MaP._Connect[3*i_P+1];
		int NP_3=MaP._Connect[3*i_P+2];

		double Coord_bary_i_P[2]={Bary[2*i_P],Bary[2*i_P+1]};
		int Vois_N=monInterpol_S.getNearestNode(Coord_bary_i_P)+1;
		int Ni=MaS._ReversNConnectIndex[Vois_N-1];
		int Nf=MaS._ReversNConnectIndex[Vois_N];
		int diff=Nf-Ni;

		for(int j=0;j<diff;j++)
		  {  
		    int N_Maille=MaS._ReversNConnect[Ni-1+j];
		    int NS_1=MaS._Connect[3*(N_Maille-1)];
		    int NS_2=MaS._Connect[3*(N_Maille-1)+1];
		    int NS_3=MaS._Connect[3*(N_Maille-1)+2];


		    //debug cout << "mailles sources testées : " << N_Maille << endl;
		    vector<double> Inter;
		    INTERP_UTILS::intersec_de_triangle(MaS._Coord+2*(NS_1-1),MaS._Coord+2*(NS_2-1),
						       MaS._Coord+2*(NS_3-1),MaP._Coord+2*(NP_1-1),
						       MaP._Coord+2*(NP_2-1),MaP._Coord+2*(NP_3-1),
						       Inter,_DimCaracteristic, _Precision);
		    _counter++;
		    if(Inter.size()>0)
		      {
			//debug cout << "Localise : maille proche barycentre trouvée : " << N_Maille << endl; 
			Test=1;
			localis[i_P]=N_Maille;
			break;
		      }
		  }
		if(Test==0)
		  {
		    int NoeuMaillP[3]={NP_1,NP_2,NP_3};
		    for(int Num=0;Num<3;Num++)
		      {
			int Num_noeud=NoeuMaillP[Num];
			double coord_i_P_0[2];
			//VB
			coord_i_P_0[0]= MaP._Coord[2*(Num_noeud-1)];
			coord_i_P_0[1]=MaP._Coord[2*(Num_noeud-1)+1];
			//VB
			int Vois_B=monInterpol_S.getNearestNode(coord_i_P_0)+1;
			int Ni=MaS._ReversNConnectIndex[Vois_B-1];
			int Nf=MaS._ReversNConnectIndex[Vois_B];
			int diff=Nf-Ni;

			for(int j=0;j<diff;j++)
			  {
			    int N_Maille=MaS._ReversNConnect[Ni-1+j];
			    int N_1=MaS._Connect[3*(N_Maille-1)];
			    int N_2=MaS._Connect[3*(N_Maille-1)+1];
			    int N_3=MaS._Connect[3*(N_Maille-1)+2];

			    //debug cout << "mailles sources testées : " << N_Maille << endl;
			    vector<double> Inter;
			    INTERP_UTILS::intersec_de_triangle(MaS._Coord+2*(N_1-1),MaS._Coord+2*(N_2-1),
							       MaS._Coord+2*(N_3-1),MaP._Coord+2*(NP_1-1),
							       MaP._Coord+2*(NP_2-1),MaP._Coord+2*(NP_3-1),
							       Inter, _DimCaracteristic, _Precision);	
			    _counter++;
			    if(Inter.size()>0)
			      {
				//debug cout << "Localise : maille proche sommet " << Num+1 << " trouvée : " << N_Maille << endl; 
				Test=1;
				localis[i_P]=N_Maille;
				break;//on sort de la boucle sur les maille commune à un noeud de i_S
			      }
			  }
			if(Test==1)
			  {break;}//on sort de la boucle sur les noeuds de i_P
		      }   
		  }
	      }
	  }
      }
  }









  /* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ */
  /*  fonction qui permet de remplir le vecteur donnant la surface d'intersection           */
  /*  de la maille i_P du maillage P (projeté) avec la maille i_S du maillage S (support)  */   
  /* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ _ _  */


  inline void Interpolation2D::rempli_vect_d_intersect(int i_P,int i_S1,const INTERP_UTILS::monMaillageP& MaP,const INTERP_UTILS::monMaillageS& MaS,	
						       vector<map<int,double> >& Surface_d_intersect,FIELD<double>& myField_P)
  {

    
    //on récupere le n° des noeuds.

    //debug cout << "\nintersect maille " << i_P << endl;
    int NP_1=MaP._Connect[3*(i_P-1)];
    int NP_2=MaP._Connect[3*(i_P-1)+1];
    int NP_3=MaP._Connect[3*(i_P-1)+2];


    //on calcule la surface de la maille i_P

    double Surf_i_P =INTERP_UTILS::Surf_Tri(MaP._Coord+2*(NP_1-1),MaP._Coord+2*(NP_2-1),
					    MaP._Coord+2*(NP_3-1));

    double Surface = 0 ;
    vector<int> maill_deja_vu_S ;

    maill_deja_vu_S.push_back(i_S1);

    for (int M_S=0; M_S<maill_deja_vu_S.size(); M_S++)
      {
	if( abs(Surf_i_P-Surface)/Surf_i_P>_Precision && M_S!=maill_deja_vu_S.size() )
	  {
	    int i_S=maill_deja_vu_S[M_S];

	    int NS_1=MaS._Connect[3*(i_S-1)];
	    int NS_2=MaS._Connect[3*(i_S-1)+1];
	    int NS_3=MaS._Connect[3*(i_S-1)+2];


	    vector<double> Inter;
	    INTERP_UTILS::intersec_de_triangle(MaS._Coord+2*(NS_1-1),MaS._Coord+2*(NS_2-1),
					       MaS._Coord+2*(NS_3-1),MaP._Coord+2*(NP_1-1),
					       MaP._Coord+2*(NP_2-1),MaP._Coord+2*(NP_3-1),
					       Inter,_DimCaracteristic, _Precision);
	    _counter++;

	    //on teste l'intersection.
	    int taille=Inter.size()/2;
	    //debug cout << "  -> maille source : " << i_S << " , nb intersection=" <<  taille << endl;

	    /* CAS1  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
	    // taille ==0, on ne fait rien

	    /* CAS 2  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
	    if(taille==1 || taille==2)
	      {
		//on ne remplit pas le vecteur des correspondances n° maille-surfaces 
		//d'intersections mais on récupère le numéro des mailles voisines de la maille i_S.
		vector<int> M_Vois=INTERP_UTILS::touv_maill_vois(i_S,MaS);
		for(int i=0;i< M_Vois.size();i++)
		  {INTERP_UTILS::verif_maill_dans_vect(M_Vois[i],maill_deja_vu_S);}
	      }

	    /* CAS 3  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/ 
	    else if(taille==3)
	      {
		double Surf_inter = INTERP_UTILS::Surf_Poly(Inter) ;

		//on remplit le vecteur des correspondances n° maille-surfaces d'intersections.
		Surface_d_intersect[i_P-1][i_S]=Surf_inter;
				
		// on récupère le numéro des mailles voisines de la maille i_S.
		vector<int> M_Vois=INTERP_UTILS::touv_maill_vois(i_S,MaS);
		for(int i_M=0;i_M<(M_Vois).size();i_M++)
		  {INTERP_UTILS::verif_maill_dans_vect(M_Vois[i_M],maill_deja_vu_S);}

		Surface = Surface + Surf_inter ; 
	      }
			
	    /* CAS 4  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

	    else if (taille > 3) //taille>3
	      {
		vector<double> Poly_Inter=INTERP_UTILS::reconstruct_polygon(Inter);

		double Surf_inter =INTERP_UTILS::Surf_Poly(Poly_Inter) ;

		//on remplit le vecteur des correspondances n° maille-surfaces d'intersection
		(Surface_d_intersect[i_P-1])[i_S]=Surf_inter;

		// on récupère le numéro des mailles voisines de la maille i_S.
		vector<int> M_Vois=INTERP_UTILS::touv_maill_vois(i_S,MaS);
		for(int i_M=0;i_M<(M_Vois).size();i_M++)
		  {INTERP_UTILS::verif_maill_dans_vect(M_Vois[i_M],maill_deja_vu_S);}

		Surface = Surface + Surf_inter ; 
	      }
	  }
	//debug cout << "     surface = " << Surface << endl << flush;
      }

    /* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on rentre la fraction totale de la maille i_P qui a été considérer lors de l'interpolation.
    double Fract=Surface/Surf_i_P;
    myField_P.setValueIJ(i_P,1,Fract);

  }


  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  //fonction principale pour interpoler deux maillages triangulaires.
  vector<map<int,double> > Interpolation2D::interpolateMeshes(const MEDMEM::MESH& myMesh_S,
							       const MEDMEM::MESH& myMesh_P)
  {

    long global_start =clock();    
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
    INTERP_UTILS::monMaillageS MaS;
    INTERP_UTILS::monMaillageP MaP;

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

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/ 
    //on a besoin de recupérer connectivité inverse de S.
    MaS._ReversNConnect=
      myMesh_S.getReverseConnectivity(MED_NODAL);
    MaS._ReversNConnectIndex=
      myMesh_S.getReverseConnectivityIndex(MED_NODAL);


    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on cherche la dimension caractéristique des maillages
    vector<vector<double> > BoxS=myMesh_S.getBoundingBox();
    vector<vector<double> > BoxP=myMesh_P.getBoundingBox();
    double AreaS=sqrt((BoxS[1][0]-BoxS[0][0])*(BoxS[1][0]-BoxS[0][0])+(BoxS[1][1]-BoxS[0][1])*(BoxS[1][1]-BoxS[0][1]));
    MaS._DimCaracteristic=sqrt(4./sqrt(3.)*AreaS/MaS._NbMaille);
    double AreaP=sqrt((BoxP[1][0]-BoxP[0][0])*(BoxP[1][0]-BoxP[0][0])+(BoxP[1][1]-BoxP[0][1])*(BoxP[1][1]-BoxP[0][1]));
    MaP._DimCaracteristic=sqrt(4./sqrt(3.)*AreaP/MaP._NbMaille);
    _DimCaracteristic=min(MaS._DimCaracteristic,MaP._DimCaracteristic);
    if (_PrintLevel)
      {
	cout<<"recherche de la dimension caractéristique des maillages 2D :"<<endl;
	cout<<"  - dimension caractéristique du maillage source : "<<MaS._DimCaracteristic<<endl;
	cout<<"  - dimension caractéristique du maillage projeté: "<<MaS._DimCaracteristic<<endl;
	cout<<"  - d'où la dimension caractéristique: "<<_DimCaracteristic<<endl;
      }
    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on cherche pour chaque maille i_P du maillage projetté
    // une maille i_S du maillage S qui chevauche la maille i_P.

    long start_filtering=clock();
    vector<int> localis;
    localis.reserve(MaP._NbMaille);
    MEDMEM::MESH* ptr_S = const_cast<MEDMEM::MESH*>(&myMesh_S);
    MEDMEM::MESH* ptr_P = const_cast<MEDMEM::MESH*>(&myMesh_P);
    
    cout << "Interpolation2D::local_iP_dansS"<<endl;
    local_iP_dans_S(*ptr_S,*ptr_P,MaP,MaS,localis);
    long end_filtering=clock();

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on crée un tableau où l'on rentrera la valeurs des intersections.
    cout << "Interpolation2D::calcul intersec"<<endl;
    map<int,double> MAP_init;
    vector<map<int,double> > Surface_d_intersect(MaP._NbMaille,MAP_init);//on initialise.

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //création d'un champ pour rentrer les fractions de mailles considérées.

    std::auto_ptr<SUPPORT> mySupport_P (new SUPPORT(const_cast<MEDMEM::MESH*>(&myMesh_P),"Support on all CELLS",MED_CELL) );
    MEDMEM::FIELD<double> myField_P(mySupport_P.get(),1);
    string NameF=" M2 dans M1" ;
    myField_P.setName(NameF);
    string NameC="fracSurf";
    string ComponentsNames[1]={NameC};
    myField_P.setComponentsNames(ComponentsNames);
    myField_P.setComponentsDescriptions(ComponentsNames);
    string ComponentsUnits[1]={"%"};
    myField_P.setMEDComponentsUnits(ComponentsUnits);

    /*à remplacer par:
      WriteFractSurfInFile(vector<<vector<pair<int,double> > >& FractVol,MEDMEM::MESH* myMesh1,
      MEDMEM::MESH* myMesh2,string FileName,string NameMeshP,string NameMeshS)
    */

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //boucle sur les mailles de P.
    //Coeur de l'algorithme
    long start_intersection=clock();
    for(int i_P=0; i_P<MaP._NbMaille ; i_P++)
      {
	int i_S_init=localis[i_P];
	if(i_S_init>0)
	  rempli_vect_d_intersect(i_P+1,i_S_init,MaP,MaS,Surface_d_intersect,myField_P);

      }
    if (_PrintLevel >= 1)
      {
	long end_intersection=clock();
	if (_PrintLevel >= 2)
	  {
	    cout<<endl<<"Printing intersection areas:"<<endl<<endl;
	    cout<<"(source cell, target cell): intersection areas"<<endl;
	    double total=0.0;
	    double total_interm=0.0;
	    int nb_result_areas = Surface_d_intersect.size();
	    for(int i=0; i< nb_result_areas;i++)
	      { 
		map<int,double>::iterator surface;
		total_interm=0.0;
		for( surface=Surface_d_intersect[i].begin();surface!=Surface_d_intersect[i].end();surface++)
		  {
		    cout<<"    ("<<i+1<<" , "<<(*surface).first<<")"<<" : "<<(*surface).second<<endl;
		    total_interm +=(*surface).second;
		  }
		cout<< " elem " << i+1 << " area= " << total_interm << endl;
		total+=total_interm;
	      }
	    cout << "total area "<<total<<endl;
	  }
	cout<< "Barycenter localisation time= " << end_filtering-start_filtering<< endl;
	cout<< "Intersection time= " << end_intersection-start_intersection<< endl;
	cout<< "counter= " << _counter << endl;
	long global_end =clock();    
	cout<< "Global time= " << global_end - global_start << endl;
      }
    
    //    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //    //On sauvegarde le champ crée sur la maillage P.
    //    if (_PrintLevel>=3)
    //    {
    ////	string NameFile="fractSurf.med";
    ////	int id1=myMesh_P.addDriver(MED_DRIVER,NameFile,"M1");
    ////	int id2=myMesh_S.addDriver(MED_DRIVER,NameFile,"M2");
    ////	int id3=myField_P.addDriver(MED_DRIVER,NameFile,NameF);
    ////	myMesh_P.write(id1);
    ////	myMesh_S.write(id2);
    ////	myField_P.write(id3);
    ////
    ////	cout<<endl<<"Pour chaque mailles de " <<myMesh_S.getName();
    ////	cout<<"Impression de la fraction surfaciques totale dans le maillage : "<<myMesh_P.getName()<<endl; 
    ////	for(int i=0;i<MaP._NbMaille;i++)
    ////	    cout<<"maille n°"<<i+1<<" de "<<myMesh_P.getName()<<": "<<myField_P.getValueIJ(i+1,1)<<endl;
    //    }

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //   for (int i=0; i< Surface_d_intersect.size();i++)
    //       {
    // 	double size=0;
    // 	for (map<int,double>::const_iterator iter = Surface_d_intersect[i].begin();
    // 	     iter !=Surface_d_intersect[i].end();
    // 	     iter++)
    // 	  size+= iter->second;
    // 	//debug cout << "\nintersect maille " << i_P << endl;
    // 	int NP_1=MaP._Connect[3*(i)];
    // 	int NP_2=MaP._Connect[3*(i)+1];
    // 	int NP_3=MaP._Connect[3*(i)+2];


    //     //on calcule la surface de la maille i_P

    // 	double Surf_i_P =Surf_Tri(MaP._Coord+2*(NP_1-1),MaP._Coord+2*(NP_2-1),
    // 				  MaP._Coord+2*(NP_3-1));	
    // 	cout <<size<<" "<<Surf_i_P<<endl;
    //       }
    return Surface_d_intersect;

  }


};
