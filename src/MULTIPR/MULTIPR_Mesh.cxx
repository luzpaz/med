//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
 * \file    MULTIPR_Mesh.cxx
 *
 * \brief   see MULTIPR_Mesh.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Mesh.hxx"
#include "MULTIPR_Nodes.hxx"
#include "MULTIPR_Elements.hxx"
#include "MULTIPR_Family.hxx"
#include "MULTIPR_Profil.hxx"
#include "MULTIPR_GaussLoc.hxx"
#include "MULTIPR_Field.hxx"
#include "MULTIPR_MeshDis.hxx"
#include "MULTIPR_PointOfField.hxx"
#include "MULTIPR_DecimationFilter.hxx"
#include "MULTIPR_Utils.hxx"
#include "MULTIPR_Exceptions.hxx"
#include "MULTIPR_Globals.hxx"
#include "MULTIPR_API.hxx"
#include <stdio.h>
#include <cmath>

using namespace std;

namespace multipr
{

const med_geometrie_element CELL_TYPES[MED_NBR_GEOMETRIE_MAILLE] = 
{
    MED_POINT1,
    MED_SEG2, 
    MED_SEG3,
    MED_TRIA3,
    MED_TRIA6,
    MED_QUAD4,
    MED_QUAD8,
    MED_TETRA4,
    MED_TETRA10,
    MED_HEXA8,
    MED_HEXA20,
    MED_PENTA6,
    MED_PENTA15,
    MED_PYRA5,
    MED_PYRA13
};

   
char CELL_NAMES[MED_NBR_GEOMETRIE_MAILLE][MED_TAILLE_NOM + 1] =  
{
    "MED_POINT1",
    "MED_SEG2", 
    "MED_SEG3",
    "MED_TRIA3",
    "MED_TRIA6",
    "MED_QUAD4",
    "MED_QUAD8",
    "MED_TETRA4",
    "MED_TETRA10",
    "MED_HEXA8",
    "MED_HEXA20",
    "MED_PENTA6",
    "MED_PENTA15",
    "MED_PYRA5",
    "MED_PYRA13"
};

// Number of points to consider when looking for significant nodes in a mesh.
// ie the n first nodes.
const int CELL_NB_NODE[MED_NBR_GEOMETRIE_MAILLE] =
{
    1,  //MED_POINT1
    2,  //MED_SEG2
    2,  //MED_SEG3
    3,  //MED_TRIA3
    3,  //MED_TRIA6
    4,  //MED_QUAD4
    4,  //MED_QUAD8
    4,  //MED_TETRA4
    4,  //MED_TETRA10
    8,  //MED_HEXA8
    8,  //MED_HEXA20
    6,  //MED_PENTA6
    6,  //MED_PENTA15
    5,  //MED_PYRA5
    5   //MED_PYRA13
};



//*****************************************************************************
// Class Mesh implementation
//*****************************************************************************

Mesh::Mesh()
{
    mNodes    = NULL;
    for (int i = 0; i < eMaxMedMesh; i++)
    {
        mElements[i] = NULL;
    }

    reset();
}


Mesh::~Mesh()
{
    reset();
}


void Mesh::reset()
{
    mMEDfilename[0] = '\0';
    mMEDfile        = 0;
    
    mMeshName[0]    = '\0';
    mMeshUName[0]   = '\0';
    mMeshDesc[0]    = '\0';
    mMeshDim        = -1;
    mMeshType       = MED_NON_STRUCTURE;
    
    for (int itDim = 0 ; itDim < 3 ; itDim++) 
    { 
        mMeshBBoxMin[itDim] = numeric_limits<med_float>::quiet_NaN();
        mMeshBBoxMax[itDim] = numeric_limits<med_float>::quiet_NaN();
    }
    
    if (mNodes != NULL)    { delete mNodes;    mNodes = NULL; }
    
    for (int i = 0; i < eMaxMedMesh; i++)
    {
        if (mElements[i] != NULL)
        {
            delete mElements[i];
            mElements[i] = NULL;
        }
    }
    for (unsigned itFam = 0 ; itFam < mFamilies.size() ; itFam++)
    {
        delete mFamilies[itFam];
    }  
    mFamilies.clear();
    mFamIdToFam.clear();
    
    for (unsigned itGroup = 0 ; itGroup < mGroups.size() ; itGroup++)
    {
        delete mGroups[itGroup];
    }  
    mGroups.clear();
    mGroupNameToGroup.clear();
    
    for (unsigned itGaussLoc = 0 ; itGaussLoc < mGaussLoc.size() ; itGaussLoc++)
    {
        delete mGaussLoc[itGaussLoc];
    }  
    mGaussLoc.clear();
    mGaussLocNameToGaussLoc.clear();
    
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        //delete mFields[itField];
    }  
    mFields.clear();
    
    for (unsigned itProfil = 0 ; itProfil < mProfils.size() ; itProfil++)
    {
        delete mProfils[itProfil];
    }  
    mProfils.clear();
    
    mFlagPrintAll = false;
    
    mGaussIndex.clear();
}


vector<string> Mesh::getNameScalarFields() const
{
    vector<string> res;
    
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        
        // only get scalar fields, not vectorial fields
        // (because, currently, decimation can only be performed on scalar fields)
        if (currentField->getNumberOfComponents() == 1)
        {
            res.push_back(currentField->getName());
        }
    }
    
    return res;
}


int Mesh::getTimeStamps(const char* pFieldName) const
{
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        if (strcmp(currentField->getName(), pFieldName) == 0)
        {
            return currentField->getNumberOfTimeSteps();
        }
    }
    
    return 0;
}

Field* Mesh::getFieldByName(const char* pFieldName, eMeshType pGeomType) const
{
    if (pFieldName == NULL) throw NullArgumentException("pFieldName should not be NULL", __FILE__, __LINE__);
    
    Field* retField = NULL;
    
    // for each field
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        // Check if this is the field we want.
        if (strncmp(pFieldName, currentField->getName(), MED_TAILLE_NOM) == 0 && 
            (currentField->getGeomIdx() == pGeomType || currentField->isFieldOnNodes()))
        {
            // field found!
            retField = currentField;
            break;
        }
    }
    
    return retField;
}

void Mesh::getFieldMinMax(const char* pFieldName, float& pMin, float& pMax) const
{
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        // Check if this is the field we want.
        if (strncmp(pFieldName, currentField->getName(), MED_TAILLE_NOM) == 0)
        {
            currentField->getMinMax(pMin, pMax);
            break;
        }
    }
}

GaussLoc* Mesh::getGaussLocByName(const char* pGaussLocName) const
{
    if (pGaussLocName == NULL) throw NullArgumentException("pGaussLocName should not be NULL", __FILE__, __LINE__);
    
    map<string, GaussLoc*>::const_iterator itGaussLoc = mGaussLocNameToGaussLoc.find(pGaussLocName);
    GaussLoc* retGaussLoc = NULL;
    
    if (itGaussLoc != mGaussLocNameToGaussLoc.end())
    {
        retGaussLoc = (*itGaussLoc).second;
    }
    
    return retGaussLoc;
}


int Mesh::getNumberOfElements(eMeshType pGeomType) const
{
    if (mElements[pGeomType])
    {
        return mElements[pGeomType]->getNumberOfElements();
    }
    else
    {
        return 0;
    }
}

int Mesh::getNumberOfElements() const
{
    int accum = 0;

    for (int i = 0; i < eMaxMedMesh; ++i)
    {
        if (mElements[i])
        {
            accum += mElements[i]->getNumberOfElements();
        }
    }
    return accum;
}

Profil* Mesh::getProfil(const std::string pProfilName)
{
    Profil* profile = NULL;
    std::vector<Profil*>::iterator it;
    if (pProfilName.size() > 0)
    {
        for (it = mProfils.begin(); it != mProfils.end(); ++it)
        {
            if ((*it)->getName() == pProfilName)
            {
                profile = (*it);
                break;
            }
        }
    }
    return profile;
}

