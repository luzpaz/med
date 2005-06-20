#ifndef __MEDMEM_RCBASE_HXX__
#define __MEDMEM_RCBASE_HXX__

namespace MEDMEM {
  
  class RCBASE {
  public:
    virtual void addReference() const = 0;
    virtual void removeReference() const = 0;
  };

}

#endif
