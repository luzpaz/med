/*
  creation d'une geometrie 3d : un cube [0,1]^3
  maillé uniformement en hexahedres reguliers;
  avec en plus une partie des faces (une partie
  des faces de la frontiere) du maillage.
  ATTENTION : 3 noeuds dans chaque direction
*/

#include <med.h>
#include <string.h>

int main (int argc, char **argv)
{
  med_err ret;
  med_idt fid;
  char maa[MED_TAILLE_NOM+1] = "CUBE_EN_HEXA8_QUAD4_WRONG";
  char maadesc[MED_TAILLE_DESC+1] = "Example de maillage non structure 3D";
  med_int mdim = 3;
  med_int nnoe = 27;
  /*
    les noeuds:
  */
  med_float coo[81] = {
    0.0, 0.0, 0.0, 
    0.5, 0.0, 0.0, 
    1.0, 0.0, 0.0, 
    0.0, 0.5, 0.0, 
    0.5, 0.5, 0.0, 
    1.0, 0.5, 0.0, 
    0.0, 1.0, 0.0, 
    0.5, 1.0, 0.0, 
    1.0, 1.0, 0.0, 
    0.0, 0.0, 0.5, 
    0.5, 0.0, 0.5, 
    1.0, 0.0, 0.5, 
    0.0, 0.5, 0.5, 
    0.5, 0.5, 0.5, 
    1.0, 0.5, 0.5, 
    0.0, 1.0, 0.5, 
    0.5, 1.0, 0.5, 
    1.0, 1.0, 0.5, 
    0.0, 0.0, 1.0, 
    0.5, 0.0, 1.0, 
    1.0, 0.0, 1.0, 
    0.0, 0.5, 1.0, 
    0.5, 0.5, 1.0, 
    1.0, 0.5, 1.0, 
    0.0, 1.0, 1.0, 
    0.5, 1.0, 1.0, 
    1.0, 1.0, 1.0
  };
  char nomcoo[3*MED_TAILLE_PNOM+1] = "x               y               z               ";
  char unicoo[3*MED_TAILLE_PNOM+1] = "cm              cm              cm              ";
  /*  char nomnoe[19*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4";*/
  char *nomnoe ;
  med_int numnoe[27] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
  med_int nufano[27] = {1,1,1,1,1,1,1,1,1,3,3,3,3,0,3,3,3,3,2,2,2,2,2,2,2,2,2};
  /*
    les elements:
  */
  med_int nhexa8 = 8;
  med_int hexa8[64] = {
    4, 13, 14, 5, 1, 10, 11, 2,
    5, 14, 15, 6, 2, 11, 12, 3,
    7, 16, 17, 8, 4, 13, 14, 5,
    8, 17, 18, 9, 5, 14, 15, 6,
    13, 22, 23, 14, 10, 19, 20, 11,
    14, 23, 24, 15, 11, 20, 21, 12,
    16, 25, 26, 17, 13, 22, 23, 14,
    17, 26, 27, 18, 14, 23, 24, 15
  };
  char nomhexa8[MED_TAILLE_PNOM*8+1] = "hexa1           hexa2           hexa3           hexa4           hexa5           hexa6           hexa7           hexa8           ";
  med_int numhexa8[8] = {1,2,3,4,5,6,7,8};
  med_int nufahexa8[8] = {-1,-1,-1,-1,-2,-2,-2,-2};

  /*
    Les Faces qui dans ce cas (2D) sont des arretes

    a face is wrongly oriented, it is just to test the applidation
    duplicateMEDMESH
  */

  med_int nquad4 = 8;
  med_int quad4[32] = {
    20, 21, 24, 23,
    22, 23, 26, 25,
    19, 20, 23, 22,
    23, 24, 27, 26,
    /*1 , 4, 5, 2,*/
    1 , 2, 5, 4,
    5, 8, 9, 6, 
    2, 5, 6, 3, 
    7, 8, 5, 4
  };

  char nomquad4[MED_TAILLE_PNOM*8+1] = "quad1           quad2           quad3           quad4           quad5           quad6           quad7           quad8          ";
  med_int numquad4[8] = {1,2,3,4,5,6,7,8};
  med_int nufaquad4[8] = {-3,-3,-3,-3,-4, -4, -4 , -4};

  char nomfam[MED_TAILLE_NOM+1];
  med_int numfam;
  char attdes[MED_TAILLE_DESC+1];
  med_int natt;
  med_int attide;
  med_int attval;
  med_int ngro;
  char gro[MED_TAILLE_LNOM+1];
  int i;
  int nfame = 1; 
  int nfamn = 1;
  int nfamf = 1;

  /*
    Some fields : 2 on nodes : one int and one double , one on cells : double
  */

  char champ1[MED_TAILLE_NOM+1]="fieldnodeint" ;
  char champ1_comp[MED_TAILLE_PNOM+1]="comp1           " ;
  char champ1_unit[MED_TAILLE_PNOM+1]="M               " ;
  med_int fieldnodeint[27] = {1,1,3,2,2,3,4,4,5,1,1,3,2,2,3,4,4,5,1,1,3,2,2,3,4,4,5};

  char champ2[MED_TAILLE_NOM+1]="fieldnodedouble" ;
  char champ2_comp[MED_TAILLE_PNOM+1]="comp1           " ;
  char champ2_unit[MED_TAILLE_PNOM+1]="J               " ;
  med_float fieldnodedouble1[27] = {1.,3.,4.,1.,3.,4.,3.,2.,5.,1.,3.,4.,1.,3.,4.,3.,2.,5.,1.,3.,4.,1.,3.,4.,3.,2.,5.};
  med_float fieldnodedouble2[27] = {1.,2.,2.,3.,3.,3.,4.,4.,5.,1.,2.,2.,3.,3.,3.,4.,4.,5.,1.,2.,2.,3.,3.,3.,4.,4.,5.};

  char champ3[MED_TAILLE_NOM+1]="fieldcelldouble" ;
  char champ3_comp[MED_TAILLE_PNOM*3+1]="comp1           comp2           comp3           " ;
  char champ3_unit[MED_TAILLE_PNOM*3+1]="M/S             m/s             m/s             " ;
  med_float fieldcelldouble[8*3] = {0.,1.,1.,1.,1.,2.,2.,3.,0.,1.,1.,1.,1.,2.,2.,3.,0.,1.,1.,1.,1.,2.,2.,3.};

  /***************************************************************************/
  fid = MEDouvrir("cube_hexa8_quad4_wrong.med",MED_LECTURE_ECRITURE);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("MEDouvrir : %d\n",ret);

  /***************************************************************************/
  if (ret == 0)
    ret = MEDmaaCr(fid,maa,mdim,MED_NON_STRUCTURE,maadesc);
  printf("MEDmaaCr : %d\n",ret);
  if (ret == 0)
    ret = MEDunvCr(fid,maa);
  printf("MEDunvCr : %d\n",ret);

  /***************************************************************************/
  if (ret == 0)
    ret = MEDnoeudsEcr(fid,maa,mdim,coo,MED_FULL_INTERLACE,MED_CART,
		       nomcoo,unicoo,nomnoe,MED_FAUX,numnoe,MED_VRAI,
		       nufano,nnoe);
  printf("MEDnoeudsEcr : %d\n",ret);

  /* ecriture des mailles MED_HEXA8 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,hexa8,MED_FULL_INTERLACE,
			 nomhexa8,MED_FAUX,numhexa8,MED_VRAI,nufahexa8,nhexa8,
			 MED_MAILLE,MED_HEXA8,MED_NOD);
  printf("MEDelementsEcr : %d \n",ret);

  /* ecriture des mailles MED_QUAD4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,quad4,MED_FULL_INTERLACE,
			 nomquad4,MED_FAUX,numquad4,MED_VRAI,nufaquad4,nquad4,
			 MED_FACE,MED_QUAD4,MED_NOD);
  printf("MEDelementsEcr : %d \n",ret);

  /***************************************************************************/
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
      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,0,
		     gro,0);
    }
  printf("%d \n",ret);

  /* on cree :
      - 2 familles d'elements (-1,-2) et 
      - 3 familles de noeuds (1,2,3) 
      - 1 famille(s) d'elements de dimension (d-1)
        en fait de face (-3)
  */

  if (ret == 0)
    {
      nfame = 2;
      for (i=0;i<nfame;i++)
	{
	  if (ret == 0)
	    {
	      strcpy(nomfam,"FAMILLE_ELEMENT_");
	      numfam = -(i+1);
	      sprintf(nomfam,"%s%d",nomfam,-numfam);
	      attide = 1;
	      attval = numfam*100;
              natt = 1;
	      strcpy(attdes,"description attribut");
              strcpy(gro,"groupe1");
	      ngro = 1;

              /*printf("nomfam : %s - numfam : %d - attide : %d - attval : %d - ngro : %d \n",nomfam,numfam,attide,attval,ngro);*/

	      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
			     natt,gro,ngro);
              printf("MEDfamCr : %d\n",ret);
	    }
	}
    }
  
  if (ret == 0)
    {
      nfamn = 3;
      for (i=0;i<nfamn;i++)
	{
	  if(ret == 0)
	    {
	      strcpy(nomfam,"FAMILLE_NOEUD_");
	      numfam = i+1;
	      sprintf(nomfam,"%s%d",nomfam,numfam);
	      attide = 1;
	      attval = numfam*100;
              natt = 1;
	      strcpy(attdes,"description attribut");
              strcpy(gro,"groupe2");
	      ngro = 1;
	      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
			     natt,gro,ngro);
              printf("MEDfamCr : %d\n",ret);
	    }
	}
    }

  if (ret == 0)
    {
      nfamf = 2;
      for (i=0;i<nfamf;i++)
	{
	  if (ret == 0)
	    {
	      strcpy(nomfam,"FAMILLE_FACE_");
	      numfam = -(i+3);
	      sprintf(nomfam,"%s%d",nomfam,-numfam);
	      attide = 1;
	      attval = numfam*100;
              natt = 1;
	      strcpy(attdes,"description attribut");
              strcpy(gro,"groupe3");
	      ngro = 1;
              /*printf("nomfam : %s - numfam : %d - attide : %d - attval : %d - ngro : %d \n",nomfam,numfam,attide,attval,ngro);*/
	      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
			     natt,gro,ngro);
              printf("MEDfamCr : %d\n",ret);
	    }
	}
    }

  /***************************************************************************/
  /*
    Les champs
  */
  if (ret == 0)
    {
      ret = MEDchampCr(fid,champ1,MED_INT32,champ1_comp,champ1_unit,1);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
	ret = MEDchampEcr(fid, maa, champ1, (unsigned char *)fieldnodeint,
			  MED_FULL_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
			  MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 0,
			  MED_NOPDT,"        ", 0., MED_NONOR);
	
	printf("MEDchampEcr : %d \n",ret);
      }
    }
  
  if (ret == 0)
    {
      ret = MEDchampCr(fid,champ2,MED_FLOAT64,champ2_comp,champ2_unit,1);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
	ret = MEDchampEcr(fid, maa, champ2, (unsigned char *)fieldnodedouble1,
			  MED_FULL_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
			  MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 0,
			  1,"S       ", 1.1 , MED_NONOR);
	printf("MEDchampEcr1 : %d \n",ret);
	ret = MEDchampEcr(fid, maa, champ2, (unsigned char *)fieldnodedouble2,
			  MED_FULL_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
			  MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 0,
			  2,"S       ", 1.2 , MED_NONOR);
	printf("MEDchampEcr2 : %d \n",ret);
      }
    }
  
  /* on met champ2 sans pas de temps pour pouvoir le lire aussi par defaut !*/
  if (ret == 0)
    {
      ret = MEDchampEcr(fid, maa, champ2, (unsigned char *)fieldnodedouble1,
			MED_FULL_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
			MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 0,
			MED_NOPDT,"        ", 0. , MED_NONOR);
      printf("MEDchampEcr : %d \n",ret); 
    }

  if (ret == 0)
    {
      ret = MEDchampCr(fid,champ3,MED_FLOAT64,champ3_comp,champ3_unit,3);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
	ret = MEDchampEcr(fid, maa, champ3, (unsigned char *)fieldcelldouble,
			  MED_FULL_INTERLACE, nhexa8, MED_NOGAUSS, MED_ALL,
			  MED_NOPFL, MED_NO_PFLMOD, MED_MAILLE, MED_HEXA8,
			  MED_NOPDT,"        ", 0., MED_NONOR);
	printf("MEDchampEcr : %d \n",ret);
      }
    }

  /***************************************************************************/

  ret = MEDfermer(fid);
  printf("%d\n",ret);
  
  return 0;
}
