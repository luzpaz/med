//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

/*
  creation d'une geometrie 2d : un cube [0,1]^2
  maillé uniformement en quadrangle reguliers;
  avec en plus une partie des aretes (une partie
  des arretes de la frontiere) du maillage.
  ATTENTION : 3 noeuds dans chaque direction
*/

#include <med.h>
#include <string.h>

int main (int argc, char **argv)
{
  med_err ret;
  med_idt fid;
  char maa[MED_TAILLE_NOM+1] = "carre_en_quad4_seg2";
  char maadesc[MED_TAILLE_DESC+1] = "Example de maillage non structure 2D";
  med_int mdim = 2;
  med_int nnoe = 9;
  /*
    les noeuds:
  */
  med_float coo[18] = {
    0.0, 0.0,
    0.5, 0.0,
    1.0, 0.0,
    0.0, 0.5,
    0.5, 0.5,
    1.0, 0.5,
    0.0, 1.0,
    0.5, 1.0,
    1.0, 1.0
  };
  char nomcoo[2*MED_TAILLE_PNOM+1] = "x               y               ";
  char unicoo[2*MED_TAILLE_PNOM+1] = "cm              cm              ";
  /*  char nomnoe[19*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4";*/
  char *nomnoe ;
  med_int numnoe[9] = {1,2,3,4,5,6,7,8,9};
  med_int nufano[9] = {0,0,0,0,0,0,0,0,0};
  /*
    les elements:
  */
  med_int nquad4 = 4;
  med_int quad4[16] = {
    4, 5, 2, 1,
    5, 6, 3, 2,
    7, 8, 5, 4,
    8, 9, 6, 5
  };
  char nomquad4[MED_TAILLE_PNOM*4+1] = "quad1           quad2           quad3           quad4           ";
  med_int numquad4[4] = {1,2,3,4};
  med_int nufaquad4[4] = {-10,-10,0,0};

  /*
    Les Faces qui dans ce cas (2D) sont des arretes
  */

  med_int nseg2 = 6;
  med_int seg2[18] = {
    6, 3, 0,
    8, 9, 0,
    3, 2, 0,
    9, 6, 0,
    2, 5, 0,
    5, 8, 0
  };
  char nomseg2[MED_TAILLE_PNOM*6+1] = "seg1            seg2            seg3            seg4            seg5            seg6            ";
  med_int numseg2[6] = {1,2,3,4,5,6};
  med_int nufaseg2[6] = {-1,-2,-1,-1,-2,-2};

  char nomfam[MED_TAILLE_NOM+1];
  med_int numfam;
  char attdes[MED_TAILLE_DESC+1];
  med_int natt;
  med_int attide;
  med_int attval;
  med_int ngro;
  char gro[MED_TAILLE_LNOM+1];
  //int i;
  //int nfame = 1; 
  //int nfamn = 1;

  /*
    Some fields : 2 on nodes : one int and one double , one on cells : double
   */
  char champ1[MED_TAILLE_NOM+1]="fieldnodeint" ;
  char champ1_comp[MED_TAILLE_PNOM+1]="comp1           " ;
  char champ1_unit[MED_TAILLE_PNOM+1]="M               " ;
  med_int     fieldnodeint[9]    = {1,1,3,2,2,3,4,4,5};

  char champ2[MED_TAILLE_NOM+1]="fieldnodedouble" ;
  char champ2_comp[MED_TAILLE_PNOM+1]="comp1           " ;
  char champ2_unit[MED_TAILLE_PNOM+1]="J               " ;
  med_float   fieldnodedouble1[9] = {1.,3.,4.,1.,3.,4.,3.,2.,5.};
  med_float   fieldnodedouble2[9] = {1.,2.,2.,3.,3.,3.,4.,4.,5.};

  char champ3[MED_TAILLE_NOM+1]="fieldcelldouble" ;
  char champ3_comp[MED_TAILLE_PNOM*2+1]="comp1           comp2           " ;
  char champ3_unit[MED_TAILLE_PNOM*2+1]="M/S             m/s             " ;
  med_float   fieldcelldouble[4*2] = {0.,1.,1.,1.,1.,2.,2.,3.};

  /***************************************************************************/
  fid = MEDouvrir("carre_en_quad4_seg2.med",MED_LECTURE_ECRITURE);
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

  /* ecriture des mailles MED_QUAD4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,quad4,MED_FULL_INTERLACE,
                         nomquad4,MED_FAUX,numquad4,MED_VRAI,nufaquad4,nquad4,
                         MED_MAILLE,MED_QUAD4,MED_NOD);
  printf("MEDelementsEcr : %d \n",ret);

  /* ecriture des mailles MED_SEG2 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,seg2,MED_FULL_INTERLACE,
                         nomseg2,MED_FAUX,numseg2,MED_VRAI,nufaseg2,nseg2,
                         MED_MAILLE,MED_SEG2,MED_NOD);
  printf("%d \n",ret);

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
      - 1 familles d'elements de dimension (d-1)
        en fait de edge (-1)
      - 1 familles d'elements de dimension (d-1)
        en fait de edge (-2)
      - 1 familles d'elements de dimension (d)
        en fait de face (-10)
  */

  if (ret == 0)
    {
      numfam = -1;
      strcpy(nomfam,"FAMILLE_EDGE_");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      strcpy(attdes,"description attribut");
      strcpy(gro,"groupe1");
      ngro = 1;

      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
                     natt,gro,ngro);
      printf("MEDfamCr : %d\n",ret);
    }
  if (ret == 0)
    {
      numfam = -2;
      strcpy(nomfam,"FAMILLE_EDGE_");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      strcpy(attdes,"description attribut");
      strcpy(gro,"groupe1");
      ngro = 1;

      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
                     natt,gro,ngro);
      printf("MEDfamCr : %d\n",ret);
    }
  if (ret == 0)
    {
      numfam = -10;
      strcpy(nomfam,"FAMILLE_CELL_");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      strcpy(attdes,"description attribut");
      strcpy(gro,"groupe0");
      ngro = 1;

      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
                     natt,gro,ngro);
      printf("MEDfamCr : %d\n",ret);
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
                          MED_NO_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
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
                          MED_NO_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
                          MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 0,
                          1,"S       ", 1.1 , MED_NONOR);
        printf("MEDchampEcr1 : %d \n",ret);
        ret = MEDchampEcr(fid, maa, champ2, (unsigned char *)fieldnodedouble2,
                          MED_NO_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
                          MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 0,
                          2,"S       ", 1.2 , MED_NONOR);
        printf("MEDchampEcr2 : %d \n",ret);
      }
    }
  
  /* on met champ2 sans pas de temps pour pouvoir le lire aussi par defaut !*/
  if (ret == 0)
    {
      ret = MEDchampEcr(fid, maa, champ2, (unsigned char *)fieldnodedouble1,
                        MED_NO_INTERLACE, nnoe, MED_NOGAUSS, MED_ALL,
                        MED_NOPFL, MED_NO_PFLMOD, MED_NOEUD, 0,
                        MED_NOPDT,"        ", 0. , MED_NONOR);
      printf("MEDchampEcr : %d \n",ret); 
    }

  if (ret == 0)
    {
      ret = MEDchampCr(fid,champ3,MED_FLOAT64,champ3_comp,champ3_unit,2);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDchampEcr(fid, maa, champ3, (unsigned char *)fieldcelldouble,
                          MED_NO_INTERLACE, nquad4, MED_NOGAUSS, MED_ALL,
                          MED_NOPFL, MED_NO_PFLMOD, MED_MAILLE, MED_QUAD4,
                          MED_NOPDT,"        ", 0., MED_NONOR);
        printf("MEDchampEcr : %d \n",ret);
      }
    }
  
  /***************************************************************************/

  ret = MEDfermer(fid);
  printf("%d\n",ret);
  
  if ( getenv("srcdir") ) 
    /* we are in 'make check' */
    remove( "carre_en_quad4_seg2.med" );

  return 0;
}
