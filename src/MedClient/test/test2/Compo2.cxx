#ifndef _DEBUG_
#define _DEBUG_
#endif

#include "Compo2.hxx"
#include <utilities.h>
#include <typeinfo>
#include <iostream>

using namespace MEDMEM;
using namespace MED_EN;

Compo2::Compo2() : _F(NULL)
{
}

Compo2::~Compo2()
{
  if (_F) delete _F;
}

std::string Compo2::Calcul(const MESH &M)
{
  BEGIN_OF("std::string Compo2::Calcul(MESH &M)");

  MESSAGE("type virtuel : MESH");
  MESSAGE("type reel : " << typeid(M).name());

  std::string name = M.getName();

  name += " recu";

  try {
    std::cerr << M << std::endl;
  }
  catch (...) {
    std::cerr << "erreur Compo2::Calcul" << std::endl;
  }

  END_OF("std::string Compo2::Calcul(MESH &M)");
  return name;
}

const FAMILY * Compo2::Calcul2(const MESH &M)
{
  BEGIN_OF("const FAMILY * Compo2::Calcul2(const MESH &M)");

  const FAMILY * F = M.getFamily(MED_CELL, 1);
  std::cerr << "ok ici 2" << std::endl;
  try {
    std::cout << F << std::endl;
  }
  catch (...) {
    std::cerr << "erreur Compo2::Calcul2" << std::endl;
  }

  END_OF("const FAMILY * Compo2::Calcul2(const MESH &M)");
  return F;
}

void Compo2::Calcul3(const FIELD<double> &F)
{
  BEGIN_OF("void Compo2::Calcul3(const FIELD<double> *)");

  const double *v = F.getValue(MED_FULL_INTERLACE);
  long i, n = F.getSupport()->getNumberOfElements(MED_CELL);
  int j, m = F.getNumberOfComponents();

  for (i=0; i<n; i++) {
    std::cout << i << " : ";
    for (j=0; j<m; j++)
      std::cout << v[j + i*m];
    std::cout << std::endl;
  }
  
  END_OF("void Compo2::Calcul2(const FIELD<double> *)");
}
