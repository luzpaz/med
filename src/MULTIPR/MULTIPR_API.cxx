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
// Partitioning/decimation module for the SALOME v3.2 platform
//
/**
 * \file    MULTIPR_API.cxx
 *
 * \brief   see MULTIPR_API.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_API.hxx"
#include "MULTIPR_Globals.hxx"
#include "MULTIPR_Mesh.hxx"
#include "MULTIPR_MeshDis.hxx"
#include "MULTIPR_Utils.hxx"
#include "MULTIPR_Exceptions.hxx"

extern "C"
{
    #include "med.h"
}

#include "MEDSPLITTER_API.hxx"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


//*****************************************************************************
// Implementation
//*****************************************************************************

const char* multipr::getVersion()
{
    return "2.0.1";
}


void multipr::partitionneDomaine(const char* pMEDfilename, const char* pMeshName)
{    
    if (pMEDfilename == NULL) throw NullArgumentException("pMEDfilename should not be NULL", __FILE__, __LINE__);
    if (pMeshName == NULL)    throw NullArgumentException("pMeshName should not be NULL", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Read the sequential mesh
    //---------------------------------------------------------------------
    cout << "Read sequential MED file: " <<
    pMEDfilename << ": please wait... " << endl;
    
    multipr::Mesh mesh;
    mesh.readSequentialMED(pMEDfilename, pMeshName, false);
    std::vector<Group*>* lGroups = mesh.getGroups();
    cout << mesh << endl;
    
    //---------------------------------------------------------------------
    // Build distributed mesh from groups
    //---------------------------------------------------------------------
    cout << "Build distributed mesh: please wait... " << endl;
    
    multipr::MeshDis* meshDis = NULL;
    
    try
    {
        GaussIndexList* lGaussList;
        meshDis = mesh.splitGroupsOfElements();
        lGaussList = mesh.editGaussIndex();
        std::vector<Profil*>& lProfils = mesh.getProfils();
        //-------------------------------------------------------------
        // Write distributed mesh
        //-------------------------------------------------------------
        cout << "Write distributed mesh: please wait... " << endl;
        string strPrefix = removeExtension(pMEDfilename, ".med");    
        meshDis->writeDistributedMED(strPrefix.c_str());
        meshDis->readAndWriteFields(pMeshName, lGroups, lGaussList, lProfils);
        // need to delete some garb
        lGaussList->clear();
        delete meshDis;
    }
    catch (RuntimeException& e)
    {
        delete meshDis;
        throw e;
    }
}


void multipr::partitionneGroupe(
    const char* pMEDfilename, 
    const char* pGroupName, 
    int         pNbParts, 
    int         pPartitionner)
{
    if (pMEDfilename == NULL) throw NullArgumentException("pMEDfilename should not be NULL", __FILE__, __LINE__);
    if (pGroupName == NULL)   throw NullArgumentException("pGroupName should not be NULL", __FILE__, __LINE__);
    if (pNbParts < 2)         throw IllegalArgumentException("pNbParts should be >= 2", __FILE__, __LINE__);
    if ((pPartitionner != MULTIPR_METIS) && (pPartitionner != MULTIPR_SCOTCH)) throw NullArgumentException("pPartitionner should be METIS (0) or SCOTCH (1)", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Read the distributed mesh
    //---------------------------------------------------------------------
    MULTIPR_LOG("Read distributed MED file: " << pMEDfilename << ": please wait... " << endl);
    
    int ret = MEDformatConforme(pMEDfilename);
    if (ret == 0) throw IOException("waiting for a distributed MED file (not a sequential one)", __FILE__, __LINE__);

    multipr::MeshDis meshDis;
    meshDis.readDistributedMED(pMEDfilename);
    cout << meshDis << endl;
    
    //---------------------------------------------------------------------
    // Split the given part (pGroupName)
    //---------------------------------------------------------------------
    if (pPartitionner == MULTIPR_METIS)
    {
        cout << "Use METIS" << endl;
    }
    else if (pPartitionner == MULTIPR_SCOTCH)
    {
        cout << "Use SCOTCH" << endl;
    }
    
    meshDis.splitPart(pGroupName, pNbParts, pPartitionner);
    cout << meshDis << endl;
    
    //---------------------------------------------------------------------
    // Write new distributed mesh
    //---------------------------------------------------------------------
    string strPrefix = multipr::removeExtension(pMEDfilename, ".med");
    meshDis.writeDistributedMED(strPrefix.c_str());
}


void multipr::decimePartition(
    const char* pMEDfilename,
    const char* pPartName,
    const char* pFieldName,
    int         pFieldIt,
    const char* pFilterName,
    const char* pFilterParams)
{
    //---------------------------------------------------------------------
    // Check arguments
    //---------------------------------------------------------------------
    if (pMEDfilename == NULL) throw NullArgumentException("pMEDfilename should not be NULL", __FILE__, __LINE__);
    if (pPartName == NULL) throw NullArgumentException("pPartName should not be NULL", __FILE__, __LINE__);
    if (pFieldName == NULL) throw NullArgumentException("pFieldName should not be NULL", __FILE__, __LINE__);
    if (pFieldIt < 1) throw IllegalArgumentException("pFieldIt: invalid field iteration; should be >= 1", __FILE__, __LINE__);
    /*
        if (pTMed < 0.0) throw IllegalArgumentException("med res.: threshold must be > 0", __FILE__, __LINE__);
    if (pTMed >= pTLow) throw IllegalArgumentException("threshold for med res. must be < threshold for low res.", __FILE__, __LINE__);
    if (pRadius <= 0.0) throw IllegalArgumentException("radius should be > 0", __FILE__, __LINE__);
    if ((pBoxing < 1) || (pBoxing > 200)) throw IllegalArgumentException("boxing should be in [1..200]", __FILE__, __LINE__);
        */
    
    //cout << "--decim file=" << pMEDfilename << " part=" << pPartName << " filter=" << pFilterName << " tmed=" << pTMed << " tlow=" << pTLow << " radius=" << pRadius << endl;
    
    //---------------------------------------------------------------------
    // Read the distributed mesh
    //---------------------------------------------------------------------
    MULTIPR_LOG("Read distributed MED file: " << pMEDfilename << ": please wait... " << endl);

    int ret = MEDformatConforme(pMEDfilename);
    if (ret == 0) throw IOException("waiting for a distributed MED file (not a sequential one)", __FILE__, __LINE__);
    
    multipr::MeshDis meshDis;
    meshDis.readDistributedMED(pMEDfilename);
    cout << meshDis << endl;
    
    //---------------------------------------------------------------------
    // Create 3 resolutions of the given part
    //---------------------------------------------------------------------
    meshDis.decimatePart(
        pPartName, 
        pFieldName,
        pFieldIt,
        pFilterName,
        pFilterParams);
    cout << meshDis << endl;
    
    //---------------------------------------------------------------------
    // Write new distributed mesh
    //---------------------------------------------------------------------
    string strPrefix = removeExtension(pMEDfilename, ".med"); // get prefix from the original MED filename    
    meshDis.writeDistributedMED(strPrefix.c_str());
}


