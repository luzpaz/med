#ifndef __COMPO2
#define __COMPO2

#ifndef SWIG
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Field.hxx"
#endif

class Compo2
{

  FIELD<double> * _F;

public:
  Compo2();
  ~Compo2();

  std::string Calcul(const MESH &M);
  const FAMILY * Calcul2(const MESH &M);
  void Calcul3(const FIELD<double> &S);
};

#endif
