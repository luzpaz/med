// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : testUModulusArray.cxx
//  Module : MED
//
#include <cstdlib>
#include <exception>
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_ModulusArray.hxx"
using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv) 
{

  using namespace std ;

  int array[5]={0,1,2,1,4} ;
  MEDMODULUSARRAY modulusArray(5,array);

  cout << " ModulusArray : " << endl;
  cout << " _______________" << endl;
  for(int i=-10;i<15;i++)
  {
        cout <<"  - A[" << i <<"]="<<modulusArray[i]<<endl;
  }
  cout << endl;

  // ------------
  // test compare
  // ------------

  int ret ;
  int array2[5]={1,4,0,1,2} ;
  MEDMODULUSARRAY modulusArray2(5,array2) ;

  cout << " Test compare : mêmes tableaux mêmes ordres ..." << endl; 

  try
  {
        ret=modulusArray2.compare(modulusArray);
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=1)
  {
        cout << "-----------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray et modulusArray2 " << endl;
        cout << " Les deux tableaux sont identiques " << endl;
        cout << "-----------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray et modulusArray2 :  OK" << endl;
    cout << endl;
  }

  int array3[5]={1,2,1,0,4} ;
  MEDMODULUSARRAY modulusArray3(5,array3) ;

  cout << " Test compare : mêmes tableaux ordres inverses ..." << endl; 

  try
  {
        ret=modulusArray3.compare(modulusArray);
  }
  catch (const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=-1)
  {
        cout << "-----------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray et modulusArray3 " << endl;
        cout << " Les deux tableaux sont inverses " << endl;
        cout << "-----------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray et modulusArray3 :  OK" << endl;
    cout << endl;
  }

  int array4[6]={1,2,1,0} ;
  MEDMODULUSARRAY modulusArray4(4,array4) ;

  cout << " Test compare : Tableaux de differentes tailles ... " << endl;
  try
  {
        ret=modulusArray4.compare(modulusArray);
  }
  catch (const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=0)
  {
        cout << "-----------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray et modulusArray4 " << endl;
        cout << " Les deux n ont pas la meme taille "  << endl;
        cout << "-----------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray et modulusArray4 :  OK" << endl;
    cout << endl;
  }
  
  int array5[5]={1,2,1,0,1} ;
  MEDMODULUSARRAY modulusArray5(5,array5) ;
  cout << " Test compare : tableaux differents de même taille ..." << endl;
  try
  {
        ret=modulusArray5.compare(modulusArray);
  }
  catch (const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=0)
  {
        cout << "-----------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray et modulusArray5 " << endl;
        cout << " Les deux  tableaux sont differents  "  << endl;
        cout << "-----------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray et modulusArray5 :  OK" << endl;
    cout << endl;
  }

  //  test small array :
  //  taille 1
  int array6[1]={1} ;
  MEDMODULUSARRAY modulusArray6(1,array6);

  cout << " ModulusArray  1 : " << endl;
  cout << " __________________" << endl;

  for(int i=-10;i<15;i++)
  {
        cout <<"  - A[" << i <<"]="<<modulusArray6[i]<<endl;
  }
  cout << endl;

  int array7[1]={1} ;
  MEDMODULUSARRAY modulusArray7(1,array7);

  cout << " Test compare : mêmes tableaux mêmes ordres ..." << endl;
  try
  {
        ret=modulusArray6.compare(modulusArray7);
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=1)
  {
        cout << "-----------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray6 et modulusArray7" << endl;
        cout << " Les deux tableaux sont identiques " << endl;
        cout << "-----------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray6 et modulusArray7 :  OK" << endl ;
    cout << endl;
  }

  int array8[1]={2} ;
  MEDMODULUSARRAY modulusArray8(1,array8);

   cout << " Test compare : tableaux differents "<< endl;
  try
  {
        ret=modulusArray6.compare(modulusArray8);
 }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }
  
  if (ret!=0)
  {
        cout << "-----------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray6 et modulusArray8" << endl;
        cout << " Les deux tableaux sont  differents" << endl;
        cout << "-----------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray6 et modulusArray8 :  OK" << endl;
    cout << endl;
  }

  
  // 2
  int array60[2]={1,2} ;
  MEDMODULUSARRAY modulusArray60(2,array60);


  cout << " ModulusArray  2 : " << endl;
  cout << " __________________" << endl;

  for(int i=-10;i<15;i++)
  {
        cout <<"  - A[" << i <<"]="<<modulusArray60[i]<<endl;
  }
  cout << endl;

  int array70[2]={1,2} ;
  MEDMODULUSARRAY modulusArray70(2,array70);
  cout << " Test compare : mêmes tableaux mêmes ordres ..." << endl;

  try
  {
        ret=modulusArray60.compare(modulusArray70);
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=1)
  {
        cout << "--------------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray60 et modulusArray70 " << endl;
        cout << " Les deux tableaux sont identiques " << endl;
        cout << "--------------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray60 et modulusArray70 :  OK" << endl;
    cout << endl;
  }

  int array80[2]={2,2} ;
  MEDMODULUSARRAY modulusArray80(2,array80);

  cout << " Test compare : tableaux differents  ..." << endl;

  try
  {
        ret=modulusArray60.compare(modulusArray80);
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=0)
  {
        cout << "--------------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray60 et modulusArray80 " << endl;
        cout << " Les deux tableaux sont differents " << endl;
        cout << "--------------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray60 et modulusArray80 :  OK" << endl;
    cout << endl;
  }

  int array90[2]={2,1} ;
  MEDMODULUSARRAY modulusArray90(2,array90);
  cout << " Test compare : tableaux differents ordres inverses ..." << endl;
  try
  {
        ret=modulusArray60.compare(modulusArray90);
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec compare " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  if (ret!=-1)
  {
        cout << "--------------------------------------------------------" << endl;
        cout << " pb avec compare entre modulusArray60 et modulusArray90 " << endl;
        cout << " Les deux tableaux sont differents " << endl;
        cout << "--------------------------------------------------------" << endl;
        return EXIT_FAILURE ;
  }
  else
  {
    cout << " compare entre modulusArray60 et modulusArray90 :  OK" << endl;
    cout << endl;
  }

  return EXIT_SUCCESS ;
}
