#include "Topology.hxx"
#include "ComponentTopology.hxx"
#include "ParaMESH.hxx"
#include "ParaFIELD.hxx"
#include "MEDMEM_ConnectZone.hxx"
#include <string>
namespace ICoCo
{
  class TrioField:public Field
    {
    public:
      
      TrioField();

    public:
			int _mesh_dim;
			int _space_dim;
			int _nbnodes;
			int _nodes_per_elem;
			int _nb_elems;
			int _itnumber;
			std::string _name;
			double _time;
			int* _connectivity;
			double* _coords;
			int _nb_field_components;
			double* _field;
    };
};
