#ifndef MEDSPLITTER_UTILS_HXX_
#define MEDSPLITTER_UTILS_HXX_

#include <string>

namespace MEDSPLITTER {
	std::string trim(std::string& s,const std::string& drop = " ")
	{
		std::string r=s.erase(s.find_last_not_of(drop)+1);
		return r.erase(0,r.find_first_not_of(drop));
	}

}
#endif /*MEDSPLITTER_UTILS_HXX_*/