Mesh* Mesh::createFromSetOfElements(const std::set<med_int>* pSetOfElements, const char* pNewMeshName)
{
    if (pNewMeshName == NULL) throw NullArgumentException("pNewMeshName should not be NULL", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Create a new mesh
    //---------------------------------------------------------------------
    Mesh* mesh = new Mesh();
    
    //---------------------------------------------------------------------
    // Build name of the new mesh
    //---------------------------------------------------------------------    
    strcpy(mesh->mMeshName, pNewMeshName);
    
    MULTIPR_LOG("Mesh name=|" << mesh->mMeshName << "|" << endl);
    
    //---------------------------------------------------------------------
    // Fill general infos
    //---------------------------------------------------------------------
    strcpy(mesh->mMeshUName, mMeshUName);
    strcpy(mesh->mMeshDesc, mMeshDesc);
    
    mesh->mMeshDim = mMeshDim;
    mesh->mMeshType = mMeshType;
    
    MULTIPR_LOG("Mesh u. name=|" << mesh->mMeshUName << "|" << endl);
    MULTIPR_LOG("Mesh desc=|" << mesh->mMeshDesc << "|" << endl);
    MULTIPR_LOG("Mesh dim=" << mesh->mMeshDim << endl);
    MULTIPR_LOG("Mesh Type=" << mesh->mMeshType << endl);
    
    //---------------------------------------------------------------------
    // Build nodes and elements
    //---------------------------------------------------------------------
    // get all elements involved
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (pSetOfElements[i].size() != 0)
                {
                mesh->mElements[i] = mElements[i]->extractSubSet(pSetOfElements[i]);
                MULTIPR_LOG((*(mesh->mElements[i])) << endl);
                }
        }
    
    // get all nodes involved
        set<med_int> setOfNodes;
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mElements[i] != NULL && mesh->mElements[i] != NULL)
                {
                const set<med_int>& curSetOfNodes = mesh->mElements[i]->getSetOfNodes();
                        setOfNodes.insert(curSetOfNodes.begin(), curSetOfNodes.end());
                }
        }
    mesh->mNodes = mNodes->extractSubSet(setOfNodes);
    MULTIPR_LOG((*(mesh->mNodes)) << endl);
    
    //---------------------------------------------------------------------
    // Remap nodes
    //---------------------------------------------------------------------
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mElements[i] != NULL && mesh->mElements[i] != NULL)
                {
                        mesh->mElements[i]->remap(setOfNodes);
                        MULTIPR_LOG((*(mesh->mElements[i])) << endl);
                }
        }
    

    //---------------------------------------------------------------------
    // Build families
    //---------------------------------------------------------------------
    MULTIPR_LOG("Build fam.:" << endl);
    // get families of nodes
    {
        set<med_int> famOfNodes = mesh->mNodes->getSetOfFamilies();
        for (set<med_int>::iterator itFam = famOfNodes.begin() ; itFam != famOfNodes.end() ; itFam++)
        {
            Family* famSrc = mFamIdToFam[*itFam];
            if (mesh->mFamIdToFam.find(famSrc->getId()) == mesh->mFamIdToFam.end())
            {
                Family* famDest = famSrc->extractGroup(NULL);
                mesh->mFamilies.push_back(famDest);
                mesh->mFamIdToFam[famDest->getId()] = famDest;              
            }
        }
    }
    
    // get families of elements
    {
        set<med_int> famOfElt;
                for (int i = 0; i < eMaxMedMesh; ++i)
                {
                        if (mElements[i] != NULL && mesh->mElements[i] != NULL)
                        {
                                set<med_int> curSetOfFamilies = mesh->mElements[i]->getSetOfFamilies();
                                famOfElt.insert(curSetOfFamilies.begin(), curSetOfFamilies.end());
                        }
                }
        for (set<med_int>::iterator itFam = famOfElt.begin() ; itFam != famOfElt.end() ; itFam++)
        {
            Family* famSrc = mFamIdToFam[*itFam];
            if (mesh->mFamIdToFam.find(famSrc->getId()) == mesh->mFamIdToFam.end())
            {
                Family* famDest = famSrc->extractGroup(NULL);
                mesh->mFamilies.push_back(famDest);
                mesh->mFamIdToFam[famDest->getId()] = famDest;
            }
        }
    }
    
    MULTIPR_LOG("Finalize:");
    
    // fill families with elements and build groups
    //mesh->finalizeFamiliesAndGroups();
    
    MULTIPR_LOG("OK\n");
    
        //---------------------------------------------------------------------
    // Build profils.
    //---------------------------------------------------------------------
    for (std::vector<Profil*>::iterator it = mProfils.begin(); it != mProfils.end(); ++it)
    {
        Profil* lProfil = new Profil();
        lProfil->create((*it)->getName());
        std::set<med_int> lSet;
        if ((*it)->getBinding() == OnNodes)
        {
            (*it)->extractSetOfElement(setOfNodes, lSet);
        }
        else
        {
            (*it)->extractSetOfElement(pSetOfElements[lProfil->getGeomIdx()], lSet);
        }
        if (lSet.size() == 0)
        {
            delete lProfil;
            continue;
        }
        lProfil->set(lSet);        
        mesh->mProfils.push_back(lProfil);
    }
    
    //---------------------------------------------------------------------
    // Create new fields and collect Gauss
    //---------------------------------------------------------------------
    // for each field
    set<string> newSetOfGauss;
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        
        Field* newField = NULL;
        if (currentField->isFieldOnNodes())
        {
            newField = currentField->extractSubSet(setOfNodes);
        }
        else
        {
                        if (pSetOfElements[currentField->getGeomIdx()].size() != 0)
                        {
                newField = currentField->extractSubSet(pSetOfElements[currentField->getGeomIdx()]);
                        }
        }
        
        if (newField != NULL && !newField->isEmpty())
        {
            mesh->mFields.push_back(newField);
            newField->getSetOfGaussLoc(newSetOfGauss);
        }
    }
    MULTIPR_LOG("Collect fields: ok: #gauss=" << newSetOfGauss.size() << endl);

    //---------------------------------------------------------------------
    // Build Gauss infos
    //---------------------------------------------------------------------
    for (set<string>::iterator itSet = newSetOfGauss.begin() ; itSet != newSetOfGauss.end(); itSet++)
    {
        const string& keyName = (*itSet);
        
        GaussLoc* gaussLoc = getGaussLocByName(keyName.c_str());
        if (gaussLoc != NULL)
        {
            GaussLoc* copyGaussLoc = new GaussLoc(*gaussLoc);
            mesh->mGaussLoc.push_back(copyGaussLoc);
            mesh->mGaussLocNameToGaussLoc.insert(make_pair(copyGaussLoc->getName(), copyGaussLoc));
        }
    }
    
    //---------------------------------------------------------------------
    // Compute bbox
    //---------------------------------------------------------------------
    mesh->mNodes->getBBox(mesh->mMeshBBoxMin, mesh->mMeshBBoxMax);
    
    return mesh;
}


