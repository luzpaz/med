#include <med.h>
#include <string.h>


/*****************************************************************************************************/

void affiche_noeuds(med_float * nodes,int nnpl)
	{
	int nbr_nodes=nnpl*nnpl*nnpl;
	int i;

	for (i=0;i<nbr_nodes;i++) printf("Noeud %d : ( %4.5f ; %4.5f ; %4.5f )\n",i,nodes[3*i],nodes[3*i+1],nodes[3*i+2]);
	
	}


void cree_nodes(med_float * coord_nodes,int nnpl, int flag)
	{
	int i,j,k;
	int nbr_nodes=nnpl*nnpl*nnpl;
	int num_noeud;
	int diviseur=nnpl-1+flag;
	
	/*coord_nodes=(med_float *) malloc(3*nbr_nodes*sizeof(med_float));*/
	
	for (i=0;i<nnpl;i++) for (j=0;j<nnpl;j++) for (k=0;k<nnpl;k++) 
		{
		num_noeud=i+nnpl*j+nnpl*nnpl*k;
		
		if (3*num_noeud  >=3*nbr_nodes) {printf("%d : OUT OF RANGE REQUEST\n",num_noeud);exit(-1);}
		if (3*num_noeud+1>=3*nbr_nodes) {printf("%d : OUT OF RANGE REQUEST\n",num_noeud);exit(-1);}
		if (3*num_noeud+2>=3*nbr_nodes) {printf("%d : OUT OF RANGE REQUEST\n",num_noeud);exit(-1);}
		
		coord_nodes[3*num_noeud  ]= (double) (i+flag)/diviseur;
		coord_nodes[3*num_noeud+1]= (double) (j+flag)/diviseur;
		coord_nodes[3*num_noeud+2]= (double) (k+flag)/diviseur;
		}
		
	affiche_noeuds(coord_nodes,nnpl);
	
	}
void cree_num_nodes(med_int * num_nodes,int nnpl)
	{
	int nbr_nodes=nnpl*nnpl*nnpl;
	int i;
	/*num_nodes=(med_int *) malloc(nbr_nodes*sizeof(med_int));*/
	for (i=0;i<nbr_nodes;i++) num_nodes[i]=i+1;//nbr_nodes+i;
	}
