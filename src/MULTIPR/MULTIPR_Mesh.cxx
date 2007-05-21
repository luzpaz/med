// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

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

#include <cmath>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class Mesh implementation
//*****************************************************************************

Mesh::Mesh()
{
    mNodes    = NULL;
    mElements = NULL;
    
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
    if (mElements != NULL) { delete mElements; mElements = NULL; }
    
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
        delete mFields[itField];
    }  
    mFields.clear();
    
    for (unsigned itProfil = 0 ; itProfil < mProfils.size() ; itProfil++)
    {
        delete mProfils[itProfil];
    }  
    mProfils.clear();
    
    mFlagPrintAll = false;
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


Field* Mesh::getFieldByName(const char* pFieldName) const
{
    if (pFieldName == NULL) throw NullArgumentException("pFieldName should not be NULL", __FILE__, __LINE__);
    
    Field* retField = NULL;
    
    // for each field
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* currentField = mFields[itField];
        if (strcmp(pFieldName, currentField->getName()) == 0)
        {
            // field found!
            retField = currentField;
            break;
        }
    }
    
    return retField;
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


int Mesh::getNumberOfElements() const
{
    if (mElements == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    
    return mElements->getNumberOfElements();
}


Mesh* Mesh::createFromSetOfElements(const std::set<med_int>& pSetOfElements, const char* pNewMeshName)
{
    if (pNewMeshName == NULL) throw NullArgumentException("pNewMeshName", __FILE__, __LINE__);
    
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
    mesh->mElements = mElements->extractSubSet(pSetOfElements);
    MULTIPR_LOG((*(mesh->mElements)) << endl);
    
    // get all nodes involved
    const set<med_int> setOfNodes = mesh->mElements->getSetOfNodes();
    mesh->mNodes = mNodes->extractSubSet(setOfNodes);
    MULTIPR_LOG((*(mesh->mNodes)) << endl);
    
    //---------------------------------------------------------------------
    // Remap nodes
    //---------------------------------------------------------------------
    mesh->mElements->remap();
    MULTIPR_LOG((*(mesh->mElements)) << endl);

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
            cout << (*famSrc) << endl;
            Family* famDest = famSrc->extractGroup(NULL);
            mesh->mFamilies.push_back(famDest);
        }
    }
    
    // get families of elements
    {
        set<med_int> famOfElt = mesh->mElements->getSetOfFamilies();
        for (set<med_int>::iterator itFam = famOfElt.begin() ; itFam != famOfElt.end() ; itFam++)
        {
            Family* famSrc = mFamIdToFam[*itFam];
            Family* famDest = famSrc->extractGroup(NULL);
            mesh->mFamilies.push_back(famDest);
        }
    }
    
    MULTIPR_LOG("Finalize:");
    
    // fill families with elements and build groups
    mesh->finalizeFamiliesAndGroups();
    
    MULTIPR_LOG("OK\n");
    
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
            newField = currentField->extractSubSet(pSetOfElements);
        }
        
        if (!newField->isEmpty())
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
    // get all elements involved
    const set<med_int> setOfElt = pGroup->getSetOfElt();
    mesh->mElements = mElements->extractSubSet(setOfElt);
    MULTIPR_LOG((*(mesh->mElements)) << endl);
    
    // get all nodes involved
    const set<med_int> setOfNodes = mesh->mElements->getSetOfNodes();
    mesh->mNodes = mNodes->extractSubSet(setOfNodes);
    MULTIPR_LOG((*(mesh->mNodes)) << endl);
    
    //---------------------------------------------------------------------
    // Remap nodes
    //---------------------------------------------------------------------
    mesh->mElements->remap();
    MULTIPR_LOG((*(mesh->mElements)) << endl);

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
        set<med_int> famOfElt = mesh->mElements->getSetOfFamilies();
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
            newField = currentField->extractSubSet(setOfElt);
        }
        
        if (!newField->isEmpty())
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


