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

#include <cmath>

using namespace std;
using namespace MEDMEM;

/*!
 *  Check method shallowCopy()
 */
int main (int argc, char** argv)
{
  int ld = 7, nb = 4;
  double rr = 13.0, h0 = 7.0;
  double * aValues = new double [ld * nb];
  for (int nn = 0; nn < nb; nn++) {
    aValues[nn*ld + 0] = nn;
    aValues[nn*ld + 1] = rr * cos(nn * M_PI/4.);
    aValues[nn*ld + 2] = rr * sin(nn * M_PI/4.);
    aValues[nn*ld + 3] = h0 + nn;
    aValues[nn*ld + 4] = 2.0 * rr * cos(nn * M_PI/4.);
    aValues[nn*ld + 5] = 2.0 * rr * sin(nn * M_PI/4.);
    aValues[nn*ld + 6] = h0 - nn;
  }

  MEDARRAY<double> * aSpiral =
    new MEDARRAY<double> (aValues, ld, nb, MED_EN::MED_FULL_INTERLACE,
                          /*shallowCopy=*/false, /*ownershipOfValues=*/false);

  // Check method shallowCopy()
  MEDARRAY<double> shc;

  // (BUG) MEDARRAY<T>::shallowCopy() usage leads to memory leaks,
  // because on attempt to free both arrays we have Segmentation Fault
  shc.shallowCopy(*aSpiral);

  delete aSpiral;

  // hangs up
  //MEDARRAY<double> arr (10, 10);
  //MEDARRAY<double> shc;
  //shc.shallowCopy(arr);

  // Segmentation fault
  //MEDARRAY<double>* arr = new MEDARRAY<double> (10, 10);
  //MEDARRAY<double>* shc = new MEDARRAY<double> (10, 10);
  //shc->shallowCopy(*arr);
  //delete arr;
  //delete shc;
}
