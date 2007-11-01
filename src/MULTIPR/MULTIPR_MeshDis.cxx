// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_MeshDis.cxx
 *
 * \brief   see MULTIPR_MeshDis.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_MeshDis.hxx"
#include "MULTIPR_Mesh.hxx"
#include "MULTIPR_DecimationFilter.hxx"
#include "MULTIPR_Utils.hxx"
#include "MULTIPR_Globals.hxx"
#include "MULTIPR_API.hxx"
#include "MULTIPR_Exceptions.hxx"
#include "MULTIPR_ProgressCallback.hxx"
#include "MULTIPR_Field.hxx"
#include "MULTIPR_Profil.hxx"

#include "MEDSPLITTER_API.hxx"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

// Standard includes for decimation statistics.
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>


using namespace std;


namespace multipr
{

//*****************************************************************************
// Global variables (exported)
//*****************************************************************************

// callback used to report progress about a long task (e.g. save to disk)
MULTIPR_ProgressCallback* gProgressCallback = NULL;

// callback used to report empty meshes
MULTIPR_EmptyMeshCallback* gEmptyMeshCallback = NULL;


//*****************************************************************************
// Class MeshDisEntry implementation
//*****************************************************************************

MeshDisPart::MeshDisPart()
{
    mMesh             = NULL;
    mCollection       = NULL;
    mOldCollection    = NULL;
    
    reset();
}


MeshDisPart::~MeshDisPart()
{
    reset();
}


void MeshDisPart::reset()
{
    mToDoOnNextWrite = MULTIPR_UNDEFINED;
    
    mMeshName[0]     = '\0';
    mId              = 0;
    mPartName[0]     = '\0';
    mPath[0]         = '\0';
    mMEDFileName[0]  = '\0';
    
    if (mMesh != NULL)
    {
        delete mMesh;
        mMesh = NULL;
    }
    
    mSplit           = 0;
    
    if (mCollection != NULL)
    {
        delete mCollection;
        mCollection = NULL;
    }
    
    if (mOldCollection != NULL)
    {
        delete mOldCollection;
        mOldCollection = NULL;
    }
}


const char* MeshDisPart::getMEDFileNameSuffix() const
{    
    // Examples:
    //    "agregat100grains_12pas_groupe97.med"                      -> "groupe97"
    //    "agregat100grains_12pas_groupe100_part2.med"               -> "groupe100_part2"
    //    "agregat100grains_12pas_groupe98_gradmoy-low-25.0-0.3.med" -> "groupe98_gradmoy-low-25-0.3"
    
    string prefix = removeExtension(mMEDFileName, ".med");
    prefix.erase(0, prefix.rfind("groupe"));
    return prefix.c_str();
}


void MeshDisPart::create(
        OnNextWrite pToDoOnNextWrite,
        const char* pMeshName, 
        int         pId,
        const char* pPartName,
        const char* pPath,
        const char* pMEDFileName,
        Mesh*       pMesh)
{
    if (pToDoOnNextWrite == MULTIPR_UNDEFINED) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pId < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (pPartName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pPath == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pMEDFileName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    reset();
    
    mToDoOnNextWrite = pToDoOnNextWrite;
    strcpy(mMeshName, pMeshName);
    mId = pId;
    strcpy(mPartName, pPartName);
    strcpy(mPath, pPath);
    strcpy(mMEDFileName, pMEDFileName);
    mMesh = pMesh;
}


void MeshDisPart::readMED()
{
    if (mMesh != NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mCollection != NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mOldCollection != NULL) throw IllegalStateException("", __FILE__, __LINE__);
    
    mMesh = new Mesh();
    mMesh->readSequentialMED(mMEDFileName, mMeshName);
}


ostream& operator<<(ostream& pOs, MeshDisPart& pM)
{
    switch (pM.mToDoOnNextWrite)
    {
        case MeshDisPart::MULTIPR_UNDEFINED: 
            pOs << "undefined"; 
            break;
            
        case MeshDisPart::MULTIPR_KEEP_AS_IT: 
            pOs << pM.mMeshName << " " << pM.mId << " " << pM.mPartName << " " << pM.mPath << " " << pM.mMEDFileName;
            break;
            
        case MeshDisPart::MULTIPR_WRITE_MESH:
            pOs << pM.mMeshName << " " << pM.mId << " " << pM.mPartName << " " << pM.mPath << " " << pM.mMEDFileName;
            break;
            
        case MeshDisPart::MULTIPR_WRITE_PARTS:
            pOs << pM.mMeshName << " " << pM.mId << " " << pM.mPartName << " " << pM.mPath << " " << pM.mMEDFileName << " SPLIT " << pM.mSplit;
            break;
        
        default: throw IllegalStateException("", __FILE__, __LINE__);
    }
    
    return pOs;
}

//*****************************************************************************
// Class TLockProxy implementation
//*****************************************************************************
/*
TLockProxy::TLockProxy (MeshDis* theMeshDis, boost::shared_ptr<boost::mutex> theMutex)
  : myMeshDis(theMeshDis),
    myMutex(theMutex)
{
  //boost::detail::thread::lock_ops<MeshDis::TMutex>::lock(myMeshDis->myMutex);
  boost::detail::thread::lock_ops<boost::mutex>::lock(*myMutex);
}

TLockProxy::~TLockProxy()
{
  //boost::detail::thread::lock_ops<MeshDis::TMutex>::unlock(myMeshDis->myMutex);
  boost::detail::thread::lock_ops<boost::mutex>::unlock(*myMutex);
}

MeshDis* TLockProxy::operator-> () const // never throws
{
  return myMeshDis;
}
*/
//*****************************************************************************
// Class MeshDis implementation
//*****************************************************************************

MeshDis::MeshDis()
{
    reset();
}


MeshDis::~MeshDis()
{
    reset();
}


void MeshDis::reset()
{
    mSequentialMEDFilename[0]  = '\0';
    mDistributedMEDFilename[0] = '\0';
    
    for (unsigned itPart = 0 ; itPart != mParts.size() ; itPart++)
    {
        MeshDisPart* part = mParts[itPart];
        delete part;
    }
    mParts.clear();

    boost::recursive_mutex::scoped_lock aLock (mWriteMutex);
    mWriteProgress = 100;
}


void MeshDis::setSequentialMEDFilename(const char* pFilename) 
{ 
    strcpy(mSequentialMEDFilename, pFilename); 
}


void MeshDis::addMesh(
    MeshDisPart::OnNextWrite pToDoOnNextWrite,
    const char* pMeshName, 
    int         pId,
    const char* pPartName,
    const char* pPath,
    const char* pMEDFileName,
    Mesh*       pMesh)
{
    MeshDisPart* part = new MeshDisPart();
    
    part->create(
        pToDoOnNextWrite,
        pMeshName,
        pId,
        pPartName,
        pPath,
        pMEDFileName,
        pMesh);
    
    mParts.push_back(part);
}


void MeshDis::insertMesh(
    MeshDisPart::OnNextWrite pToDoOnNextWrite,
    const char* pMeshName, 
    int         pId,
    const char* pPartName,
    const char* pPath,
    const char* pMEDFileName,
    Mesh*       pMesh,
    int         pPosition)
{
    MeshDisPart* part = new MeshDisPart();
    
    part->create(
        pToDoOnNextWrite,
        pMeshName,
        pId,
        pPartName,
        pPath,
        pMEDFileName,
        pMesh);
    
    mParts.insert(mParts.begin() + pPosition, part);
    
    // rename id of following parts
    for (unsigned i = pPosition + 1 ; i < mParts.size() ; i++)
    {
        mParts[i]->mId++;
    }
}


void MeshDis::removeParts(const char* pPrefixPartName)
{
    if (pPrefixPartName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    char strPrefix[256];
    sprintf(strPrefix, "%s_", pPrefixPartName);
    
    for (vector<MeshDisPart*>::iterator itPart = mParts.begin() ; itPart != mParts.end() ; itPart++)
    {
        MeshDisPart* currentPart = (*itPart);
        
        // remove part which have the same name and all sub_parts
        // e.g. if pPrefixPartName="PART_4" => remove "PART_4" and "PART_4_*", but not "PART41"        
        if ((strcmp(currentPart->getPartName(), pPrefixPartName) == 0) || 
            startsWith(currentPart->getPartName(), strPrefix))
        {
            mParts.erase(itPart);
        
            // decrement id of following parts
            for (vector<MeshDisPart*>::iterator itPart2 = itPart ; itPart2 != mParts.end() ; itPart2++)
            {
                (*itPart2)->mId--;
            }
            
            itPart--;
            if (currentPart->mMEDFileName != NULL)
            {
                remove(currentPart->mMEDFileName);
            }
            
            delete currentPart;
        }
    }
}


MeshDisPart* MeshDis::findPart(const char* pPartName)
{
    if (pPartName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    MeshDisPart* part = NULL;

    for (unsigned itPart = 0 ; itPart < mParts.size() ; itPart++)
    {
        MeshDisPart* currentPart = mParts[itPart];
        if (strcmp(currentPart->getPartName(), pPartName) == 0)
        {
             part = currentPart;
            break;
        }
    }
    
    return part;
    
}


vector<string> MeshDis::getMeshes() const
{
    vector<string> res;
    
    if (mParts.size() > 0)
    {
        MeshDisPart* part = mParts[0];
        const char* meshName = part->getMeshName();
        res.push_back(meshName);
    }
    
    return res;
}


vector<string> MeshDis::getFields(const char* pPartList,  bool pAddNbGaussPoint) const
{
    vector<string>	res;
    MeshDisPart*	curPart = NULL;
    unsigned		i, pos, len;

    if (mParts.size() == 0)
    {
        return res;
    }

    // get the right part.
    pos = 0;
    len = 0;
    // Get the next separator.
    for (; pPartList[pos+len] && pPartList[pos+len] != '|'; ++len);
    // Find the corresponding part.
    for (i = 0; i < mParts.size(); ++i)
    {
        // Avoid a match between MAIL_9 and MAIL_99
        if (len != strlen(mParts[i]->getPartName()))
        {
            continue;
        }
        if (strncmp(mParts[i]->getPartName(), &pPartList[pos], len) == 0)
        {
            curPart = mParts[i];
        }
    }
    if (curPart == NULL)
    {
        return res;
    }

    // all the parts of the distributed MED file should have the same fields
    // => just return the name of fields of the first part
    switch (curPart->mToDoOnNextWrite)
    {
        case MeshDisPart::MULTIPR_KEEP_AS_IT: 
        case MeshDisPart::MULTIPR_WRITE_PARTS:
        {
            vector<pair<string, int> > tmp;
            multipr::getListScalarFields(curPart->getMEDFileName(), tmp,
                                         pAddNbGaussPoint, curPart->getMeshName());

            for (unsigned i = 0 ; i < tmp.size() ; i++)
            {
                res.push_back(tmp[i].first);
            }
            return res;
        }
            
        case MeshDisPart::MULTIPR_WRITE_MESH:
            return curPart->mMesh->getNameScalarFields();
        
        default: 
            throw IllegalStateException("Don't know what to do now.", __FILE__, __LINE__);
    }
}


int MeshDis::getTimeStamps(const char* pPartList, const char* pFieldName) const
{
    MeshDisPart*	curPart = NULL;
    unsigned		i, pos, len;

    if (mParts.size() == 0)
    {
        // no parts in this distributed MED file => no fields => #iteration = 0
        return 0;
    }

    // get the right part.
    pos = 0;
    len = 0;
    // Get the next separator.
    for (; pPartList[pos+len] && pPartList[pos+len] != '|'; ++len);
    // Find the corresponding part.
    for (i = 0; i < mParts.size(); ++i)
    {
      // Avoid a match between MAIL_9 and MAIL_99
      if (len != strlen(mParts[i]->getPartName()))
      {
        continue;
      }
      if (strncmp(mParts[i]->getPartName(), &pPartList[pos], len) == 0)
      {
        curPart = mParts[i];
      }
    }

    // all the parts of the distributed MED file should have the same fields
    // => just return the number of iteration found in the field of the first part
    switch (mParts[0]->mToDoOnNextWrite)
    {
        case MeshDisPart::MULTIPR_KEEP_AS_IT: 
        case MeshDisPart::MULTIPR_WRITE_PARTS:
        {
            vector<pair<string, int> > tmp;
            multipr::getListScalarFields(curPart->getMEDFileName(), tmp);

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
            
        case MeshDisPart::MULTIPR_WRITE_MESH:
            return mParts[0]->mMesh->getTimeStamps(pFieldName);
        
        default: 
            throw IllegalStateException("", __FILE__, __LINE__);
    }
}


string MeshDis::getPartInfo(const char* pPartName)
{
    MeshDisPart* part = findPart(pPartName);
    
    if (part != NULL)
    {
        char num[16];
        sprintf(num, "%d", part->mId);
        
        string res = 
            string(part->mMeshName) + 
            string(" ") +
            string(num) +
            string(" ") +
            string(part->mPartName) +
            string(" ") +
            string(part->mPath) +
            string(" ") +
            string(part->mMEDFileName);

        return res;
    }
    else
    {
        // part not found => return empty string
        return "";
    }
}


void MeshDis::splitPart(const char* pPartName, int pNbParts, int pPartitionner)
{
    if (pPartName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pNbParts < 2) throw IllegalArgumentException("", __FILE__, __LINE__);
    if ((pPartitionner != MULTIPR_METIS) && (pPartitionner != MULTIPR_SCOTCH))
      throw IllegalArgumentException("should be 0=METIS or 1=SCOTCH", __FILE__, __LINE__);

    //---------------------------------------------------------------------
    // Find the MED file corresponding to the given part
    //---------------------------------------------------------------------
    MeshDisPart* part = findPart(pPartName);
    
    if (part == NULL)
    {
        throw IllegalArgumentException("part not found in this distributed MED file", __FILE__, __LINE__);
    }
    
    //---------------------------------------------------------------------
    // Load the sequential MED file
    //---------------------------------------------------------------------
    MEDSPLITTER::MESHCollection* collection =
      new MEDSPLITTER::MESHCollection(part->getMEDFileName(), part->getMeshName());
    collection->setDriverType(MEDSPLITTER::MedAscii);
    
    //---------------------------------------------------------------------
    // Partition the group
    //---------------------------------------------------------------------
    MEDSPLITTER::Topology* topology;
    if (pPartitionner == MULTIPR_METIS)
    {
        try
        {
            topology = collection->createPartition(pNbParts, MEDSPLITTER::Graph::METIS);
        }
        catch (...)
        {
            throw RuntimeException("MEDSPLITTER error: createPartition(), using METIS", __FILE__, __LINE__);
        }
    }
    else if (pPartitionner == MULTIPR_SCOTCH)
    {
        try
        {
            topology = collection->createPartition(pNbParts, MEDSPLITTER::Graph::SCOTCH);
        }
        catch (...)
        {
            throw RuntimeException("MEDSPLITTER error: createPartition(), using SCOTCH", __FILE__, __LINE__);
        }
    }
    else
    {
        throw IllegalStateException("unknown partitionner", __FILE__, __LINE__);
    }
    
    try
    {
        MEDSPLITTER::MESHCollection* newCollection = new MEDSPLITTER::MESHCollection(*collection, topology);
        newCollection->setDriverType(MEDSPLITTER::MedAscii);

        part->mToDoOnNextWrite = MeshDisPart::MULTIPR_WRITE_PARTS; 
        part->mSplit           = pNbParts;
        part->mOldCollection   = collection;
        part->mCollection      = newCollection;
    }
    catch (...)
    {
        throw RuntimeException("MEDSPLITTER error: new MESHCollection()", __FILE__, __LINE__);
    }
}


std::list<std::string> MeshDis::decimatePart (const char* pPartName, 
                                              const char* pFieldName,
                                              med_int     pFieldIt,
                                              const char* pFilterName,
                                              const char* pFilterParams)
{
    char		stats[512];
    float		lowResStat = 0.0f;
    float		medResStat = 0.0f;
    unsigned	lowResSize = 0;
    unsigned	medResSize = 0;
    struct stat	fileStat;

    fileStat.st_size = 0;

    //---------------------------------------------------------------------
    // Check arguments
    //---------------------------------------------------------------------
    if (pPartName == NULL)
      throw NullArgumentException("partname should not be NULL", __FILE__, __LINE__);
    if (pFieldName == NULL)
      throw NullArgumentException("fieldname should not be NULL", __FILE__, __LINE__);
    if (pFieldIt < med_int(1))
      throw IllegalArgumentException("invalid field iteration; should be >= 1", __FILE__, __LINE__);
    if (pFilterParams == NULL)
      throw IllegalArgumentException("filterparams should not be NULL", __FILE__, __LINE__);

    //---------------------------------------------------------------------
    // Find the MED file corresponding to the given part
    //---------------------------------------------------------------------
    MeshDisPart* part = findPart(pPartName);
    if (part == NULL)
    {
        throw IllegalArgumentException("part not found in the given distributed MED file",
                                       __FILE__, __LINE__);
    }

    //---------------------------------------------------------------------
    // Load the associated sequential MED file
    //---------------------------------------------------------------------
    if (part->mMesh == NULL)
    {
        part->readMED();
    }

    Mesh* meshFull = part->mMesh;
    cout << (*meshFull) << endl;

    std::list<std::string> ret;
    
    const char* originalFilename = part->getMEDFileName();
    string strPrefix = removeExtension(originalFilename, ".med");

    // debug
    //cout << (*this) << endl;

    //---------------------------------------------------------------------
    // Decimates the given mesh
    //---------------------------------------------------------------------
    // arguments for decimation are passed as a string for genericity
    char argv[256];
    char newPartName[MED_TAILLE_NOM + 1];
    char newMEDFileNameLow[256];
    char newMEDFileNameMed[256];
    float lTMed;
    float lTLow;
    float lRadius = 0.0f;
    int lBoxing = 0;

    // *** create a new mesh = MEDIUM resolution ***
    if (strcmp(pFilterName, "Filtre_GradientMoyen") == 0)
    {
      sscanf(pFilterParams, "%f %f %f %d", &lTMed, &lTLow, &lRadius, &lBoxing);
      sprintf(newMEDFileNameMed, "%s_%s-med-%s-%s.med", 
              strPrefix.c_str(), 
              "gradmoy",
              realToString(lTMed).c_str(), 
              realToString(lRadius).c_str());
      sprintf(newMEDFileNameLow, "%s_%s-low-%s-%s.med", 
              strPrefix.c_str(), 
              "gradmoy",
              realToString(lTLow).c_str(), 
              realToString(lRadius).c_str());
    }
    else if (strcmp(pFilterName, "Filtre_Direct") == 0)
    {
      sscanf(pFilterParams, "%f %f", &lTMed, &lTLow);
      sprintf(newMEDFileNameMed, "%s_%s-med-%s.med", 
              strPrefix.c_str(), 
              "direct",
              realToString(lTMed).c_str());
      sprintf(newMEDFileNameLow, "%s_%s-low-%s.med", 
              strPrefix.c_str(), 
              "direct",
              realToString(lTLow).c_str());
    }
    else
    {
      throw IllegalArgumentException("Filter not found !", __FILE__, __LINE__);
    }

    sprintf(argv, "%s %d %f %f %d", pFieldName, pFieldIt, lTMed, lRadius, lBoxing);
    sprintf(newPartName, "%s_MED", pPartName);
    {
      Mesh* meshMedium = meshFull->decimate(pFilterName, argv, part->getMeshName());
      cout << (*meshMedium) << endl;
      if (meshMedium->getNumberOfElements())
      {
        medResStat = 1.0f - (float)meshMedium->getNumberOfElements() / (float)meshFull->getNumberOfElements();

        // We write the file in /tmp to get it's size.
        try {
          meshMedium->writeMED("/tmp/delete-me.med");
          stat("/tmp/delete-me.med", &fileStat);
          medResSize = fileStat.st_size;
          remove("/tmp/delete-me.med");
        }
        catch (...)
        {
          medResSize = 0;
        }

        insertMesh(
                   MeshDisPart::MULTIPR_WRITE_MESH,
                   part->getMeshName(),
                   part->mId + 1,
                   newPartName,
                   "localhost",
                   newMEDFileNameMed,
                   meshMedium,
                   part->mId + 0);
      }
      else
      {
        ret.push_back(newPartName);
        delete meshMedium;
      }
    }

    // *** create a new mesh = LOW resolution ***
    sprintf(argv, "%s %d %lf %lf %d", pFieldName, pFieldIt, lTLow, lRadius, lBoxing);
    sprintf(newPartName, "%s_LOW", pPartName);
    {
      Mesh* meshLow = meshFull->decimate(pFilterName, argv, part->getMeshName());
      cout << (*meshLow) << endl;
      if (meshLow->getNumberOfElements())
      {		
        lowResStat = 1.0f - (float)meshLow->getNumberOfElements() / (float)meshFull->getNumberOfElements();

        // We write the file in /tmp to get it's size.
        try {
          meshLow->writeMED("/tmp/delete-me.med");
          stat("/tmp/delete-me.med", &fileStat);
          lowResSize = fileStat.st_size;
          remove("/tmp/delete-me.med");
        }
        catch (...)
        {
          lowResSize = 0;
        }

        insertMesh(MeshDisPart::MULTIPR_WRITE_MESH,
                   part->getMeshName(),
                   part->mId + 2,
                   newPartName,
                   "localhost",
                   newMEDFileNameLow,
                   meshLow,
                   part->mId + 1);
      }
      else
      {
        ret.push_back(newPartName);
        delete meshLow;
      }
    }
    // We get the original file size.
    std::string filename = part->getMEDFileName();
    stat(filename.c_str(), &fileStat);

    snprintf(stats, 512, "Mesh compression : Low resolution=%.1f\%%. Medium resolution=%.1f%%.\nFile compression :   Low resolution=%.1f%%. Medium resolution=%.1f%%.", 
             lowResStat * 100.0f, medResStat * 100.0f, 
             (1.0f - (float)lowResSize / (float)fileStat.st_size) * 100.0f,
             (1.0f - (float)medResSize / (float)fileStat.st_size) * 100.0f);
    this->mStats = stats;

    return ret;
}


string MeshDis::evalDecimationParams(
    const char* pPartName, 
    const char* pFieldName, 
    int         pFieldIt, 
    const char* pFilterName,
    const char* pFilterParams)
{
    MeshDisPart* part = findPart(pPartName);
    if (part == NULL) 
    {
        return "";
    }
    
    try
    {
        if (part->mMesh == NULL)
        {
            part->readMED();
        }
    
        multipr::DecimationFilter* filter = multipr::DecimationFilter::create(pFilterName);
        if (filter == NULL) 
        {
            return "";
        }
        
        multipr::DecimationFilterGradAvg* filterGrad = dynamic_cast<multipr::DecimationFilterGradAvg*>(filter);
        
        if (filterGrad != NULL)
        {
            int mode;
            
            int ret = sscanf(pFilterParams, "%d", &mode);
            
            // mode 2 = GET RADIUS
            if ((ret == 1) && (mode == 2))
            {
                double radius = part->mMesh->evalDefaultRadius(8);
                char res[256];
                sprintf(res, "%f", radius);
                return res;
            }
            
            float radius;
            int boxing;
            
            ret = sscanf(pFilterParams, "%d %f %d", &mode, &radius, &boxing);
            
            // mode 1 = GET GRADIENT MIN, MAX and AVG
            if ((ret == 3) && (mode == 1))
            {
                double gradMin = 0.1, gradAvg = 0.15, gradMax = 0.2;
                
                filterGrad->getGradientInfo(
                    part->mMesh,
                    pFieldName,
                    pFieldIt,
                    radius,
                    boxing,
                    &gradMin,
                    &gradAvg,
                    &gradMax);
                
                char res[2048];
                sprintf(res, "%f %f %f", gradMin, gradAvg, gradMax);
                return res;
            }
        }
        
        delete filter;
    }
    catch(...)
    {
    }
    
    return "";
}


int MeshDis::computeNumParts()
{
    int numParts = 0;
    
    for (unsigned itPart = 0 ; itPart < mParts.size() ; itPart++)
    {
        switch (mParts[itPart]->mToDoOnNextWrite)
        {
        case MeshDisPart::MULTIPR_KEEP_AS_IT: 
        case MeshDisPart::MULTIPR_WRITE_MESH:
            numParts++;
            break;
        
        case MeshDisPart::MULTIPR_WRITE_PARTS:
            numParts += mParts[itPart]->mSplit + 1;
            break;
        
        default: throw IllegalStateException("", __FILE__, __LINE__);
        }
    }
    
    return numParts;
}


void MeshDis::readDistributedMED(const char* pMEDfilename)
{
    if (pMEDfilename == NULL) throw NullArgumentException("filename should not be NULL", __FILE__, __LINE__);
    
    const int MAX_SIZEOF_LINE = 1024;
    
    reset();
    strcpy(mDistributedMEDFilename, pMEDfilename);
    
    //---------------------------------------------------------------------
    // Open master file (ASCII file)
    //---------------------------------------------------------------------
    ifstream fileMaster(mDistributedMEDFilename);
    if (fileMaster.fail()) throw IOException("i/o error while opening MED master file", __FILE__, __LINE__);

    //---------------------------------------------------------------------
    // Read header
    //---------------------------------------------------------------------
    char charbuffer[MAX_SIZEOF_LINE];
    fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
    if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);
    
    // check format
    if ((charbuffer[0] != '#') ||
        (charbuffer[1] != ' ') ||
        (charbuffer[2] != 'M') ||
        (charbuffer[3] != 'E') ||
        (charbuffer[4] != 'D'))
        throw IOException("not a valid distributed MED file", __FILE__, __LINE__);
        
    while ((charbuffer[0] == '#') || (strlen(charbuffer) == 0))
    {
        char* strTag = NULL;
        if ((charbuffer[0] == '#') && ((strTag = strstr(charbuffer, "[SOURCE]=")) != NULL))
        {
            char strSequentialMEDFilename[256];
            int ret = sscanf(strTag, "[SOURCE]=%s", strSequentialMEDFilename);
            if (ret == 1)
            {
                setSequentialMEDFilename(strSequentialMEDFilename);
            }
        }
        fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
        if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);
    }
    
    // read number of parts
    int nbParts = atoi(charbuffer);
    
    //---------------------------------------------------------------------
    // Read infos about sub-parts
    //---------------------------------------------------------------------
    char   lMeshName[MED_TAILLE_NOM + 1];
    int    lId;
    char   lPartName[MED_TAILLE_NOM + 1];
    char   lPath[256];
    char   lMEDFileName[256];    
    
    for (int i = 0 ; i < nbParts ; i++)
    {
        fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
        if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);
        
        while ((charbuffer[0] == '#') || (strlen(charbuffer) == 0))
        {
            fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
            if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);
        }
        
        lMeshName[0]    = '\0';
        lId             = 0;
        lPartName[0]    = '\0';
        lPath[0]        = '\0';
        lMEDFileName[0] = '\0';
        
        int ret = sscanf(charbuffer, "%s %d %s %s %s", 
            lMeshName,
            &lId,
            lPartName,
            lPath,
            lMEDFileName);
    
        if (ret != 5) throw IOException("i/o error while reading MED master file; bad format", __FILE__, __LINE__);

        //cout << "DBG: read: " << lMeshName << " " << lId << " " << lPartName << endl;
        addMesh(
            MeshDisPart::MULTIPR_KEEP_AS_IT,
            lMeshName,
            lId,
            lPartName,
            lPath,
            lMEDFileName,
            NULL);
    }
    
    //---------------------------------------------------------------------
    // Close master file
    //---------------------------------------------------------------------
    fileMaster.close();
    if (fileMaster.fail()) throw IOException("i/o error while closing MED master file", __FILE__, __LINE__);
}

void MeshDis::readPersistentDistributedMED(const char* pMEDfilename)
{
  if (pMEDfilename == NULL) throw NullArgumentException("filename should not be NULL", __FILE__, __LINE__);

  const int MAX_SIZEOF_LINE = 1024;

  reset();
  strcpy(mDistributedMEDFilename, pMEDfilename);

  //---------------------------------------------------------------------
  // Open master file (ASCII file)
  //---------------------------------------------------------------------
  ifstream fileMaster(mDistributedMEDFilename);
  if (fileMaster.fail()) throw IOException("i/o error while opening MED master file", __FILE__, __LINE__);

  //---------------------------------------------------------------------
  // Read header
  //---------------------------------------------------------------------
  char charbuffer[MAX_SIZEOF_LINE];
  fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
  if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);

