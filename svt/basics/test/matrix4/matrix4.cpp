#include <svt_basics.h>
#include <svt_matrix4.h>

int main()
{
    svt_matrix4<float> oMat;
    svt_matrix4<float> oMatB;

    cout << oMat;
    oMat.loadIdentity();
    cout << oMat;
    oMat.translate( 5, 10, 15);
    cout << oMat;

    oMatB.loadIdentity();
    oMatB.rotate( 1, deg2rad( 180 ) );
    //oMatB.translate( 1, 1, 1 );
    oMat = oMatB * oMat;
    cout << oMat;

    return 0;
}