int multipr::merge(
    std::vector<std::string> pMEDFilenameSrc,
    const char*              pMeshName,
    const char*              pFieldName,
    const char*              pMEDFilenameDst)
{
    if (pMEDFilenameSrc.size() < 2) throw IllegalArgumentException("list must contain two files at least", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("pMeshName should not be NULL", __FILE__, __LINE__);
    if (pMEDFilenameDst == NULL)  throw NullArgumentException("pMEDFilenameDst should not be NULL", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Read sources
    //---------------------------------------------------------------------
    std::vector<multipr::Mesh*> meshes;
    for (unsigned i = 0 ; i < pMEDFilenameSrc.size() ; i++)
    {
        multipr::Mesh* mesh = new multipr::Mesh();
        mesh->readSequentialMED(pMEDFilenameSrc[i].c_str(), 1);

        if (mesh->getNumberOfElements() != 0)
        {
            meshes.push_back(mesh);
        }
        else
        {
            delete mesh;
        }
    }
    
    // if no mesh (all meshes are empty) => return
    if (meshes.size() == 0) return 0;
    
    // if only one non-empty mesh
    if (meshes.size() == 1)
    {
        multipr::Mesh* mesh = meshes[0];
        mesh->writeMED(pMEDFilenameDst);        
        delete mesh;
        return 1;
    }
        
    //---------------------------------------------------------------------
    // Merge sources and write resulting mesh to disk
    //---------------------------------------------------------------------    
    multipr::Mesh* meshFirst = meshes.back();
    meshes.pop_back();
    multipr::Mesh* meshMerged = meshFirst->mergePartial(meshes, pFieldName, -1);
    //meshMerged->setPrintAll(true);
    //cout << (*meshMerged) << endl;
    
    meshMerged->writeMED(pMEDFilenameDst, pMeshName);
    
    delete meshMerged;
    delete meshFirst;
    for (unsigned i = 0 ; i < meshes.size(); i++)
    {
        delete meshes[i];
    }
    return 1;
}

    
// EOF
