#include "FIELDClient.hxx"

using namespace MEDMEM;

//=============================================================================
/*!
 *   Transfert les valeurs du "Champ Corba" dans le "Champ local"
 *   (version FIELD<double>)
 */
//=============================================================================
void FIELDClient_value(FIELD<double> *F, const SALOME_MED::FIELD_ptr IOR_Field)
{
  BEGIN_OF("FIELDClient_value(double)");

  F->setValueType(MED_REEL64);

  const SALOME_MED::FIELDDOUBLE_var IOR_FieldDouble 
    = SALOME_MED::FIELDDOUBLE::_narrow(IOR_Field);
  SCRUTE(IOR_FieldDouble);

  SALOME_MED::double_array_var v_corba 
    = IOR_FieldDouble->getValue(MED_FULL_INTERLACE);

  long i, n = v_corba->length();
  SCRUTE(n);
  double *v = new double[n];

  for (i=0; i<n; i++) v[i] = v_corba[i];
  MEDARRAY<double> * M = new MEDARRAY<double> 
    (v, F->getNumberOfComponents(),F->getNumberOfValues(), MED_FULL_INTERLACE);
  F->setValue(M); 

  END_OF("FIELDClient_value(double)");
}

//=============================================================================
/*!
 *   Transfert les valeurs du "Champ Corba" dans le "Champ local"
 *   (version FIELD<int>)
 */
//=============================================================================
void FIELDClient_value(FIELD<int> *F, const SALOME_MED::FIELD_ptr IOR_Field)
{
  BEGIN_OF("FIELDClient_value(int)");

  F->setValueType(MED_INT32);

  const SALOME_MED::FIELDINT_var IOR_FieldInt 
    = SALOME_MED::FIELDINT::_narrow(IOR_Field);

  SALOME_MED::long_array_var v_corba 
    = IOR_FieldInt->getValue(MED_FULL_INTERLACE);

  long i, n = v_corba->length();
  SCRUTE(n);
  int *v = new int[n];
  for (i=0; i<n; i++) v[i] = v_corba[i];

  MEDARRAY<int> * M = new MEDARRAY<int> 
    (v, F->getNumberOfComponents(),F->getNumberOfValues(), MED_FULL_INTERLACE);
  F->setValue(M); 
 
  END_OF("FIELDClient_value(int)");
}
