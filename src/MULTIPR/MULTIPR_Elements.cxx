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
 * \file    MULTIPR_Elements.cxx
 *
 * \brief   see MULTIPR_Elements.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Elements.hxx"
#include "MULTIPR_Nodes.hxx"
#include "MULTIPR_Exceptions.hxx"

#include <iostream>
#include <set>
#include <map>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class Elements implementation
//*****************************************************************************

Elements::Elements() 
{
    mId       = NULL;
    mFamIdent = NULL;
    mNames    = NULL;
    mCon      = NULL;
    mFlagPrintAll = false;
    reset(); 
}

Elements::Elements(med_geometrie_element pGeomType) 
{
    mId       = NULL;
    mFamIdent = NULL;
    mNames    = NULL;
    mCon      = NULL;
    mNum      = 0;
    mGeom          = pGeomType;
    mNumNodesByElt = mGeom % 100;
    mDim           = mGeom / 100;
    mEntity        = MED_MAILLE;
    mFlagPrintAll  = false;
}

Elements::~Elements()  
{ 
    reset();  
}


void Elements::reset() 
{ 
    mNum           = 0;
    mEntity        = MED_MAILLE;
    mGeom          = MED_NONE;
    mNumNodesByElt = 0;
    mDim           = 0;
    
    if (mId != NULL)       { delete[] mId;       mId       = NULL; }
    if (mFamIdent != NULL) { delete[] mFamIdent; mFamIdent = NULL; }
    if (mNames != NULL)    { delete[] mNames;    mNames    = NULL; }
    if (mCon != NULL)      { delete[] mCon;      mCon      = NULL; }

    mSetOfNodes.clear();
    
    mFlagPrintAll = false;
}


med_int Elements::getFamilyIdentifier(med_int pIndex) const 
{ 
    if ((pIndex < 0) || (pIndex >= mNum)) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    return mFamIdent[pIndex]; 
}


const med_int* Elements::getConnectivity(int pIndex) const
{
    if ((pIndex < 0) || (pIndex >= mNum)) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    return mCon + mNumNodesByElt * pIndex;
}


