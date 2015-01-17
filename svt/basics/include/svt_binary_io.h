
#ifndef SVT_BINARY_IO
#define SVT_BINARY_IO


#include <svt_types.h>




inline DLLEXPORT bool svt_BigEndian()
{ 
  static Int16 sTest=0x0102;
  return *((char*)&sTest) == 0x01;
}





template <class T>
inline DLLEXPORT T svt_swap(const T& value)
{

  T ret;
  unsigned i,j;
  for ( j=sizeof(T)-1,  i=0;  i<sizeof(T);  j--, i++)
	*( ((char*)&ret) +i) = *( ((char*)&value) +j); 

  return ret;
}

#endif
