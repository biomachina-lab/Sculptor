#include <svt_basics.h>
#include <svt_matrix4.h>
#include <svt_vector4.h>
#include <svt_types.h>
#include <svt_cmath.h>
#include <svt_stlVector.h>
#include <svt_rnd.h>
#include <svt_point_cloud.h>

int main()
{

  
  svt_matrix4<Real64> M;
  cout << "Creation of M: " << endl << M << endl;

  M.rotate(0,20).translate(1,2,3);
  cout << "rotating M in x by 20°, then translating it by (1,2,3): " << endl 
	   << M << endl;

  svt_matrix4<Real64> M2(M);
  cout << "constructing M2 from M: " << endl << M2; 

  M2.invert();
  cout << "inverting M2: " << endl << M2 << endl;

  cout << "Product of M and M2 should give the identity: " << endl 
	   << M*M2 << endl;

  M.scale(0.5);
  M[0][0]=1;  M[0][1]=3;  M[0][2]=1;  M[0][3]=1;
  M[1][0]=4;  M[1][1]=5;  M[1][2]=6;  M[1][3]=1;
  M[2][0]=10; M[2][1]=11; M[2][2]=12; M[2][3]=1;
  M[3][0]=0;  M[3][1]=0;  M[3][2]=0;  M[3][3]=1;
  M2=M;

  cout << "Now testing invert() with more comlex example..." << endl
	   << "Therefore setting M and M2 to " << endl << M << endl; 

  M.invert();

  cout << "inverting M: " << endl << M << endl
	   << "M*M2:" << endl << M*M2 << endl;



  svt_vector4<Real64> oVecA;
  cout << endl << "creation of svt_vector4<Real64> oVecA:" << endl 
	   << oVecA << endl;
  
  svt_vector4<Real64> oVecB(0.0,0.0,1.0);
  cout << endl << "creation of svt_vector4<Real64> oVecB(0.0, 0.0, 1.0):" << endl 
	   << oVecB << endl;



  svt_matrix4<Real64> oMat;
  cout << endl << "creation of svt_matrix4<Real64> oMat (shoul be I):" << endl 
	   << oMat << endl;


  oMat.translate(1,0,0);
  cout << "oMat.translate( 1, 0, 0):" << endl 
  	   << oMat << endl;
  
  oMat.translate(1,0,0);
  cout << "oMat.translate( 1, 0, 0):" << endl 
  	   << oMat << endl;
  
  
  cout << "Applied to oVecA:" << oMat*oVecA << endl;

  oMat.rotate(2, deg2rad(90));
  cout << "rotating by 90° (" << deg2rad(90) << ") araound z-axis: " << endl << oMat<< endl;
  cout << "Applied to oVecA:" << oMat*oVecA << endl;

  oMat.rotate(2, deg2rad(90));
  cout << "rotating by 90° araound z-axis: " << endl << oMat<< endl;
  cout << "Applied to oVecA:" << oMat*oVecA << endl;


  oMat.loadIdentity();
  cout << "restarting - oMat = " << oMat;

  oVecB[0] = 1;
  oVecB[1] = 2;
  oVecB[2] = 3;
  oVecB[3] = deg2rad(30);
  oVecB.normalize();



  cout << "setting oVecB to " << endl << oVecB << endl;
  cout << "-oVecB is therefore " << endl << -oVecB << endl;

  oMat.rotate(oVecB);
  cout << "rotating oMat by vector oVecB:" << endl << oMat << endl;
  oMat.rotate(-oVecB);
  cout << "rotating oMat by vector -oVecB shoud give I (w-coordinate not affected by negate-operator):"
	   << endl << oMat << endl;

  svt_vector4<Real64> oVecC;
  cout << "undo all for oMat and introducing new vector oVecC: " << endl << oVecC << endl ;

  oMat.loadIdentity();

  oMat.translate(1,2,3).rotate(oVecB).scale(1.5, 1.8, 2);
  cout << "translating oMat by (1,2,3), rotating it by vector oVecB, then scale by (1.5,1.8,2):" << endl << oMat << endl;
  oVecC*=oMat;
  cout << "applied to oVecC: " << endl << oVecC << endl;

  cout << "inverting oMat: " << endl << oMat.invert() << endl;
  oVecC*=oMat;
  cout << "applied to oVecC should re-transform it into origin: " << endl << oVecC << endl;

  //testing mixed assign operators
  svt_matrix4<Real32> oMat2;
  oMat2=oMat;
  cout << "Assigning oMat to float-matrix oMat2: " << endl << oMat2;
  oMat2.loadIdentity();
  oMat=oMat2;
  cout << "setting oMat2=I and assing oMat to it: " << endl << oMat;


  // stress testing the memory management
  for(unsigned int j=0;j<10;j++)
  {
      svt_point_cloud< svt_vector4<Real64> > aArray;

      aArray.delAllPoints();

      for(unsigned int i=0; i<1000000; i++)
      {
	  aArray.addPoint( svt_genrand(), svt_genrand(), svt_genrand() );
      }
  }

  return 0;
}
