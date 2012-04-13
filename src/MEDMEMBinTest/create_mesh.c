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

#include <med.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
  med_err ret;
  med_idt fid;
  char maa[MED_NAME_SIZE+1] = "maa1";
  char maadesc[MED_COMMENT_SIZE+1] = "Example de maillage non structure 3D";
  med_int mdim = 3;
  med_int nnoe = 19;
  med_float coo[57] = {
    0.0, 0.0, 0.0, 
    0.0, 0.0, 1.0, 
    2.0, 0.0, 1.0, 
    0.0, 2.0, 1.0, 
    -2.0, 0.0, 1.0, 
    0.0, -2.0, 1.0, 
    1.0, 1.0, 2.0, 
    -1.0, 1.0, 2.0, 
    -1.0, -1.0, 2.0, 
    1.0, -1.0, 2.0, 
    1.0, 1.0, 3.0, 
    -1.0, 1.0, 3.0, 
    -1.0, -1.0, 3.0, 
    1.0, -1.0, 3.0, 
    1.0, 1.0, 4.0, 
    -1.0, 1.0, 4.0, 
    -1.0, -1.0, 4.0, 
    1.0, -1.0, 4.0,
    0.0, 0.0, 5.0
  };
  char nomcoo[3*MED_SNAME_SIZE+1] = "x               y               z               ";
  char unicoo[3*MED_SNAME_SIZE+1] = "cm              cm              cm              ";
  //char *nomnoe ;
