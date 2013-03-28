// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
  creation d'une geometrie 2d : un cube [0,1]^2
  maill� uniformement en quadrangle reguliers;
  avec n (=argv[1]) noeuds dans chaque direction.
  2 champs:
  - DbleVectNode champ vectoriel reel sur les noeuds
  - DbleVectCell champ vectoriel reel sur les cellules

  En sortie, il y aura production d'un fichier MED
  carre_quad4_n.med qui contiendra un seul maillage et 2 champs
  avec une seule famille la FAMILLE_0
*/

#include <med.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>

int main (int argc, char **argv)
{
  med_err ret;
  med_idt fid;
  char maa[MED_TAILLE_NOM+1] = "carre_quad4";
  med_int mdim = 2;
  int nnoe_dir;
  int nelt_dir;
  med_int nnoe;
  int i, j, ij;

  med_float * coo;
  med_int * numnoe;
  med_int * nufano;

  med_float hxsize;
  med_float hysize;

  med_float * DbleVectNode;
  med_float * DbleVectCell;

  time_t t1;

  /*
    Le maillage
   */

  char nomcoo[2*MED_TAILLE_PNOM+1] = "x       y       ";
  char unicoo[2*MED_TAILLE_PNOM+1] = "cm      cm      ";
  /*  char nomnoe[19*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4";*/
  char *nomnoe ;

  med_int nquad4;
  med_int * quad4;
  med_int * numquad4;
  med_int * nufaquad4;
  /* char nomquad4[MED_TAILLE_PNOM*4+1] = "quad1   quad2   quad3   quad4   ";*/
  char * nomquad4;
  int indexN1, indexN2, indexN3, indexN4;

  char nomfam[MED_TAILLE_NOM+1];
  med_int numfam;
  char attdes[MED_TAILLE_DESC+1];
  med_int natt;
  med_int attide;
  med_int attval;
  med_int ngro;
  char gro[MED_TAILLE_LNOM+1];
  int nfame = 0; 
  int nfamn = 0;

  char MedFile[100] = "carre_quad4_";
  char buff[100];

  /*
    Les champs
  */

  char champDbleVectNode[MED_TAILLE_NOM+1] = "DbleVectNode";
  char compDbleVectNode[MED_TAILLE_PNOM*2+1] = "comp1   comp2   " ;
  char unitDbleVectNode[MED_TAILLE_PNOM*2+1] = "unit1   unit2   " ;

  char champDbleVectCell[MED_TAILLE_NOM+1] = "DbleVectCell";
  char compDbleVectCell[MED_TAILLE_PNOM*2+1] = "comp1   comp2   " ;
  char unitDbleVectCell[MED_TAILLE_PNOM*2+1] = "unit1   unit2   " ;

  if (argc != 2)
    {
      printf("Usage: %s <n> \n",argv[0]);
      printf("       where\n");
      printf("       - n is the number of nodes in each direction.\n");
      printf("\n");
      printf("This program will produce a MED file carre_quad4_n.med\n");
      exit(0);
    }

  nnoe_dir = atoi(argv[1]);
  nelt_dir = nnoe_dir-1;
  nnoe = nnoe_dir*nnoe_dir;

  coo = malloc(mdim*nnoe*sizeof(med_float));
  numnoe = malloc(nnoe*sizeof(med_int));
  nufano = malloc(nnoe*sizeof(med_int));
  nomnoe = malloc((MED_TAILLE_PNOM*nnoe+1)*sizeof(char));

  hxsize = 1./((med_float) (nnoe_dir - 1));
  hysize = hxsize;

  nquad4 = nelt_dir*nelt_dir;
  quad4 = malloc(4*nquad4*sizeof(med_int));
  numquad4 = malloc(nquad4*sizeof(med_int));
  nufaquad4 = malloc(nquad4*sizeof(med_int));
  nomquad4 = malloc((MED_TAILLE_PNOM*nquad4+1)*sizeof(char));

  DbleVectNode = malloc(mdim*nnoe*sizeof(med_float));
  DbleVectCell = malloc(mdim*nquad4*sizeof(med_float));

  /*
    les noeuds:
  */

  for(j=0;j<nnoe_dir;j++)
    {
      for (i=0;i<nnoe_dir;i++)
        {
          int ij = j*nnoe_dir+i;

          numnoe[ij] = ij+1;
          nufano[ij] = 0;

          coo[mdim*ij] = ((med_float) i)*hxsize;
          coo[mdim*ij+1] = ((med_float) j)*hysize;

          /*
          printf("Coordonn�es %d   X = %lf  Y = %lf\n",(ij+1),coo[mdim*ij],coo[mdim*ij+1]);
          */
        }
    }

  /*
    les elements:
  */

  for(j=0;j<nelt_dir;j++)
    {
      for (i=0;i<nelt_dir;i++)
        {
          int ij = j*nelt_dir+i;

          numquad4[ij] = ij+1;
          nufaquad4[ij] = 0;

          indexN4 = j*nnoe_dir+i+1;
          indexN3 = indexN4+1;
          indexN1 = indexN4+nnoe_dir;
          indexN2 = indexN3+nnoe_dir;

          quad4[4*ij] = indexN1;
          quad4[4*ij+1] = indexN2;
          quad4[4*ij+2] = indexN3;
          quad4[4*ij+3] = indexN4;

          /*
          printf("Connectivit�e %d  i1 = %d  i2 = %d  i3 = %d  i4 = %d\n",(ij+1),quad4[4*ij],quad4[4*ij+1],quad4[4*ij+2],quad4[4*ij+3]);
          */

        }
    }

  /*
    Les champs
  */

  (void) time(&t1);
  
   srand((int) t1); /* use time in seconds to set seed */  

   for(i=0;i<nnoe;i++)
     {
       DbleVectNode[mdim*i] =
         (med_float) (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       DbleVectNode[mdim*i+1] =
         (med_float) (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       /*
         printf("i %d DbleVectNode %lf %lf\n",i,DbleVectNode[mdim*i],
         DbleVectNode[mdim*i+1]);
       */
     }

   for(i=0;i<nquad4;i++)
     {
       DbleVectCell[mdim*i] =
         (med_float) (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       DbleVectCell[mdim*i+1] =
         (med_float) (1+(int) (1000*rand()/(RAND_MAX+1.0)));

       /*
         printf("i %d DbleVectCell %lf %lf\n",i,DbleVectCell[mdim*i],
         DbleVectCell[mdim*i+1]);
       */
     }

  /***************************************************************************/

  sprintf(buff,"%d",nnoe_dir);
  strcat(MedFile,buff);
  strcat(MedFile,".med");

  fid = MEDouvrir(MedFile,RDWR);

  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("%d\n",ret);

  /***************************************************************************/
  if (ret == 0)
    ret = MEDmaaCr(fid,maa,mdim);
  printf("%d\n",ret);

  if (ret == 0)
    ret = MEDunvCr(fid,maa);
  printf("%d\n",ret);

  /***************************************************************************/

  if (ret == 0)
    ret = MEDnoeudsEcr(fid,maa,mdim,coo,MED_FULL_INTERLACE,MED_CART,
                       nomcoo,unicoo,nomnoe,MED_FAUX,numnoe,MED_VRAI,
                       nufano,nnoe,WRONLY);
  printf("%d\n",ret);

  /*
    ecriture des mailles MED_QUAD4 :
    - connectivite
    - noms (optionnel) 
    - numeros (optionnel)
    - numeros des familles
  */

  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,quad4,MED_FULL_INTERLACE,
                         nomquad4,MED_FAUX,numquad4,MED_VRAI,nufaquad4,nquad4,
                         MED_MAILLE,MED_QUAD4,MED_NOD,WRONLY);
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

  /***************************************************************************/
  /*
    Les Champs
  */

  if (ret == 0)
    {
      ret = MEDchampCr(fid,champDbleVectNode,MED_REEL64,compDbleVectNode,
                       unitDbleVectNode,mdim);

      printf("MEDchampCr DbleVectNode : %d \n",ret);

      if (ret == 0)
        {
          ret = MEDchampEcr(fid, maa, champDbleVectNode,
                            (unsigned char *)DbleVectNode,
                            MED_NO_INTERLACE, nnoe,
                            MED_NOPG, MED_ALL, MED_NOPFL, WRONLY, MED_NOEUD, 
                            0, MED_NOPDT,"        ", 0., MED_NONOR);
        
          printf("MEDchampEcr DbleVectNode : %d \n",ret);
        }
    }

  if (ret == 0)
    {
      ret = MEDchampCr(fid,champDbleVectCell,MED_REEL64,compDbleVectCell,
                       unitDbleVectCell,mdim);

      printf("MEDchampCr DbleVectCell : %d \n",ret);

      if (ret == 0)
        {
          ret = MEDchampEcr(fid, maa, champDbleVectCell,
                            (unsigned char *)DbleVectCell,
                            MED_NO_INTERLACE, nquad4,
                            MED_NOPG, MED_ALL, MED_NOPFL, WRONLY, MED_MAILLE,
                            MED_QUAD4, MED_NOPDT,"        ", 0., MED_NONOR);
        
          printf("MEDchampEcr DbleVectCell : %d \n",ret);
        }
    }

  /***************************************************************************/

  ret = MEDfermer(fid);
  printf("%d\n",ret);
  
  free(coo);
  free(numnoe);
  free(nufano);
  free(nomnoe);
  free(quad4);
  free(numquad4);
  free(nufaquad4);
  free(nomquad4);
  free(DbleVectNode);
  free(DbleVectCell);

  return 0;
}

