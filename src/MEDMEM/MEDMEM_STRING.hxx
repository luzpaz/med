# ifndef STRING_HXX
# define STRING_HXX

# include <string>
# include <sstream>

using namespace std;

class STRING : public string
{
private :
	ostringstream _s ;
public :
	operator const char*() const
	{
		return _s.str().c_str() ;
	}
	STRING() : _s()
	{
	}
	template <class T> STRING( const T &valeur ) : _s()
	{
		_s << valeur ;
	}
	template <class T> STRING &operator<<( const T &valeur )
	{
		_s << valeur ;
		return *this ;
	}
} ;

// Exemple d'utilisation avec les exceptions SALOME
//      STRING msgErr;
//      msgErr << "ESSAI::ESSAI() : This Object cannot be instanciated by now ! Try "<< 11 << "times just to see what happens ";
//      throw SALOME_EXCEPTION (LOCALIZED(msgErr)) ;

# endif
