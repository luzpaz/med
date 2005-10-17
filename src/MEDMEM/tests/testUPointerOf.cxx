//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : testUPointerOf.cxx
//  Module : MED

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
	ASSERT((int *)P == NULL);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------" << endl;
        cout << " pb avec set(taille = 0) " << endl;
        cout << "-------------------------" << endl;
        MESSAGE( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }

  try
  {
	P.set(-1 * size);
	ASSERT((int *)P == NULL);
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------" << endl;
        cout << " pb avec set(taille) " << endl;
        cout << "---------------------" << endl;
        MESSAGE( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }

  try
  {
	P.set(size);
	ASSERT((int *)P != NULL);
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------" << endl;
        cout << " pb avec set(taille) " << endl;
        cout << "---------------------" << endl;
        MESSAGE( "catched exception : " << e.what() ) ;
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
        SCRUTE(P2[i]);
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
        SCRUTE(P2[i]);
        SCRUTE(P3[i]);
  }

  const PointerOf<int> P4(p);
  const PointerOf<int> P5(P4);
  delete [] p;
  
}