Mesh* Mesh::mergePartial(const Mesh* pMesh)
{
    if (pMesh == NULL) throw NullArgumentException("pMesh should not be NULL", __FILE__, __LINE__);
    
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
    
    if (mMeshDim != pMesh->mMeshDim) throw IllegalStateException("the two mesh should have same dimension", __FILE__, __LINE__);
    if (mMeshType != pMesh->mMeshType) throw IllegalStateException("the two mesh should have same type", __FILE__, __LINE__);
    
    mesh->mMeshDim  = mMeshDim;
    mesh->mMeshType = mMeshType;
    
    //---------------------------------------------------------------------
    // Merge nodes and elements
    //---------------------------------------------------------------------
    mesh->mNodes    = mNodes->mergePartial(pMesh->mNodes);
    mesh->mElements = mElements->mergePartial(pMesh->mElements, mNodes->getNumberOfNodes());
    
    //---------------------------------------------------------------------
    // Merge families
    //---------------------------------------------------------------------
    for (unsigned i = 0 ; i < mFamilies.size() ; i++)
    {
        Family* family = new Family(*(mFamilies[i]));
        mesh->mFamilies.push_back(family);
        mesh->mFamIdToFam.insert(make_pair(family->getId(), family));
    }
    
    for (unsigned i = 0 ; i < pMesh->mFamilies.size() ; i++)
    {   
        // test if there is a fimaly with the same id
        map<med_int, Family*>::iterator itFam = mesh->mFamIdToFam.find(pMesh->mFamilies[i]->getId());        
        
        if (itFam == mesh->mFamIdToFam.end())
        {        
            // id not found: create a new family
            Family* family = new Family(*(pMesh->mFamilies[i]));
            mesh->mFamilies.push_back(family);
            mesh->mFamIdToFam.insert(make_pair(family->getId(), family));
        }
    }
    
    // fill families with elements and build groups
    //mesh->finalizeFamiliesAndGroups();
    
    //---------------------------------------------------------------------
    // Merge fields
    //---------------------------------------------------------------------
    // for each field
    set<string> newSetOfGauss;
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* field = mFields[itField]->merge(pMesh->mFields[itField]);
        mesh->mFields.push_back(field);
        field->getSetOfGaussLoc(newSetOfGauss);
    }    
    
    //---------------------------------------------------------------------
    // Merge Gauss infos
    //---------------------------------------------------------------------    
    // WARNING: assume Gauss infos are the same for the two meshes.    
    if (mGaussLoc.size() != pMesh->mGaussLoc.size()) throw IllegalStateException("gauss localization should be the same", __FILE__, __LINE__);
    for (unsigned i = 0 ; i < mGaussLoc.size() ; i++)
    {
        GaussLoc* copyGaussLoc = new GaussLoc(*(mGaussLoc[i]));
        mesh->mGaussLoc.push_back(copyGaussLoc);
        mesh->mGaussLocNameToGaussLoc.insert(make_pair(copyGaussLoc->getName(), copyGaussLoc));
    }    
    
    //---------------------------------------------------------------------
    // Compute bbox
    //---------------------------------------------------------------------
    //mesh->mNodes->getBBox(mesh->mMeshBBoxMin, mesh->mMeshBBoxMax);
    
    return mesh;
}