  // check format
  if ((charbuffer[0] != '#') ||
      (charbuffer[1] != ' ') ||
      (charbuffer[2] != 'M') ||
      (charbuffer[3] != 'E') ||
      (charbuffer[4] != 'D'))
    throw IOException("not a valid distributed MED file", __FILE__, __LINE__);

  while ((charbuffer[0] == '#') || (strlen(charbuffer) == 0))
  {
    char* strTag = NULL;
    if ((charbuffer[0] == '#') && ((strTag = strstr(charbuffer, "[SOURCE]=")) != NULL))
    {
      char strSequentialMEDFilename[256];
      int ret = sscanf(strTag, "[SOURCE]=%s", strSequentialMEDFilename);
      if (ret == 1)
      {
        setSequentialMEDFilename(strSequentialMEDFilename);
      }
    }
    fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
    if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);
  }

  // read number of parts
  int nbParts = atoi(charbuffer);

  //---------------------------------------------------------------------
  // Read infos about sub-parts
  //---------------------------------------------------------------------
  char   lMeshName[MED_TAILLE_NOM + 1];
  int    lId;
  char   lPartName[MED_TAILLE_NOM + 1];
  char   lPath[256];
  char   lMEDFileName[256];    

  for (int i = 0 ; i < nbParts ; i++)
  {
    fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
    if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);

    while ((charbuffer[0] == '#') || (strlen(charbuffer) == 0))
    {
      fileMaster.getline(charbuffer, MAX_SIZEOF_LINE);
      if (fileMaster.fail()) throw IOException("i/o error while reading MED master file", __FILE__, __LINE__);
    }

    lMeshName[0]    = '\0';
    lId             = 0;
    lPartName[0]    = '\0';
    lPath[0]        = '\0';
    lMEDFileName[0] = '\0';

    int ret = sscanf(charbuffer, "%s %d %s %s %s", 
                     lMeshName,
                     &lId,
                     lPartName,
                     lPath,
                     lMEDFileName);

    if (ret != 5)
      throw IOException("i/o error while reading MED master file; bad format", __FILE__, __LINE__);

    // For Persistent (most probably moved) set of files:
    // - replace path to the part in <lMEDFileName> by path to the master file.
    string masterFilePath = multipr::getPath(pMEDfilename);
    string partFileName   = multipr::getFilenameWithoutPath(lMEDFileName);
    string newMEDFileName = masterFilePath + partFileName;

    //cout << "DBG: read: " << lMeshName << " " << lId << " " << lPartName << endl;
    addMesh(MeshDisPart::MULTIPR_KEEP_AS_IT,
            lMeshName,
            lId,
            lPartName,
            lPath,
            newMEDFileName.c_str(), // lMEDFileName
            NULL);
  }

  //---------------------------------------------------------------------
  // Close master file
  //---------------------------------------------------------------------
  fileMaster.close();
  if (fileMaster.fail()) throw IOException("i/o error while closing MED master file", __FILE__, __LINE__);
}