Mesh* Mesh::createFromGroup(const Group* pGroup, const char* pNewMeshName)
{
    if (pGroup == NULL) throw NullArgumentException("pGroup should not be NULL", __FILE__, __LINE__);
    if (pNewMeshName == NULL) throw NullArgumentException("pNewMeshName should not be NULL", __FILE__, __LINE__);
    if (strlen(pNewMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("pNewMeshName length too long", __FILE__, __LINE__);
    //---------------------------------------------------------------------
    // Create a new mesh
    //---------------------------------------------------------------------
    Mesh* mesh = new Mesh();
    
    //---------------------------------------------------------------------
    // Build name of the new mesh
    //---------------------------------------------------------------------    
    strcpy(mesh->mMeshName, pNewMeshName);
    
    MULTIPR_LOG("Mesh name=|" << mesh->mMeshName << "|" << endl);
    
    //---------------------------------------------------------------------
    // Fill general infos
    //---------------------------------------------------------------------
    strcpy(mesh->mMeshUName, mMeshUName);
    strcpy(mesh->mMeshDesc, mMeshDesc);
    
    mesh->mMeshDim = mMeshDim;
    mesh->mMeshType = mMeshType;
    
    MULTIPR_LOG("Mesh u. name=|" << mesh->mMeshUName << "|" << endl);
    MULTIPR_LOG("Mesh desc=|" << mesh->mMeshDesc << "|" << endl);
    MULTIPR_LOG("Mesh dim=" << mesh->mMeshDim << endl);
    MULTIPR_LOG("Mesh Type=" << mesh->mMeshType << endl);
    
    //---------------------------------------------------------------------
    // Build nodes and elements
    //---------------------------------------------------------------------
    // Get all elements involved
        std::set< med_int>* setOfEltList = new std::set< med_int>[eMaxMedMesh];
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mElements[i] != NULL)
                {
                        const set<med_int> setOfElt = pGroup->getSetOfElt((eMeshType)i);
                        mesh->mElements[i] = mElements[i]->extractSubSet(setOfElt);
                        setOfEltList[i] = setOfElt;
                }
        }
    
    // Get all nodes involved
        // The nodes a common for all elements so we don't need to store them separately.
        set<med_int> setOfNodes;
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mesh->mElements[i] != NULL)
                {
                        const set<med_int>& curSetOfNodes = mesh->mElements[i]->getSetOfNodes();
                        setOfNodes.insert(curSetOfNodes.begin(), curSetOfNodes.end());
                }
        }
    mesh->mNodes = mNodes->extractSubSet(setOfNodes);
        
        // We need this for the optimized memory management.
        this->mGaussIndex.push_back(IndexPair(setOfEltList, setOfNodes));
    //---------------------------------------------------------------------
    // Remap nodes
    //---------------------------------------------------------------------
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mesh->mElements[i] != NULL)
                {
                        mesh->mElements[i]->remap(setOfNodes);
                }
        }

    //---------------------------------------------------------------------
    // Build families
    //---------------------------------------------------------------------
    MULTIPR_LOG("Build fam.:" << endl);
    // get families of nodes
    {
                set<med_int> famOfNodes = mesh->mNodes->getSetOfFamilies();
                for (set<med_int>::iterator itFam = famOfNodes.begin() ; itFam != famOfNodes.end() ; itFam++)
        {
            Family* famSrc = mFamIdToFam[*itFam];
            Family* famDest = famSrc->extractGroup(pGroup->getName().c_str());
            mesh->mFamilies.push_back(famDest);
        }
        }

    // get families of elements
    {
                set<med_int> famOfElt;
                for (int i = 0; i < eMaxMedMesh; ++i)
                {
                        if (mesh->mElements[i] != NULL)
                        {
                                set<med_int> curSetOfFamilies = mesh->mElements[i]->getSetOfFamilies();
                                famOfElt.insert(curSetOfFamilies.begin(), curSetOfFamilies.end());
                        }
                }
        for (set<med_int>::iterator itFam = famOfElt.begin() ; itFam != famOfElt.end() ; itFam++)
        {
            Family* famSrc = mFamIdToFam[*itFam];
            Family* famDest = famSrc->extractGroup(pGroup->getName().c_str());
            mesh->mFamilies.push_back(famDest);
        }
    }
    
    MULTIPR_LOG("Finalize:");
    
    // fill families with elements and build groups
    mesh->finalizeFamiliesAndGroups();
    
    MULTIPR_LOG("OK\n");
    
    //---------------------------------------------------------------------
    // Build profils.
    //---------------------------------------------------------------------
    for (std::vector<Profil*>::iterator it = mProfils.begin(); it != mProfils.end(); ++it)
    {
        Profil* lProfil = new Profil();
        lProfil->create((*it)->getName());
        std::set<med_int> lSet;
        if ((*it)->getBinding() == OnNodes)
        {
            (*it)->extractSetOfElement(setOfNodes, lSet);
        }
        else
        {
            (*it)->extractSetOfElement(setOfEltList[lProfil->getGeomIdx()], lSet);
        }
        if (lSet.size() == 0)
        {
            delete lProfil;
            continue;
        }
        lProfil->set(lSet);        
        mesh->mProfils.push_back(lProfil);
    }
    
    //---------------------------------------------------------------------
    // Create new fields and collect Gauss
    //---------------------------------------------------------------------
    // for each field
    set<string> newSetOfGauss;
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        
        Field* newField;
        if (currentField->isFieldOnNodes())
        {
            newField = currentField->extractSubSet(setOfNodes);
        }
        else
        {
                        if (setOfEltList[currentField->getGeomIdx()].size() != 0)
                        {
                newField = currentField->extractSubSet(setOfEltList[currentField->getGeomIdx()]);
                        }
        }
        
        if (!newField->isEmpty())
        {
            mesh->mFields.push_back(newField);
            newField->getSetOfGaussLoc(newSetOfGauss);
        }
    }
        // Get gauss locs for optimized fields reading.
        if (mFields.size() == 0)
        {
                for (unsigned itGaussLoc = 0 ; itGaussLoc < mGaussLoc.size() ; itGaussLoc++)
                {
                        const string& gaussLocName = mGaussLoc[itGaussLoc]->getName();
                        
                        if (gaussLocName.length() != 0)
                        {
                                newSetOfGauss.insert(gaussLocName);
                        }
                }
        }
    MULTIPR_LOG("Collect fields: ok: #gauss=" << newSetOfGauss.size() << endl);

    //---------------------------------------------------------------------
    // Build Gauss infos
    //---------------------------------------------------------------------
        for (set<string>::iterator itSet = newSetOfGauss.begin() ; itSet != newSetOfGauss.end(); itSet++)
    {
        const string& keyName = (*itSet);
        
        GaussLoc* gaussLoc = getGaussLocByName(keyName.c_str());
        if (gaussLoc != NULL)
        {
            GaussLoc* copyGaussLoc = new GaussLoc(*gaussLoc);
            mesh->mGaussLoc.push_back(copyGaussLoc);
            mesh->mGaussLocNameToGaussLoc.insert(make_pair(copyGaussLoc->getName(), copyGaussLoc));
        }
    }
    
    //---------------------------------------------------------------------
    // Compute bbox
    //---------------------------------------------------------------------
    mesh->mNodes->getBBox(mesh->mMeshBBoxMin, mesh->mMeshBBoxMax);
    
    return mesh;
}


