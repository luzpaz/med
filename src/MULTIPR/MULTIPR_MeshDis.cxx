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

#include "MEDSPLITTER_API.hxx"

#include <iostream>
#include <fstream>

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
    //    "agregat100grains_12pas_grain97.med"                       -> "grain97"
    //    "agregat100grains_12pas_grain100_part2.med"                -> "grain100_part2"
    //    "aagregat100grains_12pas_grain98_gradmoy-low-25.0-0.3.med" -> "grain98_gradmoy-low-25-0.3"
    
    string prefix = removeExtension(mMEDFileName, ".med");
    prefix.erase(0, prefix.rfind("grain"));
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


vector<string> MeshDis::getFields() const
{
    vector<string> res;
    
    if (mParts.size() == 0)
    {
        return res;
    }
    
    // all the parts of the distributed MED file should have the same fields
    // => just return the name of fields of the first part
    switch (mParts[0]->mToDoOnNextWrite)
    {
        case MeshDisPart::MULTIPR_KEEP_AS_IT: 
        case MeshDisPart::MULTIPR_WRITE_PARTS:
        {
            vector<pair<string, int> > tmp = multipr::getListScalarFields(mParts[0]->getMEDFileName());
    
            for (unsigned i = 0 ; i < tmp.size() ; i++)
            {
                res.push_back(tmp[i].first);
            }
            return res;
        }
            
        case MeshDisPart::MULTIPR_WRITE_MESH:
            return mParts[0]->mMesh->getNameScalarFields();
        
        default: 
            throw IllegalStateException("", __FILE__, __LINE__);
    }
}


