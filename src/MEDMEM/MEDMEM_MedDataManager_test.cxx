//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDMEM_MedDataManager.hxx"

// Standard C include (for getenv)
#include <stdlib.h>
#include <string>
#define MED_ROOT_DIR getenv("MED_ROOT_DIR")
#define CONCATSTR(x,y) (new string(x))->append(y)
#define TESTDIRNAME CONCATSTR(MED_ROOT_DIR,"/share/salome/resources/med/")
#define TESTFILENAME CONCATSTR(TESTDIRNAME,"TimeStamps.med")
#define TESTFIELDNAME "temperature"

//
// Helper macro for redirect standard error stream (very verbose in MEDMEM)
//
#include <streambuf>
static streambuf* cout_sbuf = cerr.rdbuf(); // save original sbuf
#define ERR_REDIRECT(filename) \
  ofstream ferr(filename); \
  cerr.rdbuf(ferr.rdbuf()); // redirect 'cerr' to the output file stream 'filename'

#define ERR_RESTORE \
  cerr.rdbuf(cout_sbuf); // restore the original stream buffer 


// ================================================================================
void TEST_getField() {
  string testFilename = "/home/gboulant/development/projets/salome/devel/medmem/XMED/XMED_SRC/resources/datafiles/testfield.med";
  //string testFilename = TESTFILENAME;
  MedDataManager * medDataManager = new MedDataManager(testFilename);
  
  int fieldIdx = 0;
  int iterationIdx = 0;

  FIELD<double> * field = medDataManager->getFieldDoubleFromId(fieldIdx,iterationIdx);
  //medDataManager->printFieldDouble(field);

  fieldIdx = 1;
  field = medDataManager->getFieldDoubleFromId(fieldIdx,iterationIdx);
  //medDataManager->printFieldDouble(field);

  fieldIdx = 0;
  field = medDataManager->getFieldDoubleFromId(fieldIdx,iterationIdx);
  //medDataManager->printFieldDouble(field);
}




// ================================================================================
int main(int argc, char ** argv) {
  // We first redirect the error output stream (very verbose in MEDMEM)
  ERR_REDIRECT("log.err");

  // List of use case to be executed
  TEST_getField();


  ERR_RESTORE;
  return 0;
}
