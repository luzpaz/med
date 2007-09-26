#ifndef _ICOCOFIELD_HXX_
#define  _ICOCOFIELD_HXX_

#include <string>
namespace ICoCo
{
class Field
  {
  public:
     Field(){};
    virtual ~Field(){};
    void setName(const std::string& name) {_name=name;}
    std::string getName() {return _name;}
   
  private:
    std::string _name;
  };
}
#endif
