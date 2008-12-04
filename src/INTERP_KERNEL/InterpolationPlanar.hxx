//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _INTERPOLATIONPLANAR_HXX_
#define _INTERPOLATIONPLANAR_HXX_

#include "Interpolation.hxx"
#include "PlanarIntersector.hxx"
#include "NormalizedUnstructuredMesh.hxx"
#include "InterpolationOptions.hxx"

namespace INTERP_KERNEL
{
  template<class RealPlanar>
  class INTERPKERNEL_EXPORT InterpolationPlanar : public Interpolation< InterpolationPlanar<RealPlanar> >
  {
  private:
    double _dimCaracteristic;
    static const double DEFAULT_PRECISION;

  public:
    InterpolationPlanar();
    InterpolationPlanar(const InterpolationOptions & io);

    // geometric precision, debug print level, coice of the median plane, intersection etc ...
    void setOptions(double precision, int printLevel,
                    IntersectionType intersectionType, int orientation=0);

    // Main function to interpolate triangular and quadratic meshes
    template<class MatrixType, class MyMeshType>
    void interpolateMeshes(const MyMeshType& mesh1, const MyMeshType& mesh2, MatrixType& result);

  public:
    bool doRotate() const { return asLeafInterpPlanar().doRotate(); }
    double medianPlane() const { return asLeafInterpPlanar().medianPlane(); }
    template<class MyMeshType>
    void performAdjustmentOfBB(PlanarIntersector<MyMeshType>* intersector, std::vector<double>& bbox) const
    { return asLeafInterpPlanar().performAdjustmentOfBB(intersector,bbox); }

  protected:
    RealPlanar& asLeafInterpPlanar() { return static_cast<RealPlanar&>(*this); }
    const RealPlanar& asLeafInterpPlanar() const { return static_cast< const RealPlanar& >(*this); }
  };
}

#endif
