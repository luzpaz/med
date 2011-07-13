// Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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

#ifndef __MEDCOUPLINGFIELDOVERTIMECLIENT_HXX__
#define __MEDCOUPLINGFIELDOVERTIMECLIENT_HXX__

#include "SALOMEconfig.h"
#ifdef WNT
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingClient.hxx"
#include "MEDCouplingFieldOverTime.hxx"

namespace ParaMEDMEM
{
  class MEDCouplingDefinitionTime;

  class MEDCouplingFieldOverTimeClient : public MEDCouplingFieldOverTime
  {
  public:
    MEDCOUPLINGCLIENT_EXPORT static MEDCouplingFieldOverTimeClient *New(SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr field);
    std::vector<double> getTimeSteps() const;
  private:
    void fetchDefTimeIfNeeded() const;
    void retrieveFieldTemplates();
    MEDCouplingFieldOverTimeClient(SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr field);
    ~MEDCouplingFieldOverTimeClient();
  private:
    SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_var _field_ptr;
    mutable MEDCouplingDefinitionTime *_df;
  };
}

#endif
