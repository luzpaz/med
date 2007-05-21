// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_Family.cxx
 *
 * \brief   see MULTIPR_Family.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Family.hxx"
#include "MULTIPR_Exceptions.hxx"

#include <iostream>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class Attributs implementation
//*****************************************************************************

Attributs::Attributs()
{
    mId   = NULL;
    mVal  = NULL;
    mDesc = NULL;
    
    reset();
}


Attributs::~Attributs()
{
    reset();
}


Attributs& Attributs::operator=(const Attributs& pAttr)
{
    if (this != &pAttr) // check for self-assignement
    {
        reset();
        
        mNum = pAttr.mNum;
        
        if (mNum < 0) throw IllegalStateException("", __FILE__, __LINE__);
        
        if (pAttr.mId != NULL)
        {
            mId = new med_int[mNum];
            memcpy(mId, pAttr.mId, sizeof(med_int) * mNum);
        }
        
        if (pAttr.mVal != NULL)
        {
            mVal = new med_int[mNum];
            memcpy(mVal, pAttr.mVal, sizeof(med_int) * mNum);
        }
        
        if (pAttr.mDesc != NULL)
        {
            mDesc = new char[MED_TAILLE_DESC * mNum + 1];
            memcpy(mDesc, pAttr.mDesc, MED_TAILLE_DESC * mNum + 1);
        }
    }
    
    return *this;
}


void Attributs::reset()
{
    mNum = 0;
    
    if (mId   != NULL) { delete[] mId;   mId   = NULL; }
    if (mVal  != NULL) { delete[] mVal;  mVal  = NULL; }
    if (mDesc != NULL) { delete[] mDesc; mDesc = NULL; }
}


ostream& operator<<(ostream& pOs, Attributs& pA)
{
    if (pA.mNum == 0)
    {
        pOs << "NONE" << endl;
    }
    else
    {
        pOs << endl << "        #attr=" << pA.mNum << endl;
        
        if (pA.mId != NULL)
        {
            pOs << "        Id  =[";
            for (int i = 0 ; i < pA.mNum ; i++)
            {
                pOs << pA.mId[i] << " ";
            }
            pOs << "]" << endl;
        }
        else
        {
            pOs << "        Id  =NULL" << endl;
        }
        
        if (pA.mVal != NULL)
        {
            pOs << "        Val =[";
            for (int i = 0 ; i < pA.mNum ; i++)
            {
                pOs << pA.mVal[i] << " ";
            }
            pOs << "]" << endl;
        }
        else
        {
            pOs << "        Val =NULL";
        }
        
        if (pA.mDesc != NULL)
        {
            pOs << "        Desc=|" << pA.mDesc << "|" << endl;
        }
        else
        {
            pOs << "        Desc=NULL" << endl;
        }
    }
    
    return pOs;
}


//*****************************************************************************
// Class Family implementation
//*****************************************************************************

Family::Family() 
{
    reset(); 
}


Family::Family(const Family& pFamily)  
{ 
    strcpy(mName, pFamily.mName);
    mId            = pFamily.mId;
    mStrNameGroups = pFamily.mStrNameGroups;
    mNameGroups    = pFamily.mNameGroups;
    mAttributs     = pFamily.mAttributs;
}


Family::~Family()  
{ 
    reset();  
}


void Family::reset() 
{ 
    mName[0]       = '\0';
    mId            = 0;
    mStrNameGroups = "";
    
    mElt.clear();
    mNameGroups.clear();
    mAttributs.reset();
    
    mIsFamilyOfNodes = true;
    
    mFlagPrintAll = false;
}


void Family::insertElt(med_int pIndexElt) 
{ 
    if (pIndexElt < 1) throw new IllegalArgumentException("", __FILE__, __LINE__);
    
    mElt.insert(pIndexElt); 
}


