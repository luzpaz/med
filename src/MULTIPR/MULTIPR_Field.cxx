// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_Field.cxx
 *
 * \brief   see MULTIPR_Field.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_Field.hxx"
#include "MULTIPR_Exceptions.hxx"

#include <iostream>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class Field implementation
//*****************************************************************************

Field::Field() 
{
    reset(); 
}


Field::~Field()  
{ 
    reset();  
}


void Field::reset() 
{ 
    mName[0]       = '\0';
    mEntity        = MED_NOEUD;
    mGeom          = MED_NONE;
    mType          = MED_FLOAT64;
    mSizeOfType    = 8;
    mNumComponents = 0;
    mStrComponent  = "";
    mStrUnit       = "";
    
    mNGauss.clear();
    mDT.clear();
    mNumDT.clear();
    mDTUnit.clear();
    mNumO.clear();
    mGaussLoc.clear();
    mProfil.clear();
    mSizeOfData.clear();
    mNVal.clear();
    
    for (unsigned it = 0 ; it < mVal.size() ; it++)
    {
        delete[] mVal[it];
    }
    mVal.clear();
    
    mFlagPrintAll = false;
}


bool Field::isEmpty() const
{
    return (mNGauss.size() == 0);
}


int Field::getNumberOfGaussPointsByElement(int pTimeStepIt) const
{
    if ((pTimeStepIt < 1) || (pTimeStepIt > int(mNGauss.size()))) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    return mNGauss[pTimeStepIt - 1];
}


const string& Field::getNameGaussLoc(int pTimeStepIt) const
{
    if ((pTimeStepIt < 1) || (pTimeStepIt > int(mNGauss.size()))) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    return mGaussLoc[pTimeStepIt - 1];
}


const unsigned char* Field::getValue(int pTimeStepIt, int pIndex) const
{
    if ((pTimeStepIt < 1) || (pTimeStepIt > int(mNGauss.size()))) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    if ((pIndex < 1) || (pIndex > mNVal[pTimeStepIt - 1] / mNGauss[pTimeStepIt - 1])) throw IndexOutOfBoundsException("", __FILE__, __LINE__);
    
    // size of one data = size of type * number of components * number of Gauss points
    int sizeOfOneData = mSizeOfType * mNumComponents * mNGauss[pTimeStepIt - 1];
    
    unsigned char* ret = mVal[pTimeStepIt - 1] + (pIndex - 1) * sizeOfOneData;
    
    return ret;
}


Field* Field::extractSubSet(const set<med_int>& pSetIndices) const
{
    Field* subset = new Field();
    
    memcpy(subset->mName, mName, MED_TAILLE_NOM + 1);
    subset->mEntity        = mEntity;
    subset->mGeom          = mGeom;
    subset->mType          = mType;
    subset->mSizeOfType    = mSizeOfType;
    subset->mNumComponents = mNumComponents;
    subset->mStrComponent  = mStrComponent;
    subset->mStrUnit       = mStrUnit;
    
    subset->mNGauss        = mNGauss;
    subset->mDT            = mDT;
    subset->mNumDT         = mNumDT;
    subset->mDTUnit        = mDTUnit;
    subset->mNumO          = mNumO;
    subset->mGaussLoc      = mGaussLoc;
    subset->mProfil        = mProfil;
    
    // for each time step
    for (unsigned itTimeStep = 0 ; itTimeStep < mNGauss.size() ; itTimeStep++)
    {
        if (mProfil[itTimeStep].size() != 0) throw UnsupportedOperationException("", __FILE__, __LINE__);
        // WARNING : do not manage profil for the moment
        // if there is a profil,
        // 1. we should set mProfil to NO_PROFIL for this time_step
        // 2. we should extract data according to the profil
        
        int nval = pSetIndices.size() * subset->mNGauss[itTimeStep];
        subset->mNVal.push_back(nval);
        
        int sizeOfData = nval * mSizeOfType * mNumComponents;
        subset->mSizeOfData.push_back(sizeOfData);
        
        unsigned char* data = new unsigned char[sizeOfData];
        unsigned char* dest = data;
        int sizeOfOneData = mSizeOfType * mNumComponents * subset->mNGauss[itTimeStep];
        
        // for each element to extract
        for (set<med_int>::iterator itSet = pSetIndices.begin() ; itSet != pSetIndices.end() ; itSet++)
        {
            int indexElt = (*itSet);
            
            // MED index start at 1.
            if (indexElt < 1) throw new IllegalArgumentException("", __FILE__, __LINE__);
            
            unsigned char* src = mVal[itTimeStep] + (indexElt - 1) * sizeOfOneData;
            memcpy(dest, src, sizeOfOneData);
            
            dest += sizeOfOneData;
        }
        subset->mVal.push_back(data);
    }
    
    return subset;
}


