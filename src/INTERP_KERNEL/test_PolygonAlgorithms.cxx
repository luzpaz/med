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
#include "PolygonAlgorithms.hxx" 
#include "PolygonAlgorithms.cxx"
 
using namespace std;

int main ()
{
    cerr<< endl;
    cerr<< " ************ Test 1  ************ "<< endl;
    INTERP_UTILS::PolygonAlgorithms<3> P_1(1e-6,1e-6); 
  
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
    
    deque< double > resultat_test1 = P_1.intersect_convex_polygons(Losange1,Losange2,4,4);
    
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
    INTERP_UTILS::PolygonAlgorithms<3> P_2(1e-6,1e-6); 

    const double Losange3[12]=
      {
	2.5,0.5,0,
	1.5,1.5,0,
	0.5,0.5,0,
	1.5,-0.5,0
      };
    deque< double > resultat_test2 = P_2.intersect_convex_polygons(Losange1,Losange3,4,4);
    
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
  INTERP_UTILS::PolygonAlgorithms<3> P_3(1e-6,1e-6); 

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
 deque< double > resultat_test3 = P_3.intersect_convex_polygons( Carre1, Carre2,4,4);

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

    INTERP_UTILS::PolygonAlgorithms<3> P_4(1e-6,1e-6); 
  const double Losange4[12]=
    {
      3,0,0,
      2,1,0,
      1,0,0,
      2,-1,0
    };
 deque< double > resultat_test4 = P_4.intersect_convex_polygons( Losange1, Losange4,4,4);

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
  INTERP_UTILS::PolygonAlgorithms<3> P_5(1e-6,1e-6); 
  deque< double > resultat_test5 = P_5.intersect_convex_polygons( Carre1, Carre1,4,4);

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
  INTERP_UTILS::PolygonAlgorithms<3> P_6(1e-6,1e-6); 
 
  const double Losange5[12]= 
    {
      1.5,0,0,
      0,1.5,0,
      -1.5,0,0,
      0,-1.5,0
    };
 deque< double > resultat_test6 = P_6.intersect_convex_polygons( Carre1, Losange5,4,4);

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
  INTERP_UTILS::PolygonAlgorithms<3> P_7(1e-6,1e-6); 

  deque< double > resultat_test7 = P_7.intersect_convex_polygons( Losange1, Carre1,4,4);

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
  INTERP_UTILS::PolygonAlgorithms<3> P_8(1e-6,1e-6); 

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
  
  deque< double > resultat_test8 = P_8.intersect_convex_polygons(Losange6,Losange7,6,5);

  cerr<<  " Test8: Résultat théorique" << endl;
  cerr<< 0.5 << " ,"<< -0.5 << " ," << 0 << endl;
  cerr<< 0.5 << " ,"<< -0.5 << " ," << 0 << endl;
  cerr<<  0 << " ,"<< 0 << " ,"<< 0 << endl;
  cerr<< 0.5 << " ,"<< 0.5 << " ,"<< 0 << endl;
  cerr<< 0.5 << " ,"<< 0.5 << " ,"<< 0 << endl;
  cerr<< 1 << " ," << 0 << " ," << 0 << endl;

  cerr<< " Test8: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test8.size()/3; i++)
    {
      cerr << resultat_test8[3*i] << "  ";
      cerr << resultat_test8[3*i+1] << "  ";
      cerr << resultat_test8[3*i+2] << "  ";
      cerr << endl ;
    }
 
  cerr<< " ************ Test 9  ************ "<< endl;
  INTERP_UTILS::PolygonAlgorithms<3> P_9(1e-6,1e-6); 

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

  deque< double > resultat_test9 = P_9.intersect_convex_polygons(Carre4,Carre3,4,5);

  cerr<<  " Test9: Résultat théorique" << endl;
  cerr<< -0.5 << " ,"<<  1 << " ," << 0 << endl;
  cerr<< -0.5 << " ,"<< -1 << " ," << 0 << endl;
  cerr<<  1   << " ,"<< -1 << " ," << 0 << endl;
  cerr<<  1   << " ,"<< 1 << " ," << 0 << endl;

  cerr<< " Test9: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test9.size()/3; i++)
    {
      cerr << resultat_test9[3*i] << "  ";
      cerr << resultat_test9[3*i+1] << "  ";
      cerr << resultat_test9[3*i+2] << "  ";
      cerr << endl ;   
    } 
    
  cerr<< " ************ Test 10  ************ "<< endl;
  INTERP_UTILS::PolygonAlgorithms<3> P_10(1e-6,1e-6); 

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
  deque< double > resultat_test10 = P_10.intersect_convex_polygons(Losange8,Carre5,4,5);
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
  INTERP_UTILS::PolygonAlgorithms<3> P_11(1e-6,1e-6); 

  const double Losange9[12]= 
    {
       0.5,0,0,
      0,1,0,
      -1.5,0,0,
      0,-1,0
    };

  deque< double > resultat_test11 = P_11.intersect_convex_polygons(Losange1,Losange9,4,4);

  cerr<<  " Test11: Résultat théorique" << endl;
  cerr<< 0 << " ,"<< -1 << " ," << 0 << endl;
  cerr<< 0 << " ,"<< -1 << " ," << 0 << endl;
  cerr<<  -1 << " ,"<< 0 << " ,"<< 0 << endl;
  cerr<< 0 << " ,"<< 1 << " ,"<< 0 << endl;
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
    
  cerr<< " ************ Test 12  ************ "<< endl;
  INTERP_UTILS::PolygonAlgorithms<3> P_12(1e-6,1e-6); 

  const double hexagone1[18]= 
    {
       -2,0,0,
      -1,-1,0,
      1,-1,0,
       2,0,0,
       1,1,0,
       -1,1,0
    };
  const double hexagone2[18]= 
    {
       -1.5,0.5,0,
      -1,-1,0,
      1,-1,0,
       2,1,0,
       1,1,0,
       -1,1,0
    };
  deque< double > resultat_test12 = P_12.intersect_convex_polygons(hexagone1,hexagone2,6,6);

  cerr<<  " Test12: Résultat théorique" << endl;
  cerr<< 1 << " ,"<< -1 << " ," << 0 << endl;
  cerr<<  -1 << " ,"<< -1 << " ,"<< 0 << endl;
  cerr<< -1.5 << " ,"<< 0.5 << " ,"<< 0 << endl;
  cerr<< -1 << " ," << 1 << " ," << 0 << endl;
  cerr<< 1 << " ,"<< 1 << " ," << 0 << endl;
  cerr<< 5./3 << " ,"<< 1/3. << " ," << 0 << endl;

  cerr<< " Test12: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test12.size()/3; i++)
    {
      cerr << resultat_test12[3*i] << "  ";
      cerr << resultat_test12[3*i+1] << "  ";
      cerr << resultat_test12[3*i+2] << "  ";
      cerr << endl ;
    }
    
  cerr<< " ************ Test 13  ************ "<< endl;
  INTERP_UTILS::PolygonAlgorithms<3> P_13(1e-6,1e-6); 

  const double hexagone3[18]= 
    {
       -2,2,0,
      -1,1,0,
      1,1,0,
       2,2,0,
       1,3,0,
       -1,3,0
    };
  deque< double > resultat_test13 = P_13.intersect_convex_polygons(hexagone1,hexagone3,6,6);

  cerr<<  " Test13: Résultat théorique" << endl;

  cerr<< " Test13: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test13.size()/3; i++)
    {
      cerr << resultat_test13[3*i] << "  ";
      cerr << resultat_test13[3*i+1] << "  ";
      cerr << resultat_test13[3*i+2] << "  ";
      cerr << endl ;
    }
   
  cerr<< " ************ Test 14  ************ "<< endl;
  INTERP_UTILS::PolygonAlgorithms<3> P_14(1e-6,1e-6); 

  const double Carre6[12]=
    {
      -1,1,0,
      -1,3,0,
      0.5,3,0,
      0.5,1,0
    };
 deque< double > resultat_test14 = P_14.intersect_convex_polygons( Carre1, Carre6,4,4);

  cerr<<  " Test14: Résultat théorique" << endl;

  cerr<< " Test14: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test14.size()/3; i++)
    {
      cerr << resultat_test14[3*i] << "  ";
      cerr << resultat_test14[3*i+1] << "  ";
      cerr << resultat_test14[3*i+2] << "  ";
      cerr << endl ;
    }

  cerr<< " ***************** Test 15 ***************** " << endl;
  INTERP_UTILS::PolygonAlgorithms<3> P_15(1e-6,1e-6); 
  
  const double Losange10[12]=
    {
      0,-1,0,
      1,-2,0,
      0,-3,0,
      -1,-2,0
    };
 deque< double > resultat_test15 = P_15.intersect_convex_polygons( Losange1, Losange10,4,4);

  cerr<<  " Test15: Résultat théorique" << endl;

  cerr<< " Test15: Résultat obtenu" << endl;
  for (int i=0; i<resultat_test15.size()/3; i++)
    {
      cerr << resultat_test15[3*i] << "  ";
      cerr << resultat_test15[3*i+1] << "  ";
      cerr << resultat_test15[3*i+2] << "  ";
      cerr << endl ;
    }

    cerr<< " ************ Test 16  ************ "<< endl;
    INTERP_UTILS::PolygonAlgorithms<3> P_16(1e-6,1e-6); 
  

    const double triangle1[9]=
      {
	0.5,0,0,
	1,1,0,
	0,1,0
      };
    
    deque< double > resultat_test16 = P_16.intersect_convex_polygons(Losange1,triangle1,4,3);
    
    cerr<<  " Test16: Résultat théorique" << endl;
    cerr<< 0.5 << " ,"<< 0 << " ,"<< 0 << endl; 
    cerr<< 0 << " ,"<< 1 << " ," << 0 << endl;
    cerr<< 2/3 << " ," << 1/3 << " ," << 0 << endl; 
    
    cerr<< " Test16: Résultat obtenu" << endl;
    for (int i=0; i<resultat_test16.size()/3; i++)
      {
	cerr << resultat_test16[3*i] << "  ";
	cerr << resultat_test16[3*i+1] << "  ";
	cerr << resultat_test16[3*i+2] << "  ";
	cerr << endl ;
      }

    cerr<< " ************ Test 17  ************ "<< endl;
    INTERP_UTILS::PolygonAlgorithms<3> P_17(1e-6,1e-6); 
  

    const double triangle2[9]=
      {
	0,0.5,0,
	0,-0.5,0,
	1.5,0,0
      };
    
    deque< double > resultat_test17 = P_17.intersect_convex_polygons(Losange1,triangle1,4,3);
    
    cerr<<  " Test17: Résultat théorique" << endl;
    cerr<< 0 << " ,"<< 0.5 << " ,"<< 0 << endl; 
    cerr<< 0 << " ,"<< -0.5 << " ," << 0 << endl;
    cerr<< 2./3 << " ," << 1/3 << " ," << 0 << endl; 
    
    cerr<< " Test17: Résultat obtenu" << endl;
    for (int i=0; i<resultat_test17.size()/3; i++)
      {
	cerr << resultat_test17[3*i] << "  ";
	cerr << resultat_test17[3*i+1] << "  ";
	cerr << resultat_test17[3*i+2] << "  ";
	cerr << endl ;
      }
    cerr<< " ************ Test 18  ************ "<< endl;
    INTERP_UTILS::PolygonAlgorithms<3> P_18(1e-6,1e-6); 
  

    const double triangle3[9]=
      {
	3.928371006592e-03, 0.000000000000e+00, -1.276720577142e-02 ,
	0.000000000000e+00, 0.000000000000e+00, -1.669557677802e-02 ,
	3.928371006592e-03, 0.000000000000e+00, -1.669557677802e-02 ,
      };
    const double triangle4[9]=
      {  
	0.000000000000e+00, 0.000000000000e+00, -1.767766952966e-02, 
	4.419417382416e-03, 0.000000000000e+00, -1.325825214725e-02, 
	0.000000000000e+00, 0.000000000000e+00, -1.325825214725e-02, 
      };
    
    deque< double > resultat_test18 = P_18.intersect_convex_polygons(triangle3,triangle4,3,3);
    
    cerr<<  " Test18: Résultat théorique" << endl;
    cerr<< 0.001 << " ,"<< 0. << " ,"<< -0.02 << endl; 
    cerr<< 0 << " ,"<< 0. << " ," << -1.669557677802e-02 << endl;
    cerr<< 0.0034 << " ," << 0. << " ," << -0.0132 << endl; 
    cerr<< 0.0039 << " ," << 0. << " ," << -0.0132 << endl;
    cerr<< 0.0039 << " ," << 0. << " ," << -0.014 << endl;

    cerr<< " Test18: Résultat obtenu" << endl;
    for (int i=0; i<resultat_test18.size()/3; i++)
      {
	cerr << resultat_test18[3*i] << "  ";
	cerr << resultat_test18[3*i+1] << "  ";
	cerr << resultat_test18[3*i+2] << "  ";
	cerr << endl ;
      }

};
