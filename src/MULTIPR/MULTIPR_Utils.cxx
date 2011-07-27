//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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

// Partitioning/decimation module for the SALOME v3.2 platform
//
/**
 * \file    MULTIPR_Utils.cxx
 *
 * \brief   see MULTIPR_Utils.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Utils.hxx"
#include "MULTIPR_Exceptions.hxx"
#include "MULTIPR_Mesh.hxx"

#include <iostream>
#include <cstring>

using namespace std;

//*****************************************************************************
// Implementation
//*****************************************************************************

void multipr::trim(char* pStr, char pChar)
{
    int len = strlen(pStr) - 1;
    int p = len;
    while (pStr[p] == pChar)
    {
        p--;
    }
    if (p != len)
    {
        pStr[p + 1] = '\0';
    }
}


bool multipr::startsWith(const char* pStr, const char* pStrPrefix)
{
    int i = 0;
    while ((pStr[i] != '\0') && (pStrPrefix[i] != '\0') && (pStr[i] == pStrPrefix[i]))
    {
        i++;
    }
    return (pStrPrefix[i] == '\0');
}


string multipr::removeExtension(const char* pFilename, const char* pExtension)
{
    string strPrefix(pFilename); 
    strPrefix.erase(strPrefix.rfind(pExtension), strlen(pExtension));
    
    return strPrefix;
}


string multipr::getFilenameWithoutPath(const char* pFilename)
{
    const char* res = strrchr(pFilename, '/');
    if (res == NULL)
    {
        return pFilename;
    }
    else
    {
        string name = res + 1;
        return name;
    }
}


string multipr::getPath(const char* pFilename)
{
  // find (reverse) the first occurrence of '/' in the given string
  std::string aFilename(pFilename);
  std::string::size_type aPos = aFilename.rfind('/');

  if ( aPos == std::string::npos )
    return "";

  aFilename = aFilename.substr( 0, aPos + 1 );
  while ( true ) {
    aPos = aFilename.find( "//" );

    if ( aPos == std::string::npos )
      break;

    aFilename.replace( aPos, 2, "/" );
  }

  return aFilename;
}


void multipr::copyFile(const char* pFilename, const char* pDestDir)
{
    std::string srcDir = multipr::getPath(pFilename);
    if (strcmp(srcDir.c_str(), pDestDir) == 0) return;

    std::string filenameWithoutPath = multipr::getFilenameWithoutPath(pFilename);
    std::string pDstFilename (pDestDir);
    pDstFilename += filenameWithoutPath;

    // open binary source file
    FILE* src = fopen(pFilename, "rb");
    if (src == NULL) return;

    // open (create) binary destination file
    FILE* dst = fopen(pDstFilename.c_str(), "wb");
    if (dst == NULL) return;

    const int size = 65536; // size of buffer is 64Kb
    char* buf = new char[size];
    int ret;
    ret = fread(buf, 1, size, src);
    while (ret != 0)
    {
        fwrite(buf, 1, ret, dst); // write to destination
        ret = fread(buf, 1, size, src); // read from source
    }

    delete[] buf;
    fclose(src);
    fclose(dst);
}


void multipr::printArray2D(
     const med_float* pData, 
    const int        pNumElt,
    const int        pDimElt,
    const char*      pPrefix)
{
    for (int itElt = 0 ; itElt < pNumElt ; itElt++)
    {
        cout << pPrefix << " " << (itElt + 1) << ": ";
        for (int itDim = 0 ; itDim < pDimElt ; itDim++)
        {
            cout << pData[itElt * pDimElt + itDim] << " ";
        }
        cout << endl;
    }
}


string multipr::realToString(med_float mV)
{
    char str[32];
    sprintf(str, "%lf", mV);
    trim(str, '0');
    int len = strlen(str);
    if (str[len - 1] == '.')
    {
        str[len] = '0';
        str[len + 1] = '\0';
    }
    return string(str);
}


vector<string> multipr::getListMeshes(const char* pMEDfilename)
{
    if (pMEDfilename == NULL) throw multipr::NullArgumentException("", __FILE__, __LINE__);
    
    vector<string> res;
    med_err ret;
    
    //---------------------------------------------------------------------
    // Open MED file (READ_ONLY)
    //---------------------------------------------------------------------
    med_idt file = MEDouvrir(const_cast<char*>(pMEDfilename), MED_LECTURE); // open MED file for reading
    if (file <= 0) throw multipr::IOException("MED file not found or not a sequential MED file", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Read name of meshes
    //---------------------------------------------------------------------
    med_int nbMeshes = MEDnMaa(file);
    if (nbMeshes <= 0) throw multipr::IOException("i/o error while reading number of meshes in MED file", __FILE__, __LINE__);    
    
    // for each mesh in the file (warning: first mesh is number 1)
    for (int itMesh = 1 ; itMesh <= nbMeshes ; itMesh++)
    {
        char meshName[MED_TAILLE_NOM + 1];
        
        med_int meshDim;
        med_maillage meshType;
        char meshDesc[MED_TAILLE_DESC + 1]; 
        
        ret = MEDmaaInfo(
            file, 
            itMesh, 
            meshName, 
            &meshDim, 
            &meshType, 
            meshDesc);
            
        if (ret != 0) throw multipr::IOException("i/o error while reading mesh information in MED file", __FILE__, __LINE__);
        
        res.push_back(meshName);
    }
    
    //---------------------------------------------------------------------
    // Close the MED file
    //---------------------------------------------------------------------
    ret = MEDfermer(file);
    if (ret != 0) throw multipr::IOException("i/o error while closing MED file", __FILE__, __LINE__);
    
    return res;
}


void multipr::getListScalarFields(const char* pMEDfilename, vector<pair<string, int> >& pFields, bool pAddNbGaussPoint, const char *pMeshName)
{
    if (pMEDfilename == NULL) throw multipr::NullArgumentException("", __FILE__, __LINE__);
    med_err ret;
    med_int ngauss = 0;
    bool    isOnNodes = true;
    //---------------------------------------------------------------------
    // Open MED file (READ_ONLY)
    //---------------------------------------------------------------------
    med_idt file = MEDouvrir(const_cast<char*>(pMEDfilename), MED_LECTURE); // open MED file for reading
    if (file <= 0) throw multipr::IOException("MED file not found or not a sequential MED file", __FILE__, __LINE__);

    //---------------------------------------------------------------------
    // Read number of fields
    //---------------------------------------------------------------------
    med_int numFields = MEDnChamp(file, 0);
    if (numFields < 0) throw IOException("Can't read number of fields.", __FILE__, __LINE__);
    if (numFields == 0)
    {
        return ;
    }

    //---------------------------------------------------------------------
    // For each field, read its name
    //---------------------------------------------------------------------
    for (int itField = 1 ; itField <= numFields ; itField++)
    {
        char*           name = new char[MED_TAILLE_NOM + 1024];
        med_type_champ  type;
        med_int         numComponents = MEDnChamp(file, itField);
        
        if (numComponents < 0) throw IOException("Number of component less than zero.", __FILE__, __LINE__);
        
        // collect scalar field only (not vectorial fields)
        if (numComponents != 1)
        {
            continue;
        }
        // temporary buffers
        char* strComponent = new char[numComponents * MED_TAILLE_PNOM + 1];
        char* strUnit      = new char[numComponents * MED_TAILLE_PNOM + 1];
        strComponent[0] = '\0';
        strUnit[0] = '\0';
        
        // Get field info.
        med_err ret = MEDchampInfo(
            file, 
            itField, 
            name, 
            &(type), 
            strComponent, 
            strUnit, 
            numComponents);
        delete[] strUnit;
        delete[] strComponent;
        if (ret != 0) throw IOException("Can't get information on a field.", __FILE__, __LINE__);
        
        // Get number of time step on nodes.
        med_int numTimeStamps = MEDnPasdetemps(
            file, 
            name, 
            MED_NOEUD, 
            (med_geometrie_element) 0);
        if (numTimeStamps < 0) throw IOException("Can't get number of time steps on nodes", __FILE__, __LINE__);
        
        // Get the number of time step on elements.
        if (numTimeStamps == 0)
        {
            for (int i = 0; i < eMaxMedMesh; ++i)
            {
                numTimeStamps = MEDnPasdetemps(
                    file, 
                    name, 
                    MED_MAILLE, 
                    CELL_TYPES[i]);
                if (numTimeStamps > 0)
                {
                    isOnNodes = false;
                    break;
                }
            }
        }

        if (numTimeStamps  < 0) throw IOException("Can't get number of time steps on elements", __FILE__, __LINE__);    
        // For GUI only : add the nomber of gauss points to the string.
        if (pAddNbGaussPoint == true && pMeshName != NULL)
        {
            char*       tmp = new char[1024];
            char*       tmp2 = new char[1024];
            med_booleen local;
            med_int     numdt, numo, nmaa;
            med_float   dt;
            med_int     nb_values = -1;

            if (isOnNodes)
            {
                // Get number of gauss points on nodes.
                ret = MEDpasdetempsInfo(file, name, MED_NOEUD, (med_geometrie_element) 0, 1, &ngauss, &numdt, &numo, tmp2, &dt, tmp, &local, &nmaa);
                nb_values = MEDnVal(file, name, MED_NOEUD,(med_geometrie_element) 0, numdt, numo, const_cast<char*>(pMeshName), MED_COMPACT);
            }
            else
            {
                // Get number of gauss points on elements.
                // For each geometry type.
                for (int itCell = 0 ; itCell < eMaxMedMesh ; ++itCell)
                {
                    tmp[0] = '\0';
                    // For each time step.
                    for (int j = 1; j <= numTimeStamps; ++j)
                    {
                        ret = MEDpasdetempsInfo(file, name, MED_MAILLE, const_cast<med_geometrie_element*>(CELL_TYPES)[itCell], j, &ngauss, &numdt, &numo, tmp2, &dt, tmp, &local, &nmaa);
                        // If we can get info on this field, it must exist.
                        if (ret == 0)
                        {
                            nb_values = MEDnVal(file, name, MED_MAILLE,const_cast<med_geometrie_element*>(CELL_TYPES)[itCell], numdt, numo, const_cast<char*>(pMeshName), MED_COMPACT);
                            if (nb_values > 0)
                            {
                                break;
                            }
                        }
                    }
                    // We got what we need : don't need to go further.
                    if (ret == 0)
                    {
                        break;
                    }
                    
                }
            }
            tmp[0] = '\0';
            sprintf(tmp, "%s : %d gauss points", name, nb_values);
            sprintf(name, "%s", tmp);
        }

        // add the pair(name, #time stamps) to the result
        pFields.push_back(make_pair(name, numTimeStamps));
    }
        
    //---------------------------------------------------------------------
    // Close the MED file
    //---------------------------------------------------------------------
    ret = MEDfermer(file);
    if (ret != 0) throw multipr::IOException("i/o error while closing MED file", __FILE__, __LINE__);
}


// EOF
