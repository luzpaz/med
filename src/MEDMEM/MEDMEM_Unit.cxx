using namespace std;
/*
 File MEDMEM_Unit.cxx
 $Header$
*/

#include "MEDMEM_Unit.hxx"
using namespace MEDMEM;

UNIT::UNIT():_name(""),_description(""),_masse(0),_length(0),_time(0),_temperature(0),_matterQuantity(0),_currentStrength(0),_lightIntensity(0) {
  MESSAGE("UNIT()");
};

UNIT::UNIT(string Name, string Description):_name(Name),_description(Description),_masse(0),_length(0),_time(0),_temperature(0),_matterQuantity(0),_currentStrength(0),_lightIntensity(0) {
  MESSAGE("UNIT(string Name, string Description)");
};

UNIT::~UNIT() {
  MESSAGE("~UNIT()") ;
};

UNIT & UNIT::operator=(const UNIT &unit) {
  MESSAGE("UNIT::operateur=") ;
  _name=unit._name ;
  _description=unit._description ;
  _masse=unit._masse ; 
  _length=unit._length ; 
  _time=unit._time ; 
  _temperature=unit._temperature ; 
  _matterQuantity=unit._matterQuantity ; 
  _currentStrength=unit._currentStrength ; 
  _lightIntensity=unit._lightIntensity ; 
  return *this;
};
