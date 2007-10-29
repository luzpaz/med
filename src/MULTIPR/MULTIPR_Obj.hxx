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
#include <list>

#include <boost/thread/recursive_mutex.hpp>

#include "MULTIPR_Globals.hxx"

namespace multipr
{

class MeshDis;
class Group;
class Profil;

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
     * Returns the name of mesh, defined via setMesh().
     * \return the name of mesh, defined via setMesh().
     */
    std::string getMeshName() const { return mMeshName; }
    
    /**
     * Returns the list of meshes contained in the sequential MED file.
     * Assumes this object encapsulates a sequential MED file.
     * \return the list of meshes contained in the sequential MED file.
     */
    std::vector<std::string> getMeshes() const;
    
    /**
     * Returns the list of fields contained in the sequential MED file.
     * Assumes this object encapsulates a sequential MED file.
     * \param pPartList The list of parts to get the fields from (separator is '|').
     * \return the list of fields contained in the sequential MED file.
     */
    std::vector<std::string> getFields(const char* pPartList) const;
    
    /**
     * Returns the number of timestamps for a given field.
     * Assumes this object encapsulates a sequential MED file.
     * \param  pFieldName name of any field.
     * \param pPartList The list of parts to get the time stamps from (separator is '|').	 
     * \return the number of timestamps for a given field; 0 if field not found.
     */
    int getTimeStamps(const char* pPartList, const char* pFieldName) const;

    /**
     * Get the minimum and maximum value of a part's field.
     * \param pPartName The name of the part.
     * \param pFieldName The name of the field.
     * \param pMin The mininum value to fill.
     * \param pMax The maxinum value to fill.
     */
    void getFieldMinMax(const char* pPartName, const char* pFieldName, float& pMin, float& pMax);
        
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
    std::vector<std::string> partitionneGroupe(
        const char* pPartName, 
        int         pNbParts, 
        int         pPartitionner=0);
    
    /**
     * Creates 3 resolutions of the given part of a distributed MED file (V2.3).
     *         Assumes:
     *         - the file is a distributed MED file, previously created by partitionneDomaine() or partitionneGroupe()
     *           (=> each part only contain 1 mesh, TETRA10 elements only)
     * \param  pPartName    name of the part to be decimated.
     * \param  pFieldName   name of the field used for decimation.
     * \param  pFieldIt     iteration (time step) of the field.
     * \param  pFilterName  name of the filter to be used.
     * \param  pFilterParams params to be used with the filter (depends on filter; this string will be parsed).
     * \return the name of each part.
     * \throw  RuntimeException if any error occurs.
     */
    std::vector<std::string> decimePartition(
        const char* pPartName,
        const char* pFieldName,
        int         pFieldIt,
        const char* pFilterName,
        const char* pFilterParam);

    /**
     * Works exactly like the above method and has the same parameters
     * and returning value meaning, except the last additional parameter.
     * \param  pEmptyMeshes list of names of empty meshes, corresponding to medium and/or lower resolution.
     */
    std::vector<std::string> decimePartition(
        const char* pPartName,
        const char* pFieldName,
        int         pFieldIt,
        const char* pFilterName,
        const char* pFilterParam,
        std::list<std::string>& pEmptyMeshes);

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
    
	/**
	 * Get mesh statistics.
	 * \param pStats [the number of mesh] space [number of gauss points]
	 * \param pPartName The path to the part.
	 */
	void getMEDInfo(char* pStats, char* pPartName);
	
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
     * Reset save progress to zero.
     * \param pPercents current save progress in percents.
     */
    void resetProgress();

    /**
     * Obtain save progress.
     * \return current save progress in percents.
     */
    int getProgress();

    /**
     * Saves the associated MED file to the given location.
     * Calling this method does not influence the object state.
     *
     * \note This method is mentioned to be used only in persistence.
     *
     * \param  pPath path where to save the file.
     * \throw  IOException if any i/o error occurs.
     */
    void savePersistent(const char* pPath);

    /**
     * Works like \a create(), but assumes that the distributed MED file was moved
     * from the original location and its ASCII master file needs to be updated.
     *
     * \note This method is mentioned to be used only in persistence.
     */
    void restorePersistent(const char* pMEDfilename);
    
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
    
    std::string            mMEDfilename;      /**< Name of the MED file: sequential or distributed. */
    std::string            mMeshName;         /**< Mesh to be partitionned. */
    ObjState               mState;            /**< State of this object. */
    MeshDis*               mMeshDis;          /**< Distributed mesh. */
    bool                   mDoReadWriteFields;/**< Flag for optimized domain split. */
    std::vector<Group*>    mGroups;           /**< We need to keep the groups for optimized domain split. */
    GaussIndexList         mGaussList;        /**< We need to keep gauss information for optimized domain split. */
    std::vector<Profil*>   mProfils;          /**< We need to keep the profiles for optimized domain split. */
    boost::recursive_mutex mWriteMutex;       /**< Write progress thread safe access. */

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
