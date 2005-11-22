# ifndef MEDMEM_STRING_HXX
# define MEDMEM_STRING_HXX

using namespace std;

# include <string>
# include <sstream>

/*!
 A class to generate string from any type:

 here is a simple use case
      STRING msgErr;
      msgErr << "This Object cannot be instanciated by now ! Try "<< 11 << "times just to see what happens ";
      throw SALOME_EXCEPTION (LOCALIZED(msgErr)) ;
								*/
namespace MEDMEM {
  class STRING : public string
  {

  private :
    ostringstream _s ;

  public :
  
    STRING() :string(), _s() 
    {
    }

    ~STRING()
    {
    }

    operator const char * () const 
    {
      // return const_cast <const char *> (this->c_str()) ;
      return this->c_str();
    }

    template <class T> STRING( const T &valeur ) : string(), _s() 
    {
      _s << valeur ;

      this->string::operator =( _s.str());
    }

    template <class T> STRING & operator<<( const T &valeur )
    {
      _s << valeur ;

      this->string::operator = ( _s.str() ) ;  // freeze is true by now

      return *this ;
    }
  } ;
} ;

# endif
