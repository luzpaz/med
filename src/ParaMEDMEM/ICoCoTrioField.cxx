//////////////////////////////////////////////////////////////////////////////
//
// File:        ICoCoTrioField.cpp
// Directory:   $TRIO_U_ROOT/Kernel/Framework
// Version:     
//
//////////////////////////////////////////////////////////////////////////////

#include <ICoCoTrioField.hxx>
#include <string.h>
#include <iostream>

using namespace ICoCo;
using namespace std;

TrioField::TrioField() :
  _type(0),
  _connectivity(0),
  _coords(0),
  _field(0),
  _has_field_ownership(false) { }

TrioField::~TrioField() {
  clear();
}

// After the call to clear(), all pointers are null and field ownership is false.
// Arrays are deleted if necessary
void TrioField::clear() {
  if (_connectivity)
    delete[] _connectivity;
  if (_coords)
    delete[] _coords;
  if (_field && _has_field_ownership)
    delete[] _field;
  _connectivity=0;
  _coords=0;
  _field=0;
  _has_field_ownership=false;
}

int TrioField::nb_values() const
{
  if (_type==0)
    return _nb_elems;
  else   
    if (_type==1)
      return _nbnodes;
  exit(-1);
  return -1;
  
}
void TrioField::save(ostream& os) const{
  os <<getName()<<endl;
  os<<_type<<endl;
  os<<_mesh_dim<<endl;
  os<< _space_dim<<endl;
  os<< _nbnodes<<endl;
  os<< _nodes_per_elem<<endl;
  os<< _nb_elems<<endl;
    
  os<< _itnumber<<endl;
  for (int i=0;i<_nb_elems;i++) 
    {
      for (int j=0;j<_nodes_per_elem;j++)
        os << " "<<_connectivity[i*_nodes_per_elem+j];
      os<<endl;
    }
  
  
  for (int i=0;i<_nbnodes;i++) 
    {
      for (int j=0;j<_space_dim;j++)
        os <<" "<< _coords[i*_space_dim+j] ;
      os  << endl;
    }
  
  os<< _time1<<endl;
  os<<_time2<<endl;
  os<< _nb_field_components<<endl;
  if (_field)
    {
      os <<1<<endl;
      for (int i=0;i<nb_values();i++) 
        {
          for (int j=0;j<_nb_field_components;j++)
            os <<" "<< _field[i*_nb_field_components+j];
          os << endl;
        }
    }
  else
    {
      os<<0<<endl;
    }
  os<< _has_field_ownership<<endl;
}



void TrioField::restore(istream& in) {
  string name;
  in >> name; 
  setName(name);
  in>>_type;
  in>>_mesh_dim;
  in>> _space_dim;
  in>> _nbnodes;
  in>> _nodes_per_elem;
  in>> _nb_elems;
    
  in>> _itnumber;
  if (_connectivity)
    delete [] _connectivity;
  _connectivity=new int[_nodes_per_elem*_nb_elems];
  for (int i=0;i<_nb_elems;i++) 
    {
      for (int j=0;j<_nodes_per_elem;j++)
        in>>_connectivity[i*_nodes_per_elem+j];
      
    }
  if (_coords)
    delete [] _coords;
  _coords=new double[_nbnodes*_space_dim];
  for (int i=0;i<_nbnodes;i++) 
    {
      for (int j=0;j<_space_dim;j++)
        in>> _coords[i*_space_dim+j] ;
     
    }
  
  in>> _time1;
  in>>_time2;
  in>> _nb_field_components;
  int test;
  in>> test;
  if (test)
    {
      if (_field)
        delete [] _field;
      _field=new double[_nb_field_components*nb_values()];
      for (int i=0;i<nb_values();i++) 
        {
          for (int j=0;j<_nb_field_components;j++)
            in>> _field[i*_nb_field_components+j];
         
        }
    }
  else
    {
      _field=0;
    }
  in>> _has_field_ownership;
}
   void TrioField::print() {

  }

// After the call to set_standalone(), field ownership is true and field is allocated
// to the size _nb_field_components*_nb_elems.
// The values of the field have been copied if necessary.
void TrioField::set_standalone() {
  if (!_field) {
    _field=new double[_nb_field_components*_nb_elems];
    _has_field_ownership=true;
    
  }
  else if (!_has_field_ownership) {
    double *tmp_field=new double[_nb_field_components*_nb_elems];
    memcpy(tmp_field,_field,_nb_field_components*_nb_elems*sizeof(double));
    _field=tmp_field;
    _has_field_ownership=true;
  }
}

// Used to simulate a 0D geometry (Cathare/Trio for example).
void TrioField::dummy_geom() {
  _mesh_dim=2;
  _space_dim=2;
  _nbnodes=3;
  _nodes_per_elem=3;
  _nb_elems=1;
  _itnumber=0;
  if (_connectivity)
    delete[] _connectivity;
  _connectivity=new int[3];
  _connectivity[0]=1;
  _connectivity[1]=2;
  _connectivity[2]=3;
  if (_coords)
    delete[] _coords;
  _coords=new double[6];
  _coords[0]=0;
  _coords[1]=0;
  _coords[2]=1;
  _coords[3]=0;
  _coords[4]=0;
  _coords[5]=1;
  if (_field && _has_field_ownership)
    delete[] _field;
  _has_field_ownership=false;
  _field=0;
}

// Surcharge de l'operateur = pour la classe TrioField
// remplace les valeurs trouvee dans le TrioField donnee en parametre
TrioField& TrioField::operator=(const TrioField& NewField){

    clear();
    _type=NewField._type;
    _mesh_dim=NewField._mesh_dim;
    _space_dim=NewField._space_dim;
    _nbnodes=NewField._nbnodes;
    _nodes_per_elem=NewField._nodes_per_elem;
    _nb_elems=NewField._nb_elems;
    _itnumber=NewField._itnumber;
      // std::string _name; // ?? H�rit� de la classe m�re
    _time1=NewField._time1;
    _time2=NewField._time2;
    _nb_field_components=NewField._nb_field_components;

    if (!NewField._connectivity)
        _connectivity=0;
    else {
      _connectivity=new int[_nodes_per_elem*_nb_elems];
      memcpy( _connectivity,NewField._connectivity,_nodes_per_elem*_nb_elems*sizeof(int));
    }

    if (!NewField._coords)
        _coords=0;
    else {
       _coords=new double[_nbnodes*_space_dim];
       memcpy( _coords,NewField._coords,_nbnodes*_space_dim*sizeof(double));
    }

    //Copie des valeurs du champ
    _has_field_ownership=NewField._has_field_ownership;
    if (_has_field_ownership) {
      _field=new double[_nb_elems*_nb_field_components];
      memcpy(_field,NewField._field,_nb_elems*_nb_field_components*sizeof(double));
    }
    else
      _field=NewField._field;

    return(*this);

}