/**
 * Retrieves the output of MEDSPLITTER and convert it for MULTIPR.
 */
int convertMedsplitterToMultipr(
    ofstream&    pFileMaster, 
    const char*  pTmpFilename, 
    int          pId, 
    MeshDisPart* pPart, 
    string       pDestPath)
{
    MULTIPR_LOG("convert" << endl);

    const int MAX_SIZEOF_LINE = 1024;
    char charbuffer[MAX_SIZEOF_LINE];

    // Open medsplitter master file (ASCII file)
    ifstream fileMasterMedsplitter(pTmpFilename);
    if (fileMasterMedsplitter.fail())
      throw IOException("i/o error while opening MEDSPLITTER master file", __FILE__, __LINE__);

    fileMasterMedsplitter.getline(charbuffer, MAX_SIZEOF_LINE);
    if (fileMasterMedsplitter.fail())
      throw IOException("i/o error while reading MEDSPLITTER master file", __FILE__, __LINE__);    

    while ((charbuffer[0] == '#') || (strlen(charbuffer) == 0))
    {
      fileMasterMedsplitter.getline(charbuffer, MAX_SIZEOF_LINE);
      if (fileMasterMedsplitter.fail())
        throw IOException("i/o error while reading MEDSPLITTER master file", __FILE__, __LINE__);
    }

    // read number of parts
    int nbParts = atoi(charbuffer);
    //cout << "nb parts=" << nbParts << endl;

    char   lMeshName[MED_TAILLE_NOM + 1];
    int    lId;
    char   lPartName[MED_TAILLE_NOM + 1];
    char   lPath[256];
    char   lMEDFileName[256];

    for (int i = 0 ; i < nbParts ; i++)
    {
      fileMasterMedsplitter.getline(charbuffer, MAX_SIZEOF_LINE);
      if (fileMasterMedsplitter.fail()) throw IOException("", __FILE__, __LINE__);

      // parses the current line
      lMeshName[0]    = '\0';
      lId             = 0;
      lPartName[0]    = '\0';
      lPath[0]        = '\0';
      lMEDFileName[0] = '\0';

      int ret = sscanf(charbuffer, "%s %d %s %s %s",
                       lMeshName,
                       &lId,
                       lPartName,
                       lPath,
                       lMEDFileName);

      if (ret != 5)
        throw IOException("i/o error while reading MEDSPLITTER master file; bad format", __FILE__, __LINE__);

      string strDestFilename = pDestPath + multipr::getFilenameWithoutPath(lMEDFileName);
      if (strcmp(lMEDFileName, strDestFilename.c_str()) != 0)
      {
        multipr::copyFile(lMEDFileName, pDestPath.c_str());
        strcpy(lMEDFileName, strDestFilename.c_str());
      }

      pFileMaster << lMeshName << "_" << (i + 1) << " " << (pId + i + 1)
                  << " " << pPart->getPartName() << "_" << (i + 1)
                  << " " << lPath << " " << lMEDFileName << endl;
    }

    fileMasterMedsplitter.close();
    if (fileMasterMedsplitter.fail())
      throw IOException("i/o error while closing MEDSPLITTER master file", __FILE__, __LINE__);

    // remove master file generated by MEDSPLITTER
    remove(pTmpFilename);

    return nbParts + 1;
}


