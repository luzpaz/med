#ifndef __COMPO2
#define __COMPO2

#ifndef SWIG
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Field.hxx"
#endif

class Compo2
{

  MEDMEM::FIELD<double> * _F;

public:
  Compo2();
  ~Compo2();

  std::string Calcul(const MEDMEM::MESH &M);
  const MEDMEM::FAMILY * Calcul2(const MEDMEM::MESH &M);
  void Calcul3(const MEDMEM::FIELD<double> &S);
};

#endif
