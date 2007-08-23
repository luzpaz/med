#ifndef _LOG_H_
#define _LOG_H_

/* Simple pre-processor logging utility.
 * Replaces LOG( x ) with "if(x <= LOG_LEVEL) std::cout" when logging is active (LOG_ACTIVE defined)
 * x is the level at which the message should be logged. 
 *
 *
 *
 */



#define LOG_ACTIVE

#ifdef LOG_ACTIVE

#include <iostream>

#define LOG_LEVEL 1

#define LOG(x, msg) if(x <= LOG_LEVEL) std::cout << msg << std::endl; 

#else

#define LOG(x, msg)

#endif











#endif
