#include <svt_stlVector.h>
#include <svt_str.h>
#include <svt_bfstream.h>
#include <svt_streamableVectors.h>


//
// Test-Aufruf: streamVector_test < test/streamList_test.txt
//

int main()
{
  vector<int> viList;
  vector<float> vfList;
  vector<svt_str> vpszList;


  cin >> viList >> vfList >> lineStrings >> vpszList >> viList;
  cout << containerSize << viList << endl << vfList << endl << vpszList << endl;

  //cin >> liList >> lfList >> noContainerSize >> lineStrings >> lpszList;
  //  cout << containerSize << liList << endl << lfList << endl << lpszList << endl;



  cout << "----------------" << endl;

  viList[0]=10;
  vfList[0]=10;
  

  cout << vpszList[0] << endl;

  svt_str s("hallo frank");
  vpszList[0]=s;

  s="nix mehr";

  vpszList[1]="nochmal hallo";



  cout << "----------------" << endl;
  cout << containerSize << viList << endl << vfList << endl << vpszList << endl;




  return 0;

}
