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
 * \file    MULTIPR_Nodes.cxx
 *
 * \brief   see MULTIPR_Nodes.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Nodes.hxx"
#include "MULTIPR_Exceptions.hxx"

#include <iostream>
#include <limits>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class Nodes implementation
//*****************************************************************************

Nodes::Nodes() 
{
    mId           = NULL;
    mFamIdent     = NULL;
    mNames        = NULL;
    mCoo          = NULL;
    mNamesCoo     = NULL;
    mNamesUnitCoo = NULL;
    
    reset(); 
}


Nodes::~Nodes()  
{ 
    reset();  
}


void Nodes::reset() 
{ 
    mNum         = 0;
    mDim         = 0;
    mCoordSystem = MED_CART;
    
    if (mId           != NULL) { delete[] mId;           mId           = NULL; }
    if (mFamIdent     != NULL) { delete[] mFamIdent;     mFamIdent     = NULL; }
    if (mNames        != NULL) { delete[] mNames;        mNames        = NULL; }
    if (mCoo          != NULL) { delete[] mCoo;          mCoo          = NULL; }
    if (mNamesCoo     != NULL) { delete[] mNamesCoo;     mNamesCoo     = NULL; }
    if (mNamesUnitCoo != NULL) { delete[] mNamesUnitCoo; mNamesUnitCoo = NULL; }
    
    mFlagPrintAll = false;
}


const med_float* Nodes::getCoordinates(med_int pIndexNode) const 
{ 
    if ((pIndexNode < 0) || (pIndexNode >= mNum)) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    return mCoo + pIndexNode * mDim; 
}


med_int Nodes::getFamIdent(med_int pIndexNode) const 
{ 
    if ((pIndexNode < 0) || (pIndexNode >= mNum)) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    return mFamIdent[pIndexNode]; 
}


void Nodes::getBBox(med_float pMin[3], med_float pMax[3]) const
{
    //---------------------------------------------------------------------
    // Special case: no nodes => bbox = [0 ; 0] x [0 ; 0] x [0 ; 0]
    //---------------------------------------------------------------------
    if (mNum == 0)
    {
        for (int itDim = 0 ; itDim < mDim ; itDim++) 
        { 
            pMin[itDim] = med_float(0.0);
            pMax[itDim] = med_float(0.0);
        }
        
        return;
    }
    
    //---------------------------------------------------------------------
    // Compute axis-aligned bounding box
    //---------------------------------------------------------------------
    for (int itDim = 0 ; itDim < mDim ; itDim++) 
    { 
        pMin[itDim] = numeric_limits<med_float>::max();
        pMax[itDim] = -pMin[itDim];
    }
    for (int itNode = 0 ; itNode < mNum ; itNode++)
    {    
        for (int itDim = 0 ; itDim < mDim ; itDim++)
        {
            med_float coord = mCoo[itNode * mDim + itDim];
            if (coord < pMin[itDim]) pMin[itDim] = coord;
            if (coord > pMax[itDim]) pMax[itDim] = coord;
        }
    }
}


set<med_int> Nodes::getSetOfFamilies() const
{
    set<med_int> setOfFamilies;
    
    // for each node, ad its family to the set
    for (int itNode = 0 ; itNode < mNum ; itNode++)
    {
        setOfFamilies.insert(mFamIdent[itNode]);
    }
    
    return setOfFamilies;
}