Field* Field::merge(Field* pField)
{    
    Field* field = new Field();
    
    if (strcmp(mName, pField->mName) != 0) throw IllegalStateException("incompatible field", __FILE__, __LINE__);    
    memcpy(field->mName, mName, MED_TAILLE_NOM + 1);
    
    if (mEntity != pField->mEntity) throw IllegalStateException("incompatible entity", __FILE__, __LINE__);
    field->mEntity        = mEntity;
    
    if (mGeom != pField->mGeom) throw IllegalStateException("incompatible geom", __FILE__, __LINE__);
    field->mGeom          = mGeom;
    
    if (mType != pField->mType) throw IllegalStateException("incompatible type", __FILE__, __LINE__);
    field->mType          = mType;
    
    field->mSizeOfType    = mSizeOfType;
    
    if (mNumComponents != pField->mNumComponents) throw IllegalStateException("incompatible #components", __FILE__, __LINE__);
    field->mNumComponents = mNumComponents;
    
    field->mStrComponent  = mStrComponent;
    field->mStrUnit       = mStrUnit;
    
    if (mNGauss.size() != pField->mNGauss.size()) throw IllegalStateException("incompatible #time stamps", __FILE__, __LINE__);
    field->mNGauss        = mNGauss;
    
    field->mDT            = mDT;
    field->mNumDT         = mNumDT;
    field->mDTUnit        = mDTUnit;
    field->mNumO          = mNumO;
    field->mGaussLoc      = mGaussLoc;
    field->mProfil        = mProfil;
    
    // for each time step
    for (unsigned itTimeStep = 0 ; itTimeStep < mNGauss.size() ; itTimeStep++)
    {
        int sizeOfData = mSizeOfData[itTimeStep] + pField->mSizeOfData[itTimeStep];
        field->mSizeOfData.push_back(sizeOfData);
        
        int nVal = mNVal[itTimeStep] + pField->mNVal[itTimeStep];
        field->mNVal.push_back(nVal);
        
        unsigned char* data = new unsigned char[sizeOfData];
        memcpy(data, mVal[itTimeStep], mSizeOfData[itTimeStep]);
        memcpy(data + mSizeOfData[itTimeStep], pField->mVal[itTimeStep], pField->mSizeOfData[itTimeStep]);
        field->mVal.push_back(data);
    }
    
    return field;
}


