#include <stdlib.h>
//#include <stdio.h>

#include <svt_exception.h>
// #include <svt_bfstream.h>
#include <svt_matrix.h>
#include <svt_types.h>
#include <svt_cmath.h>

#include <svt_stlVector.h>

//template class vector<int>;
//template class vector<double>;
//template class svt_matrix<int>;

int main()
{

  try
	{

	//	vector<float> vf;
	//vf.resize(100);

	svt_matrix<int> a(5,10,10);
	cout << "created matrix A: " << a.height() << "x" << a.width()
		 << " with default values 10:" << endl;
	cout << a << endl;

	svt_matrix<int> b(2,6);
	cout << "created matrix B: " << b.height() << "x" << b.width()
		 << " (uninitialized)" << endl;
	cout << b << endl;
	

	svt_matrix<int> c(a);
	cout << "created matrix C: " << c.height() << "x" << c.width()
		 << " as a copy of A:" << endl << c << endl;


	a=1;
	cout << endl << "setting a=1:" << endl << a << endl;
	cout << "C must not have changed:" << endl << c << endl;
  
	b=4;
	cout << "setting b=4:" << endl << b << endl;

	cout << "dereferencing a[1][1]: " << a[1][1] << endl;
	a[3][1] = 7;
	cout << "setting a[3][1] = 7  -  A is now" << endl << a << endl;

	cout << "C must not have changed:" << endl << c << endl;

	c=b;
	cout << "setting c=b -> C: " << endl << c << endl;

	++c;
	cout << endl << "setting ++c ->" << endl << c << endl;
    
	c++;
	cout << endl << "setting c++ ->" << endl << c << endl;

	--c;
	cout << endl << "setting --c ->" << endl << c << endl;
    
	c--;
	cout << endl << "setting c-- ->" << endl << c << endl;

	c+=5;
	cout << endl << "setting c+=5 ->" << endl << c << endl;
  
	c-=7;
	cout << endl << "setting c-=7 ->" << endl << c << endl;
  
	c*=10;
	cout << endl << "setting c*=10 ->" << endl << c << endl;
  
	c+= b;
 	cout << endl << "setting c+=b ->" << endl << c << endl;

	cout << endl 
		 << (a==b) << "   " << (a!=b) << endl
		 << (b==c) << "   " << (b!=c) << endl
		 << (b+20 == c) << endl;

	c /= 20;
	cout << endl << "setting c/=20 ->" << endl << c << endl;

 	cout << endl << "2+c*3 =" << endl << (2+c*3) << endl;

 	cout << endl << "(2+c)*3 =" << endl << ((2+c)*3) << endl;

 	cout << endl << "3*2+c =" << endl << (3*2+c) << endl;

	cout << endl << "min/max of a is " << min(a) << " " << max(a);
	cout << endl << "min/max of -a is " << min(-a) << " " << max(-a) << endl;

	cout << "where a is" << endl << endl << a << endl;
	
	cout << endl << "checking at method:" << endl;
	try
	  {
	  cout << "a.at(3,1): " << a.at(3,1) << endl;
	  }
	catch(svt_exception oError)
	  {
	  oError.uiMessage();
	  }

	try
	  {
	  cout << "a.at(3,3): " << a.at(3,3) << endl;
	  }
	catch(svt_exception oError)
	  {
	  oError.uiMessage();
	  }

	try
	  {
	  cout << "a.at(7,7): " << a.at(7,7) << endl;
	  }
	catch(svt_exception oError)
	  {
	  oError.uiMessage();
	  }

	cout << endl 
		 << "checking matrix multiplication:" << endl
		 << "-------------------------------" << endl << endl;


	a.resize(7,3, svt_matrix<int>::Clear, 2);
	b.resize(3,7, svt_matrix<int>::Clear, 3);

	cout << endl << a << endl << b << endl;

	try
	  {
	  cout << endl << "trying c=a*b:" << endl;
	  c=a*b;
	  cout << c << endl;
	  }
	catch(svt_exception oError)
	  {
	  oError.uiMessage();
	  }

	try
	  {
	  cout << endl << "trying c=a+b:" << endl;
	  c=a+b;
	  cout << c << endl;
	  }
	catch(svt_exception oError)
	  {
	  oError.uiMessage();
	  }

	try
	  {
	  cout << endl << "trying c=b*a:" << endl;
	  c=b*a;
	  cout << c << endl;
	  }
	catch(svt_exception oError)
	  {
	  oError.uiMessage();
	  }

	try
	  {
	  cout << endl << "trying c*=b:" << endl;
	  c*=b;
	  cout << c << endl;
	  }
	catch(svt_exception oError)
	  {
	  oError.uiMessage();
	  }

	cout << "3*a:" << endl << (3*a) << endl;
	cout << "a*3:" << endl << a*3 << endl;
	cout << "3+a:" << endl << 3+a << endl;
	cout << "a+3:" << endl << a+3 << endl;
	cout << "3-a:" << endl << 3-a << endl;
	cout << "a-3:" << endl << a-3 << endl;


	cout << endl 
		 << "checking matrix addition:" << endl
		 << "-------------------------------" << endl << endl;
	
	a=a+(a+7);
	cout << "a=a+(a+7)->" << endl << a << endl;

	a+=a;
	cout << "a+=a ->" << endl << a << endl;

	a-=20;
	cout << "a-=20 ->" << endl << a << endl;

	a-=(a-3);
	cout << "a-=(a-3) ->" << endl << a << endl;


	svt_matrix<float> A(9,9);
	int k=0;
	for (int i=0; i<9;i++)
	  for (int j=0; j<9;j++)
		A[i][j]=++k;

	svt_matrix<float> B(A);

	cout << "testing different resize methods for " << endl << B << endl;

	B.resize(17,17,svt_matrix<float>::Uninitialized);
	cout << "Uninitialized: " << endl << B << endl;

	B=A;
	B.resize(17,17,svt_matrix<float>::Clear, 3);
	cout << "Clear with 3: " <<  endl << B << endl;

	B=A;
	B.resize(17,17,svt_matrix<float>::Save, 100);
	cout << "Save with 100: " << endl << B << endl;

	B=A;
	B.resize(17,17,svt_matrix<float>::SaveClamp, 100);
	cout << "SaveClamp with 100: " << endl << B << endl;

	B=A;
	B.resize(17,17,svt_matrix<float>::SaveCenter, 100);
	cout << "SaveCenter with 100: " << endl << B << endl;

	B=A;
	B.resize(17,17,svt_matrix<float>::SaveCenterClamp, 100);
	cout << "SaveCenterClamp with 100: " << endl << B << endl;


	B=A;
	B.resize(17,17,svt_matrix<float>::Interpolate, 100);
	cout << "Interpolate with 100: " << endl << B << endl;

	cout << "Now resizing to smaller dimensions:" << endl;

	B=A;
	B.resize(5,5,svt_matrix<float>::Clear, 3);
	cout << "Clear with 3: " <<  endl << B << endl;

	B=A;
	B.resize(5,5,svt_matrix<float>::Save, 100);
	cout << "Save with 100: " << endl << B << endl;

	B=A;
	B.resize(5,5,svt_matrix<float>::SaveClamp, 100);
	cout << "SaveClamp with 100: " << endl << B << endl;

	B=A;
	B.resize(5,5,svt_matrix<float>::SaveCenter, 100);
	cout << "SaveCenter with 100: " << endl << B << endl;

	B=A;
	B.resize(5,5,svt_matrix<float>::SaveCenterClamp, 100);
	cout << "SaveCenterClamp with 100: " << endl << B << endl;
	
 
	
	B=A;
	B.resize(5,5,svt_matrix<float>::Interpolate, 100);
	cout << "Interpolate with 100: " << endl << B << endl;

	B=A;
	B.resize(3,3,svt_matrix<float>::Interpolate, 100);
	cout << "Interpolate with 100: " << endl << B << endl;

	B=A;
	B.resize(2,2,svt_matrix<float>::Interpolate, 100);
	cout << "Interpolate with 100: " << endl << B << endl;

	B=A;
	B.resize(1,1,svt_matrix<float>::Interpolate, 100);
	cout << "Interpolate with 100: " << endl << B << endl;

	B=A;
	B.resize(9,1,svt_matrix<float>::Interpolate, 100);
	cout << "Interpolate with 100: " << endl << B << endl;

	B=A;
	B.resize(1,9,svt_matrix<float>::Interpolate, 100);
	cout << "Interpolate with 100: " << endl << B << endl;
	  
	cout << endl << "--------------------------" << endl
		 << "now some tests with svt_next_pow2 function" << endl;

	UInt32 n;
	UInt32 size;

	size=svt_next_pow2(100, n);
	cout << 100 << " -> " << size << " (" << n << ")" << endl;

	size=svt_next_pow2(50, n);
	cout << 50 << " -> " << size << " (" << n << ")" << endl;

	size=svt_next_pow2(127, n);
	cout << 127 << " -> " << size << " (" << n << ")" << endl;

	size=svt_next_pow2(128, n);
	cout << 128 << " -> " << size << " (" << n << ")" << endl;

	size=svt_next_pow2(129, n);
	cout << 129 << " -> " << size << " (" << n << ")" << endl;
	
	
	}

  catch(svt_exception oError)
	{
	oError.uiMessage();
	}
  return 0;

}
