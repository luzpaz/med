// Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
//           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
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
#ifndef __MEDMEM_TRAITSFORFIELDS_HXX__
#define __MEDMEM_TRAITSFORFIELDS_HXX__

#include CORBA_SERVER_HEADER(MED)
#include "MEDMEM_Tags.hxx"
#include "MEDMEM_InterlacingPolicy.hxx"

namespace MEDMEM
{
template < typename T, typename INTERLACING_TAG >
struct FIELDI_TRAITS
{
  typedef typename T::InterfaceForServant InterfaceForServant;
  typedef typename T::FieldCorbaPtrType FieldCorbaPtrType;
  typedef typename T::SimpleFieldCorbaPtrType SimpleFieldCorbaPtrType;
  typedef typename T::SimpleFieldGlobalType SimpleFieldGlobalType;
  typedef typename T::SeqType SeqType;
  typedef typename T::SeqVarType SeqVarType;
  typedef typename T::SenderPtrType SenderPtrType;
  typedef typename T::SenderVarType SenderVarType;
};

template < >
struct FIELDI_TRAITS<double, FullInterlace>
{
  typedef POA_SALOME_MED::FIELDDOUBLEFULL InterfaceForServant;
  typedef SALOME_MED::FIELDDOUBLEFULL_ptr FieldCorbaPtrType;
  typedef SALOME_MED::FIELDDOUBLE_ptr SimpleFieldCorbaPtrType;
  typedef SALOME_MED::FIELDDOUBLE SimpleFieldGlobalType;
  typedef SALOME_MED::double_array SeqType;
  typedef SALOME_MED::double_array_var SeqVarType;
  typedef SALOME::SenderDouble_ptr SenderPtrType;
  typedef SALOME::SenderDouble_var SenderVarType;
};

template < >
struct FIELDI_TRAITS<double, NoInterlace>
{
  typedef POA_SALOME_MED::FIELDDOUBLENO InterfaceForServant;
  typedef SALOME_MED::FIELDDOUBLENO_ptr FieldCorbaPtrType;
  typedef SALOME_MED::FIELDDOUBLE_ptr SimpleFieldCorbaPtrType;
  typedef SALOME_MED::FIELDDOUBLE SimpleFieldGlobalType;
  typedef SALOME_MED::double_array SeqType;
  typedef SALOME_MED::double_array_var SeqVarType;
  typedef SALOME::SenderDouble_ptr SenderPtrType;
  typedef SALOME::SenderDouble_var SenderVarType;
};

template < >
struct FIELDI_TRAITS<int, FullInterlace>
{
  typedef POA_SALOME_MED::FIELDINTFULL InterfaceForServant;
  typedef SALOME_MED::FIELDINTFULL_ptr FieldCorbaPtrType;
  typedef SALOME_MED::FIELDINT_ptr SimpleFieldCorbaPtrType;
  typedef SALOME_MED::FIELDINT SimpleFieldGlobalType;
  typedef SALOME_MED::long_array SeqType;
  typedef SALOME_MED::long_array_var SeqVarType;
  typedef SALOME::SenderInt_ptr SenderPtrType;
  typedef SALOME::SenderInt_var SenderVarType;
};

template < >
struct FIELDI_TRAITS<int, NoInterlace>
{
  typedef POA_SALOME_MED::FIELDINTNO InterfaceForServant;
  typedef SALOME_MED::FIELDINTNO_ptr FieldCorbaPtrType;
  typedef SALOME_MED::FIELDINT_ptr SimpleFieldCorbaPtrType;
  typedef SALOME_MED::FIELDINT SimpleFieldGlobalType;
  typedef SALOME_MED::long_array SeqType;
  typedef SALOME_MED::long_array_var SeqVarType;
  typedef SALOME::SenderInt_ptr SenderPtrType;
  typedef SALOME::SenderInt_var SenderVarType;
};
}

#endif
