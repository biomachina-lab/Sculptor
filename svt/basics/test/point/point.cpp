#include <svt_Point3f.h>


int main()
{
  svt_Point3f p1(1,0,0), p2(0,1,0);
  svt_Point3f p3(0,0,1);

  cout << p1 << " " << p2 << " " << p3 << endl;

  p3= vectorProduct(p1,p2*4);
  cout << p3 << " - Länge: " << p3.length() << endl;

  p3.normalize();
  cout << p3 << " - Länge: " << p3.length() << endl;

  cin >> p1 >> p2;
  p3=vectorProduct(p1,p2);
  cout << "vector product of " << p1 << " and " << p2 << " is " 
	   << p3 << endl; 



  return  0;
}