Field* Field::merge(vector<Field*> pFields, int pFieldIt)
{    
    if (pFields.size() == 0) throw IllegalArgumentException("pElements should contain at least 1 element", __FILE__, __LINE__);

    Field* field = new Field();
    
    // check if fields are compatible
    for (unsigned i = 0 ; i < pFields.size() ; i++)
    {    
        if (strcmp(mName, pFields[i]->mName) != 0) throw IllegalStateException("incompatible field", __FILE__, __LINE__);
        if (mEntity != pFields[i]->mEntity) throw IllegalStateException("incompatible entity", __FILE__, __LINE__);
        if (mGeom != pFields[i]->mGeom) throw IllegalStateException("incompatible geom", __FILE__, __LINE__);
        if (mType != pFields[i]->mType) throw IllegalStateException("incompatible type", __FILE__, __LINE__);
        if (mNumComponents != pFields[i]->mNumComponents) throw IllegalStateException("incompatible #components", __FILE__, __LINE__);
        if (mNGauss.size() != pFields[i]->mNGauss.size()) throw IllegalStateException("incompatible #time stamps", __FILE__, __LINE__);
    }   
    
    memcpy(field->mName, mName, MED_TAILLE_NOM + 1);        
    field->mEntity        = mEntity;        
    field->mGeom          = mGeom;        
    field->mType          = mType;    
    field->mSizeOfType    = mSizeOfType;        
    field->mNumComponents = mNumComponents;    
    field->mStrComponent  = mStrComponent;
    field->mStrUnit       = mStrUnit;
    
    if (pFieldIt == -1) // merge all time step
    {       
        field->mNGauss        = mNGauss;    
        field->mDT            = mDT;
        field->mNumDT         = mNumDT;
        field->mDTUnit        = mDTUnit;
        field->mNumO          = mNumO;
        field->mGaussLoc      = mGaussLoc;
        field->mProfil        = mProfil;      
        
        // for each time step
        for (unsigned itTimeStep = 0 ; itTimeStep < mNGauss.size() ; itTimeStep++)
        {
            int sizeOfData = mSizeOfData[itTimeStep];
            int nVal = mNVal[itTimeStep];
            
            for (unsigned i = 0 ; i < pFields.size() ; i++)
            {
                sizeOfData += pFields[i]->mSizeOfData[itTimeStep];
                nVal += pFields[i]->mNVal[itTimeStep];
            }                        
            
            field->mSizeOfData.push_back(sizeOfData);                        
            field->mNVal.push_back(nVal);
            
            unsigned char* data = new unsigned char[sizeOfData];
            memcpy(data, mVal[itTimeStep], mSizeOfData[itTimeStep]);
            
            int offsetData = mSizeOfData[itTimeStep];
            for (unsigned i = 0 ; i < pFields.size() ; i++)
            {
                memcpy(data + offsetData, pFields[i]->mVal[itTimeStep], pFields[i]->mSizeOfData[itTimeStep]);
                offsetData += pFields[i]->mSizeOfData[itTimeStep];
            }
            
            field->mVal.push_back(data);
        }
    }
    else // only merge the given time step
    {        
        field->mNGauss.push_back(mNGauss[pFieldIt]);
        field->mDT.push_back(mDT[pFieldIt]);
        field->mNumDT.push_back(mNumDT[pFieldIt]);
        field->mDTUnit.push_back(mDTUnit[pFieldIt]);
        field->mNumO.push_back(mNumO[pFieldIt]);
        
        if (mGaussLoc.size() != 0) field->mGaussLoc.push_back(mGaussLoc[pFieldIt]);
        if (mProfil.size() != 0) field->mProfil.push_back(mProfil[pFieldIt]);
        
        // to finish
        throw UnsupportedOperationException("not yet implemented", __FILE__, __LINE__);
    }
    
    return field;
}


void Field::getSetOfGaussLoc(set<string>& pSetOfGaussLoc) const
{
    for (unsigned itGaussLoc = 0 ; itGaussLoc < mGaussLoc.size() ; itGaussLoc++)
    {
        const string& gaussLocName = mGaussLoc[itGaussLoc];
        
        if (gaussLocName.length() != 0)
        {
            pSetOfGaussLoc.insert(gaussLocName);
        }
    }
}


