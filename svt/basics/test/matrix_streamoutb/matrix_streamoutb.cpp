#include <stdlib.h>
#include <stdio.h>    
// only included to see if stdio causes any problems when mixed with streams



#include <svt_exception.h>
#include <svt_bfstream.h>

#include <svt_stlList.h>
#include <svt_streamableLists.h>
#include <svt_streamableVectors.h>

#include <svt_matrix.h>
#include <svt_str.h>
#include <svt_vector4.h>

int main()
{

  try {

    svt_vector4<Real32> p1(1,2,3);
	svt_vector4<Real32> p(1,1,1);
	
	vector< svt_vector4<Real32> > voPoints(10000);
	  
	vector<int> viValues(10000);

	for (int i=0;i<10000;i++)
	  {
	  voPoints[i] = (p1+=p);
	  viValues[i] = i;
	  }
	//	svt_matrix<int> a(1000,1000, 5);

	svt_obfstream s("test/matrix", SVT_OPEN_MODE(0), true);

	//s.setSwapMode(NEVER_SWAP);
	//    s << containerSize;
	s << voPoints << viValues;

  //  s << noContainerSize << liList << lineStrings << oList;
	//s << noContainerSize << loPoints;
  }

  catch (svt_userInterrupt) {
    cout << "Cancelled by User!" << endl;
    }

  catch (svt_exception oException) {
    cout << oException.message() << endl;
  }



  return 0;
}
  
