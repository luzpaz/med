#ifndef __MEMORYSPY_HXX__
#define __MEMORYSPY_HXX__

#include <stdio.h>
#include <stdlib.h>

class MemorySpy
{
public:
  MemorySpy();
  ~MemorySpy();
  long getCurrentMemoryUsage();
#ifdef PCLINUX 
  //SOLARIS
private:
  long parseString(char* line);
private:
  int _currentPid;
  long _sizeofPage;
  FILE *_statmFile;
#endif
};

#endif