void Family::buildGroups(vector<Group*>& pGroups, map<string, Group*>& pGroupNameToGroup) const
{
    // pGroups / pGroupNameToGroup can be empty or not
    
    // for each group in this family
    for (unsigned itGroup = 0 ; itGroup < mNameGroups.size() ; itGroup++)
    {
        const string& keyName = mNameGroups[itGroup];
        
        // check if the group already exist
        map<string, Group*>::iterator it = pGroupNameToGroup.find(keyName);
        
        Group* group = NULL;
        if (it != pGroupNameToGroup.end())
        {
            // the group already exists
            group = (*it).second;
        }
        else
        {
            // a new group have been identified: create a new entry
            group = new Group();
            group->setName(keyName);
            group->setIsGroupOfNodes(isFamilyOfNodes());
            
            pGroups.push_back(group);
            pGroupNameToGroup.insert(make_pair(keyName, group));
        }
        
        // add all elements of the this family to the group
        for (set<med_int>::iterator itElt = mElt.begin() ; itElt != mElt.end() ; itElt++)
        {
            group->insertElt(*itElt);
        }
        
    }
}


Family* Family::extractGroup(const char* pGroupName)
{
    Family* family = new Family();
    
    strcpy(family->mName, mName);
    family->mId = mId;
    family->mAttributs = mAttributs;
    
    if (pGroupName == NULL)
    {
        family->mStrNameGroups = mStrNameGroups;
        family->mNameGroups = mNameGroups;
    }
    else
    {
        if (strlen(pGroupName) > MED_TAILLE_LNOM) throw IllegalArgumentException("", __FILE__, __LINE__);
        if (strlen(pGroupName) == 0) throw IllegalArgumentException("", __FILE__, __LINE__);
        
        if (strstr(mStrNameGroups.c_str(), pGroupName) == 0)
        {
            // pGroupName found in the list of groups => just keep pGroupName
            family->mStrNameGroups = mStrNameGroups;
            family->mNameGroups = mNameGroups;
        }
        else
        {
            // pGroupName not found in the current list of groups 
            // probably not a group of the same nature => keep all the groups
            family->mStrNameGroups = pGroupName;
            family->mNameGroups.push_back(pGroupName);
        }
    }
    
    return family;
}


void Family::readMED(med_idt pMEDfile, char* pMeshName, med_int pIndex)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (pIndex < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    reset();
    
    med_int numGroups = MEDnGroupe(pMEDfile, pMeshName, pIndex);
    
    if (numGroups < 0) throw IOException("i/o error while reading number of groups in MED file", __FILE__, __LINE__);
    
    med_int numAttr = MEDnAttribut(pMEDfile, pMeshName, pIndex);
    
    med_int* attrId   = new med_int[numAttr];
    med_int* attrVal  = new med_int[numAttr];
    char*    attrDesc = new char[MED_TAILLE_DESC * numAttr + 1];
    attrDesc[0] = '\0';
    
    char* nameGroups = new char[MED_TAILLE_LNOM * numGroups + 1];
    nameGroups[0] = '\0';
    
    med_err ret = MEDfamInfo(
        pMEDfile, 
        pMeshName, 
        pIndex, 
        mName, 
        &mId, 
        attrId, 
        attrVal, 
        attrDesc, 
        &numAttr, 
        nameGroups, 
        &numGroups);
    
    if (ret != 0) throw IOException("i/o error while reading family information in MED file", __FILE__, __LINE__);
    
    attrDesc[MED_TAILLE_DESC * numAttr] = '\0';
    
    mAttributs.mNum  = numAttr;
    mAttributs.mId   = attrId;
    mAttributs.mVal  = attrVal;
    mAttributs.mDesc = attrDesc;
    
    mStrNameGroups = nameGroups;
    
    // split nameGroups
    for (int itGroup = 0 ; itGroup < numGroups ; itGroup++)
    {
        char str[MED_TAILLE_LNOM + 1];
        strncpy(str, nameGroups + itGroup * MED_TAILLE_LNOM, MED_TAILLE_LNOM);
        str[MED_TAILLE_LNOM] = '\0';
        mNameGroups.push_back(str);
    }
    
    delete[] nameGroups;
    
    // MEDfamLire is not necessary as we used MEDnoeudsLire/MEDelementsLire instead
}


