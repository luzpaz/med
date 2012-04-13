// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File      : MEDSPLITTER_MeshSendReceive.hxx
// Created   : Tue Jun 30 19:14:51 2009
// Author    : Edward AGAPOV (eap)


#ifndef __MEDSPLITTER_MeshSendReceive_HXX__
#define __MEDSPLITTER_MeshSendReceive_HXX__

#include "MEDSPLITTER.hxx"

#ifdef HAVE_MPI2
#include <mpi.h>
#else
typedef int MPI_Request;
#endif

#include <vector>

namespace MEDMEM
{
  class MESH;
}

namespace MEDSPLITTER
{
  /*!
   * \brief Sender-receiver of the mesh.
   *
   * We also send-receive
   * 1) global node ids for the sake of fusing domain parts
   * 2) global cell ids for the sake of detecting cell/cell joints using global graph
   * 3) global face ids for the sake of not losing face sharing among domains
   */
  class MEDSPLITTER_EXPORT MeshSendReceive
  {
  public:

    MeshSendReceive();
    ~MeshSendReceive();

    // Sends the mesh of idomain to irank processor asynchronously
    // WARNING: do NOT delete the mesh until this->isSent()!
    void send(int irank, int idomain, MEDMEM::MESH* mesh,
              const std::vector<int>& cell_glob_nums,
              const std::vector<int>& face_glob_nums,
              const std::vector<int>& node_glob_nums);

    // Receives the mesh of idomain from irank processor synchronously
    MEDMEM::MESH* recv(int irank, int idomain,
                       std::vector<int>& cell_glob_numbers,
                       std::vector<int>& face_glob_numbers,
                       std::vector<int>& node_glob_numbers);

     //!< Returns the received or sent mesh. The user is resposible for freeing the received mesh.
    MEDMEM::MESH* getMesh() { return _mesh; }

    // Returns true if mesh is already sent
    bool isSent();

    // Frees buffers waiting until isSent()
    void clear();

  private:

    std::vector< int > _int_buf;
    std::vector< int > _node_glob_numbers, _cell_glob_numbers, _face_glob_numbers;
    std::vector< char> _char_buf;

    MEDMEM::MESH*      _mesh;

    MPI_Request        _int_request, _coord_request, _char_request;
    MPI_Request        _node_nums_request, _cell_nums_request, _face_nums_request;
  };
}


#endif
