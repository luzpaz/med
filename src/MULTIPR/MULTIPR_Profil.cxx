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
 * \file    MULTIPR_Profil.cxx
 *
 * \brief   see MULTIPR_Profil.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Profil.hxx"
#include "MULTIPR_Exceptions.hxx"
#include "MULTIPR_Mesh.hxx"

#include <iostream>
#include <cstring>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class Profil implementation
//*****************************************************************************

Profil::Profil() 
{
    reset(); 
}

Profil::Profil(const Profil& pProfil)
{
    strcpy(mName, pProfil.mName);
    mTable = pProfil.mTable;
    mBinding = pProfil.mBinding;
    mGeomIdx = pProfil.mGeomIdx;
}

Profil::~Profil()  
{ 
    reset();  
}


void Profil::reset() 
{ 
    mName[0] = '\0'; 
    mTable.clear();
    mBinding = Undef;
    mGeomIdx = 0;
}


void Profil::create(const char* pName)
{
    if (pName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    reset();
    strcpy(mName, pName);
}



const char* Profil::getName() const
{
    return mName;
}

void Profil::add(med_int pElt) 
{
    mTable.insert(pElt); 
}

bool    Profil::find(med_int pElt)
{
    if (mTable.find(pElt) != mTable.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void    Profil::filterSetOfElement(std::set<med_int>& pIn, std::set<med_int>& pOut)
{
    if (&pOut == &pIn) throw IllegalStateException("pIn and pOut must be different !", __FILE__, __LINE__);
    int count = 1;

    for (std::set< med_int>::iterator it = mTable.begin(); it != mTable.end(); ++it)
    {
        if (pIn.find(*it) != pIn.end())
        {
            pOut.insert(count);
        }
        ++count;
    }
}

void    Profil::extractSetOfElement(const std::set<med_int>& pIn, std::set<med_int>& pOut)
{
    if (&pOut == &pIn) throw IllegalStateException("pIn and pOut must be different !", __FILE__, __LINE__); 
    int count = 1;
    
    for (std::set< med_int>::iterator it = pIn.begin(); it != pIn.end(); ++it)
    {
        if (mTable.find(*it) != mTable.end())
        {
            pOut.insert(count);
        }
        ++count;
    }
}

void Profil::readMED(med_idt pMEDfile, med_int pIndexProfil)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pIndexProfil < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    reset();

    // Get the profil info.
    med_int numData;
    med_err ret = MEDprofilInfo(
        pMEDfile, 
        pIndexProfil, 
        mName, 
        &numData);
    if (ret != 0) throw IOException("", __FILE__, __LINE__);

    // Read the data of the profil.    
    med_int* dataTmp = new med_int[numData];
    ret = MEDprofilLire(
        pMEDfile, 
        dataTmp, 
        mName);
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    for (int itData = 0 ; itData < numData ; itData++)
    {
        mTable.insert(dataTmp[itData]);
    }
    
    delete[] dataTmp;
}

void    Profil::readProfilBinding(med_idt pMEDfile, char* pMeshName)
{
    if (strlen(mName) == 0) throw IllegalStateException("The name of the Profil is empty", __FILE__, __LINE__); 

    int             nbField;
    med_err         ret;
    char            cha[MED_TAILLE_NOM + 1];
    med_type_champ  type;
    char*           comp;
    char*           unit;
    med_int         numComponents;
    char            gaussLocName[MED_TAILLE_NOM + 1];
    char            profilName[MED_TAILLE_NOM + 1];
    int             sizeOfData;
    int             sizeOfType;
    //unsigned char*  fieldData;
    bool            fieldOnNodes = false;
    med_entite_maillage     entity;
    med_geometrie_element   geom;
    med_int                 ngauss;
    med_int                 numdt;
    med_int                 numo;
    char                    dtunit[MED_TAILLE_PNOM + 1];
    med_float               dt;
    char                    maa[MED_TAILLE_NOM + 1];
    med_booleen             local;
    med_int                 nmaa;
    int                     geomIdx = 0;
    
    // Get the number of field in this file.
    nbField = MEDnChamp(pMEDfile, 0);
    if (nbField < 0) throw IOException("Can't read number of fields.", __FILE__, __LINE__);
    // For each field.
    for (int i = 1; i <= nbField; ++i)
    {
        // Get the number of component.
        numComponents = MEDnChamp(pMEDfile, i);
        comp = new char[numComponents * MED_TAILLE_PNOM + 1];
        unit = new char[numComponents * MED_TAILLE_PNOM + 1];
        // Get some information about the field.
        ret = MEDchampInfo(pMEDfile, i, cha, &type, comp, unit, numComponents);
        if (nbField < 0) throw IOException("Can't read fields info.", __FILE__, __LINE__);
        // Get the size of one component.
        switch (type)
        {
            case MED_FLOAT64: sizeOfType = 8; break;
            case MED_INT64: sizeOfType = 8; break;
            case MED_INT32: sizeOfType = 4; break;
            case MED_INT: sizeOfType = 4; break;
            default: throw IllegalStateException("Unknown field data type", __FILE__, __LINE__);
        }

        // Try to figure out if the field is on nodes or on elements.
        med_int numTimeStepNodes = MEDnPasdetemps(
            pMEDfile, 
            cha, 
            MED_NOEUD, 
            (med_geometrie_element) 0);

        if (numTimeStepNodes < 0) throw IOException("Can't read fields number of time steps.", __FILE__, __LINE__);
        if (numTimeStepNodes > 0)
        {
            // Its on nodes.
            fieldOnNodes = true;
            entity = MED_NOEUD;
            geom = (med_geometrie_element) 0;
        }
        else
        {
            // Its on elements.
            fieldOnNodes = false;
            entity = MED_MAILLE;
            // Get the geometry.
            for (int i = 0; i < eMaxMedMesh; ++i)
            {
                if (MEDnPasdetemps(pMEDfile, cha, entity, CELL_TYPES[i]) > 0)
                {
                    geom = CELL_TYPES[i];
                    geomIdx = i;
                    break;
                }
            }
        }

        // Get some information about the first time step.
        ret = MEDpasdetempsInfo(
            pMEDfile, 
            cha, 
            entity, 
            geom, 
            1, 
                        &ngauss, 
            &numdt, 
            &numo, 
            dtunit, 
            &dt, 
            maa, 
            &local, 
            &nmaa);
        if (ret < 0) throw IOException("Can't read fields time steps info.", __FILE__, __LINE__);
        // Get the number of values for the first time step.
        med_int nval = MEDnVal(
            pMEDfile,
            cha,
            entity,
            geom,
            numdt,
            numo,
            pMeshName,
            MED_GLOBAL);
        if (nval < 0) throw IOException("Can't read fields number values.", __FILE__, __LINE__);
        // Read the one time step of the field.                   
        sizeOfData = sizeOfType * numComponents * nval * 10;
        unsigned char* fieldData = new unsigned char[sizeOfData];
        gaussLocName[0] = '\0';
        ret = MEDchampLire(
            pMEDfile,
            pMeshName,
            cha,
            fieldData,
            MED_FULL_INTERLACE,
            MED_ALL,
            gaussLocName,
            profilName,
            MED_GLOBAL,
            entity,
            geom,
            numdt,
            numo);
        delete[] fieldData;
        if (ret < 0) throw IOException("Can't read field.", __FILE__, __LINE__);
        // Does this field use our profile ?
        if (strncmp(profilName, mName, MED_TAILLE_NOM) == 0)
        {
            // It does !
            if (fieldOnNodes)
            {
                mBinding = OnNodes;
                mGeomIdx = geomIdx;
            }
            else
            {
                mBinding = OnElements;
                mGeomIdx = geomIdx;
            }
            break;
        }
    }
}

void Profil::writeMED(med_idt pMEDfile)
{
    if (pMEDfile == 0) throw IOException("pMEDfile is 0", __FILE__, __LINE__);
    if (strlen(mName) > MED_TAILLE_NOM) throw IllegalStateException("Empty name", __FILE__, __LINE__);
    if (mTable.size() == 0) throw IllegalStateException("Empty table", __FILE__, __LINE__);
    int itData;
    std::set<med_int>::iterator it;
    med_int* dataTmp = new med_int[mTable.size()];
    for (it = mTable.begin(), itData = 0; it != mTable.end(); ++it, ++itData)
    {
        dataTmp[itData] = *it;
    }
    med_err ret = MEDprofilEcr(
        pMEDfile,
        dataTmp,
        mTable.size(),
        mName);

    if (ret != 0) throw IOException("Can't write profil", __FILE__, __LINE__);
    
    delete[] dataTmp;
}


ostream& operator<<(ostream& pOs, Profil& pP)
{
    pOs << "Profil:" << endl;
    pOs << "    Name=|" << pP.mName << "|" << endl;
    pOs << "    Size=" << pP.mTable.size() << endl;
    
    pOs << "    Entities=[";
    for (std::set<med_int>::iterator it = pP.mTable.begin();
     it != pP.mTable.end(); ++it)
    {
        pOs << *it << " ";
    }
    pOs << "]";
    
    return pOs;
}


} // namespace  multipr

// EOF
