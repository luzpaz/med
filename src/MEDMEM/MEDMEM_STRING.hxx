# ifndef STRING_HXX
# define STRING_HXX

# include <string>
//# include <sstream>
# include <strstream>

using namespace std;

class STRING : public string
{

private :
  //  ostringstream _s ;
  ostrstream _s ;

public :
  
  operator  const char * () const {     
    return const_cast <const char *> (this->c_str()) ;
  }

  ~STRING()
  {
    _s.freeze(false);
  }
  
  STRING() :string() , _s()
  {
  }

  template <class T> STRING( const T &valeur ) : string() ,  _s()
  {
    _s.freeze(false);
    _s << valeur ;
    this->string::operator =( _s.str());  // freeze is true by now
  }

  template <class T> STRING &operator<<( const T &valeur )
  {
    _s.freeze(false);
    _s << valeur ;
    this->string::operator = ( _s.str()) ;  // freeze is true by now
    return *this ;
  }
} ;

// Exemple d'utilisation avec les exceptions SALOME
//      STRING msgErr;
//      msgErr << "ESSAI::ESSAI() : This Object cannot be instanciated by now ! Try "<< 11 << "times just to see what happens ";
//      throw SALOME_EXCEPTION (LOCALIZED(msgErr)) ;

# endif
