// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_GaussLoc.hxx
 *
 * \brief   Class MULTIPR_Obj. 
 *          This class is used as an interface to implement MULTIPR services in the salome MODULE 
 *          as described in the spec/design doc.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_OBJ_HXX
#define MULTIPR_OBJ_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

#include <iostream>
#include <string>
#include <vector>


namespace multipr
{

class MeshDis;


//*****************************************************************************
// Class Obj
//*****************************************************************************

enum ObjState
{
    MULTIPR_OBJ_STATE_ERROR,
    MULTIPR_OBJ_STATE_RESET,
    MULTIPR_OBJ_STATE_SEQ_INIT,
    MULTIPR_OBJ_STATE_SEQ,
    MULTIPR_OBJ_STATE_DIS,
    MULTIPR_OBJ_STATE_DIS_MEM
}; // enum ObjState


class Obj
{
public:

    /** 
     * Builds an empty Gauss reference (default constructor).
     */
    Obj();
    
    /**
     * Destructor. Removes everything.
     */
    ~Obj();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    /**
     * Associate a MED file (sequential or distributed) with this object.
     * This method also:
     * - reset everything before starting
     * - determine if the given file is a sequential (SEQ) or a distributed (DIS) MED file
     * - read the ASCII master file if it is a distributed MED file
     * - set state
     */
    void create(const char* pMEDfilename);
     
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //--------------------------------------------------------------------
    
    /**
     * Returns true iff this obj represents a valid sequential MED file.
     * \return true iff this obj represents a valid sequential MED file.
     */
    bool isValidSequentialMEDFile() const { return (mState == MULTIPR_OBJ_STATE_SEQ) || (mState == MULTIPR_OBJ_STATE_SEQ_INIT); }
    
    /**
     * Returns true iff this obj represents a valid distributed MED file.
     * \return true iff this obj represents a valid distributed  MED file.
     */
    bool isValidDistributedMEDFile() const { return (mState == MULTIPR_OBJ_STATE_DIS) || (mState == MULTIPR_OBJ_STATE_DIS_MEM); }
     
    /**
     * Returns the name of the associated MED file.
     * \return the name of the associated MED file.
     */
    std::string getMEDFilename() const { return mMEDfilename; }
    
    /**
     * Returns the name of the associated sequential MED file.
     * \return the name of the associated sequential MED file.
     */
    std::string getSequentialMEDFilename() const;
    
    /**
     * Defines the mesh to be processed.
     * \param  pMeshName name of the mesh to be partitionned.
     */
    void setMesh(const char* pMeshName);
     
    /**
     * Returns the list of meshes contained in the sequential MED file.
     * Assumes this object encapsulates a sequential MED file.
     * \return the list of meshes contained in the sequential MED file.
     */
    std::vector<std::string> getMeshes() const;
    
    /**
     * Returns the list of fields contained in the sequential MED file.
     * Assumes this object encapsulates a sequential MED file.
     * \return the list of fields contained in the sequential MED file.
     */
    std::vector<std::string> getFields() const;
    
    /**
     * Returns the number of timestamps for a given field.
     * Assumes this object encapsulates a sequential MED file.
     * \param  pFieldName name of any field.
     * \return the number of timestamps for a given field; 0 if field not found.
     */
    int getTimeStamps(const char* pFieldName) const;
    
    /**
     * Returns the name of all partitions.
     * Assumes this object encapsulates a distributed MED file.
     * \return the name of all partitions.
     */
    std::vector<std::string> getParts() const;
    
    /**
     * Returns all information about a part.
     * \param  pPartName name of the part.
     * \return all information about a part.
     */
    std::string getPartInfo(const char* pPartName) const;
    
    /**
     * Removes all the part beginning by pPrefixPartName from the distributed MED file.
     * Example: if pPrefixPartName="PART_4" => remove "PART_4" and all sub-parts "PART_4_*", but not "PART41".
     * Assume this object encapsulates a distributed MED file.
     * \param  pPrefixPartName name of the part.
     */
    void removeParts(const char* pPrefixPartName);
    
      
    //---------------------------------------------------------------------
    // Algorithms
    //--------------------------------------------------------------------
    
