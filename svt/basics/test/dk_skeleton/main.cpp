/***************************************************************************
                          main
                          ----
    begin                : 07/17/08
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_exception.h>
#include <svt_matrix4.h>

#define DK cout << svt_trimName("direct_kinematics")

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 2)
    {
	DK << "usage: dk <input pdb> <output pdb> <output psf>" << endl;
	return 1;
    }
    
    char pFname_in[1256];
    strcpy(pFname_in, argv[1]);
    DK << "PDB file     : " << pFname_in << endl;
    
    char pFname_out[1256];
    strcpy(pFname_out, argv[2]);
    DK << "Output file  : " << pFname_out << endl;
    
    char pFname_psf[1256];
    strcpy(pFname_psf, argv[3]);
    DK << "Output psf   : " << pFname_psf << endl;
    
     
    svt_point_cloud_pdb<svt_vector4<Real64 > > oPDB1;
    
    try
    {
        // load data
        oPDB1.loadPDB( pFname_in );
    }
    catch(svt_exception oError)
    {
	oError.uiMessage();
        return 1;
    }
    
    //simple routine to add bonds between consecutive atoms in the pdb 
    for (unsigned int iIndex = 0; iIndex < oPDB1.size()-1; iIndex++)
    {
        oPDB1.addLink( iIndex, iIndex+1 );
        oPDB1.addBond( iIndex, iIndex+1 );
    }
    
    //write out the connections
    oPDB1.writePSF( pFname_psf );
    // oPDB1.writePDB( pFname_out );
    
    oPDB1.updateCoords();
    oPDB1.writePDB( pFname_out/*, true*/ );
   
    unsigned int iLinkNum = oPDB1.getLinksNum();
    int iAngle = 30;
    
    unsigned int iIndexLink1, iIndexLink2;
    iIndexLink1 = 2;
    iIndexLink2 = 5;
    
    Real64 fMinLen = -2.0;
    Real64 fMaxLen = 2.0;
    Real64 fIncLen = 0.5;
    
  //  for (unsigned int iIndexLink=0; iIndexLink<iLinkNum; iIndexLink++)
    {
        for (int iAngle1=0; iAngle1<360; iAngle1+=iAngle)
        {
            oPDB1.rotateLink(iIndexLink1, 0, deg2rad((Real64)iAngle));

            for (int iAngle2=0; iAngle2<360; iAngle2+=iAngle)
            {
//                 for(Real64 fLen=fMinLen; fLen < fMaxLen; fLen = fLen+fIncLen )
                {
                    
                    oPDB1.rotateLink(iIndexLink2, 0, deg2rad((Real64)iAngle));
                   // oPDB1.adjustLinkLength(iIndexLink2, fLen);
                    
                    oPDB1.updateCoords();
//                    oPDB1.resetLinksRotation();
                    
                    oPDB1.writePDB( pFname_out, true );


                }
            }
        }
    }

    return 1;
}
