// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MemorySpy.hxx"

#if defined(PCLINUX)  || defined(PCLINUX64) || defined(PCLINUX64_32)
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