int MeshDis::getTimeStamps(const char* pFieldName) const
{
    if (mParts.size() == 0)
    {
        // no parts in this distributed MED file => no fields => #iteration = 0
        return 0;
    }
    
    // all the parts of the distributed MED file should have the same fields
    // => just return the number of iteration found in the field of the first part
    switch (mParts[0]->mToDoOnNextWrite)
    {
        case MeshDisPart::MULTIPR_KEEP_AS_IT: 
        case MeshDisPart::MULTIPR_WRITE_PARTS:
        {
            vector<pair<string, int> > tmp = multipr::getListScalarFields(mParts[0]->getMEDFileName());
        
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
    //cout << "MULTIPR: MeshDis::splitPart()" << endl;
    if (pPartName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pNbParts < 2) throw IllegalArgumentException("", __FILE__, __LINE__);
    if ((pPartitionner != MULTIPR_METIS) && (pPartitionner != MULTIPR_SCOTCH)) throw IllegalArgumentException("should be 0=METIS or 1=SCOTCH", __FILE__, __LINE__);
    
    //cout << "MULTIPR: file=" << __FILE__ << " line=" << __LINE__ << " args OK" << endl;
    
    //---------------------------------------------------------------------
    // Find the MED file corresponding to the given part
    //---------------------------------------------------------------------
    MeshDisPart* part = findPart(pPartName);
    
    if (part == NULL)
    {
        throw IllegalArgumentException("part not found in this distributed MED file", __FILE__, __LINE__);
    }

    //cout << "MULTIPR: file=" << __FILE__ << " line=" << __LINE__ << " find part OK" << endl;
    
    //---------------------------------------------------------------------
    // Load the sequential MED file
    //---------------------------------------------------------------------
    MEDSPLITTER::MESHCollection* collection;
    collection = new MEDSPLITTER::MESHCollection(part->getMEDFileName(), part->getMeshName());
    
    //cout << "MULTIPR: file=" << __FILE__ << " line=" << __LINE__ << " MEDSPLITTER collection OK" << endl;
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
            //cout << "MULTIPR: file=" << __FILE__ << " line=" << __LINE__ << " try to create partition using SCOTCH: #parts=" << pNbParts << endl;
            topology = collection->createPartition(pNbParts, MEDSPLITTER::Graph::SCOTCH);
            //cout << "MULTIPR: file=" << __FILE__ << " line=" << __LINE__ << " assigned SCOTCH" << endl;
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
        
        part->mToDoOnNextWrite = MeshDisPart::MULTIPR_WRITE_PARTS; 
        part->mSplit           = pNbParts;
        part->mOldCollection   = collection;
        part->mCollection      = newCollection;
    }
    catch (...)
    {
        //cout << "MEDSPLITTER error: new MESHCollection()" << endl;
        throw RuntimeException("MEDSPLITTER error: new MESHCollection()", __FILE__, __LINE__);
    }
}


std::list<std::string> MeshDis::decimatePart (const char* pPartName,
                                              const char* pFieldName,
                                              med_int     pFieldIt,
                                              const char* pFilterName,
                                              med_float   pTMed,
                                              med_float   pTLow,
                                              med_float   pRadius,
                                              int         pBoxing)
{
    //---------------------------------------------------------------------
    // Check arguments
    //---------------------------------------------------------------------
    if (pPartName == NULL)
      throw NullArgumentException("partname should not be NULL", __FILE__, __LINE__);
    if (pFieldName == NULL)
      throw NullArgumentException("fieldname should not be NULL", __FILE__, __LINE__);
    if (pFieldIt < med_int(1))
      throw IllegalArgumentException("invalid field iteration; should be >= 1", __FILE__, __LINE__);
    if (pTMed < 0.0)
      throw IllegalArgumentException("med res.: threshold must be > 0", __FILE__, __LINE__);
    if (pTMed >= pTLow)
      throw IllegalArgumentException("threshold for med res. must be < threshold for low res.",
                                     __FILE__, __LINE__);
    if (pRadius <= med_float(0.0))
      throw IllegalArgumentException("radius should be > 0", __FILE__, __LINE__);
    if ((pBoxing < 1) || (pBoxing > 200))
      throw IllegalArgumentException("boxing should be in [1..200]", __FILE__, __LINE__);

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
    char newMEDFileName[256];

    // *** create a new mesh = MEDIUM resolution ***
    sprintf(argv, "%s %d %lf %lf %d", pFieldName, pFieldIt, pTMed, pRadius, pBoxing);
    sprintf(newPartName, "%s_MED", pPartName);
    sprintf(newMEDFileName, "%s_gradmoy-med-%s-%s.med",
            strPrefix.c_str(),
            realToString(pTMed).c_str(),
            realToString(pRadius).c_str());

    {
      Mesh* meshMedium = meshFull->decimate(pFilterName, argv, part->getMeshName());
      cout << (*meshMedium) << endl;

      // insert only non-empty meshes
      if (meshMedium->getNumberOfElements() > 0)
      {
        insertMesh(MeshDisPart::MULTIPR_WRITE_MESH,
                   part->getMeshName(),
                   part->mId + 1,
                   newPartName,
                   "localhost",
                   newMEDFileName,
                   meshMedium,
                   part->mId + 0);
      }
      else
      {
        if (gEmptyMeshCallback != NULL)
        {
          gEmptyMeshCallback->reportEmptyMesh(newPartName);
        }
        ret.push_back(newPartName);
      }
    }

    // *** create a new mesh = LOW resolution ***
    sprintf(argv, "%s %d %lf %lf %d", pFieldName, pFieldIt, pTLow, pRadius, pBoxing);
    sprintf(newPartName, "%s_LOW", pPartName);
    sprintf(newMEDFileName, "%s_gradmoy-low-%s-%s.med", 
            strPrefix.c_str(), 
            realToString(pTLow).c_str(), 
            realToString(pRadius).c_str());

    {
      Mesh* meshLow = meshFull->decimate(pFilterName, argv, part->getMeshName());
      cout << (*meshLow) << endl;

      // insert only non-empty meshes
      if (meshLow->getNumberOfElements() > 0)
      {
        insertMesh(MeshDisPart::MULTIPR_WRITE_MESH,
                   part->getMeshName(),
                   part->mId + 2,
                   newPartName,
                   "localhost",
                   newMEDFileName,
                   meshLow,
                   part->mId + 1);
      }
      else
      {
        if (gEmptyMeshCallback != NULL)
        {
          gEmptyMeshCallback->reportEmptyMesh(newPartName);
        }
        ret.push_back(newPartName);
      }
    }

    // debug
    //cout << (*this) << endl;

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
            numParts += mParts[itPart]->mSplit;
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
int convertMedsplitterToMultipr(ofstream& pFileMaster, const char* pTmpFilename, int pId, MeshDisPart* pPart, string pDestPath)
{
    MULTIPR_LOG("convert" << endl);
    
    const int MAX_SIZEOF_LINE = 1024;
    char charbuffer[MAX_SIZEOF_LINE];
    
    // Open medsplitter master file (ASCII file)
    ifstream fileMasterMedsplitter(pTmpFilename);
    if (fileMasterMedsplitter.fail()) throw IOException("i/o error while opening MEDSPLITTER master file", __FILE__, __LINE__);
    
    fileMasterMedsplitter.getline(charbuffer, MAX_SIZEOF_LINE);
    if (fileMasterMedsplitter.fail()) throw IOException("i/o error while reading MEDSPLITTER master file", __FILE__, __LINE__);    

    while ((charbuffer[0] == '#') || (strlen(charbuffer) == 0))
    {
        fileMasterMedsplitter.getline(charbuffer, MAX_SIZEOF_LINE);
        if (fileMasterMedsplitter.fail()) throw IOException("i/o error while reading MEDSPLITTER master file", __FILE__, __LINE__);
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
            
        if (ret != 5) throw IOException("i/o error while reading MEDSPLITTER master file; bad format", __FILE__, __LINE__);
        
        string strDestFilename = pDestPath + multipr::getFilenameWithoutPath(lMEDFileName);
        if (strcmp(lMEDFileName, strDestFilename.c_str()) != 0)
        {
            multipr::copyFile(lMEDFileName, pDestPath.c_str());
            strcpy(lMEDFileName, strDestFilename.c_str());
        }
                
        pFileMaster << lMeshName << " " << (pId + i) << " " << pPart->getPartName() << "_" << (i + 1) << " " << lPath << " " << lMEDFileName << endl;
    }
    
    fileMasterMedsplitter.close();
    if (fileMasterMedsplitter.fail()) throw IOException("i/o error while closing MEDSPLITTER master file", __FILE__, __LINE__);
    
    // remove master file generated by MEDSPLITTER
    remove(pTmpFilename);
    
    return nbParts;
}


void MeshDis::writeDistributedMED(const char* pMEDfilenamePrefix)
{
    if (pMEDfilenamePrefix == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Build master filename
    //---------------------------------------------------------------------
    string strPrefix = string(pMEDfilenamePrefix);
    const char* strExtension = ".med";
    string strMasterFilename;
    
    // add suffix "_grains_maitre" iff it is not yet in the filename
    if (strstr(pMEDfilenamePrefix, "_grains_maitre") == 0)
    {
        strMasterFilename= strPrefix + "_grains_maitre" + strExtension;
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
                mParts[itPart]->mId = id;
                id++;
                
                // copy file in another directory?
                string strSrcPath = multipr::getPath(mParts[itPart]->getMEDFileName());
                if (strSrcPath != strDestPath)
                {
                    cout << "Write: KEEP_AS_IT: copy file" << endl;
                    string strDestFilename = strDestPath +
                      multipr::getFilenameWithoutPath(mParts[itPart]->getMEDFileName());
                    multipr::copyFile(mParts[itPart]->getMEDFileName(), strDestPath.c_str());
                    strcpy(mParts[itPart]->mMEDFileName, strDestFilename.c_str());
                }
                
                fileMaster << (*mParts[itPart]) << endl;
                cout << (*mParts[itPart]) << endl;
                break;
            }
            
            case MeshDisPart::MULTIPR_WRITE_MESH:
            {
                if (strlen(mParts[itPart]->getMEDFileName()) == 0)
                  throw IOException("MED filename is empty", __FILE__, __LINE__);
                if (mParts[itPart]->mMesh == NULL)
                  throw IllegalStateException("invalid mesh (shoult not be NULL)", __FILE__, __LINE__);
                
                string strDestFilename = strDestPath +
                  multipr::getFilenameWithoutPath(mParts[itPart]->getMEDFileName());
                strcpy(mParts[itPart]->mMEDFileName, strDestFilename.c_str());
                
                mParts[itPart]->mMesh->writeMED(mParts[itPart]->getMEDFileName());
                mParts[itPart]->mId = id;
                id++;
                fileMaster << (*mParts[itPart]) << endl;
                cout << (*mParts[itPart]) << endl;
                break;
            }
            
            case MeshDisPart::MULTIPR_WRITE_PARTS:
            {
                // split this part using medsplitter
                if (mParts[itPart]->mOldCollection == NULL)
                  throw IllegalStateException("", __FILE__, __LINE__);
                string strPrefix = removeExtension(mParts[itPart]->getMEDFileName(), ".med"); 
                char tmpFilename[256];
                sprintf(tmpFilename, "%s_part", strPrefix.c_str());
                mParts[itPart]->mCollection->write(tmpFilename);
                mParts[itPart]->mCollection->castAllFields(*(mParts[itPart]->mOldCollection));
                int ret = convertMedsplitterToMultipr(fileMaster, tmpFilename, id,
                                                      mParts[itPart], strDestPath);
                id += ret;
                remove(mParts[itPart]->getMEDFileName());
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
    setProgress(100);

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
