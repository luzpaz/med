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
