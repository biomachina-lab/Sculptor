#include <svt_basics.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_pdb.h>

int main()
{
    svt_matrix4<Real64> oMat;
    svt_vector4<Real64> oVec;
    
    cout << oMat;
    oMat.loadIdentity();
    cout << oMat;
    
    oVec.x(1.0f);
    oVec.y(1.0f);
    oVec.z(1.0f);
    
    oVec.normalize();
    
    svt_point_cloud_pdb<svt_vector4<Real64> > oPdb, oPdbTmp;
    oPdb.loadPDB("test.pdb");

    svt_matrix4 <Real64> oTranslation;
    oTranslation.loadIdentity();
    oTranslation.setTranslation(-oPDB.coa());

    int iCurrentTimestep = oPDB.getTimestep();
    for(int iTS=0; iTS<oPDB.getMaxTimestep(); iTS++)
    {
	oPDB.setTimestep( iTS );

	for( unsigned int i=0; i<m_pPdbNode->size(); i++)
	{
	    oPDB[i] = oTranslation * oPDB[i];
	}
    }
    oPDB.setTimestep( iCurrentTimestep );

    oPdb.writePDB("center.pdb");
    
    oPdb.writePDB("rot.pdb");
    
    for (Real64 fAngle = deg2rad(-180.0f); fAngle <  deg2rad(180.0f); fAngle+=deg2rad((360.0f)/10.0f))
    {
        oMat.loadIdentity();
        oMat.rotate(oVec, fAngle);
        cout << oMat;
        
        oPdbTmp = oMat*oPdb;
        
        oPdbTmp.writePDB("rot.pdb", true);
    }
    
    oMat.loadIdentity();
    oMat.rotate(oVec, deg2rad(36.0f));
    
    oPdbTmp = oMat*oPdb;
        
    oPdbTmp.writePDB("36.pdb");
    
    //check if by applying a rotation by -36 deg one gets the same results as the starting matrix
     
    oMat.loadIdentity();
    oMat.rotate(oVec, deg2rad(-36.0f));
    
    oPdbTmp = oMat*oPdbTmp;
    oPdbTmp.writePDB("same_as_center.pdb"); 

    return 0;
}
