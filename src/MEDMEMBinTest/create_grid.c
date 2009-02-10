//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*----------------------------------------------------------------------------
  MED MEDMEM : MED files in memory

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
    ************************************************************** */

  med_err ret;
  med_idt fid;
  /* la dimension du maillage */
  med_int mdim = 2;
  /* nom du maillage de longueur maxi MED_TAILLE_NOM */
  char maa[MED_TAILLE_NOM+1] = "maa1";
  /* description du maillage de longueur maxi MED_TAIIL_DESC */
  char maadesc[MED_TAILLE_DESC+1] = "Example de maillage structure 2D";
  /* le nombre de noeuds */
  med_int nnoe = 4;
  /* table des coordonnees  
     profil : (dimension * nombre de noeuds) */
  med_float coo[8] = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
  med_int nbr[2] = {2, 2};
  /* tables des noms et des unites des coordonnees 
     profil : (dimension*MED_TAILLE_PNOM+1) */
  char nomcoo[2*MED_TAILLE_PNOM+1] = "x               y               ";
  char unicoo[2*MED_TAILLE_PNOM+1] = "cm              cm              ";

  char nomcooi[MED_TAILLE_PNOM+1] = "x               ";
  char unicooi[MED_TAILLE_PNOM+1] = "cm              ";
  /* tables des noms, numeros, numeros de familles des noeuds
     autant d'elements que de noeuds - les noms ont pout longueur
     MED_TAILLE_PNOM */
  char nomnoe[4*MED_TAILLE_PNOM+1] = "nom1            nom2            nom3            nom4            ";
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
  int i, ip1;
  int nfame = 1; 
  int nfamn = 2;
  med_int famNodeStd[4];
  med_int famElmtStd[1];
  med_int famFaceStd[4];
  med_int famNodeCart[16];
  med_int famElmtCart[9];
  med_int famFaceCart[24];
  /*
    les elements:
  */
  med_int nquad4 = 1;
  med_int quad4[4] = {
    1, 2, 4, 3
  };
  char nomquad4[MED_TAILLE_PNOM*1+1] = "quad1           ";
  med_int numquad4[1] = {1};
  med_int nufaquad4[1] = {-1};

  fid = MEDouvrir("test19.med",MED_LECTURE_ECRITURE);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("MEDouvrir : %d\n",ret);

  /* creation du maillage maa de dimension 2 */
  if (ret == 0)
    ret = MEDmaaCr(fid,maa,mdim,MED_NON_STRUCTURE,maadesc);
  printf("MEDmaaCr : %d\n",ret);

  /* ecriture des noeuds d'un maillage MED : 
     - des coo en mode MED_FULL_INTERLACE : (X1,Y1,X2,Y2,X3,Y3,...) 
     dans un repere cartesien 
     - des noms (optionnel dans un fichier MED) 
     - des numeros (optionnel dans un fichier MED) 
     - des numeros de familles des noeuds */	      
  if (ret == 0)
    ret = MEDnoeudsEcr(fid,maa,mdim,coo,MED_FULL_INTERLACE,MED_CART,
		       nomcoo,unicoo,nomnoe,MED_VRAI,numnoe,MED_VRAI,
		       nufano,nnoe);
  printf("MEDnoeudsEcr : %d\n",ret);

  /* ecriture des mailles MED_QUAD4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,quad4,MED_FULL_INTERLACE,
			 nomquad4,MED_FAUX,numquad4,MED_VRAI,nufaquad4,nquad4,
			 MED_MAILLE,MED_QUAD4,MED_NOD);
  printf("MEDelementsEcr : %d\n",ret);

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
  printf("MEDfamCr : %d \n",ret);

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
  printf("MEDfermer : %d\n",ret);

  /* Ecriture d'un deuxieme maillage structure : grille standard
    ************************************************************* */

  fid = MEDouvrir("test19.med", MED_LECTURE_ECRITURE);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("MEDouvrir : %d\n",ret);

  strcpy(maa, "Grille Standard");
  strcpy(maadesc, "Example de maillage structure grille standard 2D");
  /* creation du maillage grille standard maa de dimension 2 */
  if (ret == 0)
    ret = MEDmaaCr(fid, maa, mdim, MED_STRUCTURE, maadesc);
  printf("MEDmaaCr : %d\n",ret);

  if (ret == 0)
    ret = MEDnatureGrilleEcr(fid, maa, MED_GRILLE_STANDARD);
  printf("MEDnatureGrilleEcr : %d\n",ret);

  /* ecriture des noeuds d'un maillage MED : 
     - des coo en mode MED_FULL_INTERLACE : (X1,Y1,X2,Y2,X3,Y3,...) 
     dans un repere cartesien 
     - des noms (optionnel dans un fichier MED) 
     - des numeros (optionnel dans un fichier MED) 
     - des numeros de familles des noeuds */	      
