//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : testUUnit.cxx
//  Module : MED

#include "InterpolationUtils.hxx"
using namespace std;

int main ()
{
  cerr<< endl;
  cerr<< " ************ Test 1  ************ "<< endl;

  const double Losange1[12]= 
    {
      1,0,0,
      0,1,0,
      -1,0,0,
      0,-1,0
    };
  
  const double Losange2[12]=
    {
      2,0,0,
      1,1,0,
      0,0,0,
      1,-1,0
    };

  deque< double > resultat_test1 = INTERP_UTILS::intersect_convex_polygons(Losange1,Losange2,4,4,1,1e-6);

  cerr<<  " Test1: Résultat théorique" << endl;
  cerr<< 0.5 << " ,"<< -0.5 << " ," << 0 << endl;
  cerr<<  0 << " ,"<< 0 << " ,"<< 0 << endl;
  cerr<< 0.5 << " ,"<< 0.5 << " ,"<< 0 << endl;
  cerr<< 1 << " ," << 0 << " ," << 0 << endl;

  cerr<< " Test1: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test1.size()/3; i++)
    {
      cerr << resultat_test1[3*i] << "  ";
      cerr << resultat_test1[3*i+1] << "  ";
      cerr << resultat_test1[3*i+2] << "  ";
      cerr << endl ;
    }

  cerr<< " ************ Test 2 ************ "<< endl;
  
  const double Losange3[12]=
    {
      2.5,0.5,0,
      1.5,1.5,0,
      0.5,0.5,0,
      1.5,-0.5,0
    };
  deque< double > resultat_test2 = INTERP_UTILS::intersect_convex_polygons(Losange1,Losange3,4,4,1,1e-6);

  cerr<<  " Test2: Résultat théorique" << endl;
//   cerr<< 0.5 << " ,"<< 0.5 << " ," << 0 << endl;
//   cerr<< 1 << " ,"<< 0 << " ," << 0 << endl;

  cerr<< " Test2: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test2.size()/3; i++)
    {
      cerr << resultat_test2[3*i] << "  ";
      cerr << resultat_test2[3*i+1] << "  ";
      cerr << resultat_test2[3*i+2] << "  ";
      cerr << endl ;
    }

  cerr<< " ************ Test 3  ************ "<< endl;
  
  const double Carre1[12]=
    {
      -1,-1,0,
      -1,1,0,
      1,1,0,
      1,-1,0
    };
  const double Carre2[12]=
    {
      1,-0.25,0,
      0,-0.25,0,
      0,0.25,0,
      1,0.25,0
    };
 deque< double > resultat_test3 = INTERP_UTILS::intersect_convex_polygons( Carre1, Carre2,4,4,1,1e-6);

  cerr<<  " Test3: Résultat théorique" << endl;
  cerr<< 0 << " ,"<< 0.25 << " ," << 0 << endl;
  cerr<< 0 << " ,"<< -0.25 << " ," << 0 << endl;
  cerr<< 1 << " ,"<< -0.25 << " ,"<< 0 << endl;
  cerr<< 1 << " ,"<< 0.25 << " ," << 0 << endl;

  cerr<< " Test3: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test3.size()/3; i++)
    {
      cerr << resultat_test3[3*i] << "  ";
      cerr << resultat_test3[3*i+1] << "  ";
      cerr << resultat_test3[3*i+2] << "  ";
      cerr << endl ;
    }
  cerr<< " ***************** Test 4 ***************** " << endl;
  
  const double Losange4[12]=
    {
      3,0,0,
      2,1,0,
      1,0,0,
      2,-1,0
    };
 deque< double > resultat_test4 = INTERP_UTILS::intersect_convex_polygons( Losange1, Losange4,4,4,1,1e-6);

  cerr<<  " Test4: Résultat théorique" << endl;
  cerr<< 1 << " ,"<< 0 << " ,"<< 0 << endl;

  cerr<< " Test4: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test4.size()/3; i++)
    {
      cerr << resultat_test4[3*i] << "  ";
      cerr << resultat_test4[3*i+1] << "  ";
      cerr << resultat_test4[3*i+2] << "  ";
      cerr << endl ;
    }

  cerr<< " ***************** Test 5 ***************** " << endl;
 deque< double > resultat_test5 = INTERP_UTILS::intersect_convex_polygons( Carre1, Carre1,4,4,1,1e-6);

  cerr<<  " Test5: Résultat théorique" << endl;
  cerr<< -1 << " ,"<<  1 << " ," << 0 << endl;
  cerr<< -1 << " ,"<< -1 << " ," << 0 << endl;
  cerr<<  1 << " ,"<< -1 << " ," << 0 << endl;
  cerr<<  1 << " ,"<<  1 << " ," << 0 << endl;

  cerr<< " Test5: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test5.size()/3; i++)
    {
      cerr << resultat_test5[3*i] << "  ";
      cerr << resultat_test5[3*i+1] << "  ";
      cerr << resultat_test5[3*i+2] << "  ";
      cerr << endl ;
    }

  cerr<< " ***************** Test 6 ***************** " << endl;

  const double Losange5[12]= 
    {
      1.5,0,0,
      0,1.5,0,
      -1.5,0,0,
      0,-1.5,0
    };
 deque< double > resultat_test6 = INTERP_UTILS::intersect_convex_polygons( Carre1, Losange5,4,4,1,1e-6);

  cerr<<  " Test6: Résultat théorique" << endl;
  cerr<< 1 << " ,"<< -0.5 << " ," << 0 << endl;
  cerr<< 0.5 << " ,"<< -1 << " ," << 0 << endl;
  cerr<< -0.5 << " ,"<< -1 << " ," << 0 << endl;
  cerr<< -1 << " ,"<< -0.5 << " ," << 0 << endl;
  cerr<< -1 << " ,"<< 0.5 << " ," << 0 << endl;
  cerr<< -0.5 << " ,"<< 1 << " ," << 0 << endl;
  cerr<< 0.5 << " ,"<< 1 << " ,"<< 0 << endl;
  cerr<< 1 << " ,"<< 0.5 << " ," << 0 << endl;

  cerr<< " Test6: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test6.size()/3; i++)
    {
      cerr << resultat_test6[3*i] << "  ";
      cerr << resultat_test6[3*i+1] << "  ";
      cerr << resultat_test6[3*i+2] << "  ";
      cerr << endl ;
    }
  cerr<< " ***************** Test 7 ***************** " << endl;

  deque< double > resultat_test7 = INTERP_UTILS::intersect_convex_polygons( Losange1, Carre1,4,4,1,1e-6);

  cerr<<  " Test7: Résultat théorique" << endl;
  cerr<< 0 << " ,"<< -1 << " ," << 0 << endl;
  cerr<< -1 << " ,"<< 0 << " ," << 0 << endl;
  cerr<< 0 << " ,"<< 1 << " ,"<< 0 << endl;
  cerr<< 1 << " ,"<< 0 << " ," << 0 << endl;

  cerr<< " Test7: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test7.size()/3; i++)
    {
      cerr << resultat_test7[3*i] << "  ";
      cerr << resultat_test7[3*i+1] << "  ";
      cerr << resultat_test7[3*i+2] << "  ";
      cerr << endl ;
    }
  cerr<< " ************ Test 8  ************ "<< endl;

  const double Losange6[18]=
    {
      2,0,0,
      1,1,0,
      0.5,0.5,0,
      0,0,0,
      0.5,-0.5,0,
      1,-1,0
    };
  const double Losange7[15]= 
    {
      1,0,0,
      0,1,0,
      -1,0,0,
      0,-1,0,
      0.5,-0.5,0
    };
  
  deque< double > resultat_test8 = INTERP_UTILS::intersect_convex_polygons(Losange6,Losange7,6,5,1,1e-6);

  cerr<<  " Test8: Résultat théorique" << endl;
  cerr<< 0.5 << " ,"<< -0.5 << " ," << 0 << endl;
  cerr<<  0 << " ,"<< 0 << " ,"<< 0 << endl;
  cerr<< 0.5 << " ,"<< 0.5 << " ,"<< 0 << endl;
  cerr<< 1 << " ," << 0 << " ," << 0 << endl;

  cerr<< " Test8: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test1.size()/3; i++)
    {
      cerr << resultat_test1[3*i] << "  ";
      cerr << resultat_test1[3*i+1] << "  ";
      cerr << resultat_test1[3*i+2] << "  ";
      cerr << endl ;
    }
  cerr<< " ************ Test 9  ************ "<< endl;
  const double Carre3[15]=
    {
      -1,-1,0,
      -1,1,0,
      0.5,1,0,
      1,1,0,
      1,-1,0
    };
  const double Carre4[12]=
    {
      -0.5,-1,0,
      -0.5,1,0,
      1.5,1,0,
      1.5,-1,0
    };

  deque< double > resultat_test9 = INTERP_UTILS::intersect_convex_polygons(Carre4,Carre3,4,5,1,1e-6);

  cerr<<  " Test9: Résultat théorique" << endl;
  cerr<< -0.5 << " ,"<< -1 << " ," << 0 << endl;
  cerr<< -0.5 << " ,"<<  1 << " ," << 0 << endl;
  cerr<<  1   << " ,"<< 1 << " ," << 0 << endl;
  cerr<<  1   << " ,"<< -1 << " ," << 0 << endl;

  cerr<< " Test9: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test9.size()/3; i++)
    {
      cerr << resultat_test9[3*i] << "  ";
      cerr << resultat_test9[3*i+1] << "  ";
      cerr << resultat_test9[3*i+2] << "  ";
      cerr << endl ;
    }

  cerr<< " ************ Test 10  ************ "<< endl;
  const double Carre5[15]=
    {
      -1,-1,0,
      -1,1,0,
      0,1,0,
      1,1,0,
      1,-1,0
    };
  const double Losange8[12]=
    {
      0,1,0,
      1,-1,0,
      0,-1.5,0,
      -0.5,-1,0
    };
  deque< double > resultat_test10 = INTERP_UTILS::intersect_convex_polygons(Losange8,Carre5,4,5,1,1e-6);
  cerr<<  " Test10: Résultat théorique" << endl;
  cerr<< 0 << " ,"<< 1 << " ," << 0 << endl;
  cerr<< -0.5 << " ,"<< -1 << " ," << 0 << endl;
  cerr<< 1 << " ,"<< -1 << " ," << 0 << endl;

  cerr<< " Test10: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test10.size()/3; i++)
    {
      cerr << resultat_test10[3*i] << "  ";
      cerr << resultat_test10[3*i+1] << "  ";
      cerr << resultat_test10[3*i+2] << "  ";
      cerr << endl ;
    }

  cerr<< " ************ Test 11  ************ "<< endl;

  const double Losange9[12]= 
    {
       0.5,0,0,
      0,1,0,
      -1.5,0,0,
      0,-1,0
    };

  deque< double > resultat_test11 = INTERP_UTILS::intersect_convex_polygons(Losange1,Losange9,4,4,1,1e-6);

  cerr<<  " Test11: Résultat théorique" << endl;
  cerr<< 0 << " ,"<< -1 << " ," << 0 << endl;
  cerr<<  -1 << " ,"<< 0 << " ,"<< 0 << endl;
  cerr<< 0 << " ,"<< 1 << " ,"<< 0 << endl;
  cerr<< 0.5 << " ," << 0 << " ," << 0 << endl;

  cerr<< " Test11: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test11.size()/3; i++)
    {
      cerr << resultat_test11[3*i] << "  ";
      cerr << resultat_test11[3*i+1] << "  ";
      cerr << resultat_test11[3*i+2] << "  ";
      cerr << endl ;
    }
};
