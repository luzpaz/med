/*----------------------------------------------------------------------------
MED MEDMEM : MED files in memory

 Copyright (C) 2003  CEA/DEN, EDF R&D



  File   : create_grid.c
Module : MED
----------------------------------------------------------------------------*/

/******************************************************************************
 * - Nom du fichier : test19.c
 *
 * - Description : ecriture des objets MED relatifs aux grilles
 *                 MED V2.0
 *
 *****************************************************************************/

#include <med.h>
#include <string.h>

/******************************************************************************
 *    - creation d'un fichier MED
 *    - ecriture des noeuds : coordonnees en mode MED_FULL_INTERLACE,
 *                            noms,numeros,numeros de familles
 *    - ecriture des familles d'un maillage MED 
 *      2.0 via les routines de bas niveau
 *    - fermeture du fichier
 *****************************************************************************/

int main (int argc, char **argv)
{

/* Ecriture d'un premier maillage non structure (test14 + test8)
   ************************************************************* */

  med_err ret;
  med_idt fid;
  /* la dimension du maillage */
  med_int mdim = 2;
  /* nom du maillage de longueur maxi MED_TAILLE_NOM */
  char maa[MED_TAILLE_NOM+1] = "maa1";
  /* le nombre de noeuds */
  med_int nnoe = 4;
  /* table des coordonnees  
     profil : (dimension * nombre de noeuds) */
  med_float coo[8] = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
  med_int nbr[2] = {2, 2};
  /* tables des noms et des unites des coordonnees 
     profil : (dimension*MED_TAILLE_PNOM+1) */
  char nomcoo[2*MED_TAILLE_PNOM+1] = "x       y       ";
  char unicoo[2*MED_TAILLE_PNOM+1] = "cm      cm      ";
  /* tables des noms, numeros, numeros de familles des noeuds
     autant d'elements que de noeuds - les noms ont pout longueur
     MED_TAILLE_PNOM */
  char nomnoe[4*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4    ";
  med_int numnoe[4] = {1,2,3,4};
  med_int nufano[4] = {0,1,2,2};
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
  int nfamn = 2;
  med_int fam[16];
  /*
    les elements:
  */
  med_int nquad4 = 1;
  med_int quad4[4] = {
    1, 2, 4, 3
  };
  char nomquad4[MED_TAILLE_PNOM*1+1] = "quad1   ";
  med_int numquad4[1] = {1};
  med_int nufaquad4[1] = {-1};

  fid = MEDouvrir("test19.med",MED_REMP);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("%d\n",ret);

  /* creation du maillage maa de dimension 2 */
  if (ret == 0)
    ret = MEDmaaCr(fid,maa,mdim);
  printf("%d\n",ret);

  /* ecriture des noeuds d'un maillage MED : 
     - des coo en mode MED_FULL_INTERLACE : (X1,Y1,X2,Y2,X3,Y3,...) 
       dans un repere cartesien 
     - des noms (optionnel dans un fichier MED) 
     - des numeros (optionnel dans un fichier MED) 
     - des numeros de familles des noeuds */	      
  if (ret == 0)
    ret = MEDnoeudsEcr(fid,maa,mdim,coo,MED_FULL_INTERLACE,MED_CART,
		       nomcoo,unicoo,nomnoe,MED_VRAI,numnoe,MED_VRAI,
		       nufano,nnoe,MED_ECRI);
  printf("%d\n",ret);

  /* ecriture des mailles MED_QUAD4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,quad4,MED_FULL_INTERLACE,
			 nomquad4,MED_FAUX,numquad4,MED_VRAI,nufaquad4,nquad4,
			 MED_MAILLE,MED_QUAD4,MED_NOD,MED_ECRI);
  printf("%d \n",ret);

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

  /* on cree pour correspondre aux cas tests precedents, 3 familles
     d'elements (-1,-2,-3) et deux familles de noeuds (1,2) */
  if (ret == 0)
    {
      nfame = 1;
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
              printf("%s - %d - %d - %d - %d \n",nomfam,numfam,attide,attval,
		     ngro);
	      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
			     natt,gro,ngro);
              printf("MEDfamCr (elements) : %d\n",ret);
	    }
	}
    }
  
    if (ret == 0)
    {
      nfamn = 2;
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
              strcpy(gro,"groupe1");
	      ngro = 1;
              printf("%s - %d - %d - %d - %d \n",nomfam,numfam,attide,attval,
		     ngro);
	      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
			     natt,gro,ngro);
              printf("MEDfamCr (nodes) : %d\n",ret);
	    }
	}
    }


  /* fermeture du fichier */
  ret = MEDfermer(fid);
  printf("%d\n",ret);

