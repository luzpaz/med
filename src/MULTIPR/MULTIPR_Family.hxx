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
 * \file    MULTIPR_Family.hxx
 *
 * \brief   Class Family, Group and Attributs used to wrap MED file structures.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_FAMILY_HXX
#define MULTIPR_FAMILY_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}


#include <iostream>
#include <vector>
#include <set>
#include <map>

#include "MULTIPR_Mesh.hxx"

namespace multipr
{

//*****************************************************************************
// Predeclaration
//*****************************************************************************

class Group;


//*****************************************************************************
// Class Attributs
//*****************************************************************************

class Attributs
{
public:
    
    /** 
     * Builds an empty Attributs (default constructor).
     */
    Attributs();
    
    /**
     * Destructor. Removes everything.
     */
    ~Attributs();
    
    /**
     * Assignment operator (deep copy).
     * \param  pAttr any Atttibuts to be copied.
     * \return a reference towards the copy.
     */
    Attributs& operator=(const Attributs& pAttr);
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Dumps any Attributs to the given output stream.
     * \param  pOs any output stream.
     * \param  pA  any Attributs.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Attributs& pA);
    
public:

    med_int   mNum;     /**< Number of attributes. */
    med_int*  mId;      /**< Table of identifiers: for each attribute, its identifier. */
    med_int*  mVal;     /**< Table of values; for each attribute, its value. */
    char*     mDesc;    /**< Table of description; for each attribute, its description. */
    
private:

    // do not allow copy constructor
    Attributs(const Attributs&);
    
    // do not allow operator ==
    bool operator==(const Attributs&); 
    
}; // class Attribut


//*****************************************************************************
// Class Family
//*****************************************************************************

class Family
{
public:

    /** 
     * Builds an empty Family (default constructor).
     */
    Family();
    
    /**
     * Copy constructor.
     */
    Family(const Family& pFamily);
    
    /**
     * Destructor. Removes everything.
     */
    ~Family();    
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns the identifier (= number) of this Family.
     * \return the identifier of this Family.
     */
    med_int getId() const { return mId; }
    
    /**
     * Set the identifier of this family.
     * \param pId The identifier.
     */
    void    setId(med_int pId) { mId = pId; };
     
    /**
     * Returns the name of this Family.
     * \return the name of this Family.
     */
    const char* getName() const { return mName; }
    
    /**
     * Returns true if this Family if a family of nodes, false if is a family of elements (=cells)).
     * \return true iff this Family if a family of nodes.
     */
    bool isFamilyOfNodes() const { return mIsFamilyOfNodes; }
    
    /**
     * Sets whether this Family is a family of nodes or family of elements (= cells).
     * \param  pIsFamilyOfNodes flag.
     */
    void setIsFamilyOfNodes(bool pIsFamilyOfNodes) { mIsFamilyOfNodes = pIsFamilyOfNodes; }
    
    /**
     * Inserts a new element (by its index) into this Family.
     * \param  pIndexElt index of the element to be added; must be >= 1.
         * \param  pMeshIndex Index of the mesh (eMED_TETRA4 etc...). Default value when its a family of nodes.
     * \throw  IllegalArgumentException if pIndexElt <= 0.
     */
    void insertElt(med_int pIndexElt, eMeshType pMeshIndex = eMED_POINT1); 
    
    /**
     * Returns the number of elements in this Family.
     * \return the number of elements in this Family.
     */
    int getSize(eMeshType pMeshIndex = eMED_POINT1) const { return mElt[pMeshIndex].size(); }
    
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------

    /**
     * Adds all the groups of this Family into the set of groups described by (pGroups, pGroupNameToGroup).
     * \param  pGroups            current list of groups.
     * \param  pGroupNameToGroup  table (map) to retrieve a Group* from its name.
     */
    void buildGroups(
        std::vector<Group*>&            pGroups, 
        std::map<std::string, Group*>&  pGroupNameToGroup) const;
    
    /**
     * Constructor. Returns a copy of this family restricted to the given group if pGroupName != NULL.
     * Examples: 
     * 1. If current family have 3 groups "A", "B" and "C" and pGroupName="B", then the new family will only reference the group "B"
     * 2. If current family have 3 groups "A", "B" and "C" and pGroupName="D", then the new family will reference groups "A", "B" and "C".
     * WARNING: elements are not copied
     * \param   pGroupName name of the group to keep.
     * \return  a copy of this family.
     */
    Family* extractGroup(const char* pGroupName);
    
        /**
     * Returns the set of all the elements referenced in this Family.
     * \return the set of all the elements referenced in this Family.
     */
    const std::set<med_int>& getSetOfElt(eMeshType pMeshIndex) const { return mElt[pMeshIndex]; }
        
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------

    /**
     * Reads a Family from a MED file.
     * \param  pMEDfile  any valid MED file opened for reading.
     * \param  pMeshName name of the mesh. 
     * \param  pIndex    index of the family to be read (must be >= 1).
     * \throw  IOException if any i/o error occurs.
     */
    void readMED(med_idt pMEDfile, char* pMeshName, med_int pIndex);
    
