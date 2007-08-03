
#ifndef _ICOCOMEDFIELD_HXX_
#define _ICOCOMEDFIELD_HXX_

#include <vector>
#include "ICoCoField.hxx"
namespace ParaMEDMEM
{
	class ParaMESH;
	class ParaFIELD;
	class ProcessorGroup;
}
namespace MEDMEM
{
	class CONNECTZONE;
}
namespace ICoCo
{
	class TrioField;
	
  class MEDField:public Field
    {
    public:
      
      MEDField(){};
      MEDField(ParaMEDMEM::ParaMESH* mesh, ParaMEDMEM::ParaFIELD* field): _mesh(mesh), _field(field){};
			MEDField(const TrioField&, const ParaMEDMEM::ProcessorGroup& group);
      ParaMEDMEM::ParaFIELD* getField() const  {return _field;}
      ParaMEDMEM::ParaMESH* getMesh()const {return _mesh;}

    private:
      ParaMEDMEM::ParaMESH* _mesh;
      ParaMEDMEM::ParaFIELD* _field;
      std::vector<MEDMEM::CONNECTZONE*> _connectzones;
    };
};

#endif
