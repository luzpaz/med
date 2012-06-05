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
  avec n (=argv[1]) noeuds dans chaque direction.
  2 champs:
  - DbleVectNode champ vectoriel reel sur les noeuds
  - DbleVectCell champ vectoriel reel sur les cellules

  En sortie, il y aura production d'un fichier MED
  cube_hexa8_n.med qui contiendra un seul maillage et 2 champs
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
  char maa[MED_TAILLE_NOM+1] = "cube_hexa8";
  med_int mdim = 3;

  int nnoe_dir;
  int nelt_dir;
  med_int nnoe;
  int i, j, k, ijk;

  med_float * coo;
  med_int * numnoe;
  med_int * nufano;

  med_float hxsize;
  med_float hysize;
  med_float hzsize;

  med_float * DbleVectNode;
  med_float * DbleVectCell;

  time_t t1;

  /*
    Le maillage
   */

  char nomcoo[3*MED_TAILLE_PNOM+1] = "x       y       z       ";
  char unicoo[3*MED_TAILLE_PNOM+1] = "cm      cm      cm      ";
  /*  char nomnoe[19*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4";*/
  char *nomnoe ;

  med_int nhexa8;
  med_int * hexa8;
  med_int * numhexa8;
  med_int * nufahexa8;

  char * nomhexa8;
  int indexN1, indexN2, indexN3, indexN4, indexN5, indexN6, indexN7, indexN8;

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

  char MedFile[100] = "cube_hexa8_";
  char buff[100];

  /*
    Les champs
  */

  char champDbleVectNode[MED_TAILLE_NOM+1] = "DbleVectNode";
  char compDbleVectNode[MED_TAILLE_PNOM*3+1] = "comp1   comp2   comp3   " ;
  char unitDbleVectNode[MED_TAILLE_PNOM*3+1] = "unit1   unit2   unit3   " ;

  char champDbleVectCell[MED_TAILLE_NOM+1] = "DbleVectCell";
  char compDbleVectCell[MED_TAILLE_PNOM*3+1] = "comp1   comp2   comp3   " ;
  char unitDbleVectCell[MED_TAILLE_PNOM*3+1] = "unit1   unit2   unit3   " ;

  if (argc != 2)
    {
      printf("Usage: %s <n> \n",argv[0]);
      printf("       where\n");
      printf("       - n is the number of nodes in each direction.\n");
      printf("\n");
      printf("This program will produce a MED file cube_hexa8_n.med\n");
      exit(0);
    }

  nnoe_dir = atoi(argv[1]);
  nelt_dir = nnoe_dir-1;
  nnoe = nnoe_dir*nnoe_dir*nnoe_dir;

  coo = malloc(mdim*nnoe*sizeof(med_float));
  numnoe = malloc(nnoe*sizeof(med_int));
  nufano = malloc(nnoe*sizeof(med_int));
  nomnoe = malloc((MED_TAILLE_PNOM*nnoe+1)*sizeof(char));

  hxsize = 1./((med_float) (nnoe_dir - 1));
  hysize = hxsize;
  hzsize = hxsize;

  nhexa8 = nelt_dir*nelt_dir*nelt_dir;
  hexa8 = malloc(8*nhexa8*sizeof(med_int));
  numhexa8 = malloc(nhexa8*sizeof(med_int));
  nufahexa8 = malloc(nhexa8*sizeof(med_int));
  nomhexa8 = malloc((MED_TAILLE_PNOM*nhexa8+1)*sizeof(char));

  DbleVectNode = malloc(mdim*nnoe*sizeof(med_float));
  DbleVectCell = malloc(mdim*nhexa8*sizeof(med_float));

  /*
    les noeuds:
  */

  for(k=0;k<nnoe_dir;k++)
    {
      for(j=0;j<nnoe_dir;j++)
        {
          for (i=0;i<nnoe_dir;i++)
            {
              int ijk = k*nnoe_dir*nnoe_dir+j*nnoe_dir+i;

              numnoe[ijk] = ijk+1;
              nufano[ijk] = 0;

              coo[mdim*ijk] = ((med_float) i)*hxsize;
              coo[mdim*ijk+1] = ((med_float) j)*hysize;
              coo[mdim*ijk+2] = ((med_float) k)*hzsize;

              /*
              printf("Coordonnées %d   X = %lf  Y = %lf  Z = %lf\n",(ijk+1),coo[mdim*ijk],coo[mdim*ijk+1],coo[mdim*ijk+2]);
              */
            }
        }
    }

  /*
    les elements:
  */

  for(k=0;k<nelt_dir;k++)
    {
      for(j=0;j<nelt_dir;j++)
        {
          for (i=0;i<nelt_dir;i++)
            {
              int ijk = k*nelt_dir*nelt_dir+j*nelt_dir+i;

              numhexa8[ijk] = ijk+1;
              nufahexa8[ijk] = 0;

              indexN5 = k*nnoe_dir*nnoe_dir+j*nnoe_dir+i+1;
              indexN8 = indexN5+1;
              indexN1 = indexN5+nnoe_dir;
              indexN4 = indexN8+nnoe_dir;

              indexN6 = indexN5+nnoe_dir*nnoe_dir;
              indexN7 = indexN8+nnoe_dir*nnoe_dir;
              indexN2 = indexN1+nnoe_dir*nnoe_dir;
              indexN3 = indexN4+nnoe_dir*nnoe_dir;

              hexa8[8*ijk] = indexN1;
              hexa8[8*ijk+1] = indexN2;
              hexa8[8*ijk+2] = indexN3;
              hexa8[8*ijk+3] = indexN4;
              hexa8[8*ijk+4] = indexN5;
              hexa8[8*ijk+5] = indexN6;
              hexa8[8*ijk+6] = indexN7;
              hexa8[8*ijk+7] = indexN8;

              /*
              printf("Connectivitée %d  i1 = %d  i2 = %d  i3 = %d  i4 = %d  i5 = %d  i6 = %d  i7 = %d  i8 = %d\n",(ijk+1),hexa8[8*ijk],hexa8[8*ijk+1],hexa8[8*ijk+2],hexa8[8*ijk+3],hexa8[8*ijk+4],hexa8[8*ijk+5],hexa8[8*ijk+6],hexa8[8*ijk+7]);
              */
            }
        }
    }

  /*
    Les champs
  */

  (void) time(&t1);
  
   srand((int) t1); /* use time in seconds to set seed */  

   for(i=0;i<nnoe;i++)
     {
       DbleVectNode[mdim*i] = (med_float)
         (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       DbleVectNode[mdim*i+1] = (med_float)
         (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       DbleVectNode[mdim*i+2] = (med_float)
         (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       /*
         printf("i %d DbleVectNode %lf %lf\n",i,DbleVectNode[mdim*i],
         DbleVectNode[mdim*i+1],DbleVectNode[mdim*i+2]);
       */
     }

   for(i=0;i<nhexa8;i++)
     {
       DbleVectCell[mdim*i] = (med_float)
         (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       DbleVectCell[mdim*i+1] = (med_float)
         (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       DbleVectCell[mdim*i+2] = (med_float)
         (1+(int) (100.0*rand()/(RAND_MAX+1.0)));

       /*
         printf("i %d DbleVectCell %lf %lf\n",i,DbleVectCell[mdim*i],
         DbleVectCell[mdim*i+1],DbleVectCell[mdim*i+2]);
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
    ecriture des mailles MED_HEXA8 :
    - connectivite
    - noms (optionnel) 
    - numeros (optionnel)
    - numeros des familles
  */

  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,hexa8,MED_FULL_INTERLACE,
                         nomhexa8,MED_FAUX,numhexa8,MED_VRAI,nufahexa8,nhexa8,
                         MED_MAILLE,MED_HEXA8,MED_NOD,WRONLY);
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
                            MED_NO_INTERLACE, nhexa8,
                            MED_NOPG, MED_ALL, MED_NOPFL, WRONLY, MED_MAILLE,
                            MED_HEXA8, MED_NOPDT,"        ", 0., MED_NONOR);
        
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
  free(hexa8);
  free(numhexa8);
  free(nufahexa8);
  free(nomhexa8);
  free(DbleVectNode);
  free(DbleVectCell);

  return 0;
}


