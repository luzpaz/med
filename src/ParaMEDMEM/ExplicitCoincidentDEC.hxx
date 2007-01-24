#ifndef ExplicitCOINCIDENTDEC_HXX_
#define ExplicitCOINCIDENTDEC_HXX_

#include "DEC.hxx"
#include "ExplicitTopology.hxx"
#include <map>

namespace ParaMEDMEM
{
class DEC;
class BlockTopology;
class ExplicitCoincidentDEC: public DEC
{
public:
	ExplicitCoincidentDEC();
	virtual ~ExplicitCoincidentDEC();
	void synchronize();
	void broadcastTopology(BlockTopology*&, int tag);
	void broadcastTopology(const ExplicitTopology& toposend, ExplicitTopology& toporecv, int tag);
	
	void prepareSourceDE();
	void prepareTargetDE();
	void recvData();
	void sendData();
private :
	
	ExplicitTopology* _toposource;
	ExplicitTopology* _topotarget;
	int* _sendcounts;
	int* _recvcounts;
	int* _senddispls;
	int* _recvdispls;
	double* _recvbuffer;
	double* _sendbuffer;
	std::map<int,std::pair<int,int> > _distant_elems;
};

}

#endif /*ExplicitCOINCIDENTDEC_HXX_*/
	
