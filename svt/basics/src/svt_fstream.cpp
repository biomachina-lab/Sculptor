#include <stdio.h>
#include <errno.h>

#ifdef WIN32_MSVC
#  include <io.h>
#else
#  include <unistd.h>
#endif


#include <svt_fstream.h>

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_ifstream                                                  //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

unsigned svt_ifstream_static::sm_uMaxStrLen=1024;



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  c-type functions                                                     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////



bool svt_file_exisits(const char* filename)
{
#ifdef WIN32_MSVC
  return (_access(filename, 0) == 0);
#else
  return (access(filename, F_OK) == 0);
#endif 
}


bool svt_read_access(const char* filename)
{
#ifdef WIN32_MSVC
  return ( _access(filename, 4) == 0 );
#else
  return ( access(filename, R_OK) == 0 );	
#endif
}

bool svt_write_access(const char* filename)
{
#ifdef WIN32_MSVC
  return ( _access(filename, 2) == 0 );
#else
  return ( access(filename, W_OK) == 0 );	
#endif
}

const char* svt_os_error(const char* filename, const char* mode)
{
  FILE* f =fopen(filename, mode);

  if (f)
	{
	fclose(f);
	return "(unknown)";
	}
  return strerror(errno);
}
