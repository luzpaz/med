//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
//  Partitioning/decimation module for the SALOME v3.2 platform
//
/**
 * \file    MULTIPR_Utils.hxx
 *
 * \brief   Some useful miscellaneous tools.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_UTILS_HXX
#define MULTIPR_UTILS_HXX

extern "C"
{
    #include "med.h"
}

#include <string>
#include <vector>


namespace multipr
{


/**
 * Removes all the pChar at the end of the string.
 * \param  pStr  any valid C string ending with the char '\0'.
 * \param  pChar any char; SPACE by default.
 * \return the same string where the ending spaces have been removed.
 */
void trim(char* pStr, char pChar=' ');


/**
 * Returns true iff the given string starts by prefix.
 * \param  pStr       any valid C string ending with the char '\0'.
 * \param  pStrPrefix any valid C string ending with the char '\0'.
 * \return true iff the given string starts by prefix.
 */
bool startsWith(const char* pStr, const char* pStrPrefix);

 
/**
 * Removes the extension (suffix) of a filename.
 * Example: removeExtension("agregat100grains_12pas.med", ".med") -> "agregat100grains_12pas"
 * \param  pPilename  any valid C string ending with the char '\0'.
 * \param  pExtension any valid C string ending with the char '\0'.
 * \return the filename without extension.
 */
std::string removeExtension(const char* pFilename, const char* pExtension);


/**
 * Returns the name of the file, excluding the path.
 * Example: getFilenameWithoutPath("/home/data/agregat100grains_12pas.med") -> "agregat100grains_12pas.med"
 * \param  pPilename  any valid C string ending with the char '\0'.
 * \return the name of the file, excluding the path.
 */
std::string getFilenameWithoutPath(const char* pFilename);


/**
 * Returns the path of the filename (which may be relative or absolute).
 * Example: getPath("/home/data/agregat100grains_12pas.med") -> "/home/data/"
 * \param  pPilename  any valid C string ending with the char '\0'.
 * \return the path of the filename (which may be relative or absolute).
 */
std::string getPath(const char* pFilename);


/**
 * Copies the given file into a directory.
 * \param  pFilename  name of any file to be copied.
 * \param  pDestDir   destination directory.
 */
void copyFile(const char* pFilename, const char* pDestDir);
 
 
/**
 * Prints all the elements of a 2D array.
 * \param  pData   all the data (should contain pNumberOfElements * pDimOfElements values)
 * \param  pNumElt number of elements to display.
 * \param  pDimElt Dimension of elements.
 * \param  pPrefix string to display before each element.
 */
void printArray2D(
    const med_float* pData, 
    const int        pNumElt,
    const int        pDimElt,
    const char*      pPrefix);
    

/**
 * Converts any float value to a string (remove unecessary 0).
 * \param  mV any float value.
 */
std::string realToString(med_float mV);


/**
 * Returns the name of all meshes contained in a sequential MED file.
 * \param  pMEDfilename name of any valid sequential MED file; must not be NULL.
 * \return a list of mesh names.
 * \throw  NullArgumentException if pMEDfilename is NULL.
 * \throw  IOException if any other error occurs while reading MED file.
 */
 std::vector<std::string> getListMeshes(const char* pMEDfilename);

 
 /**
  * For each scalar field in a sequential MED file, returns its name and the related number of time stamps.
  * Do not returns info about vectorial fields (because, currently, decimation can only be performed on scalar fields).
  * \param  pMEDfilename name of any valid sequential MED file; must not be NULL.
  * \param  pFields The information on the fields.
  * \param  pAddNbGaussPoint If set to true, the number of gauss point of each field is added.
  * \param  pMeshName Mesh name used if pAddNbGaussPoint is true.
  * \throw  NullArgumentException if pMEDfilename is NULL.
  * \throw  IOException if any other error occurs while reading MED file.
  */
 void	getListScalarFields(const char* pMEDfilename, std::vector<std::pair<std::string, int> >& pFields, bool pAddNbGaussPoint = false, const char *pMeshName = NULL);

} // namespace MULTIPR


#endif // MULTIPR_UTILS_HXX

// EOF
