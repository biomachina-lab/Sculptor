#include <svt_stlList.h>
#include <svt_str.h>
#include <svt_streamableLists.h>



//
// Test-Aufruf: streamList_test < test/streamList_test.txt
//

int main()
{
  list<int> liList;
  list<float> lfList;
  list<svt_str> lpszList;


  cin >> liList >> lfList >> lineStrings >> lpszList >> liList;
  cout << containerSize << liList << endl << lfList << endl << lpszList << endl;

  //cin >> liList >> lfList >> noContainerSize >> lineStrings >> lpszList;
  //  cout << containerSize << liList << endl << lfList << endl << lpszList << endl;

  svt_str s("hallo frank");
  lpszList.push_back(s);
  lpszList.push_back("nochmal hallo");
  s="nix mehr";

  cout << "----------------" << endl;

  cout << lpszList << endl;

  cout << "----------------" << endl;

  list <svt_str>::const_iterator il;
  int i=1;
  for(il=lpszList.begin(); il!=lpszList.end();i++,il++)
  	cout << i << ": <" << *il << ">" << endl;


  return 0;

}
