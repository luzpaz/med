// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_MeshDis.hxx
 *
 * \brief   Class MeshDis: distributed mesh.
 *          = MASTER file (ASCII) -> list of sequential MED file.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_MESHDIS_HXX
#define MULTIPR_MESHDIS_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <iostream>
#include <vector>

// include MEDSPLITTER used to split mesh using METIS or SCOTCH
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_Topology.hxx"


namespace multipr
{

//*****************************************************************************
// Pre-declaration
//*****************************************************************************

class Mesh;


//*****************************************************************************
// Class MeshDisPart = a sub-part of a distributed mesh.
//
// It can be :
// - a sequential MED file representing a Group (scenario 1)
// - a sequential MED file representing a part of a Group (scenario 2 -> MEDSPLITTER)
// - a lower resolution of a part (decimation)
//*****************************************************************************

class MeshDisPart
{

public:

    /**
     * Action to be done for this part on next writing on disk.
     */
    enum OnNextWrite
    {
        MULTIPR_UNDEFINED,
        MULTIPR_KEEP_AS_IT,
        MULTIPR_WRITE_MESH,
        MULTIPR_WRITE_PARTS
    };
    
public:

    /**
     * Builds an empty part of a distributed mesh (default constructor).
     */
    MeshDisPart();
    
    /**
     * Destructor. Removes everything.
     */
    ~MeshDisPart();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    /**
     * Creates a MeshDisPart.
     * \param  pToDoOnNextWrite
     * \param  pMeshName
     * \param  pId
     * \param  pPartName
     * \param  pPath
     * \param  pMEDFileName file name excluding the path.
     * \param  pMesh can be NULL.
     */
    void create(
        OnNextWrite pToDoOnNextWrite,
        const char* pMeshName, 
        int         pId,
        const char* pPartName,
        const char* pPath,
        const char* pMEDFileName,
        Mesh*       pMesh);
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns the name of this part.
     * \return the name of this part.
     */
    const char* getPartName() const { return mPartName; }
    
    /**
     * Returns the name of the mesh of this part.
     * \return the name of the mesh of this part.
     */
    const char* getMeshName() const { return mMeshName; }
    
    /**
     * Returns the MED filename which contain this part.
     * \return the MED filename which contain this part.
     */
    const char* getMEDFileName() const { return mMEDFileName; }
    
    /**
     * Returns the suffix of the related MED filename (without .med extension).
     * For examples:
     * 1. "agregat100grains_12pas_grain97.med"                       -> "grain97"
     * 2. "agregat100grains_12pas_grain100_part2.med"                -> "grain100_part2"
     * 3. "aagregat100grains_12pas_grain98_gradmoy-low-25.0-0.3.med" -> "grain98_gradmoy-low-25-0.3"
     */
    const char* getMEDFileNameSuffix() const;
    
    /**
     * Returns the action to be performed on this part on next write.
     * \return the action to be performed on this part on next write.
     */
    OnNextWrite getOnNextWrite() const { return mToDoOnNextWrite; }
     
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Reads the sequentiel MED file corresponding to this part.
     * \throw  IOException if an i/o error occurs.
     */
    void readMED();
     
    /**
     * Dumps any MeshDisPart to the given output stream.
     * \param  pOs any output stream.
     * \param  pM  any MeshDisPart.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, MeshDisPart& pM);
    
private:

    // MeshDisPart can be used:
    // 1 (KEEP_AS_IT)  : To store data read from one line of an ASCII master file (distributed MED file)
    // 2 (WRITE_MESH)  : As a temporary structure to store all infos about a mesh corresponding to a group (before writing on disk).
    // 3 (WRITE_PARTS) : As a temporary structure to store all infos about splitting using MEDSPLITTER.
    
    OnNextWrite  mToDoOnNextWrite;               /**< See enum OnNextWrite. */
    
    //---------------------------------------------------------------------
    // Case 1, 2, and 3
    //---------------------------------------------------------------------        
    char   mMeshName[MED_TAILLE_NOM + 1];        /**< Name of the mesh. */
    int    mId;                                  /**< Id of this part in [1..n]. */
    char   mPartName[MED_TAILLE_NOM + 1];        /**< Name of this part. */
    char   mPath[256];                           /**< Path of the MED file. */
    char   mMEDFileName[256];                    /**< Name of the MED file which contain this part. */
    
