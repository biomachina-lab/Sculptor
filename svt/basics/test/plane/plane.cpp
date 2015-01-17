#include <svt_Plane.h>

int main()
{
  svt_vector4<Real32> P1(0,0,1), P2(1,0,0), P3(0,1,0), P4(0,0,3);
  svt_Plane oPlane(P1, P2, P3);

  cout << "plane normal: " << oPlane.normal() << endl;
  cout << "d(P4, Plane)= "  << oPlane.signedDist(P4) << endl;
  cout << "plane z-value at (1,1):" << oPlane.z(1,1) << endl;


  return 0;



}