Mesh* Mesh::createFromFamily(const Family* pFamily, const char* pNewMeshName)
{
    if (pFamily == NULL) throw NullArgumentException("pFamily should not be NULL", __FILE__, __LINE__);
    if (pNewMeshName == NULL) throw NullArgumentException("pNewMeshName should not be NULL", __FILE__, __LINE__);    
    if (strlen(pNewMeshName) > MED_TAILLE_NOM) 
    {
        char msg[256];
        sprintf(msg, "pNewMeshName length (=%d) too long: max size allowed is %d", int(strlen(pNewMeshName)), MED_TAILLE_NOM);
        throw IllegalArgumentException(msg, __FILE__, __LINE__);
    }
    //---------------------------------------------------------------------
    // Create a new mesh
    //---------------------------------------------------------------------
    Mesh* mesh = new Mesh();
    
    //---------------------------------------------------------------------
    // Build name of the new mesh
    //---------------------------------------------------------------------    
    strcpy(mesh->mMeshName, pNewMeshName);
    
    MULTIPR_LOG("Mesh name=|" << mesh->mMeshName << "|" << endl);
    
    //---------------------------------------------------------------------
    // Fill general infos
    //---------------------------------------------------------------------
    strcpy(mesh->mMeshUName, mMeshUName);
    strcpy(mesh->mMeshDesc, mMeshDesc);
    
    mesh->mMeshDim = mMeshDim;
    mesh->mMeshType = mMeshType;
    
    MULTIPR_LOG("Mesh u. name=|" << mesh->mMeshUName << "|" << endl);
    MULTIPR_LOG("Mesh desc=|" << mesh->mMeshDesc << "|" << endl);
    MULTIPR_LOG("Mesh dim=" << mesh->mMeshDim << endl);
    MULTIPR_LOG("Mesh Type=" << mesh->mMeshType << endl);
    
    //---------------------------------------------------------------------
    // Build nodes and elements
    //---------------------------------------------------------------------
    // Get all elements involved
        std::set< med_int>* setOfEltList = new std::set< med_int>[eMaxMedMesh];
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mElements[i] != NULL)
                {
                        const set<med_int> setOfElt = pFamily->getSetOfElt((eMeshType)i);
                        mesh->mElements[i] = mElements[i]->extractSubSet(setOfElt);
                        setOfEltList[i] = setOfElt;
                }
        }
    
    // Get all nodes involved
        // The nodes a common for all elements so we don't need to store them separately.
        set<med_int> setOfNodes;
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mesh->mElements[i] != NULL)
                {
                        const set<med_int>& curSetOfNodes = mesh->mElements[i]->getSetOfNodes();
                        setOfNodes.insert(curSetOfNodes.begin(), curSetOfNodes.end());
                }
        }
    mesh->mNodes = mNodes->extractSubSet(setOfNodes);
        
        // We need this for the optimized memory management.
        this->mGaussIndex.push_back(IndexPair(setOfEltList, setOfNodes));
    //---------------------------------------------------------------------
    // Remap nodes
    //---------------------------------------------------------------------
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mesh->mElements[i] != NULL)
                {
                        mesh->mElements[i]->remap(setOfNodes);
                }
        }

    //---------------------------------------------------------------------
    // Build families
    //---------------------------------------------------------------------
    MULTIPR_LOG("Build fam.:" << endl);
    // get families of nodes
        Family*    lFam = new Family(*pFamily);
    mesh->mFamilies.push_back(lFam);
            
    MULTIPR_LOG("Finalize:");
    
    // fill families with elements and build groups
    mesh->finalizeFamiliesAndGroups();
    
    MULTIPR_LOG("OK\n");
    
    //---------------------------------------------------------------------
    // Build profils.
    //---------------------------------------------------------------------
    for (std::vector<Profil*>::iterator it = mProfils.begin(); it != mProfils.end(); ++it)
    {
        Profil* lProfil = new Profil();
        lProfil->create((*it)->getName());
        std::set<med_int> lSet;
        if ((*it)->getBinding() == OnNodes)
        {
            (*it)->extractSetOfElement(setOfNodes, lSet);
        }
        else
        {
            (*it)->extractSetOfElement(setOfEltList[lProfil->getGeomIdx()], lSet);
        }
        if (lSet.size() == 0)
        {
            delete lProfil;
            continue;
        }
        lProfil->set(lSet);        
        mesh->mProfils.push_back(lProfil);
    }
    
    //---------------------------------------------------------------------
    // Create new fields and collect Gauss
    //---------------------------------------------------------------------
    // for each field
    set<string> newSetOfGauss;
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        
        Field* newField;
        if (currentField->isFieldOnNodes())
        {
            newField = currentField->extractSubSet(setOfNodes);
        }
        else
        {
                        if (setOfEltList[currentField->getGeomIdx()].size() != 0)
                        {
                newField = currentField->extractSubSet(setOfEltList[currentField->getGeomIdx()]);
                        }
        }
        
        if (!newField->isEmpty())
        {
            mesh->mFields.push_back(newField);
            newField->getSetOfGaussLoc(newSetOfGauss);
        }
    }
        // Get gauss locs for optimized fields reading.
        if (mFields.size() == 0)
        {
                for (unsigned itGaussLoc = 0 ; itGaussLoc < mGaussLoc.size() ; itGaussLoc++)
                {
                        const string& gaussLocName = mGaussLoc[itGaussLoc]->getName();
                        
                        if (gaussLocName.length() != 0)
                        {
                                newSetOfGauss.insert(gaussLocName);
                        }
                }
        }
    MULTIPR_LOG("Collect fields: ok: #gauss=" << newSetOfGauss.size() << endl);

    //---------------------------------------------------------------------
    // Build Gauss infos
    //---------------------------------------------------------------------
        for (set<string>::iterator itSet = newSetOfGauss.begin() ; itSet != newSetOfGauss.end(); itSet++)
    {
        const string& keyName = (*itSet);
        
        GaussLoc* gaussLoc = getGaussLocByName(keyName.c_str());
        if (gaussLoc != NULL)
        {
            GaussLoc* copyGaussLoc = new GaussLoc(*gaussLoc);
            mesh->mGaussLoc.push_back(copyGaussLoc);
            mesh->mGaussLocNameToGaussLoc.insert(make_pair(copyGaussLoc->getName(), copyGaussLoc));
        }
    }
    
    //---------------------------------------------------------------------
    // Compute bbox
    //---------------------------------------------------------------------
    mesh->mNodes->getBBox(mesh->mMeshBBoxMin, mesh->mMeshBBoxMax);
    
    return mesh;
}

Mesh* Mesh::mergePartial(vector<Mesh*> pMeshes, const char* pFieldName, int pFieldIt)
{   
    if (pMeshes.size() == 0) throw IllegalArgumentException("list must contain one mesh at least", __FILE__, __LINE__);

    //---------------------------------------------------------------------
    // Create a new mesh
    //---------------------------------------------------------------------
    Mesh* mesh = new Mesh();
    
    //---------------------------------------------------------------------
    // Build name of the new mesh
    //---------------------------------------------------------------------    
    strcpy(mesh->mMeshName, mMeshName);
    
    //---------------------------------------------------------------------
    // Merge general infos
    //---------------------------------------------------------------------
    strcpy(mesh->mMeshUName, mMeshUName);
    strcpy(mesh->mMeshDesc, mMeshDesc);    
    
    mesh->mMeshDim  = mMeshDim;
    mesh->mMeshType = mMeshType;
    
    //---------------------------------------------------------------------
    // Merge nodes and elements
    //---------------------------------------------------------------------
    vector<Nodes*>    nodes;    
    vector<Elements*> elements[eMaxMedMesh];
    vector<int>       offsets[eMaxMedMesh];
    
    int offset[eMaxMedMesh];
    for (unsigned j = 0; j < eMaxMedMesh; ++j)
    {
        offset[j] = mNodes->getNumberOfNodes();
    }
    
    for (unsigned i = 0 ; i < pMeshes.size() ; i++)
    {
        if (mMeshDim != pMeshes[i]->mMeshDim) throw IllegalStateException("all meshes should have same dimension", __FILE__, __LINE__);
        if (mMeshType != pMeshes[i]->mMeshType) throw IllegalStateException("all meshes should have same type", __FILE__, __LINE__);
        
        
        nodes.push_back(pMeshes[i]->mNodes);
        for (unsigned j = 0; j < eMaxMedMesh; ++j)
        {

            if (pMeshes[i]->mElements[j] != NULL)
            {
                elements[j].push_back(pMeshes[i]->mElements[j]);
                offsets[j].push_back(offset[j]);
            }
            offset[j] += pMeshes[i]->mNodes->getNumberOfNodes();            
        }
    }
    
    mesh->mNodes = mNodes->mergePartial(nodes);
    for (unsigned i = 0; i < eMaxMedMesh; ++i)
    {
        if (elements[i].size() != 0)
        {
            if (mElements[i] == NULL)
            {
                mElements[i] = new Elements(CELL_TYPES[i]);
                mElements[i]->mergePartial(elements[i].front(), offsets[i].front());
                elements[i].erase(elements[i].begin());
            }
            mesh->mElements[i] = mElements[i]->mergePartial(elements[i], offsets[i]);
        }
        else if (mElements[i] != NULL)
        {
            mesh->mElements[i] = mElements[i];
        }
    }

    
    //---------------------------------------------------------------------
    // Merge families
    //---------------------------------------------------------------------
    for (unsigned i = 0 ; i < mFamilies.size() ; i++)
    {
        Family* family = new Family(*(mFamilies[i]));
        mesh->mFamilies.push_back(family);
        mesh->mFamIdToFam.insert(make_pair(family->getId(), family));
    }
    
    for (unsigned j = 0 ; j < pMeshes.size() ; j++)
    {
        for (unsigned i = 0 ; i < pMeshes[j]->mFamilies.size() ; i++)
        {   
            // test if there is a fimaly with the same id
            map<med_int, Family*>::iterator itFam = mesh->mFamIdToFam.find(pMeshes[j]->mFamilies[i]->getId());        
            
            if (itFam == mesh->mFamIdToFam.end())
            {        
                // id not found: create a new family
                Family* family = new Family(*(pMeshes[j]->mFamilies[i]));
                mesh->mFamilies.push_back(family);
                mesh->mFamIdToFam.insert(make_pair(family->getId(), family));
            }
        }
    }
    
    //---------------------------------------------------------------------
    // Merge fields
    //---------------------------------------------------------------------
    // for each mesh

    vector<Field*> fields;
    for (unsigned i = 0 ; i < pMeshes.size() ; i++)
    {
        for (std::vector<Field*>::iterator it = pMeshes[i]->mFields.begin(); 
            it != pMeshes[i]->mFields.end(); ++it)
        {
            if (strcmp((*it)->getName(), pFieldName) == 0)
            {
                fields.push_back(*it);
                break;
            }
        }
    }
    bool    hasField = false;
    for (std::vector<Field*>::iterator it = mFields.begin(); 
        it != mFields.end(); ++it)
    {
        if (strcmp((*it)->getName(), pFieldName) == 0)
        {
            Field* field = (*it)->merge(fields, pFieldIt);
            mesh->mFields.push_back(field);
            hasField = true;
            break;
        }
    }
    if (hasField == false)
    {
        Field*  lField = fields.back();
        fields.pop_back();
        Field* field = lField->merge(fields, pFieldIt);
        mesh->mFields.push_back(field);
    }

    //---------------------------------------------------------------------
    // Merge Gauss infos
    //---------------------------------------------------------------------    
    // WARNING: assume Gauss infos are the same for the two meshes.    
    for (unsigned i = 0 ; i < mGaussLoc.size() ; i++)
    {
        GaussLoc* copyGaussLoc = new GaussLoc(*(mGaussLoc[i]));
        mesh->mGaussLoc.push_back(copyGaussLoc);
        mesh->mGaussLocNameToGaussLoc.insert(make_pair(copyGaussLoc->getName(), copyGaussLoc));
    }    
    
    return mesh;
}


