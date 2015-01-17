/***************************************************************************
                          svt_types.cpp  -  description
                             -------------------
    begin                : Wed Nov 22 2000
    copyright            : (C) 2000 by Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_types.h>

// clib includes
#include <stdlib.h>
#include <stdio.h>
#include <svt_iostream.h>

#ifdef WIN32
  #ifndef STATICQT
  extern "C" DLLEXPORT BOOL WINAPI DllMain( HANDLE hModule, DWORD dwFunction, LPVOID lpNot )
  {
    svt_registerCout( &std::cout );
    return true;
  };
  #endif
#endif

Int16 svt_types::sm_swapTest = 0x0102;

svt_types::svt_types()
{
  char *p;
  p = (char *)&sm_swapTest;             /* Zeiger auf Wert 0x0102 */
  m_isBigEndian = (*p == 0x01);        /* erstes Byte 0x01 => Big-Endian-Format */
}


void svt_types::checkTypes()
{
  int error = 0;
  if (sizeof(Int8) != 1)
    {
      cerr << "svt_types: Int8 has size " << sizeof(Int8) << ", but should have size 1" << endl;
      error = 1;
    }
  if (sizeof(UInt8) != 1)
    {
      cerr << "svt_types: UInt8 has size " << sizeof(UInt8) << ", but should have size 1" << endl;
      error = 1;
    }
  if (sizeof(Int16) != 2)
    {
      cerr << "svt_types: Int16 has size " << sizeof(Int16) << ", but should have size 2" << endl;
      error = 1;
    }
  if (sizeof(UInt16) != 2)
    {
      cerr << "svt_types: UInt16 has size " << sizeof(UInt16) << ", but should have size 2" << endl;
      error = 1;
    }
  if (sizeof(Int32) != 4)
    {
      cerr << "svt_types: Int32 has size " << sizeof(Int32) << ", but should have size 4" << endl;
      error = 1;
    }
  if (sizeof(UInt32) != 4)
    {
      cerr << "svt_types: UInt32 has size " << sizeof(UInt32) << ", but should have size 4" << endl;
      error = 1;
    }
#ifdef _LONG
  if (sizeof(Int64) != 8)
    {
      cerr << "svt_types: Int64 has size " << sizeof(Int64) << ", but should have size 8" << endl;
      error = 1;
    }
  if (sizeof(UInt64) != 8)
    {
      cerr << "svt_types: UInt64 has size " << sizeof(UInt64) << ", but should have size 8" << endl;
      error = 1;
    }
#endif
  if (sizeof(Real32) != 4)
    {
      cerr << "svt_types: Real32 has size " << sizeof(Real32) << ", but should have size 4" << endl;
      error = 1;
    }
  if (sizeof(Real64) != 8)
    {
      cerr << "svt_types: Real64 has size " << sizeof(Real64) << ", but should have size 8" << endl;
      error = 1;
    }
  if (sizeof(Vector3f) != 12)
    {
      cerr << "svt_types: Vector3f has size " << sizeof(Vector3f) << ", but should have size 12" << endl;
      error = 1;
    }
  if (sizeof(Point3f) != 12)
    {
      cerr << "svt_types: Point3f has size " << sizeof(Point3f) << ", but should have size 12" << endl;
      error = 1;
    }
  if (sizeof(Matrix4f) != 64)
    {
      cerr << "svt_types: Matrix4f has size " << sizeof(Matrix4f) << ", but should have size 64" << endl;
      error = 1;
    }
  if (sizeof(Quaternion) != 16)
    {
      cerr << "svt_types: Quaternion has size " << sizeof(Quaternion) << ", but should have size 8" << endl;
      error = 1;
    }
  if (error==1)
    {
      exit(1);
    }
}

bool svt_types::isBigEndian() const
{
  return m_isBigEndian;
}

bool svt_types::BigEndian()
{
  static const char *p = (char *)&sm_swapTest;
  return (*p == 0x01);
  
}
