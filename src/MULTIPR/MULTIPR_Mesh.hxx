// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_Mesh.hxx
 *
 * \brief   Class Mesh used by the MULTIPR API; used to wrap MED file meshes.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_MESH_HXX
#define MULTIPR_MESH_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <string>


namespace multipr
{

//*****************************************************************************
// Pre-declaration
//*****************************************************************************

class GaussLoc;
class Profil;
class Nodes;
class Elements;
class Family;
class Field;
class Group;
class MeshDis;
class PointOfField;


//*****************************************************************************
// Class Mesh
//*****************************************************************************

/**
 * Assumes:
 * - 3D mesh in a 3D space
 * - Unstructured mesh (not a grid)
 * - Nodal connectivity
 * - Cartesian coordinates system
 * Always use FULL_INTERLACE arrays
 */
class Mesh
{
public:

    /**
     * Builds an empty Mesh (default constructor).
     */
    Mesh();
     
    /**
     * Destructor. Removes everything.
     */
    ~Mesh();
     
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns the name of this Mesh.
     * \return the name of this Mesh.
     */
    const char* getName() const { return mMeshName; }
    
    /**
     * Returns the name of all the scalar fields.
     * \return the name of all the scalar fields.
     */
    std::vector<std::string> getNameScalarFields() const; 
    
    /**
     * Returns the number of iteration for a given field.
     * \return the number of iteration for a given field.
     */
    int getTimeStamps(const char* pFieldName) const; 
    
    /**
     * Returns a Field from its name; NULL if it does not exist.
     * \param  pFieldName name of the field to be retrieved.
     * \return the Field pFieldName of it exists, NULL otherwise.
     * \throw  NullArgumentException if pFieldName is NULL.
     */
    Field* getFieldByName(const char* pFieldName) const;
    
    /**
     * Returns a GaussLoc from its name; NULL if it does not exist.
     * \param  pGaussLocName name of the GaussLoc to be retrieved.
     * \return the GaussLoc pGaussLocName if it exists, NULL otherwise.
     * \throw  NullArgumentException if pGaussLocName is NULL.
     */
    GaussLoc* getGaussLocByName(const char* pGaussLocName) const;    
     
    /**
     * Returns the number of elements.
     * \return the number of elements.
     */
    int getNumberOfElements() const;
    
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------
    
    /**
     * Creates a Mesh from a subset of its elements (cells).
     * \param  pSetOfElements subset of elements to keep.
     * \param  pNewMeshName   name of the new Mesh.
     * \return a new Mesh which is a restriction of this Mesh to the given set of elements.
     * \throw  NullArgumentException if pNewMeshName is NULL.
     */
    Mesh* createFromSetOfElements(const std::set<med_int>& pSetOfElements, const char* pNewMeshName);
    
    /**
     * Creates a Mesh from one of its group.
     * \param  pGroup       any group of this Mesh.  
     * \param  pNewMeshName name of the new Mesh.
     * \return a new Mesh which is a restriction of this Mesh to pGroup.
     * \throw  NullArgumentException if pGroup or pNewMeshName is NULL.
     */
    Mesh* createFromGroup(const Group* pGroup, const char* pNewMeshName);
    
    /**
     * Creates a Mesh by merging this one with the given one.
     * Warning: not all the data are merged (e.g. bounding box if not computed and family/groups are partially filled).
     * This method is intended to be used to build mesh for visualization (VISU integration).
     * Merge is partial to improve speed.
     * \param  pMesh any Mesh
     * \return a new Mesh which is a the union of this and pMesh.
     * \throw  NullArgumentException if pGroup or pNewMeshName is NULL.
     */
    Mesh* mergePartial(const Mesh* pMesh);
    Mesh* mergePartial(std::vector<Mesh*> pMeshes, const char* pFieldName, int pFieldIt);
    
    /**
     * Creates a distributed mesh (MeshDis) by creating a new mesh for each group of elements in this Mesh.
     * \return a distributed mesh from groups of this Mesh.
     */
    MeshDis* splitGroupsOfElements();
    
    /**
     * Creates a new mesh by decimating this one.
     * \param  pFilterName  name of the filter to be used for decimation (e.g. Filtre_GradientMoyen); should not be NULL.
     * \param  pArgv        all the arguments for filtering as a single string.
     * \param  pNameNewMesh name of the new mesh.
     * \return the decimated mesh.
     * \throw  NullArgumentException if one of the arguments is NULL.
     * \throw  RuntimeException if any error occurs while decimating data.
     */
    Mesh* decimate(
        const char* pFilterName,
        const char* pArgv,
        const char* pNameNewMesh);
    
