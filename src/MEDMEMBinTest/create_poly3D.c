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

int main (int argc, char **argv)
{
  med_err ret;
  med_idt fid;
  char maa[MED_TAILLE_NOM+1] = "poly3D";
  char maadesc[MED_TAILLE_DESC+1] = "Example de maillage non structure 3D avec 2 polyedres+1tetra4 comme mailles et 3 polygones comme faces";
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
  char nomcoo[3*MED_TAILLE_PNOM+1] = "x               y               z               ";
  char unicoo[3*MED_TAILLE_PNOM+1] = "cm              cm              cm              ";
  /*  char nomnoe[18*MED_TAILLE_PNOM+1] = "nom1    nom2    nom3    nom4";*/
  char *nomnoe ;
  med_int numnoe[19] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
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
  char nomquad4[MED_TAILLE_PNOM*8+1] = "quad1           quad2           quad3           quad4           quad5           quad6           quad7           quad8           ";
  med_int numquad4[9] = {2,3,4,5,6,7,12,17};
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
  char nomtria3[MED_TAILLE_PNOM*6+1] = "tria1           tria2           tria3           tria4           tria5           tria6           ";
  med_int numtria3[6] = {8,13,14,15,16,18};
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
  char nompolygon[MED_TAILLE_PNOM*3+1] = "polygon1        polygon2        polygon3        ";
  med_int numpolygon[3] = {1,9,10};
  med_int nufapolygon[3] = {-3,-3,-3};

  /* Mailles POLYEDRE */
  med_int npolyhedron = 2;
  med_int npolyhedronfaces = 19;
  med_int npolyhedronnodes = 74;
  med_int polyhedronconnectivity[74] = {1,2,3,4,5,6,1,7,8,2,2,8,9,3,4,3,9,10,5,4,10,11,6,5,11,12,1,6,12,7,7,12,8,10,9,8,12,11,
                                        13,14,15,3,2,13,2,8,16,14,13,16,17,15,14,17,15,17,18,15,18,9,3,15,9,2,3,9,8,8,9,17,16,9,18,17};
  med_int polyhedronfacesindex[20] = {1,7,11,15,19,23,27,31,34,
                                      39,44,48,52,55,58,61,64,68,72,75};
  med_int polyhedronindex[3] = {1,10,20};
  char nompolyhedron[MED_TAILLE_PNOM*2+1] = "polyhedron1     polyhedron2     ";
  med_int numpolyhedron[2] = {2,3};
  med_int nufapolyhedron[2] = {-4,-4};

  /* Mailles TETRA4 */
  med_int ntetra4 = 1;
  med_int tetra4[4] = {
    17, 9, 18, 19
  };
  char nomtetra4[MED_TAILLE_PNOM*1+1] = "tetra1          ";
  med_int numtetra4[1] = {1};
  med_int nufatetra4[1] = {-5};


  char nomfam[MED_TAILLE_NOM+1];
  med_int numfam;
  char attdes[MED_TAILLE_DESC+1];
  med_int natt;
  med_int attide;
  med_int attval;
  med_int ngro;
  char gro[MED_TAILLE_LNOM+1];
  int i;
  int nfame = 2;
  int nfamn = 1;

  /*
    Some fields : 2 on nodes : one int and one double , one on cells : double
   */
  char champ1[MED_TAILLE_NOM+1]="fieldnodeint" ;
  char champ1_comp[MED_TAILLE_PNOM+1]="comp1           " ;
  char champ1_unit[MED_TAILLE_PNOM+1]="M               " ;
  med_int     fieldnodeint[19]    = {1,1,3,2,2,3,4,4,5,6,6,7,8,8,9,9,9,10,5};

  char champ2[MED_TAILLE_NOM+1]="fieldnodedouble" ;
  char champ2_comp[MED_TAILLE_PNOM+1]="comp1           " ;
  char champ2_unit[MED_TAILLE_PNOM+1]="J               " ;
  med_float   fieldnodedouble1[19] = {1.,3.,4.,1.,3.,4.,3.,2.,5.,6.,4.,3.,1.,5.,6.,4.,3.,7.,3.};
  med_float   fieldnodedouble2[19] = {1.,2.,2.,3.,3.,3.,4.,4.,5.,2.,8.,9.,6.,7.,1.,2.,5.,8.,4.};

  char champ3[MED_TAILLE_NOM+1]="fieldfacedouble" ;
  char champ3_comp[MED_TAILLE_PNOM*2+1]="comp1           comp2           " ;
  char champ3_unit[MED_TAILLE_PNOM*2+1]="M/S             m/s             " ;
  med_float   fieldfacedouble[6*2] = {0.,1.,1.,1.,1.,2.,2.,3.,3.,4.,4.,6.};

  /***************************************************************************/
  fid = MEDouvrir("poly3D.med",MED_LECTURE_ECRITURE);
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

  /* ecriture des faces MED_QUAD4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,quad4,MED_FULL_INTERLACE,
                         nomquad4,MED_FAUX,numquad4,MED_VRAI,nufaquad4,nquad4,
                         MED_MAILLE,MED_QUAD4,MED_NOD);
  printf("MEDelementsEcr for quad : %d \n",ret);

  /* ecriture des faces MED_TRIA3 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0) 
    ret = MEDelementsEcr(fid,maa,mdim,tria3,MED_FULL_INTERLACE,
                         nomtria3,MED_FAUX,numtria3,MED_VRAI,nufatria3,ntria3,
                         MED_MAILLE,MED_TRIA3,MED_NOD);
  printf("MEDelementsEcr for tria : %d \n",ret);

  /* ecriture des faces MED_POLYGONE :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles
     Dans ce cas il n existe pas de routine globale (Warning !!) */
  if (ret == 0)
    ret = MEDpolygoneConnEcr(fid,maa,indexpolygon,nindexpolygon,polygon,
                             MED_MAILLE,MED_NOD);
  printf("MEDpolygoneConnEcr: %d \n",ret);

  if (ret == 0)
    MEDnomEcr(fid,maa,nompolygon,npolygon,MED_MAILLE,MED_POLYGONE);
  printf("MEDnomEcr for polygon : %d \n",ret);

  if (ret == 0)
    MEDnumEcr(fid,maa,numpolygon,npolygon,MED_MAILLE,MED_POLYGONE);
  printf("MEDnumEcr for polygon : %d \n",ret);

  if (ret == 0)
    MEDfamEcr(fid,maa,nufapolygon,npolygon,MED_MAILLE,MED_POLYGONE);
  printf("MEDfamEcr for polygon : %d \n",ret);

  /* ecriture des mailles MED_POLYEDRE :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles
     Dans ce cas il n existe pas de routine globale (Warning !!) */
  if (ret == 0)
    ret = MEDpolyedreConnEcr(fid,maa,polyhedronindex,npolyhedron+1,polyhedronfacesindex,npolyhedronfaces+1,polyhedronconnectivity,MED_NOD);
  printf("MEDpolyedreConnEcr: %d \n",ret);

  if (ret == 0)
    ret = MEDnomEcr(fid,maa,nompolyhedron,npolyhedron,MED_MAILLE,MED_POLYEDRE);
  printf("MEDnomEcr for polyhedron : %d \n",ret);

  if (ret == 0)
    ret = MEDnumEcr(fid,maa,numpolyhedron,npolyhedron,MED_MAILLE,MED_POLYEDRE);
  printf("MEDnumEcr for polyhedron : %d \n",ret);

  if (ret == 0)
    ret = MEDfamEcr(fid,maa,nufapolyhedron,npolyhedron,MED_MAILLE,MED_POLYEDRE);
  printf("MEDfamEcr for polyhedron : %d \n",ret);

  /* ecriture des mailles TETRA4 :
     - connectivite
     - noms (optionnel) 
     - numeros (optionnel)
     - numeros des familles */
  if (ret == 0)
    ret = MEDelementsEcr(fid,maa,mdim,tetra4,MED_FULL_INTERLACE,
                         nomtetra4,MED_FAUX,numtetra4,MED_VRAI,nufatetra4,ntetra4,
                         MED_MAILLE,MED_TETRA4,MED_NOD);
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
      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,0,
                     gro,0);
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
      strcpy(attdes,"description attribut");
      strcpy(gro,"groupe0");
      ngro = 1;

      ret = MEDfamCr(fid,maa,nomfam,numfam,&attide,&attval,attdes,
                     natt,gro,ngro);
      printf("MEDfamCr : %d\n",ret);

      numfam = -2;
      strcpy(nomfam,"FAMILLE_FACE_TRIA3");
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

      numfam = -3;
      strcpy(nomfam,"FAMILLE_FACE_POLYGONS");
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

      numfam = -4;
      strcpy(nomfam,"FAMILLE_MAILLE_POLYHEDRON");
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

      numfam = -5;
      strcpy(nomfam,"FAMILLE_MAILLE_TETRA4");
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
  */
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
  
  // on met champ2 sans pas de temps pour pouvoir le lire aussi par defaut !
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
        ret = MEDchampEcr(fid, maa, champ3, (unsigned char *)fieldfacedouble,
                          MED_NO_INTERLACE, ntria3, MED_NOGAUSS, MED_ALL,
                          MED_NOPFL, MED_NO_PFLMOD, MED_FACE, MED_TRIA3,
                          MED_NOPDT,"        ", 0., MED_NONOR);
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