void MeshDis::writeDistributedMED (const char* pMEDfilenamePrefix,
                                   bool        pIsPersistence)
{
    if (pMEDfilenamePrefix == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Build master filename
    //---------------------------------------------------------------------
    string strPrefix = string(pMEDfilenamePrefix);
    const char* strExtension = ".med";
    string strMasterFilename;
    
    // add suffix "_groupes_maitre" iff it is not yet in the filename
    if (strstr(pMEDfilenamePrefix, "_groupes_maitre") == 0)
    {
        strMasterFilename= strPrefix + "_groupes_maitre" + strExtension;
    }
    else
    {
        strMasterFilename = strPrefix + strExtension;
    }
    
    string strDestPath = multipr::getPath(strMasterFilename.c_str());
    
    MULTIPR_LOG("Create master: " << strMasterFilename << endl);
    strcpy(mDistributedMEDFilename, strMasterFilename.c_str());
    
    //---------------------------------------------------------------------
    // Create an ASCII master file for the resulting distributed mesh and write header
    //---------------------------------------------------------------------
    remove(strMasterFilename.c_str());
    ofstream fileMaster(strMasterFilename.c_str());
    
    if (fileMaster == 0) throw IOException("i/o error while creating MED master file", __FILE__, __LINE__);
    
    fileMaster << "# MED file v2.3 - Master file created by MULTIPR v" << getVersion() << endl;
    fileMaster << "#" << " " << endl;
    fileMaster << "# [SOURCE]=" << mSequentialMEDFilename << endl;
    fileMaster << "#" << " " << endl;
    
    fileMaster << computeNumParts() << endl;
    if (fileMaster.fail()) throw IOException("i/o error while writing MED master file", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Create a new MED file (v2.3)
    //---------------------------------------------------------------------
    int id = 1;

    if (gProgressCallback != NULL) gProgressCallback->start("Save mesh", mParts.size());
    setProgress(0);

    try
    {
      // for each sub-meshes
      for (unsigned itPart = 0 ; itPart < mParts.size() ; itPart++)
      {
        switch (mParts[itPart]->mToDoOnNextWrite)
        {
            case MeshDisPart::MULTIPR_KEEP_AS_IT: 
            {
                string curFilename = mParts[itPart]->getMEDFileName();
                int curId = mParts[itPart]->mId;

                mParts[itPart]->mId = id;
                id++;
                
                // copy file in another directory?
                string strSrcPath = multipr::getPath(curFilename.c_str());
                if (strSrcPath != strDestPath)
                {
                  string strDestFilename = strDestPath +
                    multipr::getFilenameWithoutPath(curFilename.c_str());
                  multipr::copyFile(mParts[itPart]->getMEDFileName(), strDestPath.c_str());
                  strcpy(mParts[itPart]->mMEDFileName, strDestFilename.c_str());
                }
                
                fileMaster << (*mParts[itPart]) << endl;
                cout << (*mParts[itPart]) << endl;

                // restore old filename and id in case of persistence
                if (pIsPersistence)
                {
                  strcpy(mParts[itPart]->mMEDFileName, curFilename.c_str());
                  mParts[itPart]->mId = curId;
                }
                break;
            }
            
            case MeshDisPart::MULTIPR_WRITE_MESH:
            {
                if (strlen(mParts[itPart]->getMEDFileName()) == 0)
                  throw IOException("MED filename should not be empty", __FILE__, __LINE__);
                if (mParts[itPart]->mMesh == NULL)
                  throw IllegalStateException("invalid mesh (shoult not be NULL)", __FILE__, __LINE__);
                
                string curFilename = mParts[itPart]->getMEDFileName();
                int curId = mParts[itPart]->mId;

                string strDestFilename = strDestPath +
                  multipr::getFilenameWithoutPath(curFilename.c_str());
                strcpy(mParts[itPart]->mMEDFileName, strDestFilename.c_str());
                
                mParts[itPart]->mMesh->writeMED(mParts[itPart]->getMEDFileName());
                mParts[itPart]->mId = id;
                id++;
                fileMaster << (*mParts[itPart]) << endl;
                cout << (*mParts[itPart]) << endl;

                // restore old filename and id in case of persistence
                if (pIsPersistence)
                {
                  strcpy(mParts[itPart]->mMEDFileName, curFilename.c_str());
                  mParts[itPart]->mId = curId;
                }
                break;
            }
            
            case MeshDisPart::MULTIPR_WRITE_PARTS:
            {
                // We need to keep the original file.
                fileMaster << (*mParts[itPart]) << endl;
                // split this part using medsplitter
                if (mParts[itPart]->mOldCollection == NULL)
                  throw IllegalStateException("collection should not be NULL", __FILE__, __LINE__);
                string strPrefix = removeExtension(mParts[itPart]->getMEDFileName(), ".med"); 
                char tmpFilename[256];
                sprintf(tmpFilename, "%s_part", strPrefix.c_str());
                
                // remove previous file
                remove(tmpFilename);
                for (int i = 1 ; i <= mParts[itPart]->mSplit ; i++)
                {
                    char filename[256];
                    sprintf(filename, "%s%d.med", tmpFilename, i);
                    remove(filename);
                }
                
                mParts[itPart]->mCollection->write(tmpFilename);
                mParts[itPart]->mCollection->castAllFields(*(mParts[itPart]->mOldCollection));
                int ret = convertMedsplitterToMultipr(fileMaster, tmpFilename, id,
                                                      mParts[itPart], strDestPath);
                id += ret;
                //remove(mParts[itPart]->getMEDFileName());
                break;
            }
        
            default: throw IllegalStateException("should not be there", __FILE__, __LINE__);
        }

        if (gProgressCallback != NULL) gProgressCallback->moveOn();
        setProgress((itPart + 1) * 100 / mParts.size());
      }
    }
    catch (RuntimeException& e)
    {
        if (gProgressCallback != NULL) gProgressCallback->done();
        setProgress(100);
        throw e;
    }

    if (gProgressCallback != NULL) gProgressCallback->done();
    setProgress(90);

    //---------------------------------------------------------------------
    // Close master file
    //---------------------------------------------------------------------
    fileMaster.close();

    if (fileMaster.fail()) throw IOException("i/o error while closing MED master file", __FILE__, __LINE__);
}

void MeshDis::setProgress (int pPercents)
{
  boost::recursive_mutex::scoped_lock aLock (mWriteMutex);
  mWriteProgress = pPercents;
}

int MeshDis::getProgress()
{
  boost::recursive_mutex::scoped_lock aLock (mWriteMutex);
  int ret = mWriteProgress;
  return ret;
}


void MeshDis::readAndWriteFields (const char* pMeshName, std::vector<Group*>* pGroups,
                                  GaussIndexList* pGaussList, std::vector<Profil*>& pProfils)
{
    med_int                 lMEDfile, lCurMEDfile;
    std::vector<med_int>    lFiles;
    std::map<std::string, Profil*>  lProfilByName;

    // Open the original MED file.
    lMEDfile = MEDouvrir(mSequentialMEDFilename, MED_LECTURE);
    //---------------------------------------------------------------------
    // Read number of fields
    //---------------------------------------------------------------------
    MULTIPR_LOG("Read fields: ");
    med_int numFields = MEDnChamp(lMEDfile, 0);
    if (numFields <= 0)
      throw IOException("error while reading number of fields in the MED file", __FILE__, __LINE__);
    MULTIPR_LOG(numFields << ": OK\n");

    // For each part open the corresponding MED file.
    for (unsigned itPart = 0 ; itPart < mParts.size() ; itPart++)
    {
        lCurMEDfile = MEDouvrir(const_cast<char*>(mParts[itPart]->getMEDFileName()), MED_LECTURE_ECRITURE);
        // Keep the files descriptors. 
        lFiles.push_back(lCurMEDfile);
        if (lCurMEDfile == -1)
        {
            throw IOException("Can't open part med file.", __FILE__, __LINE__);
        }
    }

    // For easy acces we create an associative container to find the profil by its name.
    for (std::vector<Profil*>::iterator it = pProfils.begin(); it != pProfils.end(); ++it)
    {
        std::string lName = (*it)->getName();
        lProfilByName[lName] = (*it);
    }
    
    //---------------------------------------------------------------------
    // Iterate over fields
    //---------------------------------------------------------------------
    // for each field, read and write number of components and others infos
    for (int itField = 1 ; itField <= numFields ; ++itField)
    {
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
            // Create and read the field.
            Field* field = new Field();
            field->readMED(lMEDfile, itField, const_cast<char*>(pMeshName), CELL_TYPES[i]);
            // if the nth field does not apply on our mesh => skip it
            if (!field->isEmpty()) 
            {
                // Split and write this field in the MED files of the parts.
                field->writeMEDOptimized(&mParts, pMeshName, pGaussList, i, lFiles, lProfilByName);
                // If this field is on nodes, we dont need to iterate over the geometry types.
                if (field->isFieldOnNodes())
                {
                    delete field;
                    break;
                }
            }
            delete field;
        }
    }
    // Close all MED files.
    for (unsigned itPart = 0 ; itPart < mParts.size() ; itPart++)
    {
        MEDfermer(lFiles[itPart]);
    }
}

ostream& operator<<(ostream& pOs, MeshDis& pM)
{
    pOs << "Mesh Dis.:" << endl;
    pOs << "    Sequential filename (source) =|" << pM.mSequentialMEDFilename << "|" << endl;
    pOs << "    Distributed filename (master)=|" << pM.mDistributedMEDFilename << "|" << endl;
    pOs << "    #Sub-meshes =" << pM.mParts.size() << endl;
    
    for (unsigned itPart = 0 ; itPart < pM.mParts.size() ; itPart++)
    {
        cout << "        " << (itPart + 1) << ": " << (*(pM.mParts[itPart])) << endl; 
    }
    
    return pOs;
}


} // namespace multipr

// EOF