/*   if (ret == 0) */
/*     ret = MEDstructureCoordEcr(fid,maa,mdim,nbr); */
/*   printf("MEDstructureCoordEcr : %d\n",ret); */

  if (ret == 0)
    ret = MEDcoordEcr(fid,maa,mdim,coo,MED_FULL_INTERLACE,nnoe,MED_CART,
		      nomcoo,unicoo);
  printf("MEDcoordEcr : %d\n",ret);

  if (ret == 0)
    ret = MEDstructureCoordEcr(fid,maa,mdim,nbr);
  printf("MEDstructureCoordEcr : %d\n",ret);

  /* Ecriture des familles de la grille standard */

  /* les noeuds */

  if (ret == 0) {
    famNodeStd[ 0]=1; famNodeStd[ 1]=1;
    famNodeStd[ 2]=2; famNodeStd[ 3]=2;

    ret = MEDfamEcr(fid, maa, famNodeStd, 4, MED_NOEUD, 0);
  };
  printf("MEDfamEcr for Nodes : %d\n",ret);

  /* les elements */

  if (ret == 0) {
    famElmtStd[ 0]=0;

    ret = MEDfamEcr(fid, maa, famElmtStd, 1, MED_MAILLE, MED_QUAD4);
  };
  printf("MEDfamEcr for Elements : %d\n",ret);

  /* les faces/edges */

  if (ret == 0) {
    for (i=0; i<4; i++) famFaceStd[i]=0;

    ret = MEDfamEcr(fid, maa, famFaceStd, 4, MED_ARETE, MED_SEG2);
  };
  printf("MEDfamEcr for Elements : %d\n",ret);

  /* la famille 0 */
  if (ret == 0)
    {
      strcpy(nomfam,"FAMILLE_0");
      numfam = 0;
      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,0,
		     gro,0);
    }
  printf("MEDfamCr : %d\n",ret);

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
  printf("MEDfermer : %d\n",ret);

  /* Ecriture d'un troisieme maillage structure : grille cartesienne
    *************************************************************** */

  fid = MEDouvrir("test19.med", MED_LECTURE_ECRITURE);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("MEDouvrir : %d\n",ret);

  strcpy(maa, "Grille Cartesienne");
  strcpy(maadesc, "Example de maillage structure grille cartesienne 2D");
  /* creation d'une grille cartesienne maa de dimension 2 */
  if (ret == 0)
    ret = MEDmaaCr(fid, maa, mdim, MED_STRUCTURE, maadesc);
  printf("MEDmaaCr : %d\n",ret);

  if (ret == 0)
    ret = MEDnatureGrilleEcr(fid, maa, MED_GRILLE_CARTESIENNE);
  printf("MEDnatureGrilleEcr : %d\n",ret);

  /* Ecriture des indices de la grille cartesienne :
     - des coo en mode MED_FULL_INTERLACE : (X1,Y1,X2,Y2,X3,Y3,...) */
  for (i=0; i<mdim; i++) {
    ip1 = i + 1;
    coo[0] = 1.1+i;
    coo[1] = 1.2+i;
    coo[2] = 1.3+i;
    coo[3] = 1.4+i;
    if (ret == 0) {
      ret = MEDindicesCoordEcr(fid, maa, mdim, coo, nnoe, ip1,
			       nomcooi, unicooi);
    };
  };
  printf("MEDindicesCoordEcr : %d\n",ret);

  /* Ecriture des familles de la grille cartesienne */

  /* les noeuds */

  if (ret == 0) {
    famNodeCart[ 0]=3; famNodeCart[ 1]=3; famNodeCart[ 2]=2; famNodeCart[ 3]=1;
    famNodeCart[ 4]=7; famNodeCart[ 5]=8; famNodeCart[ 6]=2; famNodeCart[ 7]=4;
    famNodeCart[ 8]=2; famNodeCart[ 9]=9; famNodeCart[10]=0; famNodeCart[11]=2;
    famNodeCart[12]=5; famNodeCart[13]=5; famNodeCart[14]=6; famNodeCart[15]=7;

    ret = MEDfamEcr(fid, maa, famNodeCart, 16, MED_NOEUD, 0);
  };
  printf("MEDfamEcr for Nodes : %d\n",ret);

  /* les elements */

  if (ret == 0) {
    for(i=0; i<9; i++) famElmtCart[i]=0;

    ret = MEDfamEcr(fid, maa, famElmtCart, 9, MED_MAILLE, MED_QUAD4);
  };
  printf("MEDfamEcr for Elements : %d\n",ret);

  /* les faces/edges */

  if (ret == 0) {
    for(i=0; i<24; i++) famFaceCart[i]=0;

    ret = MEDfamEcr(fid, maa, famFaceCart, 24, MED_ARETE, MED_SEG2);
  };
  printf("MEDfamEcr for Elements : %d\n",ret);

  /* la famille 0 */
  if (ret == 0)
    {
      strcpy(nomfam,"FAMILLE_0");
      numfam = 0;
      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,0,
		     gro,0);
    }
  printf("MEDfamCr : %d \n",ret);

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
  printf("MEDfermer : %d\n",ret);

  if ( getenv("srcdir") ) {
    /* we are in 'make check' */
    remove( "test19.med" );
    printf("Remove generated file");
  }

  return 0;
}
