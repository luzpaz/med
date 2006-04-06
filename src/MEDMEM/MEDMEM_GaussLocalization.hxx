#ifndef GAUSS_LOCALIZATION_HXX
#define GAUSS_LOCALIZATION_HXX

#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_nArray.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_SetInterlacingType.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

namespace MEDMEM {

  class GAUSS_LOCALIZATION_ {
  public:
    virtual MED_EN::medModeSwitch getInterlacingType() const {return MED_EN::MED_UNDEFINED_INTERLACE;}
    virtual ~GAUSS_LOCALIZATION_() {}; //Indispensable pour d�truire le vrai objet point�
  };

  template <class INTERLACING_TAG=FullInterlace> class GAUSS_LOCALIZATION;

  template <class INTERLACING_TAG> ostream & operator<< (ostream &os,
							 const GAUSS_LOCALIZATION<INTERLACING_TAG> &loc);

  template <class INTERLACING_TAG> class GAUSS_LOCALIZATION : public GAUSS_LOCALIZATION_{
  public:
    typedef  typename MEDMEM_ArrayInterface<double,INTERLACING_TAG,NoGauss>::Array ArrayNoGauss;

  protected:

    string                      _locName;
    MED_EN::medGeometryElement  _typeGeo;
    int                         _nGauss;
    ArrayNoGauss                _cooRef;
    ArrayNoGauss                _cooGauss;
    vector<double>              _wg;
    MED_EN::medModeSwitch       _interlacingType;

  public:
    friend ostream & operator<< <INTERLACING_TAG>(ostream &os,
						  const GAUSS_LOCALIZATION<INTERLACING_TAG> &loc);

    GAUSS_LOCALIZATION() throw (MEDEXCEPTION);
    GAUSS_LOCALIZATION(const string & locName,
		       const MED_EN::medGeometryElement typeGeo,
		       const int  nGauss,
		       const ArrayNoGauss & cooRef,
		       const ArrayNoGauss & cooGauss,
		       const vector<double>  & wg) throw (MEDEXCEPTION);

    GAUSS_LOCALIZATION(const string & locName,
		       const MED_EN::medGeometryElement  typeGeo,
		       const int  nGauss,
		       const double  * const cooRef,
		       const double  * const cooGauss,
		       const double  * const wg) throw (MEDEXCEPTION);

    //GAUSS_LOCALIZATION(const GAUSS_LOCALIZATION & loc); constructeur de recopie par d�faut correct
    virtual ~GAUSS_LOCALIZATION() {};
    GAUSS_LOCALIZATION & operator=(const GAUSS_LOCALIZATION & gaussLoc);
    bool operator == (const GAUSS_LOCALIZATION &loc) const;

    string          getName()    const {return _locName;}
    MED_EN::medGeometryElement getType() const {return _typeGeo;}
    int             getNbGauss() const {return _nGauss;}
    ArrayNoGauss    getRefCoo () const {return _cooRef;}     //Ces tableaux sont petits
    ArrayNoGauss    getGsCoo  () const {return _cooGauss;}   //Ces tableaux sont petits
    vector <double> getWeight () const {return _wg;}         //Ces tableaux sont petits
    inline MED_EN::medModeSwitch  getInterlacingType() const { return _interlacingType;}

  };
  template <class INTERLACING_TAG> GAUSS_LOCALIZATION<INTERLACING_TAG>::GAUSS_LOCALIZATION() throw (MEDEXCEPTION) :
    _typeGeo(MED_EN::MED_NONE), _nGauss(-1),
    _interlacingType( SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType) 
  {};

