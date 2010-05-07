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

#ifndef MEDMEM_INTERPOLATION_TOOLS_HXX
#define MEDMEM_INTERPOLATION_TOOLS_HXX

#include "MEDMEM_define.hxx"

#define _TEMPLATE_ template <class CHAMP,class VALEURCHAMP,class NUAGENOEUD, class NOEUD, class NUAGEMAILLE>
#define _PARAM_ CHAMP,VALEURCHAMP,NUAGENOEUD,NOEUD,NUAGEMAILLE

// ces macros définissent pour une face carrée plane la fonction projection sur cette face, non normalisée, la numérotation est dans le cas d'un hexaedre
#define face2367(x,y,z) ((x6*(-y2 + y3) + x3*(y2 - y6) + x2*(-y3 + y6))*z - x6*y3*z2 + x3*y6*z2 + x6*y2*z3 - x2*y6*z3 - x3*y2*z6 + x2*y3*z6 + y*(x6*(z2 - z3) + x2*(z3 - z6) + x3*(-z2 + z6)) + x*(y6*(-z2 + z3) + y3*(z2 - z6) + y2*(-z3 + z6)))
#define face4567(x,y,z) ((x6*(-y4 + y5) + x5*(y4 - y6) + x4*(-y5 + y6))*z - x6*y5*z4 + x5*y6*z4 + x6*y4*z5 - x4*y6*z5 - x5*y4*z6 + x4*y5*z6 + y*(x6*(z4 - z5) + x4*(z5 - z6) + x5*(-z4 + z6)) + x*(y6*(-z4 + z5) + y5*(z4 - z6) + y4*(-z5 + z6)))
#define face1256(x,y,z) ((x5*(-y1 + y2) + x2*(y1 - y5) + x1*(-y2 + y5))*z - x5*y2*z1 + x2*y5*z1 + x5*y1*z2 - x1*y5*z2 - x2*y1*z5 + x1*y2*z5 + y*(x5*(z1 - z2) + x1*(z2 - z5) + x2*(-z1 + z5)) + x*(y5*(-z1 + z2) + y2*(z1 - z5) + y1*(-z2 + z5)))
#define face0347(x,y,z) ((x4*(-y0 + y3) + x3*(y0 - y4) + x0*(-y3 + y4))*z - x4*y3*z0 + x3*y4*z0 + x4*y0*z3 - x0*y4*z3 - x3*y0*z4 + x0*y3*z4 + y*(x4*(z0 - z3) + x0*(z3 - z4) + x3*(-z0 + z4)) + x*(y4*(-z0 + z3) + y3*(z0 - z4) + y0*(-z3 + z4)))
#define face0145(x,y,z) ((x4*(-y0 + y1) + x1*(y0 - y4) + x0*(-y1 + y4))*z - x4*y1*z0 + x1*y4*z0 + x4*y0*z1 - x0*y4*z1 - x1*y0*z4 + x0*y1*z4 + y*(x4*(z0 - z1) + x0*(z1 - z4) + x1*(-z0 + z4)) + x*(y4*(-z0 + z1) + y1*(z0 - z4) + y0*(-z1 + z4)))
#define face0123(x,y,z) ((x2*(-y0 + y1) + x1*(y0 - y2) + x0*(-y1 + y2))*z - x2*y1*z0 + x1*y2*z0 + x2*y0*z1 - x0*y2*z1 - x1*y0*z2 + x0*y1*z2 + y*(x2*(z0 - z1) + x0*(z1 - z2) + x1*(-z0 + z2)) + x*(y2*(-z0 + z1) + y1*(z0 - z2) + y0*(-z1 + z2)))
// des macros définissent pour une face triangulaire orientée vers l'extérieur de la maille la fonction de projection, non normalisée ( =(12^13).1M )
#define face(x1,y1,z1,x2,y2,z2,x3,y3,z3,x,y,z)  ( ((y2-y1)*(z3-z1)-(z2-z1)*(y3-y1))*(x-x1)+((z2-z1)*(x3-x1)-(x2-x1)*(z3-z1))*(y-y1)+((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1))*(z-z1) )
#define projection(x1,y1,z1,x2,y2,z2,x3,y3,z3,x0,y0,z0,x,y,z) (face(x1,y1,z1,x2,y2,z2,x3,y3,z3,x,y,z)/face(x1,y1,z1,x2,y2,z2,x3,y3,z3,x0,y0,z0))