void Family::writeMED(med_idt pMEDfile, char* pMeshName)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (strlen(mName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (mAttributs.mVal == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mAttributs.mDesc == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mAttributs.mId == NULL) throw IllegalStateException("", __FILE__, __LINE__);
        
    //cout << "Write family: " << mName << " " << mId << endl;
    
    med_err ret = MEDfamCr(
        pMEDfile,
        pMeshName,
        mName,
        mId,
        mAttributs.mId,
        mAttributs.mVal,
        mAttributs.mDesc,
        mAttributs.mNum,
        const_cast<char*>(mStrNameGroups.c_str()),
        mNameGroups.size());
    
    if (ret != 0) throw IOException("i/o error while creating family in MED file", __FILE__, __LINE__);
    
    // MEDfamEcr is not necessary as we used MEDnoeudsEcr/MEDelementsEcr instead

}


ostream& operator<<(ostream& pOs, Family& pF)
{
    pOs << "Family: " << endl;
    pOs << "    Name      =|" << pF.mName << "| size=" << strlen(pF.mName) << endl;
    pOs << "    Id        =" << pF.mId << endl;
    pOs << "    Family of =" << (pF.isFamilyOfNodes()?"NODES":"ELEMENTS") << endl;    
    
    pOs << "    Groups: #groups=" << pF.mNameGroups.size() << endl;
    for (unsigned itGroup = 0 ; itGroup < pF.mNameGroups.size() ; itGroup++)
    {
        pOs << "        Group " << (itGroup + 1) << ": |" << pF.mNameGroups[itGroup] << "| size=" << pF.mNameGroups[itGroup].length() << endl;
    }
    pOs << "    Attributs: " << pF.mAttributs;
    
    if (pF.mElt.size() != 0)
    {
        set<med_int>::iterator itSet = pF.mElt.end();
        itSet--;
        pOs << "    Elements: #elt=" << pF.mElt.size() << " min_id=" << (*(pF.mElt.begin())) << " max_id=" << (*itSet) << endl;
        if (pF.mFlagPrintAll)
        {
            pOs << "        ";
            for (set<med_int>::iterator itSet = pF.mElt.begin() ; itSet != pF.mElt.end() ; itSet++)
            {
                pOs << (*itSet) << " ";
            }
            pOs << endl;
        }
    }
    else
    {
        pOs << "    Elements: #elt=0" << endl;
    }
    
    return pOs;
}


//*****************************************************************************
// Class Group implementation
//*****************************************************************************

Group::Group() 
{
    reset(); 
}


Group::~Group()  
{ 
    reset();  
}


void Group::reset() 
{ 
    mName = "";
    mElt.clear();
    mIsGroupOfNodes = true;
    
    mFlagPrintAll = false;
}


void Group::setName(const string& pName) 
{ 
    if (pName.length() > MED_TAILLE_LNOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    mName = pName; 
}


void Group::insertElt(med_int pIndexElt) 
{ 
    if (pIndexElt < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    mElt.insert(pIndexElt); 
}

ostream& operator<<(ostream& pOs, Group& pG)
{
    pOs << "Group: " << endl;
    pOs << "    Name     =|" << pG.mName << "| size=" << pG.mName.length() << endl;
    pOs << "    Group of =" << (pG.isGroupOfNodes()?"NODES":"ELEMENTS") << endl;    

    if (pG.mElt.size() != 0)
    {
        set<med_int>::iterator itSet = pG.mElt.end();
        itSet--;
        pOs << "    Elements: #elt=" << pG.mElt.size() << " min_id=" << (*(pG.mElt.begin())) << " max_id=" << (*itSet) << endl;
        if (pG.mFlagPrintAll)
        {
            pOs << "        ";
            for (set<med_int>::iterator itSet = pG.mElt.begin() ; itSet != pG.mElt.end() ; itSet++)
            {
                pOs << (*itSet) << " ";
            }
            pOs << endl;
        }
    }
    else
    {
        pOs << "    Elements: #elt=0" << endl;
    }
    
    return pOs;
}


} // namespace  multipr

// EOF
