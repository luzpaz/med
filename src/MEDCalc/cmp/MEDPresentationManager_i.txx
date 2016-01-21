// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#ifndef _MED_PRESENTATION_MANAGER_I_TPP_
#define _MED_PRESENTATION_MANAGER_I_TPP_

template<typename PresentationType, typename PresentationParameters>
MEDPresentation::TypeID
MEDPresentationManager_i::_makePresentation(PresentationParameters params)
{
  // Create a new presentation instance
  PresentationType* presentation = NULL;
  try {
    presentation = new PresentationType(params);  // on stack or on heap?? stack for now
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  MEDPresentation::TypeID newID = MEDPresentationManager_i::GenerateID();
  _presentations.insert( std::pair<MEDPresentation::TypeID, MEDPresentation *>(newID, presentation) );
  presentation->generatePipeline();
  return newID;
}

#endif // _MED_PRESENTATION_MANAGER_I_TPP_
