/*
 File CellModel.hxx
 $Header$
*/

#ifndef CELLMODEL_HXX
#define CELLMODEL_HXX

#include <vector>
#include <set>
#include <map>
#include <string>

#include "MEDMEM_define.hxx"

using namespace MED_EN;

class CELLMODEL
{

protected :

  string _name ;
  medGeometryElement _type ;
  int _dimension ;     // Cell _dimension (!= space _dimension)
  int _numberOfNodes ;
  int _numberOfVertexes ;
  vector< vector< vector<int> > > _constituents ; 
		       // define local connectivity for each constituents 
		       // composing it ( 
		       // first vector : for each cell _dimension 
                       // (first : dim-1, second if any : dim-2)
		       // second vector : for each constituents of this _dimension,
		       // third vector : list of local nodes
  vector< vector<medGeometryElement> > _constituentsType ;

public :

  CELLMODEL(){
    _type=MED_NONE ;
    _dimension = 0 ;
    _numberOfNodes = 0 ;
    _numberOfVertexes = 0 ;
  }
  CELLMODEL(medGeometryElement t) ;
  CELLMODEL(const CELLMODEL &m){
    _name= m._name ;
    _type= m._type ;
    _dimension = m._dimension ;
    _numberOfNodes = m._numberOfNodes ;
    _numberOfVertexes = m._numberOfVertexes ;
    _constituents = m._constituents ;
    _constituentsType = m._constituentsType ;
  }
  ~CELLMODEL() {
  };

  CELLMODEL & operator=(const CELLMODEL &m) {
    _name=m._name ;
    _type= m._type ;
    _dimension = m._dimension ;
    _numberOfNodes = m._numberOfNodes ;
    _numberOfVertexes = m._numberOfVertexes ;
    _constituents = m._constituents ;
    _constituentsType = m._constituentsType ;
    return *this ;
  }
  friend ostream & operator<<(ostream &os,const CELLMODEL &my);

  inline string                 getName() const ;
  inline int 			getNumberOfVertexes() const;
  inline int 			getNumberOfNodes() const;
  inline int 			getDimension() const;
  inline medGeometryElement  getType() const;

  // Return all constituents which dimension is _dimension-dim.
  vector< vector<int> > getConstituents(int dim) const; 

  // Return number of constituents which dimension is _dimension-dim.
  int getNumberOfConstituents(int dim) const;

  // Return local nodes numbers vector for num-th constituent which dimension is _dimension-dim.
  vector<int> getNodesConstituent(int dim,int num) const; 

  // Return local node number of nodes_index-th node for num-th constituent which dimension is _dimension-dim.
  int getNodeConstituent(int dim,int num,int nodes_index); 

  // Return types of each constituents which dimension is _dimension-dim.
  vector <medGeometryElement>  getConstituentsType(int dim) const; 

  // Return type of num-th constituent which dimension is _dimension-dim.
  medGeometryElement getConstituentType(int dim,int num) const;


  // Return number of constituents type (which dimension is _dimension-1).
  int getNumberOfConstituentsType() const;

  // Return all types of constituents which dimension is (_dimension-1).
  set <medGeometryElement>  getAllConstituentsType() const; 

  // Return number of constituents foreach type (which dimension is _dimension-1).
  map <medGeometryElement,int>  getNumberOfConstituentsForeachType() const; 


};

// ------------------------------------------
//	Methodes Inline
// ------------------------------------------

inline string CELLMODEL::getName() const
{
	return _name ;
}
inline int CELLMODEL::getNumberOfVertexes() const
{
	return _numberOfVertexes;
}
inline int CELLMODEL::getNumberOfNodes() const
{
	return _numberOfNodes;
}
inline int CELLMODEL::getDimension() const
{
	return _dimension;
}
inline medGeometryElement CELLMODEL::getType() const
{
	return _type;
}
inline vector< vector<int> > CELLMODEL::getConstituents(int dim) const
{
  return _constituents[dim-1] ;
}
inline int CELLMODEL::getNumberOfConstituents(int dim) const
{
  return _constituents[dim-1].size() ;
}
inline vector<int> CELLMODEL::getNodesConstituent(int dim,int num) const
{
  return _constituents[dim-1][num-1];
}
inline int CELLMODEL::getNodeConstituent(int dim,int num,int nodesNumber)
{
  return _constituents[dim-1][num-1][nodesNumber-1] ;
}
inline vector<medGeometryElement> CELLMODEL::getConstituentsType(int dim) const
{
  return _constituentsType[dim-1];
}
inline medGeometryElement CELLMODEL::getConstituentType(int dim,int num) const
{
  return _constituentsType[dim-1][num-1];
}

#endif /* CELLMODEL_HXX */
