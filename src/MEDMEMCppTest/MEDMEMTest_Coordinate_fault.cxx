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

#include "MEDMEM_Array.hxx"
#include "MEDMEM_Coordinate.hxx"

using namespace std;
using namespace MEDMEM;

int main (int argc, char** argv)
{
  COORDINATE anEmptyC (2, 10, MED_EN::MED_FULL_INTERLACE);

  // (BUG) Incoherence between setCoordinateName() and getCoordinateName()
  //       C++ index notation is used in "set" methods, while in "get" methods indices start from 1.
  anEmptyC.setCoordinateName("alpha", 1);
  anEmptyC.setCoordinateName("betta", 2);
  // (BUG) Incoherence between setCoordinateUnit() and getCoordinateUnit()
  //       C++ index notation is used in "set" methods, while in "get" methods indices start from 1.
  anEmptyC.setCoordinateUnit("ttt", 1);
  anEmptyC.setCoordinateUnit("sss", 2);

  //CPPUNIT_ASSERT(anEmptyC.getCoordinateName(1) == "alpha");
  //CPPUNIT_ASSERT(anEmptyC.getCoordinateName(2) == "betta");
  //CPPUNIT_ASSERT(anEmptyC.getCoordinateUnit(1) == "ttt");
  //CPPUNIT_ASSERT(anEmptyC.getCoordinateUnit(2) == "sss");

  int len = 10 * 2;
  double * cc = new double[len];
  for (int i = 0; i < len; i++) {
    cc[i] = (double)(i + 1);
  }

  MEDARRAY<double> mcc (cc, 2, 10, MED_EN::MED_FULL_INTERLACE, false, false);
  delete [] cc;

  // (BUG) Bug in COORDINATE::setCoordinates() in shallow copy mode:
  // Segmentation Fault or Hang up after anEmptyC and mcc destruction,
  // because array will be owned by two pointers (in mcc and in anEmptyC) after this call
  anEmptyC.setCoordinates(&mcc, true);
  // In other case (if we dynamically allocate mcc and do not free it) we will have memory leak.
}