// DECLARATION

_TEMPLATE_ class Calcul_Interpolation
{
protected :
        NUAGENOEUD * noeuds;
        NUAGEMAILLE * mailles;
        CHAMP * champ;
public : 
        Calcul_Interpolation():noeuds(NULL),mailles(NULL),champ(NULL) {}
        Calcul_Interpolation(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):noeuds(nn),mailles(nm),champ(c) {}
        ~Calcul_Interpolation() {}
        virtual VALEURCHAMP operator() (const NOEUD & n, int num_maille) {cerr<<"APPEL OPERATOR() DE LA CLASSE MERE CALCUL_INTERPOLATION => EXIT(-1)"<<endl;exit(-1);}
};

_TEMPLATE_ class Calcul_Interpolation_P0;

_TEMPLATE_ class Calcul_Interpolation_Tria3;
_TEMPLATE_ class Calcul_Interpolation_Tria6;
_TEMPLATE_ class Calcul_Interpolation_Quad4;
_TEMPLATE_ class Calcul_Interpolation_Quad8;
_TEMPLATE_ class Calcul_Interpolation_Tetra4;
_TEMPLATE_ class Calcul_Interpolation_Tetra10;
_TEMPLATE_ class Calcul_Interpolation_Hexa8;
_TEMPLATE_ class Calcul_Interpolation_Hexa20;
_TEMPLATE_ class Calcul_Interpolation_Penta6;
_TEMPLATE_ class Calcul_Interpolation_Penta15;
_TEMPLATE_ class Calcul_Interpolation_Pyra5;
_TEMPLATE_ class Calcul_Interpolation_Pyra13;

_TEMPLATE_ class Calcul_Hybride
{
protected :
        NUAGEMAILLE * mailles;
        std::map<int,Calcul_Interpolation<_PARAM_> *> fonctions;
public : 
        Calcul_Hybride():mailles(NULL) {}
        Calcul_Hybride(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c);
        ~Calcul_Hybride() {}
        VALEURCHAMP operator() (const NOEUD & n, int num_maille);
};

//CODE

_TEMPLATE_ Calcul_Hybride<_PARAM_>::Calcul_Hybride(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):mailles(nm)
        {
        fonctions[ MED_EN::MED_TRIA3  ]=new Calcul_Interpolation_Tria3  <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_TRIA6  ]=new Calcul_Interpolation_Tria6  <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_QUAD4  ]=new Calcul_Interpolation_Quad4  <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_QUAD8  ]=new Calcul_Interpolation_Quad8  <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_TETRA4 ]=new Calcul_Interpolation_Tetra4 <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_TETRA10]=new Calcul_Interpolation_Tetra10<_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_HEXA8  ]=new Calcul_Interpolation_Hexa8  <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_HEXA20 ]=new Calcul_Interpolation_Hexa20 <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_PENTA6 ]=new Calcul_Interpolation_Penta6 <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_PENTA15]=new Calcul_Interpolation_Penta15<_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_PYRA5  ]=new Calcul_Interpolation_Pyra5  <_PARAM_>(nn,nm,c);
        fonctions[ MED_EN::MED_PYRA13 ]=new Calcul_Interpolation_Pyra13 <_PARAM_>(nn,nm,c);
        }

_TEMPLATE_ VALEURCHAMP Calcul_Hybride<_PARAM_>::operator() (const NOEUD & n, int num_maille)
        {
        return fonctions[(*mailles)[num_maille].DONNE_TYPE_MED_MAILLE()]->operator()(n,num_maille);
        }