void Field::readMED(med_idt pMEDfile, med_int pIndex, char* pMeshName)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (pIndex < 1) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    reset();
    
    mNumComponents = MEDnChamp(pMEDfile, pIndex);
    
    if (mNumComponents < 0) throw IOException("", __FILE__, __LINE__);
    
    char* strComponent = new char[mNumComponents * MED_TAILLE_PNOM + 1];
    char* strUnit      = new char[mNumComponents * MED_TAILLE_PNOM + 1];
    
    strComponent[0] = '\0';
    strUnit[0] = '\0';
    
    med_err ret = MEDchampInfo(
        pMEDfile, 
        pIndex, 
        mName, 
        &(mType), 
        strComponent, 
        strUnit, 
        mNumComponents);
    
    if (ret != 0) throw IOException("", __FILE__, __LINE__);
    
    mStrComponent = strComponent;
    mStrUnit = strUnit;
    
    delete[] strUnit;
    delete[] strComponent;
    
    switch (mType)
    {
        case MED_FLOAT64: mSizeOfType = 8; break;
        case MED_INT64: mSizeOfType = 8; break;
        case MED_INT32: mSizeOfType = 4; break;
        case MED_INT: mSizeOfType = 4; break;
        default: throw IllegalStateException("should not be there", __FILE__, __LINE__);
    }
    
    //---------------------------------------------------------------------
    // Read fields over nodes
    //---------------------------------------------------------------------
    bool fieldOnNodes = false;
    {
        med_int numTimeStepNodes = MEDnPasdetemps(
            pMEDfile, 
            mName, 
            MED_NOEUD, 
            (med_geometrie_element) 0);
        
        if (numTimeStepNodes < 0) throw IOException("", __FILE__, __LINE__);
        
        if (numTimeStepNodes != 0)
        {
            fieldOnNodes = true;
            mEntity = MED_NOEUD;
            mGeom = (med_geometrie_element) 0;
            readMEDtimeSteps(pMEDfile, numTimeStepNodes, pMeshName);
        }
    }

    //---------------------------------------------------------------------
    // Read fields over elements
    //---------------------------------------------------------------------
    {
        med_int numTimeStepElt = MEDnPasdetemps(
            pMEDfile, 
            mName, 
            MED_MAILLE, 
            MED_TETRA10);
        
        if (numTimeStepElt  < 0) throw IOException("", __FILE__, __LINE__);    
        
        if (numTimeStepElt != 0)
        {
            if (fieldOnNodes) throw IllegalStateException("", __FILE__, __LINE__);
            
            mEntity = MED_MAILLE;
            mGeom = MED_TETRA10;
            readMEDtimeSteps(pMEDfile, numTimeStepElt, pMeshName);
        }
    }
}


