// Copyright (C) 2007-2015  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENT_HXX__
#define __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENT_HXX__

#include <string>

namespace ParaMEDMEM
{
  class MEDCouplingUMesh;
  class MEDCouplingFieldDouble;
  class MEDCouplingFieldTemplate;
  class MEDCouplingExtrudedMesh;
  class MEDCoupling1DGTUMesh;
  class MEDCoupling1SGTUMesh;
  class MEDCouplingCMesh;
  class MEDCouplingIMesh;
  class MEDCouplingCurveLinearMesh;
  class DataArrayDouble;
  class DataArrayInt;
  class MEDCouplingMultiFields;
  class MEDCouplingFieldOverTime;
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
    static ParaMEDMEM::MEDCouplingIMesh *buildIMesh();
    static ParaMEDMEM::MEDCouplingCurveLinearMesh *buildCLMesh();
    static ParaMEDMEM::MEDCoupling1SGTUMesh *build1SGTUMesh();
    static ParaMEDMEM::MEDCoupling1DGTUMesh *build1DGTUMesh();
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
    static ParaMEDMEM::MEDCouplingFieldTemplate *buildFieldTemplateCellOn2D();
    static ParaMEDMEM::MEDCouplingFieldTemplate *buildFieldTemplateNodeOn2D();
    static ParaMEDMEM::MEDCouplingFieldTemplate *buildFieldTemplateGaussPtOn2D();
    static ParaMEDMEM::MEDCouplingFieldTemplate *buildFieldTemplateGaussNEOn2D();
    static ParaMEDMEM::MEDCouplingMultiFields *buildMultiFields1();
    static ParaMEDMEM::DataArrayDouble *buildArrayDouble1();
    static ParaMEDMEM::DataArrayDouble *buildArrayDouble2();
    static ParaMEDMEM::DataArrayDouble *buildArrayDouble3();
    static ParaMEDMEM::DataArrayInt *buildArrayInt1();
    static ParaMEDMEM::DataArrayInt *buildArrayInt2();
    static ParaMEDMEM::DataArrayInt *buildArrayInt3();
    static ParaMEDMEM::MEDCouplingFieldOverTime *buildMultiFields2();
    static std::string buildFileNameForIOR();
  };
}

#endif
