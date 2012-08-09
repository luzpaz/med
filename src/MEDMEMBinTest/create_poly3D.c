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
  creation d'une geometrie 3d : 
  maillé en :
  - 2 polyedres
    ayant pour faces 3 polygones
                     9 quadrangles
                     6 triangles
  - 1 tetra4

*/

#include <med.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
  med_err ret;
  med_idt fid;
  char maa[MED_NAME_SIZE+1] = "poly3D";
  char maadesc[MED_COMMENT_SIZE+1] = "Example de maillage non structure 3D avec 2 polyedres+1tetra4 comme mailles et 3 polygones comme faces";
  med_int mdim = 3;
  med_int nnoe = 19;
  /*
    les noeuds:
  */
  med_float coo[57] = {
    2.0, 3.0, 2.0,
    3.0, 2.0, 2.0,
    4.0, 1.0, 2.0,
    2.0, 0.0, 2.0,
    0.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 3.0, 1.0,
    3.0, 2.0, 0.0,
    4.0, 1.0, 0.0,
    2.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 2.0, 0.0,
    5.0, 3.0, 2.0,
    7.0, 2.0, 2.0,
    6.0, 0.0, 2.0,
    6.0, 3.0, 0.0,
    7.0, 2.0, 0.0,
    6.0, 0.0, -1.0,
    5.0, 1.0, -3.0 
  };
  char nomcoo[3*MED_SNAME_SIZE+1] = "x               y               z               ";
  char unicoo[3*MED_SNAME_SIZE+1] = "cm              cm              cm              ";
  /*  char nomnoe[18*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4";*/
  //char *nomnoe ;
