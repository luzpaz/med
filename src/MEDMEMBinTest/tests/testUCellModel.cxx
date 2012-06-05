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

//  File   : testUCellModel.cxx
//  Module : MED
//
#include <set>
#include <cstdlib> 
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_DriversDef.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define MED_NBR_GEOMETRIE_MAILLE 15

static void usage(char * name)
{
  cout << "    " << name << endl;
}

int main (int argc, char ** argv)
{

  if (argc < 1) usage(argv[0]);


  cout << "CELLMODEL Test" << endl ;
  cout << "--------------" << endl;

  /*    -----------------------------------------------------
        Construction de tous les types de cellmodel possibles
        -----------------------------------------------------
  */

  CELLMODEL * tous  =new CELLMODEL[MED_NBR_GEOMETRIE_MAILLE ];
  const medGeometryElement  allCellType[MED_NBR_GEOMETRIE_MAILLE ]=
    {MED_POINT1, MED_SEG2,    MED_SEG3,   MED_TRIA3,
     MED_QUAD4,  MED_TRIA6,   MED_QUAD8,  MED_TETRA4,
     MED_PYRA5,  MED_PENTA6,  MED_HEXA8,  MED_TETRA10,
     MED_PYRA13, MED_PENTA15, MED_HEXA20 };

  for (int i=0; i<MED_NBR_GEOMETRIE_MAILLE ; i++)
    {
      tous[i]=CELLMODEL(allCellType[i]);
    }

  /*    -----------------------------------------------------
        Boucle sur  tous les types de cellmodel possibles
        - Chaque Méthode de cellmodel est appelee
        -----------------------------------------------------
  */
  for (int i=0; i<MED_NBR_GEOMETRIE_MAILLE ; i++)
    {
      cout << endl;
      cout << geoNames[tous[i].getType()] << endl;
      cout << "__________________"  << endl;

      try 
        {
          cout << "CellModel de Nom : " << tous[i].getName() << endl;
        }
      catch ( const std::exception &e )
        {
          cout << "-------------------------------" << endl;
          cout << "     Pb au getName()         " << endl;
          cout << "-------------------------------" << endl;
          MESSAGE_MED( "catched exception : " << e.what() ) ;
          return EXIT_FAILURE ;
        }
      catch (...)
        {
          cout << "-------------------------------" << endl;
          cout << "     Pb au getName()         " << endl;
          cout << "-------------------------------" << endl;
          return EXIT_FAILURE ;
        };

      try 
        {
          cout << "Nb de Vertexes   : " << tous[i].getNumberOfVertexes() << endl;
        }
      catch ( const std::exception &e )
        {
          cout << "-------------------------------" << endl;
          cout << "     Pb au getNumberOfVertexes()     " << endl;
          cout << "-------------------------------" << endl;
          MESSAGE_MED( "catched exception : " << e.what() ) ;
          return EXIT_FAILURE ;
        }
      catch (...)
        {
          cout << "---------------------------------------" << endl;
          cout << "     Pb au getNumberOfVertexes()     " << endl;
          cout << "---------------------------------------" << endl;
          return EXIT_FAILURE ;
        };

      try 
        {
          cout << "Nb de Noeuds     : " << tous[i].getNumberOfNodes() << endl;
        }
      catch ( const std::exception &e )
        {
          cout << "-------------------------------" << endl;
          cout << "     Pb au getNumberOfNodes()  " << endl;
          cout << "-------------------------------" << endl;
          MESSAGE_MED( "catched exception : " << e.what() ) ;
          return EXIT_FAILURE ;
        }
      catch (...)
        {
          cout << "---------------------------------" << endl;
          cout << "     Pb au getNumberOfNodes()  " << endl;
          cout << "---------------------------------" << endl;
          return EXIT_FAILURE ;
        };

      int dimension;
      try 
        {
          dimension=tous[i].getDimension();
          cout << "Dimension        : " << dimension << endl;   
        }
      catch ( const std::exception &e )
        {
          cout << "-------------------------------" << endl;
          cout << "     Pb au getDimension()    " << endl;
          cout << "-------------------------------" << endl;
          MESSAGE_MED( "catched exception : " << e.what() ) ;
          return EXIT_FAILURE ;
        }
      catch (...)
        {
          cout << "-------------------------------" << endl;
          cout << "     Pb au getDimension()    " << endl;
          cout << "-------------------------------" << endl;
          return EXIT_FAILURE ;
        }

      for (int dim=1; dim< dimension; dim++)
        {
          cout << "Nb d elements de dimension " << dim << " : ";

          try
            {
              cout << tous[i].getNumberOfConstituents(dim)<< endl;
            }
          catch ( const std::exception &e )
            {
              cout << "------------------------------------" << endl;
              cout << "  Pb au getNumberOfConstituents()   " << endl;
              cout << "------------------------------------" << endl;
              MESSAGE_MED( "catched exception : " << e.what() ) ;
              return EXIT_FAILURE ;
            }
          catch (...)
            {
              cout << "------------------------------------" << endl;
              cout << "  Pb au getNumberOfConstituents()   " << endl;
              cout << "------------------------------------" << endl;
              return EXIT_FAILURE ;
            };
        }
/*

                try
                {
                        int** v1=tous[i].getConstituents(dim2);
                        if ( v1.size() != tous[i].getNumberOfConstituents(dim2) )
                        {
                                cout << "----------------------------" << endl;
                                cout << "  Pb au getConstituents()   " << endl;
                                cout << "----------------------------" << endl;
                        }
                }
                catch ( const std::exception &e )
                {
                        cout << "-------------------------------" << endl;
                        cout << "  Pb au getConstituents()   " << endl;
                        cout << "-------------------------------" << endl;
                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                        return EXIT_FAILURE ;
                }
                catch (...)
                {
                        cout << "----------------------------" << endl;
                        cout << "  Pb au getConstituents()   " << endl;
                        cout << "----------------------------" << endl;
                        return EXIT_FAILURE ;
                };

                cout << "Types de ces elements " << endl; 

                set <medGeometryElement> setGeomElt;
                medGeometryElement * vt;
                int taille;
                try
                {
                        vt=tous[i].getConstituentsType(dim2); 
                        taille=tous[i].getNumberOfConstituents(dim2);
                }
                catch ( const std::exception &e )
                {
                        cout << "-------------------------------" << endl;
                        cout << "  Pb au getConstituentsType()   " << endl;
                        cout << "-------------------------------" << endl;
                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                        return EXIT_FAILURE ;
                }
                catch (...)
                {
                        cout << "--------------------------------" << endl;
                        cout << "  Pb au getConstituentsType()   " << endl;
                        cout << "--------------------------------" << endl;
                        return EXIT_FAILURE ;
                };
                for (int k=0; k<taille;k++)
                {
                        try
                        {
                                medGeometryElement me=tous[i].getConstituentType(dim2,k+1);
                                if  ( vt[k] != me )
                                {
                                        cout << "-------------------------------" << endl;
                                        cout << "  Pb au getConstituentType()   " << endl;
                                        cout << "-------------------------------" << endl;
                                }
                                setGeomElt.insert(me);
                        }
                        catch ( const std::exception &e )
                        {
                                cout << "-------------------------------" << endl;
                                cout << "  Pb au getConstituentType()   " << endl;
                                cout << "-------------------------------" << endl;
                                MESSAGE_MED( "catched exception : " << e.what() ) ;
                                return EXIT_FAILURE ;
                        }
                        catch (...)
                        {
                                cout << "--------------------------------" << endl;
                                cout << "  Pb au getConstituentsType()   " << endl;
                                cout << "--------------------------------" << endl;
                                return EXIT_FAILURE ;
                        }
                }
*/
/*
                set <medGeometryElement>::iterator iter_s;
                for (iter_s=setGeomElt.begin() ;iter_s!=setGeomElt.end();iter_s++)
                {
                        cout << geoNames[(*iter_s)] << endl;    
                }
                
                cout << "Impression des coordonnées du 1er Element de ce type" << endl;
                try
                {
                        vector<int> v2=tous[i].getNodesConstituent(dim2,1); 
                        for (int  k=0; k<v2.size();k++)
                        {
                                cout << v2[k] << " ";
                                try
                                {
                                        int verif=tous[i].getNodeConstituent(dim2,1,k+1);
                                        if ( v2[k] != verif)
                                        {
                                                cout << "-------------------------------" << endl;
                                                cout << "  Pb au getNodeConstituent()   " << endl;
                                                cout << "-------------------------------" << endl;
                                        }
                                }
                                catch ( const std::exception &e )
                                {
                                        cout << "-------------------------------" << endl;
                                        cout << "  Pb au getNodeConstituent()   " << endl;
                                        cout << "-------------------------------" << endl;
                                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                                        return EXIT_FAILURE ;
                                }
                                catch (...)
                                {
                                        cout << "-------------------------------" << endl;
                                        cout << "  Pb au getNodeConstituent()   " << endl;
                                        cout << "-------------------------------" << endl;
                                        return EXIT_FAILURE ;
                                }
                        }
                        cout << endl;
                }
                catch ( const std::exception &e )
                {
                        cout << "-------------------------------" << endl;
                        cout << "  Pb au getNodeConstituent()    " << endl;
                        cout << "-------------------------------" << endl;
                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                        return EXIT_FAILURE ;
                }
                catch (...)
                {
                        cout << "--------------------------------" << endl;
                        cout << "  Pb au getNodeConstituent()    " << endl;
                        cout << "--------------------------------" << endl;
                        return EXIT_FAILURE ;
                }
*/

      cout << endl;
/*
        if ( dimension > 1 )
        {
                try
                {
                        cout << "Informations particulieres -  dimension " << dimension - 1 << endl;
                        set <medGeometryElement>::iterator iter_s;
                        for (iter_s=tous[i].getAllConstituentsType().begin() ;
                        iter_s!=tous[i].getAllConstituentsType().end();iter_s++)
                        {
                                cout << geoNames[*iter_s] << endl;      
                        }
                }
                catch ( const std::exception &e )
                {
                        cout << "-------------------------------" << endl;
                        cout << "  Pb au getAllConstituentsType()    " << endl;
                        cout << "-------------------------------" << endl;
                        MESSAGE_MED( "catched exception : " << e.what() ) ;
                        return EXIT_FAILURE ;
                }
                catch (...)
                {
                        cout << "------------------------------------" << endl;
                        cout << "  Pb au getAllConstituentsType()    " << endl;
                        cout << "------------------------------------" << endl;
                        return EXIT_FAILURE ;
                }
        }
*/

    }

  delete[] tous ;

  return EXIT_SUCCESS ;
/*
  Reste a Tester
  // Return number of constituents foreach type (which dimension is _dimension-1).
  map <medGeometryElement,int>  getNumberOfConstituentsForeachType() const; 
*/
}
