// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef __MEDMEM_UTILITIES
#define __MEDMEM_UTILITIES

// standart Linux/Unix functions 
#include <string>

#ifndef WNT
  #include <libgen.h>
#endif
static std::string getBaseName( std::string dataname ) {
  std::string aBaseName = "";
#ifndef WNT
  aBaseName = basename((char*)dataname.c_str());
#else
  for ( int i = dataname.size()-1; i >= 0; i-- ) {
    char aSymb = dataname[i];
    if ( dataname[i] == '\\' || dataname[i] == '/' )
      break;
    aBaseName = dataname[i] + aBaseName;
  }
#endif
  return aBaseName;
}

static std::string getDirName( std::string dataname ) {
  std::string aDirName = "";
#ifndef WNT
  aDirName = dirname((char*)dataname.c_str());
#else
  bool aFindLine = false;
  for ( int i = dataname.size()-1; i >= 0; i-- ) {
    char aSymb = dataname[i];
    if ( !aFindLine )
      aFindLine = dataname[i] == '\\' || dataname[i] == '/';
    else
      aDirName = dataname[i] + aDirName;
  }
  if ( !aFindLine )
    aDirName = '.';
#endif
  return aDirName;
}

#ifdef MED_WITHOUT_KERNEL

#  include <cstdlib>
#  include <iostream>
using namespace std;

/* ---  INFOS is always defined (without _DEBUG_): to be used for warnings, with release version --- */

# define HEREWEARE {cout<<flush ; cerr << __FILE__ << " [" << __LINE__ << "] : " << flush ;}
# define INFOS(chain) {HEREWEARE ; cerr << chain << endl ;}
# define PYSCRIPT(chain) {cout<<flush ; cerr << "---PYSCRIPT--- " << chain << endl ;}


/* --- To print date and time of compilation of current source on stdout --- */

# if defined ( __GNUC__ )
# define COMPILER		"g++" ;
# elif defined ( __sun )
# define COMPILER		"CC" ;
# elif defined ( __KCC )
# define COMPILER		"KCC" ;
# elif defined ( __PGI )
# define COMPILER		"pgCC" ;
# else
# define COMPILER		"undefined" ;
# endif

# ifdef INFOS_COMPILATION
# undef INFOS_COMPILATION
# endif
# define INFOS_COMPILATION	{\
					cerr << flush;\
					cout << __FILE__ ;\
					cout << " [" << __LINE__ << "] : " ;\
					cout << "COMPILED with " << COMPILER ;\
					cout << ", " << __DATE__ ; \
					cout << " at " << __TIME__ << endl ;\
					cout << "\n\n" ;\
					cout << flush ;\
				}

# ifdef _DEBUG_

/* --- the following MACROS are useful at debug time --- */

# define HERE {cout<<flush ; cerr << "- Trace " << __FILE__ << " [" << __LINE__ << "] : " << flush ;}
# define SCRUTE(var) {HERE ; cerr << #var << "=" << var << endl ;}
# define MESSAGE(chain) {HERE ; cerr << chain << endl ;}
# define INTERRUPTION(code) {HERE ; cerr << "INTERRUPTION return code= " << code << endl ; exit(code) ;}

# ifndef ASSERT
# define ASSERT(condition) if (!(condition)){ HERE ; cerr << "CONDITION " << #condition << " NOT VERIFIED"<< endl ; INTERRUPTION(1) ;}
# endif /* ASSERT */

#define REPERE {cout<<flush ; cerr << "   --------------" << endl << flush ;}
#define BEGIN_OF(chain) {REPERE ; HERE ; cerr << "Begin of: " << chain << endl ; REPERE ; }
#define END_OF(chain) {REPERE ; HERE ; cerr << "Normal end of: " << chain << endl ; REPERE ; }



# else /* ifdef _DEBUG_*/

# define HERE
# define SCRUTE(var) {}
# define MESSAGE(chain) {}
# define INTERRUPTION(code) {}

# ifndef ASSERT
# define ASSERT(condition) {}
# endif /* ASSERT */

#define REPERE
#define BEGIN_OF(chain) {}
#define END_OF(chain) {}

#endif

#else
// #ifdef _SALOME

#  include <utilities.h>
#endif
#endif