void Elements::getCoordinates(med_int pIndexElt, const Nodes* pNodes, med_float* pCoo, int pFirst) const
{
    if ((pIndexElt < 0) || (pIndexElt >= mNum)) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    if (pNodes == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pCoo == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    // get the list of nodes of the element
    const med_int* con = getConnectivity(pIndexElt);
    
    med_float* destCoo = pCoo;
    int size = sizeof(med_float) * mDim;
    
    // for each node of the element
    int n = (mNumNodesByElt < pFirst) ? mNumNodesByElt : pFirst;
    for (int i = 0 ; i < n ; i++)
    {
        // get index of node (MED index start at 1)
        med_int indexNode = con[i] - 1;
        
        // get coordinates of this node
        const med_float* srcCoo = pNodes->getCoordinates(indexNode);
        
        // copy coordinates to destCoo
        memcpy(destCoo, srcCoo, size);
        
        // prepare next point
        destCoo += mDim;
    }
}


Elements* Elements::extractSubSet(const set<med_int>& pSetIndices) const
{
    Elements* subset = new Elements();
    
    //---------------------------------------------------------------------
    // Copy parameters
    //---------------------------------------------------------------------
    subset->mNum           = pSetIndices.size();
    subset->mEntity        = mEntity;
    subset->mGeom          = mGeom;          // e.g. 310 for a TETRA10
    subset->mNumNodesByElt = mNumNodesByElt; // e.g. 10 for a TETRA10
    subset->mDim           = mDim;           // e.g. 3 for a TETRA10
    
    //---------------------------------------------------------------------
    // Allocate arrays
    //---------------------------------------------------------------------
    subset->mFamIdent     = new med_int[subset->mNum];
    subset->mCon          = new med_int[mNumNodesByElt * subset->mNum];
    
    //---------------------------------------------------------------------
    // Copy subset of familys id and connectivity.
    //---------------------------------------------------------------------
    med_int* destCon = subset->mCon;
    set<med_int>::iterator itSet = pSetIndices.begin();
    for (int itElt = 0 ; itElt < subset->mNum && itSet != pSetIndices.end(); itElt++)
    {
        med_int srcIndex = (*itSet) - 1; // MED index start at 1
		subset->mFamIdent[itElt] = mFamIdent[srcIndex]; 

        med_int* srcCon = mCon + srcIndex * mNumNodesByElt;
        memcpy(destCon, srcCon, sizeof(med_int) * mNumNodesByElt);
        
        destCon += mNumNodesByElt;
        itSet++;
    }
    //---------------------------------------------------------------------
    // Copy subset of identifiers if necessary
    //---------------------------------------------------------------------
    if (isIdentifiers())  
    { 
        itSet = pSetIndices.begin();
        subset->mId = new med_int[subset->mNum]; 
        for (int itElt = 0 ; itElt < subset->mNum && itSet != pSetIndices.end(); itElt++)
        {
            med_int srcIndex = (*itSet) - 1; // MED index start at 1
            subset->mId[itElt] = mId[srcIndex];
            
            itSet++;
        }
    }
    
    //---------------------------------------------------------------------
    // Copy subset of names if necessary
    //---------------------------------------------------------------------
    if (isNames())       
    { 
        itSet = pSetIndices.begin();
        subset->mNames = new char[MED_TAILLE_PNOM * subset->mNum + 1]; 
        char* destPtr = subset->mNames;
        for (int itElt = 0 ; itElt < subset->mNum && itSet != pSetIndices.end(); itElt++)
        {
            med_int srcIndex = (*itSet) - 1; // MED index start at 1
            char* srcPtr = mNames + srcIndex * MED_TAILLE_PNOM; 
            memcpy(destPtr, srcPtr, MED_TAILLE_PNOM);
            
            destPtr += MED_TAILLE_PNOM;
            itSet++;
        }
        subset->mNames[MED_TAILLE_PNOM * subset->mNum] = '\0';
    }
    
    return subset;
}

void        Elements::extractSubSetFromNodes(const std::set<med_int>& pSetOfNodes, 
                                   std::set<med_int>& pSubSetOfElements) const
{
    if (&pSetOfNodes == &pSubSetOfElements) throw IllegalStateException("pSetOfNodes and pSubSetOfElements must be different !", __FILE__, __LINE__);
    
    int     numOfNodes = pSetOfNodes.size();
    bool    keepElt = false;
    for (int itElt = 0; itElt < mNum; ++itElt)
    {
        keepElt = false;
        for (std::set<med_int>::iterator itNode = pSetOfNodes.begin();
             itNode != pSetOfNodes.end() && keepElt == false; ++itNode)
        {
            for (int itCon = 0; itCon < mNumNodesByElt && keepElt == false; ++itCon)
            {
                if ((*itNode) == mCon[itElt * mNumNodesByElt + itCon])
                {
                    keepElt = true;
                }
            }
        }
        if (keepElt)
        {
            pSubSetOfElements.insert(itElt + 1);
        }
    }
}

const set<med_int>& Elements::getSetOfNodes()
{    
    // lazy get: test if mSetOfNodes has already been built
    if (mSetOfNodes.size() == 0)
    {
        buildSetOfNodes();
    }
    
    return mSetOfNodes;
}


set<med_int> Elements::getSetOfFamilies() const
{
    // set of families is empty at the beginning
    set<med_int> setOfFamilies;
    
    // for each element, add its family to the set
    for (int itElt = 0 ; itElt < mNum ; itElt++)
    {
        setOfFamilies.insert(mFamIdent[itElt]);
    }
    
    return setOfFamilies;
}


void Elements::remap() 
{
	int itNode, size;
    // build set of nodes if necessary
    if (mSetOfNodes.size() == 0)
    {
        buildSetOfNodes();
    }
    
    // build the map for indices convertion
    map<med_int, med_int> mapOldIndexToNewIndex;
    med_int newIndex = 1; // MED index start at 1
    for (set<med_int>::iterator itSet = mSetOfNodes.begin(); itSet != mSetOfNodes.end() ; itSet++)
    {
        med_int oldIndex = (*itSet);
        mapOldIndexToNewIndex.insert(make_pair(oldIndex, newIndex));
        newIndex++;
    }
    
    // for each node referenced by this set of elements
    for (itNode = 0, size = mNum * mNumNodesByElt ; itNode < size ; itNode++)
    {
        // convert old index to new index (remap)
        mCon[itNode] = mapOldIndexToNewIndex[mCon[itNode]];
    }
    
    buildSetOfNodes();
}

void Elements::remap(std::set<med_int>& pSetOfNodes)
{
	int itNode, size;
	
	// build the map for indices convertion
    map<med_int, med_int> mapOldIndexToNewIndex;
    med_int newIndex = 1; // MED index start at 1
	itNode = 1;
    for (std::set<med_int>::iterator it = pSetOfNodes.begin(); it != pSetOfNodes.end(); ++it)
    {
        med_int oldIndex = *it;
        mapOldIndexToNewIndex.insert(make_pair(oldIndex, itNode));
		++itNode;
    }
	// for each node referenced by this set of elements
    for (itNode = 0, size = mNum * mNumNodesByElt ; itNode < size ; itNode++)
    {
        // convert old index to new index (remap).
        mCon[itNode] = mapOldIndexToNewIndex[mCon[itNode]];;
    }
    buildSetOfNodes();

}

Elements* Elements::mergePartial(Elements* pElements, int pOffset)
{
    Elements* elements = new Elements();
    
    //---------------------------------------------------------------------
    // Copy parameters
    //---------------------------------------------------------------------
    elements->mNum           = mNum + pElements->mNum;
    
    if (mEntity != pElements->mEntity) throw IllegalStateException("entity should be the same", __FILE__, __LINE__);
    elements->mEntity        = mEntity;
    
    elements->mGeom          = mGeom;          // e.g. 310 for a TETRA10
    elements->mNumNodesByElt = mNumNodesByElt; // e.g. 10 for a TETRA10
    if (mDim != pElements->mDim) throw IllegalStateException("dimension should be the same", __FILE__, __LINE__);
    elements->mDim           = mDim;           // e.g. 3 for a TETRA10
    
    elements->mId            = NULL;
    elements->mNames         = NULL;
    
    //---------------------------------------------------------------------
    // Allocate arrays
    //---------------------------------------------------------------------
    elements->mFamIdent     = new med_int[elements->mNum];
    elements->mCon          = new med_int[mNumNodesByElt * elements->mNum];
    
    //---------------------------------------------------------------------
    // Copy familys id and connectivity.
    //---------------------------------------------------------------------
    memcpy(elements->mFamIdent, mFamIdent, mNum * sizeof(med_int));
    memcpy(elements->mFamIdent + mNum, pElements->mFamIdent, pElements->mNum * sizeof(med_int));
    
    memcpy(elements->mCon, mCon, mNum * mNumNodesByElt * sizeof(med_int));
    
    med_int* dst = elements->mCon + mNum * mNumNodesByElt;
    for (int i = 0, n = pElements->mNum * mNumNodesByElt ; i < n ; i++)
    {
        dst[i] = pElements->mCon[i] + pOffset;
    }    
    
    //cout << "WARNING: do no build set of nodes" << endl;
    //elements->buildSetOfNodes();
    
    return elements;
}


Elements* Elements::mergePartial(vector<Elements*> pElements, vector<int>& pOffsets)
{
    if (pElements.size() == 0) throw IllegalArgumentException("pElements should contain at least 1 element", __FILE__, __LINE__);
    
    Elements* elements = new Elements();
    
    //---------------------------------------------------------------------
    // Copy parameters
    //---------------------------------------------------------------------
    elements->mNum = mNum;
    for (unsigned i = 0 ; i < pElements.size() ; i++)
    {
        elements->mNum += pElements[i]->mNum;
    
        if (mEntity != pElements[i]->mEntity) throw IllegalStateException("entity should be the same", __FILE__, __LINE__);
        if (mDim != pElements[i]->mDim) throw IllegalStateException("dimension should be the same", __FILE__, __LINE__);
    }
        
    elements->mEntity        = mEntity;    
    elements->mGeom          = mGeom;          // e.g. 310 for a TETRA10
    elements->mNumNodesByElt = mNumNodesByElt; // e.g. 10 for a TETRA10    
    elements->mDim           = mDim;           // e.g. 3 for a TETRA10
    
    elements->mId            = NULL;
    elements->mNames         = NULL;
    
    //---------------------------------------------------------------------
    // Allocate arrays
    //---------------------------------------------------------------------
    elements->mFamIdent     = new med_int[elements->mNum];
    elements->mCon          = new med_int[mNumNodesByElt * elements->mNum];
    
    //---------------------------------------------------------------------
    // Copy familys id and connectivity.
    //---------------------------------------------------------------------
    memcpy(elements->mFamIdent, mFamIdent, mNum * sizeof(med_int));
    memcpy(elements->mCon, mCon, mNum * mNumNodesByElt * sizeof(med_int));
    
    int offsetFamIdent = mNum;
    int offsetCon      = mNum;
    for (unsigned j = 0 ; j < pElements.size() ; j++)
    {            
        memcpy(elements->mFamIdent + offsetFamIdent, pElements[j]->mFamIdent, pElements[j]->mNum * sizeof(med_int));  offsetFamIdent += pElements[j]->mNum;
        
        med_int* dst = elements->mCon + offsetCon * mNumNodesByElt;
        for (int i = 0, n = pElements[j]->mNum * mNumNodesByElt ; i < n ; i++)
        {
            dst[i] = pElements[j]->mCon[i] + pOffsets[j];
        }    
        offsetCon += pElements[j]->mNum;
    }
    
    //cout << "WARNING: do no build set of nodes" << endl;
    //elements->buildSetOfNodes();

    return elements;
}


void Elements::buildSetOfNodes()
{
    if (mSetOfNodes.size() != 0)
    {
        mSetOfNodes.clear();
    }
    
    // for each node referenced by this set of elements
    for (int itNode = 0, size = mNum * mNumNodesByElt ; itNode < size ; itNode++)
    {
        // add the node to the set
        mSetOfNodes.insert(mCon[itNode]);
    }
}


void Elements::readMED(
    med_idt               pMEDfile, 
    char*                 pMeshName, 
    med_int               pMeshDim, 
    med_entite_maillage   pEntity, 
    med_geometrie_element pGeom)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if ((pMeshDim < 1) || (pMeshDim > 3)) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    reset();
    
    mEntity        = pEntity;
    mGeom          = pGeom;
    mNumNodesByElt = mGeom % 100;
    mDim           = mGeom / 100;
    
    mNum = MEDnEntMaa(
        pMEDfile, 
        pMeshName, 
        MED_CONN,  // type of information requested = CONNECTIVITY
        pEntity, 
        pGeom, 
        MED_NOD);  // nodal connectivity
    
    if (mNum < 0) throw IOException("i/o error while reading information about elements in MED file", __FILE__, __LINE__);
    
    if (mNum == 0)
    {
        // empty mesh
        return;
    }
    
    mCon      = new med_int[mNumNodesByElt * mNum];
    mNames    = new char[MED_TAILLE_PNOM * mNum + 1];
    mId       = new med_int[mNum];
    mFamIdent = new med_int[mNum]; 
    med_booleen isNames;
    med_booleen isIdentifiers;
    
    med_err ret = MEDelementsLire(
        pMEDfile, 
        pMeshName, 
        pMeshDim, 
        mCon, 
        MED_FULL_INTERLACE,
        mNames, 
        &isNames,
        mId,
        &isIdentifiers,
        mFamIdent,
        mNum, 
        mEntity, 
        mGeom, 
        MED_NOD); // NODAL CONNECTIVITY
    
    if (ret != 0) throw IOException("i/o error while reading elements in MED file", __FILE__, __LINE__);
    
    if (!isNames)
    {
        delete[] mNames;
        mNames = NULL;
    }
    
    if (!isIdentifiers)
    {
        delete[] mId;
        mId = NULL;
    }
}