void Field::readMEDtimeSteps(med_idt pMEDfile, med_int pNumberOfTimeSteps, char* pMeshName)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (pNumberOfTimeSteps < 0) throw IllegalArgumentException("", __FILE__, __LINE__);
    
    char strEntity[8];
    switch (mEntity)
    {
        case MED_ARETE:
        case MED_FACE:
        case MED_MAILLE: strcpy(strEntity, "CELLS"); break;
        case MED_NOEUD: strcpy(strEntity, "NODES"); break;
        default: strcpy(strEntity, "UNKNOWN"); break;
    }
    
    // iterates over time step
    for (int itTimeStep = 1 ; itTimeStep <= pNumberOfTimeSteps ; itTimeStep++)
    {
        med_int ngauss;
        med_int numdt;
        med_int numo;
        char dtunit[MED_TAILLE_PNOM + 1];
        med_float dt;
        char maa[MED_TAILLE_NOM + 1];
        med_booleen local;
        med_int nmaa;
        
        med_err ret = MEDpasdetempsInfo(
            pMEDfile, 
            mName, 
            mEntity, 
            mGeom, 
            itTimeStep, 
                &ngauss, 
            &numdt, 
            &numo, 
            dtunit, 
            &dt, 
            maa, 
            &local, 
            &nmaa);
            
        if (ret != 0) throw IOException("i/o error while reading #timesteps in MED file", __FILE__, __LINE__);
        
        // mesh must be local
        if (local != MED_VRAI) throw IllegalStateException("only local fields are currently supported", __FILE__, __LINE__);
        
        // #mesh must be 1
        if (nmaa != 1) throw IllegalStateException("field shoud be associated with 1 mesh only", __FILE__, __LINE__);
        
        // mesh must be pMeshName
        // if field does not apply on the current mesh, skip
        if (strcmp(maa, pMeshName) != 0) 
        {
            continue;
        }
        
        mNGauss.push_back(ngauss);
        mDT.push_back(dt);
        mNumDT.push_back(numdt);
        mDTUnit.push_back(dtunit);
        mNumO.push_back(numo);
        
        med_int nval = MEDnVal(
            pMEDfile, 
            mName, 
            mEntity, 
            mGeom, 
            numdt, 
            numo, 
            pMeshName, 
            MED_GLOBAL);
        
        if (nval < 0) throw IOException("i/o error while reading field in MED file", __FILE__, __LINE__);
        
        mNVal.push_back(nval);
        
        char gaussLocName[MED_TAILLE_NOM + 1];
        char profilName[MED_TAILLE_NOM + 1];
        int sizeOfData = mSizeOfType * mNumComponents * nval;
        mSizeOfData.push_back(sizeOfData);
        unsigned char* fieldData = new unsigned char[sizeOfData];
        
        ret = MEDchampLire(
            pMEDfile, 
            pMeshName,
            mName, 
            fieldData, 
            MED_FULL_INTERLACE, 
            MED_ALL, 
            gaussLocName, 
            profilName, 
            MED_COMPACT, // should be: MED_GLOBAL, MED_NO_PFLMOD, MED_COMPACT
            mEntity, 
            mGeom, 
            numdt, 
            numo);
        
        if (ret != 0) throw IOException("i/o error while reading field in MED file", __FILE__, __LINE__);
    
        mGaussLoc.push_back(gaussLocName);
        mProfil.push_back(profilName);
        mVal.push_back(fieldData);
    }
}


void Field::writeMED(med_idt pMEDfile, char* pMeshName)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (pMeshName == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    if (strlen(pMeshName) > MED_TAILLE_NOM) throw IllegalArgumentException("", __FILE__, __LINE__);
    if (mNumComponents < 1) throw IllegalStateException("", __FILE__, __LINE__);
    
    med_err ret = MEDchampCr(
        pMEDfile, 
        mName,                                        // name of the field
        mType,                                        // type of data (MED_FLOAT64, MED_INT32, etc.)
        const_cast<char*>(mStrComponent.c_str()),     // name of components
        const_cast<char*>(mStrUnit.c_str()),          // name of units
        mNumComponents);                              // number of components
    
    if (ret != 0) throw IOException("i/o error while creating field in MED file", __FILE__, __LINE__);
    
    // for each time step
    for (unsigned i = 0 ; i < mNGauss.size() ; i++)
    {
        // skip if no values
        if (mNVal[i] == 0) continue;
        
        ret = MEDchampEcr(
            pMEDfile,
            pMeshName,          // name of the mesh (first call to MEDchampEcr => name of reference)
            mName,              // name of the field
            mVal[i],            // data (= values)
            MED_FULL_INTERLACE, // data organization
            mNVal[i],           // number of values
            const_cast<char*>(mGaussLoc[i].c_str()), // name of Gauss reference
            MED_ALL,            // components to be selected
            const_cast<char*>(mProfil[i].c_str()), // name of profil
            MED_GLOBAL,         // how to read data: MED_NO_PFLMOD,MED_COMPACT,MED_GLOBAL
            mEntity,            // type of entity (MED_NOEUD, MED_MAILLE, etc.)
            mGeom,              // type of geometry (TETRA10, etc.)
            mNumDT[i],          // time step iteration
            const_cast<char*>(mDTUnit[i].c_str()), // unit of time step
            mDT[i],             // time key
            mNumO[i]);          // order number
        
        if (ret != 0) throw IOException("i/o error while writing field in MED file", __FILE__, __LINE__);
    }
}


