#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"
#include "MxN_Mapping.hxx"

namespace ParaMEDMEM
{

MxN_Mapping::MxN_Mapping(const ProcessorGroup& local_group, const ProcessorGroup& distant_group)
: _union_group(local_group.fuse(distant_group))
{
  _send_proc_offsets.resize(_union_group->size()+1,0);
  _recv_proc_offsets.resize(_union_group->size()+1,0);
  
}

MxN_Mapping::~MxN_Mapping()
{
  delete _union_group;
}

void MxN_Mapping::addElementFromSource(int local_element, int distant_proc, int distant_element)
{
  _sending_ids.push_back(make_pair(distant_proc,distant_element));
 // _local_ids.push_back(local_element);
  for (int i=distant_proc; i<_union_group->size(); i++)
    _send_proc_offsets[i+1]++;
}

void MxN_Mapping::prepareSendRecv()
{
  CommInterface comm_interface=_union_group->getCommInterface();
  // sending count pattern
  int* nbsend=new int[_union_group->size()];
  int* nbrecv=new int[_union_group->size()];
  for (int i=0; i<_union_group->size(); i++)
  {
    nbsend[i]=_send_proc_offsets[i+1]-_send_proc_offsets[i];
  }
  
  MPIProcessorGroup* group = static_cast<MPIProcessorGroup*>(_union_group);
  const MPI_Comm* comm=group->getComm();
  comm_interface.allToAll(nbsend, 1, MPI_INT,
                           nbrecv, 1, MPI_INT,
                           *comm);
         
  for (int i=0; i<_union_group->size(); i++)
  {
    for (int j=i+1;j<_union_group->size()+1; j++)
       _recv_proc_offsets[j]+=nbrecv[i];
    
  } 

  delete[] nbsend;
  delete[] nbrecv;

  _recv_ids.resize(_recv_proc_offsets[_union_group->size()]);
  int* isendbuf=0;
  int* irecvbuf=0;
  if (_sending_ids.size()>0)
    isendbuf = new int[_sending_ids.size()];
  if (_recv_ids.size()>0)  
    irecvbuf = new int[_recv_ids.size()];
  int* sendcounts = new int[_union_group->size()];
  int* senddispls=new int[_union_group->size()];
  int* recvcounts=new int[_union_group->size()];
  int* recvdispls=new int[_union_group->size()];
  for (int i=0; i< _union_group->size(); i++)
  {
    sendcounts[i]=_send_proc_offsets[i+1]-_send_proc_offsets[i];
    senddispls[i]=_send_proc_offsets[i];
    recvcounts[i]=_recv_proc_offsets[i+1]-_recv_proc_offsets[i];
    recvdispls[i]=_recv_proc_offsets[i];
  }
  vector<int> offsets = _send_proc_offsets;
  for (int i=0; i<_sending_ids.size();i++)
  {
    int iproc = _sending_ids[i].first;
    isendbuf[offsets[iproc]]=_sending_ids[i].second;
    offsets[iproc]++;
  }
  comm_interface.allToAllV(isendbuf, sendcounts, senddispls, MPI_INT,
                           irecvbuf, recvcounts, recvdispls, MPI_INT,
                           *comm);
                           
  for (int i=0; i< _recv_proc_offsets[_union_group->size()]; i++)
    _recv_ids[i]=irecvbuf[i];                           
 
  if (_sending_ids.size()>0)
    delete[] isendbuf;
  if (_recv_ids.size()>0)  
    delete[] irecvbuf;
  delete[] sendcounts;
  delete[]recvcounts;
  delete[]senddispls;
  delete[] recvdispls;
}


/*! Exchanging field data between two groups of processes
 * 
 * \param field MEDMEM field containing the values to be sent
 * 
 * The ids that were defined by addElementFromSource method
 * are sent.
 */ 
void MxN_Mapping::sendRecv(MEDMEM::FIELD<double>& field)
{
  CommInterface comm_interface=_union_group->getCommInterface();
  const MPIProcessorGroup* group = static_cast<const MPIProcessorGroup*>(_union_group);
 
  int nbcomp=field.getNumberOfComponents();
  double* sendbuf=0;
  double* recvbuf=0;
  if (_sending_ids.size() >0)
    sendbuf = new double[_sending_ids.size()*nbcomp];
  if (_recv_ids.size()>0)
    recvbuf = new double[_recv_ids.size()*nbcomp];
    
  int* sendcounts = new int[_union_group->size()];
  int* senddispls=new int[_union_group->size()];
  int* recvcounts=new int[_union_group->size()];
  int* recvdispls=new int[_union_group->size()];
  
  for (int i=0; i< _union_group->size(); i++)
  {
    sendcounts[i]=nbcomp*(_send_proc_offsets[i+1]-_send_proc_offsets[i]);
    senddispls[i]=nbcomp*(_send_proc_offsets[i]);
    recvcounts[i]=nbcomp*(_recv_proc_offsets[i+1]-_recv_proc_offsets[i]);
    recvdispls[i]=nbcomp*(_recv_proc_offsets[i]);
  }
  //building the buffer of the elements to be sent
  vector<int> offsets = _send_proc_offsets;
  for (int i=0; i<_sending_ids.size();i++)
  { 
    int iproc = _sending_ids[i].first;
    for (int icomp=0; icomp<nbcomp; icomp++)
      sendbuf[offsets[iproc]*nbcomp+icomp]=field.getValueIJ(i+1, icomp+1);
    offsets[iproc]++;
  }
  
  //communication phase
  const MPI_Comm* comm = group->getComm();
  comm_interface.allToAllV(sendbuf, sendcounts, senddispls, MPI_INT,
                           recvbuf, recvcounts, recvdispls, MPI_INT,
                           *comm);
             
  
  //setting the received values in the field
  double* recvptr;                         
  for (int i=0; i< _recv_proc_offsets[_union_group->size()]; i++)
  {
    for (int icomp=0; icomp<nbcomp; icomp++){
      field.setValueIJ(_recv_ids[i],icomp+1,*recvptr);
      recvptr++;
    }  
  }       
  
  if (sendbuf!=0) delete[] sendbuf;
  if (recvbuf!=0) delete[] recvbuf;
  delete[] sendcounts;
  delete[] recvcounts;
  delete[] senddispls; 
  delete[] recvdispls;
  
}
/*! Exchanging field data between two groups of processes
 * 
 * \param field MEDMEM field containing the values to be sent
 * 
 * The ids that were defined by addElementFromSource method
 * are sent.
 */ 
void MxN_Mapping::sendRecv(double* sendfield, MEDMEM::FIELD<double>& field) const 
{
  CommInterface comm_interface=_union_group->getCommInterface();
  const MPIProcessorGroup* group = static_cast<const MPIProcessorGroup*>(_union_group);
 
  int nbcomp=field.getNumberOfComponents();
  double* sendbuf=0;
  double* recvbuf=0;
  if (_sending_ids.size() >0)
    sendbuf = new double[_sending_ids.size()*nbcomp];
  if (_recv_ids.size()>0)
    recvbuf = new double[_recv_ids.size()*nbcomp];
    
  int* sendcounts = new int[_union_group->size()];
  int* senddispls=new int[_union_group->size()];
  int* recvcounts=new int[_union_group->size()];
  int* recvdispls=new int[_union_group->size()];
  
  for (int i=0; i< _union_group->size(); i++)
  {
    sendcounts[i]=nbcomp*(_send_proc_offsets[i+1]-_send_proc_offsets[i]);
    senddispls[i]=nbcomp*(_send_proc_offsets[i]);
    recvcounts[i]=nbcomp*(_recv_proc_offsets[i+1]-_recv_proc_offsets[i]);
    recvdispls[i]=nbcomp*(_recv_proc_offsets[i]);
  }
  //building the buffer of the elements to be sent
  vector<int> offsets = _send_proc_offsets;
  for (int i=0; i<_sending_ids.size();i++)
  { 
    int iproc = _sending_ids[i].first;
    for (int icomp=0; icomp<nbcomp; icomp++)
      //      sendbuf[offsets[iproc]*nbcomp+icomp]=sendfield[i+1*nbcomp+icomp];
      sendbuf[offsets[iproc]*nbcomp+icomp]=sendfield[i*nbcomp+icomp];
    offsets[iproc]++;
  }
  
  //communication phase
  const MPI_Comm* comm = group->getComm();
  comm_interface.allToAllV(sendbuf, sendcounts, senddispls, MPI_DOUBLE,
                           recvbuf, recvcounts, recvdispls, MPI_DOUBLE,
                           *comm);
             
  
  //setting zero values in the field
//   for (int i=0; i< field.getSupport()->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);i++)
//     for (int j=0; j<field.getNumberOfComponents(); j++)
//       field.setValueIJ(i+1,j+1,0.0);
  
  //setting the received values in the field
  double* recvptr=recvbuf;                         
  for (int i=0; i< _recv_proc_offsets[_union_group->size()]; i++)
  {
    for (int icomp=0; icomp<nbcomp; icomp++){
      double temp = field.getValueIJ(_recv_ids[i],icomp+1);
      field.setValueIJ(_recv_ids[i],icomp+1,temp+*recvptr);
      recvptr++;
    }  
  }       
  
  if (sendbuf!=0) delete[] sendbuf;
  if (recvbuf!=0) delete[] recvbuf;
  delete[] sendcounts;
  delete[] recvcounts;
  delete[] senddispls; 
  delete[] recvdispls;
  
}


}