void Elements::writeMED(med_idt pMEDfile, char* pMeshName, med_int pMeshDim)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if ((pMeshDim < 1) || (pMeshDim > 3)) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    // special case: if no elements => do nothing
    if (mNum == 0) return;
    
    med_err ret = MEDelementsEcr(
        pMEDfile,
        pMeshName,
        pMeshDim,
        mCon,
        MED_FULL_INTERLACE,
        mNames,
        isNames()?MED_VRAI:MED_FAUX,
        mId,
        isIdentifiers()?MED_VRAI:MED_FAUX,
        mFamIdent,
        mNum,
        mEntity,
        mGeom,
        MED_NOD); // NODAL CONNECTIVITY
        
    if (ret != 0) throw IOException("i/o error while writing elements in MED file", __FILE__, __LINE__);
}


ostream& operator<<(ostream& pOs, Elements& pE)
{
    char strEntity[16];
    switch (pE.mEntity) 
    {
        case MED_MAILLE: strcpy(strEntity, "MED_MAILLE"); break;
        case MED_FACE:   strcpy(strEntity, "MED_FACE"); break;
        case MED_ARETE:  strcpy(strEntity, "MED_ARETE"); break;
        case MED_NOEUD:  strcpy(strEntity, "MED_NOEUD"); break;
        default:         strcpy(strEntity, "UNKNOWN"); break;
    }
    
    pOs << "Elements: " << endl;
    pOs << "    #number  =" << pE.mNum << endl;
    pOs << "    Entity   =" << strEntity << endl;
    pOs << "    Geom     =" << pE.mGeom << endl; 
    pOs << "    Has names?" << (pE.isNames()?"yes":"no") << endl;
    pOs << "    Has id   ?" << (pE.isIdentifiers()?"yes":"no") << endl;

    {
        set<med_int> setOfFam = pE.getSetOfFamilies();
        if (setOfFam.size() == 0)
        {
            pOs << "    Families: #fam=0" << endl;
        }
        else
        {
            set<med_int>::iterator itFam = setOfFam.end();
            itFam--;
            pOs << "    Families: #fam=" << setOfFam.size() << " id_min=" << (*(setOfFam.begin())) << " id_max=" << (*itFam) << endl;
            
            if (pE.mFlagPrintAll)
            {
                for (int i = 0 ; i < pE.mNum; i++)
                {
                    pOs << "        Elt " << (i + 1) << ": " << pE.mFamIdent[i] << endl;
                }
            }
        }
    }
    
    {
        set<med_int> setOfNodes = pE.getSetOfNodes();
        if (setOfNodes.size() == 0)
        {
            pOs << "    Connectivity: #nodes=0" << endl;
        }
        else
        {
            set<med_int>::iterator itNode = setOfNodes.end();
            itNode--;
            pOs << "    Connectivity: #nodes=" << setOfNodes.size() << " id_min=" << (*(setOfNodes.begin())) << " id_max=" << (*itNode) << endl;
            
            if (pE.mFlagPrintAll)
            {
                for (int i = 0 ; i < pE.mNum ; i++)
                {
                    pOs << "        Elt " << (i + 1) << ": ";
                    for (int j = 0 ; j < pE.mNumNodesByElt ; j++)
                    {
                        pOs << pE.mCon[i * pE.mNumNodesByElt + j] << " ";
                    }
                    pOs << endl;
                }
            }
        }
    }
    
    if (pE.mFlagPrintAll)
    {
        
        if (pE.isIdentifiers())
        {
            pOs << "    Num (identifier): " << endl;
            for (int i = 0 ; i < pE.mNum; i++)
            {
                pOs << "        Elt " << (i + 1) << ": " << pE.mId[i] << " " << endl;
            }
        }
        
        if (pE.isNames())
        {
            pE.mNames[MED_TAILLE_PNOM * pE.mNum] = '\0';
            pOs << "    Names: |" << pE.mNames << "|" << endl;
        }
        
    }
    
    return pOs;
}


} // namespace  multipr

// EOF