Nodes* Nodes::extractSubSet(const set<med_int>& pSetIndices) const
{
    Nodes* subset = new Nodes();
    
    subset->mNum         = pSetIndices.size();
    subset->mDim         = mDim;
    subset->mCoordSystem = mCoordSystem;
    
    //---------------------------------------------------------------------
    // Allocate arrays
    //---------------------------------------------------------------------
    subset->mFamIdent     = new med_int[subset->mNum];    
    subset->mCoo          = new med_float[subset->mDim * subset->mNum];
    subset->mNamesCoo     = new char[subset->mDim * MED_TAILLE_PNOM + 1];
    subset->mNamesUnitCoo = new char[subset->mDim * MED_TAILLE_PNOM + 1];
    
    memcpy(subset->mNamesCoo, mNamesCoo, subset->mDim * MED_TAILLE_PNOM + 1);
    memcpy(subset->mNamesUnitCoo, mNamesUnitCoo, subset->mDim * MED_TAILLE_PNOM + 1);
    
    //---------------------------------------------------------------------
    // Copy subset of familys id and coords.
    //---------------------------------------------------------------------
    set<med_int>::iterator itSet = pSetIndices.begin();
    for (int i = 0 ; i < subset->mNum && itSet != pSetIndices.end(); i++)
    {
        
        med_int srcIndex = (*itSet) - 1; // MED index start at 1
		subset->mFamIdent[i] = mFamIdent[srcIndex];
        med_float* srcCoo = mCoo + srcIndex * mDim;
        med_float* destCoo = subset->mCoo + i * subset->mDim;
        for (int itDim = 0 ; itDim < mDim ; itDim++)
        {
            destCoo[itDim] = srcCoo[itDim];
        }
        
        itSet++;
    }
    //---------------------------------------------------------------------
    // Copy subset of identifiers if necessary
    //---------------------------------------------------------------------
    if (isIdentifiers())  
    { 
        itSet = pSetIndices.begin();
        subset->mId = new med_int[subset->mNum]; 
        for (int i = 0 ; i < subset->mNum && itSet != pSetIndices.end(); i++)
        {
            med_int srcIndex = (*itSet) - 1; // MED index start at 1
            subset->mId[i] = mId[srcIndex];
            
            itSet++;
        }
    }
    
    //---------------------------------------------------------------------
    // Copy subset of names if necessary
    //---------------------------------------------------------------------
    if (isNames())       
    { 
        subset->mNames = new char[MED_TAILLE_PNOM * subset->mNum + 1]; 
        char* destPtr = subset->mNames;
        itSet = pSetIndices.begin();
        for (int i = 0 ; i < subset->mNum && itSet != pSetIndices.end(); i++)
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


Nodes* Nodes::mergePartial(Nodes* pNodes)
{
    Nodes* nodes = new Nodes();
    
    nodes->mNum          = mNum + pNodes->mNum;
    
    if (mDim != pNodes->mDim) throw IllegalStateException("dimension should be the same", __FILE__, __LINE__);
    nodes->mDim          = mDim;    
    
    if (mCoordSystem != pNodes->mCoordSystem) throw IllegalStateException("coord. sytstem should be the same", __FILE__, __LINE__);
    nodes->mCoordSystem  = mCoordSystem;
    
    nodes->mId           = NULL; // optional: ignored
    nodes->mNames        = NULL; // optional: ignored
    
    //---------------------------------------------------------------------
    // Allocate arrays
    //---------------------------------------------------------------------
    nodes->mFamIdent      = new med_int[nodes->mNum];    
    nodes->mCoo           = new med_float[nodes->mDim * nodes->mNum];
    nodes->mNamesCoo      = new char[nodes->mDim * MED_TAILLE_PNOM + 1];
    nodes->mNamesUnitCoo  = new char[nodes->mDim * MED_TAILLE_PNOM + 1];
    
    memcpy(nodes->mNamesCoo, mNamesCoo, nodes->mDim * MED_TAILLE_PNOM + 1);
    memcpy(nodes->mNamesUnitCoo, mNamesUnitCoo, nodes->mDim * MED_TAILLE_PNOM + 1);
    
    //---------------------------------------------------------------------
    // Copy familys id and coords.
    //---------------------------------------------------------------------    
    memcpy(nodes->mFamIdent, mFamIdent, mNum * sizeof(med_int));
    memcpy(nodes->mFamIdent + mNum, pNodes->mFamIdent, pNodes->mNum * sizeof(med_int));
    
    memcpy(nodes->mCoo, mCoo, mNum * mDim * sizeof(med_float));
    memcpy(nodes->mCoo + (mNum * mDim), pNodes->mCoo, pNodes->mNum * pNodes->mDim * sizeof(med_float));
        
    return nodes;
}


Nodes* Nodes::mergePartial(vector<Nodes*> pNodes)
{
    if (pNodes.size() == 0) throw IllegalArgumentException("pNodes should contain at least 1 element", __FILE__, __LINE__);
    
    Nodes* nodes = new Nodes();
    
    // count total number of nodes and check if all set of nodes are compatible
    nodes->mNum = mNum;
    for (unsigned i = 0 ; i < pNodes.size() ; i++) 
    {
        nodes->mNum += pNodes[i]->mNum;
        
        if (mDim != pNodes[i]->mDim) throw IllegalStateException("dimension should be the same", __FILE__, __LINE__);
        if (mCoordSystem != pNodes[i]->mCoordSystem) throw IllegalStateException("coord. sytstem should be the same", __FILE__, __LINE__);
    }
    
    nodes->mDim = mDim;
    nodes->mCoordSystem  = mCoordSystem;
    
    nodes->mId    = NULL; // optional: ignored
    nodes->mNames = NULL; // optional: ignored
    
    //---------------------------------------------------------------------
    // Allocate arrays
    //---------------------------------------------------------------------
    nodes->mFamIdent      = new med_int[nodes->mNum];    
    nodes->mCoo           = new med_float[nodes->mDim * nodes->mNum];
    nodes->mNamesCoo      = new char[nodes->mDim * MED_TAILLE_PNOM + 1];
    nodes->mNamesUnitCoo  = new char[nodes->mDim * MED_TAILLE_PNOM + 1];
    
    memcpy(nodes->mNamesCoo, mNamesCoo, nodes->mDim * MED_TAILLE_PNOM + 1);
    memcpy(nodes->mNamesUnitCoo, mNamesUnitCoo, nodes->mDim * MED_TAILLE_PNOM + 1);
    
    //---------------------------------------------------------------------
    // Copy familys id and coords.
    //---------------------------------------------------------------------    
    memcpy(nodes->mFamIdent, mFamIdent, mNum * sizeof(med_int));
    memcpy(nodes->mCoo, mCoo, mNum * mDim * sizeof(med_float));
    
    int offsetFamIdent = mNum;
    int offsetCoo      = mNum;
    for (unsigned i = 0 ; i < pNodes.size() ; i++)
    {
        memcpy(nodes->mFamIdent + offsetFamIdent, pNodes[i]->mFamIdent, pNodes[i]->mNum * sizeof(med_int));   
        memcpy(nodes->mCoo + (offsetCoo * mDim), pNodes[i]->mCoo, pNodes[i]->mNum * mDim * sizeof(med_float));
        
        offsetFamIdent += pNodes[i]->mNum;
        offsetCoo      += pNodes[i]->mNum;
    }

    return nodes;
}


void Nodes::readMED(med_idt pMEDfile, char* pMeshName, med_int pDim)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (pDim != 3) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    reset();
    
    mDim = pDim;
    mNum = MEDnEntMaa(
        pMEDfile, 
        pMeshName, 
        MED_COOR, 
        MED_NOEUD, 
        med_geometrie_element(0), 
        med_connectivite(0));
    
    if (mNum < 0) throw IOException("", __FILE__, __LINE__);
    
    if (mNum == 0)
    {
        // empty mesh        
        return;
    }
    
    mId           = new med_int[mNum];
    mFamIdent     = new med_int[mNum];    
    mNames        = new char[MED_TAILLE_PNOM * mNum + 1];
    mCoo          = new med_float[mDim * mNum];
    mNamesCoo     = new char[mDim * MED_TAILLE_PNOM + 1];
    mNamesUnitCoo = new char[mDim * MED_TAILLE_PNOM + 1];
    
    mNames[0] = '\0';
    mNamesCoo[0] = '\0';
    mNamesUnitCoo[0] = '\0';
    
    med_booleen isIdentifiers;
    med_booleen isNames;
    
    med_err ret = MEDnoeudsLire(
        pMEDfile, 
        pMeshName, 
        mDim,
        mCoo, 
        MED_FULL_INTERLACE,
        &mCoordSystem, 
        mNamesCoo, 
        mNamesUnitCoo,
        mNames, 
        &isNames, 
        mId, 
        &isIdentifiers,
        mFamIdent,    
        mNum);
        
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    
    // check if coordinates system is CARTESIAN
    if (mCoordSystem != MED_CART) throw IllegalStateException("", __FILE__, __LINE__);
    
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


void Nodes::writeMED(med_idt pMEDfile, char* pMeshName) const
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if ((mDim < 1) || (mDim > 3)) throw IllegalStateException("", __FILE__, __LINE__);
    if (mFamIdent == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mCoo == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mNamesCoo == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mNamesUnitCoo == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    
    // special case: if no nodes => do nothing
    if (mNum == 0) return;
    
    med_err ret = MEDnoeudsEcr(
        pMEDfile,
        pMeshName,
        mDim,
        mCoo,
        MED_FULL_INTERLACE,
        mCoordSystem,
        mNamesCoo,
        mNamesUnitCoo,
        mNames,
        isNames()?MED_VRAI:MED_FAUX,
        mId,
        isIdentifiers()?MED_VRAI:MED_FAUX,
        mFamIdent,
        mNum);

    if (ret != 0) throw IOException("i/o error while writing nodes", __FILE__, __LINE__);
    
}


ostream& operator<<(ostream& pOs, Nodes& pN)
{
    char strCoordSystem[16];
    switch (pN.mCoordSystem) 
    {
        case MED_CART:  strcpy(strCoordSystem, "CARTESIAN"); break;
        case MED_CYL:   strcpy(strCoordSystem, "CYLINDRIC"); break;
        case MED_SPHER: strcpy(strCoordSystem, "SPHERIC"); break;
        default:        strcpy(strCoordSystem, "UNKNOWN"); break;
    }
    
    pOs << "Nodes: " << endl;
    pOs << "    #number      =" << pN.mNum << endl;
    pOs << "    Dimension    =" << pN.mDim << endl;
    pOs << "    Coord. system=" << strCoordSystem << endl;
    pOs << "    Has names    ?" << (pN.isNames()?"yes":"no") << endl;
    pOs << "    Has id       ?" << (pN.isIdentifiers()?"yes":"no") << endl;
    if (pN.mNamesCoo != NULL) pOs << "    Axis names   =" << "|" << pN.mNamesCoo << "|" << endl;
    if (pN.mNamesUnitCoo != NULL) pOs << "    Unit axis    =" << "|" << pN.mNamesUnitCoo << "|" << endl;
    
    {
        set<med_int> setOfFam = pN.getSetOfFamilies();
        if (setOfFam.size() == 0)
        {
            pOs << "    Families: #fam=0" << endl;
        }
        else
        {
            set<med_int>::iterator itFam = setOfFam.end();
            itFam--;
            pOs << "    Families: #fam=" << setOfFam.size() << " id_min=" << (*(setOfFam.begin())) << " id_max=" << (*itFam) << endl;
        }
        
        if (pN.mFlagPrintAll)
        {
            for (int itNode = 0 ; itNode < pN.mNum; itNode++)
            {
                pOs << "        Node " << (itNode+1) << ": " << pN.mFamIdent[itNode] << endl;
            }
        }
    }
    
    med_float bboxMin[3], bboxMax[3];
    pN.getBBox(bboxMin, bboxMax);
    pOs << "    BBox: [" << bboxMin[0] << " ; " << bboxMax[0] << "] x [" << bboxMin[1] << " ; " << bboxMax[1] << "] x [" << bboxMin[2] << " ; " << bboxMax[2] << "]" << endl; 
    
    if (pN.mFlagPrintAll)
    {
        pOs << "    Coordinates: " << endl;
        for (int itNode = 0 ; itNode < pN.mNum ; itNode++)
        {
            pOs << "        Node " << (itNode+1) << ": ";
            for (int itDim = 0 ; itDim < pN.mDim ; itDim++)
            {
                pOs << pN.mCoo[itNode * pN.mDim + itDim] << " ";
            }
            pOs << endl;
        }
        
        if (pN.isIdentifiers())
        {
            pOs << "    Num: " << endl;
            for (int itNode = 0 ; itNode < pN.mNum; itNode++)
            {
                pOs << "        Node " << (itNode+1) << ": " << pN.mId[itNode] << endl;
            }
        }
        
        if (pN.isNames())
        {
            pN.mNames[MED_TAILLE_PNOM * pN.mNum] = '\0';
            pOs << "    Names: |" << pN.mNames << "|" << endl;
        }
        
    }
    
    return pOs;
}


} // namespace  multipr

// EOF
