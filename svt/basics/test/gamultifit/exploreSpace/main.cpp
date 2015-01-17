/***************************************************************************
                          main
                          program that allows the computation of scores for different know 
                          structures to enable the study of weights
                          ---------
    begin                : 01/21/2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

// the classes of the algorithm
#include "/mnt/biomachina/home/mrusu/Dev/sculptor/projects/multidomain/ga/svt_gamultifit.h"

// streams
#include <iostream>
using namespace std;

#define SVT_GA cout << svt_trimName("svt_ga")
// main routine
int main(int argc, char* argv[])
{
    
        // check command line arguments
    if (argc < 3)
    {
	SVT_GA << "usage: gafit <pdb files> <target map > <target struc> <output dir>" << endl;
	return 10;
    }

    vector<string> oPdbFnames,oCvFnames;
    string oMapFname, oPdbTarFname;
    
    unsigned int iNoUnits = (argc-8)/2;
    
    vector<svt_point_cloud_pdb < svt_vector4< Real64 > > > oPdbs, oCVs;
    svt_point_cloud_pdb < svt_vector4< Real64 > > oPdb, oPdbTar, oPdbCA, oCV;
    
    
    for (unsigned int iUnit=1; iUnit<=iNoUnits; iUnit++)
    {
        oPdbFnames.push_back( argv[iUnit] );
        oCvFnames.push_back( argv[iNoUnits+iUnit] );
        
        SVT_GA << "Loading PDB from :" << argv[iUnit] << endl;
        oPdb.loadPDB(oPdbFnames[iUnit-1].c_str());
        oPdbs.push_back(oPdb);
        
        oCV.loadPDB(oCvFnames[iUnit-1].c_str());
        oCVs.push_back(oCV);
    }
    
    SVT_GA << "Load Map:" << oMapFname << endl;
    oMapFname = argv[argc-7];
    
    svt_volume < Real64 > oMap;
    oMap.loadSitus(oMapFname.c_str());
    
    Real64 fRes = 20.0f;
    
    Real64 fOrigX = oMap.getGridX();
    Real64 fOrigY = oMap.getGridY();
    Real64 fOrigZ = oMap.getGridZ();
    Real64 fWidth = oMap.getWidth();
    
    int iSizeX = oMap.getSizeX();
    int iSizeY = oMap.getSizeY();
    int iSizeZ = oMap.getSizeZ();
    
    
    oPdbTarFname = argv[argc-6];
    oPdbTar.loadPDB(oPdbTarFname.c_str());
    

     
    Real64 fMinX1, fMaxX1, fMinY1, fMaxY1, fMinZ1, fMaxZ1;
    vector<Real64> oMinX, oMaxX, oMinY, oMaxY, oMinZ, oMaxZ;
    Real64 fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ;
    
    fMinX1 = oPdbTar.getMinXCoord();
    fMaxX1 = oPdbTar.getMaxXCoord();
    fMinY1 = oPdbTar.getMinYCoord();
    fMaxY1 = oPdbTar.getMaxYCoord();
    fMinZ1 = oPdbTar.getMinZCoord();
    fMaxZ1 = oPdbTar.getMaxZCoord();

    unsigned int iGenes = iNoUnits*6;
    
    svt_gamultifit oGA( iGenes );
    oGA.setUnits( oPdbs );
    oGA.setCoarseUnits( oCVs );
    oGA.setTarget( oMap );
    oGA.setTarStr( oPdbTar );
    
    oGA.setTarResolution( fRes ); 
    
    score eScore = (score)atoi(argv[argc-5]);
    oGA.setScore(eScore); 
    
    Real64 fWeightPenalty = atof(argv[argc-4]);
    oGA.setWeightPenalty( fWeightPenalty );
    
    Real64 fWeightTail = atof(argv[argc-3]);
    oGA.setWeightTail( fWeightTail );
    
    Real64 fPenaltyOffset = atof(argv[argc-2]);
    oGA.setPenaltyOffset( fPenaltyOffset );
    
    Real64 fDistEq = 0.3 /*atof(argv[argc-3])*/;
    oGA.setDistEq(fDistEq);
    
    Real64 fDistMin = 0.245/*atof(argv[argc-2])*/;
    oGA.setDistMin(fDistMin);
     
    //make path
    char pPath[1106];
    strcpy(pPath, argv[argc-1]);
    oGA.setOutputPath( pPath );
    
    char pCmd[1106];
    sprintf( pCmd, "if [ -d %s ]\n then\n rm -r %s\n fi\n mkdir %s\n", pPath, pPath, pPath);
    svt_system( pCmd );
    
    oGA.initPopulation(iGenes);

    vector<Real64> oGenes;
    for (unsigned int iIndex=0; iIndex < iNoUnits; iIndex++)
    {
        oGenes.push_back(0.5f);
        oGenes.push_back(0.5f);
        oGenes.push_back(0.5f);
        oGenes.push_back(0.5f);
        oGenes.push_back(0.0f);   
        oGenes.push_back(0.5f);
    }
    
    oGA.setPrint(true);
    
    svt_ga_ind oInd; 
    svt_point_cloud_pdb<svt_vector4<Real64> > oModel, oCoarseModel,oCoarseModel2, oCoarseModel3;
    svt_volume<Real64> oModelVol;
    char pFname[106],pFname2[106];
    unsigned int iIndex = 0;
    cout << "Model: " << iIndex << endl;
    unsigned int iUnit;
    for (iUnit=0; iUnit<iNoUnits; iUnit++)
    {
        fMinX = oPdbs[iUnit].getMinXCoord();
        fMaxX = oPdbs[iUnit].getMaxXCoord();
        fMinY = oPdbs[iUnit].getMinYCoord();
        fMaxY = oPdbs[iUnit].getMaxYCoord();
        fMinZ = oPdbs[iUnit].getMinZCoord();
        fMaxZ = oPdbs[iUnit].getMaxZCoord();
        
        oMinX.push_back(fMinX);
        oMinY.push_back(fMinY);
        oMinZ.push_back(fMinZ);
        
        oMaxX.push_back(fMaxX);
        oMaxY.push_back(fMaxY);
        oMaxZ.push_back(fMaxZ);
        
        oGenes[iUnit*6+0] =(0.5*(fMaxX+fMinX) - oMap.getGridX() )/(oMap.getWidth()*(Real64)oMap.getSizeX());
        oGenes[iUnit*6+1] =(0.5*(fMaxY+fMinY) - oMap.getGridY() )/(oMap.getWidth()*(Real64)oMap.getSizeY());
        oGenes[iUnit*6+2] =(0.5*(fMaxZ+fMinZ) - oMap.getGridZ() )/(oMap.getWidth()*(Real64)oMap.getSizeZ());
    }
    
    oInd.setGenes( oGenes );
    oGA.updateFitness(&oInd);
    cout << oInd.getFitness() << "\t"; oInd.printGenes();
    
    oModel      = oGA.getModel();
    oModelVol   = oGA.getModelVol();
    oCoarseModel = oGA.getCoarseModel();
    oCoarseModel2 = oGA.getCoarseModel2();
    
    sprintf(pFname, "%s/Model2.pdb", pPath);
    oModel.writePDB( pFname );
    
    oGA.calcTransFeatures(&oInd);
    
    sprintf(pFname, "%s/CoarseModel.pdb", pPath);
    oCoarseModel.writePDB( pFname );
    
    sprintf(pFname, "%s/CoarseModel.psf", pPath);
    oCoarseModel2.writePSF( pFname );
    
    
    Real64 x0 = (oMaxX[0]+oMinX[0])/2.0f;
    Real64 x1 = (oMaxX[1]+oMinX[1])/2.0f;
    Real64 x2 = (oMaxX[2]+oMinX[2])/2.0f;
    
    Real64 y0 = (oMaxY[0]+oMinY[0])/2.0f;
    Real64 y1 = (oMaxY[1]+oMinY[1])/2.0f;
    Real64 y2 = (oMaxY[2]+oMinY[2])/2.0f;
    
    Real64 z0 = (oMaxZ[0]+oMinZ[0])/2.0f;
    Real64 z1 = (oMaxZ[1]+oMinZ[1])/2.0f;
    Real64 z2 = (oMaxZ[2]+oMinZ[2])/2.0f;
    
    cout << x0 << "\t" << x1 << "\t" << x2 << endl; 
    cout << y0 << "\t" << y1 << "\t" << y2 << endl; 
    cout << z0 << "\t" << z1 << "\t" << z2 << endl; 
    
    
    Real64 fD  = x0*y1*z2 + x1*y2*z0 + x2*y0*z1 - x2*y1*z0 - x0*y2*z1 - x1*y0*z2;
    Real64 fDa = 1.0*y1*z2 +1.0*y2*z0 + 1.0*y0*z1 -1.0*y1*z0 -1.0*y2*z1 -1.0*y0*z2;
    Real64 fDb = x0*1.0*z2 + x1*1.0*z0 + x2*1.0*z1 - x2*1.0*z0 - x0*1.0*z1 - x1*1.0*z2;
    Real64 fDc = x0*y1*1.0 + x1*y2*1.0 + x2*y0*1.0 - x2*y1*1.0 - x0*y2* 1.0- x1*y0*1.0 ;
    
    cout << fD << "\t" << fDa << "\t" << fDb << "\t" << fDc << "\t" << endl; 
    
    Real64 a = -1.0/fD*fDa;
    Real64 b = -1.0/fD*fDb;
    Real64 c = -1.0/fD*fDc;
    
    cout << a << "\t" << b << "\t" << c << "\t" << 1.0 << "\t" << endl; 
    
    oGA.calcTransFeatures( &oInd );
    
    int iMinX=-5, iMinY=-5, iMinZ=-5, iMaxX=+5, iMaxY=+5, iMaxZ=+5;
    int iSpacing = 1;
    int iZ = 0;
    Real64 fX, fY, fZ;
    svt_matrix<Real64> oCC;
    oCC.resize((int)((iMaxX-iMinX+iSpacing)/(Real64)iSpacing), (int)((iMaxY-iMinY+iSpacing)/(Real64)iSpacing) );
    
    Real64 fAngSpacing = 0.25/*20.0/360.0*/;
    Real64 fTmpCC,fMaxCC;
    
    Real64 fMinCCRx, fMinCCRy, fMinCCRz;
    
    sprintf(pFname,"%s/cc.tab",pPath);
    FILE *file;
    file = fopen(pFname,"w");
    
    long iStartTime;
    long iETime;
    iUnit=0;
    
    oGA.setPrint(false);
    
    int iIndex1=-1,iIndex2=-1;
    for (int iX=iMinX; iX<=iMaxX; iX+=iSpacing)
    {
        iIndex1++;
        iIndex2 = -1;
        cout << "  "  << iIndex1 << endl;
        
        fX = 0.5*(oMaxX[iUnit]+oMinX[iUnit]) - iX*(x2-x1)/5.0f;
                
        for (int iY=iMinY; iY<=iMaxY; iY+=iSpacing)
        {
            iIndex2++;
            cout << iIndex2 << endl;
            
            fY = 0.5*(oMaxY[iUnit]+oMinY[iUnit]) - iY*(y2-y1)/20.0f;
            
            fZ =  (fD-fX*fDa-fY*fDb)/fDc;
            
            cout << fX << "\t" << fY << "\t" << fZ << "\t" << endl; 
            
            oGenes[iUnit*6+0] =(fX - fOrigX)/(fWidth*(Real64)iSizeX);
            oGenes[iUnit*6+1] =(fY - fOrigY)/(fWidth*(Real64)iSizeY);
            oGenes[iUnit*6+2] =(fZ - fOrigZ)/(fWidth*(Real64)iSizeZ);
    
            iStartTime = svt_getElapsedTime();
            fMaxCC = -1e10;
            for (Real64 iRx=0; iRx < 1; iRx += fAngSpacing )
            {
                oGenes[iUnit*6+3] = iRx;
                for (Real64 iRy=0; iRy < 1; iRy += fAngSpacing*2 )
                {
                    oGenes[iUnit*6+4] = iRy; 
                    for (Real64 iRz=0; iRz < 1; iRz += fAngSpacing )
                    {
                        oGenes[iUnit*6+5] = iRz;
                        oInd.setGenes( oGenes ); 
                        oGA.updateFitness(&oInd);
                        //oInd.printGenes();
                        
                        fTmpCC = oInd.getFitness();
                        if (fTmpCC > fMaxCC)
                        {
                            fMaxCC = fTmpCC;
                            fMinCCRx = iRx;
                            fMinCCRy = iRy;
                            fMinCCRz = iRz;
                        }
                    }
                }
            }
            
            oCC[iIndex1][iIndex2] = fMaxCC;
            
            oGenes[iUnit*6+3]  = fMinCCRx;
            oGenes[iUnit*6+4]  = fMinCCRy;
            oGenes[iUnit*6+5]  = fMinCCRz;
            
            oInd.setGenes( oGenes ); 
            oGA.setPrint(true);
            oGA.updateFitness(&oInd);
            oInd.printGenes();
            oGA.setPrint(false);
            
            oModel      = oGA.getModel();
            sprintf(pFname, "%s/Model.pdb", pPath);
            oModel.writePDB( pFname, true );
            
            fprintf(file, "%8.6f ", oCC[iIndex1][iIndex2] );
            iETime = svt_getElapsedTime() - iStartTime;
            cout << "Elapsed time: " << iETime/(1000.0f) << " s" <<  endl;
            iStartTime = svt_getElapsedTime();
        }
        fprintf(file, "\n");
        fflush(file);

    }
  
    fclose(file);
    
    return 0;
}
