// Copyright (C) 2006  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>
#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (12), defined in MEDMEM_PointerOf.hxx:
 *  template <typename T> class PointerOf {
 *   (+) PointerOf();
 *   (+) ~PointerOf();
 *   (+) PointerOf(const int &size);
 *   (+) PointerOf(const T *pointer);
 *   (+) PointerOf(const int &size, const T *pointer);
 *   (+) PointerOf(const PointerOf<T> & pointerOf);
 *   (yetno) PointerOf(const int &size, const PointerOf<T> & pointerOf);
 *   (+) operator T*();
 *   (+) operator const T*() const;
 *   (+) void set(const int &size);
 *   (+) void set(const T *pointer);
 *   (+) void set(const int &size, const T *pointer);
 *   (+) void setShallowAndOwnership(const T *pointer);
 *   (+) PointerOf<T>& operator=(const PointerOf<T> &pointer);
 *  }
 *
 */
void MEDMEMTest::testPointerOf()
{
  const int size=10;
  PointerOf<int> P;

  try
  {
    P.set(0);
    CPPUNIT_ASSERT((int *)P == NULL);
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }

  try
  {
    P.set(-1 * size);
    CPPUNIT_ASSERT((int *)P == NULL);
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }

  try
  {
    P.set(size);
    CPPUNIT_ASSERT((int *)P != NULL);
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
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
    CPPUNIT_ASSERT_EQUAL(P2[i],P3[i]);
  }

  const PointerOf<int> P4(p);
  for (int i=0; i < size; i++)
  {
    CPPUNIT_ASSERT_EQUAL(P4[i],p[i]);
  }

  const PointerOf<int> P5(P4);
  CPPUNIT_ASSERT((const int*)P5 != NULL);

  const PointerOf<int> P7(10, p);
  CPPUNIT_FAIL("Can not create: PointerOf(const int &size, const PointerOf<T> & pointerOf)");

  CPPUNIT_ASSERT_THROW(P3.set(0, p),MEDEXCEPTION);

  P.setShallowAndOwnership( p );
  for (int i=0; i < size; i++)
  {
    CPPUNIT_ASSERT_EQUAL(P[i],p[i]);
  }

  //delete [] p; not needed - P already owns p and will delete it when the scope is over
}


