#ifndef DRIVERTOOLS_HXX
#define DRIVERTOOLS_HXX


#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <iostream>

namespace MEDMEM {
class MESH;
class CONNECTIVITY;
class COORDINATE;
class GROUP;
struct _noeud
{
    mutable int number;
    std::vector<double> coord;
};

struct _maille
{
    typedef std::map<int,_noeud>::iterator iter;
    MED_EN::medGeometryElement geometricType;
    std::vector< iter > sommets;
    mutable unsigned ordre; // l'ordre est fixé après insertion dans le set, et ne change ni l'état, ni l'ordre -> mutable

    _maille(MED_EN::medGeometryElement _geometricType, size_t nelem) : geometricType(_geometricType),ordre(0)
    {
	sommets.reserve(nelem);
    };
    int dimension() const // retourne la dimension de la maille
    {
	return geometricType/100;
    };
    bool operator < (const _maille& ma) const;
    MED_EN::medEntityMesh getEntity(const int meshDimension) const throw (MEDEXCEPTION);
};

struct _mailleIteratorCompare // pour ordonner le set d'iterateurs sur mailles
{
    bool operator () (std::set<_maille>::iterator i1, std::set<_maille>::iterator i2)
    {
	return *i1<*i2;
    }
};

struct _groupe
{
    typedef std::set< std::set<_maille>::iterator>::const_iterator mailleIter;
    std::string nom;
    std::set< std::set<_maille>::iterator, _mailleIteratorCompare > mailles; // iterateurs sur les mailles composant le groupe
    std::list<int> groupes; // indices des sous-groupes composant le groupe
};

/*!
 * \if developper
 * Intermediate structure used by drivers to store data read from the other format file.
 * The structure provides functions that transform the stored data to the MED format : 
 * getCoordinate(), getConnectivity(), getGroups().
 * The elements inserted in maillage and points are automaticaly ordered.
 * Renumbering are performed by getConnectivity & getGroups before writing the MED structures.
 * Read the conception ducumentation for more details.
 * \endif
 */
struct _intermediateMED
{
    std::set<_maille> maillage;
    std::vector<_groupe> groupes;
    std::map< int, _noeud > points;

    CONNECTIVITY * getConnectivity(); // set MED connectivity from the intermediate structure
    COORDINATE * getCoordinate(); // set MED coordinate from the intermediate structure
    void getGroups(std::vector<GROUP *> & _groupCell, std::vector<GROUP *> & _groupFace, std::vector<GROUP *> & _groupEdge, std::vector<GROUP *> & _groupNode, MESH * _ptrMesh);

    // used by previous functions to renumber points & mesh.
    void numerotationMaillage(); 
    void numerotationPoints();

};

std::ostream& operator << (std::ostream& , const _maille& );
std::ostream& operator << (std::ostream& , const _groupe& );
std::ostream& operator << (std::ostream& , const _noeud& );
std::ostream& operator << (std::ostream& , const _intermediateMED& );

};
#endif /* DRIVERTOOLS_HXX */