    /**
     * Creates a distributed MED file (v2.3) by extracting all the groups from the current mesh of the current MED sequential MED file.
     *         Assumes:
     *         - the file is in MED format and can be read using MED file v2.3.
     *         - the file is sequential (not a distributed MED).
     *         - the file only contains TETRA10 elements (dimension of space and mesh is 3).
     *         - the file have no profil.
     * \return the name of each part.
     * \throw  RuntimeException if any error occurs.
     */
    std::vector<std::string> partitionneDomaine();
    
    /** 
     * Creates a distributed MED file (V2.3) by splitting a group of a MED file previously created by partitionneDomaine.
     *         Assumes:
     *         - the file is a distributed MED file, previously created by partitionneDomaine()
     *           (=> each part only contain 1 mesh, TETRA10 elements only)
     *         - nbPart > 1
     * \param  pPartName     name of the part to be splitted.
     * \param  pNbParts      number of parts; must be > 1.
     * \param  pPartitionner use value 0=MULTIPR_METIS for Metis or 1=MULTIPR_SCOTCH for Scotch.
     * \return the name of each part.
     * \throw  RuntimeException if any error occurs.
     */
    std::vector<std::string> partitionneGrain(
        const char* pPartName, 
        int         pNbParts, 
        int         pPartitionner=0);
    
    /**
     * Creates 3 resolutions of the given part of a distributed MED file (V2.3).
     *         Assumes:
     *         - the file is a distributed MED file, previously created by partitionneDomaine() or partitionneGrain()
     *           (=> each part only contain 1 mesh, TETRA10 elements only)
     * \param  pPartName    name of the part to be decimated.
     * \param  pFieldName   name of the field used for decimation.
     * \param  pFieldIt     iteration (time step) of the field.
     * \param  pFilterName  name of the filter to be used.
     * \param  pTmed        threshold used for medium resolution.
     * \param  pTlow        threshold used for low resolution; tmed must be less than tlow
     * \param  pTadius      radius used to determine the neighbourhood.
     * \param  pBoxing      number of cells along each axis; must be >= 1; e.g. if 100 then acceleration grid will have 100*100*100 = 10**6 cells.
     * \return the name of each part.
     * \throw  RuntimeException if any error occurs.
     */
    std::vector<std::string> decimePartition(
        const char* pPartName,
        const char* pFieldName,
        int         pFieldIt,
        const char* pFilterName,
        double      pTmed,
        double      pTlow,
        double      pRadius,
        int         pBoxing);
    
    /**
     * Returns useful information to configure decimation parameters.
     * Depends on part, field and filter: generic operation.
     * \param  pPartName     name of the part.
     * \param  pFieldName    name of the field used for decimation.
     * \param  pFieldIt      iteration (time step) of the field.
     * \param  pFilterName   name of the filter to be used.
     * \param  pFilterParams params to be used with the filter (depends on filter; this string will be parsed).
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
     * Saves the associated MED file if necessary.
     * \param  pPath path where to save the file.
     * \throw  IOException if any i/o error occurs.
     */
    void save(const char* pPath);

    /**
     * Dumps any Obj to the given output stream.
     * \param  pOs any output stream.
     * \param  pO  any Obj.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Obj& pO);
    
private:

    /**
     * Returns the list of parts contained in the current distributed mesh.
     * \return the list of parts contained in the current distributed mesh.
     * \throw  IllegalStateException if the distributed mesh does not still exist.
     */
    std::vector<std::string> getListParts() const;

private:
    
    std::string  mMEDfilename;     /**< Name of the MED file: sequential or distributed. */
    std::string  mMeshName;        /**< Mesh to be partitionned. */
    ObjState     mState;           /**< State of this object. */
    MeshDis*     mMeshDis;         /**< Distributed mesh. */
    
    
private:
    
    // do not allow copy constructor
    Obj(const Obj&);
    
    // do not allow copy
    Obj& operator=(const Obj&);
    
    // do not allow operator ==
    bool operator==(const Obj&); 
    
}; // class Obj


} // namespace MULTIPR


#endif // MULTIPR_OBJ_HXX

// EOF
