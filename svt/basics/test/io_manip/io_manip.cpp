#include <stdlib.h>



#include <svt_iostream.h>



#include <svt_iomanip.h>


#ifdef TRUE_WIN32
#  define usleep(i) Sleep((i)/1000)
#else
#  include <unistd.h>
#endif

int main()
{

  int i, c;

  svt_cbreak(true);

  for (i=0;i<10;i++)
	{
	cout << "checking...." << endl;
 	usleep(1000000);
	while ( (c = svt_getKey()) >= 0  )
	  cout << "got key: " << char(c) << " (" << c << ")" << endl;
	} 

  svt_cbreak(false);

  cout << "return key: " << int('\n') << endl;

  cout << "key 13: " << char(13) << endl;

  return 0;



}



