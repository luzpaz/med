// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_API.hxx
 *
 * \brief   Main header of the high level MULTIPR API.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_API_HXX
#define MULTIPR_API_HXX

#include <vector>
#include <string>

namespace multipr
{


enum Partitionner
{
    MULTIPR_METIS   = 0,
    MULTIPR_SCOTCH  = 1
    
}; // enum Partitionner


/**
 * \fn     const char* getVersion()
 * \brief  returns the current version of the MULTIPR API.
 * \return the current version of the MULTIPR API. 
 */
const char* getVersion();


/**
 * \fn     void partitionneDomaine(const char* medFilename, const char* meshName)
 * \brief  creates a distributed MED file (v2.3) by extracting all the groups from the mesh of a sequential MED file.
 *         Assumes:
 *         - the file is in MED format and can be read using MED file v2.3.
 *         - the file is sequential (not a distributed MED).
 *         - the file only contains TETRA10 elements (dimension of space and mesh is 3).
 *         - the file have no profil.
 * \param  medFilename filename of any valid sequential MED file with TETRA10 elements only.
 * \param  meshName    name of the mesh to be distributed.
 * \throw  RuntimeException if any error occurs.
 */
void partitionneDomaine(
    const char* medFilename, 
    const char* meshName);


/**
 * \fn     void partitionneGroupe(const char* medFilename, const char* groupName, int nbParts, int partitionner)
 * \brief  creates a distributed MED file (V2.3) by splitting a group of a MED file previously created by partitionneDomaine.
 *         Assumes:
 *         - the file is a distributed MED file, previously created by partitionneDomaine()
 *           (=> each part only contain 1 mesh, TETRA10 elements only)
 *         - nbPart > 1
 * \param  medFilename  filename of any valid distributed MED file previously created by partitionneDomaine(). 
 * \param  partName     name of the part to be splitted.
 * \param  nbParts      number of parts; must be > 1.
 * \param  partitionner use value MULTIPR_METIS for Metis or MULTIPR_SCOTCH for Scotch.
 * \throw  RuntimeException if any error occurs.
 */
 void partitionneGroupe(
     const char* medFilename, 
     const char* partName, 
     int         nbParts, 
     int         partitionner=MULTIPR_METIS);
 

/**
 * \fn     void decimePartition(const char* medFilename, const char* partName, const char* fieldName, int fieldIt, const char* filterName, double tmed, double tlow, double radius);
 * \brief  creates 3 resolutions of the given part of a distributed MED file (V2.3).
 *         Assumes:
 *         - the file is a distributed MED file, previously created by partitionneDomaine() or partitionneGroupe()
 *           (=> each part only contain 1 mesh, TETRA10 elements only)
 * \param  medFilename filename of any valid distributed MED file previously created by partitionneDomaine or partitionneGroupe.
 * \param  partName    name of the part to be decimated.
 * \param  fieldName   name of the field used for decimation.
 * \param  fieldIt     iteration (time step) of the field.
 * \param  filterName  name of the filter to be used.
 * \param  tmed        threshold used for medium resolution.
 * \param  tlow        threshold used for low resolution; tmed must be less than tlow
 * \param  radius      radius used to determine the neighbourhood.
 * \param  boxing      number of cells along each axis; must be >= 1; e.g. if 100 then acceleration grid will have 100*100*100 = 10**6 cells.
 * \throw  RuntimeException if any error occurs.
 */
void decimePartition(
    const char* medFilename,
    const char* partName,
    const char* fieldName,
    int         fieldIt,
    const char* filterName,
    const char*	filterParams);

    
/**
 * \fn     void merge(...);
 * \brief  merge a list of sequential MED file if possible.
 * \param  medFilenameSrc  list of source file (sequential MED files).
 * \param  meshName        name of the mesh (all mesh must have the same name).
 * \param  fieldName       name of the field to merge (if NULL, merge all fields).
 * \param  medFilenameDst  destination file.
 * \return 1 if a mesh has been generated, 0 otherwise (e.g. if all the sources are empty meshes).
 * \throw  RuntimeException if any error occurs.
 */
int merge(
    std::vector<std::string> medFilenameSrc,
    const char* meshName,
    const char* fieldName,
    const char* medFilenameDst);

        
} // namespace MULTIPR


#endif // MULTIPR_API_HXX

// EOF
