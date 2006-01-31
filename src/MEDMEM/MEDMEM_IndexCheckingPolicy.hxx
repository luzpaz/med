#ifndef MEDMEM_INDEX_CHECKING_POLICY_HXX
#define MEDMEM_INDEX_CHECKING_POLICY_HXX 

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_STRING.hxx"

namespace MEDMEM {

class IndexCheckPolicy {
public :
  inline void checkMoreThanZero(const std::string & classname, int index) const {
    if (index <= 0)
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", index : " << index << " is less or equal to zero"));
  }
  inline void checkLessOrEqualThan(const std::string & classname, int max, int index) const {
    if (index > max)
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", index : " << index << " is more than " << max ));
  }

  inline void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const {
    if (index < min || index > max )
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", index : " << index 
					   << " not in rang [" << min << "," << max <<"]"));
  }

  inline void checkEquality(const std::string & classname, int a, int b) const {
    if ( a == b )
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", Value shouldn't be : " << a ));
  }

};

class NoIndexCheckPolicy {
public :
  inline void checkMoreThanZero(const string &classname, int index) const {}
  inline void checkLessOrEqualThan(const std::string & classname, int max, int index) const {}
  inline void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const {}
  inline void checkEquality(const std::string & classname, int a, int b) const {}
};

}// END NAMESPACE
#endif