    /**
     * Writes this Family to a MED file.
     * WARNING: mesh must have been created and added to the MED file before.
     * \param  pMEDfile  any valid MED file opened for writing.
     * \param  pPeshName name of the mesh.
     * \throw  IOException if any i/o error occurs.
     */
    void writeMED(med_idt pMEDfile, char* pMeshName);
    
    /**
     * Sets the flag which control the stream operator <<.
     * \param  pFlag new flag value.
     */
    void setPrintAll(bool pFlag) { mFlagPrintAll = pFlag; } 
    
    /**
     * Dumps any Family to the given output stream.
     * \param  pOs any output stream.
     * \param  pF  any Family.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Family& pF);
    
private:
    
    char                     mName[MED_TAILLE_NOM + 1];   /** Name of the Family */
    med_int                  mId;               /**< Id: > 0 if family of nodes; < 0 if family of elements. */
    std::set<med_int>        mElt[eMaxMedMesh]; /**< Set of all the elements (by their index in 1..*). */
    std::string              mStrNameGroups;    /**< A string with name of all groups which contain the Family. */
    std::vector<std::string> mNameGroups;       /**< List of groups (by name) which contain the Family. */
    Attributs                mAttributs;        /**< All the attributed related to the Family. */
    bool                     mIsFamilyOfNodes;  /**< Is it a family of nodes or a family of elements? */
    
    bool                     mFlagPrintAll;     /** Flag to control the behaviour of the stream operator <<. */
    
private:

    // do not allow copy constructor
    //Family(const Family&);
    
    // do not allow copy
    Family& operator=(const Family&);
    
    // do not allow operator ==
    bool operator==(const Family&); 
    
}; // class Family


//*****************************************************************************
// Class Group
//*****************************************************************************

class Group
{
public:

    /** 
     * Builds an empty group (default constructor).
     */
    Group();
    
    /**
     * Destructor. Removes everything.
     */
    ~Group();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns true if it is a group of nodes, false if it is a group of elements.
     * \return true if it is a group of nodes, false if it is a group of elements.
     */
    bool isGroupOfNodes() const { return mIsGroupOfNodes; }
    
    /**
     * Sets whether it is a group of nodes or a group of elements. 
     * \param  pIsGroupOfNodes true for a group of nodes, false for a group of elements.
     */
    void setIsGroupOfNodes(bool pIsGroupOfNodes) { mIsGroupOfNodes = pIsGroupOfNodes; }
    
    /**
     * Returns the name of this Group.
     * \return the name of this Group.
     */
    const std::string& getName() const { return mName; }
     
    /**
     * Sets the name of this Group.
     * \param  pName new name of this Group (length of name must not exceed MED_TAILLE_LNOM).
     */
    void setName(const std::string& pName);
    
    /**
     * Adds the index of a new element to this Group.
     * \param  pIndexElt must be >= 1.
     * \param pMeshIndex The index of the mesh type.
     */
    void insertElt(med_int pIndexElt, eMeshType pMeshIndex);
    
    /**
     * Set a complete set of elements.
     * \param pMeshIndex The index of the mesh type.
     * \param pSetOfElt The set of elements.
     */
    void setSetOfElt(eMeshType pMeshIndex, const std::set<med_int>& pSetOfElt) { mElt[pMeshIndex] = pSetOfElt; };
    
    /**
     * Returns the set of all the elements referenced in this Group.
     * \return the set of all the elements referenced in this Group.
     */
    const std::set<med_int>& getSetOfElt(eMeshType pMeshIndex) const { return mElt[pMeshIndex]; }
    
    /**
     * Returns the number of elements referenced in this Group.
     * \return the number of elements referenced in this Group.
     */
    int getSize(eMeshType pMeshIndex) const { return mElt[pMeshIndex].size(); }
        
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Sets the flag which control the stream operator <<.
     * \param  pFlag new flag value.
     */
    void setPrintAll(bool pFlag) { mFlagPrintAll = pFlag; } 
    
    /**
     * Dumps any Group to the given output stream.
     * \param  pOs any output stream.
     * \param  pG  any Group.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Group& pG);
    
private:
    
    std::string        mName;            /**< Name of the group. */
    std::set<med_int>  mElt[eMaxMedMesh];             /**< All elements of this group; each element is referenced by its index >= 1; each element is unique. */
    bool               mIsGroupOfNodes;  /**< Is it a group of nodes or a group of elements? */
    
    bool               mFlagPrintAll;    /** Flag to control the behaviour of the stream operator <<. */
        
private:

    // do not allow copy constructor
    Group(const Group&);
    
    // do not allow copy
    Group& operator=(const Group&);
    
    // do not allow operator ==
    bool operator==(const Group&); 
    
}; // class Group


} // namespace MULTIPR


#endif // MULTIPR_FAMILY_HXX

// EOF
