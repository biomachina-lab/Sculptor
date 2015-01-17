/***************************************************************************
                          fam
                          ---
    begin                : 08/23/2007
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_iostream.h>
#include <svt_stlVector.h>
#include <svt_rnd.h>
#include <svt_types.h>
#include <svt_vector4.h>

#include <svt_point_cloud.h>
#include <svt_point_cloud_pdb.h>
#include <svt_clustering_trn.h>
#include <svt_clustering_mtrn.h>

#define FAM cout << svt_trimName("fam")

///////////////////////////////////////////////////////////////////////////
// Main Routine
///////////////////////////////////////////////////////////////////////////

// main routine
int main(int argc, char* argv[])
{
    ///////////////////////////////////////////////////////////////////////////
    // Preparation
    ///////////////////////////////////////////////////////////////////////////

    // check command line arguments
    if (argc < 2)
    {
	FAM << "usage: fam <input pdb file conf 1> <input pdb file conf 2> <min cv> <max cv> <stepsize cv> < min res> <max res> <stepsize res> <threshold> <output file structures>" << endl;
	return 1;
    }
    
    //svt_sgenrand( svt_getToD() );
    
    // temporary files names
    char pFname[1256];
    char pTemp[1256];
    strcpy(pFname, argv[10]);
    svt_stripExtension(pFname, pTemp);
    char pFnameCvDock[1256];
    char pFnameSitus[1256];
    char pFnameInputLbg[1256];
    sprintf(pFnameInputLbg , "%s.lbg.in", pTemp);
    char pLbgCmdLn[1256];
     
    //Variables
    unsigned int iVoxelSize;
    
    ///////////////////////////////////////////////////////////////////////////
    // Input data
    ///////////////////////////////////////////////////////////////////////////

    // load structure
    FAM << "Loading dock pdb file: " << argv[1] << endl;
    FAM << "Loading target pdb file: " << argv[2] << endl;

    svt_point_cloud_pdb< svt_vector4<Real64> > oDockPdb, oTargetPdb;
    
    oDockPdb.loadPDB( argv[1] );
    oDockPdb.setIgnoreWater( false );
    
    oTargetPdb.loadPDB( argv[2] );
    oTargetPdb.setIgnoreWater( false );
    
    unsigned int iMinCv         = atoi( argv[3] );
    unsigned int iMaxCv         = atoi( argv[4] );
    unsigned int iStepsizeCv    = atoi( argv[5] );
       
    unsigned int iMinRes        = atoi( argv[6] );
    unsigned int iMaxRes        = atoi( argv[7] );
    unsigned int iStepsizeRes   = atoi( argv[8] );
    
    Real64 fThreshold           = atof( argv[9] );
    
    FAM << "Codebook count: from "  << iMinCv   << " to " << iMaxCv     << " by " << iStepsizeCv    << endl;
    FAM << "Resolution from "       << iMinRes  << " to " << iMaxRes    << " by " << iStepsizeRes   << endl;
    FAM << "Threshold: "            <<  fThreshold  << endl;
    
    //high resolution volumes
    svt_volume< Real64 > *oVolHighRes = oTargetPdb.blur(2, 3);
    oVolHighRes->setWidth(2);
    
    oVolHighRes->setCutoff( fThreshold );
    
    sprintf(pFnameSitus , "%s_res_3_dock.situs", pTemp);
    oVolHighRes->saveSitus( pFnameSitus );
    
    for (unsigned int iCv = iMinCv; iCv <= iMaxCv; iCv += iStepsizeCv )
    {
        FAM << "Codebook vectors selected:" << iCv << endl;
        sprintf(pFnameSitus , "%s_res_3_dock.situs", pTemp);
        ///////////////////////////////////////////////////////////////////////////
        // Compute Codebook vectors for High Resolution structures
        ///////////////////////////////////////////////////////////////////////////
        
        svt_point_cloud< svt_vector4<Real64> > oCvDock, oCvFamHigh, oCvFamLow, oCvTrnHigh, oCvTrnLow, oCvLbgHigh, oCvLbgLow;
        vector< Real64 > oRmsd;
        svt_point_cloud< svt_vector4< Real64> > oCVTargetFAM, oCVTargetTRN, oCVTargetLBG;
        svt_point_cloud_pdb< svt_vector4< Real64> > oResultTRN, oResultFAM, oResultLBG;
        
        // Dock pdb
        svt_clustering_trn< svt_vector4<Real64> > oClusterPDB;
        oClusterPDB.setMaxstep( 100000 );
        oDockPdb.setIgnoreWater( true );
        oCvDock = oClusterPDB.cluster(iCv, 8, oDockPdb);
        
        //write pdb
        sprintf(pFnameCvDock , "%s_dock_cv_%d.pdb", pTemp, iCv);
        oCvDock.writePDB( pFnameCvDock );
        
        //LBG
        //write the input file
        FILE* pFile = NULL;
        pFile = fopen(pFnameInputLbg,"w");
        fprintf(pFile,"%d\n%g\n%d\n%d\n", 1, fThreshold, 1 ,1);
        fclose(pFile);
        
        //run lbg in situs
        sprintf(pFname , "%s_cv_%d_res_%d_lbg.pdb", pTemp, iCv, 2);
        sprintf(pLbgCmdLn,"qvol %s %s %s < %s", pFnameSitus, pFnameCvDock, pFname, pFnameInputLbg);
        svt_system( pLbgCmdLn );
        
        //read cv output of the lbg
        oCvLbgHigh.loadPDB( pFname );
        oResultLBG.loadPDB( pFname );
        
        // Target PDB - high resolution map
        //TRN
        svt_clustering_trn< svt_vector4<Real64> > oClusterTRN;
        oClusterTRN.setMaxstep( 100000 );
        oCvTrnHigh = oClusterTRN.cluster(iCv, 8, *oVolHighRes);
        
        //write pdb
        sprintf(pFname , "%s_cv_%d_trn.pdb", pTemp, iCv);
        oCvTrnHigh.writePDB( pFname );
        oResultTRN.loadPDB( pFname );
        
        //FAM
        svt_clustering_mtrn< svt_vector4<Real64> > oClusterAlgo;
        oClusterAlgo.setMaxstep( 100000 );
        oClusterAlgo.setEpsilon( 0.01f, 0.000000005f );
        oClusterAlgo.addSystem( oCvDock, oCvDock );

        FAM << "Cluster analysis of the high resolution target ..." << endl;
        oClusterAlgo.train( *oVolHighRes );
        oCvFamHigh = oClusterAlgo.getCodebook( 0 );
        
        //write pdb
        sprintf(pFname , "%s_cv_%d_fam.pdb", pTemp, iCv);
        oCvFamHigh.writePDB( pFname );
        oResultFAM.loadPDB( pFname );
        
        for (unsigned int iRes = iMinRes; iRes < iMaxRes; iRes += iStepsizeRes)
        {
            FAM << "Resolution : " << iRes << endl;
            
            // get voxel size
            if (iRes < 9)
                iVoxelSize = 2;
            else 
                if (iRes < 12)
                    iVoxelSize = 3;
                else 
                    iVoxelSize = 4;
                
                
            // First step: blur the 2nd conformation
            svt_volume< Real64 > *oTargetVol = oTargetPdb.blur(iVoxelSize, iRes);
            oTargetVol->setWidth( iVoxelSize );
            
            Real64 fThresh = oTargetVol->correspondingISO( *oVolHighRes, fThreshold );
            oTargetVol->setCutoff( fThresh );
            
            // Save the situs file
            sprintf(pFnameSitus , "%s_res_%d.situs", pTemp, iRes);
            oTargetVol->saveSitus(pFnameSitus);
            
            // some stats about the volume
            FAM << "Min density: " << oTargetVol->getMinDensity() << endl;
            FAM << "Max density: " << oTargetVol->getMaxDensity() << endl;
            FAM << "Threshold:   " << fThresh << endl;
            
            
            //LBG
            //write the input file
            FILE* pFile = NULL;
            pFile = fopen(pFnameInputLbg,"w");
            fprintf(pFile,"%d\n%g\n%d\n%d\n", 1, fThresh, 1 ,1);
            fclose(pFile);
            
            //run lbg in situs
            sprintf(pFname , "%s_cv_%d_res_%d_lbg.pdb", pTemp, iCv, iRes);
            sprintf(pLbgCmdLn,"qvol %s %s %s < %s", pFnameSitus, pFnameCvDock, pFname, pFnameInputLbg);
            svt_system( pLbgCmdLn );
            
            //read cv output of the lbg
            oCvLbgLow.loadPDB( pFname );
            
            // Second step: Cluster the MAP
            //TRN
            oCvTrnLow = oClusterTRN.cluster(iCv, 8, *oTargetVol);
//             sprintf(pFname , "%s_cv_%d_res_%d_trn.pdb", pTemp, iCv, iRes);
//             oCvTrnLow.writePDB( pFname );
            
            //FAM
            // set parameters for the clustering
            svt_clustering_mtrn< svt_vector4<Real64> > oClusterAlgo;
            oClusterAlgo.setMaxstep( 100000 );
            oClusterAlgo.setEpsilon( 0.01f, 0.000000005f );
            
            oClusterAlgo.addSystem( oCvDock, oCvDock );
            oClusterAlgo.train( *oTargetVol );
        
            oCvFamLow = oClusterAlgo.getCodebook( 0 );
//             sprintf(pFname , "%s_cv_%d_res_%d_fam.pdb", pTemp, iCv, iRes);
//             oCvFamLow.writePDB( pFname );
            
           
            
            //compute RMSDs
            oRmsd.push_back(oCvTrnLow.rmsd(oCvTrnHigh) );
            oRmsd.push_back(oCvTrnLow.rmsd_NN(oCvTrnHigh) );
            oRmsd.push_back(oCvFamLow.rmsd(oCvFamHigh) );
            oRmsd.push_back(oCvFamLow.rmsd_NN(oCvFamHigh) );
            oRmsd.push_back(oCvLbgLow.rmsd(oCvLbgHigh) );
            oRmsd.push_back(oCvLbgLow.rmsd_NN(oCvLbgHigh) );
            
            FAM << endl;
            FAM << "TRN RMSD:" << oCvTrnLow.rmsd(oCvTrnHigh)  << " " << oCvTrnLow.rmsd_NN(oCvTrnHigh)  << " " << endl;
            FAM << "FAM RMSD:" << oCvFamLow.rmsd(oCvFamHigh)  << " " << oCvFamLow.rmsd_NN(oCvFamHigh) << " " << endl;
            FAM << "LBG RMSD:" << oCvLbgLow.rmsd(oCvLbgHigh)  << " " << oCvLbgLow.rmsd_NN(oCvLbgHigh) << " " << endl;
            
            //save the frame
            oResultTRN.addTimestep();
            oResultFAM.addTimestep();
            oResultLBG.addTimestep();
            for (unsigned int iIndexAtom = 0; iIndexAtom < oCvTrnHigh.size(); iIndexAtom++)
            {
                oResultTRN.setPoint(iIndexAtom, oCvTrnLow[iIndexAtom]);
                oResultFAM.setPoint(iIndexAtom, oCvFamLow[iIndexAtom]);
                oResultLBG.setPoint(iIndexAtom, oCvLbgLow[iIndexAtom]);
                //FAM << "DEBUG x:" << oCvTrnLow[iIndexAtom].x() << "\ty:" << oCvTrnLow[iIndexAtom].y() << "\tz:" << oCvTrnLow[iIndexAtom].z() << endl;
            }
        }
        
        unsigned int iInc = 6;
        sprintf(pFname , "%s_%d_rmsd.tab", pTemp, iCv);
        pFile = fopen(pFname,"w");
        for (unsigned int iIndex=0; iIndex < oRmsd.size(); iIndex+=iInc)
        {
            fprintf(pFile,"%d\t%g\t%g\t%g\t%g\t%g\t%g\n",iMinRes+iIndex/iInc*iStepsizeRes,
                    oRmsd[ iIndex ], oRmsd[ iIndex + 1 ], 
                    oRmsd[ iIndex + 2 ], oRmsd[ iIndex + 3 ],
                    oRmsd[ iIndex + 4 ], oRmsd[ iIndex + 5 ] );
        }
        fclose(pFile);
        
        oResultTRN.setTimestep(0);
        oResultFAM.setTimestep(0);
        oResultLBG.setTimestep(0);
        FAM << "Saving feature vectors for map file " << argv[7] << endl;
        
        sprintf(pFname , "%s_cv_%d_res_%d_%d_trn.pdb", pTemp, iCv, iMinRes, iMaxRes);
        oResultTRN.writePDB( pFname );
        sprintf(pFname , "%s_cv_%d_res_%d_%d_fam.pdb", pTemp, iCv, iMinRes, iMaxRes);
        oResultFAM.writePDB( pFname );
        sprintf(pFname , "%s_cv_%d_res_%d_%d_lbg.pdb", pTemp, iCv, iMinRes, iMaxRes);
        oResultLBG.writePDB( pFname );
    }
 
    return 0;
}
