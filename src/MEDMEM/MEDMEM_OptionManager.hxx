#ifndef _OPTION_MANAGER_HXX
#define _OPTION_MANAGER_HXX

#include <string>
#include <map>
#include "MEDMEM_Exception.hxx"
namespace MEDMEM
{
  class OptionGeneric
  {
    //dummy method to have some polymorphism...
    virtual void dummy()=0;
  };
  
  template < class T > 
  class Option : public OptionGeneric
  {
  private:
    T * _var;
  public:
    Option( T defaut, T * var):
      _var(var)
    {
      *_var= defaut;
    }
    void setValue(T value)
    {
      *_var=value;
    }
    T getValue()
    {
      return  * _var;
    }

    //dummy method to have some polymorphism...
    void dummy(){}
  };
    
  class OptionManager
  {
  private:
    std::map< std::string, OptionGeneric* > _optionList;

  public:
  
    ~OptionManager()
    {
			std::map< std::string, OptionGeneric*>::iterator mi;
      for(mi = _optionList.begin() ; mi != _optionList.end() ; mi++) delete (*mi).second;
    }
    
    template <class T> void  registerOption( T * var, const std::string& name, T defaut)
    {
      OptionGeneric  * newoption = new Option<T>( defaut, var);
      _optionList.insert(make_pair(name, newoption));			
    }

    template <class T> void getOption(const std::string& name, T& var)
    {
      if(_optionList.find(name) != _optionList.end())  
				{
					Option<T>* option_ptr = dynamic_cast<Option<T>*>(_optionList.find(name)->second);
					var= option_ptr->getValue();
				}
			else throw MEDEXCEPTION 
				("Option is not listed, please register the option before using the getOption command");
    }
		
    template <class T> void setOption(const std::string& name, T value)
    {
      if(_optionList.find(name) != _optionList.end())
				{
					Option<T>* option_ptr = dynamic_cast<Option<T>*>(_optionList.find(name)->second);
					if (option_ptr != 0 )
						option_ptr->setValue(value);
					else throw MEDEXCEPTION ("Error setOption: Option is registered with a different type");
				}
      else throw MEDEXCEPTION 
				("Option is not listed, please register the option before using the setOption command");
    }
    
    void setOption(const std::string& name, int value)
    {
      if(_optionList.find(name) != _optionList.end())
				{
					Option<double>* option_double_ptr = dynamic_cast<Option<double>*> (_optionList.find(name)->second);
					if (option_double_ptr!=0)
						setOption(name,(double) value);
					else
						{
							Option<int>* option_ptr =dynamic_cast<Option<int>*>(_optionList.find(name)->second);
							if (option_ptr != 0 )
								option_ptr->setValue(value);
							else throw MEDEXCEPTION ("Error setOption: Option is registered with a different type");
						}
				}
      else throw MEDEXCEPTION 
				("Option is not listed, please register the option before using the setOption command");
    }
    
  };
}

#endif
