/***************************************************************************
                          main
                          fit multiple domains into map
                          ---------
    begin                : 01/21/2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_gamultifit.h>
#include <svt_config.h>
#include <svt_iostream.h>
#define SVT_GA cout << svt_trimName("svt_ga")

// Define __GAFIT_FORK to activate the process-based parallelization (it seems to be a tad slower then pthreads, but that needs to be tested more and might depend on the platform).
//#define __GAFIT_FORK

#ifdef __GAFIT_FORK
#include <sys/wait.h>
#endif

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
    unsigned int iNoUnits = oConf.getValue("NumberUnits", 2);
    char param[256];

    //
    // Parse the main parameters
    //
    // Map Filename
    string oMapFname           = oConf.getValue("MapFilename", "");
    if (oMapFname.size() == 0)
    {
        SVT_GA << "Error: Please specify a map filename (e.g. MapFilename = test.situs) in your config file!" << endl;
        exit(1);
    }
    // Target PDB structure filename
    string oPdbTarFname        = oConf.getValue("TargetFilename", "");
    // Target PDB structure filename
    string oCoarseTarFname     = oConf.getValue("CoarseTargetFilename", "");
    // Resolution
    Real64 fRes                = oConf.getValue("Resolution", 10.0);
    // Population size
    int iPopSize               = oConf.getValue("PopulationSize", 500);
    // Scoring function
    int eScore                 = oConf.getValue("ScoreFunction", 2);
    // Reinsertion scheme
    int eReinsertion 	       = oConf.getValue("ReinsertionScheme", 1);
    // Mutation probability
    Real64 fMP                 = oConf.getValue("MutationProbability", 0.05);
    // Mutation offset
    Real64 fMO                 = oConf.getValue("MutationOffset", 0.05);
    // Crossover probability
    Real64 fCP                 = oConf.getValue("CrossoverProbability", 0.95);
    // Selective pressure
    Real64 fSP                 = oConf.getValue("SelectivePressure", 1.3);
    // Output path
    string oPath               = oConf.getValue("OutputPath", "");
    if (oPath.size() == 0)
    {
        SVT_GA << "Error: Please specify an output path (e.g. OutputPath = /tmp/test) in your config file!" << endl;
        exit(1);
    }
    // max generations
    int iMaxGen                = oConf.getValue("MaxGenerations", 2500);
    // max runs
    int iMaxRun                = oConf.getValue("MaxRuns", 5);
    // max Threads
    int iMaxThread             = oConf.getValue("MaxThreads", 2);
    // angular step size
    Real64 fAngularStepSize    = oConf.getValue("AngularStepSize", 1.0 );
    // angular search range
    Real64 fPsiFrom            = oConf.getValue("PsiFrom"  , 0.0 );
    Real64 fPsiTo              = oConf.getValue("PsiTo"    , 360.0 );
    Real64 fThetaFrom          = oConf.getValue("ThetaFrom", 0.0 );
    Real64 fThetaTo            = oConf.getValue("ThetaTo"  , 180.0 );
    Real64 fPhiFrom            = oConf.getValue("PhiFrom"  , 0.0 );
    Real64 fPhiTo              = oConf.getValue("PhiTo"    , 360.0 );
    
    //the relative Translational Search Range
    Real64 fRelXFrom           = oConf.getValue("XFrom"  , 0.0 );
    Real64 fRelXTo             = oConf.getValue("XTo"    , 0.0 );
    Real64 fRelYFrom           = oConf.getValue("YFrom"  , 0.0 );
    Real64 fRelYTo             = oConf.getValue("YTo"    , 0.0 );
    Real64 fRelZFrom           = oConf.getValue("ZFrom"  , 0.0 );
    Real64 fRelZTo             = oConf.getValue("ZTo"    , 0.0 );
    
    // transposition probability
    Real64 fTranspositionProb  = oConf.getValue("TranspositionProbability", 0.0);
    // cutoff distance
    Real64 fDistanceThreshold  = oConf.getValue("DistanceThreshold", 10.0/* 0.01875*/);// 0.03/16 - 0.01 - original distance for 1NEK / 16 - number of genes of 1nek
    // cutoff distance penalty
    Real64 fDistancePenalty    = oConf.getValue("DistancePenalty", 0.90);
    // all mutate on / off
    bool bMutateAll            = oConf.getValue("MutateAll", true );
    // all mutate Proportion
    Real64 fMutateAllProportion= oConf.getValue("MutateAllProportion", 1.00 );
    // logfile
    string oLogFname           = oConf.getValue("Logfile", "");
    // how often should the model get exported, every generation (=1) or every 10 (=10) or never (=0)
    int iWriteModelInterval    = oConf.getValue("WriteModelInterval", 0);
    // stopping criterion
    Real64 fStopScore          = oConf.getValue("StopScore", 0.0);
    // how many solutions should be written out?
    unsigned int iWriteSolutions = oConf.getValue("WriteSolutions", 1 );
    // shall only the trace be used for the score calculation?
    bool bOnlyTrace            = oConf.getValue("OnlyTrace", false );
    // shall only the backbone be used for the score calculation?
    bool bOnlyBackBone         = oConf.getValue("OnlyBackbone", false );
    // size of tabu window - see svt_ga.h for details
    unsigned int iTabuWindowSize = oConf.getValue("TabuWindowSize", 30 );
    // tabu threshold - see svt_ga.h for details
    Real64 fTabuThreshold      = oConf.getValue("TabuThreshold", 0.5 );
    // tabu region size - see svt_ga.h for details
    Real64 fTabuRegionSize     = oConf.getValue("TabuRegionSize", 3.0 ); // 0.01/16 - 0.01 - original distance for 1NEK / 16 - number of genes of 1neK
    // sync after how many generations?
    int iSyncGen               = oConf.getValue("ThreadSyncGenerations", 100);
    //niche size
    Real64 fNicheSize 	       = oConf.getValue("NicheSize", 0.0375);
    //maxPopPerNiche - how many individuals allowed in one niche - proportion 0.05 = 5% of the population
    Real64 fMaxPopPerNiche     = oConf.getValue("MapPopPerNiche", 0.05);
    //Penalty for individuals in the same niche 
    Real64 fSameNichePenalty   = oConf.getValue("SameNichePenalty", 0.99);
    //should target structure be blurred : and don't use target volume and blur at resolution indicated: 
    bool bBlurTarget           = oConf.getValue("BlurTargetStruct", false);
    // RefinementMaxMutPerGene
    int iRefinementMaxMutPerGene = oConf.getValue("RefinementMaxMutPerGene", 20);
    //should one used coarse models? 0-all, 1- BB, 2-CA, 3-Codebook Vectors (see iNoAtomsPerCv )
    int iCoarseModelType       = oConf.getValue("CoarseModelType", 3); //coarse model
    //how many atoms should be considered per codebook computed
    int iNoAtomsPerCv          = oConf.getValue("AtomsPerCodebook", 30); //coarse model
    //refine tabus in GA bRestartWithTabus for 500 generations
    bool bRestartWithTabus     = oConf.getValue("RestartWithTabus", true); //refine
    //proportion of individuals copied/mutated from the input structure
    Real64 fPropInputStr       = oConf.getValue("ProportionInputStr", 0.0);
    //should a random rotation be applied to the input units
    bool bApplyRandRot         = oConf.getValue("ApplyRandRotation", false);
    //how many individuals should be created around the feature vectors of the map
    int iPopSizeAroundCenters = oConf.getValue("PopSizeAroundCenters", iPopSize );

    
    //
    // Load the units
    //
    vector<svt_point_cloud_pdb < svt_ga_vec > > oPdbs, oCoarsePdbs;
    svt_point_cloud_pdb < svt_ga_vec > oPdb, oCoarsePdb, oPdbTar, oCoarseTarget, oGenesInRealSpace;
    vector<string> oPdbFnames;
    
    svt_vector4<Real64> oCoa,oVec, oNull,oProd;
    svt_matrix4<Real64> oMat;
    oVec.x(0.0);    
    oVec.y(0.0);
    oVec.z(1.0);
    oNull.x(0.0);
    oNull.y(0.0);
    oNull.z(0.0);
    
     
    for (unsigned int iUnit=1; iUnit<=iNoUnits; iUnit++)
    {
        sprintf( param, "Unit%iFilename", iUnit );
        string oUnitFname = oConf.getValue( param, "" );
        if (oUnitFname.size() == 0)
        {
            SVT_GA << "Error: Please specify the pdb filename of the subunits (e.g. Unit1Filename = unit.pdb) in your config file!" << endl;
            exit(1);
        }
        oPdbFnames.push_back( oUnitFname );
        
        SVT_GA << "Loading PDB from :" << oUnitFname << endl;
        oPdb.loadPDB(oPdbFnames[iUnit-1].c_str());
	
	if (bApplyRandRot)
	{
	    oCoa = oPdb.coa();
	    
	    Real32 fRX = svt_rand()*360.0;
	    Real32 fRY = svt_rand()*180.0;
	    Real32 fRZ = svt_rand()*360.0;
	    	   
	    oMat.loadIdentity();
	    oMat.rotatePTP( deg2rad(fRX), deg2rad(fRY), deg2rad(fRZ) );
	    
	    oPdb = oMat*oPdb;
	    
	    oMat = oMat.invert();
	    
	    oMat.translate( oCoa );
	    
	    
	    oProd = oMat*oNull;
            oGenesInRealSpace.addAtom(*oPdb.getAtom(0), oProd );
	    SVT_GA << "Center : [" << oProd.x() << "," << oProd.y() << "," << oProd.z() << "]" << endl;
            
            oProd = oMat*oVec;
            oGenesInRealSpace.addAtom(*oPdb.getAtom(0), oProd );
	    SVT_GA << "Rotation: [" << oProd.x() << "," << oProd.y() << "," << oProd.z() << "]" << endl;
	}
	
        if (bOnlyTrace)
            oPdbs.push_back( oPdb.getTrace() );
        else if (bOnlyBackBone)
            oPdbs.push_back( oPdb.getBackbone() );
        else
            oPdbs.push_back( oPdb );
	
	    
	switch (iCoarseModelType)
	{
	    case ALL_ATOMS:
		oCoarsePdb = oPdb;
	    break;
	    case BB_ATOMS:
		oCoarsePdb = oPdb.getBackbone();
	    break;
	    case CA_ATOMS:
		oCoarsePdb = oPdb.getTrace();
	    break;
	    case CUSTOMIZE: //cv -1 cv for each 50 atoms
		if (iNoAtomsPerCv > 0)
		{
		unsigned int iNoCv = floor( (Real64)oPdb.size()/(Real64)iNoAtomsPerCv+0.5 );
		svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo; 
		Real64 fEi = 0.1;
		Real64 fEf = 0.001;
		Real64 fLi = 0.2;
		Real64 fLf = 0.02;
		
		unsigned int iMaxStep = 100000;
        
		oClusterAlgo.setLambda( fLi, fLf );
		oClusterAlgo.setEpsilon( fEi, fEf );
		oClusterAlgo.setMaxstep( iMaxStep );
		
		oCoarsePdb = oClusterAlgo.cluster(iNoCv, 8, oPdb);
		for (unsigned int iAtom=0; iAtom < oCoarsePdb.size(); iAtom++)
		    oCoarsePdb.getAtom(iAtom)->setChainID(oPdb.getAtom(0)->getChainID());
		}
		else
		{
		    SVT_GA << " Codebook vectors could not be computed because the AtomsPerCodevook  parameters is <= 0. Using all atoms " << endl;
		    oCoarsePdb = oPdb;
		}
	    break;
	}
	
	oCoarsePdbs.push_back(oCoarsePdb);
    } 
    
    svt_ga_vol oMap, oCoarseMap, oOldMap;
    
    if ( oCoarseTarFname.size() != 0)
       oCoarseTarget.loadPDB( oCoarseTarFname.c_str() );
    
    //
    // Load the comparison structure (correct oligomer)
    //
    if (oPdbTarFname.size() != 0)
    {
        oPdbTar.loadPDB(oPdbTarFname.c_str());
	
	//blur target stucture if indicated by parameters
	Real64 fWidth;
	if (bBlurTarget)
	{
	    if (fRes < 8)
	    {
		fWidth = 2.0;
	    }
	    else
	    {
		if (fRes < 13)
		    fWidth = 3.0;
		else
		    fWidth = 4.0;
	    }
	    SVT_GA << "Blurring structure at resolution " << fRes << " and voxel width " << fWidth << endl;
	    oMap = *oPdbTar.blur(fWidth, fRes);	  
	}
	
        if (bOnlyTrace)
            oPdbTar = oPdbTar.getTrace();
        else if (bOnlyBackBone)
            oPdbTar = oPdbTar.getBackbone();
        else
            oPdbTar = oPdbTar;
    }
    
    //
    // Load the map
    //
    if (!bBlurTarget || oPdbTarFname.size() == 0)
    {
	SVT_GA << "Load Map:" << oMapFname << endl;
    	oMap.loadSitus(oMapFname.c_str());	
    }
    
    if (iCoarseModelType == CUSTOMIZE)
    {
	oOldMap = oMap;
	oMap.interpolate_map( oMap.getWidth() * 2.0 );
    }

    // compute the feature vectors of the map - same number as the no of units
    svt_point_cloud_pdb< svt_vector4<Real64> > oCoarseMapPdb;
    if (iPopSizeAroundCenters > 0 && oPdbs.size() > 0 )
    {
	unsigned int iNoCv = oPdbs.size();
	svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo; 
	Real64 fEi = 0.1;
	Real64 fEf = 0.001;
	Real64 fLi = 0.2;
	Real64 fLf = 0.02;
	
	unsigned int iMaxStep = 100000;

	oClusterAlgo.setLambda( fLi, fLf );
	oClusterAlgo.setEpsilon( fEi, fEf );
	oClusterAlgo.setMaxstep( iMaxStep );
	
	oCoarseMapPdb = oClusterAlgo.cluster(iNoCv, 8, oOldMap);
	
    }
    
    //
    // Create GA
    //
    unsigned int iGenes;
    iGenes = iNoUnits*4;

    // set all parameters
    svt_gamultifit<svt_gamultifit_ind> oGA( iGenes );
    oGA.setReinsertionScheme( eReinsertion );
    oGA.setMaxGen( iMaxGen );  
    oGA.setUnits( oPdbs, oCoarsePdbs );
    oGA.setTarget( oMap );
    if (iPopSizeAroundCenters > 0)
    {
	oGA.setPopSizeAroundCenters( iPopSizeAroundCenters );
        oGA.setCoarseTarget( oCoarseMapPdb ); 
    }
    oGA.setTarStr( oPdbTar );
    if ( oCoarseTarFname.size() != 0) 
	oGA.setCoarseTarget( oCoarseTarget );
    oGA.setResolution( fRes );
    oGA.setScore( (score)eScore );
    oGA.setMutationProb( fMP );
    oGA.setCrossoverProb( fCP );
    oGA.setMutationOffset( fMO );
    oGA.setPopSize( iPopSize );
    oGA.setSelectivePressure( fSP );
    oGA.setOutputPath( oPath.c_str() );
    char pCmd[1256];
    sprintf( pCmd, "if [ -d %s ]\n then\n rm -r %s\n fi\n mkdir %s\n", oPath.c_str(), oPath.c_str(), oPath.c_str());
    svt_system( pCmd );
    oGA.setAngularStepSize( fAngularStepSize );
    oGA.setAngularSearchRange( fPsiFrom, fPsiTo, fThetaFrom, fThetaTo, fPhiFrom, fPhiTo );    
    oGA.setRelativeTranslSearchRange( fRelXFrom, fRelXTo, fRelYFrom, fRelYTo, fRelZFrom, fRelZTo);
    oGA.setTranspositionProb( fTranspositionProb );
    oGA.setDistanceThreshold( fDistanceThreshold );
    oGA.setDistanceThresholdPenalty( fDistancePenalty );
    oGA.setWriteModelInterval( iWriteModelInterval );
    oGA.setMutateAll( bMutateAll );
    oGA.setMutateAllProportion( fMutateAllProportion );
    oGA.setStopScore( fStopScore );
    oGA.setTabuWindowSize( iTabuWindowSize );
    oGA.setTabuThreshold(  fTabuThreshold );
    oGA.setTabuRegionSize( fTabuRegionSize );
    oGA.setNicheSize( fNicheSize );
    oGA.setMaxPopPerNiche( fMaxPopPerNiche );
    oGA.setSameNichePenalty( fSameNichePenalty );
    oGA.setRefinementMaxMutPerGene( iRefinementMaxMutPerGene );
    oGA.setPropInputStr(fPropInputStr);
    oGA.setMaxThread(iMaxThread);
    oGA.setMaxGen( iMaxGen );
    oGA.setSyncGen( iSyncGen );
    
    if (bApplyRandRot)
    {
	char pFname[2560];
	sprintf(pFname, "%s/SolInRealSpace.pdb", oPath.c_str() );
	oGenesInRealSpace.writePDB( pFname ); 
	for (unsigned int iUnit=0; iUnit<iNoUnits; iUnit++)
	{
	    sprintf(pFname, "%s/Unit1_%2d.pdb", oPath.c_str(), iUnit );
	    oPdbs[iUnit].writePDB( pFname ); 
	}
    }
    
    //
    // Output the settings in a configuration file in the output directory
    //
    char pFnameParam[2560];
    sprintf( pFnameParam, "%s/config.param", oPath.c_str() );
    
   /*
     FILE* pFileParam = fopen( pFnameParam, "w" );
    
    fprintf( pFileParam, "NumberUnits = %i\n", iNoUnits );
    for (unsigned int iUnit=1; iUnit<=iNoUnits; iUnit++)
        fprintf( pFileParam, "Unit%iFilename = %s\n", iUnit, oPdbFnames[iUnit-1].c_str() );
    fprintf( pFileParam, "MapFilename = %s\n",              oMapFname.c_str() );
    if (oPdbTarFname.size() != 0)
        fprintf( pFileParam, "TargetFilename = %s\n",       oPdbTarFname.c_str() );
    if ( oCoarseTarFname.size() != 0)
	fprintf( pFileParam, "CoarseTargetFilename = %s\n", oCoarseTarFname.c_str() );
 
    fprintf( pFileParam, "WriteSolutions = %i\n",           iWriteSolutions );
    fprintf( pFileParam, "MaxRuns = %i\n",                  iMaxRun );
    fprintf( pFileParam, "ThreadSyncGenerations = %i\n",    iSyncGen );
    if (oLogFname.size() != 0)
        fprintf( pFileParam, "Logfile = %s\n",              oLogFname.c_str() );
    if (bOnlyTrace)
        fprintf( pFileParam, "OnlyTrace = TRUE\n" );
    else
        fprintf( pFileParam, "OnlyTrace = FALSE\n" );
    if (bOnlyBackBone)
        fprintf( pFileParam, "OnlyBackbone = TRUE\n" );
    else
        fprintf( pFileParam, "OnlyBackbone = FALSE\n" );
    if (bBlurTarget)
        fprintf( pFileParam, "BlurTargetStruct = TRUE\n" );
    else
	fprintf( pFileParam, "BlurTargetStruct = FALSE\n" );
    
    fprintf( pFileParam, "CoarseModelType = %d\n",          iCoarseModelType); 
    fprintf( pFileParam, "AtomsPerCodebook = %d\n",         iNoAtomsPerCv); 
    if (bRestartWithTabus)    
	fprintf( pFileParam, "RestartWithTabus = TRUE\n");
    else
	fprintf( pFileParam, "RestartWithTabus = FALSE\n");
    if (bApplyRandRot)    
 	fprintf( pFileParam, "ApplyRandRotation = TRUE\n");
     else
	fprintf( pFileParam, "ApplyRandRotation = FALSE\n");
    fprintf( pFileParam, "PopSizeAroundCenters = %d\n",          iPopSizeAroundCenters);


    fclose( pFileParam );
   */ 
    oGA.writeConfiguration(pFnameParam);
      
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

    //
    // Run the genetic algorithm
    //
    svt_population<svt_gamultifit_ind> oTabuPop, oPop;
    for( int iRun=0; iRun < iMaxRun; iRun++ )
    {
	oGA.setRun(iRun);
	oGA.setMaxThread(iMaxThread);
	oGA.setMaxGen( iMaxGen );
	oGA.setSyncGen( iSyncGen );
	oGA.setDone( false );
	
	//the init population seems to be required before the oGA.execute to create all the variables
	oGA.initPopulation( oGA.getPopSize() );
	oPop = oGA.execute();
	oTabuPop = oGA.getTabuRegions();
	
	char pFilename[1024];
//	strcpy(pFilename,"Solution");
//	oGA.writeSolutions(oPop, iWriteSolutions, pFilename);
//	
	if (bRestartWithTabus)
	{
		
	    if(iCoarseModelType == CUSTOMIZE)
	    {
		oGA.setUnits( oPdbs, oPdbs );
		oGA.setTarget( oOldMap );
		oGA.setRelativeTranslSearchRange( oMap.getGridX(), oMap.getGridX()+oMap.getSizeX()*oMap.getWidth(), 
		                     		  oMap.getGridY(), oMap.getGridY()+oMap.getSizeY()*oMap.getWidth(), 
						  oMap.getGridZ(), oMap.getGridZ()+oMap.getSizeZ()*oMap.getWidth());
	    }
	    
	    //old settings specific to gamultifit
	    unsigned int iWriteModelInterval 		= oGA.getWriteModelInterval();
	    score eScore 	      		 	= oGA.getScore();

	    int eNewScore = 3; //projectMass - with blurring
	    oGA.setScore( (score)eNewScore );
	    oGA.setWriteModelInterval( 25 );

	    oGA.refine(oTabuPop, 10);

	    oPop = oGA.getPopulation();
	    strcpy(pFilename,"RefinedSolution");
	    oGA.writeSolutions(oPop,  iWriteSolutions, pFilename);
	    
	    oGA.setScore( eScore );
	    oGA.setWriteModelInterval( iWriteModelInterval );
	    if(iCoarseModelType == CUSTOMIZE)
	    {
		oGA.setUnits( oPdbs, oCoarsePdbs );
		oGA.setTarget( oMap );
	    }
	}
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