void cree_Hexa8(med_int * conn_hexa8, int nnpl,med_int * num_nodes)
	{
	int i,j,k;
	int i0,j0,k0,i1,j1,k1;
	int nbr_nodes=nnpl*nnpl*nnpl;
	int nbr_hexa8=(nnpl-1)*(nnpl-1)*(nnpl-1);
	int num_hexa8;
	
	/*conn_hexa8=(med_int *) malloc(8*nbr_hexa8*sizeof(med_int));*/
	
	for (i=0;i<nnpl-1;i++) for (j=0;j<nnpl-1;j++) for (k=0;k<nnpl-1;k++) 
		{
		i0=i;j0=j;k0=k;
		i1=i+1;j1=j+1;k1=k+1;
		
		num_hexa8=i+(nnpl-1)*j+(nnpl-1)*(nnpl-1)*k;
		
		if (8*num_hexa8  >=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		if (8*num_hexa8+1>=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		if (8*num_hexa8+2>=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		if (8*num_hexa8+3>=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		if (8*num_hexa8+4>=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		if (8*num_hexa8+5>=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		if (8*num_hexa8+6>=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		if (8*num_hexa8+7>=8*nbr_hexa8) {printf("%d : OUT OF RANGE REQUEST\n",num_hexa8);exit(-1);}
		
		conn_hexa8[8*num_hexa8  ] = num_nodes[ i0+nnpl*j1+nnpl*nnpl*k1 ]; 
		conn_hexa8[8*num_hexa8+1] = num_nodes[ i0+nnpl*j0+nnpl*nnpl*k1 ]; 
		conn_hexa8[8*num_hexa8+2] = num_nodes[ i1+nnpl*j0+nnpl*nnpl*k1 ]; 
		conn_hexa8[8*num_hexa8+3] = num_nodes[ i1+nnpl*j1+nnpl*nnpl*k1 ]; 
		conn_hexa8[8*num_hexa8+4] = num_nodes[ i0+nnpl*j1+nnpl*nnpl*k0 ]; 
		conn_hexa8[8*num_hexa8+5] = num_nodes[ i0+nnpl*j0+nnpl*nnpl*k0 ]; 
		conn_hexa8[8*num_hexa8+6] = num_nodes[ i1+nnpl*j0+nnpl*nnpl*k0 ]; 
		conn_hexa8[8*num_hexa8+7] = num_nodes[ i1+nnpl*j1+nnpl*nnpl*k0 ]; 
		
		
		}
		
	for (num_hexa8=0;num_hexa8<nbr_hexa8;num_hexa8++)
		{		
		printf("Maille numéro %4d :",num_hexa8);		
		printf("%4d ",conn_hexa8[8*num_hexa8  ]);
		printf("%4d ",conn_hexa8[8*num_hexa8+1]);
		printf("%4d ",conn_hexa8[8*num_hexa8+2]);
		printf("%4d ",conn_hexa8[8*num_hexa8+3]);
		printf("%4d ",conn_hexa8[8*num_hexa8+4]);
		printf("%4d ",conn_hexa8[8*num_hexa8+5]);
		printf("%4d ",conn_hexa8[8*num_hexa8+6]);
		printf("%4d ",conn_hexa8[8*num_hexa8+7]);
		printf("\n");
		}
	}	
void cree_noms_mailles(char * noms, int nnpl)
	{
	int nbr_hexa8=(nnpl-1)*(nnpl-1)*(nnpl-1);	
	int i;
	char pnom[MED_TAILLE_PNOM+1]="hexa    ";
	
	/*noms=(char *) malloc((nbr_hexa8*MED_TAILLE_PNOM+1)*sizeof(char));*/
	
	for (i=0;i<nbr_hexa8;i++) strncpy(&noms[i*MED_TAILLE_PNOM],pnom,MED_TAILLE_PNOM);
	
	noms[nbr_hexa8*MED_TAILLE_PNOM]='\n';
	
	}
void cree_num_mailles(med_int * num_mailles,int nnpl)
	{
	int nbr_hexa8=(nnpl-1)*(nnpl-1)*(nnpl-1);	
	int i;
	
	/*num_mailles=(med_int *) malloc(nbr_hexa8*sizeof(med_int));*/
	
	for (i=0;i<nbr_hexa8;i++) num_mailles[i]=i+1;
	}
void cree_fam_nodes(med_int * fam,int nnpl)
	{
	int nbr_nodes=nnpl*nnpl*nnpl;
	int i;
	
	/*fam=(med_int *) malloc(nbr_nodes*sizeof(med_int));*/
	
	for (i=0;i<nbr_nodes;i++) fam[i]=0;
	}
void cree_fam_mailles(med_int * fam,int nnpl)
	{
	int nbr_hexa8=(nnpl-1)*(nnpl-1)*(nnpl-1);	
	int i;
	
	/*fam=(med_int *) malloc(nbr_hexa8*sizeof(med_int));*/
	
	for (i=0;i<nbr_hexa8;i++) fam[i]=0;
	}
void cree_valeurs_champ_node(med_float * val,int nnpl)
	{
	int i,j,k;
	int nbr_nodes=nnpl*nnpl*nnpl;
	int num_noeud;
	int diviseur=3*(nnpl-1);
	
	/*val=(med_float *) malloc(nbr_nodes*sizeof(med_float));*/
	
	for (i=0;i<nnpl;i++) for (j=0;j<nnpl;j++) for (k=0;k<nnpl;k++) 
		{
		num_noeud=i+nnpl*j+nnpl*nnpl*k;
		val[num_noeud] = (med_float) (i+j+k)/diviseur;
		}
	for (num_noeud=0;num_noeud<nbr_nodes;num_noeud++) printf("Valeur Scalaire noeud %5d : %4.5f\n",num_noeud,val[num_noeud]);
	}
void cree_valeurs_champ_vector_node(med_float * val,int nnpl)
	{
	int i,j,k;
	int nbr_nodes=nnpl*nnpl*nnpl;
	int num_noeud;
	
	/*val=(med_float *) malloc(nbr_nodes*sizeof(med_float));*/
	
	for (i=0;i<nnpl;i++) for (j=0;j<nnpl;j++) for (k=0;k<nnpl;k++) 
		{
		num_noeud=i+nnpl*j+nnpl*nnpl*k;
		val[3*num_noeud  ] = (med_float) 0;
		val[3*num_noeud+1] = (med_float) 1;
		val[3*num_noeud+2] = (med_float) 2;
		}
	for (num_noeud=0;num_noeud<nbr_nodes;num_noeud++) printf("Valeur Vectorielle noeud %5d : %4.5f %4.5f %4.5f \n ",num_noeud,val[3*num_noeud],val[3*num_noeud+1],val[3*num_noeud+2]);
	}
void cree_valeurs_champ_cell(med_float * val,int nnpl)
	{
	int i,j,k;
	int nbr_cells=(nnpl-1)*(nnpl-1)*(nnpl-1);	
	int num_cell;
	
	med_float diviseur=3*(nnpl-2);
	
	/*val=(med_float *) malloc(3*nbr_cells*sizeof(med_float));*/
	
	for (i=0;i<nnpl-1;i++) for (j=0;j<nnpl-1;j++) for (k=0;k<nnpl-1;k++) 
		{
		num_cell=i+(nnpl-1)*j+(nnpl-1)*(nnpl-1)*k;
		val[num_cell  ] =  (med_float) (i+j+k)/diviseur;
		}
	for (num_cell=0;num_cell<nbr_cells;num_cell++) printf("Valeur scalaire maille %5d : %4.5f\n ",num_cell,val[num_cell]);
	}	
void cree_valeurs_champ_vector_cell(med_float * val,int nnpl)
	{
	int i,j,k;
	int nbr_cells=(nnpl-1)*(nnpl-1)*(nnpl-1);	
	int num_cell;
	
	/*val=(med_float *) malloc(3*nbr_cells*sizeof(med_float));*/
	
	for (i=0;i<nnpl-1;i++) for (j=0;j<nnpl-1;j++) for (k=0;k<nnpl-1;k++) 
		{
		num_cell=i+(nnpl-1)*j+(nnpl-1)*(nnpl-1)*k;
		val[3*num_cell  ] = (med_float) 0;
		val[3*num_cell+1] = (med_float) 1;
		val[3*num_cell+2] = (med_float) 2; 	   
		}
	for (num_cell=0;num_cell<nbr_cells;num_cell++) printf("Valeur Vectorielle maille %5d : %4.5f %4.5f %4.5f \n ",num_cell,val[3*num_cell],val[3*num_cell+1],val[3*num_cell+2]);
	}

	
/*****************************************************************************************************/
	
int main (int argc, char **argv)
	{
		
	int i,j,k;
	
	int nnpl;
	
	int nbr_nodes, nbr_hexa8;

	med_err ret;
	med_idt fromfid;
	med_idt tofid;
	char frommaa[MED_TAILLE_NOM+1] = "fromMesh";
	char tomaa[MED_TAILLE_NOM+1]   = "toMesh";
	char frommaadesc[MED_TAILLE_DESC+1] = "Initial Mesh description";
	char tomaadesc[MED_TAILLE_DESC+1] = "Target Mesh description";
	med_maillage type = MED_NON_STRUCTURE;
	const med_int mdim = 3;
	med_int fromnnoe;
	med_int   tonnoe;
	
	med_float * tocoo;                   
	med_float * fromcoo;                 
	
	char nomcoo[3*MED_TAILLE_PNOM+1] = "x	    y	    z	    ";
	char unicoo[3*MED_TAILLE_PNOM+1] = "cm      cm      cm      ";

	char * nomnoe = NULL ;
	
	med_int  * fromnumnoe;              
	med_int  * fromnufano;              

	med_int  * tonumnoe;                
	med_int  * tonufano;                

	med_int fromnhexa8;
	med_int  * fromhexa8;                                          
			     
	med_int tonhexa8;
	med_int  * tohexa8;                                          
			     
			     
	char  * fromnomhexa8;             
	med_int * fromnumhexa8;                                              
	med_int * fromnufahexa8;                                             

	char  * tonomhexa8;             
	med_int * tonumhexa8;                                              
	med_int * tonufahexa8;                                             

/*****************************************************************************************************/

	char nomfam[MED_TAILLE_NOM+1];
	med_int numfam;
	char attdes[MED_TAILLE_DESC+1];
	med_int natt;
	med_int attide;
	med_int attval;
	med_int ngro;
	char gro[MED_TAILLE_LNOM+1];
	char gro2[MED_TAILLE_LNOM*2+1];
	char gro3[MED_TAILLE_LNOM*3+1];
	int nfame = 1; 
	int nfamn = 1;

/*****************************************************************************************************/

	/* Some fields : one on nodes : double , one on cells : double */

	char champnode[MED_TAILLE_NOM+1]="fieldnodedouble" ;
	char champnode_comp[MED_TAILLE_PNOM+1]="X+Y+Z   " ;
	char champnode_unit[MED_TAILLE_PNOM+1]="X+Y+Z   " ;
	med_float   * fieldnodedouble;

	char champnodevector[MED_TAILLE_NOM+1]="fieldnodedoublevector" ;
	char champnodevector_comp[MED_TAILLE_PNOM*3+1]="0	1	2	" ;
	char champnodevector_unit[MED_TAILLE_PNOM*3+1]="O	1	2	" ;
	med_float   * fieldnodedoublevector;

	char champcellscalar[MED_TAILLE_NOM+1]="fieldcelldouble" ;
	char champcellscalar_comp[MED_TAILLE_PNOM+1]="X+Y+Z   " ;
	char champcellscalar_unit[MED_TAILLE_PNOM+1]="X+Y+Z   " ;
	med_float   * fieldcelldouble;

	char champcell[MED_TAILLE_NOM+1]="fieldcelldoublevector" ;
	char champcell_comp[MED_TAILLE_PNOM*3+1]="0       1       2       " ;
	char champcell_unit[MED_TAILLE_PNOM*3+1]="0       1       2       " ;
	med_float   * fieldcelldoublevector;

/*****************************************************************************************************/

	
	if (argc!=2) 
		{
		printf("Il manque un paramètre : le nombre de point par ligne\n");
		exit(-1);
		}

	sscanf(argv[1],"%d",&nnpl);
	
	printf("VERSION 2.0\n");
	printf("Traitement avec %d noeuds par ligne\n",nnpl);

	fromnnoe   = nnpl*nnpl*nnpl             ;
	fromnhexa8 = (nnpl-1)*(nnpl-1)*(nnpl-1) ;
	tonnoe     = (nnpl-1)*(nnpl-1)*(nnpl-1) ;
	tonhexa8   = (nnpl-2)*(nnpl-2)*(nnpl-2) ;



	nbr_nodes=nnpl*nnpl*nnpl;
	nbr_hexa8=(nnpl-1)*(nnpl-1)*(nnpl-1);

	fromcoo       = (med_float *) malloc(3*nbr_nodes*sizeof(med_float));		  cree_nodes	    ( fromcoo	    , nnpl, 0 );
	fromnumnoe    = (med_int *)   malloc(nbr_nodes*sizeof(med_int));		  cree_num_nodes    ( fromnumnoe    , nnpl );
	fromnufano    = (med_int *)   malloc(nbr_nodes*sizeof(med_int));		  cree_fam_nodes    ( fromnufano    , nnpl );
	fromhexa8     = (med_int *)   malloc(8*nbr_hexa8*sizeof(med_int));		  cree_Hexa8	    ( fromhexa8     , nnpl ,fromnumnoe);
	fromnomhexa8  = (char *)      malloc((nbr_hexa8*MED_TAILLE_PNOM+1)*sizeof(char)); cree_noms_mailles ( fromnomhexa8  , nnpl );
	fromnumhexa8  = (med_int *)   malloc(nbr_hexa8*sizeof(med_int));		  cree_num_mailles  ( fromnumhexa8  , nnpl );
	fromnufahexa8 = (med_int *)   malloc(nbr_hexa8*sizeof(med_int));		  cree_fam_mailles  ( fromnufahexa8 , nnpl );

	fieldnodedouble       = (med_float *) malloc(nbr_nodes*sizeof(med_float));	    cree_valeurs_champ_node	   ( fieldnodedouble	   , nnpl );
	fieldnodedoublevector = (med_float *) malloc(3*nbr_nodes*sizeof(med_float));	    cree_valeurs_champ_vector_node ( fieldnodedoublevector , nnpl );
	fieldcelldouble       = (med_float *) malloc(nbr_hexa8*sizeof(med_float));	    cree_valeurs_champ_cell	   ( fieldcelldouble	   , nnpl );
	fieldcelldoublevector = (med_float *) malloc(3*nbr_hexa8*sizeof(med_float));	    cree_valeurs_champ_vector_cell ( fieldcelldoublevector , nnpl );
	

	nbr_nodes=(nnpl-1)*(nnpl-1)*(nnpl-1);
	nbr_hexa8=(nnpl-2)*(nnpl-2)*(nnpl-2);

	tocoo       = (med_float *) malloc(3*nbr_nodes*sizeof(med_float));	        cree_nodes	  ( tocoo       , nnpl-1, 1 );
	tonumnoe    = (med_int *)   malloc(nbr_nodes*sizeof(med_int)); 		        cree_num_nodes    ( tonumnoe    , nnpl-1 );
	tonufano    = (med_int *)   malloc(nbr_nodes*sizeof(med_int)); 		        cree_fam_nodes    ( tonufano    , nnpl-1 );
	tohexa8     = (med_int *)   malloc(8*nbr_hexa8*sizeof(med_int));		cree_Hexa8	  ( tohexa8     , nnpl-1 ,tonumnoe);
	tonomhexa8  = (char *)      malloc((nbr_hexa8*MED_TAILLE_PNOM+1)*sizeof(char)); cree_noms_mailles ( tonomhexa8  , nnpl-1 );
	tonumhexa8  = (med_int *)   malloc(nbr_hexa8*sizeof(med_int)); 		        cree_num_mailles  ( tonumhexa8  , nnpl-1 );
	tonufahexa8 = (med_int *)   malloc(nbr_hexa8*sizeof(med_int)); 		        cree_fam_mailles  ( tonufahexa8 , nnpl-1 );

	
/*****************************************************************************************************/
	fromfid = MEDouvrir("fromMesh.med",MED_LECTURE_AJOUT);
	if (fromfid < 0)
		ret = -1;
	else
		ret = 0;
	printf("MEDouvrir : %d\n",ret);

/*****************************************************************************************************/
	tofid = MEDouvrir("toMesh.med",MED_LECTURE_AJOUT);
	if (tofid < 0)
		ret = -1;
	else
		ret = 0;
	printf("MEDouvrir : %d\n",ret);
/*****************************************************************************************************/
	if (ret == 0)
  		ret = MEDmaaCr(fromfid,frommaa,mdim,type,frommaadesc);
	printf("MEDmaaCr : %d\n",ret);

/*****************************************************************************************************/

	if (ret == 0)
  		ret = MEDmaaCr(tofid,tomaa,mdim,type,tomaadesc);
	printf("MEDmaaCr : %d\n",ret);

/*****************************************************************************************************/
	if (ret == 0)
  		ret = MEDnoeudsEcr(fromfid,frommaa,mdim,fromcoo,MED_FULL_INTERLACE,MED_CART,
        			     // nomcoo,unicoo,nomnoe,MED_FAUX,fromnumnoe,MED_VRAI,
				     nomcoo,unicoo,nomnoe,MED_FAUX,fromnumnoe,MED_FAUX,
	        		     fromnufano,fromnnoe);
	printf("MEDnoeudsEcr : %d\n",ret);
/*****************************************************************************************************/

	if (ret == 0)
  		ret = MEDnoeudsEcr(tofid,tomaa,mdim,tocoo,MED_FULL_INTERLACE,MED_CART,
        			     //nomcoo,unicoo,nomnoe,MED_FAUX,tonumnoe,MED_VRAI,
				     nomcoo,unicoo,nomnoe,MED_FAUX,tonumnoe,MED_FAUX,
	        		     tonufano,tonnoe);
	printf("MEDnoeudsEcr : %d\n",ret);


/*****************************************************************************************************/
/* ecriture des mailles MED_HEXA8 :
   - connectivite
   - noms (optionnel) 
   - numeros (optionnel)
   - numeros des familles */
	if (ret == 0) 
  		ret = MEDelementsEcr(fromfid,frommaa,mdim,fromhexa8,MED_FULL_INTERLACE,
        		       		fromnomhexa8,MED_FAUX,fromnumhexa8,MED_VRAI,fromnufahexa8,fromnhexa8,
        	       			MED_MAILLE,MED_HEXA8,MED_NOD);
	printf("MEDelementsEcr : %d \n",ret);

/*****************************************************************************************************/
/* ecriture des mailles MED_HEXA8 :
   - connectivite
   - noms (optionnel) 
   - numeros (optionnel)
   - numeros des familles */

	if (ret == 0) 
  		ret = MEDelementsEcr(tofid,tomaa,mdim,tohexa8,MED_FULL_INTERLACE,
        	       			tonomhexa8,MED_FAUX,tonumhexa8,MED_VRAI,tonufahexa8,tonhexa8,
		        	        MED_MAILLE,MED_HEXA8,MED_NOD);
	printf("MEDelementsEcr : %d \n",ret);

/*****************************************************************************************************/
/* ecriture des familles */
/* Conventions :
   - toujours creer une famille de numero 0 ne comportant aucun attribut
     ni groupe (famille de reference pour les noeuds ou les elements
     qui ne sont rattaches a aucun groupe ni attribut)
   - les numeros de familles de noeuds sont > 0
   - les numeros de familles des elements sont < 0
   - rien d'imposer sur les noms de familles
 */ 

/* la famille 0 */
	if (ret == 0)
  		{
    		strcpy(nomfam,"FAMILLE_0");
		numfam = 0;
    		ret = MEDfamCr(fromfid,frommaa,nomfam,numfam,&attide,&attval,attdes,0,gro,0);
  		}
	printf("MEDfamCr : %d \n",ret);
/*****************************************************************************************************/

	if (ret == 0)
  		{
    		strcpy(nomfam,"FAMILLE_0");
    		numfam = 0;
    		ret = MEDfamCr(tofid,tomaa,nomfam,numfam,&attide,&attval,attdes,0,gro,0);
  		}
	printf("MEDfamCr : %d \n",ret);

/*****************************************************************************************************/
/* Les champs */

	if (ret == 0)
  		{
		ret = MEDchampCr(fromfid,champnode,MED_FLOAT64,champnode_comp,champnode_unit,1); 
		printf("MEDchampCr : %d \n",ret); 
    		if (ret == 0) 
			{
      			ret = MEDchampEcr(fromfid, frommaa, champnode, (unsigned char *)fieldnodedouble,
        					MED_FULL_INTERLACE, fromnnoe,
        					MED_NOGAUSS, MED_ALL, MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 
        					0, MED_NOPDT,"        ", 0. , MED_NONOR);
      			printf("MEDchampEcr : %d \n",ret);
    			}
  		}

	if (ret == 0)
  		{
    		ret = MEDchampCr(fromfid,champcell,MED_FLOAT64,champcell_comp,champcell_unit,3);
    		printf("MEDchampCr : %d \n",ret);
    		if (ret == 0) 
			{
      			ret = MEDchampEcr(fromfid, frommaa, champcell, (unsigned char *)fieldcelldoublevector,
        					MED_FULL_INTERLACE, fromnhexa8,
        					MED_NOGAUSS, MED_ALL, MED_NOPFL, MED_NO_PFLMOD, MED_MAILLE, 
        					MED_HEXA8, MED_NOPDT,"        ", 0., MED_NONOR);
      			printf("MEDchampEcr : %d \n",ret);
    			}
  		}

	if (ret == 0)
  		{
		ret = MEDchampCr(fromfid,champcellscalar,MED_FLOAT64,champcellscalar_comp,champcellscalar_unit,1); 
		printf("MEDchampCr : %d \n",ret); 
    		if (ret == 0) 
			{
      			ret = MEDchampEcr(fromfid, frommaa, champcellscalar, (unsigned char *)fieldcelldouble,
        					MED_FULL_INTERLACE, fromnhexa8,
        					MED_NOGAUSS, MED_ALL, MED_NOPFL, MED_NO_PFLMOD, MED_MAILLE, 
        					MED_HEXA8, MED_NOPDT,"        ", 0., MED_NONOR);
      			printf("MEDchampEcr : %d \n",ret);
    			}
  		}

	if (ret == 0)
  		{
    		ret = MEDchampCr(fromfid,champnodevector,MED_FLOAT64,champnodevector_comp,champnodevector_unit,3);
    		printf("MEDchampCr : %d \n",ret);
    		if (ret == 0) 
			{
      			ret = MEDchampEcr(fromfid, frommaa, champnodevector, (unsigned char *)fieldnodedoublevector,
        					MED_FULL_INTERLACE, fromnnoe,
        					MED_NOGAUSS, MED_ALL, MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 
        					0, MED_NOPDT,"        ", 0. , MED_NONOR);
      			printf("MEDchampEcr : %d \n",ret);
    			}
  		}


/***************************************************************************/
ret = MEDfermer(fromfid);
printf("MEDfermer : %d\n",ret);
/***************************************************************************/
ret = MEDfermer(tofid);
printf("MEDfermer : %d\n",ret);

return 0;
}