MeshDis* Mesh::splitGroupsOfElements()
{
    MeshDis* meshDis = new MeshDis();
    meshDis->setSequentialMEDFilename(mMEDfilename);
    
    // get prefix from the original MED filename
    string strPrefix = removeExtension(mMEDfilename, ".med");
    int numGroup = 1;
        int numFam = 1;
    
    // for each group
    for (unsigned itGroup = 0 ; itGroup < mGroups.size() ; itGroup++)
    {
        Group* currentGroup = mGroups[itGroup];
        
        // skip this group if it is a group of nodes
        if (currentGroup->isGroupOfNodes()) 
        {
                        this->mGaussIndex.push_back(IndexPair());
                        continue;
        }
        
        char strPartName[256];
        sprintf(strPartName, "%s_%d", mMeshName, numGroup);
        
        char strMEDfilename[256];
                char strMedGroup[256];
                int i;
                for (i = 0; currentGroup->getName()[i] && currentGroup->getName()[i] != ' '; ++i)
                {
                        strMedGroup[i] = currentGroup->getName()[i];
                }
                strMedGroup[i] = '\0';
        sprintf(strMEDfilename, "%s_%s.med", strPrefix.c_str(), strMedGroup);

        Mesh* mesh = createFromGroup(currentGroup, mMeshName);
        
        // skip the group which contain all the others groups, even if it contains only 1 group
                if (mesh->getNumberOfElements() == this->getNumberOfElements())
        {
                        for (int i = 0; i < eMaxMedMesh; ++i)
                        {
                                this->mGaussIndex.back().first[i].clear();
                        }
                        this->mGaussIndex.back().second.clear();
            delete mesh;
                        mesh = NULL; 
                        continue;
                }
        meshDis->addMesh(
            MeshDisPart::MULTIPR_WRITE_MESH,
            mMeshName,
            numGroup,
            strPartName,
            "localhost",
            strMEDfilename,
            mesh);
        
        numGroup++;
    }
        if (mGroups.size() == 0)
        {
                for (unsigned itFam = 0; itFam < mFamilies.size(); ++itFam)
                {
                        
                        Family* currentFam = mFamilies[itFam];
        
                        // skip this family if it is a family of nodes
                        if (currentFam->isFamilyOfNodes()) 
                        {
                                this->mGaussIndex.push_back(IndexPair());
                                continue;
                        }
                        
                        char strPartName[256];
                        sprintf(strPartName, "%s_%d", mMeshName, numGroup);
                        
                        char strMEDfilename[256];
                        char strMedFam[256];
                        int i;
                        for (i = 0; currentFam->getName()[i] && currentFam->getName()[i] != ' '; ++i)
                        {
                                strMedFam[i] = currentFam->getName()[i];
                        }
                        strMedFam[i] = '\0';
                        sprintf(strMEDfilename, "%s_%s.med", strPrefix.c_str(), strMedFam);
        
                        Mesh* mesh = createFromFamily(currentFam, mMeshName);

                        meshDis->addMesh(
                                MeshDisPart::MULTIPR_WRITE_MESH,
                                mMeshName,
                                numFam,
                                strPartName,
                                "localhost",
                                strMEDfilename,
                                mesh);
                        
                        numFam++;
                }
        }
    
    return meshDis;
}


