/***************************************************************************
                          main
                          fit multiple domains into map
                          ---------
    begin                : 01/21/2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_gacylinder.h>
#include <svt_gacylinder_ind.h>
#include <svt_config.h>
#include <svt_iostream.h>
#include <svt_stride.h>

#define SVT_GA cout << svt_trimName("svt_gacylinder")

// Define __GAFIT_FORK to activate the process-based parallelization (it seems to be a tad slower then pthreads, but that needs to be tested more and might depend on the platform).
//#define __GAFIT_FORK

#ifdef __GAFIT_FORK
#include <sys/wait.h>
#endif

enum
{   NONE,       //0
    GAUSSIAN,   //1
    BILATERAL,  //2
    LOCAL    
};
//
// Main routine
//
int main(int argc, char* argv[])
{
    svt_sgenrand( svt_getToD() );

    //
    // Check command line arguments
    //
    if (argc < 2)
    {
    SVT_GA << "usage: gafit <config file>" << endl;
    return 10;
    }

    //
    // Configuration file
    //
    svt_config oConf( argv[1] );

    //
    // Parse the main parameters
    //
    // Map Filename
    string oMapFname            = oConf.getValue("MapFilename", "");
    // Target PDB structure filename
    string oPdbTarFname         = oConf.getValue("TargetFilename", "");
    if (oMapFname.size() == 0 && oPdbTarFname.size()==0)
    {
        SVT_GA << "Error: Please specify the input (e.g. MapFilename = test.situs; or TargetFilename=test.pdb ) in your config file!" << endl;
        exit(1);
    }
    //the filename of the secondary structure prediction
    string  oSsePredFname       = oConf.getValue("SsePredFilename", "");
    //evantual template - represent one unit that will be repeated several times
    string oTemplateFname       = oConf.getValue("TemplateFilename", "");
    // Resolution
    Real64 fRes                 = oConf.getValue("Resolution", 10.0);
    //voxel
    Real64 fWidth               = oConf.getValue("VoxelWidth", 2.0);
    // Population size
    int iPopSize                = oConf.getValue("PopulationSize", 500);
    // Scoring function
    //int eScore                  = oConf.getValue("ScoreFunction", 3);
    // Reinsertion scheme
    int eReinsertion            = oConf.getValue("ReinsertionScheme", 1);
    // Mutation probability
    Real64 fMP                  = oConf.getValue("MutationProbability", 0.05);
    // Mutation offset
    Real64 fMO                  = oConf.getValue("MutationOffset", 0.05);
    // Crossover probability
    Real64 fCP                  = oConf.getValue("CrossoverProbability", 0.95);
    // Selective pressure
    Real64 fSP                  = oConf.getValue("SelectivePressure", 1.3);
    // Output path
    string oPath                = oConf.getValue("OutputPath", "");
    if (oPath.size() == 0)
    {
        SVT_GA << "Error: Please specify an output path (e.g. OutputPath = /tmp/test) in your config file!" << endl;
        exit(1);
    }
    // max generations
    int iMaxGen                 = oConf.getValue("MaxGenerations", 10000);
    // max runs
    int iMaxRun                 = oConf.getValue("MaxRuns", 5);
    // max Threads
    int iMaxThread              = oConf.getValue("MaxThreads", 2);
    // angular step size    
    Real64 fAngularStepSize     = oConf.getValue("AngularStepSize", 1.0 );
    // angular search range
    Real64 fPsiFrom             = oConf.getValue("PsiFrom"  , 0.0 );
    Real64 fPsiTo               = oConf.getValue("PsiTo"    , 360.0 );
    Real64 fThetaFrom           = oConf.getValue("ThetaFrom", 0.0 );
    Real64 fThetaTo             = oConf.getValue("ThetaTo"  , 180.0 );
    Real64 fPhiFrom             = oConf.getValue("PhiFrom"  , 0.0 );
    Real64 fPhiTo               = oConf.getValue("PhiTo"    , 0.0 );
   
    // transposition probability
    Real64 fTranspositionProb   = oConf.getValue("TranspositionProbability", 0.0);
    // cutoff distance
    Real64 fDistanceThreshold   = oConf.getValue("DistanceThreshold", 5.0 ); 
    // cutoff distance penalty
    Real64 fDistancePenalty     = oConf.getValue("DistancePenalty", 0.90);
    // all mutate on / off
    bool bMutateAll             = oConf.getValue("MutateAll", true );
    // all mutate Proportion
    Real64 fMutateAllProportion = oConf.getValue("MutateAllProportion", 1.00 );
    // logfile
    string oLogFname            = oConf.getValue("Logfile", "");
    // how often should the model get exported, every generation (=1) or every 10 (=10) or never (=0)
    int iWriteModelInterval     = oConf.getValue("WriteModelInterval", 0);
    // stopping criterion
    Real64 fStopScore           = oConf.getValue("StopScore", 0.0);
    // size of tabu window - see svt_ga.h for details
    unsigned int iTabuWindowSize = oConf.getValue("TabuWindowSize", 35 );
    // tabu threshold - see svt_ga.h for details
    Real64 fTabuThreshold       = oConf.getValue("TabuThreshold", 4.0 );
    // tabu region size - see svt_ga.h for details
   Real64 fTabuRegionSize       = oConf.getValue("TabuRegionSize", 6.0); 
    // sync after how many generations?
    int iSyncGen                = oConf.getValue("ThreadSyncGenerations", 100);
   //should target structure be blurred : and don't use target volume and blur at resolution indicated: 
    bool bBlurTarget            = oConf.getValue("BlurTargetStruct", false);
    // RefinementMaxMutPerGene
    int iRefinementMaxMutPerGene= oConf.getValue("RefinementMaxMutPerGene", 5);
    // Number of Tubes to output: if = 0 estimate from target
    int iNoOfTubes              = oConf.getValue("NumberOfTraces",0);
    // shoud the model be burred
    bool bApplyBlurring2Model   = false; //oConf.getValue("ApplyBlurring2Model", true);
    // the radius of the template used in the expansion
    Real64 fTemplateRadius      = oConf.getValue("TemplateRadius", 1.0);
    // the radius of the template used in the ga Search
    Real64 fSearchTemplateRadius= oConf.getValue("SearchTemplateRadius", 2.0);
    // the number of points in the template n-1 are in the circle and one in the center
    unsigned int iTemplatePointCount  = oConf.getValue("TemplatePointCount", 31.0);
    // number of times the circle is repeated in the search template
    int iTemplateRepeats        = oConf.getValue("TemplateRepeats", 8 );
    // number of times the circle is repeated in the search template
    int iSearchTemplateRepeats  = oConf.getValue("SearchTemplateRepeats", 20 );
     // distance between repeats
    Real64 fDistBetweenRepeats  = oConf.getValue("DistBetweenRepeats", 1.0);
    // distance covered in one crawling step
    Real64 fCrawlingStep        = oConf.getValue("CrawlingStepSize", 1.4); // experimental value
    // how much of the original score is allowed to accept a move ~ 0.7*OrigScore
    Real64 fAcceptMoveRatio     = oConf.getValue("AcceptMoveRatio", 0.70);
    // number of template copies in the model
    int iMinResInHelix          = oConf.getValue("MinNumberOfResiduesInHelix", 7 ); 
    // the number of failed crawls that are accepted before stopping
    unsigned int iMaxFailedCrawls = oConf.getValue("MaxFailedCrawls", 2);
    // should a high resolution helix be fitted in the rods
    bool bFitHelix              = oConf.getValue("FitHelix", false);
    // type of filter to apply
    unsigned int iFilter        = oConf.getValue("Filter",  3);
    //local normalization sigma 
    Real64 fSigma              = oConf.getValue("LocalNormSigma", 2.5); 
    // anisotropic correction
    Real64 fAni                 = oConf.getValue("AnisotropicCorrection",1);
    // lambda used for anisotropic correction
    Real64 fLambda              = oConf.getValue("Lambda",1);



    //
    // Create output folder
    // 
    char pCmd[1256];
    sprintf( pCmd, "if [ -d %s ]\n then\n rm -r %s\n fi\n mkdir %s\n", oPath.c_str(), oPath.c_str(), oPath.c_str());
    svt_system( pCmd );

    //
    // Load the atomic coordinates 
    //
    svt_point_cloud_pdb < svt_ga_vec > oPdbTarXtal, oPdbTar, oTemplate;

    //load template
    if (oTemplateFname.size() != 0)
        oTemplate.loadPDB(oTemplateFname.c_str());

    
    svt_ga_vol oMap, oMapCylinder; sprintf( pCmd, "if [ -d %s ]\n then\n rm -r %s\n fi\n mkdir %s\n", oPath.c_str(), oPath.c_str(), oPath.c_str());

    string oSeq, oSsePdb;
    unsigned int iHResCount=0; 

    //
    // Load the comparison structure (correct oligomer)
    //
    if (oPdbTarFname.size() != 0)
    {
        oPdbTarXtal.loadPDB(oPdbTarFname.c_str());
        oPdbTar.loadPDB(oPdbTarFname.c_str());
         
        //blur target stucture if indicated by parameters
        if (bBlurTarget)
        {   
            for (unsigned int iAtom=0; iAtom< oPdbTar.size(); iAtom++)
                oPdbTar.getAtom(iAtom)->setModel( 0 );

            oMap = *oPdbTar.blur(fWidth, fRes);

            char pFname[256];
            sprintf( pFname, "%s/tar.mrc", oPath.c_str());
            oMap.save(pFname);
            
            sprintf( pFname, "%s/tar.pdb", oPath.c_str());
            oPdbTar.writePDB(pFname);

            //Default parameters for synthetic data
            fAcceptMoveRatio     = 0.80;
            fSigma               = 1.5;
        }
    }
 
    //
    // Load the map
    //
    if (!bBlurTarget || oPdbTarFname.size() == 0)
    {
        SVT_GA << "Load Map:" << oMapFname << endl;
        oMap.load(oMapFname.c_str());
    }

    Real64 fOrigZWoAni = oMap.getGridZ();

    char pFname[256];
    sprintf( pFname, "%s/voltrac_target.situs", oPath.c_str());
    oMap.saveSitus( pFname );


    svt_volume<Real64> oGaussian;   
    switch (iFilter)
    {
        case GAUSSIAN:
            oGaussian.createGaussian( fSigma, 1.0 );
            oMap.convolve( oGaussian, true ); 
        break;
        case BILATERAL:
            //oMap.filtrBilateral( fSigma1, fSigma2, fSize, false );    
        break;
        case LOCAL:
            oMap.locallyNormalizeCorrectBorders(1.5, 1.0);
            svt_volume<Real64> oGaussian;
            oGaussian.createGaussian(1.5, 1.0 );
            oMap.convolve( oGaussian, false );
        break;
    }
 
    //process map 
     
     //discard negative values
    oMap.threshold( 0.0 , oMap.getMaxDensity() );

    if (fAni != 1) // some anisotropic correction needed
        oMap.interpolate_map(oMap.getWidth(), oMap.getWidth(), oMap.getWidth()*fAni);

    // read the secondary structure elements prediction
    vector <svt_sse_pdb> oPredHelices;
    if (oSsePredFname.size() != 0)
    {    
        /**
        * erase info from the pdb ; chains, residue no
        */ 
        int iLastRes = -1;
        int iResSeq = 1;
        for (unsigned int iIndex=0; iIndex < oPdbTar.size(); iIndex++)
        {
            oPdbTar.getAtom(iIndex)->setChainID('A');
            if (iIndex<oPdbTar.size()-1 && (oPdbTar.getAtom(iIndex)->getResidueSeq()!=oPdbTar.getAtom(iIndex+1)->getResidueSeq()))
            {
                oPdbTar.getAtom(iIndex)->setResidueSeq( iResSeq );
                iResSeq++;
            }else
               oPdbTar.getAtom(iIndex)->setResidueSeq( iResSeq ); 
    
            
            if (iLastRes != oPdbTar.getAtom(iIndex)->getResidueSeq())
            {
                if (oPdbTar.getAtom(iIndex)->getShortResname()!='-')
                    oSeq += oPdbTar.getAtom(iIndex)->getShortResname();
                oSsePdb += oPdbTar.getAtom(iIndex)->getSecStruct();
                iLastRes =  oPdbTar.getAtom(iIndex)->getResidueSeq();

                if (oPdbTar.getAtom(iIndex)->getSecStruct()=='H' || 
                    oPdbTar.getAtom(iIndex)->getSecStruct()=='I' || 
                    oPdbTar.getAtom(iIndex)->getSecStruct()=='G')
                    iHResCount++;
            }
        }

        cout << oSeq << endl;

        char pSeq[4096], pLine[4096];
        FILE *pSsePredFile = fopen( oSsePredFname.c_str(),"r" );
        
        //read the sequence  and discard
        int i = fscanf(pSsePredFile, "%s", pSeq);
        cout << pSeq << endl;
    
        // read the prediction
        i = fscanf(pSsePredFile, "%s", pLine);
        cout << i << " " << pLine << endl;
        cout << oSsePdb << endl;

        //parse prediction
        unsigned int iLen = 0;
        unsigned int iTPCount = 0, iFPCount=0, iPCount=0;
        svt_sse_pdb oHelix;
        
        char aInitialResname[5];
        char cInitialResChainID;
        int  iInitialResSeq = -1;
        bool bSetInitial = true; // set the info for the initial residue in the sse element  

        for (unsigned int iIndex=0; iIndex < strlen(pLine); iIndex++)
        {
            if (pLine[iIndex]=='H')
            {
                iLen ++;

                if (bSetInitial)
                {
                    sprintf(aInitialResname, "  %c", pSeq[iIndex]);
                    cInitialResChainID = 'A';
                    iInitialResSeq     = iIndex;
                    bSetInitial = false;
                }
                
                //check if assignement is correct
                iPCount++;
                if (iIndex< oSeq.size() && pSeq[iIndex] == oSeq[iIndex])
                {
                    if (oSsePdb[iIndex]=='H' || oSsePdb[iIndex]=='I' || oSsePdb[iIndex]=='G' )
                        iTPCount++;
                    else
                        iFPCount++;
                }
            }
            else
            {   
                if (iLen > 2/*iMinResInHelix*/ ) //already found a helix; then add to list; as the helix is already done
                {
                    strcpy(oHelix.m_aType,  "HELIX ");
                    oHelix.m_iNum = oPredHelices.size() + 1; 
                    sprintf( oHelix.m_aID, "%3d", (int)oPredHelices.size() + 1);

                    strcpy(oHelix.m_aInitialResname, aInitialResname );
                    oHelix.m_cInitialResChainID = cInitialResChainID;
                    oHelix.m_iInitialResSeq = iInitialResSeq;
                    oHelix.m_cInitialICode = ' ';

                    sprintf(aInitialResname, "  %c", pSeq[iIndex]);
                    strcpy(oHelix.m_aTerminalResname,  aInitialResname );
                    oHelix.m_cTerminalResChainID = cInitialResChainID;
                    oHelix.m_iTerminalResSeq = iIndex;
                    oHelix.m_cTerminalICode = ' ';
            
                    oHelix.m_iClass = 1;
                    sprintf(oHelix.m_aComment,  " " );
                    oHelix.m_iLen = iLen;
 
                    oPredHelices.push_back( oHelix );
                }
                iLen = 0;
                bSetInitial = true;
            }       
        }
    
        if (iLen  >= 2/*iMinResInHelix*/ )
        {
            strcpy(oHelix.m_aType,  "HELIX ");
            oHelix.m_iNum = oPredHelices.size() + 1;; 
            sprintf( oHelix.m_aID, "%3d", (int)oPredHelices.size() + 1);

            strcpy(oHelix.m_aInitialResname, aInitialResname );
            oHelix.m_cInitialResChainID = cInitialResChainID;
            oHelix.m_iInitialResSeq = iInitialResSeq;
            oHelix.m_cInitialICode = ' ';

            sprintf(aInitialResname, "  %c", pSeq[strlen(pLine)-1]);
            strcpy(oHelix.m_aTerminalResname,  aInitialResname );
            oHelix.m_cTerminalResChainID = cInitialResChainID;
            oHelix.m_iTerminalResSeq = strlen(pLine)-1;
            oHelix.m_cTerminalICode = ' ';
    
            oHelix.m_iClass = 1;
            sprintf(oHelix.m_aComment,  " " );
            oHelix.m_iLen = iLen;

            oPredHelices.push_back( oHelix );
        }

        SVT_GA << "Number of predicted helices: " <<   oPredHelices.size() << endl;      
        SVT_GA <<  "Tot Pos: " << iHResCount << " TP:" << iTPCount <<  " " << " FN:" << iHResCount-iTPCount << " se=" << iTPCount/(Real64)iHResCount << endl;
        SVT_GA <<  "Tot Neg: " << oSeq.size() - iHResCount << " FP:" << iFPCount << " TN:" <<  oSeq.size() - iHResCount - iFPCount <<  " sp=" << (oSeq.size() - iHResCount - iFPCount)/Real64(oSeq.size() - iHResCount ) <<  endl; 
