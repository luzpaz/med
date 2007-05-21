// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_Obj.cxx
 *
 * \brief   see MULTIPR_Obj.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Obj.hxx"
#include "MULTIPR_Exceptions.hxx"
#include "MULTIPR_Mesh.hxx"
#include "MULTIPR_MeshDis.hxx"
#include "MULTIPR_Utils.hxx"
#include "MED_Factory.hxx"

#include <stdio.h>
#include <iostream>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class Obj implementation
//*****************************************************************************

Obj::Obj() 
{
    mMeshDis = NULL;
    
    reset(); 
}


Obj::~Obj()  
{ 
    reset();  
}


void Obj::reset() 
{ 
    mMEDfilename     = "";
    mMeshName        = "";
    mState           = MULTIPR_OBJ_STATE_RESET;
    
    if (mMeshDis != NULL) { delete mMeshDis; mMeshDis = NULL; }
}


void Obj::create(const char* pMEDfilename)
{
    if (pMEDfilename == NULL) throw multipr::NullArgumentException("file name must not be NULL", __FILE__, __LINE__);
    
    // reset everything before associating a new MED file to this object
    reset();
    
    mMEDfilename = pMEDfilename;
    mState = MULTIPR_OBJ_STATE_ERROR;
    
    // check if file exists
    FILE* f = fopen(pMEDfilename, "rb");
    if (f == 0) 
    {
        // file does not exist
        mState = MULTIPR_OBJ_STATE_ERROR;
        throw FileNotFoundException("file not found", __FILE__, __LINE__);
    }
    fclose(f);

    // test whether it is a sequential MED file or a distributed MED file
    MED::EVersion aVersion = MED::GetVersionId(pMEDfilename, true);
    if (aVersion != MED::eVUnknown)
    {
        mState = MULTIPR_OBJ_STATE_SEQ_INIT;
        cout << "Sequential MED file " << pMEDfilename << " has been successfuly opened" << endl;
    }
    else
    {
        // CASE 2: distributed MED file?
        try
        {
            mMeshDis = new multipr::MeshDis();
            mMeshDis->readDistributedMED(pMEDfilename);
        
            mState = MULTIPR_OBJ_STATE_DIS;
            cout << "Distributed MED file " << pMEDfilename << " has been successfuly opened" << endl;
        }
        catch (...)
        {
            // neither a sequential MED file, nor a distributed MED file => error
            mState = MULTIPR_OBJ_STATE_ERROR;
            throw IOException("file is nor a sequential MED file, neither a distributed MED file", __FILE__, __LINE__);
        }
    }
}


std::string Obj::getSequentialMEDFilename() const
{
    // partitionneGrain() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS_MEM) &&
        (mState != MULTIPR_OBJ_STATE_DIS)) throw IllegalStateException("unexpected operation", __FILE__, __LINE__);
    
        return mMeshDis->getSequentialMEDFilename();
}


void Obj::setMesh(const char* pMeshName)
{
    // setMesh() is only available for sequential MED file (not distributed MED file)
    if ((mState != MULTIPR_OBJ_STATE_SEQ_INIT) &&
        (mState != MULTIPR_OBJ_STATE_SEQ)) throw IllegalStateException("expected a sequential MED file", __FILE__, __LINE__);
    
    mMeshName = pMeshName;
    
    // change state to MULTIPR_OBJ_STATE_SEQ (in case of state=MULTIPR_OBJ_STATE_SEQ_INIT)
    mState = MULTIPR_OBJ_STATE_SEQ;
}


vector<string> Obj::getMeshes() const
{
    // test whether it is a sequential MED file or a distributed MED file
    if ((mState == MULTIPR_OBJ_STATE_SEQ_INIT) ||
        (mState == MULTIPR_OBJ_STATE_SEQ))
    {
        // CASE 1: sequential MED file
        return multipr::getListMeshes(mMEDfilename.c_str());
    }
    else
    {
        // CASE 2: distributed MED file
        if (mMeshDis == NULL)
        {
            throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
        }
        
        return mMeshDis->getMeshes();
    }
}


vector<string> Obj::getFields() const
{
    // test whether it is a sequential MED file or a distributed MED file
    if ((mState == MULTIPR_OBJ_STATE_SEQ_INIT) ||
        (mState == MULTIPR_OBJ_STATE_SEQ)) 
    {
        // CASE 1: sequential MED file
        vector<pair<string, int> > tmp = multipr::getListScalarFields(mMEDfilename.c_str());
        
        vector<string> res;
        for (unsigned i = 0 ; i < tmp.size() ; i++)
        {
            res.push_back(tmp[i].first);
        }
        
        return res;
    }
    else
    {
        // CASE 2: distributed MED file
        if (mMeshDis == NULL) throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
        
        return mMeshDis->getFields();
    }
}


