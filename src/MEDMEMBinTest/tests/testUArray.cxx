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

//  File   : testUArray.cxx
//  Module : MED
//
#include <cstdlib>      // pour l'acces à EXIT_SUCCESS et EXIT_FAILURE
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Array.hxx"

using namespace std;
using namespace MEDMEM;

static void imprime(string titre,const int * myValues,const int * myOthers, int lignes, int colonnes)
{
  cout << "             " <<titre << endl<< endl;
  cout << "Full Interlace : " << endl;
  if (myValues != NULL)
  {
        for (int i=0; i<lignes; i++)
        {
                cout << " - ";
                for (int k=0; k< colonnes; k++)
                {
                        cout << myValues[k + i * colonnes]<< " " ; 
                }
                cout << endl;
        }
  }
  else
  {
                cout << "Pointeur Null : pas de valeur" << endl << endl;
  }
  cout << endl;
  cout << "No Interlace : " << endl;

  if (myOthers != NULL)
  {
        for (int k=0; k< colonnes; k++)
        {
                cout << " - ";
                for (int i=0; i<lignes; i++)
                {
                        cout << myOthers[i + k*lignes]<< " " ; 
                }
                cout << endl;
        }
  }
  else
  {
                cout << "Pointeur Null : pas de valeur" << endl << endl;
  }
  cout << endl;
}

