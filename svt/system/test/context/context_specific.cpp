
// do not link this test-program against svt_system - lib

#include <svt_contextSpecific.h>
#include <svt_iostream.h>
//#include <svt_streamableVectors.h>

class testclass {

private:
  static int sm_i;
  int m_iID;

public:

  testclass() : m_iID(sm_i++) {}
  testclass(const testclass&) : m_iID(sm_i++) {}
  virtual ~testclass() {}

  void printID() const { cout << m_iID << endl;}

};


class i_n_t {
public:
  int i;
  operator int& () {return i;}
};



int testclass::sm_i = 1;

int g_i=5;
int svt_getCurrentWindow() {return g_i;}



int svt_getWindowsNum() {return 17;}


int main()
{
  svt_windowSpecific<testclass> obj;

  obj->printID();
  cout << endl;
  obj[0].printID();
  cout << endl;
  forAllWindows(obj, printID());

  cout << endl << "----" << endl << endl;

  svt_windowSpecific<i_n_t> v1;
  int i,j;
  for (i=0;i<svt_getWindowsNum(); i++)
	{
	  v1->i=5;
	}

  cout << v1->i << endl;


  svt_windowSpecific< vector<int> > v2;
  for (i=0;i<svt_getWindowsNum(); i++)
	{
	  v2[i].resize(10);
	  for (j=0;j<10;j++)
		v2[i][j]=100*i+j;
	}

  cout << v2->operator[](5) << endl;

  return 0;

}