int Obj::getTimeStamps(const char* pFieldName) const
{
    // test whether it is a sequential MED file or a distributed MED file
    if ((mState == MULTIPR_OBJ_STATE_SEQ_INIT) ||
        (mState == MULTIPR_OBJ_STATE_SEQ)) 
    {
        // CASE 1: sequential MED file
        vector<pair<string, int> > tmp = multipr::getListScalarFields(mMEDfilename.c_str());
    
        for (unsigned i = 0 ; i < tmp.size() ; i++)
        {
            if (strcmp(tmp[i].first.c_str(), pFieldName) == 0)
            {
                return tmp[i].second;
            }
        }
        
        // pFieldName not found in the list of fields
        return 0;
    }
    else
    {
        // CASE 2: distributed MED file
        if (mMeshDis == NULL) throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
        
        return mMeshDis->getTimeStamps(pFieldName);
    }
}


vector<string> Obj::getParts() const
{
    // getParts() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS) &&
        (mState != MULTIPR_OBJ_STATE_DIS_MEM)) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    
    return getListParts();
}


string Obj::getPartInfo(const char* pPartName) const
{
    // getParts() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS) &&
        (mState != MULTIPR_OBJ_STATE_DIS_MEM)) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    
    if (mMeshDis == NULL) throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
    
    return mMeshDis->getPartInfo(pPartName);
    
}


void Obj::removeParts(const char* pPrefixPartName)
{
    // removePart() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS) &&
        (mState != MULTIPR_OBJ_STATE_DIS_MEM)) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
        
    if (mMeshDis == NULL) throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
    
    mMeshDis->removeParts(pPrefixPartName);
    mState = MULTIPR_OBJ_STATE_DIS_MEM;
}


vector<string> Obj::partitionneDomaine()
{
    if (mState == MULTIPR_OBJ_STATE_SEQ_INIT) throw IllegalStateException("use setMesh() before", __FILE__, __LINE__);
    
    // partitionneDomaine() is only available for sequential MED file (not distributed MED file)
    if (mState != MULTIPR_OBJ_STATE_SEQ) throw IllegalStateException("unexpected operation", __FILE__, __LINE__);
    
    //-------------------------------------------------------------
    // Read the sequential mesh
    //-------------------------------------------------------------
    cout << "Read sequential MED file: " << mMEDfilename << ": please wait... " << endl;
    
    Mesh mesh;
    mesh.readSequentialMED(mMEDfilename.c_str(), mMeshName.c_str());
    cout << mesh << endl;
    
    //-------------------------------------------------------------
    // Build distributed mesh from groups
    //-------------------------------------------------------------
    cout << "Build distributed mesh: please wait... " << endl;
    try
    {
        mMeshDis = mesh.splitGroupsOfElements();
    }
    catch (RuntimeException& e)
    {
        delete mMeshDis;
        mMeshDis = NULL;
        throw e;
    }
    
    mState = MULTIPR_OBJ_STATE_DIS_MEM;
    
    return getListParts();
}


vector<string> Obj::partitionneGrain(
    const char* pPartName, 
    int         pNbParts, 
    int         pPartitionner)
{
    if (mMeshDis == NULL) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    if (pPartName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pNbParts < 2) throw IllegalArgumentException("", __FILE__, __LINE__);
    if ((pPartitionner != 0) && (pPartitionner != 1)) throw IllegalArgumentException("partitionner should be 0=METIS or 1=SCOTCH", __FILE__, __LINE__);

    // partitionneGrain() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS_MEM) &&
        (mState != MULTIPR_OBJ_STATE_DIS)) throw IllegalStateException("unexpected operation", __FILE__, __LINE__);
    
    // if distributed MED file is currently in memory, then write to disk before performing partitionneGrain()
    // (because MEDSPLIITER starts from a file)
    if (mState == MULTIPR_OBJ_STATE_DIS_MEM)
    {
        //-----------------------------------------------------
        // Write distributed mesh
        //-----------------------------------------------------
        cout << "Write distributed mesh: please wait... " << endl;
        string strPrefix = removeExtension(mMEDfilename.c_str(), ".med");
        mMeshDis->writeDistributedMED(strPrefix.c_str());
        
        mMEDfilename = mMeshDis->getDistributedMEDFilename();
        
        delete mMeshDis;
        
        //---------------------------------------------------------------------
        // Read the distributed mesh
        //---------------------------------------------------------------------    
        int ret = MEDformatConforme(mMEDfilename.c_str());
        if (ret == 0) throw IOException("waiting for a distributed MED file (not a sequential one)", __FILE__, __LINE__);
    
        mMeshDis = new MeshDis();
        mMeshDis->readDistributedMED(mMEDfilename.c_str());
        
        mState = MULTIPR_OBJ_STATE_DIS;
    }

    //---------------------------------------------------------------------
    // Split the given part (pGroupName)
    //---------------------------------------------------------------------
    mMeshDis->splitPart(pPartName, pNbParts, pPartitionner);
    cout << mMeshDis << endl;

    //---------------------------------------------------------------------
    // Write new distributed mesh
    //---------------------------------------------------------------------
    string strPrefix = multipr::removeExtension(mMEDfilename.c_str(), ".med");
    mMeshDis->writeDistributedMED(strPrefix.c_str());
    
    //---------------------------------------------------------------------
    // Read the distributed mesh
    //---------------------------------------------------------------------
    delete mMeshDis;
    mMeshDis = new MeshDis();
    //cout << "read dis MED file: filename=" << mMEDfilename << endl;
    mMeshDis->readDistributedMED(mMEDfilename.c_str());
    
    return getListParts();
}