int main (int argc, char ** argv) 
{

  /* ---------------------------------------------------------- */
  /*                                                            */
  /*                    INITIALISATION                          */
  /*                                                            */
  /* - constructeur new MEDARRAY<int>(int,int,mode)             */
  /* - get sur le pointeur MED_FULL_INTERLACE                   */
  /* - initialisation des valeurs (coordonnées ieme elt = I+1)  */
  /* - get sur le pointeur MED_NO_INTERLACE                     */
  /* ---------------------------------------------------------- */

  int SpaceDimension = 3;
  int NumberOfNodes  = 4; 


  
  MEDARRAY<int> * myArrayfull= new MEDARRAY<int>(SpaceDimension,NumberOfNodes,MED_EN::MED_FULL_INTERLACE);
  ASSERT_MED(myArrayfull != NULL);

  const int * myValues = myArrayfull->get(MED_EN::MED_FULL_INTERLACE);
  ASSERT_MED(myValues!= NULL);

  for (int i=0; i<NumberOfNodes; i++)
  {
        for (int k=0; k<SpaceDimension; k++)
        {
          //myValues[i* SpaceDimension +k]=i + 1; 
          myArrayfull->setIJ(i+1,k+1,i+1) ;
        }
  }
  
  const int * myOthers = myArrayfull->get(MED_EN::MED_NO_INTERLACE) ;
  ASSERT_MED(myOthers != NULL);
  
  imprime("Initialisation full interlace (xi=yi=zi=i+1)",myValues,myOthers,NumberOfNodes,SpaceDimension);

  MEDARRAY<int> * myArrayno= new MEDARRAY<int>(SpaceDimension,NumberOfNodes,MED_EN::MED_NO_INTERLACE);
  ASSERT_MED(myArrayno != NULL);
  const int * myValuesno = myArrayno->get(MED_EN::MED_NO_INTERLACE);
  ASSERT_MED(myValuesno!= NULL);

  for (int k=0; k<SpaceDimension; k++)
  {
        for (int i=0; i<NumberOfNodes; i++)
        {
          //myValuesno[i +k*NumberOfNodes]=k + 1; 
          myArrayno->setIJ(i+1,k+1,k+1) ;
        }
  }
  
  const int * myOthersno = myArrayno->get(MED_EN::MED_FULL_INTERLACE) ;
  ASSERT_MED(myOthersno != NULL);

  imprime("Initialisation no interlace (xi=yi=zi=i+1)",myOthersno,myValuesno,NumberOfNodes,SpaceDimension);


  /* ---------------------------------------------------------- */
  /*                                                            */
  /*                    Tests des Fonctions Set                 */
  /*                                                            */
  /* - setI sur l'element 1 avec des coordonnees a 100          */
  /* - setJ sur l'element 1 avec des coordonnees a 100          */
  /* - setIJ sur (1,2) avec une coordonnee = 1992               */
  /* - set   avec l ensemble des coordonnes remises à i              */
  /* ---------------------------------------------------------- */


  int * myNewLine = new int[SpaceDimension];
  for (int i = 0; i < SpaceDimension; i++) 
        myNewLine[i] = myValues[i] * 100;
  try
  {
        myArrayfull->setI(1, myNewLine);
  }
  catch ( const std::exception &e )
  {
        cout << "--------------" << endl;
        cout << "   Pb au setI " << endl;
        cout << "--------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------" << endl;
        cout << "   Pb au setI   " << endl;
        cout << "---------------" << endl;
  }

  delete [] myNewLine;
  imprime("1er element : coordonnees à 100",myValues,myOthers,NumberOfNodes,SpaceDimension);

  int * myNewCol = new int[NumberOfNodes];
  for (int i = 0; i < NumberOfNodes; i++) 
        myNewCol[i] = 100;
  try
  {
        myArrayno->setJ(1, myNewCol);
  }
  catch ( const std::exception &e )
  {
        cout << "--------------" << endl;
        cout << "   Pb au setJ " << endl;
        cout << "--------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------" << endl;
        cout << "   Pb au setJ   " << endl;
        cout << "---------------" << endl;
  }

  delete [] myNewCol;
  imprime("1eres  coordonnees à 100",myOthersno,myValuesno,NumberOfNodes,SpaceDimension);

  try
  {
        myArrayfull->setIJ(1,2,1992);
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << "   Pb au setIJ()  de 1 , 2 "  << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------------------" << endl;
        cout << "   Pb au setIJ()  de 1 , 2 "  << endl;
        cout << "-----------------------------" << endl;
  }

  imprime("1er element : 2ieme coordonnee = 1992",myValues,myOthers,NumberOfNodes,SpaceDimension);


  try
  {
        myArrayno->setIJ(1,2,1992);
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << "   Pb au setIJ()  de 1 , 2 "  << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------------------" << endl;
        cout << "   Pb au setIJ()  de 1 , 2 "  << endl;
        cout << "-----------------------------" << endl;
  }

  imprime("1er element : 2ieme coordonnee = 1992",myValues,myOthers,NumberOfNodes,SpaceDimension);

  int * mynewvalues= new int [ NumberOfNodes*SpaceDimension ];
  for (int i=0; i<NumberOfNodes*SpaceDimension; i++)
  {
        mynewvalues[i]=i;
  }
  try
  {
        myArrayfull->set(MED_EN::MED_FULL_INTERLACE,mynewvalues);
        myValues = myArrayfull->get(MED_EN::MED_FULL_INTERLACE);
        myOthers = myArrayfull->get(MED_EN::MED_NO_INTERLACE);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------" << endl;
        cout << "   Pb au set "  << endl;
        cout << "-------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "--------------" << endl;
        cout << "   Pb au set "  << endl;
        cout << "--------------" << endl;
  }
  imprime("remise valeur a i sur myArrayfull med full interlace",myValues,myOthers,NumberOfNodes,SpaceDimension);

  try
  {
        myArrayno->set(MED_EN::MED_FULL_INTERLACE,mynewvalues);
        myValuesno = myArrayfull->get(MED_EN::MED_FULL_INTERLACE);
        myOthersno = NULL;
  }
  catch ( const std::exception &e )
  {
        cout << "-------------" << endl;
        cout << "   Pb au set "  << endl;
        cout << "-------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "--------------" << endl;
        cout << "   Pb au setI "  << endl;
        cout << "--------------" << endl;
  }
  imprime("set full interlace de myArrayno",myValuesno,myOthersno,NumberOfNodes,SpaceDimension);

  /* ---------------------------------------------------------- */
  /*                                                            */
  /*                Tests des constructeurs                     */
  /*                Tests des Fonctions Get                     */
  /*                                                            */
  /*                                                            */
  /* ---------------------------------------------------------- */

  MEDARRAY<int> * myArrayShare = new MEDARRAY<int>( *myArrayfull);
  const int * sharevalues = myArrayShare->get(MED_EN::MED_FULL_INTERLACE );
  const int * shareno = myArrayShare->get(MED_EN::MED_NO_INTERLACE);
  imprime("test contructeur par recopie non profonde",sharevalues,shareno,NumberOfNodes,SpaceDimension);

  myArrayfull->setIJ(1,2,1992);
  ASSERT_MED(myArrayShare->getIJ(1,2) == 1992);
  imprime("change valeur tableau source, impression tableau cible",sharevalues,shareno,NumberOfNodes,SpaceDimension);

  myArrayShare->setIJ(1,2,1995);
  ASSERT_MED(myArrayfull->getIJ(1,2) == 1995);
  imprime("change valeur tableau cible, impression tableau source",myValues,myOthers,NumberOfNodes,SpaceDimension);

  delete myArrayShare;
  imprime("tableau cible apres destruction tableau source",myValues,myOthers,NumberOfNodes,SpaceDimension);

  MEDARRAY<int> * myArrayShare2 = new MEDARRAY<int>( *myArrayfull,true);
  sharevalues = myArrayShare2->get(MED_EN::MED_FULL_INTERLACE );
  shareno = myArrayShare2->get(MED_EN::MED_NO_INTERLACE );
  imprime("test contructeur par recopie profonde",sharevalues,shareno,NumberOfNodes,SpaceDimension);

  myArrayfull->setIJ(1,2,18);
  imprime("change valeur tableau source, impression tableau cible",sharevalues,shareno,NumberOfNodes,SpaceDimension);

  myArrayShare2->setIJ(1,2,19);
  imprime("change valeur tableau cible, impression tableau source",myValues,myOthers,NumberOfNodes,SpaceDimension);

  myArrayno->set(MED_EN::MED_NO_INTERLACE,mynewvalues);
  myArrayno->setIJ(2,1,1);
  myValuesno = myArrayno->get(MED_EN::MED_NO_INTERLACE);
  myOthersno = myArrayno->get(MED_EN::MED_FULL_INTERLACE);
  imprime("Initialisation no interlace (0...11)",myOthersno,myValuesno,NumberOfNodes,SpaceDimension);

  MEDARRAY<int> * myArrayShare3 = new MEDARRAY<int>( *myArrayno);
  sharevalues = myArrayShare3->get(MED_EN::MED_FULL_INTERLACE);
  shareno = myArrayShare3->get(MED_EN::MED_NO_INTERLACE);
  imprime("test contructeur par recopie non profonde",sharevalues,shareno,NumberOfNodes,SpaceDimension);

  myArrayno->setIJ(1,2,1992);
  ASSERT_MED(myArrayShare3->getIJ(1,2) == 1992);
  imprime("change valeur tableau source, impression tableau cible",sharevalues,shareno,NumberOfNodes,SpaceDimension);

  myArrayShare3->setIJ(1,2,1995);
  ASSERT_MED(myArrayno->getIJ(1,2) == 1995);
  imprime("change valeur tableau cible, impression tableau source",myValuesno,myOthersno,NumberOfNodes,SpaceDimension);

  delete myArrayno;
  delete [] mynewvalues;
  delete myArrayfull;
  delete myArrayShare2;
  delete myArrayShare3;
  MESSAGE_MED("FIN NORMALE DU TRAITEMENT");
  return EXIT_SUCCESS ;
}
/*
  inline medModeSwitch getMode() const ;
*/
