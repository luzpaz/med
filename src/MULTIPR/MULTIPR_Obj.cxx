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
#include "MULTIPR_Family.hxx"
#include "MULTIPR_MeshDis.hxx"
#include "MULTIPR_Utils.hxx"
#include "MULTIPR_Profil.hxx"
#include "MED_Factory.hxx"

#include <stdio.h>
#include <iostream>
#include <errno.h>

extern int errno;

using namespace std;


namespace multipr
{

//*****************************************************************************
// Class Obj implementation
//*****************************************************************************

Obj::Obj()
  : mMeshDis(NULL)
{
  reset();
}


Obj::~Obj()  
{ 
  reset();  
}


void Obj::reset() 
{ 
    mPrevPath        = "";
    mMEDfilename     = "";
    mMeshName        = "";
    mState           = MULTIPR_OBJ_STATE_RESET;
    mDoReadWriteFields = false;

    mGaussList.clear();
    for (std::vector<Group*>::iterator it = mGroups.begin(); it != mGroups.end(); ++it)
    {
        delete *it;
    }
    mGroups.clear();

    if (mMeshDis != NULL) {
      delete mMeshDis;
      mMeshDis = NULL;
    }
}


void Obj::create(const char* pMEDfilename)
{
    if (pMEDfilename == NULL)
      throw multipr::NullArgumentException("file name must not be NULL", __FILE__, __LINE__);

    // reset everything before associating a new MED file to this object
    reset();
    
    mMEDfilename = pMEDfilename;
    
    // check if file exists
    FILE* f = fopen(pMEDfilename, "rb");

    // Big file (> 2Go) problem : since the other library may use 32bits off_t we check
    // the EFBIG error. Compile with -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
    // -D_LARGEFILE64_SOURCE flags to resolve this issue.
    if (f == NULL && errno == EFBIG)
    {
      mState = MULTIPR_OBJ_STATE_ERROR;
      throw IOException("Big file error : please compile with -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE flags.", __FILE__, __LINE__);
    }
    if (f == NULL) 
    {
        // file does not exist
        mState = MULTIPR_OBJ_STATE_ERROR;
        throw FileNotFoundException("file not found", __FILE__, __LINE__);
    }
    fclose(f);

    // test whether it is a sequential MED file or a distributed MED file.
    MED::EVersion aVersion = MED::GetVersionId(pMEDfilename, true);
    if (aVersion != MED::eVUnknown)
    {
        // sequential MED file has been sucessfuly openened
        
        // CASE 1: sequential MED file
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
      catch (RuntimeException& e)
      {
        e.dump(cout);
        // neither a sequential MED file, nor a distributed MED file => error
        mState = MULTIPR_OBJ_STATE_ERROR;
        throw IOException("file is nor a sequential MED file, neither a distributed MED file", __FILE__, __LINE__);
      }
    }
}


std::string Obj::getSequentialMEDFilename() const
{
    // partitionneGroupe() is only available for distributed MED file (not sequential MED file)
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


vector<string> Obj::getFields(const char* pPartList) const
{
    vector<string> res;
    // test whether it is a sequential MED file or a distributed MED file
    if ((mState == MULTIPR_OBJ_STATE_SEQ_INIT) ||
        (mState == MULTIPR_OBJ_STATE_SEQ)) 
    {
        // CASE 1: sequential MED file
        vector<pair<string, int> > tmp;
        multipr::getListScalarFields(mMEDfilename.c_str(), tmp, true, mMeshName.c_str());

        for (unsigned i = 0 ; i < tmp.size() ; i++)
        {
            res.push_back(tmp[i].first);
        }
        
        return res;
    }
    else
    {
        // CASE 2: distributed MED file
        if (mMeshDis == NULL)
          throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
        if (pPartList != NULL)
        {
            return mMeshDis->getFields(pPartList, true);
        }
        else
        {
            return res;
        }
    }
    return res;
}


int Obj::getTimeStamps(const char* pPartList, const char* pFieldName) const
{
    // test whether it is a sequential MED file or a distributed MED file
    if ((mState == MULTIPR_OBJ_STATE_SEQ_INIT) ||
        (mState == MULTIPR_OBJ_STATE_SEQ)) 
    {
        // CASE 1: sequential MED file
        vector<pair<string, int> > tmp;
        multipr::getListScalarFields(mMEDfilename.c_str(), tmp);
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
        if (mMeshDis == NULL)
          throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
        if (pPartList != NULL)
        {
            return mMeshDis->getTimeStamps(pPartList, pFieldName);
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

void Obj::getFieldMinMax(const char* pPartName, const char* pFieldName, float& pMin, float& pMax)
{
    // getFieldMinMax() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS) &&
        (mState != MULTIPR_OBJ_STATE_DIS_MEM)) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    if (mMeshDis == NULL) throw IllegalStateException("distributed MED file should not be NULL", __FILE__, __LINE__);
    
    pMin = 0.0f;
    pMax = 0.0f;
    
    MeshDisPart* part = mMeshDis->findPart(pPartName);
    if (part == NULL)
    {
        return;
    }
    if (part->getMesh() == NULL)
    {
        part->readMED();
    }
    part->getMesh()->getFieldMinMax(pFieldName, pMin, pMax);
}

vector<string> Obj::getParts() const
{
    // getParts() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS) &&
        (mState != MULTIPR_OBJ_STATE_DIS_MEM))
    {
         throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    }
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
    mesh.readSequentialMED(mMEDfilename.c_str(), mMeshName.c_str(), false);
    // We need to COPY the groups of the original mesh.
    // We delete the mesh at the end of the domain split but we still need
    // the groups for the optimized field extraction.
    // This data will be used and deleted in the save method.
    std::vector<Group*>* lGroups = mesh.getGroups();
    std::vector<Group*>::iterator   it;
    Group*              curGroup;
    for (it = lGroups->begin(); it != lGroups->end(); ++it)
    {
        curGroup = new Group();
        curGroup->setName((*it)->getName());
        for (int i = 0; i < eMaxMedMesh; ++i)
        {
            curGroup->setSetOfElt((eMeshType)i, (*it)->getSetOfElt((eMeshType)i));
        }
        curGroup->setIsGroupOfNodes((*it)->isGroupOfNodes());
        mGroups.push_back(curGroup);
    }
    cout << mesh << endl;
    
    //-------------------------------------------------------------
    // Build distributed mesh from groups
    //-------------------------------------------------------------
    cout << "Build distributed mesh: please wait... " << endl;
    try
    {
        // We need to COPY the gauss information from the original mesh (look at previous comment).
        mMeshDis = mesh.splitGroupsOfElements();
        mGaussList = *mesh.editGaussIndex();
        mesh.editGaussIndex()->clear();
        // We need to COPY the profiles of the original mesh (look at previous comment).
        std::vector<Profil*>& lProfils = mesh.getProfils();
        for (std::vector<Profil*>::iterator it = lProfils.begin(); it != lProfils.end(); ++it)
        {
            mProfils.push_back(new Profil(*(*it)));
        }
        mDoReadWriteFields = true;
    }
    catch (RuntimeException& e)
    {
        e.dump(cout);
        delete mMeshDis;
        mMeshDis = NULL;
        throw e;
    }
    
    mState = MULTIPR_OBJ_STATE_DIS_MEM;
    
    return getListParts();
}


vector<string> Obj::partitionneGroupe(
    const char* pPartName, 
    int         pNbParts, 
    int         pPartitionner)
{
    if (mMeshDis == NULL) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);
    if (pPartName == NULL)
      throw NullArgumentException("part name is empty", __FILE__, __LINE__);
    if (pNbParts < 2)
      throw IllegalArgumentException("number of parts must be at least two", __FILE__, __LINE__);
    if ((pPartitionner != 0) && (pPartitionner != 1)) throw IllegalArgumentException("partitionner should be 0=METIS or 1=SCOTCH", __FILE__, __LINE__);

    // partitionneGroupe() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS_MEM) &&
        (mState != MULTIPR_OBJ_STATE_DIS)) throw IllegalStateException("unexpected operation", __FILE__, __LINE__);
    
    // if distributed MED file is currently in memory,
    // then write to disk before performing partitionneGroupe()
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
        if (ret == 0)
          throw IOException("waiting for a distributed MED file (not a sequential one)", __FILE__, __LINE__);
    
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
    const char* pFilterParams)
{
  std::list<std::string> pEmptyMeshes;
  return decimePartition(pPartName,
                         pFieldName,
                         pFieldIt,
                         pFilterName,
                         pFilterParams,
                         pEmptyMeshes);
}


vector<string> Obj::decimePartition (const char* pPartName,
                                     const char* pFieldName,
                                     int         pFieldIt,
                                     const char* pFilterName,
                                     const char* pFilterParams,
                                     std::list<std::string>& pEmptyMeshes)
{
    vector<string> ret;
    // decimePartition() is only available for distributed MED file (not sequential MED file)
    if ((mState != MULTIPR_OBJ_STATE_DIS_MEM) &&
        (mState != MULTIPR_OBJ_STATE_DIS))
      throw IllegalStateException("unexpected operation", __FILE__, __LINE__);

    if (mMeshDis == NULL) throw IllegalStateException("expected a distributed MED file", __FILE__, __LINE__);

    //---------------------------------------------------------------------
    // Decimate
    //---------------------------------------------------------------------
    std::list<std::string> emptyMeshes = mMeshDis->decimatePart(
        pPartName, 
        pFieldName,
        pFieldIt,
        pFilterName,
        pFilterParams);

    std::list<std::string>::iterator it = emptyMeshes.begin(), end = emptyMeshes.end();
    for (; it != end; it++)
    {
      pEmptyMeshes.push_back(*it);
    }

    mState = MULTIPR_OBJ_STATE_DIS_MEM;

    ret = getListParts();
    ret.push_back(mMeshDis->getDecimationStatistics());
    return ret;
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

void Obj::getMEDInfo(char* pStats, char* pPartName)
{

        med_idt lMEDfile;
        med_int lRet;

        // Get the mesh path
        MeshDisPart* curMeshDis = this->mMeshDis->findPart(pPartName);
        if (curMeshDis == NULL)
        {
                throw IllegalStateException("Can't find part.", __FILE__, __LINE__);
                return ;
        }
        const char* curMesh = curMeshDis->getMEDFileName();
        if (curMesh == NULL)
        {
                throw IllegalStateException("Can't find mesh.", __FILE__, __LINE__);
                return ;
        }
        
        // open MED file for reading
        lMEDfile = MEDouvrir(const_cast<char*>(curMesh), MED_LECTURE); 
        if (lMEDfile <= 0)
        {
                throw IllegalStateException("Can't open file.", __FILE__, __LINE__);
                return ;
        }
        const char* meshName = curMeshDis->getMeshName();
        if (meshName == 0)
        {
                throw IllegalStateException("Can't find mesh name.", __FILE__, __LINE__);
        }       
        // Get the number of cells
        for (int itCell = 0 ; itCell < MED_NBR_GEOMETRIE_MAILLE ; itCell++)
    {
                lRet = MEDnEntMaa(lMEDfile, const_cast<char*>(meshName), MED_CONN, MED_MAILLE, CELL_TYPES[itCell], MED_NOD);
                if (lRet > 0)
                {
                        break;
                }
        }
        sprintf(pStats, "%d", lRet);
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
    // only save file if it is a distributed MED file currently in memory or path has changed
    if ((mState == MULTIPR_OBJ_STATE_DIS_MEM) || (strcmp(pPath, mPrevPath.c_str()) != 0))
    {
        mPrevPath = pPath;
        
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
        if (mDoReadWriteFields == true)
        {
            try 
            {
                mMeshDis->readAndWriteFields(mMeshName.c_str(), &mGroups, &mGaussList, mProfils);
                
            }
            catch (RuntimeException& e)
            {
                e.dump(cout);
                delete mMeshDis;
                mMeshDis = NULL;
                throw e;
            }                
            // need to delete some garb
            mGaussList.clear();
            for (std::vector<Group*>::iterator it = mGroups.begin(); it != mGroups.end(); ++it)
            {
                delete *it;
            }
            mGroups.clear();
            mDoReadWriteFields = false;
        }
        mMEDfilename = mMeshDis->getDistributedMEDFilename();
        
        cout << "Write MED master file: " << mMEDfilename << ": OK" << endl;
        
        //-------------------------------------------------------------
        // Read the distributed mesh
        //-------------------------------------------------------------
        {
          boost::recursive_mutex::scoped_lock aLock (mWriteMutex);
          delete mMeshDis;
          mMeshDis = new MeshDis();
        }
        mMeshDis->readDistributedMED(mMEDfilename.c_str());
        
        mState = MULTIPR_OBJ_STATE_DIS;
    }
    boost::recursive_mutex::scoped_lock aLock (mWriteMutex);
    mMeshDis->setProgress(100);
}

void Obj::resetProgress()
{
  boost::recursive_mutex::scoped_lock aLock (mWriteMutex);
  mMeshDis->setProgress(0);
}

int Obj::getProgress()
{
  boost::recursive_mutex::scoped_lock aLock (mWriteMutex);
  return mMeshDis->getProgress();
}

void Obj::savePersistent (const char* pPath)
{
  if (pPath == NULL)
    throw multipr::NullArgumentException("path must not be NULL", __FILE__, __LINE__);

  //-------------------------------------------------------------
  // Write distributed mesh
  //-------------------------------------------------------------
  string filename = string(pPath) + string("/") + multipr::getFilenameWithoutPath(mMEDfilename.c_str());
  string strPrefix = multipr::removeExtension(filename.c_str(), ".med");
  mMeshDis->writeDistributedMED(strPrefix.c_str(), /*pIsPersistence*/true);
}

void Obj::restorePersistent (const char* pMEDfilename)
{
  if (pMEDfilename == NULL)
    throw multipr::NullArgumentException("file name must not be NULL", __FILE__, __LINE__);

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
      //mMeshDis.reset(new MeshDis());
      mMeshDis->readPersistentDistributedMED(pMEDfilename);

      mState = MULTIPR_OBJ_STATE_DIS;
      cout << "Distributed MED file " << pMEDfilename << " has been successfuly opened" << endl;
    }
    catch (...)
    {
      // neither a sequential MED file, nor a distributed MED file => error
      mState = MULTIPR_OBJ_STATE_ERROR;
      throw IOException("file is neither a sequential, nor a distributed MED file", __FILE__, __LINE__);
    }
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
