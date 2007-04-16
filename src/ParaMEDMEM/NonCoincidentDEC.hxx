#ifndef NONCOINCIDENTDEC_HXX_
#define NONCOINCIDENTDEC_HXX_

#include <fvm_locator.h>

typedef enum {NN} InterpolationMethod;

namespace ParaMEDMEM
{
  class DEC;
    
  class NonCoincidentDEC:public DEC
  {
    public:  
    NonCoincidentDEC();

    virtual ~NonCoincidentDEC();

    void synchronize();

    void recvData();

    void sendData();
    
    void prepareSourceDE(){};
    void prepareTargetDE(){};
    
    void setInterpolationMethod(InterpolationMethod method)
    {_method=method;}
    
    private :
    // Structure for computing the localization
    // of remote nodes on local mesh
    fvm_locator_t* _locator;
    
    //Number of distant points to be located locally 
    int _nb_distant_points;
    
    //coordinates of distant points 
    const double* _distant_coords;
    
    //local element number containing the distant points  
    const int* _distant_locations; 
   
   //inerpolation method
   InterpolationMethod _method;
  };
}


#endif
