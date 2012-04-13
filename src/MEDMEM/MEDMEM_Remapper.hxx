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

#ifndef MEDMEM_REMAPPER_HXX_
#define MEDMEM_REMAPPER_HXX_

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM.hxx"
#include "MEDNormalizedUnstructuredMesh.hxx"
#include "MEDNormalizedUnstructuredMesh.txx"
#include "InterpKernelMatrix.hxx"
#include "InterpolationOptions.hxx"



class MEDMEM_EXPORT MEDMEM_REMAPPER : public INTERP_KERNEL::InterpolationOptions
{
public:
  MEDMEM_REMAPPER();
  virtual ~MEDMEM_REMAPPER();
  int prepare(const MEDMEM::MESH& source_mesh, const MEDMEM::MESH& target_mesh, const char *method);
  void transfer(const MEDMEM::FIELD<double>& source_field, MEDMEM::FIELD<double>& target_field);
  void reverseTransfer(MEDMEM::FIELD<double>& source_field, const MEDMEM::FIELD<double>& target_field);
  MEDMEM::FIELD<double> * transferField(const MEDMEM::FIELD<double>& source_field);
  MEDMEM::FIELD<double> * reverseTransferField(const MEDMEM::FIELD<double>& target_field);
  void printMatrixInfo();
  int setOptionDouble(const std::string& key, double value);
  int setOptionInt(const std::string& key, int value);
  int setOptionString(const std::string& key, std::string& value);
private :
  INTERP_KERNEL::Matrix<double, INTERP_KERNEL::ALL_FORTRAN_MODE>* _matrix;
  MEDMEM::FIELD<double>* getSupportVolumes(const MEDMEM::SUPPORT& support);
  std::vector<double> _deno_multiply;
  std::vector<double> _deno_reverse_multiply;
  int _nb_rows;
  int _nb_cols;
  string _sourceFieldType;
  string _targetFieldType;
  const MEDMEM::MESH * _sourceMesh;
  const MEDMEM::MESH * _targetMesh;
  const MEDMEM::SUPPORT * _sourceSupport;
  const MEDMEM::SUPPORT * _targetSupport;
};


#endif /*REMAPPER_HXX_*/
