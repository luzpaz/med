// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

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

#include <iostream>

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
    char* res = strrchr(pFilename, '/');
    if (res == NULL)
    {
        return pFilename;
    }
    else
    {
        char* name = res + 1;
        return name;
    }
}


string multipr::getPath(const char* pFilename)
{
    // find (reverse) the first occurrence of '/' in the given string
    char* res = strrchr(pFilename, '/');
    
    // if there is no '/'...
    if (res == NULL)
    {
        return "";
    }
    else
    {
        int size = res - pFilename + 1;
        char path[256];
        memcpy(path, pFilename, size);
        path[size] = '\0';
        return path;
    }
}


void multipr::copyFile(const char* pFilename, const char* pDestDir)
{
    const char* srcDir = multipr::getPath(pFilename).c_str();
    if (strcmp(srcDir, pDestDir) == 0) return;
    
    const char* filenameWithoutPath = multipr::getFilenameWithoutPath(pFilename).c_str();
    char pDstFilename[256];
    sprintf(pDstFilename, "%s%s", pDestDir, filenameWithoutPath);
    
    // open binary source file
    FILE* src = fopen(pFilename, "rb");
    if (src == NULL) return;
    
    // open (create) binary destination file
    FILE* dst = fopen(pDstFilename, "wb");
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
        
        int meshDim;
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


vector<pair<string,int> > multipr::getListScalarFields(const char* pMEDfilename)
{
    if (pMEDfilename == NULL) throw multipr::NullArgumentException("", __FILE__, __LINE__);
    
    vector<pair<string, int> > res;
    med_err ret;
    
    //---------------------------------------------------------------------
    // Open MED file (READ_ONLY)
    //---------------------------------------------------------------------
    med_idt file = MEDouvrir(const_cast<char*>(pMEDfilename), MED_LECTURE); // open MED file for reading
    if (file <= 0) throw multipr::IOException("MED file not found or not a sequential MED file", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Read number of fields
    //---------------------------------------------------------------------
    med_int numFields = MEDnChamp(file, 0);
    if (numFields <= 0) throw IOException("", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // For each field, read its name
    //---------------------------------------------------------------------
    for (int itField = 1 ; itField <= numFields ; itField++)
    {
        char           name[MED_TAILLE_NOM + 1];
        med_type_champ type;
        
        med_int numComponents = MEDnChamp(file, itField);
    
        if (numComponents < 0) throw IOException("", __FILE__, __LINE__);
        
        // collect scalar field only (not vectorial fields)
        if (numComponents != 1) 
            continue;
        
        // temporary buffers
        char* strComponent = new char[numComponents * MED_TAILLE_PNOM + 1];
        char* strUnit      = new char[numComponents * MED_TAILLE_PNOM + 1];
        
        strComponent[0] = '\0';
        strUnit[0] = '\0';
        
        med_err ret = MEDchampInfo(
            file, 
            itField, 
            name, 
            &(type), 
            strComponent, 
            strUnit, 
            numComponents);
        
        if (ret != 0) throw IOException("", __FILE__, __LINE__);
        
        delete[] strUnit;
        delete[] strComponent;
        
        med_int numTimeStamps = MEDnPasdetemps(
            file, 
            name, 
            MED_NOEUD, 
            (med_geometrie_element) 0);
        
        if (numTimeStamps < 0) throw IOException("", __FILE__, __LINE__);
        
        if (numTimeStamps == 0)
        {
            numTimeStamps = MEDnPasdetemps(
                file, 
                name, 
                MED_MAILLE, 
                MED_TETRA10);
        
            if (numTimeStamps  < 0) throw IOException("", __FILE__, __LINE__);    
            
        }
        
        // add the pair(name, #time stamps) to the result
        res.push_back( make_pair(name, numTimeStamps) );
    }
    
    //---------------------------------------------------------------------
    // Close the MED file
    //---------------------------------------------------------------------
    ret = MEDfermer(file);
    if (ret != 0) throw multipr::IOException("i/o error while closing MED file", __FILE__, __LINE__);
    
    // return the list of fields
    return res;
}


// EOF
