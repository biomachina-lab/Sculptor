#include <svt_basics.h>
#include <svt_vector4.h>
#include <svt_point_cloud_pdb.h>

int main()
{
    svt_vector4<Real64> oVec;
    
    oVec.x( 1.0f );
    oVec.y( 1.0f );
    oVec.z( 1.0f );
    cout << "[" << oVec.x() << "," << oVec.y() << "," << oVec.z() << "]" << endl;

    oVec.initUniformQuaternion();
    cout << "[" << oVec.x() << "," << oVec.y() << "," << oVec.z() <<  "," << oVec.w() << "]" << endl;
    
    svt_point_cloud_pdb< svt_vector4 <Real64> > oPdb;
    svt_point_cloud_atom oAtom;
    vector<svt_vector4< Real64> > oArray;
    
    for (unsigned int iIndex=0; iIndex < 1000; iIndex++)
    {
        oVec.initUniformQuaternion();
        oArray.push_back(oVec);
        //cout << oVec.w() << endl;
        oPdb.addAtom(oAtom, oVec);
    }
    
    oPdb.writePDB("test.pdb"); // draw with VDW mode with radius 0.05
    
    for (unsigned int iIndex=0; iIndex < 1000; iIndex++)
    {
        oVec = oPdb[iIndex];
        //cout << "[" << oVec.x() << "," << oVec.y() << "," << oVec.z() <<  "," << oVec.w() << "]" << endl;
        oVec = oArray[iIndex];/*
        cout << "[" << oVec.x() << "," << oVec.y() << "," << oVec.z() <<  "," << oVec.w() << "]" << endl;
        cout << oVec.w() << "\t";*/
        oVec.setQuat2Rot();
        //oArray[iIndex] = oVec;
        oPdb[iIndex] = oVec;
        
    }
    
    oPdb.writePDB("test2.pdb"); // draw with VDW mode with radius 0.05
    
    svt_vector4<Real64> oRand, oProd;
    
    oVec = oArray[0];
    cout << "[" << oVec.x() << "," << oVec.y() << "," << oVec.z() <<  "," << oVec.w() << "]" << endl;
    
    for (unsigned int iIndex=0; iIndex < 1000; iIndex++)
    {
        oRand.initUniformQuaternion();
        oProd = quaternionVectorProduct(oVec,oRand);
        
        oPdb[iIndex] = oProd;
        cout << oRand.quaternionLengthSq() << " " << oProd.x()*oProd.x()+oProd.y()*oProd.y()+oProd.z()*oProd.z()+oProd.w()*oProd.w()<< " " << oProd.quaternionLengthSq() << endl;
    }
    
    oPdb.writePDB("test3.pdb"); // draw with VDW mode with radius 0.05
    
    

    return 0;
}
