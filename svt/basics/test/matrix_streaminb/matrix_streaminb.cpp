#include <stdlib.h>


#include <svt_exception.h>

//#include <svt_stlList.h>
//#include <svt_streamableLists.h>

#include <svt_bfstream.h>
#include <svt_streamableVectors.h>

#include <svt_bfstream.h>

//#include <svt_str.h>
//nclude <svt_matrix.h>
#include <svt_vector4.h>



int main()
{
  
  try {

	vector< svt_vector4<Real32> > voPoints;
	vector<int>         viValues;
	//	svt_matrix<int> a;

    svt_ibfstream s("test/matrix");

//	s >> noContainerSize;
	s >> voPoints >> viValues;
	cout << voPoints << endl << viValues;
  }

  catch (svt_exception oException) {
    cout << oException.message() << endl;
  }

  return 0;
}
  
