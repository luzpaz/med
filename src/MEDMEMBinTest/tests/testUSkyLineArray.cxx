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

//  File   : testUSkyLineArray.cxx
//  Module : MED
//
#include <cstdlib>
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_SkyLineArray.hxx"

using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv)
{

  int NumberOfCell = 3 ; // 1 triangle,1 quadrangle,1 triangle
  int Size = 10 ;        // 10 nodes

  int * index = new int[NumberOfCell+1];
  index[0]=1;
  index[1]=4;
  index[2]=8;
  index[3]=11;

  int * value = new int[Size];
  value[0]=1; // first
  value[1]=2;
  value[2]=5;
  value[3]=2; // second
  value[4]=3;
  value[5]=5;
  value[6]=6;
  value[7]=3; // thirst
  value[8]=4;
  value[9]=6;

  MEDSKYLINEARRAY * myArray = new MEDSKYLINEARRAY(NumberOfCell,Size,index,value) ;

  const int * ArrayIndex ;
  try
  {
        ArrayIndex = myArray->getIndex() ;
  }
  catch ( const std::exception &e )
  {
        cout << "----------------" << endl;
        cout << "pb avec getIndex" << endl;
        cout << "----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "----------------" << endl;
        cout << "pb avec getIndex" << endl;
        cout << "----------------" << endl;
        return EXIT_FAILURE ;
  }

  const int * ArrayValue ;
  try
  {
        ArrayValue  = myArray->getValue() ;
  }
  catch ( const std::exception &e )
  {
        cout << "----------------" << endl;
        cout << "pb avec getValue" << endl;
        cout << "----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "----------------" << endl;
        cout << "pb avec getValue" << endl;
        cout << "----------------" << endl;
        return EXIT_FAILURE ;
  }


  try
  {
        ASSERT_MED(myArray->getNumberOf()==NumberOfCell);
        cout << myArray->getNumberOf() << endl;
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------" << endl;
        cout << "pb avec getNumberOf" << endl;
        cout << "-------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------" << endl;
        cout << "pb avec getNumberOf" << endl;
        cout << "-------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        ASSERT_MED(myArray->getLength()==Size);
        cout << myArray->getLength() << endl; 
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------" << endl;
        cout << "pb avec getLength" << endl;
        cout << "-------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------" << endl;
        cout << "pb avec getLength" << endl;
        cout << "-------------------" << endl;
        return EXIT_FAILURE ;
  }

  cout << "Show all :" << endl ;
  for (int i=1; i< NumberOfCell+1 ; i++) 
  {
    const int * cell;
    try
    {
        cell = myArray->getI(i) ;
    }
    catch ( const std::exception &e )
    {
        cout << "-----------------------------" << endl;
        cout << "pb avec getI, valeur de i : " << i << endl;
        cout << "-----------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
    }
    catch (...)
    {
        cout << "-------------------" << endl;
        cout << "pb avec getI, valeur de i : " << i << endl;
        cout << "-------------------" << endl;
        return EXIT_FAILURE ;
    }

    int numberof;
    try
    {
        numberof= myArray->getNumberOfI(i) ;
        ASSERT_MED(numberof==index[i]-index[i-1]);
    }
    catch ( const std::exception &e )
    {
        cout << "----------------------------------" << endl;
        cout << "pb avec getNumberOfI, valeur de i : " << i << endl;
        cout << "----------------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
    }
    catch (...)
    {
        cout << "----------------------------------" << endl;
        cout << "pb avec getNumberOfI, valeur de i : " << i << endl;
        cout << "----------------------------------" << endl;
        return EXIT_FAILURE ;
    }

    cout << " - " ;
    for (int j=0;j<numberof;j++)
    {
        cout << cell[j] << " " ;
        try
        {
                int verif=myArray->getIJ(i,j+1);
                if (verif != cell[j])
                {
                        cout << "----------------------------------" << endl;
                        cout << " incoherence dans les valeurs :   " << endl;
                        cout << " cell[" << j << "] : " << cell[j] << endl;
                        cout << " getIJ(" << i <<"," << j << ") : " << verif << endl;
                        cout << "----------------------------------" << endl;
                        return EXIT_FAILURE ;
                }
        }
        catch ( const std::exception &e )
        {
                cout << "----------------------------------" << endl;
                cout << "pb avec getIJ, valeurs de i / j : " << i << " " << j<< endl;
                cout << "----------------------------------" << endl;
                MESSAGE_MED( "catched exception : " << e.what() ) ;
                return EXIT_FAILURE ;
        }
        catch (...)
        {
                cout << "----------------------------------" << endl;
                cout << "pb avec getIJ, valeurs de i / j : " << i << " " << j<< endl;
                cout << "----------------------------------" << endl;
                return EXIT_FAILURE ;
        }
    }
    cout << endl ;
  }

  delete[] index ;
  delete[] value ;

  delete myArray;
  MESSAGE_MED("FIN normale du traitement");
  return EXIT_SUCCESS ;


/*
  MEDSKYLINEARRAY( const MEDSKYLINEARRAY &myArray );
  void setMEDSKYLINEARRAY( const med_int count, const med_int length, med_int* index , med_int* value ) ;

  
*/
}