//  med_int numnoe[19] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
  med_int nufano[19] = {1,1,3,3,0,0,2,0,4,0,4,2,4,2,4,2,4,1,3};
  
  med_int ntet4 = 12;
  med_int tet4[12*4] = {
    1,2,3,6,
    1,2,4,3,
    1,2,5,4,
    1,2,6,5,
    2,7,4,3,
    2,8,5,4,
    2,9,6,5,
    2,10,3,6,
    2,7,3,10,
    2,8,4,7,
    2,9,5,8,
    2,10,6,9
  };
  //char * nomtet4 ; 
 // med_int numtet4[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
  med_int nufatet4[12] = {-1,-1,-1,-1,0,0,0,0,0,0,-2,-3}; 

  med_int npyra5 = 2;
  med_int pyra5[5*2] = {
    7,8,9,10,2,
    15,18,17,16,19
  };
 // char nompyra5[MED_TAILLE_PNOM*2+1] = "pyra1           pyra2           ";
 // med_int numpyra5[2] = {13,16};
  med_int nufapyra5[2] = {0,-3};

  med_int nhexa8 = 2;
  med_int hexa8[8*2] = {
    11,12,13,14,7,8,9,10,
    15,16,17,18,11,12,13,14
  };
 // char nomhexa8[MED_TAILLE_PNOM*2+1] = "hexa1           hexa2           ";
 // med_int numhexa8[2] = {14,15};
  med_int nufahexa8[2] = {0,0};

  char nomfam[MED_NAME_SIZE+1];
  med_int numfam;
  //char attdesMED_TAILLE_DESC+1];
  med_int natt;
  med_int attide;
  med_int attval;
  med_int ngro;
  char gro[MED_LNAME_SIZE+1];
  char gro2[MED_LNAME_SIZE*2+1];
  char dtunitp3[MED_LNAME_SIZE+1]="";
  int i;
  int nfame = 3; 


  /*
    Some fields : 2 on nodes : one int and one double , one on cells : double
   */
  char champ1[MED_NAME_SIZE+1]="fieldnodeint" ;
  char champ1_comp[MED_SNAME_SIZE+1]="comp1           " ;
  char champ1_unit[MED_SNAME_SIZE+1]="M               " ;
  med_int     fieldnodeint[19]    = {1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7};

  char champ2[MED_NAME_SIZE+1]="fieldnodedouble" ;
  char champ2_comp[MED_SNAME_SIZE+1]="comp1           " ;
  char champ2_unit[MED_SNAME_SIZE+1]="J               " ;
  med_float   fieldnodedouble1[19] = {1.,1.,1.,2.,2.,2.,3.,3.,3.,4.,4.,4.,5.,5.,5.,6.,6.,6.,7.};
  med_float   fieldnodedouble2[19] = {1.,2.,2.,2.,3.,3.,3.,4.,4.,4.,5.,5.,5.,6.,6.,6.,7.,7.,7.};

  char champ3[MED_NAME_SIZE+1]="fieldcelldoublevector" ;
  char champ3_comp[MED_SNAME_SIZE*3+1]="comp1           comp2           comp3           " ;
  char champ3_unit[MED_SNAME_SIZE*3+1]="m/s             m/s             m/s             " ;
  med_float   fieldcelldouble1[12*3] = {1.,1.,1.,2.,2.,2.,3.,3.,3.,4.,4.,4.,0.,0.,0.,1.,1.,1.,0.,0.,0.,1.,1.,1.,1.,1.,1.,0.,0.,0.,1.,1.,1.,0.,0.,0.};
  med_float   fieldcelldouble2[2*3]  = {5.,5.,0.,1.,0.,1.};
  med_float   fieldcelldouble3[2*3]  = {6.,6.,1.,0.,1.,0.};

  char champ4[MED_NAME_SIZE+1]="fieldcelldoublescalar" ;
  char champ4_comp[MED_SNAME_SIZE+1]="comp1           " ;
  char champ4_unit[MED_SNAME_SIZE+1]="m/s             " ;
  med_float   fieldcelldouble4[12] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  med_float   fieldcelldouble5[2]  = {2.,2.};
  med_float   fieldcelldouble6[2]  = {3.,3.};

  /***************************************************************************/
  fid = MEDfileOpen("pointe.med",MED_ACC_RDWR);
  if (fid < 0)
    ret = -1;
  else
    ret = 0;
  printf("MEDouvrir : %d\n",ret);

  /***************************************************************************/
  if (ret == 0)
    //ret = MEDmeshCr(fid,maa,mdim,mdim,MED_UNSTRUCTURED_MESH,maadesc,dtunitp3,
    //MED_SORT_DTIT,MED_CARTESIAN,nomcoo,unicoo);
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


  /***************************************************************************/
  /* ecriture des mailles MED_TETRA4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDmeshElementConnectivityWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                       MED_CELL,MED_TETRA4,MED_NODAL,MED_FULL_INTERLACE,
                                       ntet4,tet4);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_CELL,MED_TETRA4,ntet4,nufatet4);
  printf("MEDelementsEcr : %d \n",ret);

  /* ecriture des mailles MED_PYRA5 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDmeshElementConnectivityWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                       MED_CELL,MED_PYRA5,MED_NODAL,MED_FULL_INTERLACE,
                                       npyra5,pyra5);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_CELL,MED_PYRA5,npyra5,nufapyra5);
  printf("MEDelementsEcr : %d \n",ret);

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
  printf("MEDfamCr : %d \n",ret);

  /* on cree :
      - 3 familles d'elements (-1,-2,-3) et 
      - 2 familles de noeuds (1,2) 
  */
  if (ret == 0)
    {
      nfame = 3;
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
      strcpy(nomfam,"FAMILLE_NOEUD_1");
      numfam = 1;
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro2,"groupe2");
      strcpy(gro2+MED_NAME_SIZE,"groupe3");
      ngro = 2;
      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro2);
      printf("MEDfamCr : %d\n",ret);
    }
  if (ret == 0)
    {
      strcpy(nomfam,"FAMILLE_NOEUD_2");
      numfam = 2;
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro2,"groupe3");
      strcpy(gro2+MED_LNAME_SIZE,"groupe4");
      ngro = 2;
      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro2);
      printf("MEDfamCr : %d\n",ret);
    }
  if (ret == 0)
    {
      strcpy(nomfam,"FAMILLE_NOEUD_3");
      numfam = 3;
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro2,"groupe2");
      strcpy(gro2+MED_LNAME_SIZE,"groupe4");
      ngro = 2;
      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro2);
      printf("MEDfamCr : %d\n",ret);
    }
  if (ret == 0)
    {
      strcpy(nomfam,"FAMILLE_NOEUD_4");
      numfam = 4;
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro,"groupe5");
      ngro = 1;
      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
      printf("MEDfamCr : %d\n",ret);
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
        ret = MEDfieldValueWr(fid, champ1,MED_NO_DT,MED_NO_IT,0.,MED_NODE,0,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodeint);
        
        printf("MEDchampEcr : %d \n",ret);
      }
    }
  
  if (ret == 0)
    {
      ret = MEDfieldCr(fid,champ2,MED_FLOAT64,1,champ2_comp,champ2_unit,dtunitp3,maa);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ2,1,MED_NO_IT,1.1 ,MED_NODE,0,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodedouble1);
        printf("MEDchampEcr1 : %d \n",ret);
        ret = MEDfieldValueWr(fid, champ2,2,MED_NO_IT,1.2 ,MED_NODE,0,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodedouble2);
        printf("MEDchampEcr2 : %d \n",ret);
      }
    }
  
  /* on met champ2 sans pas de temps pour pouvoir le lire aussi par defaut !*/
  if (ret == 0)
    {
/*        ret = MEDfieldCr(fid,champ2,MED_REEL64,1,champ2_comp,champ2_unit,dtunitp3,maa); */
/*        printf("MEDchampCr : %d \n",ret); */
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ2,MED_NO_DT,MED_NO_IT,0. ,MED_NODE,0,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodedouble1);
        printf("MEDchampEcr : %d \n",ret);
      }
    }

  if (ret == 0)
    {
      ret = MEDfieldCr(fid,champ3,MED_FLOAT64,3,champ3_comp,champ3_unit,dtunitp3,maa);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ3,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_TETRA4,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,ntet4, (unsigned char *)fieldcelldouble1);
        printf("MEDchampEcr : %d \n",ret);
        ret = MEDfieldValueWr(fid, champ3,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_HEXA8,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,nhexa8, (unsigned char *)fieldcelldouble3);
        printf("MEDchampEcr : %d \n",ret);
        ret = MEDfieldValueWr(fid, champ3,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_PYRA5,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,npyra5, (unsigned char *)fieldcelldouble2);
        printf("MEDchampEcr : %d \n",ret);
      }
    }
  
  if (ret == 0)
    {
      ret = MEDfieldCr(fid,champ4,MED_FLOAT64,1,champ4_comp,champ4_unit,dtunitp3,maa);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ4,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_TETRA4,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,ntet4, (unsigned char *)fieldcelldouble4);
        printf("MEDchampEcr : %d \n",ret);
        ret = MEDfieldValueWr(fid, champ4,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_HEXA8,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,nhexa8, (unsigned char *)fieldcelldouble6);
        printf("MEDchampEcr : %d \n",ret);
        ret = MEDfieldValueWr(fid, champ4,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_PYRA5,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,npyra5, (unsigned char *)fieldcelldouble5);
        printf("MEDchampEcr : %d \n",ret);
      }
    }
  
  /***************************************************************************/
  ret = MEDfermer(fid);
  printf("MEDfermer : %d\n",ret);
  
  if ( getenv("srcdir") ) {
    /* we are in 'make check' */
    remove( "pointe.med" );
    printf("Remove generated file");
  }

  return 0;
}

