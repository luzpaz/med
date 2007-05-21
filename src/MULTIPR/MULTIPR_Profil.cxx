// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

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

#include <iostream>

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


Profil::~Profil()  
{ 
    reset();  
}


void Profil::reset() 
{ 
    mName[0] = '\0'; 
    mTable.clear(); 
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


med_int Profil::get(med_int pIndex) const 
{ 
    if ((pIndex < 0) || (pIndex >= med_int(mTable.size()))) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    return mTable[pIndex]; 
}


void Profil::add(med_int pElt) 
{
    mTable.push_back(pElt); 
}


void Profil::readMED(med_idt pMEDfile, med_int pIndexProfil)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pIndexProfil < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    reset();

    med_int numData;
    med_err ret = MEDprofilInfo(
        pMEDfile, 
        pIndexProfil, 
        mName, 
        &numData);
        
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    
    med_int* dataTmp = new med_int[numData];
    
    ret = MEDprofilLire(
        pMEDfile, 
        dataTmp, 
        mName);
    
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    
    for (int itData = 0 ; itData < numData ; itData++)
    {
        mTable.push_back(dataTmp[itData]);
    }
    
    delete[] dataTmp;
}


void Profil::writeMED(med_idt pMEDfile)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (strlen(mName) > MED_TAILLE_NOM) throw IllegalStateException("", __FILE__, __LINE__);
    if (mTable.size() == 0) throw IllegalStateException("", __FILE__, __LINE__);
    
    med_int* dataTmp = new med_int[mTable.size()];
    for (unsigned itData = 0 ; itData < mTable.size() ; itData++)
    {
        dataTmp[itData] = mTable[itData];
    }
    
    med_err ret = MEDprofilEcr(
        pMEDfile,
        dataTmp,
        mTable.size(),
        mName);
        
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    
    delete[] dataTmp;
}


ostream& operator<<(ostream& pOs, Profil& pP)
{
    pOs << "Profil:" << endl;
    pOs << "    Name=|" << pP.mName << "|" << endl;
    pOs << "    Size=" << pP.mTable.size() << endl;
    
    pOs << "    Entities=[";
    for (unsigned itElt = 0; itElt < pP.mTable.size() ; itElt++)
    {
        pOs << pP.mTable[itElt] << " ";
    }
    pOs << "]";
    
    return pOs;
}


} // namespace  multipr

// EOF