Mesh* Mesh::decimate(
    const char* pFilterName,
    const char* pArgv,
    const char* pNameNewMesh)
{
    //---------------------------------------------------------------------
    // Check parameters
    //---------------------------------------------------------------------    
    if (pFilterName == NULL) throw NullArgumentException("pFilterName should not be NULL", __FILE__, __LINE__);
    if (pArgv == NULL) throw NullArgumentException("pArgv should not be NULL", __FILE__, __LINE__);
    if (pNameNewMesh == NULL) throw NullArgumentException("pNameNewMesh should not be NULL", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Instanciate filter used for decimation
    //---------------------------------------------------------------------
    DecimationFilter* filter = DecimationFilter::create(pFilterName);
    
    //---------------------------------------------------------------------
    // Create new mesh by decimating current one
    //---------------------------------------------------------------------
    Mesh* decimatedMesh = filter->apply(this, pArgv, pNameNewMesh);
    
    //---------------------------------------------------------------------
    // Cleans
    //---------------------------------------------------------------------
    delete filter;
    
    return decimatedMesh;
}



void Mesh::getAllPointsOfField(Field* pField, int pTimeStepIt, std::vector<PointOfField>& pPoints, eMeshType pGeomType)
{
    //---------------------------------------------------------------------
    // Check arguments
    //---------------------------------------------------------------------
    if (pField == NULL) throw NullArgumentException("field should not be NULL", __FILE__, __LINE__);
    if (pTimeStepIt < 1) throw IllegalArgumentException("invalid field iteration; should be >= 1", __FILE__, __LINE__);
    
    if (mMeshDim != 3) throw UnsupportedOperationException("not yet implemented", __FILE__, __LINE__);
    if (pField->getType() != MED_FLOAT64) throw UnsupportedOperationException("not yet implemented", __FILE__, __LINE__);
    if (pField->getNumberOfComponents() != 1) throw UnsupportedOperationException("field have more than 1 component (vectorial field, expected scalar field)", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Get profile
    //---------------------------------------------------------------------

    const std::string&  profilName = pField->getProfil(pTimeStepIt);
    std::vector<Profil*>::iterator it;
    Profil* profile = NULL;
    int count = 0;
    if (profilName.size() > 0)
    {
        for (it = mProfils.begin(); it != mProfils.end(); ++it)
        {
            if ((*it)->getName() == profilName)
            {
                profile = (*it);
                break;
            }
        }
        if (it == mProfils.end()) throw IllegalStateException("Can't find the profile in the mesh.", __FILE__, __LINE__);
        
    }
    
    //---------------------------------------------------------------------
    // Collect points
    //---------------------------------------------------------------------
    
    if (pField->isFieldOnNodes())
    {
        //-------------------------------------------------------------
        // Case 1: field of nodes
        //-------------------------------------------------------------
        if (mNodes == NULL) throw IllegalStateException("no nodes in the current mesh", __FILE__, __LINE__);
        
        // for each node
        for (int itNode = 0, size = mNodes->getNumberOfNodes() ; itNode < size ; itNode++)
        {
            if (profile && profile->find(itNode) == false)
            {
                continue;
            }
            // collect coordinates and value of the point
            const med_float* coo = mNodes->getCoordinates(itNode);
            
            const med_float* val = 
                reinterpret_cast<const med_float*>(pField->getValue(pTimeStepIt, count + 1));
            // add new point
            pPoints.push_back(PointOfField(coo[0], coo[1], coo[2], val[0]));
            ++count;
        }
    }
    else
    {
        //-------------------------------------------------------------
        // Case 2: field of elements
        //-------------------------------------------------------------
    
        if (mElements[pGeomType] == NULL) throw IllegalStateException("no elements in the current mesh", __FILE__, __LINE__);
        
        const string& nameGaussLoc = pField->getNameGaussLoc(pTimeStepIt);
        GaussLoc* gaussLoc = getGaussLocByName(nameGaussLoc.c_str());
        if (gaussLoc == NULL) throw IllegalStateException("no Gauss localization for these elements", __FILE__, __LINE__);
        
        int numGauss = pField->getNumberOfGaussPointsByElement(pTimeStepIt);
        int size = gaussLoc->getDim() * gaussLoc->getNumGaussPoints();
        med_float* cooGaussPts = new med_float[size];
        
        int dim = mElements[pGeomType]->getTypeOfPrimitives() / 100;
        //int numNodes = mElements[pGeomType]->getTypeOfPrimitives() % 100;
        size = dim * numGauss;
        med_float* cooNodes = new med_float[size];
        
        // for each elements
        for (int itElt = 0, size = mElements[pGeomType]->getNumberOfElements() ; itElt < size ; itElt++)
        {
            if (profile && profile->find(itElt) == false)
            {
                continue;
            }
            // get coordinates of nodes of the current elements
            mElements[pGeomType]->getCoordinates(itElt, mNodes, cooNodes, numGauss);
            
            // compute coordinates of gauss points
            gaussLoc->getCoordGaussPoints(cooNodes, cooGaussPts, numGauss);
            
            const med_float* val = 
                reinterpret_cast<const med_float*>(pField->getValue(pTimeStepIt, count + 1));
        
            // for each point of Gauss of the element
            med_float* srcCoo = cooGaussPts;
            for (int itPtGauss = 0 ; itPtGauss < numGauss ; itPtGauss++)
            {
                pPoints.push_back(PointOfField(srcCoo[0], srcCoo[1], srcCoo[2], val[itPtGauss]));
                srcCoo += 3;
            }
            ++count;
        }
        
        delete[] cooNodes;
        delete[] cooGaussPts;
    }
}


float Mesh::evalDefaultRadius(int pN) const
{
    if (mFields.size() == 0) return 1.0f;
    
    //---------------------------------------------------------------------
    // Compute default radius
    //---------------------------------------------------------------------    
    
    med_float volumeBBox = 
        (mMeshBBoxMax[0] - mMeshBBoxMin[0]) * 
        (mMeshBBoxMax[1] - mMeshBBoxMin[1]) *
        (mMeshBBoxMax[2] - mMeshBBoxMin[2]);
        
    if (isnan(volumeBBox))
    {
        return 1.0f;
    }
    
    const med_float k = 0.8; // considered 80% of the volume
    
    // get number of gauss points in the field
    try
    {
        Field* anyField = mFields[mFields.size()-1];
        int numTimeSteps = anyField->getNumberOfTimeSteps();

        int numGaussPoints = getNumberOfElements() * anyField->getNumberOfGaussPointsByElement(numTimeSteps-1);
    
        med_float radius = med_float(pow( (3.0/4.0) * pN * k * volumeBBox / (3.1415 * numGaussPoints), 1.0/3.0));
    
        return float(radius);
    }
    catch (...)
    {
        return 1.0f;
    }
}

void Mesh::_openMEDFile(const char* pMEDfilename, med_mode_acces pMEDModeAccess)
{
    reset();
    
    //---------------------------------------------------------------------
    // Check arguments
    //---------------------------------------------------------------------
    MULTIPR_LOG("Check arguments: ");
    if (pMEDfilename == NULL) throw NullArgumentException("pMEDfilename should not be NULL", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
      
    strncpy(mMEDfilename, pMEDfilename, 256);
      
    //---------------------------------------------------------------------
    // Open MED file (READ_ONLY)
    //---------------------------------------------------------------------
    MULTIPR_LOG("Open MED file: ");
    mMEDfile = MEDouvrir(mMEDfilename, pMEDModeAccess);
    if (mMEDfile <= 0) throw FileNotFoundException("MED file not found", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
      
    //---------------------------------------------------------------------
    // Check valid HDF format
    //---------------------------------------------------------------------
    MULTIPR_LOG("Format HDF: ");
    if (MEDformatConforme(mMEDfilename) != 0) throw IOException("invalid file", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
      
    //---------------------------------------------------------------------
    // Get MED version
    //---------------------------------------------------------------------
    MULTIPR_LOG("MED version: ");
    med_int verMajor, verMinor, verRelease;
    med_err ret = MEDversionLire(mMEDfile, &verMajor, &verMinor, &verRelease);
    if (ret != 0) throw IOException("error while reading MED version", __FILE__, __LINE__);
    MULTIPR_LOG(verMajor << "." << verMinor << "." << verRelease << ": OK\n");
  }


void Mesh::_readSequentialMED(const char* pMeshName, bool pReadFields)
{    
    med_err ret = 1;

    //---------------------------------------------------------------------
    // Check arguments
    //---------------------------------------------------------------------
    MULTIPR_LOG("Check arguments: ");
    if (pMeshName == NULL) throw NullArgumentException("pMeshName should not be NULL", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
    
    strncpy(mMeshName, pMeshName, MED_TAILLE_NOM);
    
    //---------------------------------------------------------------------
    // Read profils
    //---------------------------------------------------------------------
    MULTIPR_LOG("Profils: ");
    med_int nbProfils = MEDnProfil(mMEDfile);
    for (med_int i = 1; i <= nbProfils; ++i)
    {
        Profil* profil = new Profil();
        profil->readMED(mMEDfile, i);
        profil->readProfilBinding(mMEDfile, const_cast<char*>(pMeshName));
        this->mProfils.push_back(profil);
    }
    MULTIPR_LOG("OK\n");
    
    //---------------------------------------------------------------------
    // Read all Gauss localizations
    //---------------------------------------------------------------------
    MULTIPR_LOG("Gauss localizations: ");
    readGaussLoc();
    
    //---------------------------------------------------------------------
    // Read scalars (should be 0)
    //---------------------------------------------------------------------
    MULTIPR_LOG("Scalars: ");
    med_int nbScalars = MEDnScalaire(mMEDfile);
    if (nbScalars != 0) throw UnsupportedOperationException("scalars information not yet implemented", __FILE__, __LINE__);
    MULTIPR_LOG(nbScalars << ": OK\n");    
    
    //---------------------------------------------------------------------
    // Find the mesh
    //---------------------------------------------------------------------
    // read number of meshes
    MULTIPR_LOG("Num meshes: ");
    med_int nbMeshes = MEDnMaa(mMEDfile);
    if (nbMeshes <= 0) throw IOException("i/o error while reading number of meshes in MED file", __FILE__, __LINE__);
    MULTIPR_LOG(nbMeshes << ": OK\n");
    
    med_int meshIndex = -1;
    // iteration over mesh to find the mesh we want
    // for each mesh in the file (warning: first mesh is number 1)
    for (int itMesh = 1 ; itMesh <= nbMeshes ; itMesh++)
    {
        char meshName[MED_TAILLE_NOM + 1];
        
        ret = MEDmaaInfo(
            mMEDfile, 
            itMesh, 
            meshName, 
            &mMeshDim, 
            &mMeshType, 
            mMeshDesc);
            
        if (ret != 0) throw IOException("i/o error while reading mesh information in MED file", __FILE__, __LINE__);
        MULTIPR_LOG("Mesh: |" << meshName << "|");
        
        // test if the current mesh is the mesh we want
        if (strcmp(pMeshName, meshName) == 0)
        {
            // *** mesh found ***
            MULTIPR_LOG(" OK (found)\n");
            meshIndex = itMesh;
            break;
        }
        else
        {
            // not the mesh we want: skip this mesh
            MULTIPR_LOG(" skipped\n");
        }
    }
    
    if (meshIndex == -1)
    {
        throw IllegalStateException("mesh not found in the given MED file", __FILE__, __LINE__);
    }
    
    //---------------------------------------------------------------------
    // Check mesh validity
    //---------------------------------------------------------------------
    // dimension of the mesh must be 3 (= 3D mesh)
    MULTIPR_LOG("Mesh is 3D: ");
    if (mMeshDim != 3) throw UnsupportedOperationException("dimension of the mesh should be 3; other dimension not yet implemented", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
    
    // mesh must not be a grid
    MULTIPR_LOG("Mesh is not a grid: ");
    if (mMeshType != MED_NON_STRUCTURE) 
        throw UnsupportedOperationException("grid not supported", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");

    med_connectivite connectivite = MED_NOD; // NODAL CONNECTIVITY ONLY
        // Read all the supported geometry type.
        for (int itCell = 0; itCell < eMaxMedMesh ; ++itCell)
    {
        med_int meshNumCells = MEDnEntMaa(
            mMEDfile, 
            mMeshName, 
            MED_CONN, 
            MED_MAILLE, 
            CELL_TYPES[itCell], 
            connectivite);
        
                        
                //---------------------------------------------------------------------
                // Read elements
                //---------------------------------------------------------------------
        if (meshNumCells > 0)
        {
                        mElements[itCell] = new Elements();
                        mElements[itCell]->readMED(mMEDfile, mMeshName, mMeshDim, MED_MAILLE, CELL_TYPES[itCell]);
        }
        }
    // everything is OK...
    
    //---------------------------------------------------------------------
    // Check num joint = 0
    //---------------------------------------------------------------------
    MULTIPR_LOG("Num joints: ");
    med_int numJoints = MEDnJoint(mMEDfile, mMeshName);
    MULTIPR_LOG(numJoints << ": OK\n");
    
    //---------------------------------------------------------------------
    // Check num equivalence = 0
    //---------------------------------------------------------------------
    MULTIPR_LOG("Num equivalences: ");
    med_int numEquiv = MEDnEquiv(mMEDfile, mMeshName);
    MULTIPR_LOG(numEquiv << ": OK\n");
    
    //---------------------------------------------------------------------
    // Read nodes
    //---------------------------------------------------------------------
    mNodes = new Nodes();
    mNodes->readMED(mMEDfile, mMeshName, mMeshDim);
    mNodes->getBBox(mMeshBBoxMin, mMeshBBoxMax);
    
    if (mNodes->getNumberOfNodes() != 0)
    {
    
        //---------------------------------------------------------------------
        // Read families
        //---------------------------------------------------------------------
        readFamilies();
        finalizeFamiliesAndGroups();
    
        //---------------------------------------------------------------------
        // Read fields
        //---------------------------------------------------------------------
        if (pReadFields)
                {
                        readFields();
                }
    }
    
    //---------------------------------------------------------------------
    // Close the MED file
    //---------------------------------------------------------------------
    MULTIPR_LOG("Close MED file: ");
    ret = MEDfermer(mMEDfile);
    if (ret != 0) throw IOException("i/o error while closing MED file", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
}


void Mesh::readSequentialMED(const char* pMEDfilename, const char* pMeshName, bool pReadFields)
{    
    _openMEDFile(pMEDfilename);
    _readSequentialMED(pMeshName, pReadFields);
}


void Mesh::readSequentialMED(const char* pMEDfilename, med_int pMeshNumber, bool pReadFields)
{
    _openMEDFile(pMEDfilename);

    //---------------------------------------------------------------------
    // Find the mesh
    //---------------------------------------------------------------------
    // read number of meshes
    MULTIPR_LOG("Num meshes: ");
    med_int nbMeshes = MEDnMaa(mMEDfile);
    if (nbMeshes <= 0) throw IOException("i/o error while reading number of meshes in MED file", __FILE__, __LINE__);
    MULTIPR_LOG(nbMeshes << ": OK\n");
    
    med_int meshDim;
    med_maillage meshType;
    char meshDesc[MED_TAILLE_DESC + 1]; 
    char meshName[MED_TAILLE_NOM + 1];
    
    med_err ret = MEDmaaInfo(mMEDfile, pMeshNumber, meshName, &meshDim, &meshType, meshDesc);
    if (ret != 0) throw IOException("i/o error while reading mesh information in MED file", __FILE__, __LINE__);

    _readSequentialMED(meshName, pReadFields);
}


void Mesh::writeMED(const char* pMEDfilename)
{
    writeMED(pMEDfilename, getName());
}


void Mesh::writeMED(const char* pMEDfilename, const char* pMeshName)
{
    bool noMesh = true;
    MULTIPR_LOG("Write MED: " << pMEDfilename << endl);
    if (pMEDfilename == NULL) throw NullArgumentException("pMEDfilename should not be NULL", __FILE__, __LINE__);
    if (strlen(pMEDfilename) == 0) throw IllegalArgumentException("pMEDfilename size is 0", __FILE__, __LINE__);
    
    if (pMeshName == NULL) throw NullArgumentException("pMeshName should not be NULL", __FILE__, __LINE__);
    if (strlen(pMeshName) == 0) throw IllegalArgumentException("pMeshName size is 0", __FILE__, __LINE__);

    remove(pMEDfilename);
    
    char aMeshName[MED_TAILLE_NOM + 1];
    strncpy(aMeshName, pMeshName, MED_TAILLE_NOM);

    //---------------------------------------------------------------------
    // Create the new MED file (WRITE_ONLY)
    //---------------------------------------------------------------------
    med_idt newMEDfile = MEDouvrir(const_cast<char*>(pMEDfilename), MED_CREATION);
    if (newMEDfile == -1) throw IOException("", __FILE__, __LINE__);

    //---------------------------------------------------------------------
    // Write scalars
    //---------------------------------------------------------------------
    // no scalars to write
    
    //---------------------------------------------------------------------
    // Create mesh: must be created first
    //---------------------------------------------------------------------
    med_err ret = MEDmaaCr(
        newMEDfile,
        aMeshName,
        mMeshDim,
        MED_NON_STRUCTURE,
        mMeshDesc);

    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    MULTIPR_LOG("    Create mesh: |" << aMeshName << "|: OK" << endl);
    
    //---------------------------------------------------------------------
    // Write nodes and elements (mesh must exist)
    //---------------------------------------------------------------------
    if (mNodes == NULL) throw IllegalStateException("mNodes should not be NULL", __FILE__, __LINE__);
    mNodes->writeMED(newMEDfile, aMeshName);
    MULTIPR_LOG("    Write nodes: ok" << endl);
    
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
                if (mElements[i] != NULL)
                {
                        noMesh = false;
                        mElements[i]->writeMED(newMEDfile, aMeshName, mMeshDim);
                }
        }
        if (noMesh == true)
        {
                //throw IllegalStateException("No mesh writen.", __FILE__, __LINE__);
                return ;
        }
        
        MULTIPR_LOG("    write elt: ok" << endl);
    
    //---------------------------------------------------------------------
    // Write families (mesh must exist)
    //---------------------------------------------------------------------
    // MED assume there is a family 0 in the file.
    bool    createFamZero = true;
    for(std::vector<Family*>::iterator it =  mFamilies.begin(); 
        it != mFamilies.end(); ++it)
    {
        if ((*it)->getId() == 0)
        {
            createFamZero = false;
            break;
        }
    }
    if (createFamZero)
    {
        Family  famZero;
        famZero.setId(0);
        strcpy(const_cast<char*>(famZero.getName()), "FAMILLE_ZERO");
        famZero.writeMED(newMEDfile, aMeshName);
    }
            
    for (unsigned itFam = 0 ; itFam < mFamilies.size() ; ++itFam)
    {
        Family* fam = mFamilies[itFam];
                fam->writeMED(newMEDfile, aMeshName);
    }
    MULTIPR_LOG("    Write families: ok" << endl);
    
    //---------------------------------------------------------------------
    // Write profil
    //---------------------------------------------------------------------
    for (unsigned itProf = 0; itProf < mProfils.size(); ++itProf)
    {
        Profil* prof = mProfils[itProf];
        prof->writeMED(newMEDfile);
    }
    
    //---------------------------------------------------------------------
    // Write Gauss localization (must be written before fields)
    //---------------------------------------------------------------------
    for (unsigned itGaussLoc = 0 ; itGaussLoc < mGaussLoc.size() ; 
        itGaussLoc++)
    {
        GaussLoc* gaussLoc = mGaussLoc[itGaussLoc];
        gaussLoc->writeMED(newMEDfile);
    }

    MULTIPR_LOG("    Write Gauss: ok" << endl);
    
    //---------------------------------------------------------------------
    // Write fields
    //---------------------------------------------------------------------
    std::set<std::string> fieldNameList;
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* field = mFields[itField];
        if (fieldNameList.find(std::string(field->getName())) != fieldNameList.end())
        {
            field->writeMED(newMEDfile, aMeshName, false);
        }
        else
        {
            fieldNameList.insert(std::string(field->getName()));
            field->writeMED(newMEDfile, aMeshName);
        }
    }

    MULTIPR_LOG("    Write fields: ok" << endl);
    
    //---------------------------------------------------------------------
    // Close the new MED file
    //---------------------------------------------------------------------
    ret = MEDfermer(newMEDfile);
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
}

void Mesh::readGaussLoc()
{
    MULTIPR_LOG("Gauss ref: ");
    med_int numGauss = MEDnGauss(mMEDfile);
    if (numGauss < 0) throw IOException("", __FILE__, __LINE__);
    MULTIPR_LOG(numGauss << ": OK\n");
    
    for (int itGauss = 1 ; itGauss <= numGauss ; itGauss++)
    {
        GaussLoc* gaussLoc = new GaussLoc();
        gaussLoc->readMED(mMEDfile, itGauss);
        MULTIPR_LOG((*gaussLoc) << endl);
        mGaussLoc.push_back(gaussLoc);
        mGaussLocNameToGaussLoc.insert(make_pair(gaussLoc->getName(), gaussLoc));
    }
}

void Mesh::readFamilies()
{
    med_int numFamilies = MEDnFam(mMEDfile, mMeshName);
    if (numFamilies <= 0) throw IOException("", __FILE__, __LINE__);

    for (int itFam = 1 ; itFam <= numFamilies ; ++itFam)
    {
        Family* fam = new Family();
        fam->readMED(mMEDfile, mMeshName, itFam);
        mFamilies.push_back(fam);
    }
}


void Mesh::finalizeFamiliesAndGroups()
{
    if (mFamilies.size() == 0)
    {
        return ;
    }
    //---------------------------------------------------------------------
    // Build mapping between family id and pointers towards families
    //---------------------------------------------------------------------
    for (unsigned itFam = 0 ; itFam < mFamilies.size() ; ++itFam)
    {
        Family* fam  = mFamilies[itFam];
        mFamIdToFam.insert(make_pair(fam->getId(), fam));
    }
    //---------------------------------------------------------------------
    // Fill families of nodes
    //---------------------------------------------------------------------
    for (int itNode = 1 ; itNode <= mNodes->getNumberOfNodes() ; ++itNode)
    {
        // get family of the ith nodes
        int famIdent = mNodes->getFamIdent(itNode - 1); // MED nodes start at 1
                
                map<med_int, Family*>::iterator itFam = mFamIdToFam.find(famIdent);
        
        if (itFam == mFamIdToFam.end()) 
        {
            char msg[256];
            sprintf(msg, "wrong family of nodes for node #%d: family %d not found", itNode, famIdent);
            throw IllegalStateException(msg, __FILE__, __LINE__);
        }
        
        Family* fam = (*itFam).second;
        
        // add the current node to its family
        fam->insertElt(itNode);
        fam->setIsFamilyOfNodes(true);
    }
    //---------------------------------------------------------------------
    // Fill families of elements
    //---------------------------------------------------------------------
        for (int itMesh = 0; itMesh < eMaxMedMesh; ++itMesh)
        {
                if (mElements[itMesh] != NULL)
                {
                for (int itElt = 1 ; itElt <= mElements[itMesh]->getNumberOfElements() ; itElt++)
                {
                        // get family of the ith element (MED index start at 1)
                        int famIdent = mElements[itMesh]->getFamilyIdentifier(itElt - 1);

                                map<med_int, Family*>::iterator itFam = mFamIdToFam.find(famIdent);
        
                        if (itFam == mFamIdToFam.end()) 
                        {
                        char msg[256];
                        sprintf(msg, "wrong family of elements for element #%d: family %d not found", itElt, famIdent);
                        throw IllegalStateException(msg, __FILE__, __LINE__);
                        }
        
                        Family* fam = (*itFam).second;
        
                        // add the current element to its family
                        fam->insertElt( itElt, (eMeshType)itMesh); 
                        fam->setIsFamilyOfNodes(false);
                }
                }
        }
    //---------------------------------------------------------------------
    // Build groups
    //---------------------------------------------------------------------
    // for each family
    for (unsigned itFam = 0 ; itFam < mFamilies.size() ; itFam++)
    {
        mFamilies[itFam]->buildGroups(mGroups, mGroupNameToGroup);
    }
}


void Mesh::readFields()
{
    //---------------------------------------------------------------------
    // Read number of fields
    //---------------------------------------------------------------------
    MULTIPR_LOG("Read fields: ");
    med_int numFields = MEDnChamp(mMEDfile, 0);
    if (numFields <= 0) throw IOException("", __FILE__, __LINE__);
    MULTIPR_LOG(numFields << ": OK\n");

    //---------------------------------------------------------------------
    // Iterate over fields
    //---------------------------------------------------------------------
    // for each field, read number of components and others infos
        for (int itField = 1 ; itField <= numFields ; itField++)
    {
                for (int i = 0; i < eMaxMedMesh; ++i)
                {
                Field* field = new Field();
                field->readMED(mMEDfile, itField, mMeshName, CELL_TYPES[i]);
        
            // if the nth field does not apply on our mesh => slip it
                if (field->isEmpty())
                {
                    delete field;
                }
                else
                {
                                mFields.push_back(field);
                                // ReadMed will always work with fields on node so we need to stop the first time.
                                // ie : CELL_TYPES[i] is not used in this case.
                                if (field->isFieldOnNodes())
                                {
                                        break;
                                }
                }
                }
    }
}


ostream& operator<<(ostream& pOs, Mesh& pM)
{
    pOs << "Mesh: " << endl;
    pOs << "    MED file =|" << pM.mMEDfilename << "|" << endl;
    pOs << "    Name     =|" << pM.mMeshName << "|" << endl;
    pOs << "    Unv name =|" << pM.mMeshUName << "|" << endl;
    pOs << "    Desc     =|" << pM.mMeshDesc << "|" << endl;
    pOs << "    Dim      =" << pM.mMeshDim << endl;
    pOs << "    Type     =" << ((pM.mMeshType == MED_STRUCTURE)?"STRUCTURE":"NON_STRUCTURE") << endl;
    pOs << "    BBox     =[" << pM.mMeshBBoxMin[0] << " ; " << pM.mMeshBBoxMax[0] << "] x [" << pM.mMeshBBoxMin[1] << " ; " << pM.mMeshBBoxMax[1] << "] x [" << pM.mMeshBBoxMin[2] << " ; " << pM.mMeshBBoxMax[2] << "]" << endl;    
    
    int numFamOfNodes = 0;
    for (unsigned i = 0 ; i < pM.mFamilies.size() ; i++)
    {
        if (pM.mFamilies[i]->isFamilyOfNodes()) 
        {
            numFamOfNodes++;
        }            
    }
    
    int numGroupsOfNodes = 0;
    for (unsigned i = 0 ; i < pM.mGroups.size() ; i++)
    {
        if (pM.mGroups[i]->isGroupOfNodes()) 
        {
            numGroupsOfNodes++;
        }            
    }
    
    if (pM.mFlagPrintAll)
    {
        cout << (*(pM.mNodes)) << endl;
        for (int i = 0; i < eMaxMedMesh; ++i)
                {
                        if (pM.mElements[i] != NULL)
                        {
                        cout << (*(pM.mElements[i])) << endl;
                        }
                }
        
        pOs << "    Families : #=" << pM.mFamilies.size() << " (nodes=" << numFamOfNodes << " ; elements=" << (pM.mFamilies.size() - numFamOfNodes) << ")" << endl;
        for (unsigned i = 0 ; i < pM.mFamilies.size() ; i++)
        {
            cout << (*(pM.mFamilies[i])) << endl;
        }
        
        pOs << "    Groups   : #=" << pM.mGroups.size() << " (nodes=" << numGroupsOfNodes << " ; elements=" << (pM.mGroups.size() - numGroupsOfNodes) << ")" << endl;
        for (unsigned i = 0 ; i < pM.mGroups.size() ; i++)
        {
            cout << (*(pM.mGroups[i])) << endl;
        }
        
        pOs << "    Gauss loc: #=" << pM.mGaussLoc.size() << endl;
        for (unsigned i = 0 ; i < pM.mGaussLoc.size() ; i++)
        {
            cout << (*(pM.mGaussLoc[i])) << endl;
        }
        
        pOs << "    Fields   : #=" << pM.mFields.size() << endl;
        for (unsigned i = 0 ; i < pM.mFields.size() ; i++)
        {
            cout << (*(pM.mFields[i])) << endl;
        }
    }
    else
    {
        if (pM.mNodes != NULL)
        {
            pOs << "    Nodes    : #=" << pM.mNodes->getNumberOfNodes() << endl;
        }
        for (int i = 0; i < eMaxMedMesh; ++i)
                {
                        if (pM.mElements[i] != NULL)
                        {
                                const set<med_int>& setOfNodes = pM.mElements[i]->getSetOfNodes();
                                if (setOfNodes.size() == 0)
                                {
                                        pOs << "    Elt      : #=" << pM.mElements[i]->getNumberOfElements() << endl;
                                }
                                else
                                {
                                        set<med_int>::iterator itNode = setOfNodes.end();
                                        itNode--;
                                        pOs << "    Elt      : #=" << pM.mElements[i]->getNumberOfElements() << " node_id_min=" << (*(setOfNodes.begin())) << " node_id_max=" << (*itNode) << endl;
                                }
                        }
        }
        pOs << "    Families : #=" << pM.mFamilies.size() << " (nodes=" << numFamOfNodes << " ; elements=" << (pM.mFamilies.size() - numFamOfNodes) << ")" << endl;
        pOs << "    Groups   : #=" << pM.mGroups.size() << " (nodes=" << numGroupsOfNodes << " ; elements=" << (pM.mGroups.size() - numGroupsOfNodes) << ")" << endl;
        pOs << "    Gauss loc: #=" << pM.mGaussLoc.size() << endl;
        pOs << "    Fields   : #=" << pM.mFields.size() << endl;
    }
    
    return pOs;
}


} // namespace  multipr

// EOF