    /**
     * Gets all the points in a field. Each point has coordinates and a value.
     * \param  pField      any field of this Mesh.
     * \param  pTimeStepIt time step iteration.
     * \param  pPoints     (out) list of points.
     * \throw  NullArgumentException if pField is NULL.
     * \throw  IllegalArgumentException if pTimeStepIt is invalid.
     */
    void getAllPointsOfField(Field* pField, int pTimeStepIt, std::vector<PointOfField>& pPoints);
    
    /**
     * Returns a default value for neighborhood radius.
     * Return value is such that, for any point in the field, average number of neighbours is pN.
     * \param  pN average number of neighbours.
     * \return a default value for neighborhood radius; 1.0 if some error occurs.
     */
    float evalDefaultRadius(int pN) const;
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Reads a Mesh from a sequential MED file. Resets the object before.
     * \param  pMEDfilename
     * \param  pMeshName
     * \throw  IOException if any i/o error occurs.
     */
    void readSequentialMED(const char* pMEDfilename, const char* pMeshName);
    
    /**
     * Writes this Mesh and all related things into a MED file.
     * \param  pMEDfilename
     * \throw  IOException if any i/o error occurs.
     */
    void writeMED(const char* pMEDfilename);
    
    /**
     * Sets the flag which control the stream operator <<.
     * \param pFlag new flag value.
     */
    void setPrintAll(bool pFlag) { mFlagPrintAll = pFlag; } 
    
    /**
     * Dumps any Mesh to the given output stream.
     * \param  pOs any output stream.
     * \param  pM  any Mesh.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Mesh& pM);
    
private:
     
    /**
     * Reads all Gauss localizations in the current MED file.
     * \throw  IOException if an i/o error occurs.
     */
    void readGaussLoc();
     
    /**
     * Reads families in the currentMED file and build groups.
     * \throw  IOException if an i/o error occurs.
     */
    void readFamilies();
    
    /**
     * Finalizes the constructions of families and groups. 
     * Fill structures with elements.
     */
    void finalizeFamiliesAndGroups();
    
    /**
     * Reads fields related to this mesh in the current MED file.
     * \throw  IOException if an i/o error occurs.
     */
    void readFields();
    
private:

    /**
     * Name of the associated MED file.
     */
    char                              mMEDfilename[256];
    
    /**
     * MED file handle.
     */
    med_idt                           mMEDfile;
    
    /**
     * Name of this mesh.
     */
    char                              mMeshName[MED_TAILLE_NOM + 1];
    
    /**
     * Universal name of this mesh.
     */ 
    char                              mMeshUName[MED_TAILLE_DESC + 1];
    
    /**
     * Description.
     */
    char                              mMeshDesc[MED_TAILLE_DESC + 1]; 
    
    /**
     * Dimension.
     */
    med_int                           mMeshDim;
    
    /**
     * Type of mesh (MED_NON_STRUCTURE or MED_STRUCTURE (=grid)) 
     */
    med_maillage                      mMeshType;
    
    /** 
     * Axis aligned bounding box of this mesh.
     */
    med_float                         mMeshBBoxMin[3];
    med_float                         mMeshBBoxMax[3];

    /**
     * All the nodes used by this mesh. 
     */
    Nodes*                            mNodes;
    
    /**
     * All the TETRA10 elements used by this mesh.
     */
    Elements*                         mElements;
    
    /**
     * Table of families used by this mesh.
     */
    std::vector<Family*>              mFamilies;
    
    /**
     * Map to retrieve a Family from its id.
     */
    std::map<med_int, Family*>        mFamIdToFam;
    
    /**
     * Table of groups used by this mesh.
     */
    std::vector<Group*>               mGroups;
    
    /**
     * Map to retrieve a Group from its name.
     */
    std::map<std::string, Group*>     mGroupNameToGroup;
    
    /**
     * Table of GaussLoc. 
     */
    std::vector<GaussLoc*>            mGaussLoc; 
    
    /**
     * Map to retrieve a Gauss info from its name.
     */
    std::map<std::string, GaussLoc*>  mGaussLocNameToGaussLoc;
    
    /**
     * Table of fields related to this mesh.
     * Number of fiels = mFields.size().
     */
    std::vector<Field*>               mFields;
    
    /**
     * Table of profils.
     */
    std::vector<Profil*>              mProfils;
    
    /** 
     * Flag to control the behaviour of the stream operator <<. 
     */
    bool                              mFlagPrintAll;
    
private:

    // do not allow copy constructor
    Mesh(const Mesh&);
    
    // do not allow copy
    Mesh& operator=(const Mesh&);
    
    // do not allow operator ==
    bool operator==(const Mesh&); 
    
}; // class Mesh


} // namespace MULTIPR


#endif // MULTIPR_MESH_HXX

// EOF
