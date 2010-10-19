//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

#ifndef __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENT_HXX__
#define __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENT_HXX__

#include <string>

namespace ParaMEDMEM
{
  class MEDCouplingUMesh;
  class MEDCouplingFieldDouble;
  class MEDCouplingExtrudedMesh;
  class MEDCouplingCMesh;
}

namespace SALOME_TEST
{
  class MEDCouplingCorbaServBasicsTest
  {
  public:
    static ParaMEDMEM::MEDCouplingUMesh *build1DMesh();
    static ParaMEDMEM::MEDCouplingUMesh *build2DMesh();
    static ParaMEDMEM::MEDCouplingUMesh *build3DMesh();
    static ParaMEDMEM::MEDCouplingUMesh *build3DSurfMesh();
    static ParaMEDMEM::MEDCouplingUMesh *build0DMesh();
    static ParaMEDMEM::MEDCouplingUMesh *buildM1DMesh();
    static ParaMEDMEM::MEDCouplingExtrudedMesh *buildExtrudedMesh(ParaMEDMEM::MEDCouplingUMesh *&m2D);
    static ParaMEDMEM::MEDCouplingCMesh *buildCMesh();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldScalarOn2DNT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldNodeScalarOn2DNT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldScalarOn3DNT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldScalarOn3DSurfWT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldScalarOn3DSurfCOTI();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldScalarOn2DLT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldGaussPt2DWT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldGaussPtNE2DWT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldVectorOnExtrudedWT();
    static ParaMEDMEM::MEDCouplingFieldDouble *buildFieldVectorOnCMeshWT();
    static std::string buildFileNameForIOR();
  };
}

#endif