    //---------------------------------------------------------------------
    // Case 2: mesh of the related sequential MED file (can be NULL if not loaded)
    //---------------------------------------------------------------------
    Mesh*  mMesh;                                /**< Mesh associated with this part; can be NULL. */
    
    //---------------------------------------------------------------------
    // Case 3 only: temporary result of MEDSPLITTER
    //---------------------------------------------------------------------
    int                          mSplit;         /**< For MEDSPLITTER: number of parts. Temporary. */
    MEDSPLITTER::MESHCollection* mCollection;    /**< New data after splitting. */
    MEDSPLITTER::MESHCollection* mOldCollection; /**< Data before splitting (we need them when we want to write new data on disk. */
    
private:

    // do not allow copy constructor
    MeshDisPart(const MeshDisPart&);
    
    // do not allow copy
    MeshDisPart& operator=(const MeshDisPart&);
    
    // do not allow operator ==
    bool operator==(const MeshDisPart&); 
    
    //---------------------------------------------------------------------
    // Friends
    //---------------------------------------------------------------------    
    friend class MeshDis;
    
}; // class MeshDisPart


//*****************************************************************************
// Class MeshDis
//*****************************************************************************

class MeshDis
{
public:

    /**
     * Builds an empty mesh (default constructor).
     */
    MeshDis();
     
    /**
     * Destructor. Removes everything.
     */
    ~MeshDis();
     
    /**
     * Resets this object in its state by default (empty). Clean memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------

    /**
     * Sets the name of the associated sequential MED file (=original MED file).
     * \param  pFilename name of the associated sequential MED file.
     */
    void setSequentialMEDFilename(const char* pFilename);
    
    /**
     * Returns the name of this sequential MED file (=original MED file).
     * \return the name of this sequential MED file (=original MED file).
     */
    const char* getSequentialMEDFilename() const { return mSequentialMEDFilename; }
       
    /**
     * Returns the name of this distributed MED file (=name of the master file).
     * \return the name of this distributed MED file (=name of the master file).
     */
    const char* getDistributedMEDFilename() const { return mDistributedMEDFilename; }
     
    /**
     * Adds a new part to this distributed mesh.
     * Used by the split process (extract groups).
     * \param  pToDoOnNextWrite
     * \param  pMeshName
     * \param  pId
     * \param  pPartName
     * \param  pPath
     * \param  pMEDFileName
     * \param  pMesh can be NULL.
     */
    void addMesh(
        MeshDisPart::OnNextWrite pToDoOnNextWrite,
        const char* pMeshName, 
        int         pId,
        const char* pPartName,
        const char* pPath,
        const char* pMEDFileName,
        Mesh*       pMesh);
    
    /**
     * Inserts a new part to this distributed mesh.
     * Used by the decimation process.
     * \param  pToDoOnNextWrite
     * \param  pMeshName
     * \param  pId
     * \param  pPartName
     * \param  pPath
     * \param  pMEDFileName
     * \param  pMesh can be NULL.
     * \param  pPosition insert after this position. Start at 1.
     */
    void insertMesh(
        MeshDisPart::OnNextWrite pToDoOnNextWrite,
        const char* pMeshName, 
        int         pId,
        const char* pPartName,
        const char* pPath,
        const char* pMEDFileName,
        Mesh*       pMesh,
        int         pPosition);
    
    /**
     * Removes all the part beginning by pPrefixPartName from this distributed mesh.
     * Example: if pPrefixPartName="PART_4" => remove "PART_4" and all sub-parts "PART_4_*", but not "PART41".
     * \param  pPrefixPartName name of the part.
     */
    void removeParts(const char* pPrefixPartName);
    
    /**
     * Returns the current number of parts in this distributed mesh.
     * \return the current number of parts in this distributed mesh.
     */
    int getNumParts() const { return mParts.size(); }
    
    /**
     * Returns the nth part of this distributed mesh.
     * \param  pIndex index of the part (in 0..getNumParts()-1).
     * \return the nth part of this distributed mesh.
     */
    MeshDisPart* getPart(int pIndex) const { return mParts[pIndex]; }
    
    /**
     * Returns the list of meshes contained in this distributed MED file.
     * \return the list of meshes contained in this distributed MED file.
     */
    std::vector<std::string> getMeshes() const; 
    
    /**
     * Returns the list of fields contained in this distributed MED file.
     * \return the list of fields contained in this distributed MED file.
     */
    std::vector<std::string> getFields() const; 
    