  template <class INTERLACING_TAG> GAUSS_LOCALIZATION<INTERLACING_TAG>::GAUSS_LOCALIZATION(const string & locName,
											   const MED_EN::medGeometryElement typeGeo,
											   const int  nGauss,
											   const ArrayNoGauss & cooRef,
											   const ArrayNoGauss & cooGauss,
											   const vector<double>  & wg)  throw (MEDEXCEPTION) :
    _locName(locName),_typeGeo(typeGeo),_nGauss(nGauss),_cooRef(cooRef),_cooGauss(cooGauss),_wg(wg),
    _interlacingType(SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType)
  {
    const char * LOC = "GAUSS_LOCALIZATION(locName,typeGeo, nGauss, const ArrayNoGauss & cooRef,..) : ";
    BEGIN_OF(LOC);
    if (_cooRef.getDim() != _cooGauss.getDim() )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"cooRef and cooGaus must have the same number of components")) ;

    if (_cooRef.getArraySize() != (_typeGeo%100)*(_typeGeo/100) )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"cooRef size is " << _cooRef.getArraySize()
				     << " and should be (_typeGeo%100)*(_typeGeo/100) "
				     << (_typeGeo%100)*(_typeGeo/100))) ;

    if (_cooGauss.getArraySize() != _nGauss*(_typeGeo/100) )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"cooGauss must be of size nGauss*(_typeGeo/100) "
				     << _nGauss*(_typeGeo/100) ));
    if (_wg.size() != _nGauss )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"wg must be of size nGauss "
				     << _nGauss ));

    END_OF(LOC);
  };

  template <class INTERLACING_TAG> GAUSS_LOCALIZATION<INTERLACING_TAG>::GAUSS_LOCALIZATION(const string & locName,
											   const MED_EN::medGeometryElement  typeGeo,
											   const int  nGauss,
											   const double  * const cooRef,
											   const double  * const cooGauss,
											   const double  * const wg) throw (MEDEXCEPTION) :
    _locName(locName),_typeGeo(typeGeo),_nGauss(nGauss),
    _cooRef(ArrayNoGauss(const_cast<double *>(cooRef),typeGeo/100,typeGeo%100)),
    _cooGauss(ArrayNoGauss(const_cast<double *>(cooGauss),typeGeo/100,_nGauss)),
    _wg(vector<double>(wg,wg+nGauss)),
    _interlacingType(SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType)
  {
    const char * LOC = "GAUSS_LOCALIZATION(locName,typeGeo, nGauss, const double * cooRef,..) :";
    BEGIN_OF(LOC);
    if (_cooRef.getDim() != _cooGauss.getDim() )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"cooRef and cooGaus must have the same number of components")) ;

    if (_cooRef.getArraySize() != (_typeGeo%100)*(_typeGeo/100) )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"cooRef must be of size (_typeGeo%100)*(_typeGeo/100) "
				     << (_typeGeo%100)*(_typeGeo/100))) ;

    if (_cooGauss.getArraySize() != _nGauss*(_typeGeo/100) )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"cooGauss must be of size nGauss*(_typeGeo/100) "
				     << _nGauss*(_typeGeo/100) ));
    if (_wg.size() != _nGauss )
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<"wg must be of size nGauss "
				     << _nGauss ));
    END_OF(LOC);
  };

  template <class INTERLACING_TAG> GAUSS_LOCALIZATION<INTERLACING_TAG> &
  GAUSS_LOCALIZATION<INTERLACING_TAG>::operator=(const GAUSS_LOCALIZATION & gaussLoc)
  {
    if ( this == &gaussLoc) return *this;

    _locName  = gaussLoc._locName;
    _typeGeo  = gaussLoc._typeGeo;
    _nGauss   = gaussLoc._nGauss;
    _cooRef   = ArrayNoGauss(gaussLoc._cooRef);   //utilisation de la copie superficielle par d�faut n'est pas une bonne id�e
    _cooGauss = ArrayNoGauss(gaussLoc._cooGauss);  //dans l'op�rateur = de MEDnArray
    _wg       = gaussLoc._wg;

    return *this;
  }

  template <class INTERLACING_TAG> bool
  GAUSS_LOCALIZATION<INTERLACING_TAG>::operator == (const GAUSS_LOCALIZATION & gaussLoc) const {
    return (
	    _locName  == gaussLoc._locName &&
	    _typeGeo  == gaussLoc._typeGeo &&
	    _nGauss   == gaussLoc._nGauss  &&
	    _cooRef   == gaussLoc._cooRef  &&   //utilisation de la copie superficielle par d�faut n'est pas une bonne id�e
	    _cooGauss == gaussLoc._cooGauss &&  //dans l'op�rateur = de MEDnArray
	    _wg       == gaussLoc._wg
	    );
  }


  template <class INTERLACING_TAG> ostream & operator<<(ostream &os,
								const  GAUSS_LOCALIZATION<INTERLACING_TAG> &loc) {
    os << "Localization Name     : " << loc._locName << endl;
    os << "Geometric Type        : " << MED_EN::geoNames[loc._typeGeo]<< endl;
    os << "Number Of GaussPoints : " << loc._nGauss << endl;
    os << "Ref.   Element Coords : " << endl << loc._cooRef << endl;
    os << "Gauss points Coords   : " << endl << loc._cooGauss << endl;
    os << "Gauss points weigth   : " << endl ;
    for(int i=0; i<loc._wg.size();++i)
      os << "_wg[" << i << "] = " << loc._wg[i] << endl;
    return os;
  }

} //END NAMESPACE

#endif
