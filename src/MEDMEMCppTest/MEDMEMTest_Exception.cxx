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

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_STRING.hxx"

#include <sstream>
#include <cmath>
#include <cstring>

using namespace std;
using namespace MEDMEM;

// #12: MEDMEM_Exception.hxx  }  MEDMEMTest_Exception.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_Exception.hxx:
 *   (+)     #define LOCALIZED(message) static_cast<const char *> (message) , __FILE__ , __LINE__
 *  class MEDEXCEPTION : public std::exception {
 *   (+)     MEDEXCEPTION(const char *text, const char *fileName=0, const unsigned int lineNumber=0);
 *   (+)     MEDEXCEPTION(const MEDEXCEPTION &ex);
 *   (+)     ~MEDEXCEPTION() throw ();
 *   (+)     friend ostream & operator<< (ostream &os, const MEDEXCEPTION &ex);
 *   (+)     virtual const char *what(void) const throw ();
 *  }
 *  class MED_DRIVER_NOT_FOUND_EXCEPTION : public MEDEXCEPTION {
 *   (+)     MED_DRIVER_NOT_FOUND_EXCEPTION(const char *text, const char *fileName=0,
 *                                          const unsigned int lineNumber=0);
 *   (+)     MED_DRIVER_NOT_FOUND_EXCEPTION(const MED_DRIVER_NOT_FOUND_EXCEPTION &ex);
 *   (+)     ~MED_DRIVER_NOT_FOUND_EXCEPTION() throw();
 *  }
 *
 *  Use code of MEDMEM/tests/testUMedException.cxx
 */
void MEDMEMTest::testException()
{
  {
    // test simple constructor
    MEDEXCEPTION a = MEDEXCEPTION("test med exception");
    CPPUNIT_ASSERT(a.what());
    CPPUNIT_ASSERT(strcmp(a.what(), "MED Exception : test med exception") == 0);

    // test copy constructor
    MEDEXCEPTION b (a);
    CPPUNIT_ASSERT(b.what());
    CPPUNIT_ASSERT(strcmp(b.what(), "MED Exception : test med exception") == 0);

    // test dumping of exception in stream
    ostringstream ostr;
    ostr << b;
    CPPUNIT_ASSERT(ostr.str() == "MED Exception : test med exception");
  }

  {
    // test constructor from three arguments
    MEDEXCEPTION c ("test med exception", "<file_name>.cxx", 14);
    CPPUNIT_ASSERT(c.what());
    CPPUNIT_ASSERT(strcmp(c.what(), "MED Exception in <file_name>.cxx [14] : test med exception") == 0);
  }

  {
    // Test macro LOCALIZED
    STRING msgErr;
    msgErr << "ESSAI::ESSAI()!  "<< 4 << "ieme essai ";

    const char* prefix = "MED Exception";
    const char* exctxt = static_cast<const char *>(msgErr);
    const char* filen = __FILE__;
    unsigned int linen = __LINE__ + 5;
    const size_t len = strlen(prefix) + strlen(exctxt) + strlen(filen) +
      (1 + int(log10(float(linen)))) + 10 + 1;
    char* excmsg = new char [len];
    sprintf(excmsg, "%s in %s [%u] : %s", prefix, filen, linen, exctxt);
    CPPUNIT_ASSERT(strcmp(MEDEXCEPTION(LOCALIZED(msgErr)).what(), excmsg) == 0);

    delete [] excmsg;
  }

  {
    // Test function STRING<<
    const char * LOC = "toto";
    CPPUNIT_ASSERT(MEDEXCEPTION(LOCALIZED(STRING(LOC) << " et titi")).what());

    // Test throw
    MEDEXCEPTION d (LOCALIZED(STRING(LOC) << " et titi"));
    const char* dtxt = d.what();
    try {
      throw d;
    }
    catch (MEDEXCEPTION dc) {
      CPPUNIT_ASSERT(strcmp(dc.what(), dtxt) == 0);
    }
    catch (...) {
      CPPUNIT_FAIL("Unknown exception");
    }
  }

  {
    // Test MED_DRIVER_NOT_FOUND_EXCEPTION
    MED_DRIVER_NOT_FOUND_EXCEPTION g ("mdnfe");
    try {
      throw g;
    }
    catch (MED_DRIVER_NOT_FOUND_EXCEPTION mdnfe) {
      CPPUNIT_ASSERT(strcmp(mdnfe.what(), g.what()) == 0);
    }
    catch (MEDEXCEPTION me) {
      CPPUNIT_FAIL(me.what());
    }
    catch (...) {
      CPPUNIT_FAIL("Unknown exception");
    }

    // Copy constructor
    MED_DRIVER_NOT_FOUND_EXCEPTION gcopy (g);
    CPPUNIT_ASSERT(gcopy.what());
    CPPUNIT_ASSERT(strcmp(gcopy.what(), g.what()) == 0);
  }
}