Mesh* Mesh::mergePartial(vector<Mesh*> pMeshes, const char* pFieldName, int pFieldIt)
{   
    if (pMeshes.size() == 0) throw IllegalArgumentException("list must contain one mesh", __FILE__, __LINE__);
    
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
    vector<Elements*> elements;
    vector<int>       offsets;
    
    int offset = mNodes->getNumberOfNodes();
    offsets.push_back(offset);
    
    
    for (unsigned i = 0 ; i < pMeshes.size() ; i++)
    {
        if (mMeshDim != pMeshes[i]->mMeshDim) throw IllegalStateException("meshes should have same dimension", __FILE__, __LINE__);
        if (mMeshType != pMeshes[i]->mMeshType) throw IllegalStateException("meshes should have same type", __FILE__, __LINE__);
        
        nodes.push_back(pMeshes[i]->mNodes);
        //cout << *(pMeshes[i]->mNodes) << endl;
        elements.push_back(pMeshes[i]->mElements);
        offset += pMeshes[i]->mNodes->getNumberOfNodes();
        offsets.push_back(offset);
    }
    
    mesh->mNodes    = mNodes->mergePartial(nodes);
    //cout << *(mesh->mNodes) << endl;
    mesh->mElements = mElements->mergePartial(elements, offsets);
    
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
    
    // fill families with elements and build groups
    //mesh->finalizeFamiliesAndGroups();
    
    //---------------------------------------------------------------------
    // Merge fields
    //---------------------------------------------------------------------
    // for each field
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        if (pFieldName == NULL) // merge all field
        {
            vector<Field*> fields;
            for (unsigned i = 0 ; i < pMeshes.size() ; i++)
            {
                fields.push_back(pMeshes[i]->mFields[itField]);
            }
            
            Field* field = mFields[itField]->merge(fields);
            mesh->mFields.push_back(field);
        }
        else if (strcmp(mFields[itField]->getName(), pFieldName) == 0) // only merge the given field
        {   
            vector<Field*> fields;
            for (unsigned i = 0 ; i < pMeshes.size() ; i++)
            {
                fields.push_back(pMeshes[i]->mFields[itField]);
            }                    
            
            Field* field = mFields[itField]->merge(fields, pFieldIt);
            mesh->mFields.push_back(field);
        }
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
    
    //---------------------------------------------------------------------
    // Compute bbox
    //---------------------------------------------------------------------
    //mesh->mNodes->getBBox(mesh->mMeshBBoxMin, mesh->mMeshBBoxMax);
    
    return mesh;
}