//  med_int numnoe[19] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
  med_int nufano[19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  /*
    les elements:
  */
  /* Faces QUAD4 */
  med_int nquad4 = 8;
  med_int quad4[32] = {
    1, 7, 8, 2,
    2, 8, 9, 3,
    4, 3, 9, 10,
    5, 4, 10, 11,
    6, 5, 11, 12,
    1, 6, 12, 7,
    14, 13, 16, 17,
    8, 9, 17, 16
  };
 // char nomquad4[MED_TAILLE_PNOM*8+1] = "quad1           quad2           quad3           quad4           quad5           quad6           quad7           quad8           ";
 // med_int numquad4[9] = {2,3,4,5,6,7,12,17};
  med_int nufaquad4[9] = {-1,-1,-1,-1,-1,-1,-1,-1};

  /* Faces TRIA3 */
  med_int ntria3 = 6;
  med_int tria3[18] = {
    7, 12, 8,
    15, 14, 17,
    15, 17, 18,
    15, 18, 9,
    3, 15, 9,
    18, 17, 9
  };
 // char nomtria3[MED_TAILLE_PNOM*6+1] = "tria1           tria2           tria3           tria4           tria5           tria6           ";
 // med_int numtria3[6] = {8,13,14,15,16,18};
  med_int nufatria3[6] = {-2,-2,-2,-2,-2,-2};

  /* Faces POLYGONE */
  med_int npolygon = 3;
  med_int nindexpolygon = 4;
  med_int indexpolygon [4] = {
    1, 7, 12, 17
  };
  med_int polygon[16] = {
    1, 2, 3, 4, 5, 6,
    10, 9, 8, 12, 11,
    13, 14, 15, 3, 2
  };
  //char nompolygon[MED_SNAME_SIZE*3+1] = "polygon1        polygon2        polygon3        ";
//  med_int numpolygon[3] = {1,9,10};
  med_int nufapolygon[3] = {-3,-3,-3};

  /* Mailles POLYEDRE */
  med_int npolyhedron = 2;
  med_int npolyhedronfaces = 19;
  med_int polyhedronconnectivity[74] = {1,2,3,4,5,6,1,7,8,2,2,8,9,3,4,3,9,10,5,4,10,11,6,5,11,12,1,6,12,7,7,12,8,10,9,8,12,11,
                                        13,14,15,3,2,13,2,8,16,14,13,16,17,15,14,17,15,17,18,15,18,9,3,15,9,2,3,9,8,8,9,17,16,9,18,17};
  med_int polyhedronfacesindex[20] = {1,7,11,15,19,23,27,31,34,
                                      39,44,48,52,55,58,61,64,68,72,75};
  med_int polyhedronindex[3] = {1,10,20};
  //char nompolyhedron[MED_SNAME_SIZE*2+1] = "polyhedron1     polyhedron2     ";
//  med_int numpolyhedron[2] = {2,3};
  med_int nufapolyhedron[2] = {-4,-4};

  /* Mailles TETRA4 */
  med_int ntetra4 = 1;
  med_int tetra4[4] = {
    17, 9, 18, 19
  };
 // char nomtetra4[MED_SNAME_SIZE*1+1] = "tetra1          ";
 // med_int numtetra4[1] = {1};
  med_int nufatetra4[1] = {-5};


  char nomfam[MED_NAME_SIZE+1];
  med_int numfam;
  //char attdesMED_TAILLE_DESC+1];
  //med_int attide;
  //med_int attval;
  char gro[MED_LNAME_SIZE+1];
  char dtunitp3[MED_LNAME_SIZE+1]="";

  /*
    Some fields : 2 on nodes : one int and one double , one on cells : double
   */
  char champ1[MED_NAME_SIZE+1]="fieldnodeint" ;
  char champ1_comp[MED_SNAME_SIZE+1]="comp1           " ;
  char champ1_unit[MED_SNAME_SIZE+1]="M               " ;
  med_int     fieldnodeint[19]    = {1,1,3,2,2,3,4,4,5,6,6,7,8,8,9,9,9,10,5};

  char champ2[MED_NAME_SIZE+1]="fieldnodedouble" ;
  char champ2_comp[MED_SNAME_SIZE+1]="comp1           " ;
  char champ2_unit[MED_SNAME_SIZE+1]="J               " ;
  med_float   fieldnodedouble1[19] = {1.,3.,4.,1.,3.,4.,3.,2.,5.,6.,4.,3.,1.,5.,6.,4.,3.,7.,3.};
  med_float   fieldnodedouble2[19] = {1.,2.,2.,3.,3.,3.,4.,4.,5.,2.,8.,9.,6.,7.,1.,2.,5.,8.,4.};

  char champ3[MED_NAME_SIZE+1]="fieldfacedouble" ;
  char champ3_comp[MED_SNAME_SIZE*2+1]="comp1           comp2           " ;
  char champ3_unit[MED_SNAME_SIZE*2+1]="M/S             m/s             " ;
  med_float   fieldfacedouble[6*2] = {0.,1.,1.,1.,1.,2.,2.,3.,3.,4.,4.,6.};

  /***************************************************************************/
  fid = MEDfileOpen("poly3D.med",MED_ACC_RDWR);
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

  /* ecriture des faces MED_QUAD4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDmeshElementConnectivityWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                       MED_CELL,MED_QUAD4,MED_NODAL,MED_FULL_INTERLACE,
                                       nquad4,quad4);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_CELL,MED_QUAD4,nquad4,nufaquad4);
  printf("MEDelementsEcr for quad : %d \n",ret);

  /* ecriture des faces MED_TRIA3 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDmeshElementConnectivityWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                       MED_CELL,MED_TRIA3,MED_NODAL,MED_FULL_INTERLACE,
                                       ntria3,tria3);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_CELL,MED_TRIA3,ntria3,nufatria3);
  printf("MEDelementsEcr for tria : %d \n",ret);

  /* ecriture des faces MED_POLYGONE :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles
     Dans ce cas il n existe pas de routine globale (Warning !!) */
  if (ret == 0)
    ret = MEDmeshPolygonWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT, MED_CELL,MED_NODAL, nindexpolygon,indexpolygon,polygon);
  printf("MEDpolygoneConnEcr: %d \n",ret);

/*   if (ret == 0) */
/*     MEDnomEcr(fid,maa,nompolygon,npolygon,MED_CELL,MED_POLYGONE); */
/*   printf("MEDnomEcr for polygon : %d \n",ret); */

/*   if (ret == 0) */
/*     MEDnumEcr(fid,maa,numpolygon,npolygon,MED_CELL,MED_POLYGONE); */
/*   printf("MEDnumEcr for polygon : %d \n",ret); */

  if (ret == 0)
    MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT, MED_CELL,MED_POLYGON, npolygon,nufapolygon);
  printf("MEDfamEcr for polygon : %d \n",ret);

  /* ecriture des mailles MED_POLYEDRE :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles
     Dans ce cas il n existe pas de routine globale (Warning !!) */
  if (ret == 0)
    ret = MEDmeshPolyhedronWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT, MED_CELL,MED_NODAL, npolyhedronfaces+1,polyhedronfacesindex,npolyhedron+1,polyhedronindex,polyhedronconnectivity);
  printf("MEDpolyedreConnEcr: %d \n",ret);

