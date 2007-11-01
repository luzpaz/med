#ifndef MEDSPLITTER_FACEMODEL_HXX_
#define MEDSPLITTER_FACEMODEL_HXX_

namespace MEDSPLITTER
{

class MEDSPLITTER_FaceModel
{
public:
	MEDSPLITTER_FaceModel():_conn(0){};
	virtual ~MEDSPLITTER_FaceModel() {if (_conn!=0) delete[] _conn;}
  MED_EN::medGeometryElement getType() const {return _type;}
  void setType(MED_EN::medGeometryElement type) {_type=type;}
  void setNbNodes(int nbnodes){_conn=new int[nbnodes];}
  int& operator[](int i){return *(_conn+i);}
  int getGlobal(){return _global;}
  void setGlobal(int i){_global=i;}
  
  private:
  int _nbnodes;
  int* _conn;
  MED_EN::medGeometryElement _type;
  int _global;
};

}

#endif /*MEDSPLITTER_FACEMODEL_HXX_*/
