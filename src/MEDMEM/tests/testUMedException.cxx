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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : testUMedException.cxx
//  Module : MED

#include <cstdlib>
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"

using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv)
{
	MEDEXCEPTION a = MEDEXCEPTION("test med exception");
	cout << a << endl;;
	cout << a.what() << endl;;

	MEDEXCEPTION b(a);
	cout << b << endl;
	cout << b.what() << endl;;

	MEDEXCEPTION c("test med exception",argv[0],14);
	cout << c << endl;
	cout << c.what() << endl;;

      	STRING msgErr;
	msgErr << "ESSAI::ESSAI()!  "<< 4 << "ieme essai ";
        cout << MEDEXCEPTION (LOCALIZED(msgErr)).what() << endl ;

	const char * LOC = "toto" ;
        cout << MEDEXCEPTION (LOCALIZED(STRING(LOC)<<" et titi")).what() << endl ;

	return EXIT_SUCCESS;	

};
