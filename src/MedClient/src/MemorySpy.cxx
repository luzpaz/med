#include "MemorySpy.hxx"

#ifdef PCLINUX
#include <unistd.h>
#include <string.h>

#define MAXMEM 7

MemorySpy::MemorySpy()
{
  _currentPid=getpid();
  _sizeofPage=getpagesize();
  char workStr[38];
  sprintf( workStr, "/proc/%d/statm", _currentPid);
  _statmFile=fopen ( workStr, "r" );
}

MemorySpy::~MemorySpy()
{
  if(_statmFile)
    free(_statmFile);
}

long MemorySpy::getCurrentMemoryUsage()
{
  if (!_statmFile)
    return -1;
  fseek( _statmFile, 0L, 0 );
  char workStr[52];
  if(!fread( workStr, 1, 50, _statmFile ))
    return -1;
  return parseString(workStr);
}

long MemorySpy::parseString(char* line)
{
  char *po, *po2,hstr[0x100];
  int i;
  long tab[MAXMEM];

  memset( hstr, 0, sizeof( hstr ));
  po2 = hstr;
  po = line;
  i = 0;
  while ( *po != 0x0 )
  {
    if ( ( *po != 0x20 ) )
    {
      *po2 = *po;
      po++;
      po2++;
    }
    else
    {
      tab[i] = atol( hstr ) * _sizeofPage;
      i++;
      memset( hstr, 0, sizeof( hstr ));
      while ( *po != 0x0 )
      {
        if ( ( *po != 0x20 )&&( *po != '\n' ) )
          break;
        po++;
      }
      po2 = hstr;
    }
  }
  if ( strlen( hstr ) != 0 )
  {
    tab[i] = atol( hstr ) * _sizeofPage;
  }
  return tab[0];
}
#endif

#ifdef HP
#include <sys/param.h>
#include <sys/pstat.h>
#include <sys/unistd.h>

MemorySpy::MemorySpy()
{
}

MemorySpy::~MemorySpy()
{
}

long MemorySpy::getCurrentMemoryUsage()
{
  struct pst_dynamic dyn;
  if (pstat_getdynamic(&dyn, sizeof(dyn), 1, 0) == -1)
    return -1;
  else {
    return dyn.psd_vm * getpagesize();
}
#endif