/*        for (unsigned int iIndex=0; iIndex< oPredHelices.size(); iIndex++ )
        {
            cout << iIndex + 1 << " " << ((svt_helix_pdb *)oPredHelices[iIndex])->m_iLen << endl;
        }
*/
    
        fclose(pSsePredFile);

        oPdbTar.setSecStructCompressedList( oPredHelices );
        oPdbTar.setSecStructSource(SSE_PDB);

        char pFname[256];
        sprintf( pFname, "%s/new_tar.pdb", oPath.c_str());
        oPdbTar.writePDB( pFname );
    }

    //
    // Automatically set some parameters
    //
    if (iMaxGen==0) // nothing set
        iMaxGen = 15000;  
  
    //
    // Create GA
    //
    unsigned int iGenes;
    iGenes = 4; // number of links + x,y,z coordinates for the first point

    // set all parameters
    svt_gacylinder<svt_gacylinder_ind> oGA( iGenes );
    oGA.setOutputPath( oPath.c_str() ); // set path first to be able to output input
    oGA.setReinsertionScheme( eReinsertion );
    oGA.setMaxGen( iMaxGen );  
    oGA.setTarget( oMap );
    oGA.setMinResInHelix( iMinResInHelix );
    if (oPdbTarFname.size() != 0) 
    {
        // use the xtal structure info to compute the axis...
        oGA.setTarStr( oPdbTarXtal ); // the number of helices automatically determined from the target
        oGA.setTarStr( oPdbTar, false );
        if (oPredHelices.size()>0)
            oGA.setNoOfCylinder2Detect( oPredHelices.size() );
    }
    else
        oGA.setNoOfCylinder2Detect( iNoOfTubes );
    oGA.setResolution           ( fRes );
    oGA.setMutationProb         ( fMP );
    oGA.setCrossoverProb        ( fCP );
    oGA.setMutationOffset       ( fMO );
    oGA.setPopSize              ( iPopSize );
    oGA.setSelectivePressure    ( fSP );
    oGA.setAngularStepSize      ( fAngularStepSize );
    oGA.setAngularSearchRange   ( fPsiFrom, fPsiTo, fThetaFrom, fThetaTo, fPhiFrom, fPhiTo );    
    oGA.setTranspositionProb    ( fTranspositionProb );
    oGA.setDistanceThreshold    ( fDistanceThreshold );
    oGA.setDistanceThresholdPenalty( fDistancePenalty );
    oGA.setWriteModelInterval   ( iWriteModelInterval );
    oGA.setMutateAll            ( bMutateAll );
    oGA.setMutateAllProportion  ( fMutateAllProportion );
    oGA.setStopScore            ( fStopScore );
    oGA.setTabuWindowSize       ( iTabuWindowSize );
    oGA.setTabuThreshold        (  fTabuThreshold );
    oGA.setTabuRegionSize       ( fTabuRegionSize );
    oGA.setRefinementMaxMutPerGene( iRefinementMaxMutPerGene );
    oGA.setMaxThread            (iMaxThread);
    oGA.setSyncGen              ( iSyncGen );
    oGA.setApplyBlurring2Model  ( bApplyBlurring2Model );
    oGA.setTemplateRadius       ( fTemplateRadius );
    oGA.setSearchTemplateRadius ( fSearchTemplateRadius );
    oGA.setTemplatePointCount   ( iTemplatePointCount );
    oGA.setTemplateRepeats      ( iTemplateRepeats );
    oGA.setSearchTemplateRepeats( iSearchTemplateRepeats );
    oGA.setDistBetweenRepeats   ( fDistBetweenRepeats );
    oGA.setCrawlingStepSize     ( fCrawlingStep );
    oGA.setAcceptMoveRatio      ( fAcceptMoveRatio );
    oGA.setMaxFailedCrawls      ( iMaxFailedCrawls );
    oGA.setFitHelix             ( bFitHelix );
    oGA.setAniCorr              ( fAni, fOrigZWoAni );
    oGA.setLambda               ( fLambda);

    //
    // Output the settings in a configuration file in the output directory
    //
    char pFnameParam[2560];
    sprintf( pFnameParam, "%s/config.param", oPath.c_str() );
    FILE* pFileParam = fopen( pFnameParam, "w" );
 
    if (oMapFname.size() != 0)
        fprintf( pFileParam, "MapFilename = %s\n",          oMapFname.c_str() );
    if (oPdbTarFname.size() != 0)
        fprintf( pFileParam, "TargetFilename = %s\n",       oPdbTarFname.c_str() );
    if (oSsePredFname.size() != 0)
        fprintf(pFileParam,"SsePredFilename = %s\n",        oSsePredFname.c_str() );
    if (oLogFname.size()!=0)
        fprintf(pFileParam,"Logfile = %s\n",                oLogFname.c_str() );

    fclose(pFileParam);

    oGA.writeConfiguration(pFnameParam);
     
    pFileParam = fopen( pFnameParam, "a" );

    fprintf( pFileParam, "MaxRuns = %d\n",              iMaxRun );
    fprintf( pFileParam, "Filter = %d\n",               iFilter );
    fprintf( pFileParam, "LocalNormSigma = %f\n",       fSigma );
    fprintf( pFileParam, "AnisotropicCorrection=%f\n",  fAni );
    fclose(pFileParam);
 
    //
    // Redirect output
    //
    ofstream oFile;
    streambuf* pBuffer = NULL;
    if (oLogFname.size() != 0)
    {
        char pFnameOut[2560];
        sprintf( pFnameOut, "%s/%s", oPath.c_str(), oLogFname.c_str() );
        oFile.open(pFnameOut);
        pBuffer = cout.rdbuf();
        cout.rdbuf(oFile.rdbuf());
    }

    vector<svt_tube> oTubesInAllRuns, oTubes;
    
    vector<vector <  svt_point_cloud_pdb<svt_ga_vec> > > oModels; 
    //
    // Run the genetic algorithm
    //
    svt_population<svt_gacylinder_ind> oTabuPop, oPop;
    for( int iRun=0; iRun < iMaxRun; iRun++ )
    {
        oGA.setRun(iRun);
        oGA.setMaxThread(iMaxThread);
        oGA.setMaxGen( iMaxGen );
        oGA.setSyncGen( iSyncGen );
        oGA.setDone( false );
        
        //the init population seems to be required before the oGA.execute to create all the variables
        oPop = oGA.execute();

        //for post process 
        oTubes = oGA.getCylinders();
        sort(oTubes.rbegin(), oTubes.rend(), svt_tube::lt_wPenatly);
        //only add 60 percent of the found helices
        //oTubesInAllRuns.insert(oTubesInAllRuns.end(), oTubes.begin(), oTubes.begin()+oTubes.size()*0.6);
        
        // add all 
        oTubesInAllRuns.insert(oTubesInAllRuns.end(), oTubes.begin(), oTubes.end());

        cout << "Added " << oTubes.size() << " helices. Resulting " << oTubesInAllRuns.size() << endl;  
    }
    
    //
    // Close output redirection
    //
    if (oLogFname.size() != 0)
    {
        oFile.close();
        cout.rdbuf(pBuffer);
    }
    
    return 0;
}