ostream& operator<<(ostream& pOs, Field& pF)
{
    char strEntity[16];
    switch (pF.mEntity) 
    {
        case MED_MAILLE: strcpy(strEntity, "MED_MAILLE"); break;
        case MED_FACE:   strcpy(strEntity, "MED_FACE"); break;
        case MED_ARETE:  strcpy(strEntity, "MED_ARETE"); break;
        case MED_NOEUD:  strcpy(strEntity, "MED_NOEUD"); break;
        default:         strcpy(strEntity, "UNKNOWN"); break;
    }
    
    char strType[16];
    switch (pF.mType) 
    {
        case MED_FLOAT64: strcpy(strType, "MED_FLOAT64"); break;
        case MED_INT32:   strcpy(strType, "MED_INT32"); break;
        case MED_INT64:   strcpy(strType, "MED_INT64"); break;
        case MED_INT:     strcpy(strType, "MED_INT"); break;
        default:          strcpy(strType, "UNKNOWN"); break;
    }
    
    pOs << "Field: " << endl;
    pOs << "    Name       =|" << pF.mName << "|" << endl;
    pOs << "    Entity     =" << strEntity << endl;
    pOs << "    Geom       =" << pF.mGeom << endl;
    pOs << "    Type       =" << strType << " (size=" << pF.mSizeOfType << ")" << endl;
    pOs << "    #Components=" << pF.mNumComponents << endl;
    pOs << "        Name component=|" << pF.mStrComponent << "|" << endl;
    pOs << "        Unit component=|" << pF.mStrUnit << "|" << endl;
    pOs << "    #Time steps=" << pF.mNGauss.size() << endl;
    
    for (unsigned itTimeStep = 0 ; itTimeStep < pF.mNGauss.size() ; itTimeStep++)
    {
        pOs << "        Time=" << pF.mDT[itTimeStep];
        pOs << "  it=" << pF.mNumDT[itTimeStep];
        pOs << "  order=" << pF.mNumO[itTimeStep];
        pOs << "  #gauss=" << pF.mNGauss[itTimeStep];
        pOs << "  #val=" << pF.mNVal[itTimeStep];
        pOs << "  sizeof_val=" << pF.mSizeOfData[itTimeStep];
        pOs << "  gauss_loc=|" << ((pF.mGaussLoc[itTimeStep].size() == 0)?"NONE":pF.mGaussLoc[itTimeStep]) << "|  size=" << pF.mGaussLoc[itTimeStep].size();
        pOs << "  profil=|" << ((pF.mProfil[itTimeStep].size() == 0)?"NONE":pF.mProfil[itTimeStep]) << "|  size=" << pF.mProfil[itTimeStep].size() << endl; 
        
        if (pF.mFlagPrintAll)
        {
            cout << "    Values: ";
            switch (pF.mType)
            {
            case MED_FLOAT64: 
                {
                    med_float* src = reinterpret_cast<med_float*>(pF.mVal[itTimeStep]);
                    for (int itVal = 0 ; itVal < pF.mNVal[itTimeStep] * pF.mNumComponents ; itVal++)
                    {
                        cout << src[itVal] << " ";
                    }  
                }
                break;
            case MED_INT:
            case MED_INT32:   
                {
                    med_int* src = reinterpret_cast<med_int*>(pF.mVal[itTimeStep]);
                    for (int itVal = 0 ; itVal < pF.mNVal[itTimeStep] * pF.mNumComponents ; itVal++)
                    {
                        cout << src[itVal] << " ";
                    }  
                }
                break;
            case MED_INT64:
                // not yet implemented
                throw UnsupportedOperationException("not yet implemented", __FILE__, __LINE__);
            default:          
                // should not be there
                throw IllegalStateException("should not be there", __FILE__, __LINE__);
            }
            cout << endl;
        }
        
    }
    
    return pOs;
}


} // namespace  multipr

// EOF
