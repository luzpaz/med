%module libCompo2

%{
#include "Compo2.hxx"

  using namespace MEDMEM;
  using namespace MED_EN;
%}

%include "std_string.i"
%include "libMEDClient.i"

%include "Compo2.hxx"
