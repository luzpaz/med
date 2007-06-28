#include <iostream> 
#include <vector>
#include <algorithm>

const int MIN_NB_ELEMS =10;
const int MAX_LEVEL=10;
using namespace std;

template <int dim> class BBTree
{

private:
  BBTree* _left;
  BBTree* _right;
  BBTree* _center;
  int _level;
  double _median;
  double* _bb;
  vector <int> _elems;
  bool _terminal;

public:


BBTree(double* bbs, int* elems, int level, int nbelems):
  _left(0), _right(0), _center(0), _level(level), _median(0.0), _bb(bbs), _terminal(false)
{
  if (nbelems < MIN_NB_ELEMS || level> MAX_LEVEL)
    {
      _terminal=true;
		
    }
  vector<double> nodes (nbelems*2);
  for (int i=0; i<nbelems; i++)
    {
      int elem;
      if (elems!=0)
	elem= elems[i];
      else
	elem=i;

      _elems.push_back(elem);
      nodes[i*2]=bbs[elem*dim*2+(level%dim)*2];
      nodes[i*2+1]=bbs[elem*dim*2+(level%dim)*2+1];
    }
  if (_terminal) return;

	//elem list is not useful if the node is not terminal
	_elems.clear();

  vector<double>::iterator median = nodes.begin()+nbelems;
   nth_element< vector<double>::iterator>(nodes.begin(), nodes.begin()+nbelems, nodes.end());
   _median=*median;
   // std:: cout << *median <<std::endl;

  vector<int> new_elems_left;
  vector<int> new_elems_right;
  vector<int> new_elems_center;
  for (int i=0; i<nbelems;i++)
    {
      int elem;
      if (elems!=0)
	elem= elems[i];
      else
	elem=i;
      double max=bbs[elem*dim*2+(level%dim)*2+1];
      if (max < *median)
	{
	  new_elems_left.push_back(elem);
	  continue;
	}
      double min = bbs[elem*dim*2+(level%dim)*2];
      if (min>*median)
	{
	  new_elems_right.push_back(elem);
	  continue;
	}
      new_elems_center.push_back(elem);
    }

  _left=new BBTree(bbs, &(new_elems_left[0]), level+1, new_elems_left.size());
  _right=new BBTree(bbs, &(new_elems_right[0]), level+1, new_elems_right.size());
  _center=new BBTree(bbs, &(new_elems_center[0]), level+1, new_elems_center.size());
 
  
}

~BBTree()
{
  if (_left!=0)  delete _left;
  if (_right!=0) delete _right;
  if (_center!=0) delete _center;
}

void getIntersectingElems(double* bb, vector<int>& elems)
{
  //  terminal node : return list of elements intersecting bb
  if (_terminal)
    {
      for (int i=0; i<_elems.size(); i++)
	{
	  double* bb_ptr=_bb+_elems[i]*2*dim;
	  bool intersects = true;
	  for (int idim=0; idim<dim; idim++)
	    {
	      if (bb_ptr[idim*2]>bb[idim*2+1] || bb_ptr[idim*2+1]<bb[idim*2])
		intersects=false;
	    }
	  if (intersects)
	    elems.push_back(_elems[i]);
	}
      return;
    }

  //non terminal node 
  double min = bb[(_level%dim)*2];
  double max = bb[(_level%dim)*2+1];
  if (max < _median)
    {
      _left->getIntersectingElems(bb, elems);
			_center->getIntersectingElems(bb,elems);
      return;
    }
  if (min > _median)
    {
      _right->getIntersectingElems(bb,elems);
			_center->getIntersectingElems(bb,elems);
      return;
    }
  _left->getIntersectingElems(bb,elems);
  _center->getIntersectingElems(bb,elems);
  _right->getIntersectingElems(bb,elems);
}
};