    /**
     * Returns the number of iteration for a given field.
     * \param  pFieldName field name.
     * \return the number of iteration for a given field.
     */
    int getTimeStamps(const char* pFieldName) const; 
    
    /**
     * Returns all information about a part.
     * \param  pPartName name of the part.
     * \return all information about a part.
     */
    std::string getPartInfo(const char* pPartName);
     
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------
    
    /**
     * Finds a part of this distributed mesh by its name.
     * Returns NULL if the part does not exist.
     * \param  pPartName part to be found; must not be NULL.
     * \return a pointer towards the part if it exists, NULL otherwise.
     * \throw  NullArgumentException if pPartName is NULL.
     */
    MeshDisPart* findPart(const char* pPartName);
    
    /**
     * Updates this distributed mesh by splitting one of its part.
     * This splitting method leans on medsplitter, by V. Bergeaud (CEA).
     * \param  pPartName     name of the part to be splitted.
     * \param  pNbParts      number of sub-parts.
     * \param  pPartitionner MULTIPR_METIS or MULTIPR_SCOTCH.
     * \throw  RuntimeException if any error occurs.
     */
    void splitPart(const char* pPartName, int pNbParts, int pPartitionner);
    
    /**
     * Creates 3 resolution (CURRENT = FULL, MEDIUM and LOW) of a part of this distributed mesh.
     * Names of new meshes are <original_name>_MED and <original_name>_LOW.
     * \param  pPartName
     * \param  pFielName
     * \param  pFieldIt
     * \param  pFilterName
     * \param  pTMed        threshold used to generate MEDIUM resolution.
     * \param  pTLow        threshold used to generate LOW resolution (must be >= pTMed).
     * \param  pRadius
     * \param  pBoxing number of cells along each axis; e.g. if 100 then grid will have 100*100*100 = 10**6 cells; 100 by default.
     * \throw  RuntimeException if any error occurs.
     */
    void decimatePart(
        const char* pPartName, 
        const char* pFieldName,
        med_int     pFieldIt,
        const char* pFilterName,
        med_float   pTMed, 
        med_float   pTLow,
        med_float   pRadius,
        int         pBoxing = 100);
        
    /**
     * Returns useful information to configure decimation parameters.
     * Depends on part, field and filter: generic operation.
     * \param  pPartName     name of the part.
     * \param  pFieldName    name of the field used for decimation.
     * \param  pFieldIt      iteration (time step) of the field.
     * \param  pFilterName   name of the filter to be used.
     * \param  pFilterParams params to be used with the filter (depends on filter; this string will be parsed).
     * \return 
     */
    std::string evalDecimationParams(
        const char* pPartName, 
        const char* pFieldName, 
        int         pFieldIt, 
        const char* pFilterName,
        const char* pFilterParams);
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Reads the master file of a distributed MED file.
     * \param  pMEDfilename
     * \throw  NullArgumentException if pMEDfilename is NULL.
     * \throw  IOException if any i/o error occurs.
     */
    void readDistributedMED(const char* pMEDfilename);
    
    /**
     * Writes this distributed MED file (including master file and sub MED files if necessary).
     * \param  pMEDfilenamePrefix
     * \throw  NullArgumentException if pMEDfilename is NULL.
     * \throw  IOException if any i/o error occurs.
     */
    void writeDistributedMED(const char* pMEDfilenamePrefix);
    
    /**
     * Dumps any MeshDis to the given output stream.
     * \param  pOs any output stream.
     * \param  pM  any MeshDis.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, MeshDis& pM);
    
private:

    /**
     * Recomputes the number of parts in this distributed mesh.
     * This method is used by writeDistributedMED().
     * \return the number of parts in this distributed mesh.
     */
    int computeNumParts();
    
private:

    char                      mSequentialMEDFilename[256];  /**< Name of the original MED file used to build distribyuted MED. */
    char                      mDistributedMEDFilename[256]; /**< Name of this distributed MED file (= name of the master file). */
    std::vector<MeshDisPart*> mParts;                       /**< Table of sub-parts; a distributed mesh is composed of N sub-part, where N = mParts.size(). */
    //MULTIPR_ProgressCallback*   mProgressCallback;
    
private:

    // do not allow copy constructor
    MeshDis(const MeshDis&);
    
    // do not allow copy
    MeshDis& operator=(const MeshDis&);
    
    // do not allow operator ==
    bool operator==(const MeshDis&); 
    
}; // class MeshDis


} // namespace MULTIPR


#endif // MULTIPR_MESHDIS_HXX

// EOF
