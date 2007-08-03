#ifndef _3D_VERTEX_HXX_
#define _3D_VERTEX_HXX_

namespace MEDMEM
{
  struct Vertex_Less_3D
  {
    //double * x;
    bool operator()(const double * p1,const double * p2) 
    {
      return   (*p1 < *p2)
	||
	((*p1 == *p2)&&(*(p1+1) < *(p2+1)))
	||
	((*p1 == *p2)&&(*(p1+1) == *(p2+1))&&(*(p1+2) < *(p2+2)));
    }
  };
  
  //  bool operator()(const double * p1,const double * p2)

};
#endif