vector<string> Obj::decimePartition(
    const char* pPartName,
    const char* pFieldName,
    int         pFieldIt,
    const char* pFilterName,
    double      pTmed,
    double      pTlow,
    double      pRadius,
    int         pBoxing)
{
    
    // decimePartition() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS_MEM) &&
        (mState != MULTIPR_OBJ_STATE_DIS)) throw IllegalStateException("unexpected operation", __FILE__, __LINE__);
        
    if (mMeshDis == NULL) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Decimate
    //---------------------------------------------------------------------
    mMeshDis->decimatePart(
        pPartName, 
        pFieldName,
        pFieldIt,
        pFilterName,
        pTmed,
        pTlow,
        pRadius,
        pBoxing);
        
    mState = MULTIPR_OBJ_STATE_DIS_MEM;
    
    return getListParts();
}


string Obj::evalDecimationParams(
    const char* pPartName, 
    const char* pFieldName, 
    int         pFieldIt, 
    const char* pFilterName,
    const char* pFilterParams)
{
    // decimePartition() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS_MEM) &&
        (mState != MULTIPR_OBJ_STATE_DIS)) throw IllegalStateException("unexpected operation", __FILE__, __LINE__);
        
    if (mMeshDis == NULL) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    
    string res = mMeshDis->evalDecimationParams(
        pPartName,
        pFieldName,
        pFieldIt,
        pFilterName,
        pFilterParams);
    
    return res;
}


vector<string> Obj::getListParts() const
{
    if (mMeshDis == NULL) throw IllegalStateException("not a distributed mesh", __FILE__, __LINE__);
    
    vector<string> names;
    
    int numParts = mMeshDis->getNumParts();
    for (int i = 0 ; i < numParts ; i++)
    {
        names.push_back( mMeshDis->getPart(i)->getPartName() );
    }
    
    return names;
}


void Obj::save(const char* pPath)
{
    static string prevPath = "";
    
    // only save file if it is a distributed MED file currently in memory or path has changed
    if ((mState == MULTIPR_OBJ_STATE_DIS_MEM) || (strcmp(pPath, prevPath.c_str()) != 0))
    {
        prevPath = pPath;
        
        //-------------------------------------------------------------
        // Write new distributed mesh
        //-------------------------------------------------------------
        string filename;
        if (strlen(pPath) == 0)
        {
            filename = multipr::removeExtension(mMEDfilename.c_str(), ".med");
        }
        else
        {
            filename = string(pPath) + string("/") + multipr::getFilenameWithoutPath(mMEDfilename.c_str());
        }
        string strPrefix = multipr::removeExtension(filename.c_str(), ".med");
        mMeshDis->writeDistributedMED(strPrefix.c_str());
        mMEDfilename = mMeshDis->getDistributedMEDFilename();
        
        cout << "Write MED master file: " << mMEDfilename << ": OK" << endl;
        
        //-------------------------------------------------------------
        // Read the distributed mesh
        //-------------------------------------------------------------
        delete mMeshDis;
        mMeshDis = new MeshDis();
        mMeshDis->readDistributedMED(mMEDfilename.c_str());
        
        mState = MULTIPR_OBJ_STATE_DIS;
    }
}


ostream& operator<<(ostream& pOs, Obj& pO)
{
    pOs << "Obj:" << endl;
    pOs << "    Name:" << pO.mMEDfilename << endl;
    
    return pOs;
}


} // namespace multipr

// EOF
