/***************************************************************************
                          main
                          ----
    begin                : 06/14/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_volume.h>

#define MASK cout << svt_trimName("mask")

int main(int, char* argv[])
{
    char pFname[1256];
    char pTemp[1256];
    strcpy(pFname, argv[4]);
    svt_stripExtension(pFname, pTemp);
    sprintf(pFname, "%s_blur_pdb.situs", pTemp);
    
    Real64 fRes = 20;
    
    
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    oPDB.loadPDB( argv[1] );
    svt_matrix4<Real64> oPDBMat;

    // load situs map
    svt_volume<Real64> oVol;
    svt_matrix4<Real64> oVolMat = oVol.loadSitus( argv[2] );

    // blur structure
    MASK << "Bluring structure..." << endl;
    svt_volume<Real64>* pVolMask = oPDB.blur( oVol.getWidth(), fRes );
    pVolMask->setWidth( oVol.getWidth() );
    MASK << "Mask size: " << pVolMask->getSizeX() << " x " << pVolMask->getSizeY() << " x " << pVolMask->getSizeZ() << endl;
    MASK << "Create mask..." << endl;
    pVolMask->makeMask( 3 );
    
    
    Real64 fGridX = pVolMask->getGridX();
    Real64 fGridY = pVolMask->getGridY();
    Real64 fGridZ = pVolMask->getGridZ();
    
    //pVolMask->normalize();
    
    svt_volume<Real64> oKernel;
    oKernel.createSitusBlurringKernel(oVol.getWidth(), fRes);
    pVolMask->convolve(oKernel, true);
    pVolMask->setWidth(oVol.getWidth() );
    pVolMask->setGrid( fGridX, fGridY, fGridZ);
    
    //MASK << "Invert mask..." << endl;
    //pVolMask->invertMask();
    MASK << "Apply mask..." << endl;
    oVol.mask( *pVolMask );

    // save result
    MASK << "Save result to " << argv[3] << endl;
    oVol.saveSitus( argv[3] );
};
