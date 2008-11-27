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
//  Partitioning/decimation module for the SALOME v3.2 platform
//
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
    strncpy(mName, pFamily.mName, MED_TAILLE_NOM);
    mName[MED_TAILLE_NOM] = '\0';
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
    
    for (int i = 0; i < eMaxMedMesh; ++i)
	{
		mElt[i].clear();
	}

    mNameGroups.clear();
    mAttributs.reset();
    
    mIsFamilyOfNodes = true;
    
    mFlagPrintAll = false;
}


void Family::insertElt(med_int pIndexElt, eMeshType pMeshIndex) 
{ 
    if (pIndexElt < 1) throw new IllegalArgumentException("", __FILE__, __LINE__);
    
    mElt[pMeshIndex].insert(pIndexElt);
}


void Family::buildGroups(vector<Group*>& pGroups, map<string, Group*>& pGroupNameToGroup) const
{
    // pGroups / pGroupNameToGroup can be empty or not

	if (isFamilyOfNodes())
	{
		return;
	}
    
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
        
        // add all elements of this family to the group
		for (int i = 0; i < eMaxMedMesh; ++i)
		{
			for (set<med_int>::iterator itElt = mElt[i].begin() ; itElt != mElt[i].end() ; itElt++)
    	    {
        	    group->insertElt(*itElt, (eMeshType)i);
        	}
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
	med_int*	attrId = NULL;
	med_int*	attrVal = NULL;
	char*		attrDesc = NULL;
    char*		nameGroups = NULL;

	if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (pIndex < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    reset();
    med_int numGroups = MEDnGroupe(pMEDfile, pMeshName, pIndex);
    if (numGroups < 0) throw IOException("i/o error while reading number of groups in MED file", __FILE__, __LINE__);
    
    med_int numAttr = MEDnAttribut(pMEDfile, pMeshName, pIndex);
	if (numAttr != 0)
	{
		attrId   = new med_int[numAttr];
		attrVal  = new med_int[numAttr];
		attrDesc = new char[MED_TAILLE_DESC * numAttr + 1];
		attrDesc[0] = '\0';
	}    
	if (numGroups != 0)
	{
    	nameGroups = new char[MED_TAILLE_LNOM * numGroups + 1];
    	nameGroups[0] = '\0';
	}

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

    mName[MED_TAILLE_NOM] = '\0';
	if (ret != 0) throw IOException("i/o error while reading family information in MED file", __FILE__, __LINE__);
	
	if (attrDesc != NULL)
	{
    	attrDesc[MED_TAILLE_DESC * numAttr] = '\0';
	}
    
    mAttributs.mNum  = numAttr;
    mAttributs.mId   = attrId;
    mAttributs.mVal  = attrVal;
    mAttributs.mDesc = attrDesc;
    
	if (nameGroups)
	{
	    mStrNameGroups = nameGroups;
	}
    
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
    if (pMEDfile == 0) throw IOException("Invalid MED file.", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("Invalide mesh name.", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("Mesh name is too long.", __FILE__, __LINE__);
    if (strlen(mName) > MED_TAILLE_NOM) throw IllegalArgumentException("Family name is too long.", __FILE__, __LINE__);
	
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
    
    if (ret != 0)
    {
        throw IOException("i/o error while creating family in MED file", __FILE__, __LINE__);
    }
    
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
    
	for (int i = 0; i < eMaxMedMesh; ++i)
	{
		if (pF.mElt[i].size() != 0)
		{
			set<med_int>::iterator itSet = pF.mElt[i].end();
			itSet--;
			pOs << "    Elements: #elt=" << pF.mElt[i].size() << " min_id=" << (*(pF.mElt[i].begin())) << " max_id=" << (*itSet) << endl;
			if (pF.mFlagPrintAll)
			{
				pOs << "        ";
				for (set<med_int>::iterator itSet = pF.mElt[i].begin() ; itSet != pF.mElt[i].end() ; itSet++)
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
	}
    pOs << "Name of groups=" << pF.mStrNameGroups << endl;
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
    for (int i = 0; i < eMaxMedMesh; ++i)
	{
		mElt[i].clear();
	}
    mIsGroupOfNodes = true;
    mFlagPrintAll = false;
}


void Group::setName(const string& pName) 
{ 
    if (pName.length() > MED_TAILLE_LNOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    mName = pName; 
}


void Group::insertElt(med_int pIndexElt, eMeshType pMeshIndex) 
{ 
    if (pIndexElt < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    mElt[pMeshIndex].insert(pIndexElt); 
}

ostream& operator<<(ostream& pOs, Group& pG)
{
    pOs << "Group: " << endl;
    pOs << "    Name     =|" << pG.mName << "| size=" << pG.mName.length() << endl;
    pOs << "    Group of =" << (pG.isGroupOfNodes()?"NODES":"ELEMENTS") << endl;    

	for (int i = 0; i < eMaxMedMesh; ++i)
	{
		if (pG.mElt[i].size() != 0)
		{
			set<med_int>::iterator itSet = pG.mElt[i].end();
			itSet--;
			pOs << "    Elements: #elt=" << pG.mElt[i].size() << " min_id=" << (*(pG.mElt[i].begin())) << " max_id=" << (*itSet) << endl;
			if (pG.mFlagPrintAll)
			{
				pOs << "        ";
				for (set<med_int>::iterator itSet = pG.mElt[i].begin() ; itSet != pG.mElt[i].end() ; itSet++)
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
	}
    return pOs;
}


} // namespace  multipr

// EOF
