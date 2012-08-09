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

//  File   : testUCoordinate.cxx
//  Module : MED
//
#include <cstdlib>
#include <cmath>
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_define.hxx"
#include "MEDMEM_Array.hxx"
#include "MEDMEM_Coordinate.hxx"
#include <string> 

#define pouieme 0.009

using namespace std;
using namespace MEDMEM;

static void usage(char * name)
{
  cout << "    " << name << endl;
}

int main (int argc, char ** argv)
{
 

  if (argc < 1) usage(argv[0]);

  cout << "COORDINATE Test" << endl;
  cout << "---------------" << endl;

  int SpaceDim = 3;
  int NbOfNodes = 5;

  string * noms = new string[3];
  noms[0]=" X ";
  noms[1]=" Y ";
  noms[2]=" Z ";

  string  units[3];
  units[0]=" m ";
  units[1]=" m ";
  units[2]=" m ";

  int * numbers=new int[5];
  for (int i=0; i< 5; i++) numbers[i] =10+i;

  MED_EN::medModeSwitch ModeFull=MED_EN::MED_FULL_INTERLACE;
  double coor[15] = {0,0,0,0,0,1,0,1,0,1,0,0,0.5,0.5,0.5} ;
  ASSERT_MED(SpaceDim*NbOfNodes == 15);


  cout << "      - mode : MED_FULL_INTERLACE"  << endl;
  cout << "      - dimension de l espace : " << SpaceDim << endl;
  cout << "      - nombre de noeuds : " << NbOfNodes << endl;
  
  double * coor1 = new  double[SpaceDim*NbOfNodes];
  for( int k=0 ; k<SpaceDim*NbOfNodes ; k++ ) 
        coor1[k] = coor[k] ;

  MEDARRAY<double>* CoordinateArray = new MEDARRAY<double>(coor1,SpaceDim,NbOfNodes,ModeFull);
  COORDINATE mycoo;
  try
  {
        mycoo.setCoordinates(CoordinateArray);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinates" << endl;
        cout << "-------------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinates" << endl;
        cout << "-------------------------------" << endl;
        return EXIT_FAILURE ;
  };

  //--------------------------------------------------------------------//
  //                    Tests des methodes                              //
  //                                                                    //
  // - setCoordinatesNames                                              //
  // - setCoordinatesUnits                                              //
  // - setCoordinatesSystem                                             //
  // - setNodesNumbers                                                  //
  //                                                                    //
  //--------------------------------------------------------------------//
  try
  {
        mycoo.setCoordinatesNames(noms);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinatesNames" << endl;
        cout << "-------------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinatesNames" << endl;
        cout << "-------------------------------" << endl;
        return EXIT_FAILURE ;
  };

  try
  {
        mycoo.setCoordinatesUnits(units);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinatesUnits" << endl;
        cout << "-------------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinatesUnits" << endl;
        cout << "-------------------------------" << endl;
        return EXIT_FAILURE ;
  };

  try
  {
        mycoo.setCoordinatesSystem("cartesien");
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinatesSystem" << endl;
        cout << "-------------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setCoordinatesSystem" << endl;
        cout << "-------------------------------" << endl;
        return EXIT_FAILURE ;
  };

  try
  {
        mycoo.setNodesNumbers(numbers);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setNodesNumbers" << endl;
        cout << "-------------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec setNodesNumbers" << endl;
        cout << "-------------------------------" << endl;
        return EXIT_FAILURE ;
  };
  
  //--------------------------------------------------------------------//
  //                    Tests des methodes                              //
  //                                                                    //
  // - getCoordinatesNames                                              //
  // - getCoordinatesUnits                                              //
  // - getCoordinatesUnit                                               //
  // - getCoordinatesSystem                                             //
  // - getNodesNumbers                                                  //
  //                                                                    //
  //--------------------------------------------------------------------//

  cout << endl << " Relecture des Coordonnees" << endl << endl;

  try
  {
        cout << "  Le systeme est : " << mycoo.getCoordinatesSystem() << endl;
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec getCoordinatesSystem" << endl;
        cout << "-------------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------------------" << endl;
        cout << "pb avec getCoordinatesSystem" << endl;
        cout << "-------------------------------" << endl;
        return EXIT_FAILURE ;
  };

  const string * units2;
  try
  {
        units2=mycoo.getCoordinatesUnits();
        for (int axe = 0; axe < SpaceDim; axe++)
        {
                try
                {
                        string  verif = mycoo.getCoordinateUnit(axe+1);
                        if (verif != units2[axe]) 
                                cout << "Pb avec les noms des axes" << endl;
                }
                catch ( const std::exception &e )
                {
                        cout << "-------------------------" << endl;
                        cout << "pb avec getCoordinateUnit" << endl;
                        cout << "-------------------------" << endl;
                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                        return EXIT_FAILURE ;
                }
                catch (...)
                {
                        cout << "-------------------------" << endl;
                        cout << "pb avec getCoordinateUnit" << endl;
                        cout << "-------------------------" << endl;
                        return EXIT_FAILURE ;
                };
        }
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << "pb avec getCoordinatesUnits" << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------------------" << endl;
        cout << "pb avec getCoordinatesUnits" << endl;
        cout << "---------------------------" << endl;
        return EXIT_FAILURE ;
  };

  const string * noms2;
  try
  {
        noms2=mycoo.getCoordinatesNames();
        for (int axe = 0; axe < SpaceDim; axe++)
        {
                try
                {
                        string  verif = mycoo.getCoordinateName(axe+1);
                        if (verif != noms2[axe]) 
                                cout << "Pb avec les noms des axes" << endl;
                }
                catch ( const std::exception &e )
                {
                        cout << "-------------------------" << endl;
                        cout << "pb avec getCoordinateName" << endl;
                        cout << "-------------------------" << endl;
                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                        return EXIT_FAILURE ;
                }
                catch (...)
                {
                        cout << "-------------------------" << endl;
                        cout << "pb avec getCoordinateName" << endl;
                        cout << "-------------------------" << endl;
                        return EXIT_FAILURE ;
                };
        }
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << "pb avec getCoordinatesNames" << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------------------" << endl;
        cout << "pb avec getCoordinatesNames" << endl;
        cout << "---------------------------" << endl;
        return EXIT_FAILURE ;
  };


  try
  {
        for (int axe = 0; axe < SpaceDim; axe++)
        {

                cout << "Coordonnees sur l'Axe : " <<  noms2[axe] ;
                cout << " dont l unite est " << units2[axe] << endl;

                try
                {
                        for (int num=0; num < NbOfNodes; num++)
                        {
                                try
                                {
                                        const double d = mycoo.getCoordinate(num + 1,axe+1);
                                        cout << d <<" , ";
                                        ASSERT_MED(fabs(d - mycoo.getCoordinateAxis(axe+1)[num])  < pouieme);
                                        ASSERT_MED(fabs(d - mycoo.getCoordinates(ModeFull)[(num * SpaceDim)+axe]) < pouieme);
                                        ASSERT_MED(fabs(d - coor [(num * SpaceDim)+axe]) < pouieme);
                                }
                                catch ( const std::exception &e )
                                {
                                        cout << "----------------------" << endl;
                                        cout << "pb avec getCoordinates" << endl;
                                        cout << "----------------------" << endl;
                                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                                        return EXIT_FAILURE ;
                                }
                                catch (...)
                                {
                                        cout << "----------------------" << endl;
                                        cout << "pb avec getCoordinates" << endl;
                                        cout << "----------------------" << endl;
                                        return EXIT_FAILURE ;
                                };
                        }
                        cout << endl;
                }
                catch ( const std::exception &e )
                {
                        cout << "-------------------------" << endl;
                        cout << "pb avec getCoordinateAxis" << endl;
                        cout << "-------------------------" << endl;
                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                        return EXIT_FAILURE ;
                }
                catch (...)
                {
                        cout << "-------------------------" << endl;
                        cout << "pb avec getCoordinateAxis" << endl;
                        cout << "-------------------------" << endl;
                        return EXIT_FAILURE ;
                }
        }
  }
  catch ( const std::exception &e )
  {
        cout << "----------------------" << endl;
        cout << "pb avec getCoordinates" << endl;
        cout << "----------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "----------------------" << endl;
        cout << "pb avec getCoordinates" << endl;
        cout << "----------------------" << endl;
        return EXIT_FAILURE ;
  };


  delete [] noms ;
  delete [] numbers;
  delete [] coor1;

  delete CoordinateArray ;

  MESSAGE_MED("FIN normale du traitement");
  return EXIT_SUCCESS ;
}

