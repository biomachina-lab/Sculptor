    #include <stdlib.h>
    #include <svt_iostream.h>
    #include <svt_point_cloud_pdb.h>
    #include <svt_vector4.h>
    #include <svt_basics.h>
    #include <svt_types.h>
    #include <svt_point_cloud_atom.h>
    #include <svt_clustering_trn.h>
    #include <svt_flexing_inverse_dist_weighting.h>
    #include <svt_flexing_elastic_body_spline.h>
    #include <svt_flexing_thin_plate_spline.h>
    
    using namespace std;
    
    #define FLEX cout << svt_trimName("flexing")
    /**
    * Extract the xyz coordinates from the codebook containing the pdb file  
    */
    void setCoordinates(vector<svt_vector4<Real64> >         &oVector, 
                        svt_point_cloud<svt_vector4<Real64> > &oPDBInitial,
                        svt_point_cloud<svt_vector4<Real64> > &oPDBFinal,
                        Real64 fPercent,bool bUseCos = false)
    
    {
        unsigned int iIndex;
        
        oVector.clear();
        
        if (bUseCos)
        {
            Real64 fCoeff = (1-cos(fPercent*PI))/2.0; 
            FLEX << fCoeff << endl;
                    
            for (iIndex = 0; iIndex<oPDBInitial.size(); iIndex++)
            {
                oVector.push_back(oPDBInitial[iIndex]*(1 - fCoeff)  + oPDBFinal[iIndex]*fCoeff);
            }
        }
        else
        {
            for (iIndex = 0; iIndex<oPDBInitial.size(); iIndex++)
            {
                oVector.push_back(fPercent*(oPDBFinal[iIndex] - oPDBInitial[iIndex]) + oPDBInitial[iIndex]);
            }
        }
        
    
    }
    
    
    
    int main(int argc, char** argv){ 
        
        ///////////////////////////////////////////////////////////////////////////
        // Parsing options
        ///////////////////////////////////////////////////////////////////////////
    
        if (argc < 4)
        {
            cout << endl << "usage: flexing [options] <input pdb file> <orig conf cdbk pdb file> <flexed conf cdbk pdb file> <output pdb file>" << endl;
            cout << endl;
            cout << "required pdb files:" << endl;
            cout << " <input pdb file>            - the original atomic conformation" << endl;
            cout << " <orig conf cdbk pdb file>   - codebook vectors computed for the original configuration"  << endl;
            cout << " <flexed conf cdbk pdb file> - codebook vectors computed for the flexed configuration" << endl;
            cout << " <output pdb file>           - resulting flexed conformation" << endl;
            cout << endl;
            cout << "options:" << endl;
            cout << " -w <weighting method>         ([p[ure]/H[ybrid]]" << endl;
            cout << " -p <weighting power value>    (default: 4; other: 2)" << endl;
            cout << " -d <distance type>            (default: cartesian; other: graph)" << endl;
            cout << " -s <distance in graph schema> (default: 0 - min; other 1 - weighted average)" << endl;
            cout << " -e <elasticity>               (default:0.01 - soft rubber; range: 0.01-0.49) Considered only in Elastic Body Spline" << endl;
            cout << " -b <basis funtion type>       (TPS: 1:r; 2:r^2*log(r^2)(default); 3:r^3  EBS: 1 ; 2: variant (better results)(default)" << endl;
            cout << " -f <psf filename>             (for the connections between cdbk in original conf) " << endl;
            cout << " -cp <MD filename>             (for eventual comparison with MD results)" << endl;
            cout << " -i <use idealization>         ([y/N])" << endl;
            cout << " -n <frames number>            (default: 1)" << endl; 
            cout << " -ci <codebook count>" << endl;
            cout << "                The number of codebook vectors to be used when interpolating." << endl;
            cout << "                Only the closest \"codebook count\" vectors will be used."  << endl;
            cout << "                Default: Use all codebooks" << endl;
            return 0;
        }
    
        char* pPDBOrigConf_Fname            = NULL;
        char* pPDBOrigConfCdbk_Fname        = NULL;
        char* pPDBFinalConfCdbk_Fname       = NULL;
        char* pPDBFinalConf_Fname           = NULL;
        char* pPSF_Fname                    = NULL;
        char* pPDBMD_Fname                  = NULL;
        string oWeightingMethod             = "Hybrid"; // use the distance to the most distant codebook vector
        unsigned int iWeightingPower        = 4;
        unsigned int iDistInGraphSchema     = 1;
        unsigned int iBasisFunctionType     = 2;
        Real64 fElasticity                  = 0.01;
        string pDistType                    = "cartesian";
        bool bApplyIdealization             = false;
        unsigned int iFramesNumber          = 1;
        unsigned int iClosestCdbkUsedCount  = 0;
                    
        // parse command-line
        for(int i=1; i<argc; i++)
        {
            // is this an option or a filename
            if (argv[i][0] == '-' && argv[i][1] != 0)
            {
                switch(argv[i][1])
                {
                    // Lambda parameters
                    case 'w':
                        if (argc >= i+1)
                        {
                            if (toupper(argv[i+1][0])!='H')
                            {
                                oWeightingMethod = argv[i+1];
                                FLEX << "Weighting method " << argv[i+1] << endl;
                            }
                            else
                            {
                                oWeightingMethod = "Hybrid";
                                FLEX << "Warning: Weighting method set to Hybrid" << endl;
                            }
                            i++;
                        }
                        break;
                    case 'p':
                        if (argc >= i+1)
                        {
                            
                            iWeightingPower = atoi(argv[i+1]);
                            FLEX << "Weighting Power " << iWeightingPower << endl;
                            i++;
                        }
                        break;
                    case 'f':
                        if (argc >= i+1)
                        {
                            pPSF_Fname = argv[i+1];
                            FLEX << "Psf File:" << pPSF_Fname << endl;
                            i++;
                        }
                        break;    
                    case 's':
                        if (argc >= i+1)
                        {
                            iDistInGraphSchema = atoi(argv[i+1]);
                            FLEX << "Schema for the Distance to the graph:" << iDistInGraphSchema << endl;
                            i++;
                        }
                        break;    
                    case 'd':
                        if (argc >= i+1)
                        {
                            pDistType = argv[i+1];
                            FLEX << "Distance type: \"" << argv[i+1] << "\"" << endl;
                            i++;
                        }
                        break;    
                    case 'c':
                        if (argv[i][2]=='p' && argc >= i+1)
                        {
                            pPDBMD_Fname = argv[i+1];
                            FLEX << "MD structure used for comparison: " << pPDBMD_Fname << endl;
                            i++;
                        }
                        
                        if (argv[i][2]=='i' && argc >= i+1)
                        {
                            iClosestCdbkUsedCount = atoi(argv[i+1]);
                            FLEX << "Use the Closest " << iClosestCdbkUsedCount << " codebook vectors " << endl;
                            i++;
                        }
                        break;    
                    case 'e':
                        if (argc >= i+1)
                        {
                            fElasticity = atof(argv[i+1]);
                            FLEX << "Elasticity coef: " << fElasticity << endl;
                            i++;
                        }
                        break;    
                    case 'b':
                        if (argc >= i+1)
                        {
                            iBasisFunctionType = atoi(argv[i+1]);
                            FLEX << "Basis function type: " << iBasisFunctionType << endl;
                            i++;
                        }
                        break;        
                    case 'i':
                        if (argc >= i+1)
                        {
                            if ((toupper(argv[i+1][0]))=='Y')
                            {
                                bApplyIdealization = true;
                                FLEX << "Appling idealization" << endl;
                            }
                            else
                            {
                                bApplyIdealization = false;
                                FLEX << "NOT appling idealization" << endl;
                            } 
                            
                            i++;
                        }
                        break;        
                    case 'n':
                        if (argc >= i+1)
                        {
                            iFramesNumber = atoi(argv[i+1]);
                            FLEX << "Number of frames: " << iFramesNumber << endl;
                            i++;
                        }
                        break;        
                        
                }
            } else {
    
                if (pPDBOrigConf_Fname == NULL)
                {
                    pPDBOrigConf_Fname = argv[i];
    
                    FLEX << "Original conformation: " << pPDBOrigConf_Fname << endl;
    
                } else if (pPDBOrigConfCdbk_Fname == NULL)
                {
                    pPDBOrigConfCdbk_Fname = argv[i];
    
                    FLEX << "Original conformation codebook vector " << pPDBOrigConfCdbk_Fname << endl;
    
                } else if (pPDBFinalConfCdbk_Fname == NULL)
                {
                pPDBFinalConfCdbk_Fname = argv[i];
    
                FLEX << "Final conformation codebook vector " << pPDBFinalConfCdbk_Fname << endl;
                } else {
                    
                    pPDBFinalConf_Fname = argv[i];
                    
                    FLEX << "Output file flexed conformation" << pPDBFinalConf_Fname << endl;
                }
            }
        }
    
        
    svt_point_cloud_pdb< svt_vector4<Real64> > oOrigConf;
    svt_point_cloud_pdb< svt_vector4<Real64> > oOrigConfCodebooks;
    svt_point_cloud_pdb< svt_vector4<Real64> > oFinalConfCodebooks;
    
    //Load data files  
    oOrigConf.loadPDB(pPDBOrigConf_Fname);
    
    oOrigConfCodebooks.loadPDB(pPDBOrigConfCdbk_Fname);
    if (iClosestCdbkUsedCount==0) iClosestCdbkUsedCount = oOrigConfCodebooks.size();
    if (pPSF_Fname != NULL) oOrigConfCodebooks.loadPSF(pPSF_Fname);
    oFinalConfCodebooks.loadPDB(pPDBFinalConfCdbk_Fname);
    
    //filenames
    char pFname[1256];
    char pTemp[1256];
    char oMovieName[1256];
    char oCodebookMovieName[1256];
    strcpy(pFname, pPDBFinalConf_Fname);
    svt_stripExtension(pFname, pTemp);
    if (iFramesNumber==1)
            sprintf(oMovieName, "%s_IDW.pdb", pTemp);
        else
            sprintf(oMovieName, "%s_movie_IDW.pdb", pTemp);
        
    sprintf(oCodebookMovieName, "%s_codebook_movie_IDW.pdb"   , pTemp);
    sprintf(pFname            , "%s_IDW.pdb"                  , pTemp);
    
    
    
    vector<svt_vector4<Real64> >                  oVecInitialConfCodebook;
    vector<svt_vector4<Real64> >                  oVecFinalConfCodebook;
    svt_point_cloud_pdb<svt_vector4<Real64> >     oFinalConformation, oFinalConformationByIDW;
    svt_point_cloud_pdb< svt_vector4<Real64> >    oIntermediateStructure;
    Real64                                        fStepPer100 = 1.0/iFramesNumber; 
    unsigned int                                  iIndex;  
    FILE*                                         pFile;
    bool                                          bAppend = true;
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    // 
    //     Inverse Distance Interpolation 
    // 
    /////////////////////////////////////////////////////////////////////////////////////////////
    
    svt_flexing_inverse_dist_weighting oInterpolIDW(oOrigConf,oOrigConfCodebooks,oFinalConfCodebooks,false,"Cartesian","none","Hybrid",iWeightingPower); 
    //parametrization - readed options
    oInterpolIDW.setWeightingMethod(oWeightingMethod);
    oInterpolIDW.setWeightPow(iWeightingPower);
    oInterpolIDW.setDistType(pDistType);
    oInterpolIDW.setNbCdbksOfInfluence(iClosestCdbkUsedCount-1);
    /*
    //Not working yet 
        oInterpolIDW.setDistInGraphSchema(iDistInGraphSchema);
    */
    
    
    if (iFramesNumber>1)
    {
            oOrigConf.writePDB(oMovieName);
            oOrigConfCodebooks.writePDB(oCodebookMovieName);
            bAppend = true;
    }
    else
        bAppend = false;
    
    for (iIndex=0; iIndex < iFramesNumber; iIndex++)
    {
        //Step 1: get the initial and final coordinates of the codebook
            setCoordinates(oVecInitialConfCodebook, oOrigConfCodebooks, oFinalConfCodebooks, fStepPer100*iIndex);
            setCoordinates(oVecFinalConfCodebook  , oOrigConfCodebooks, oFinalConfCodebooks, fStepPer100*(iIndex+1));
            //Step2: set the new computed codebook.
            oInterpolIDW.setCodebooks(oVecInitialConfCodebook, oVecFinalConfCodebook);
            //Step3: Interpolate
            oFinalConformationByIDW = oInterpolIDW.interpolate();
    
            oFinalConformationByIDW.writePDB(oMovieName, bAppend);
            oInterpolIDW.setOrigConf(oFinalConformationByIDW);
    }
    FLEX << "Interpolation using IDW finished. Result printed in " << pFname << endl; 
    
        
    //here refmac
    oFinalConformation = oFinalConformationByIDW;
    if (bApplyIdealization)
    {
        char pCmdLn[1256];
        char pFname_refmac_in[1256];
        char pFname_refmac_out[1256];
        char pRefmacRunFN[1256];
        char oCrystalParamString[1256];
        
        sprintf(pFname_refmac_in, "%s_struct_refmac_in.pdb", pTemp);
        sprintf(pFname_refmac_out, "%s_struct_refmac_out.pdb", pTemp);
        sprintf(pRefmacRunFN,"%s_refmac.run", pTemp);
        
        pFile = fopen(pRefmacRunFN,"w");
        fprintf(pFile,"#!/bin/csh -f\n");
        sprintf(pCmdLn,"set crdin=%s\nset crdout=%s\n", pFname_refmac_in, pFname_refmac_out);
        fprintf(pFile,pCmdLn);
        fprintf(pFile,"refmac:\n/mnt/biomachina/home/mrusu/Programs/refmac/refmac_linintel \\\nXYZIN   $crdin \\\nXYZOUT $crdout \\\n<< eor\n");
        fprintf(pFile," MAKE HYDR Y\nMAKE CHECK NONE\nREFI TYPE IDEAlise\nNCYC 20\nMONI MEDI\nend\neor\nif ($status) exit\n");
        fclose(pFile);
        
        //crystal parameters 
        Real64 fXExpand = oFinalConformation.getMaxXCoord()-oFinalConformation.getMinXCoord();
        Real64 fYExpand = oFinalConformation.getMaxYCoord()-oFinalConformation.getMinYCoord();
        Real64 fZExpand = oFinalConformation.getMaxZCoord()-oFinalConformation.getMinZCoord();
    
        sprintf(oCrystalParamString,"CRYST1%9.3f%9.3f%9.3f  90.00  90.00  90.00 P 1                     \n", fXExpand, fYExpand, fZExpand);
        sprintf(oCrystalParamString,"%sSCALE1      %8.6f  0.000000  0.000000        0.00000                         \n", oCrystalParamString, 1/fXExpand);
        sprintf(oCrystalParamString,"%sSCALE2      0.000000  %8.6f  0.000000        0.00000                         \n", oCrystalParamString, 1/fYExpand);
        sprintf(oCrystalParamString,"%sSCALE3      0.000000  0.000000  %8.6f        0.00000                         \n", oCrystalParamString, 1/fZExpand);
    
        //write file on disk for refmac to optimize
        pFile = fopen(pFname_refmac_in, "w");
        fprintf(pFile, oCrystalParamString);
        fclose(pFile);
        oFinalConformation.writePDB(pFname_refmac_in, true); 
        
        sprintf(pCmdLn,"chmod 755 %s; %s", pRefmacRunFN, pRefmacRunFN );
        svt_system(pCmdLn);
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    // 
    //     Elastic Body Spline
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////// 
        svt_flexing_elastic_body_spline oInterpolEBS(oOrigConf,oOrigConfCodebooks,oFinalConfCodebooks,false,"none","none",iBasisFunctionType,fElasticity);
    
    oInterpolEBS.setElasticity(fElasticity);
    oInterpolEBS.setBasisFunctionType(iBasisFunctionType);
    
    svt_point_cloud_pdb<svt_vector4<Real64> > & oFinalConformationByEBS  = oInterpolEBS.interpolate();
    sprintf(pFname, "%s_EBS.pdb", pTemp);
    oFinalConformationByEBS.writePDB(pFname);
    FLEX << "Interpolation using EBS finished. Result printed in " << pFname << endl; 
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    // 
    //     Thin Plate Spline
    // 
    /////////////////////////////////////////////////////////////////////////////////////////////
    svt_flexing_thin_plate_spline oInterpolTPS(oOrigConf,oOrigConfCodebooks,oFinalConfCodebooks,false,"Cartesian","none",iBasisFunctionType);
    
    oInterpolTPS.setDistType(pDistType);
    oInterpolTPS.setBasisFunctionType(iBasisFunctionType);
    
    
    svt_point_cloud_pdb<svt_vector4<Real64> > & oFinalConformationByTPS  = oInterpolTPS.interpolate();
    sprintf(pFname, "%s_TPS.pdb", pTemp);
    oFinalConformationByTPS.writePDB(pFname);
    FLEX << "Interpolation using TPS finished. Result printed in " << pFname << endl ; 
    

    
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    //
    //    compaire with MD Data if file given 
    //
    /////////////////////////////////////////////////////////////////////////////////////////////
    if (pPDBMD_Fname != NULL){
        svt_point_cloud_pdb<svt_vector4<Real64> > oMDFlexedStructure;
        oMDFlexedStructure.loadPDB(pPDBMD_Fname);
        
        FLEX << "***********************************************" << endl;
        FLEX << "******* Inverse Distance Interpolation ********" << endl; 
        FLEX << "***********************************************" << endl; 
        FLEX << "RMSD for all the atoms to MD:  " << oFinalConformationByIDW.rmsd(oMDFlexedStructure,true)                 << endl;
        FLEX << "RMSD on the backbone to MD:    " << oFinalConformationByIDW.rmsd(oMDFlexedStructure,true, BACKBONE)        << endl;
        FLEX << "RMSD on the trace to MD:       " << oFinalConformationByIDW.rmsd(oMDFlexedStructure,true, TRACE)           << endl;
        
        FLEX << "***********************************************" << endl;
        FLEX << "*******     Elastic Body Spline        ********" << endl;
        FLEX << "***********************************************" << endl;
        FLEX << "RMSD for all the atoms to MD:  " << oFinalConformationByEBS.rmsd(oMDFlexedStructure,true)            << endl;
        FLEX << "RMSD on the backbone to MD:    " << oFinalConformationByEBS.rmsd(oMDFlexedStructure,true, BACKBONE)   << endl;
        FLEX << "RMSD on the trace to MD:       " << oFinalConformationByEBS.rmsd(oMDFlexedStructure,true, TRACE)      << endl;
        
        FLEX << "***********************************************" << endl;
        FLEX << "*******     Thin Plate Spline          ********" << endl ;
        FLEX << "***********************************************" << endl;
        FLEX << "RMSD for all the atoms to MD:  " << oFinalConformationByTPS.rmsd(oMDFlexedStructure, true)           << endl;
        FLEX << "RMSD on the backbone to MD:    " << oFinalConformationByTPS.rmsd(oMDFlexedStructure,true, BACKBONE)   << endl;
        FLEX << "RMSD on the trace to MD:       " << oFinalConformationByTPS.rmsd(oMDFlexedStructure,true, TRACE)      << endl;
        FLEX << "***********************************************" << endl;
        FLEX << "***********************************************" << endl;
        
        
    }
    
    return 1;
    }