/*   if (ret == 0) */
/*     ret = MEDnomEcr(fid,maa,nompolyhedron,npolyhedron,MED_CELL,MED_POLYEDRE); */
/*   printf("MEDnomEcr for polyhedron : %d \n",ret); */

/*   if (ret == 0) */
/*     ret = MEDnumEcr(fid,maa,numpolyhedron,npolyhedron,MED_CELL,MED_POLYEDRE); */
/*   printf("MEDnumEcr for polyhedron : %d \n",ret); */

  if (ret == 0)
    ret = MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT, MED_CELL,MED_POLYHEDRON, npolyhedron,nufapolyhedron);
  printf("MEDfamEcr for polyhedron : %d \n",ret);

  /* ecriture des mailles TETRA4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0)
    ret = MEDmeshElementConnectivityWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                       MED_CELL,MED_TETRA4,MED_NODAL,MED_FULL_INTERLACE,
                                       ntetra4,tetra4);
  MEDmeshEntityFamilyNumberWr(fid,maa,MED_NO_DT,MED_NO_IT,MED_CELL,MED_TETRA4,ntetra4,nufatetra4);
  printf("MEDelementsEcr for tetra : %d \n",ret);

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
      - 2 familles d'elements de dimension (d)
        en fait de face (-10)
  */
  /*
  if (ret == 0)
    {
      numfam = -1;
      strcpy(nomfam,"FAMILLE_FACE_QUAD4");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro,"groupe0");
      ngro = 1;

      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
      printf("MEDfamCr : %d\n",ret);

      numfam = -2;
      strcpy(nomfam,"FAMILLE_FACE_TRIA3");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro,"groupe0");
      ngro = 1;

      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
      printf("MEDfamCr : %d\n",ret);

      numfam = -3;
      strcpy(nomfam,"FAMILLE_FACE_POLYGONS");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro,"groupe0");
      ngro = 1;

      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
      printf("MEDfamCr : %d\n",ret);

      numfam = -4;
      strcpy(nomfam,"FAMILLE_MAILLE_POLYHEDRON");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro,"groupe0");
      ngro = 1;

      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
      printf("MEDfamCr : %d\n",ret);

      numfam = -5;
      strcpy(nomfam,"FAMILLE_MAILLE_TETRA4");
      sprintf(nomfam,"%s%d",nomfam,-numfam);
      attide = 1;
      attval = numfam*100;
      natt = 1;
      //strcpy(attdes,"description attribut");
      strcpy(gro,"groupe0");
      ngro = 1;

      ret = MEDfamilyCr(fid,maa,nomfam,numfam,ngro,gro);
      printf("MEDfamCr : %d\n",ret);
    }
  */
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
  
  /*/ on met champ2 sans pas de temps pour pouvoir le lire aussi par defaut ! */
  if (ret == 0)
    {
      ret = MEDfieldValueWr(fid, champ2,MED_NO_DT,MED_NO_IT,0. ,MED_NODE,0,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,nnoe, (unsigned char *)fieldnodedouble1);
      printf("MEDchampEcr : %d \n",ret); 
    }

  if (ret == 0)
    {
      ret = MEDfieldCr(fid,champ3,MED_FLOAT64,2,champ3_comp,champ3_unit,dtunitp3,maa);
      printf("MEDchampCr : %d \n",ret);
      if (ret == 0) {
        ret = MEDfieldValueWr(fid, champ3,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_TRIA3,MED_NO_INTERLACE,MED_ALL_CONSTITUENT,ntria3, (unsigned char *)fieldfacedouble);
        printf("MEDchampEcr : %d \n",ret);
      }
    }
  /***************************************************************************/

  ret = MEDfermer(fid);
  printf("MEDfermer : %d\n",ret);
  
  if ( getenv("srcdir") ) 
    /* we are in 'make check' */
    remove( "poly3D.med" );

  return 0;
}