MeshDis* Mesh::splitGroupsOfElements()
{
    MeshDis* meshDis = new MeshDis();
    meshDis->setSequentialMEDFilename(mMEDfilename);
    
    // get prefix from the original MED filename
    string strPrefix = removeExtension(mMEDfilename, ".med");
    
    int numGroup = 1;
    
    // for each group
    for (unsigned itGroup = 0 ; itGroup < mGroups.size() ; itGroup++)
    {
        Group* currentGroup = mGroups[itGroup];
        
        // skip this group if it is a group of nodes
        if (currentGroup->isGroupOfNodes()) 
        {
            continue;
        }
        
        char strPartName[256];
        sprintf(strPartName, "%s_%d", mMeshName, numGroup);
        
        char strMEDfilename[256];
        sprintf(strMEDfilename, "%s_grain%d.med", strPrefix.c_str(), numGroup);
        
        Mesh* mesh = createFromGroup(currentGroup, mMeshName);
        
        // skip the group which contain all the others groups, even it contains only 1 group
        if ((mesh->mElements->getNumberOfElements() == mElements->getNumberOfElements()) && (mGroups.size() > 1))
        {
            delete mesh;
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



void Mesh::getAllPointsOfField(Field* pField, int pTimeStepIt, std::vector<PointOfField>& pPoints)
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
            // collect coordinates and value of the point
            const med_float* coo = mNodes->getCoordinates(itNode);
            
            const med_float* val = 
                reinterpret_cast<const med_float*>(pField->getValue(pTimeStepIt, itNode + 1));

            // add new point
            pPoints.push_back(PointOfField(coo[0], coo[1], coo[2], val[0]));
        }
    }
    else
    {
        //-------------------------------------------------------------
        // Case 2: field of elements
        //-------------------------------------------------------------
    
        if (mElements == NULL) throw IllegalStateException("no elements in the current mesh", __FILE__, __LINE__);
        if (mElements->getTypeOfPrimitives() != MED_TETRA10) throw UnsupportedOperationException("only support TETRA10 mesh", __FILE__, __LINE__);
        
        const string& nameGaussLoc = pField->getNameGaussLoc(pTimeStepIt);
        GaussLoc* gaussLoc = getGaussLocByName(nameGaussLoc.c_str());
        if (gaussLoc == NULL) throw IllegalStateException("no Gauss localization for these elements", __FILE__, __LINE__);
        
        int numGauss = pField->getNumberOfGaussPointsByElement(pTimeStepIt);
        
        int size = gaussLoc->getDim() * gaussLoc->getNumGaussPoints();
        med_float* cooGaussPts = new med_float[size];
        
        int dim = mElements->getTypeOfPrimitives() / 100;
        int numNodes = mElements->getTypeOfPrimitives() % 100;
        size = dim * numNodes;
        med_float* cooNodes = new med_float[size];
        
        // for each elements
        for (int itElt = 0, size = mElements->getNumberOfElements() ; itElt < size ; itElt++)
        {
            // get coordinates of nodes of the current elements
            // OPTIMIZATION: ASSUME TETRA10: ONLY GETS THE 4 FIRST NODES OF EACH ELEMENT
            mElements->getCoordinates(itElt, mNodes, cooNodes, 4);
            
            // compute coordinates of gauss points
            gaussLoc->getCoordGaussPoints(cooNodes, cooGaussPts);
            
            //printArray2D(cooGaussPts, 5, 3, "Gauss pt"); // debug
            
            const med_float* val = 
                reinterpret_cast<const med_float*>(pField->getValue(pTimeStepIt, itElt + 1));
        
            // for each point of Gauss of the element
            med_float* srcCoo = cooGaussPts;
            for (int itPtGauss = 0 ; itPtGauss < numGauss ; itPtGauss++)
            {
                pPoints.push_back(PointOfField(srcCoo[0], srcCoo[1], srcCoo[2], val[itPtGauss]));
                srcCoo += 3;
            }
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

  // get nunmber of gauss points in the field
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


med_geometrie_element CELL_TYPES[MED_NBR_GEOMETRIE_MAILLE] = 
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
    MED_PENTA15
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


void Mesh::readSequentialMED(const char* pMEDfilename, const char* pMeshName)
{    
    reset();
    
    //---------------------------------------------------------------------
    // Check arguments
    //---------------------------------------------------------------------
    MULTIPR_LOG("Check arguments: ");
    if (pMEDfilename == NULL) throw NullArgumentException("pMEDfilename should not be NULL", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("pMeshName should not be NULL", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
    
    strncpy(mMEDfilename, pMEDfilename, 256);
    strncpy(mMeshName, pMeshName, MED_TAILLE_NOM);
    
    //---------------------------------------------------------------------
    // Open MED file (READ_ONLY)
    //---------------------------------------------------------------------
    MULTIPR_LOG("Open MED file: ");
    mMEDfile = MEDouvrir(mMEDfilename, MED_LECTURE); // open MED file for reading
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
    
    //---------------------------------------------------------------------
    // Check that there is no profil
    //---------------------------------------------------------------------
    MULTIPR_LOG("#profils must be 0: ");
    med_int nbProfils = MEDnProfil(mMEDfile);
    if (nbProfils != 0) throw UnsupportedOperationException("not yet implemented", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
    
    //---------------------------------------------------------------------
    // Read all Gauss localizations
    //---------------------------------------------------------------------
    readGaussLoc();
    
    //---------------------------------------------------------------------
    // Read scalars (should be 0)
    //---------------------------------------------------------------------
    MULTIPR_LOG("Scalars: ");
    med_int nbScalars = MEDnScalaire(mMEDfile);
    if (nbScalars != 0) throw UnsupportedOperationException("not yet implemented", __FILE__, __LINE__);
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
    
    // mesh must only contain TETRA10 elements
    MULTIPR_LOG("Only TETRA10: ");
    med_connectivite connectivite = MED_NOD; // NODAL CONNECTIVITY ONLY
    bool onlyTETRA10 = true;
    int numTetra10 = -1;
    for (int itCell = 0 ; itCell < MED_NBR_GEOMETRIE_MAILLE ; itCell++)
    {
        med_int meshNumCells = MEDnEntMaa(
            mMEDfile, 
            mMeshName, 
            MED_CONN, 
            MED_MAILLE, 
            CELL_TYPES[itCell], 
            connectivite);
        
        if ((meshNumCells > 0) && (strcmp(CELL_NAMES[itCell], "MED_TETRA10") != 0))
        {
            onlyTETRA10 = false;
            break;
        }
        if (strcmp(CELL_NAMES[itCell], "MED_TETRA10") == 0)
        {
            numTetra10 = meshNumCells;
        }
    }
    
    if (!onlyTETRA10) throw UnsupportedOperationException("mesh should only contain TETRA10 elements", __FILE__, __LINE__);
    MULTIPR_LOG(numTetra10 << ": OK\n");
    
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
    
    //---------------------------------------------------------------------
    // Read elements
    //---------------------------------------------------------------------
    mElements = new Elements();
    mElements->readMED(mMEDfile, mMeshName, mMeshDim, MED_MAILLE, MED_TETRA10);
    
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
        readFields();
    }
    
    //---------------------------------------------------------------------
    // Close the MED file
    //---------------------------------------------------------------------
    MULTIPR_LOG("Close MED file: ");
    ret = MEDfermer(mMEDfile);
    if (ret != 0) throw IOException("i/o error while closing MED file", __FILE__, __LINE__);
    MULTIPR_LOG("OK\n");
}


void Mesh::writeMED(const char* pMEDfilename)
{
    MULTIPR_LOG("Write MED: " << pMEDfilename << endl);

    if (pMEDfilename == NULL) throw NullArgumentException("pMEDfilename should not be NULL", __FILE__, __LINE__);
    if (strlen(pMEDfilename) == 0) throw IllegalArgumentException("pMEDfilename size is 0", __FILE__, __LINE__);
    
    remove(pMEDfilename);
    
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
        mMeshName,
        mMeshDim,
        MED_NON_STRUCTURE,
        mMeshDesc);
    
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    MULTIPR_LOG("    Create mesh: |" << mMeshName << "|: OK" << endl);
    
    //---------------------------------------------------------------------
    // Write nodes and elements (mesh must exist)
    //---------------------------------------------------------------------
    if (mNodes == NULL) throw IllegalStateException("mNodes should not be NULL", __FILE__, __LINE__);
    mNodes->writeMED(newMEDfile, mMeshName);
    MULTIPR_LOG("    Write nodes: ok" << endl);
    
    if (mElements == NULL) throw IllegalStateException("mElements should not be NULL", __FILE__, __LINE__);
    mElements->writeMED(newMEDfile, mMeshName, mMeshDim);
    MULTIPR_LOG("    write elt: ok" << endl);
    
    //---------------------------------------------------------------------
    // Write families (mesh must exist)
    //---------------------------------------------------------------------
    for (unsigned itFam = 0 ; itFam < mFamilies.size() ; itFam++)
    {
        Family* fam = mFamilies[itFam];
        fam->writeMED(newMEDfile, mMeshName);
    }
    MULTIPR_LOG("    Write families: ok" << endl);
    
    //---------------------------------------------------------------------
    // Write profil
    //---------------------------------------------------------------------
    // no profil
    
    //---------------------------------------------------------------------
    // Write Gauss localization (must be written before fields)
    //---------------------------------------------------------------------
    for (unsigned itGaussLoc = 0 ; itGaussLoc < mGaussLoc.size() ; itGaussLoc++)
    {
        
        GaussLoc* gaussLoc = mGaussLoc[itGaussLoc];
        gaussLoc->writeMED(newMEDfile);
    }
    MULTIPR_LOG("    Write Gauss: ok" << endl);
    
    //---------------------------------------------------------------------
    // Write fields
    //---------------------------------------------------------------------
    for (unsigned itField = 0 ; itField < mFields.size() ; itField++)
    {
        Field* field = mFields[itField];
        field->writeMED(newMEDfile, mMeshName);
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
    
    for (int itFam = 1 ; itFam <= numFamilies ; itFam++)
    {
        Family* fam = new Family();
        fam->readMED(mMEDfile, mMeshName, itFam);
        mFamilies.push_back(fam);
    }
}


void Mesh::finalizeFamiliesAndGroups()
{
    //---------------------------------------------------------------------
    // Build mapping between family id and pointers towards families
    //---------------------------------------------------------------------
    for (unsigned itFam = 0 ; itFam < mFamilies.size() ; itFam++)
    {
        Family* fam  = mFamilies[itFam];
        mFamIdToFam.insert(make_pair(fam->getId(), fam));
    }
    
    //---------------------------------------------------------------------
    // Fill families of nodes
    //---------------------------------------------------------------------
    for (int itNode = 1 ; itNode <= mNodes->getNumberOfNodes() ; itNode++)
    {
        // get family of the ith nodes
        int famIdent = mNodes->getFamIdent(itNode - 1); // MED nodes start at 1
        map<med_int, Family*>::iterator itFam = mFamIdToFam.find(famIdent);
        
        if (itFam == mFamIdToFam.end()) throw IllegalStateException("", __FILE__, __LINE__);
        
        Family* fam = (*itFam).second;
        
        // insert the current node to its family
        fam->insertElt(itNode);
        fam->setIsFamilyOfNodes(true);
    }
    
    //---------------------------------------------------------------------
    // Fill families of elements
    //---------------------------------------------------------------------
    for (int itElt = 1 ; itElt <= mElements->getNumberOfElements() ; itElt++)
    {
        // get family of the ith element (MED index start at 1)
        int famIdent = mElements->getFamilyIdentifier(itElt - 1);
        map<med_int, Family*>::iterator itFam = mFamIdToFam.find(famIdent);
        
        if (itFam == mFamIdToFam.end()) throw IllegalStateException("", __FILE__, __LINE__);
        
        Family* fam = (*itFam).second;
        
        // insert the current node its family
        fam->insertElt(itElt);
        fam->setIsFamilyOfNodes(false);
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
        Field* field = new Field();
        field->readMED(mMEDfile, itField, mMeshName);
        
        // if the nth field does not apply on our mesh => slip it
        if (field->isEmpty())
        {
            delete field;
        }
        else
        {
            mFields.push_back(field);
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
    
    if (pM.mFlagPrintAll)
    {
        cout << (*(pM.mNodes)) << endl;
        cout << (*(pM.mElements)) << endl;
        
        pOs << "    Families : #=" << pM.mFamilies.size() << endl;
        for (unsigned i = 0 ; i < pM.mFamilies.size() ; i++)
        {
            cout << (*(pM.mFamilies[i])) << endl;
        }
        
        pOs << "    Groups   : #=" << pM.mGroups.size() << endl;
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
        
        if (pM.mElements != NULL)
        {
            const set<med_int>& setOfNodes = pM.mElements->getSetOfNodes();
            if (setOfNodes.size() == 0)
            {
                pOs << "    Elt      : #=" << pM.mElements->getNumberOfElements() << endl;
            }
            else
            {
                set<med_int>::iterator itNode = setOfNodes.end();
                itNode--;
                pOs << "    Elt      : #=" << pM.mElements->getNumberOfElements() << " node_id_min=" << (*(setOfNodes.begin())) << " node_id_max=" << (*itNode) << endl;
            }
        }
        
        pOs << "    Families : #=" << pM.mFamilies.size() << endl;
        pOs << "    Groups   : #=" << pM.mGroups.size() << endl;
        pOs << "    Gauss loc: #=" << pM.mGaussLoc.size() << endl;
        pOs << "    Fields   : #=" << pM.mFields.size() << endl;
    }
    
    return pOs;
}


} // namespace  multipr

// EOF
