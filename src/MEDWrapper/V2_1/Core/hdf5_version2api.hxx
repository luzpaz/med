
#ifndef _hdf5_version2api_hxx_
#define _hdf5_version2api_hxx_
// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//

#include <H5public.h>

#if H5_VERS_MAJOR < 1
#elif H5_VERS_MAJOR == 1

#if H5_VERS_MINOR < 6
#elif H5_VERS_MINOR == 6

#if H5_VERS_RELEASE < 4
#else
#if H5_VERS_RELEASE >= 7
#define HDF_NEW_API2
#define HDF_NEW_API
#else
#define HDF_NEW_API
#endif
#endif

#else /* H5_VERS_MINOR >= 7 */
#define HDF_NEW_API
#endif /* H5_VERS_MINOR */

#else /* H5_VERS_MAJOR >= 2 */
#define HDF_NEW_API
#endif /* H5_VERS_MAJOR */

#endif
