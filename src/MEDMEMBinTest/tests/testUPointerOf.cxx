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

//  File   : testUPointerOf.cxx
//  Module : MED
//
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_Exception.hxx"

using namespace std;
using namespace MEDMEM;


int main (int argc, char ** argv) 
{
  cout << "PointerOf Test" << endl ;
  cout << "--------------" << endl;

  const int size=10;
  PointerOf<int> P;

  // Creation d'un PointerOf de int
  // et vérification de la methode set en essayant avec 
  // une taille nulle, négative puis positive
  try
  {
        P.set(0);
        ASSERT_MED((int *)P == NULL);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------" << endl;
        cout << " pb avec set(taille = 0) " << endl;
        cout << "-------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }

  try
  {
        P.set(-1 * size);
        ASSERT_MED((int *)P == NULL);
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------" << endl;
        cout << " pb avec set(taille) " << endl;
        cout << "---------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }

  try
  {
        P.set(size);
        ASSERT_MED((int *)P != NULL);
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------" << endl;
        cout << " pb avec set(taille) " << endl;
        cout << "---------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  for (int i=0; i < size; i++)
  {
        P[i]=i;
  }

  PointerOf<int> P2(10);

  P2=P;
  for (int i=0; i < size; i++)
  {
        SCRUTE_MED(P2[i]);
  }

  int * p=new int [size];
  for (int i=0; i < size; i++)
  {
        p[i]=i*10;
  }
  
  P2.set(p);
  PointerOf<int> P3(p);

  for (int i=0; i < size; i++)
  {
        SCRUTE_MED(P2[i]);
        SCRUTE_MED(P3[i]);
  }

  const PointerOf<int> P4(p);
  const PointerOf<int> P5(P4);
  delete [] p;
  
}
