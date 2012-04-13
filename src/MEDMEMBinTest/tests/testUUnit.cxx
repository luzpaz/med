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

//  File   : testUUnit.cxx
//  Module : MED
//
#include <cstdlib>
#include <exception>

#include "MEDMEM_Unit.hxx"
using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv)
{

  cout << "UNIT Test" << endl ;
  cout << "---------" << endl;

  UNIT testu;
  try
  {
        testu.setName("ForTesting") ;
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec setName " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec setName " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setDescription("all values are setted 10") ;
  }
  catch ( const std::exception &e )
  {
        cout << "------------------------" << endl;
        cout << " pb avec setDescription " << endl;
        cout << "------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "------------------------" << endl;
        cout << " pb avec setDescription " << endl;
        cout << "------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setMasse(10);
  }
  catch ( const std::exception &e )
  {
        cout << "------------------" << endl;
        cout << " pb avec setMasse " << endl;
        cout << "------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "------------------" << endl;
        cout << " pb avec setMasse " << endl;
        cout << "------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setLength(10);
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------" << endl;
        cout << " pb avec setLength " << endl;
        cout << "-------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------" << endl;
        cout << " pb avec setLength " << endl;
        cout << "-------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setTime(10);
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec setTime " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec setTime " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setTemperature(10);
  }
  catch ( const std::exception &e )
  {
        cout << "------------------------" << endl;
        cout << " pb avec setTemperature " << endl;
        cout << "------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "------------------------" << endl;
        cout << " pb avec setTemperature " << endl;
        cout << "------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setMatterQuantity(10);
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << " pb avec setMatterQuantity " << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------------------" << endl;
        cout << " pb avec setMatterQuantity " << endl;
        cout << "---------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setCurrentStrength(10) ;
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << " pb avec setCurrentStrengt " << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------------------" << endl;
        cout << " pb avec setCurrentStrengt " << endl;
        cout << "---------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        testu.setLightIntensity(10) ;
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << " pb avec setLightIntensity " << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------------------" << endl;
        cout << " pb avec setLightIntensity " << endl;
        cout << "---------------------------" << endl;
        return EXIT_FAILURE ;
  }


  try
  {
        cout << testu.getName() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec getName " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec getName " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getDescription() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "------------------------" << endl;
        cout << " pb avec getDescription " << endl;
        cout << "------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "------------------------" << endl;
        cout << " pb avec getDescription " << endl;
        cout << "------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getMasse() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "------------------" << endl;
        cout << " pb avec getMasse " << endl;
        cout << "------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "------------------" << endl;
        cout << " pb avec getMasse " << endl;
        cout << "------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getLength() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "-------------------" << endl;
        cout << " pb avec getLength " << endl;
        cout << "-------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-------------------" << endl;
        cout << " pb avec getLength " << endl;
        cout << "-------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getTime() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "-----------------" << endl;
        cout << " pb avec getTime " << endl;
        cout << "-----------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "-----------------" << endl;
        cout << " pb avec getTime " << endl;
        cout << "-----------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getTemperature() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "------------------------" << endl;
        cout << " pb avec getTemperature " << endl;
        cout << "------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "------------------------" << endl;
        cout << " pb avec getTemperature " << endl;
        cout << "------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getMatterQuantity() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << " pb avec getMatterQuantity " << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------------------" << endl;
        cout << " pb avec getMatterQuantity " << endl;
        cout << "---------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getCurrentStrength() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "----------------------------" << endl;
        cout << " pb avec getCurrentStrength " << endl;
        cout << "----------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "----------------------------" << endl;
        cout << " pb avec getCurrentStrength " << endl;
        cout << "----------------------------" << endl;
        return EXIT_FAILURE ;
  }

  try
  {
        cout << testu.getLightIntensity() << endl ;
  }
  catch ( const std::exception &e )
  {
        cout << "---------------------------" << endl;
        cout << " pb avec getLightIntensity " << endl;
        cout << "---------------------------" << endl;
        MESSAGE_MED( "catched exception : " << e.what() ) ;
        return EXIT_FAILURE ;
  }
  catch (...)
  {
        cout << "---------------------------" << endl;
        cout << " pb avec getLightIntensity " << endl;
        cout << "---------------------------" << endl;
        return EXIT_FAILURE ;
  }

  return EXIT_SUCCESS ;

}
