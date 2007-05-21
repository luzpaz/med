// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_Field.hxx
 *
 * \brief   Class Field used to wrap MED file fields.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_FIELD_HXX
#define MULTIPR_FIELD_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <string>
#include <vector>
#include <set>


namespace multipr
{


//*****************************************************************************
// Class Field
//*****************************************************************************

class Field
{
public:

    /** 
     * Builds an empty field (default constructor).
     */
    Field();
    
    /**
     * Destructor. Removes everything.
     */
    ~Field();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns true iff this field is empty (no data).
     * \return true if this field is empty, false otherwise.
     */
    bool isEmpty() const;
    
    /**
     * Returns true iff this field apply on nodes (otherwise, information are related to elements).
     * \return true iff this field apply on nodes.
     */
    bool isFieldOnNodes() const { return (mEntity == MED_NOEUD); }
    
    /**
     * Returns the name of this Field.
     * \return the name of this Field.
     */
    const char* getName() const { return mName; }
    
    /**
     * Returns the number of time steps in this Field.
     * \return the number of time steps in this Field.
     */
    int getNumberOfTimeSteps() const { return mNGauss.size(); }
    
    /**
     * Returns the numeric type of information contained in this Field (e.g. MED_FLOAT64).
     * \return the numeric type of this Field.
     */
    med_type_champ getType() const { return mType; }
    
    /**
     * Returns the number of components of this Field.
     * \return the number of components of this Field.
     */
    int getNumberOfComponents() const { return mNumComponents; }
    
    /**
     * Returns the number of Gauss points for each element at the given time step.
     * \param  pTimeStepIt iteration of the field; must be in [1..MAX_ITERATION].
     * \return number of Gauss points in the elements of this Field at the given iteration.
     * \throw  IndexOutOfBoundsException if pTimeStepIt not in [1..MAX_ITERATION].
     */
    int getNumberOfGaussPointsByElement(int pTimeStepIt) const;
    
    /**
     * Returns the name of the Gauss localization related to the given iteration.
     * \param  pTimeStepIt iteration of the field; must be in [1..MAX_ITERATION].
     * \return the name of the Gauss localization related to the given iteration.
     * \throw  IndexOutOfBoundsException if pTimeStepIt not in [1..MAX_ITERATION].
     */
    const std::string& getNameGaussLoc(int pTimeStepIt) const;
    
    /**
     * Returns a pointer towards the first value of this Field for the given element at the given time step.
     * \param  pTimeStepIt iteration of the field; must be in [1..MAX_ITERATION].
     * \param  pIndex      index of the element; must be >= 1.
     * \return the value of this Field for the given element at the given time step.
     * \throw  IndexOutOfBoundsException if pTimeStepIt or pIndex are invalid.
     */
    const unsigned char* getValue(int pTimeStepIt, int pIndex) const;
    
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------
    
    /**
     * Creates a subset of this Field restricted to a set of elements (NODES or CELLS).
     * This method performs a projection of this Field on the given set of elements.     
     * \param  pSetIndices WARNING: indices start at 1.
     * \return a new Field restricted to the given set of elements.
     */
    Field* extractSubSet(const std::set<med_int>& pSetIndices) const;
    
    /**
     * Adds the set of GaussLoc used by this Field into the given set.
     * \param  pSetOfGauss any set of Gauss localisation.
     */
    void getSetOfGaussLoc(std::set<std::string>& pSetOfGaussLoc) const;
    
    /**
     * Builds a new field by merging data of this and pField if they are compatible.
     * \param  pField the field to be merged with this.
     * \throw  IllegalStateException if the two fields are incompatible.
     */
    Field* merge(Field* pField);
    Field* merge(std::vector<Field*> pFields, int pFieldIt = -1);

    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Reads a field from a MED file.
     * If the field is not related to the given mesh, the result is an empty field.
     * \param  pMEDfile  any valid MED file opened for reading.
     * \param  pIndex    index of the field to be read; must be >= 1.
     * \param  pMeshName name of the mesh (a field is always related to a mesh).
     * \throw  IOException if any i/o error occurs.
     */
    void readMED(med_idt pMEDfile, med_int pIndex, char* pMeshName);
    
    /**
     * Writes this field to a MED file. The field is linked to the given mesh.
     * WARNING: all related Gauss info must have been added to the MED file before.
     * \param  pMEDfile  any valid MED file opened for writing.
     * \param  pMeshName name of the mesh.
     * \throw  IOException if any i/o error occurs.
     */
    void writeMED(med_idt pMEDfile, char* pMeshName);
    
    /**
     * Sets the flag which control the stream operator <<.
     * \param  pFlag new flag value.
     */
    void setPrintAll(bool pFlag) { mFlagPrintAll = pFlag; } 
    
    /**
     * Dumps any Field to the given output stream.
     * \param  pOs any output stream.
     * \param  pF  any Field.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Field& pF);
    
private:

    /**
     * Reads all the information related to its field and a given time step (by its index).
     * \param  pMEDfile           MED file handle.
     * \param  pNumberOfTimeSteps number of timesteps (iteration) to be read.
     * \param  pMeshName          name of the mesh.
     * \throw  IOException if any i/o error occurs.
     */
    void readMEDtimeSteps(med_idt pMEDfile, med_int pNumberOfTimeSteps, char* pMeshName);
    
private:
    
    char                         mName[MED_TAILLE_NOM + 1];
    med_entite_maillage          mEntity;      // type of entity, e.g. MED_MAILLE
    med_geometrie_element        mGeom;        // type of primitives, e.g. MED_TETRA10 (MED_NONE for a field on nodes)
    med_type_champ               mType;        // type of field, e.g. MED_FLOAT64, MED_INT32
    int                          mSizeOfType;  // 8 for MED_FLOAT64, 4 for MED_INT32, etc.
    med_int                      mNumComponents;
    std::string                  mStrComponent;
    std::string                  mStrUnit;
    
    // Information related to time steps.
    // Each vector should contain the same number of elements.
    // Number of time step = mNGauss.size() = mDT.size() = ...
    
    std::vector<med_int>         mNGauss;        /**< For each time step, number of Gauss points in the field. */
    std::vector<med_float>       mDT;            /**< For each time step, value of time step. */
    std::vector<med_int>         mNumDT;         /**< For each time step, iteration number. */
    std::vector<std::string>     mDTUnit;        /**< For each time step, units. */
    std::vector<med_int>         mNumO;          /**< For each time step, order number. */
    std::vector<std::string>     mGaussLoc;      /**< For each time step, name of Gauss localization to be used (empty if none). */
    std::vector<std::string>     mProfil;        /**< For each time step, name of the profil to be used (empty if none). */
    std::vector<int>             mSizeOfData;    /**< For each time step, sizeof data (mVal) in bytes. */
    std::vector<med_int>         mNVal;          /**< For each time step, number of values. */
    std::vector<unsigned char*>  mVal;           /**< For each time step, raw data; can be MED_FLOAT64, MED_INT32, etc. see mType. */

    bool                         mFlagPrintAll;  /**< Flag to control the behaviour of the stream operator <<. */
    
private:

    // do not allow copy constructor
    Field(const Field&);
    
    // do not allow copy
    Field& operator=(const Field&);
    
    // do not allow operator ==
    bool operator==(const Field&); 
    
}; // class Field


} // namespace MULTIPR


#endif // MULTIPR_FIELD_HXX

// EOF
