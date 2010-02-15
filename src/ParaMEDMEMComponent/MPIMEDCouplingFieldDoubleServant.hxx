//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __MPIMEDCOUPLINGFIELDDOUBLESERVANT_HXX__
#define __MPIMEDCOUPLINGFIELDDOUBLESERVANT_HXX__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(ParaMEDMEMComponent)
#include "ParaMEDMEMComponent_i.hxx"
#include "MPIObject_i.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"

void * th_getdatabympi(void *st);

namespace ParaMEDMEM
{
  class MPIMEDCouplingFieldDoubleServant : public POA_SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface,
                                           public MEDCouplingFieldDoubleServant,
                                           public MPIObject_i
  {
  public:
    MPIMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb,ParaMEDMEMComponent_i *pcompo,MEDCouplingFieldDouble* field);
    void getDataByMPI(const char* coupling) throw(SALOME::SALOME_Exception);
    void Register();
    void Destroy();
  private:
    ParaMEDMEMComponent_i *_pcompo;
    MEDCouplingFieldDouble* _field;
  };
}
#endif
