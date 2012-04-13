// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

/*
  creation d'une geometrie 3d : un cube [0,1]^3
  maillé uniformement en hexahedres reguliers;
  avec en plus une partie des faces (une partie
  des faces de la frontiere) du maillage.
  ATTENTION : 3 noeuds dans chaque direction
*/

#include <med.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
  med_err ret;
  med_idt fid;
  char maa[MED_NAME_SIZE+1] = "CUBE_EN_HEXA8_QUAD4_WRONG";
  char maadesc[MED_COMMENT_SIZE+1] = "Example de maillage non structure 3D";
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
  char nomcoo[3*MED_SNAME_SIZE+1] = "x               y               z               ";
  char unicoo[3*MED_SNAME_SIZE+1] = "cm              cm              cm              ";
  /*  char nomnoe[19*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4";*/
  //char *nomnoe ;
//  med_int numnoe[27] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
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
 // char nomhexa8[MED_TAILLE_PNOM*8+1] = "hexa1           hexa2           hexa3           hexa4           hexa5           hexa6           hexa7           hexa8           ";
 // med_int numhexa8[8] = {1,2,3,4,5,6,7,8};
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

 // char nomquad4[MED_TAILLE_PNOM*8+1] = "quad1           quad2           quad3           quad4           quad5           quad6           quad7           quad8          ";
 // med_int numquad4[8] = {1,2,3,4,5,6,7,8};
  med_int nufaquad4[8] = {-3,-3,-3,-3,-4, -4, -4 , -4};

  char nomfam[MED_NAME_SIZE+1];
  med_int numfam;
  //char attdesMED_TAILLE_DESC+1];
  med_int natt;
  med_int attide;
  med_int attval;
  med_int ngro;
  char gro[MED_LNAME_SIZE+1];
  char dtunitp3[MED_LNAME_SIZE+1]="";
  int i;
  int nfame = 1; 
  int nfamn = 1;
  int nfamf = 1;

  /*
    Some fields : 2 on nodes : one int and one double , one on cells : double
  */

  char champ1[MED_NAME_SIZE+1]="fieldnodeint" ;
  char champ1_comp[MED_SNAME_SIZE+1]="comp1           " ;
  char champ1_unit[MED_SNAME_SIZE+1]="M               " ;
  med_int fieldnodeint[27] = {1,1,3,2,2,3,4,4,5,1,1,3,2,2,3,4,4,5,1,1,3,2,2,3,4,4,5};

  char champ2[MED_NAME_SIZE+1]="fieldnodedouble" ;
  char champ2_comp[MED_SNAME_SIZE+1]="comp1           " ;
  char champ2_unit[MED_SNAME_SIZE+1]="J               " ;
  med_float fieldnodedouble1[27] = {1.,3.,4.,1.,3.,4.,3.,2.,5.,1.,3.,4.,1.,3.,4.,3.,2.,5.,1.,3.,4.,1.,3.,4.,3.,2.,5.};
  med_float fieldnodedouble2[27] = {1.,2.,2.,3.,3.,3.,4.,4.,5.,1.,2.,2.,3.,3.,3.,4.,4.,5.,1.,2.,2.,3.,3.,3.,4.,4.,5.};

  char champ3[MED_NAME_SIZE+1]="fieldcelldouble" ;
  char champ3_comp[MED_SNAME_SIZE*3+1]="comp1           comp2           comp3           " ;
  char champ3_unit[MED_SNAME_SIZE*3+1]="M/S             m/s             m/s             " ;
  med_float fieldcelldouble[8*3] = {0.,1.,1.,1.,1.,2.,2.,3.,0.,1.,1.,1.,1.,2.,2.,3.,0.,1.,1.,1.,1.,2.,2.,3.};

  /***************************************************************************/
  fid = MEDfileOpen("cube_hexa8_quad4_wrong.med",MED_ACC_RDWR);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("MEDouvrir : %d\n",ret);

  /***************************************************************************/
  if (ret == 0)
    ret = MEDmeshCr(fid,maa,mdim,mdim,MED_UNSTRUCTURED_MESH,maadesc,dtunitp3,
                    MED_SORT_DTIT,MED_CARTESIAN,nomcoo,unicoo);
  printf("MEDmaaCr : %d\n",ret);
  if (ret == 0)
    ret = MEDunvCr(fid,maa);
  printf("MEDunvCr : %d\n",ret);

  /***************************************************************************/
  if (ret == 0)
    ret = MEDmeshNodeCoordinateWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,MED_FULL_INTERLACE,nnoe,coo);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NODE,0,nnoe,nufano);
  printf("MEDnoeudsEcr : %d\n",ret);

  /* ecriture des mailles MED_HEXA8 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDmeshElementConnectivityWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                       MED_CELL,MED_HEXA8,MED_NODAL,MED_FULL_INTERLACE,
                                       nhexa8,hexa8);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_CELL,MED_HEXA8,nhexa8,nufahexa8);
  printf("MEDelementsEcr : %d \n",ret);

  /* ecriture des mailles MED_QUAD4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDmeshElementConnectivityWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                       MED_CELL,MED_QUAD4,MED_NODAL,MED_FULL_INTERLACE,
                                       nquad4,quad4);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_CELL,MED_QUAD4,nquad4,nufaquad4);
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
      ret = MEDfamilyCr(fid,maa,nomfam,numfam,0,gro);
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
              //strcpy(attdes,"description attribut");
              strcpy(gro,"groupe1");
              ngro = 1;

              /*printf("nomfam : %s - numfam : %d - attide : %d - attval : %d - ngro : %d \n",nomfam,numfam,attide,attval,ngro);*/

              ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
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
              //strcpy(attdes,"description attribut");
              strcpy(gro,"groupe2");
              ngro = 1;
              ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
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
              //strcpy(attdes,"description attribut");
              strcpy(gro,"groupe3");
              ngro = 1;
              /*printf("nomfam : %s - numfam : %d - attide : %d - attval : %d - ngro : %d \n",nomfam,numfam,attide,attval,ngro);*/
              ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
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
      ret = MEDfieldCr(fid,champ1,MED_INT32,1,champ1_comp,champ1_unit,dtunitp3,maa);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ1,MED_NO_DT,MED_NO_IT,0.,MED_NODE,0,MED_FULL_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodeint);
        
        printf("MEDchampEcr : %d \n",ret);
      }
    }
  
  if (ret == 0)
    {
      ret = MEDfieldCr(fid,champ2,MED_FLOAT64,1,champ2_comp,champ2_unit,dtunitp3,maa);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ2,1,MED_NO_IT,1.1 ,MED_NODE,0,MED_FULL_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodedouble1);
        printf("MEDchampEcr1 : %d \n",ret);
        ret = MEDfieldValueWr(fid, champ2,2,MED_NO_IT,1.2 ,MED_NODE,0,MED_FULL_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodedouble2);
        printf("MEDchampEcr2 : %d \n",ret);
      }
    }
  
  /* on met champ2 sans pas de temps pour pouvoir le lire aussi par defaut !*/
  if (ret == 0)
    {
      ret = MEDfieldValueWr(fid, champ2,MED_NO_DT,MED_NO_IT,0. ,MED_NODE,0,MED_FULL_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodedouble1);
      printf("MEDchampEcr : %d \n",ret); 
    }

  if (ret == 0)
    {
      ret = MEDfieldCr(fid,champ3,MED_FLOAT64,3,champ3_comp,champ3_unit,dtunitp3,maa);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ3,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_HEXA8,MED_FULL_INTERLACE,MED_ALL_CONSTITUENT,nhexa8, (unsigned char *)fieldcelldouble);
        printf("MEDchampEcr : %d \n",ret);
      }
    }

  /***************************************************************************/

  ret = MEDfermer(fid);
  printf("%d\n",ret);
  
  if ( getenv("srcdir") ) 
    /* we are in 'make check' */
    remove( "cube_hexa8_quad4_wrong.med" );

  return 0;
}