_TEMPLATE_ class Calcul_Interpolation_P0      : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_P0(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        return (*Calcul_Interpolation<_PARAM_>::champ)[num_maille];
        }
};
_TEMPLATE_ class Calcul_Interpolation_Tria3   : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Tria3(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        int num0=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];
        double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];
        double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];
        double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];
        
        double x=n[0];
        double y=n[1];
        
        double lambda0=(y1-y2)*x+(x2-x1)*y+(x1*y2-x2*y1);
        double lambda1=(y2-y0)*x+(x0-x2)*y+(x2*y0-x0*y2);
        double lambda2=(y0-y1)*x+(x1-x0)*y+(x0*y1-x1*y0);
        
        double delta = (x2-x1)*y0+(x0-x2)*y1+(x1-x0)*y2;
        
        VALEURCHAMP retour(v0.SIZE());
                        
        retour=(1/delta)*(lambda0*v0+lambda1*v1+lambda2*v2);
        
        return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Tria6   : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Tria6(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        // ON SUPPOSE IMPLICITEMENT QUE LES NOEUDS SUPPLEMENTAIRES SONT BIEN DES NOEUDS MILIEUX
        int num0 =(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1 =(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2 =(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        int num01=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][3];
        int num12=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][4];
        int num20=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][5];
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];
        double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];
        double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];
        double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];
        VALEURCHAMP v01=(*Calcul_Interpolation<_PARAM_>::champ)[num01];
        VALEURCHAMP v12=(*Calcul_Interpolation<_PARAM_>::champ)[num12];
        VALEURCHAMP v20=(*Calcul_Interpolation<_PARAM_>::champ)[num20];
        
        double x=n[0];
        double y=n[1];
        
        double lambda0=(y1-y2)*x+(x2-x1)*y+(x1*y2-x2*y1);
        double lambda1=(y2-y0)*x+(x0-x2)*y+(x2*y0-x0*y2);
        double lambda2=(y0-y1)*x+(x1-x0)*y+(x0*y1-x1*y0);
        
        double delta = (x2-x1)*y0+(x0-x2)*y1+(x1-x0)*y2;


        // VALEURCHAMP retour(v0.SIZE()); //            

        return  2*(lambda0*lambda0*v0+
                   lambda1*lambda1*v1+
                   lambda2*lambda2*v2+
                   2*(lambda0*lambda1*v01+
                      lambda1*lambda2*v12+
                      lambda2*lambda0*v20))/(delta*delta)+
                (lambda0*v0+lambda1*v1+lambda2*v2)/(-delta);

        // return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Quad4   : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Quad4(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        int num0=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        int num3=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][3];
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];
        double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];
        double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];
        double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];
        double x3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][0];
        double y3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][1];
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];
        VALEURCHAMP v3=(*Calcul_Interpolation<_PARAM_>::champ)[num3];
        
        double x=n[0];
        double y=n[1];
        
        
        double mu0=-((x3*(y1-y2)+x1*(y2-y3)+x2*(y3-y1))*x*y+(x2*y2*(y1-y3)+x3*(-y1+y2)*y3+x1*y1*(y3-y2))*x+(x2*x3*(y2-y3)+x1*(x2*(y1-y2)+x3*(y3-y1)))*y+(x2*x3*y1*(y3-y2)+x1*(x3*y2*(y1-y3)+x2*(y2-y1)*y3)));
        double mu1=(x0*(y2-y3)+x2*(y3-y0)+x3*(y0-y2))*x*y+(x3*y3*(y2-y0)+x0*(-y2+y3)*y0+x2*y2*(y0-y3))*x+(x3*x0*(y3-y0)+x2*(x3*(y2-y3)+x0*(y0-y2)))*y+(x2*x0*y2*(y0-y2)+x2*(x0*y2*(y2-y0)+x3*(y3-y2)*y0));
        double mu2=-((x1*(y3-y0)+x3*(y0-y1)+x0*(y1-y3))*x*y+(x0*y0*(y3-y1)+x1*(-y3+y0)*y1+x3*y3*(y1-y0))*x+(x0*x1*(y0-y1)+x3*(x0*(y3-y0)+x1*(y1-y3)))*y+(x3*x1*y3*(y1-y2)+x3*(x1*y2*(y3-y1)+x0*(y0-y3)*y1)));
        double mu3=(x2*(y0-y1)+x0*(y1-y2)+x1*(y2-y0))*x*y+(x1*y1*(y0-y2)+x2*(-y0+y1)*y2+x0*y0*(y2-y1))*x+(x1*x2*(y1-y2)+x0*(x1*(y0-y1)+x2*(y2-y0)))*y+(x0*x2*y0*(y2-y2)+x0*(x2*y2*(y0-y2)+x1*(y1-y0)*y2));
        
        double delta=(y0-y2)*(y1-y3)*(x0*x2+x1*x3)-(x1*x2+x0*x3)*(y1-y2)*(y0-y3)-(x0*x1+x2*x3)*(y0-y1)*(y2-y3);

        /*
        cout<<"  ### Pour ( "<<x<<" , "<<y<<" )"<<endl;
        cout<<"  ### delta = "<<delta<<endl;
        cout<<"  ### Mu0 = "<<mu0<<endl;
        cout<<"  ### Mu1 = "<<mu1<<endl;
        cout<<"  ### Mu2 = "<<mu2<<endl;
        cout<<"  ### Mu3 = "<<mu3<<endl;
        */
        VALEURCHAMP retour(v0.SIZE()); //               

        retour=(mu0*v0+mu1*v1+mu2*v2+mu3*v3)/delta;

        return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Quad8   : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Quad8(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        cerr<<"Interpolation Q2 pas encore implémentée"<<endl;
        exit(-1);
        }
};
_TEMPLATE_ class Calcul_Interpolation_Tetra4  : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Tetra4(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        int num0=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        int num3=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][3];
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];
        double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];
        double z0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][2];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];
        double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];
        double z1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][2];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];
        double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];
        double z2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][2];
        double x3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][0];
        double y3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][1];
        double z3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][2];
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];
        VALEURCHAMP v3=(*Calcul_Interpolation<_PARAM_>::champ)[num3];
        
        double x=n[0];
        double y=n[1];
        double z=n[2];
        
        double lambda0=face(x1,y1,z1,x2,y2,z2,x3,y3,z3,x,y,z)/face(x1,y1,z1,x2,y2,z2,x3,y3,z3,x0,y0,z0);
        double lambda1=face(x0,y0,z0,x2,y2,z2,x3,y3,z3,x,y,z)/face(x0,y0,z0,x2,y2,z2,x3,y3,z3,x1,y1,z1);
        double lambda2=face(x1,y1,z1,x0,y0,z0,x3,y3,z3,x,y,z)/face(x1,y1,z1,x0,y0,z0,x3,y3,z3,x2,y2,z2);
        double lambda3=face(x1,y1,z1,x2,y2,z2,x0,y0,z0,x,y,z)/face(x1,y1,z1,x2,y2,z2,x0,y0,z0,x3,y3,z3);
        
        VALEURCHAMP retour(v0.SIZE()); //               

        retour=(lambda0*v0+lambda1*v1+lambda2*v2+lambda3*v3);

        return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Tetra10 : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Tetra10(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        // ON SUPPOSE IMPLICITEMENT QUE LES NOEUDS SUPPLEMENTAIRES SONT BIEN DES NOEUDS MILIEUX
        int num0 =(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1 =(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2 =(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        int num3 =(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][3];
        int num01=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][4];
        int num02=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][6];
        int num03=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][7];
        int num12=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][5];
        int num13=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][8];
        int num23=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][9];
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];double z0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][2];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];double z1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][2];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];double z2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][2];
        double x3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][0];double y3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][1];double z3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][2];
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];  
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];  
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];  
        VALEURCHAMP v3=(*Calcul_Interpolation<_PARAM_>::champ)[num3];  
        VALEURCHAMP v01=(*Calcul_Interpolation<_PARAM_>::champ)[num01];
        VALEURCHAMP v02=(*Calcul_Interpolation<_PARAM_>::champ)[num02];
        VALEURCHAMP v03=(*Calcul_Interpolation<_PARAM_>::champ)[num03];
        VALEURCHAMP v12=(*Calcul_Interpolation<_PARAM_>::champ)[num12];
        VALEURCHAMP v13=(*Calcul_Interpolation<_PARAM_>::champ)[num13];
        VALEURCHAMP v23=(*Calcul_Interpolation<_PARAM_>::champ)[num23];
        
        double x=n[0];
        double y=n[1];
        double z=n[2];
        
        double lambda0=face(x1,y1,z1,x2,y2,z2,x3,y3,z3,x,y,z)/face(x1,y1,z1,x2,y2,z2,x3,y3,z3,x0,y0,z0);
        double lambda1=face(x0,y0,z0,x2,y2,z2,x3,y3,z3,x,y,z)/face(x0,y0,z0,x2,y2,z2,x3,y3,z3,x1,y1,z1);
        double lambda2=face(x1,y1,z1,x0,y0,z0,x3,y3,z3,x,y,z)/face(x1,y1,z1,x0,y0,z0,x3,y3,z3,x2,y2,z2);
        double lambda3=face(x1,y1,z1,x2,y2,z2,x0,y0,z0,x,y,z)/face(x1,y1,z1,x2,y2,z2,x0,y0,z0,x3,y3,z3);
        
        /*
        cout<<"  ### Pour ( "<<x<<" , "<<y<<" , "<<z<<" )"<<endl;
        cout<<"  ### Lambda0 = "<<lambda0<<endl;
        cout<<"  ### Lambda1 = "<<lambda1<<endl;
        cout<<"  ### Lambda2 = "<<lambda2<<endl;
        cout<<"  ### Lambda3 = "<<lambda3<<endl;
        cout<<"  ### Controle = "<<(lambda0+lambda1+lambda2+lambda3)<<endl;
        */

        VALEURCHAMP retour(v0.SIZE()); //               

        retour=2*(lambda0*lambda0*v0+
                  lambda1*lambda1*v1+
                  lambda2*lambda2*v2+
                  lambda3*lambda3*v3+
                  2*(lambda0*lambda1*v01+
                     lambda0*lambda2*v02+
                     lambda0*lambda3*v03+
                     lambda1*lambda2*v12+
                     lambda1*lambda3*v13+
                     lambda2*lambda3*v23))+
               (-1.0)*(lambda0*v0+lambda1*v1+lambda2*v2+lambda3*v3);

        return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Hexa8   : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Hexa8(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        
        // Tres probablement numériquement mauvais, à revoir
        
        int num0=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        int num3=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][3];
        int num4=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][4];
        int num5=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][5];
        int num6=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][6];
        int num7=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][7];
        
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];double z0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][2];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];double z1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][2];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];double z2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][2];
        double x3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][0];double y3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][1];double z3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][2];
        double x4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][0];double y4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][1];double z4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][2];
        double x5=(*Calcul_Interpolation<_PARAM_>::noeuds)[num5][0];double y5=(*Calcul_Interpolation<_PARAM_>::noeuds)[num5][1];double z5=(*Calcul_Interpolation<_PARAM_>::noeuds)[num5][2];
        double x6=(*Calcul_Interpolation<_PARAM_>::noeuds)[num6][0];double y6=(*Calcul_Interpolation<_PARAM_>::noeuds)[num6][1];double z6=(*Calcul_Interpolation<_PARAM_>::noeuds)[num6][2];
        double x7=(*Calcul_Interpolation<_PARAM_>::noeuds)[num7][0];double y7=(*Calcul_Interpolation<_PARAM_>::noeuds)[num7][1];double z7=(*Calcul_Interpolation<_PARAM_>::noeuds)[num7][2];
        
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];
        VALEURCHAMP v3=(*Calcul_Interpolation<_PARAM_>::champ)[num3];
        VALEURCHAMP v4=(*Calcul_Interpolation<_PARAM_>::champ)[num4];
        VALEURCHAMP v5=(*Calcul_Interpolation<_PARAM_>::champ)[num5];
        VALEURCHAMP v6=(*Calcul_Interpolation<_PARAM_>::champ)[num6];
        VALEURCHAMP v7=(*Calcul_Interpolation<_PARAM_>::champ)[num7];

        double x=n[0];
        double y=n[1];
        double z=n[2];
        
        // double mu0=face1256(x,y,z)*face2367(x,y,z)*face4567(x,y,z)/(face1256(x0,y0,z0)*face2367(x0,y0,z0)*face4567(x0,y0,z0));
        // double mu1=face0347(x,y,z)*face2367(x,y,z)*face4567(x,y,z)/(face0347(x1,y1,z1)*face2367(x1,y1,z1)*face4567(x1,y1,z1));
        // double mu2=face0347(x,y,z)*face0145(x,y,z)*face4567(x,y,z)/(face0347(x2,y2,z2)*face0145(x2,y2,z2)*face4567(x2,y2,z2));
        // double mu3=face1256(x,y,z)*face0145(x,y,z)*face4567(x,y,z)/(face1256(x3,y3,z3)*face0145(x3,y3,z3)*face4567(x3,y3,z3));
        // double mu4=face1256(x,y,z)*face2367(x,y,z)*face0123(x,y,z)/(face1256(x4,y4,z4)*face2367(x4,y4,z4)*face0123(x4,y4,z4));
        // double mu5=face0347(x,y,z)*face2367(x,y,z)*face0123(x,y,z)/(face0347(x5,y5,z5)*face2367(x5,y5,z5)*face0123(x5,y5,z5));
        // double mu6=face0347(x,y,z)*face0145(x,y,z)*face0123(x,y,z)/(face0347(x6,y6,z6)*face0145(x6,y6,z6)*face0123(x6,y6,z6));
        // double mu7=face1256(x,y,z)*face0145(x,y,z)*face0123(x,y,z)/(face1256(x7,y7,z7)*face0145(x7,y7,z7)*face0123(x7,y7,z7));

        double mu0=projection(x1,y1,z1,x2,y2,z2,x6,y6,z6,x0,y0,z0,x,y,z)*projection(x2,y2,z2,x3,y3,z3,x7,y7,z7,x0,y0,z0,x,y,z)*projection(x4,y4,z4,x5,y5,z5,x6,y6,z6,x0,y0,z0,x,y,z);
        double mu1=projection(x0,y0,z0,x3,y3,z3,x7,y7,z7,x1,y1,z1,x,y,z)*projection(x2,y2,z2,x3,y3,z3,x7,y7,z7,x1,y1,z1,x,y,z)*projection(x4,y4,z4,x5,y5,z5,x6,y6,z6,x1,y1,z1,x,y,z);
        double mu2=projection(x0,y0,z0,x3,y3,z3,x7,y7,z7,x2,y2,z2,x,y,z)*projection(x0,y0,z0,x1,y1,z1,x5,y5,z5,x2,y2,z2,x,y,z)*projection(x4,y4,z4,x5,y5,z5,x6,y6,z6,x2,y2,z2,x,y,z);
        double mu3=projection(x1,y1,z1,x2,y2,z2,x6,y6,z6,x3,y3,z3,x,y,z)*projection(x0,y0,z0,x1,y1,z1,x5,y5,z5,x3,y3,z3,x,y,z)*projection(x4,y4,z4,x6,y6,z6,x7,y7,z7,x3,y3,z3,x,y,z);
        double mu4=projection(x2,y2,z2,x6,y6,z6,x5,y5,z5,x4,y4,z4,x,y,z)*projection(x2,y2,z2,x3,y3,z3,x7,y7,z7,x4,y4,z4,x,y,z)*projection(x0,y0,z0,x1,y1,z1,x2,y2,z2,x4,y4,z4,x,y,z);
        double mu5=projection(x3,y3,z3,x7,y7,z7,x4,y4,z4,x5,y5,z5,x,y,z)*projection(x2,y2,z2,x3,y3,z3,x6,y6,z6,x5,y5,z5,x,y,z)*projection(x0,y0,z0,x1,y1,z1,x2,y2,z2,x5,y5,z5,x,y,z);
        double mu6=projection(x3,y3,z3,x7,y7,z7,x4,y4,z4,x6,y6,z6,x,y,z)*projection(x1,y1,z1,x5,y5,z5,x4,y4,z4,x6,y6,z6,x,y,z)*projection(x0,y0,z0,x1,y1,z1,x2,y2,z2,x6,y6,z6,x,y,z);
        double mu7=projection(x2,y2,z2,x6,y6,z6,x5,y5,z5,x7,y7,z7,x,y,z)*projection(x1,y1,z1,x5,y5,z5,x4,y4,z4,x7,y7,z7,x,y,z)*projection(x0,y0,z0,x1,y1,z1,x3,y3,z3,x7,y7,z7,x,y,z);
                   
        VALEURCHAMP retour(v0.SIZE()); //               

        retour=(mu0*v0+mu1*v1+mu2*v2+mu3*v3+mu4*v4+mu5*v5+mu6*v6+mu7*v7);

        return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Hexa20  : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Hexa20(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        cerr<<"Interpolation H2 pasencore implémentée"<<endl;
        exit(-1);
        }
};
_TEMPLATE_ class Calcul_Interpolation_Penta6  : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Penta6(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        int num0=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        int num3=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][3];
        int num4=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][4];
        int num5=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][5];    
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];double z0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][2];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];double z1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][2];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];double z2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][2];
        double x3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][0];double y3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][1];double z3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][2];
        double x4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][0];double y4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][1];double z4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][2];
        double x5=(*Calcul_Interpolation<_PARAM_>::noeuds)[num5][0];double y5=(*Calcul_Interpolation<_PARAM_>::noeuds)[num5][1];double z5=(*Calcul_Interpolation<_PARAM_>::noeuds)[num5][2];    
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];
        VALEURCHAMP v3=(*Calcul_Interpolation<_PARAM_>::champ)[num3];
        VALEURCHAMP v4=(*Calcul_Interpolation<_PARAM_>::champ)[num4];
        VALEURCHAMP v5=(*Calcul_Interpolation<_PARAM_>::champ)[num5];

        double x=n[0];
        double y=n[1];
        double z=n[2];
        
        double mu0=face(x1,y1,z1,x2,y2,z2,x5,y5,z5,x,y,z)*face(x3,y3,z3,x4,y4,z4,x5,y5,z5,x,y,z)/(face(x1,y1,z1,x2,y2,z2,x5,y5,z5,x0,y0,z0)*face(x3,y3,z3,x4,y4,z4,x5,y5,z5,x0,y0,z0));
        double mu1=face(x0,y0,z0,x2,y2,z2,x5,y5,z5,x,y,z)*face(x3,y3,z3,x4,y4,z4,x5,y5,z5,x,y,z)/(face(x0,y0,z0,x2,y2,z2,x5,y5,z5,x1,y1,z1)*face(x3,y3,z3,x4,y4,z4,x5,y5,z5,x1,y1,z1));
        double mu2=face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x,y,z)*face(x3,y3,z3,x4,y4,z4,x5,y5,z5,x,y,z)/(face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x2,y2,z2)*face(x3,y3,z3,x4,y4,z4,x5,y5,z5,x2,y2,z2));
        double mu3=face(x1,y1,z1,x2,y2,z2,x5,y5,z5,x,y,z)*face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x,y,z)/(face(x1,y1,z1,x2,y2,z2,x5,y5,z5,x3,y3,z3)*face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3));
        double mu4=face(x0,y0,z0,x2,y2,z2,x5,y5,z5,x,y,z)*face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x,y,z)/(face(x0,y0,z0,x2,y2,z2,x5,y5,z5,x4,y4,z4)*face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x4,y4,z4));
        double mu5=face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x,y,z)*face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x,y,z)/(face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x5,y5,z5)*face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x5,y5,z5));

        VALEURCHAMP retour(v0.SIZE()); //               

        retour=(mu0*v0+mu1*v1+mu2*v2+mu3*v3+mu4*v4+mu5*v5);

        return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Penta15 : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Penta15(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        cerr<<"Interpolation Pe2 pasencore implémentée"<<endl;
        exit(-1);
        }
};
_TEMPLATE_ class Calcul_Interpolation_Pyra5   : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Pyra5(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        // NON TESTE
        int num0=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][0];
        int num1=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][1];
        int num2=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][2];
        int num3=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][3];
        int num4=(*Calcul_Interpolation<_PARAM_>::mailles)[num_maille][4];
        
        double x0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][0];double y0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][1];double z0=(*Calcul_Interpolation<_PARAM_>::noeuds)[num0][2];
        double x1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][0];double y1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][1];double z1=(*Calcul_Interpolation<_PARAM_>::noeuds)[num1][2];
        double x2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][0];double y2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][1];double z2=(*Calcul_Interpolation<_PARAM_>::noeuds)[num2][2];
        double x3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][0];double y3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][1];double z3=(*Calcul_Interpolation<_PARAM_>::noeuds)[num3][2];
        double x4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][0];double y4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][1];double z4=(*Calcul_Interpolation<_PARAM_>::noeuds)[num4][2];
        
        VALEURCHAMP v0=(*Calcul_Interpolation<_PARAM_>::champ)[num0];
        VALEURCHAMP v1=(*Calcul_Interpolation<_PARAM_>::champ)[num1];
        VALEURCHAMP v2=(*Calcul_Interpolation<_PARAM_>::champ)[num2];
        VALEURCHAMP v3=(*Calcul_Interpolation<_PARAM_>::champ)[num3];
        VALEURCHAMP v4=(*Calcul_Interpolation<_PARAM_>::champ)[num4];

        double x=n[0];
        double y=n[1];
        double z=n[2];
        
        double mu0=face(x1,y1,z1,x2,y2,z2,x4,y4,z4,x,y,z)*face(x2,y2,z2,x3,y3,z3,x4,y4,z4,x,y,z)/(face(x1,y1,z1,x2,y2,z2,x4,y4,z4,x0,y0,z0)*face(x2,y2,z2,x3,y3,z3,x4,y4,z4,x0,y0,z0));
        double mu1=face(x0,y0,z0,x3,y3,z3,x4,y4,z4,x,y,z)*face(x2,y2,z2,x3,y3,z3,x4,y4,z4,x,y,z)/(face(x0,y0,z0,x3,y3,z3,x4,y4,z4,x1,y1,z1)*face(x2,y2,z2,x3,y3,z3,x4,y4,z4,x1,y1,z1));
        double mu2=face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x,y,z)*face(x0,y0,z0,x3,y3,z3,x4,y4,z4,x,y,z)/(face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x2,y2,z2)*face(x0,y0,z0,x3,y3,z3,x4,y4,z4,x2,y2,z2));
        double mu3=face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x,y,z)*face(x1,y1,z1,x2,y2,z2,x4,y4,z4,x,y,z)/(face(x0,y0,z0,x1,y1,z1,x4,y4,z4,x3,y3,z3)*face(x1,y1,z1,x2,y2,z2,x4,y4,z4,x3,y3,z3));
        double mu4=face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x,y,z)/face(x0,y0,z0,x1,y1,z1,x2,y2,z2,x4,y4,z4);

        VALEURCHAMP retour(v0.SIZE()); //               

        retour=(mu0*v0+mu1*v1+mu2*v2+mu3*v3+mu4*v4);

        return retour; //
        }
};
_TEMPLATE_ class Calcul_Interpolation_Pyra13  : public Calcul_Interpolation<_PARAM_>
{
public : Calcul_Interpolation_Pyra13(NUAGENOEUD * nn,NUAGEMAILLE * nm,CHAMP * c):Calcul_Interpolation<_PARAM_>(nn,nm,c) {}
public : VALEURCHAMP operator() (const NOEUD & n, int num_maille)
        {
        cerr<<"Interpolation Py2 pasencore implémentée"<<endl;
        exit(-1);
        }
};

#undef _TEMPLATE_ 
#undef _PARAM_

#endif