/* Ecriture d'un deuxieme maillage structure : body fitted
   ******************************************************* */

  fid = MEDouvrir("test19.med",MED_ECRI);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("%d\n",ret);

  strcpy(maa, "bodyfitted");
  /* creation du maillage body fitted maa de dimension 2 */
  if (ret == 0)
    ret = MEDgridCr(fid, maa, mdim, MED_BODY_FITTED);
  printf("%d\n",ret);

  /* ecriture des noeuds d'un maillage MED : 
     - des coo en mode MED_FULL_INTERLACE : (X1,Y1,X2,Y2,X3,Y3,...) 
       dans un repere cartesien 
     - des noms (optionnel dans un fichier MED) 
     - des numeros (optionnel dans un fichier MED) 
     - des numeros de familles des noeuds */	      
  if (ret == 0)
    ret = MEDbodyFittedEcr(fid,maa,mdim,coo,nbr,MED_FULL_INTERLACE,MED_CART,
		       nomcoo,unicoo,nufano,nnoe,MED_ECRI);
  printf("%d\n",ret);

  /* la famille 0 */
  if (ret == 0)
    {
      strcpy(nomfam,"FAMILLE_0");
      numfam = 0;
      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,0,
		     gro,0);
    }
  printf("%d \n",ret);



    if (ret == 0)
    {
      nfamn = 2;
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
              strcpy(gro,"groupe1");
	      ngro = 1;
              printf("%s - %d - %d - %d - %d \n",nomfam,numfam,attide,attval,
		     ngro);
	      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
			     natt,gro,ngro);
              printf("MEDfamCr (nodes) : %d\n",ret);
	    }
	}
    }




  /* fermeture du fichier */
  ret = MEDfermer(fid);
  printf("%d\n",ret);

/* Ecriture d'un troisieme maillage structure : grille cartesienne
   *************************************************************** */

  fid = MEDouvrir("test19.med", MED_ECRI);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("%d\n",ret);

  strcpy(maa, "CartGrid");
  /* creation d'une grille cartesienne maa de dimension 2 */
  if (ret == 0)
    ret = MEDgridCr(fid, maa, mdim, MED_CARTESIAN);
  printf("%d\n",ret);

    /* Ecriture des indices de la grille cartesienne :
       - des coo en mode MED_FULL_INTERLACE : (X1,Y1,X2,Y2,X3,Y3,...) */
    for (i=0; i<mdim; i++) {
        coo[0] = 1.1+i;
        coo[1] = 1.2+i;
        coo[2] = 1.3+i;
        coo[3] = 1.4+i;
        if (ret == 0) {
            ret = MEDgridEcr(fid, maa, mdim, coo, nnoe, i, MED_FULL_INTERLACE, MED_CART, nomcoo, unicoo, MED_ECRI);
        };
    };
    printf("%d\n",ret);

    /* Ecriture des familles de la grille cartesienne */
    if (ret == 0) {
        fam[ 0]=3; fam[ 1]=3; fam[ 2]=2; fam[ 3]=1;
        fam[ 4]=7; fam[ 5]=8; fam[ 6]=2; fam[ 7]=4;
        fam[ 8]=2; fam[ 9]=9; fam[10]=0; fam[11]=2;
        fam[12]=5; fam[13]=5; fam[14]=6; fam[15]=7;
        ret = MEDfamGridEcr(fid, maa, fam, 16, MED_ECRI,MED_NOEUD);
    };


  /* la famille 0 */
  if (ret == 0)
    {
      strcpy(nomfam,"FAMILLE_0");
      numfam = 0;
      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,0,
		     gro,0);
    }
  printf("%d \n",ret);


    if (ret == 0)
    {
      nfamn = 9;
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
              strcpy(gro,"groupe1");
	      ngro = 1;
              printf("%s - %d - %d - %d - %d \n",nomfam,numfam,attide,attval,
		     ngro);
	      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
			     natt,gro,ngro);
              printf("MEDfamCr (nodes) : %d\n",ret);
	    }
	}
    }


  /* fermeture du fichier */
  ret = MEDfermer(fid);
  printf("%d\n",ret);

  return 0;
}
