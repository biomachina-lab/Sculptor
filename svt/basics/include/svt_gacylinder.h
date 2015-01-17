/***************************************************************************
                          svt_gacylinder
                          fit multiple domains into map
                          ---------
    begin                : 01/21/2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_GA_DK
#define __SVT_GA_DK

// svt classes
#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_pdb.h>
#include <svt_clustering_trn.h>
#include <svt_volume.h>
#include <svt_gamultifit_ind.h> // for the angles
#include <svt_ga.h>
#include <svt_stlVector.h>
#include <svt_tube.h>
#include <svt_string.h>
#include <iomanip>
// clib
#include <math.h>

/**
 * GA-based fitting algorithm
 * \author Mirabela Rusu
 */
template<class T> class svt_gacylinder : public svt_ga<T>
{
protected:
    //the template of the cylinder
    svt_point_cloud_pdb<svt_ga_vec> m_oTemplate, m_oBigTemplate, m_oTurn, m_oBigTurn, m_oCircle1, m_oCircle2, m_oCircle3;

    //target volume
    svt_ga_vol m_oTar;
   
    //the target structure for validation purposes only
    svt_point_cloud_pdb<svt_ga_vec> m_oTarStr;

    // the axis of the target structure
    svt_point_cloud_pdb< svt_ga_vec > m_oTarStrAxes;

    //the target structure for validation purposes only
    svt_point_cloud_pdb<svt_ga_vec> m_oTarStrCaInHelix;

    // a pdb that contains only the coa of the atoms in each axis
    svt_point_cloud_pdb< svt_ga_vec > m_oTarStrAxesCenters;

    // holds information about the helices; i.e. length
    vector< vector< Real64 > > m_oTarStrAxesInfo;

    // a volume representing the blurred axes
    svt_ga_vol m_oTarStrAxesVol;
    
    //the target structure for validation purposes only
    svt_point_cloud_pdb<svt_ga_vec> m_oCoarseTarget;
    
    //the model
    svt_point_cloud_pdb<svt_ga_vec> m_oModel;

    //model volume
    svt_ga_vol m_oModelVol;

    //the coordinate of the 0,0,0 voxel
    Real64 m_fOrigX, m_fOrigY, m_fOrigZ;
    
    //no of voxels on x,y, and z
    unsigned int m_iSizeX, m_iSizeY, m_iSizeZ;
    
    // voxel width
    Real64 m_fWidth;
    
    //the resolution
    Real64 m_fRes;
    
    // the size of the search spacetransClear()
    svt_ga_vec m_oSearchSpace;
    
   // the scoring function to be used
    //score m_eScore;
    
    //the kernel for the blurring of the model
    svt_ga_vol m_oKernel;

    // set of non-degenerate euler angles
    static svt_eulerAngles m_oAngles;

    // the angular step size
    Real64 m_fDelta;

    // the angular search range
    Real64 m_fPsiFrom;
    Real64 m_fPsiTo;
    Real64 m_fThetaFrom;
    Real64 m_fThetaTo;
    Real64 m_fPhiFrom;
    Real64 m_fPhiTo;
    
    
    bool m_bSetTranslSearchRange;
    // the translational search range
    Real64 m_fXFrom;
    Real64 m_fXTo;
    Real64 m_fYFrom;
    Real64 m_fYTo;
    Real64 m_fZFrom;
    Real64 m_fZTo;


    // result output
    char m_pPath[1256];

    // how many generations should gacylinder wait until it outputs a new model file (0 turns output off)
    unsigned int m_iWriteModelInterval;
    
    //SpringPotential
    Real64 m_fSpringPotential;

    //the max distance deviation between two points: dist(p2-p1) in state1 - dist(p2-p1) in state2
    Real64 m_fMaxDistDev;

    //how much should be disregarded on each side 
    Real64 m_fBorder;
    
    //mask durring the correlation
    bool m_bMask;    

    //value between 0-1; default 0.95 that indicates that a crawl is accepted if the score is larger that m_fAcceptMovePercent * original score
    Real64 m_fAcceptMoveRatio;

    //Number of "failed" (score < m_fAcceptMovePercent * original score) crawl steps allowed before stopping the crawl
    unsigned int m_iMaxFailedCrawls;

    //a cylinder
    vector <svt_tube> m_oCylinders;

    //a cylinder
    vector <  svt_point_cloud_pdb<svt_ga_vec> > m_oCylindersPdb;

    //pdb of the axis with the anisotromic decompression applied
    vector <  svt_point_cloud_pdb<svt_ga_vec> > m_oCylindersPdbAniDecorr;


    //any cylinders were already identified
    bool m_bCanOutputResults;

    //is refining now
    bool m_bRefining;

    //created durring the crawling; added only when the crawling was completely done
    svt_population<T> m_oTabusInCrawl;

    //was a "valid" cylinder found in this run?
    bool m_bFoundCylinder;

    // number of helices to detect
    unsigned int m_iNoOfCylinder2Detect;

    // apply blurring to the model
    bool m_bApplyBlurring2Model;

    // the radius of the expansion template in A
    Real64 m_fTemplateRadius;

    // the radius of the ga search template in A
    Real64 m_fSearchTemplateRadius;

    // the number of points in the template ( m_iTemplatePointCount - 1 points in the circle and 1 at the center  )
    unsigned int m_iTemplatePointCount;

    // number of times the circle is repeated in the expansion template
    int m_iTemplateRepeats;

    // number of times the circle is repeated in the search template
    int m_iSearchTemplateRepeats;

    // distance between repeats
    Real64 m_fDistBetweenRepeats;

    // distance covered in one crawling step
    Real64 m_fCrawlingStepSize;

    // the number of steps per turn
    Real64 m_fStepsPerTurn;

    //explored volume
    svt_ga_vol m_oExploredVol;
   
    //the amount ot the explored volume
    Real64 m_fExploredPercent;

    //an idealized hardcodded ala helix - created using modeller
    svt_point_cloud_pdb<svt_ga_vec> m_oIdealHelix;

    //should a helix be fitted on the axis
    bool m_bFitHelix;

    //
    svt_point_cloud_pdb<svt_ga_vec> m_oHelices;

    //the maximum distanced to consider 2 atoms / tubes as corresponding/mapped to each other
    Real64 m_fMaxDist4Map;

    //the rmsd output to print in outputResults; is computed in discardCylindres
    char m_pRmsdOut[1024];

    // the number of individuals created in the initInd; helps figuring if the space is full with tabu regions and the search should be stopped
    int m_iNoIndInInit;

    // min number of residues in helix 
    int m_iMinResInHelix;

    //the average score of all solutions
    Real64 m_fAvgScoreAll; 

    // Anisotric correction related terms:
    Real64 m_fAni;
    Real64 m_fOrigZWoAni;

    // the correction Lamdba - see update fitness
    Real64 m_fLambda;

    // are there any new results; variable to be used for GUI update 
    bool m_bNewResults;
public:

    /**
     * Constructor
     */
    svt_gacylinder(unsigned int iGenes);
    
    /**
     * Destructor
     */
    virtual ~svt_gacylinder();
  
    /**
     * create a new individual
     */
    virtual T initIndividual();
  
    /**
     * generate initial population
     * \param iNum number of inds in this population
     */
    virtual void initPopulation(int iNum, bool bOutputInput = false);

    /**
     * initialize the angles
     */
    void initAngles();
 
    /**
     * Create an object 
     */
    virtual svt_gacylinder<T>* createObject();


    /**
     * create the volume template
     */
    void initTemplate();

    /**
     * add helices found in threads to the global list of helices; discard the one that overlap
     * \param bFromThreads should only the helices from threads should be looked at? or all helices
     * \return the average length of the added helices
     */
    Real64 discardCylinders( bool bFromThreads = true );
  
    /**
     * discard points from the cylinders according to their score // use here the average score of all accepted points in crawling
     */
    void discardPointsoOnCylinders();
   
    /**
     * delete all helices
     */
    void  clearCylinders();
 
    /**
     * post process tubes
     */
    void postProcessTubes();  
    /**
     * sort the cylinders and fills the Cylinder pdb
     */
    void sortCylinders();

    /**
     * function to create the thread 
     * \return a population
     */
    virtual svt_population<T> execute();


    //
    // Parameters
    //

    /**
     * Set resolution of the target map
     * \param fRes the resolution of the target map
     */
    void setResolution(Real64 fRes);
    
    /**
     * Get resolution of the target map
     * \return the resolution of the target map
     */
    Real64 getResolution();

    /**
     * Set the masking when computing the correlation
     * \param bMask bool indicating whether to mask
     */
    void setMask(bool bMask);
 
    /**
     * Set the angular step size. In general: The smaller the value, the better the accuracy. In contrast to an exhaustive search, the runtime will also not be longer, if a finer step size
     * is chosen. The only limitation is the memory - as the table needs to be stored, very low numbers might result in an excessive use of memory. Recommended: ~0.5 to 1.0.
     * \param fDelta the angular step size (default 0.5)
     */
    void setAngularStepSize( Real64 fDelta );
    /**
     * Get the angular step size. In general: The smaller the value, the better the accuracy. In contrast to an exhaustive search, the runtime will also not be longer, if a finer step size
     * is chosen. The only limitation is the memory - as the table needs to be stored, very low numbers might result in an excessive use of memory. Recommended: ~0.5 to 1.0.
     * \return the angular step size (default 0.5)
     */
    Real64 getAngularStepSize( );
    
    /**
     * Returns the number of angles 
     */
    Real64 getAnglesCount();
    
    /**
     * Get angles
     */
    svt_vector4<Real64>& getAngle(long unsigned int iIndex);

    /**
     * Set the ranges for the angular search.
     * \param fPsiFrom   lower limit of the psi angles
     * \param fPsiTo     upper limit of the psi angles
     * \param fThetaFrom lower limit of the theta angles
     * \param fThetaTo   upper limit of the theta angles
     * \param fPhiFrom   lower limit of the phi angles
     * \param fPhiTo     upper limit of the phi angles
     */
    void setAngularSearchRange( Real64 fPsiFrom, Real64 fPsiTo, Real64 fThetaFrom, Real64 fThetaTo, Real64 fPhiFrom, Real64 fPhiTo );
    
    /**
     * Set the ranges for the translational search relative to the centers of the units 
     * (if fXFrom = -20 and fXTo = 20 the units moves -20 and 20 A from the current position)
     * \param fXFrom     upper limit of the X
     * \param fXTo       lower limit of the X
     * \param fYFrom     upper limit of the Y
     * \param fYTo       lower limit of the Y
     * \param fZFrom     upper limit of the Z
     * \param fZTo       lower limit of the Z
     */
    void setRelativeTranslSearchRange( Real64 fXFrom, Real64 fXTo, Real64 fYFrom, Real64 fYTo, Real64 fZFrom, Real64 fZTo );

    //
    // Units, Maps, PDB files...
    //
    /**
     * set target
     * \param oTar the map
     */
    void setTarget( svt_ga_vol &oTar);
    
    /**
     * set target str
     * only for validation purposes 
     * \param oTarStr the target structure
     */
    void setTarStr( svt_point_cloud_pdb< svt_ga_vec >  &oTarStr, bool bComputeAxis = true);
    
    /**
     * set the tempate of the cylinder
     * \param oPdb the target structure
     */
    void setTemplate( svt_point_cloud_pdb< svt_ga_vec >  &oPdb);
    
    /**
     * set coarse target
     * \param oCoarseTarget the coarse version of the target structure
     */
    void setCoarseTarget( svt_point_cloud_pdb< svt_ga_vec >  &oCoarseTarget);
  
    /**
     * Get model
     * \return the model that was last generated by calcTransformation
     */
    inline svt_point_cloud_pdb<svt_ga_vec >&  getModel(){ return m_oModel;}
    
    /**
     * Get model volume
     * \return the volume of the model that was last generated by calcTransformation
     */
    inline svt_ga_vol&  getModelVol(){return m_oModelVol;}

    /**
     * set the accept Move score percentage:e.g. 0.95 means that moves are accepted if withing 0.95 of the original score 
     * \param the score percentage
     */
    void setAcceptMoveRatio(Real64 fAcceptMoveRatio);
    
    /**
     * get the accept Move score percentage: e.g. 0.95 means that moves are accepted if withing 0.95 of the original score 
     * \return the score percentage
     */
    Real64 getAcceptMoveRatio();

    /**
     * set the max number of failed Crawls before stoping the search
     * \param iMaxFailedCrawls e.g. 2 indicates 2 times tried before stoping the crawl
     */
    void setMaxFailedCrawls( unsigned int iMaxFailedCrawls);
    
    /**
     * get the max number of failed Crawls before stoping the search
     * \return iMaxFailedCrawls e.g. 2 indicates 2 times tried before stoping the crawl
     */
    unsigned int getMaxFailedCrawls();

    /**
     * how many Cylinders shouls be searched for 
     */ 
    void setNoOfCylinder2Detect( int iNoOfCylinder2Detect );    

    /**
     * how many Cylinders shouls be searched for 
     */ 
    int getNoOfCylinder2Detect( );
 
    /**
     * was "valid" cylinder found
     */
    void setFoundCylinder( bool bFoundCylinder);
   
    /**
     *  was "valid" cylinder found
     */
    bool wasFoundCylinder();

    /**
     *  set apply Blurring to model  
     * \param state
     */
     void setApplyBlurring2Model( bool bBlurring);

    /**
     * set the radius of the template 
     * \param the radius of the template
     */
    void setTemplateRadius( Real64 fTemplateRadius );

    /**
     * get the radius of the template 
     * \param the radius of the template
     */
    Real64 getTemplateRadius();

    /**
     * set the radius of the template used for the ga Search
     * \param the radius of the template
     */
    void setSearchTemplateRadius( Real64 fSearchTemplateRadius );

    /**
     * get the radius of the template used for the ga Search
     * \param the radius of the template
     */
    Real64 getSearchTemplateRadius();
    
    /**
     * set the number of points in one of the circles of the template
     * \param  number of points in one of the circles of the template
     */
    void setTemplatePointCount( unsigned int iTemplatePointCount );

    /**
     * get the number of points in one of the circles of the template
     * \param  number of points in one of the circles of the template
     */
    unsigned int getTemplatePointCount();

    /**
     * set the number of circles copied in the expansion template
     * \param the number of repeats to be set
     */
    void setTemplateRepeats( unsigned int iTemplateRepeats ); 
    
    /**
     * get the number of circles copied in the expansion template
     * \return the number of repeats
     */
    unsigned int getTemplateRepeats(); 

    /**
     * set the number of circles copied in the search template
     * \param the number of repeats to be set
     */
    void setSearchTemplateRepeats( unsigned int iSearchTemplateRepeats ); 
    
    /**
     * get the number of circles copied in the search template
     * \return the number of repeats
     */
    unsigned int getSearchTemplateRepeats(); 

    /**
     * set the distance between two repeats
     * \param the distance between repeats
     */
    void setDistBetweenRepeats( Real64 fDistBetweenRepeats ); 
   
    /**
     * get the distance between two repeats
     * \return the distance between repeats
     */
    Real64 getDistBetweenRepeats(); 

    /**
     * set the size of a crawling step 
     * \param the size of a crawling step
     */
    void setCrawlingStepSize( Real64 fCrawlingStepSize ); 

     /**
     * get the size of a crawling step 
     * \return the size of a crawling step
     */
     Real64 getCrawlingStepSize( );

    /**
     * set fit high resolution helix
     */ 
    void setFitHelix( bool bFitHelix );
    /**
     * get fit high resolution helix
     */ 
    bool getFitHelix();
    /**
     * set the min number of residues in Helix
     * \params the number of residues
     */ 
    void setMinResInHelix( unsigned int iMinResInHelix);
    /**
     * get the min number of residues in Helix
     * \return the number of residues
     */ 
    unsigned int getMinResInHelix();

    /**
     * are results available and can be outputed
     */ 
    bool canOutputResults();

    /**
     * get the check Intersections - whether the algorithm should check the intersections of cylinders
     * \return check Intersections 
     */ 
    bool getCheckIntersections();

    /**
     * set the check Intersections - whether the algorithm should check the intersections of cylinders
     * \param check Intersections 
     */ 
    void setCheckIntersections(bool bCheckIntersections);

    /**
     * set anisotropic correction related parameters
     * \param fAni the correction, e.g. fAni=2 will compress the map 2 times
     * \param fOrigZWoAni the origin of the map before ani correction - use for decompression 
     */ 
    void setAniCorr( Real64 fAni, Real64 fOrigZWoAni);

    /**
     * get anisotropic correction 
     * \param fAni the correction, e.g. fAni=2 will compress the map 2 times
     */ 
    Real64 getAniCorr();
 
    /**
     * get Origin on Z axis of the map without anisotropic correction 
     */ 
    Real64 getOrigZWoAni();

    /**
     * get Origin on Z axis of the map with anisotropic correction 
     */ 
    Real64 getOrigZ();
  
    /**
     * Set the correction factor lambda
     */ 
    void setLambda(Real64 fLambda);

    /**
     * Get the correction factor lambda
     */ 
    Real64 getLambda();

    /**
     * set state: are there any new results to be displayed
     */
    void setNewResultsFound(bool bNewResults);

    /**
     * return state: are there any new results to be displayed
     */
    bool getNewResultsFound();

    /**
     * init kernels
     */
    void initKernels();


    //
    // Fitness function
    //

    /**
     * returns the transformation matrix
     */
    svt_ga_mat getTrans(T* pInd);  

    /**
     * Updates the coordinates of the model based on the genes of the individual
     * attention: it does not update the volume of the model (see update volume)
     */
    void updateModel(T* pInd);
    
    /**
     * Updates the volume of the model 
     * Attention: it does not update the model - the pdb remains the same
     * \param pInd the individual
     * \param bCoarse should the coarse model be used
     */
    void updateVolume(T* pInd);
   
    /**
     * update fitness
     * \param pInd pointer to individual that gets updated
     */
    virtual void updateFitness(T* pInd);

    /**
     * refine an individual; 
     * \param the individual that will be refined
     */
    virtual void refineInd(T* pInd);

    /**
     * refine an individual; by default it does nothing - overload in the classes that inherit
     * \param the individual that will be refined
     * \param iteration
     */
    void refine(T* pInd, int iIter);

    /**
     * refine the translation and rotation of an individual - calls the refinetransl and refineRot a few times
     * \param the individual that will be refined
     */
    void refineGenes(T* pInd, svt_vector4<Real64> *pCenter = NULL);

    /**
     * refine the translation of an individual
     * \param the individual that will be refined
     */
    void refineTransl(T* pInd);
    /**
     * refine the translation of an individual, only allow movements on Spheres
     * \param the individual that will be refined
     */
    void refineTranslOnSphere(T* pInd, svt_vector4<Real64> * pCenter);

    /**
     * refine the translation of an individual
     * \param the individual that will be refined
     */
    void refineRot(T* pInd, Real64 fMaxCorr=0.9925);

    /**
     * refine the rotation of an individual using random angles
     * \param the individual that will be refined
     */
    void refineRotRandom(T* pInd);

    /**
     * crawl on the tube and search for similar scoring cylinder placements
     * \param the individual that will be refined
     * \param iDirection indicates the direction  +1 for forward and -1 for backwards
     */
    void crawl(T* pInd, int iDirection);    

    /**
     * refine the length of an individual
     * \param the individual that will be refined
     */
    void refineInLength(T* pInd);
    
    /**
     * add 2 Cylinder
     */
    void add2Tube(T* pInd, bool bAddNew = false, bool bFlip = false);

    /**
     * set the cylinderes
     * \param the vector containing the helices 
     */
    void setCylinders (vector<svt_tube> & oCylinders); 
    /**
     * get the cylinderes
     * \return the vector containing the helices 
     */
    vector<svt_tube> & getCylinders();     
    /**
     * set the cylinderes
     * \param the vector containing the helices 
     */
    void setCylindersPdb(vector< svt_point_cloud_pdb<svt_ga_vec> > & oCylinders); 
    /**
     * get the cylinderes
     * \return the vector containing the helices 
     */
    vector< svt_point_cloud_pdb<svt_ga_vec> > & getCylindersPdb();      
    /**
     * Calculate the full correlation corresponding to the current individual (with blur)
     */
    Real64 getCorrelation();
   
    /**
     * Calculate the rmsd of the individual towards the target model
     * \return the rmsd 
     */
    Real64 getRMSD();

    //
    // Mutation
    //

    /**
     * custom mutation (can be changed by derived class, default implementation just calls mutationBGA)
     * \param iInd index of individual
     */
    virtual void mutationCustom(int iInd);
    
    /**
     * mutation with a cauchy distribution
     * \param iInd index of individual
     */
    void mutationCauchy(int iInd, int iRandIndex, Real64 fRatio=1.0);

    /**
     * mutation all the genes with cauchy of large standard deviation
     */
    void mutationAllCauchy(int iInd);

    /**
     * Compute Average Score of all solutions
     */ 
    void computeAvgScoreAll();

    //
    // Transposition
    //

   //
    // Output statistics, result files, etc
    //

    /**
     * output result
     */
    virtual void outputResult(bool bTabuAdded = false);

    /**
     * update result
     * \param the number of tubes to investigate
     * \param an int used for validation only
     */
    virtual void updateResults( unsigned int iNoOfTubes = 0, int iNum = 0);
 
   /**
     * output the best model
     */
    void outputBest();

    /**
     * print results (to cout)
     */
    void printResults();
    
    /**
     * writes the genes and scores of all individuals into file
     */
    void writePop(char* pFname_target);
    
    /**
     * Set the output path path
     * \param pPath pointer to array of char
     */
    void setOutputPath( const char *pPath );
    
    /**
     * Get the output path path
     * \param pPath pointer to array of char
     */
    const char* getOutputPath();

    /**
     * How many generations should gacylinder wait until it outputs a new model file (0 turns output off)
     * \param iWriteModelInterval number of generations
     */
    void setWriteModelInterval( unsigned int iWriteModelInterval );
    /**
     * How many generations should gacylinder wait until it outputs a new model file (0 turns output off)
     * \return number of generations
     */
    unsigned int getWriteModelInterval( );
    
    /**
     * output the configuration of the program
     */
    virtual void writeConfiguration(char * pFilename);

    
    //
    // Run in thread
    //
    
    /**
     * Write the top scoring solutions to the disk
     * \param oPop the population of solutions 
     * \param iWriteSolutions how many solutions to write
     */
    void writeSolutions(svt_population<T> &oPop, unsigned int iWriteSolutions, char *pFileName);

};

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
template<class T>
svt_eulerAngles svt_gacylinder<T>::m_oAngles;

/**
 * Constructor
 */
template<class T> 
svt_gacylinder<T>::svt_gacylinder(unsigned int iGenes) : svt_ga<T>( iGenes ),
    m_fWidth( 0.0f ),
    m_fRes( 8.0f ),
    m_fDelta( 0.5f ),
    m_fPsiFrom( 0.0f ),
    m_fPsiTo( 360.0f ),
    m_fThetaFrom( 0.0f ),
    m_fThetaTo( 180.0f ),
    m_fPhiFrom( 0.0f ),
    m_fPhiTo( 360.0f ), 
    m_fXFrom( 0.0f ),
    m_fXTo( 0.0f ),
    m_fYFrom( 0.0f ),
    m_fYTo( 0.0f ),
    m_fZFrom( 0.0f ),
    m_fZTo( 0.0f),
    m_fBorder(5.0f),
    m_bMask( true ),
    m_fAcceptMoveRatio(0.90),
    m_iMaxFailedCrawls(2),
    m_bCanOutputResults(false),
    m_bRefining(false),
    m_iNoOfCylinder2Detect (0),
    m_bApplyBlurring2Model( false ),
    m_fTemplateRadius( 1.0 ),
    m_fSearchTemplateRadius( 2.0 ),
    m_iTemplatePointCount( 11 ),
    m_iTemplateRepeats( 8 ),
    m_iSearchTemplateRepeats( 20 ),
    m_fDistBetweenRepeats( 5.1/4.0),
    m_fCrawlingStepSize ( 1.4/*5.4/3.6*/ ), //experimental value
    m_fExploredPercent (0.0),
    m_bFitHelix (false),
    m_fMaxDist4Map (4.0),
    m_iMinResInHelix( 4 ),
    m_fAni (1.0),
    m_fLambda (1.0),
    m_bNewResults(false)
{ 
    strcpy(m_pPath, "");
    m_fStepsPerTurn = 5.4/m_fCrawlingStepSize; 

    string oIdealHelixPdb;

    oIdealHelixPdb += "ATOM      1  N   ALA     1       0.463  -1.710  -4.234  1.00  0.00           N\n";
    oIdealHelixPdb += "ATOM      2  CA  ALA     1       1.638  -1.757  -3.415  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM      3  CB  ALA     1       2.935  -1.586  -4.226  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM      4  C   ALA     1       1.577  -0.657  -2.404  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM      5  O   ALA     1       1.939  -0.855  -1.245  1.00  0.00           O\n";
    oIdealHelixPdb += "ATOM      6  N   ALA     2       1.133   0.540  -2.832  1.00  0.00           N\n";
    oIdealHelixPdb += "ATOM      7  CA  ALA     2       1.115   1.689  -1.972  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM      8  CB  ALA     2       0.639   2.965  -2.685  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM      9  C   ALA     2       0.170   1.432  -0.845  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     10  O   ALA     2       0.449   1.777   0.305  1.00  0.00           O\n";
    oIdealHelixPdb += "ATOM     11  N   ALA     3      -0.992   0.831  -1.165  1.00  0.00           N\n";
    oIdealHelixPdb += "ATOM     12  CA  ALA     3      -2.032   0.617  -0.201  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     13  CB  ALA     3      -3.289  -0.016  -0.817  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     14  C   ALA     3      -1.517  -0.314   0.849  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     15  O   ALA     3      -1.779  -0.123   2.035  1.00  0.00           O\n";
    oIdealHelixPdb += "ATOM     16  N   ALA     4      -0.783  -1.362   0.425  1.00  0.00           N\n";
    oIdealHelixPdb += "ATOM     17  CA  ALA     4      -0.290  -2.355   1.340  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     18  CB  ALA     4       0.456  -3.490   0.618  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     19  C   ALA     4       0.677  -1.721   2.286  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     20  O   ALA     4       0.689  -2.034   3.480  1.00  0.00           O\n";
    oIdealHelixPdb += "ATOM     21  N   ALA     5       1.535  -0.823   1.763  1.00  0.00           N\n";
    oIdealHelixPdb += "ATOM     22  CA  ALA     5       2.549  -0.248   2.597  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     23  CB  ALA     5       3.495   0.696   1.834  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     24  C   ALA     5       1.892   0.543   3.685  1.00  0.00           C\n";
    oIdealHelixPdb += "ATOM     25  O   ALA     5       2.342   0.468   4.827  1.00  0.00           O\n";

    unsigned int iModel = 0;
    unsigned int iCount = 0;
    int iPrevResSeq = -1;
    int iOrdResSeq = 0;
    char cPrevChainID = '-';

    char pLine[90];
    for (unsigned int iIndex =0; iIndex < oIdealHelixPdb.length(); iIndex+=79 )
    {
        strcpy(pLine, (oIdealHelixPdb.substr(iIndex, 79).c_str()));
        m_oIdealHelix.parseLine(pLine, iModel, iCount, iPrevResSeq, iOrdResSeq, cPrevChainID);
    }

    sprintf( m_pRmsdOut," ");
};


/**
 * Destructor
 */
template<class T>
svt_gacylinder<T>::~svt_gacylinder() {};

/**
 * create a new individual
 */
template<class T>
T svt_gacylinder<T>::initIndividual( )
{
    T oInd;
    svt_array_real64 oGenes;
    // create new object - that overlap a bit with the map
    do
    {
        unsigned int iX, iY, iZ;
        do 
        {
        oGenes.clear();
            for (int j=0; j< 3; j++)
            oGenes.push_back( svt_genrand() );
            iX = int ( oGenes[0] * m_oTar.getSizeX() );
            iY = int ( oGenes[1] * m_oTar.getSizeY() );
            iZ = int ( oGenes[2] * m_oTar.getSizeZ() );            
        }while (m_oTar.at(iX, iY, iZ) <= 0.0  );
  
        for (int j=3; j< this->m_iGenes; j++)
        oGenes.push_back( svt_genrand() );
    oInd.setGenes( oGenes );
        oInd.setTurns ( m_iTemplateRepeats );
        oInd.setHeightTurn( m_fDistBetweenRepeats );
    
    makeValid( &oInd );
    updateFitness( &oInd );
        m_iNoIndInInit++;
    }while ( oInd.getFitness() < 1e-6 || this->isInTabuReg(&oInd) );

    oInd.setOrigin(RANDOM);
    oInd.resetAge();
    
    return oInd;
}

/**
 * initialize the angles
 */
template<class T>
void svt_gacylinder<T>::initAngles()
{
    // create the angle table
    if (m_oAngles.getAngleCount() <= 0)
    {
        svtout << "Create angular search table (P/T/P, Delta, #Angles, memory): ( " << m_fPsiFrom << " - " << m_fPsiTo << " / " << m_fThetaFrom << " - " << m_fThetaTo << " / " << m_fPhiFrom << " - " << m_fPhiTo << ", " << m_fDelta << ", ";

        m_oAngles.initTable( m_fPsiFrom, m_fPsiTo, m_fThetaFrom, m_fThetaTo, m_fPhiFrom, m_fPhiTo, m_fDelta );  

        cout << m_oAngles.getAngleCount() << ", " << floor((m_oAngles.getAngleCount() * sizeof(float) * 3) / 1048576) << "mb )" << endl;
    }
  
}

/**
 * generate initial population
 * \param iNum number of inds in this population
 */
template<class T>
void svt_gacylinder<T>::initPopulation(int iNum, bool bOutputInput)
{
    m_iNoIndInInit = 0; 

    svt_ga<T>::initPopulation(iNum);
   
    if (strlen(m_pPath) != 0 && bOutputInput)
    {
        // output the files
        char pFname[1256];
        if (m_fWidth!=0.0)
        {
            sprintf(pFname, "%s/target.mrc", m_pPath );
            m_oTar.save( pFname );
        }
        
        if (m_oTarStr.size()>0)
        {
            sprintf(pFname, "%s/target.pdb", m_pPath );
            m_oTarStr.writePDB( pFname );
        }

        if (m_oTemplate.size()>0)
        {
            sprintf(pFname, "%s/template.pdb", m_pPath );
            //m_oTemplate.writePDB( pFname );
        }

        if (m_oBigTemplate.size()>0)
        {
            sprintf(pFname, "%s/bigTemplate.pdb", m_pPath );
            //m_oBigTemplate.writePDB( pFname );
        }

    }
}

/**
 * Create an object 
 */
template<class T>
svt_gacylinder<T>* svt_gacylinder<T>::createObject()
{
    return new svt_gacylinder<T>( *this );
};

/**
 * create the volume template
 */
template<class T>
void svt_gacylinder<T>::initTemplate()
{
    //create the templates
    m_oTemplate.makeCylinder( m_iTemplatePointCount, m_fTemplateRadius, m_iTemplateRepeats, m_fDistBetweenRepeats);
    m_oBigTemplate.makeCylinder( m_iTemplatePointCount, m_fSearchTemplateRadius, m_iSearchTemplateRepeats, m_fDistBetweenRepeats);
    
    //create the circles
    m_oCircle1.deleteAllAtoms();
    m_oCircle2.deleteAllAtoms();
    m_oCircle3.deleteAllAtoms();

    svt_ga_vec oVec;
    oVec.x( 0.0 );
    oVec.y( 0.0 );
    oVec.z( 0.0 );
    
    svt_point_cloud_atom oAtom;
    oAtom.setName( "C");
    oAtom.setRemoteness('A');

    Real64 fDelta = 360.0/Real64(m_iTemplatePointCount-1); 
    for (Real64 fAngle = 0; fAngle < 360; fAngle += fDelta)
    {
        oVec.x ( 2*m_fTemplateRadius * cos (deg2rad( fAngle ) ) );
        oVec.y ( 2*m_fTemplateRadius * sin (deg2rad( fAngle ) ) );
        m_oCircle1.addAtom( oAtom, oVec );
        
        oVec.x ( 4*m_fTemplateRadius * cos (deg2rad( fAngle ) ) );
        oVec.y ( 4*m_fTemplateRadius * sin (deg2rad( fAngle ) ) );
        m_oCircle2.addAtom( oAtom, oVec );

        oVec.x ( 5*m_fTemplateRadius * cos (deg2rad( fAngle ) ) );
        oVec.y ( 5*m_fTemplateRadius * sin (deg2rad( fAngle ) ) );
        m_oCircle3.addAtom( oAtom, oVec );
    }
    if (strlen(m_pPath) != 0)
    {
        // output the files
        char pFname[1256];
   
        if (m_oTemplate.size()>0)
        {
            sprintf(pFname, "%s/template.pdb", m_pPath );
            m_oTemplate.writePDB( pFname );
        }

        if (m_oBigTemplate.size()>0)
        {
            sprintf(pFname, "%s/bigTemplate.pdb", m_pPath );
            m_oBigTemplate.writePDB( pFname );
        }

    }
};

/**
 * add helices found in threads to the global list of helices; discard the one that overlap
 * \param bFromThreads should only the helices from threads should be looked at? or all helices
 * \return the average length of the added helices
 */
template<class T>
Real64 svt_gacylinder<T>::discardCylinders( bool bFromThreads )
{
    //
    // get the cylinders to be considered : all vs the one detected at this thread
    //
    vector<svt_tube> oCylinders, oTmpList;
    if (bFromThreads)
    {
        for (unsigned int iThread=0; iThread < this->m_oGA_Array.size(); iThread++)
        {
            oCylinders = ((svt_gacylinder<T>*)this->m_oGA_Array[ iThread ])->getCylinders();
            oTmpList.insert(oTmpList.begin(), oCylinders.begin(), oCylinders.end());

            ((svt_gacylinder<T>*)this->m_oGA_Array[ iThread ])->clearCylinders();
        }
    }
    else
        oTmpList = m_oCylinders;

  
    //
    //check how they intersect - how much
    //   
    svt_point_cloud_pdb< svt_ga_vec > oPdb1, oPdb2;
    svt_ga_vol oVol1, oVol2;

    oVol2 = m_oTar; 
    oVol1 = m_oTar;

    //assume all helices should be added 
    vector<bool> oAdd;
    for (unsigned int iIndex=0; iIndex<oTmpList.size(); iIndex++)
        oAdd.push_back(true);
   
    for (unsigned int iIndex=0; iIndex<oTmpList.size(); iIndex++)
    {
        oPdb1 = oTmpList[iIndex].getTube();
        oAdd.push_back(true);
        //oTmpList[iIndex].estimate_curvature();

        oVol1.setValue(0.0);
        oPdb1.projectMass( &oVol1 );
        oVol1.convolve1D3D(m_oKernel, false ); // don't normalize 

        for (unsigned int iIndex1=iIndex+1; iIndex1<oTmpList.size(); iIndex1++)
        {
            oPdb2 = oTmpList[iIndex1].getTube();

            oVol2.setValue(0.0);
            oPdb2.projectMass( &oVol2 );
            oVol2.convolve1D3D(m_oKernel, false ); 
            Real64 fCorr = oVol2.correlation(oVol1, false);

            //they overlap alot?
            if (fCorr>0.5)
            { 
                if (oTmpList[iIndex].getAvgScore()*oTmpList[iIndex].getAvgScore()*oTmpList[iIndex].getLength() < 
                    oTmpList[iIndex1].getAvgScore()*oTmpList[iIndex1].getAvgScore()*oTmpList[iIndex1].getLength())
                    oAdd[ iIndex ] = false;
                else
                    oAdd[ iIndex1 ] = false;
            }
        }
    }
    
    /*
    Real64 fDotProd;
    svt_ga_vec oStart1, oStart2, oEnd1, oEnd2, oDir1, oDir2;
    if (!bFromThreads)
    for (int iIndex=0; iIndex<(int)oTmpList.size(); iIndex++)
    {
        if ( oAdd[ iIndex ] )
        {
            oPdb1 = oTmpList[iIndex].getTube();
            if (oPdb1.size()>0)
            {
                oDir1   = oTmpList[iIndex].getDirection();
                oStart1 = oPdb1[0];
                oEnd1   = oPdb1[ oPdb1.size() -1 ];
            
                for (unsigned int iIndex1=iIndex+1; iIndex1<oTmpList.size(); iIndex1++)
                {
                    //svtout << iIndex << " " << iIndex1 <<  " " << oAdd[ iIndex1 ] << " " << endl ;
                    if ( oAdd[ iIndex1 ] )
                    {
                        oDir2 = oTmpList[iIndex1].getDirection();
                        fDotProd = oDir1.x() * oDir2.x() + oDir1.y() * oDir2.y() + oDir1.z() * oDir2.z(); 

                        oPdb2 = oTmpList[iIndex1].getTube();

                        if (oPdb2.size()>0)
                        {
                            oStart2 = oPdb2[0];
                            oEnd2   = oPdb2[ oPdb2.size() -1 ];

                            
                            //cout << oStart1.x() << " " << oStart1.y() << " " <<  oStart1.z() << endl;
                            //cout << oEnd1.x() << " " << oEnd1.y() << " " <<  oEnd1.z() << endl;
                            //cout << oStart2.x() << " " << oStart2.y() << " " <<  oStart2.z() << endl;
                            //cout << oEnd2.x() << " " << oEnd2.y() << " " <<  oEnd2.z() << endl;
                            //cout << "Dir:: " << oTmpList[iIndex].getDirection() << "  ---  " << oTmpList[iIndex1].getDirection() <<  "  ---   " << fDotProd <<  " " <<  abs(abs(fDotProd) - 1) << " " << (abs(abs(fDotProd) - 1) < 0.1) << endl; 
                            //cout << oStart1.distance( oStart2 ) << " true:" << (oStart1.distance( oStart2 ) < 6.0) << endl 
                            //     << oEnd1.distance( oEnd2 ) << " true:" << (oEnd1.distance( oEnd2 ) < 6.0) << endl  << 
                            //        oStart1.distance( oEnd2 ) << " true:" << (oStart1.distance( oEnd2 ) <  6.0) << endl << 
                            //        oEnd1.distance( oStart2 ) << " true:" << (oEnd1.distance( oStart2 ) <  6.0) << endl; 
                                        

                            // abs(abs(fDotProd) - 1)< 0.1 ~ do they have the same direction: (u . v = |u|*|v|*cos(ang))
                            // ends are close
                            if ( abs(abs(fDotProd) - 1) < 0.25 && 
                                ( oStart1.distance( oStart2 ) < 6.0   || oEnd1.distance( oEnd2 )      < 6.0 || 
                                  oStart1.distance( oEnd2 )   < 6.0   || oEnd1.distance( oStart2 )    <  6.0   ) )
                            {
                                if ( (abs(fDotProd - 1) < 0.25 && (oStart1.distance( oStart2 ) < 6.0 || oEnd1.distance( oEnd2 ) < 6.0)) ||  //(abs(fDotProd - 1) < 0.25 - angle is 0 
                                     (abs(fDotProd + 1) < 0.25 && (oStart1.distance( oEnd2 ) <  6.0 || oEnd1.distance( oStart2 ) <  6.0) )  )
                                {
                                    svtout << "Dont merge" << endl;
                                }else
                                {
                                    vector<T> oElems = oTmpList[iIndex1].getElements();
            
                                  //  cout << oPdb1.size() << " " <<  oTmpList[iIndex].getAvgScore() << " " << oTmpList[iIndex].getScores()[0] << " " << oTmpList[iIndex].getScores()[1] <<" " << oTmpList[iIndex].getScores()[2] <<endl;
                                  ///  cout << oPdb2.size() << " " <<  oTmpList[iIndex1].getAvgScore() << " " << oTmpList[iIndex1].getScores()[0] << " " << oTmpList[iIndex1].getScores()[1] <<" " << oTmpList[iIndex1].getScores()[2] <<endl;
                                    oTmpList[iIndex].computeAvgScore(); 
                                    oTmpList[iIndex1].computeAvgScore(); 
                                    if ( abs(fDotProd + 1) < 0.25 ) // cos is negative 1 then flip
                                    {   
                                        if (oElems.size()>0)
                                            oTmpList[iIndex].add2Tube(oElems[0], true);
                                        for (unsigned i=1; i<oElems.size(); i++)
                                            oTmpList[iIndex].add2Tube(oElems[i]);
                                        svtout << "Merge and flip " << endl;
                                    }
                                    else
                                    {
                                       for (unsigned i=0; i<oElems.size(); i++)
                                            oTmpList[iIndex].add2Tube(oElems[i]);
                                        svtout << "Merge" << endl;
                                    }
                
                                    oTmpList[iIndex].computeAvgScore(); 
                                    oAdd[iIndex1] = false;

                                    iIndex --;
                                    iIndex1 = oTmpList.size();
                                }
                            }

                        }
                    }
                }
            }
        }
    }
    */

    //delete all before putting back the one that don't overlap; 
    if (!bFromThreads)
        m_oCylinders.clear();
    
    // for the one that are to be added; compute the other scores 
    // also create high-resolution helix
    Real64 fAvg = 0;
    unsigned int iCount = 0;
    for (unsigned int iIndex=0; iIndex<oTmpList.size(); iIndex++)
    {
        if (oAdd[iIndex])
        {             
            fAvg += oTmpList[iIndex].getTurns(m_fStepsPerTurn);
            iCount++;    

            if (m_bFitHelix)
                 oTmpList[iIndex].createHighResTube( m_oTar, &m_oIdealHelix );
       
            m_oCylinders.push_back(oTmpList[iIndex]);
            oTmpList[iIndex].fillExplored( m_oTar, &m_oExploredVol);

            oPdb1 = m_oCylinders[m_oCylinders.size()-1].getTube();
            oPdb2 = m_oCylinders[m_oCylinders.size()-1].getTube( &m_oCircle3 );
            svt_point_cloud_pdb< svt_ga_vec > oPdb3, oPdb4;
            oPdb3 = m_oCylinders[m_oCylinders.size()-1].getTube( &m_oCircle1 );
            oPdb4 = m_oCylinders[m_oCylinders.size()-1].getTube( &m_oCircle2 );
    
            /*
            char pFname[256];        
            sprintf(pFname, "%s/Tube0_%d.pdb", m_pPath, iIndex );
            oPdb1.writePDB( pFname );
            sprintf(pFname, "%s/Tube1_%d.pdb", m_pPath, iIndex );
            oPdb3.writePDB( pFname );
            sprintf(pFname, "%s/Tube2_%d.pdb", m_pPath, iIndex );
            oPdb4.writePDB( pFname );
            sprintf(pFname, "%s/Tube3_%d.pdb", m_pPath, iIndex );
            oPdb2.writePDB( pFname );
            */

            svt_ga_mat oMat; 
            Real64 fScore;
            fScore = oPdb1.projectMassCorr(&m_oTar, oMat, false);
            m_oCylinders[m_oCylinders.size()-1].setScore( 0, fScore);
            fScore = oPdb2.projectMassCorr(&m_oTar, oMat, false);
            m_oCylinders[m_oCylinders.size()-1].setScore( 1, fScore);

            Real64 fAvgCcExpansion = m_oCylinders[m_oCylinders.size()-1].getAvgScore();
            Real64 fLen = m_oCylinders[m_oCylinders.size()-1].getLength();
            Real64 fCCInterior = m_oCylinders[m_oCylinders.size()-1].getScores()[0];
            Real64 fCCExterior = m_oCylinders[m_oCylinders.size()-1].getScores()[1];
            m_oCylinders[m_oCylinders.size()-1].setScore( 2, 
                    fAvgCcExpansion*fAvgCcExpansion*fCCInterior*fCCInterior/(fCCExterior*fCCExterior)*fLen );
            if (bFromThreads)
                m_oCylinders[m_oCylinders.size()-1].setScore( 3, this->m_iParallelRun ); 
        }    
    }

    if (iCount > 0)
        fAvg /= (Real64)iCount;
    
    //
    //at the end and when a target is given compute some statistics - RMSD
    //
    if (!bFromThreads && m_oTarStr.size()>0)
    {
        svt_point_cloud_pdb< svt_ga_vec > oPdb, oPdbHel;
        vector <int> oAxesMapped, oCAMapped;
        vector <int> oPredMapped; // the predicted points that were already
        vector <Real64> oAxesPointsMinDist, oCAMinDist, oPredMinDis;
        vector <Real64> oPredPointsMinDist; // the min distance from the predicted points to the actual axes


        for (unsigned int iA2 = 0 ; iA2 < m_oTarStrAxes.size(); iA2++)
        {
            oAxesMapped.push_back( 0 );
            oAxesPointsMinDist.push_back(1e10);
            m_oTarStrAxes.getAtom(iA2)->setTempFact( 0.0 ); 
        }

        for (unsigned int iA2 = 0 ; iA2 < m_oTarStrCaInHelix.size(); iA2++)
        {
            oCAMapped.push_back( 0 );
            oCAMinDist.push_back(1e10);
            m_oTarStrCaInHelix.getAtom(iA2)->setTempFact( 0.0 );
        }

        svt_point_cloud_pdb<svt_ga_vec> oPredPoints;
        for (unsigned int iIndex=0; iIndex<m_oCylinders.size() && iIndex < m_iNoOfCylinder2Detect; iIndex++)
        {
            //rmsd on the axis
            oPdb = m_oCylinders[iIndex].getTube();
            for (unsigned int iA1 = 0 ; iA1 < oPdb.size(); iA1++)
            {
                oPredPoints.addAtom( *oPdb.getAtom(iA1), oPdb[iA1] );
                oPredPoints.getAtom( oPredPoints.size() - 1 )->setTempFact( 0.0 );
                oPredPointsMinDist.push_back(1e10);
            }
        }

        Real64 fDist, fDistMin, fRmsd = 0.0;

        int fMaxDist4MapSq = m_fMaxDist4Map*m_fMaxDist4Map;
        int iCountPredPointsMatched = 0;

        for (unsigned int iA1 = 0 ; iA1 < oPredPoints.size(); iA1++)
        {
            fDistMin =  oPredPoints[iA1].distanceSq( m_oTarStrAxes[0] );
            for (unsigned int iA2 = 1 ; iA2 < m_oTarStrAxes.size(); iA2++)
            {
                fDist = oPredPoints[iA1].distanceSq( m_oTarStrAxes[iA2] );
                if (fDist<fDistMin)
                    fDistMin = fDist;
            }

            oPredPointsMinDist[iA1] = fDistMin;         
            if (fDistMin<fMaxDist4MapSq)
            {
                iCountPredPointsMatched++;
                oPredPoints.getAtom( iA1 )->setTempFact( 7.0 );
            }
        else
            oPredPoints.getAtom( iA1 )->setTempFact( 1.0 );
    
        }
    
        int iCountAxesPointsMatched = 0;
        for (unsigned int iA2 = 0 ; iA2 < m_oTarStrAxes.size(); iA2++)
        {
            fDistMin = oPredPoints[0].distanceSq( m_oTarStrAxes[iA2] );
            for (unsigned int iA1 = 1 ; iA1 < oPredPoints.size(); iA1++)
            {
                fDist = oPredPoints[iA1].distanceSq( m_oTarStrAxes[iA2] );
                if (fDist<fDistMin)
                    fDistMin = fDist;
            }
        
            oAxesPointsMinDist[iA2] = fDistMin;
            if (fDistMin<fMaxDist4MapSq)
            {
                iCountAxesPointsMatched++;
                m_oTarStrAxes.getAtom( iA2 )->setTempFact( 7.0 );
            }
            else
                m_oTarStrAxes.getAtom( iA2 )->setTempFact( 1.0 );   
        }
        
        iCount = 0;
        fRmsd = 0;
        for (unsigned int iA2 = 0 ; iA2 < m_oTarStrAxes.size(); iA2++)
            if (oAxesPointsMinDist[iA2]< fMaxDist4MapSq)
            {      
                fRmsd+= oAxesPointsMinDist[iA2];
                iCount++;
            }

        if (iCount!=0)
            fRmsd /= iCount;
    
        fRmsd = sqrt(fRmsd);
   
        Real64 fSe  = iCountAxesPointsMatched/(Real64)m_oTarStrAxes.size();
        Real64 fPpv = iCountPredPointsMatched/(Real64)oPredPoints.size();

        sprintf(m_pRmsdOut," RMSD: %6.3f %6.2f %6.2f %4d %4d %4d %4d %4d", fRmsd, fSe*100.0, fPpv*100.0, iCountAxesPointsMatched, m_oTarStrAxes.size() , iCountPredPointsMatched, oPredPoints.size(), m_oTarStrAxes.size() );
    }

    return fAvg;
}

/**
 * discard points from the cylinders according to their score // use here the average score of all accepted points in crawling
 */
template<class T>
void  svt_gacylinder<T>::discardPointsoOnCylinders()
{
    char pOut[1024];

    // compute the score
    computeAvgScoreAll();

    svtout << "Average score " << m_fAvgScoreAll << endl;
 
    for (unsigned int iIndex=0; iIndex < m_oCylinders.size(); iIndex++)
    {
        sprintf(pOut," %2d %3d %8.3f ", iIndex, (int)m_oCylinders[iIndex].size(),  m_oCylinders[iIndex].getAvgScore());
        m_oCylinders[iIndex].discardPointsAtEnds( m_fAvgScoreAll*0.9 );
        sprintf(pOut,"%s %2d %3d %8.3f ", pOut, iIndex, (int)m_oCylinders[iIndex].size(),  m_oCylinders[iIndex].getAvgScore());
        svtout << pOut << endl;
    }

    sortCylinders();

    while(m_oCylinders.size()>0 && m_oCylinders[m_oCylinders.size()-1].getAvgScore() == 0 )
        m_oCylinders.pop_back();

   
};

/**
 * delete all helices
 */
template<class T>
void svt_gacylinder<T>::clearCylinders()
{
    m_oCylinders.clear();
};

/**
 * post process tubes
 */
template<class T>
void svt_gacylinder<T>::postProcessTubes()
{
    svt_ga_vol oVol1, oVol2;
    svt_point_cloud_pdb< svt_ga_vec > oPdb1, oPdb2;
    oVol2 = m_oTar; 
    oVol1 = m_oTar; 
 
    //assume all helices should be added 
    vector<bool> oAdd;
    for (unsigned int iIndex=0; iIndex<m_oCylinders.size(); iIndex++)
    {
        oAdd.push_back(true);
        m_oCylinders[iIndex].setPenalty( 0.0 );
    }

    bool bFoundMultipleTimes;
    for (unsigned int iTube1=0; iTube1<m_oCylinders.size(); iTube1++)
    {
        oPdb1 = m_oCylinders[iTube1].getTube();
        
        oVol1.setValue(0.0);
        oPdb1.projectMass( &oVol1 );
        oVol1.convolve1D3D(m_oKernel, false ); // don't normalize 
        
        bFoundMultipleTimes = false;
        
        if (oAdd[iTube1]) // not yet decided to discard
            for (unsigned int iTube2=0; iTube2<m_oCylinders.size() ; iTube2++)
            {
                if (iTube2!=iTube1)
                {
                    oPdb2 = m_oCylinders[iTube2].getTube();

                    oVol2.setValue(0.0);
                    oPdb2.projectMass( &oVol2 );
                    oVol2.convolve1D3D(m_oKernel, false ); 
                    Real64 fCorr = oVol2.correlation(oVol1, false);

                    //svtout << "Comparing (" << iTube1 << " - " << iTube2 << ") " << fCorr << " " ; 
                    //they overlap?
                    if (fCorr>0.5)
                    {
                        bFoundMultipleTimes = true;
                        if (m_oCylinders[iTube1].getAvgScore() < m_oCylinders[iTube2].getAvgScore())
                            oAdd[ iTube1 ] = false;
                        else
                            oAdd[ iTube2 ] = false; 
                    } 
                      
                }
            }

        if (!bFoundMultipleTimes) 
            oAdd[ iTube1 ] = false;
    } 

    vector<svt_tube> oCylinders;
    for (unsigned int iIndex=0; iIndex<m_oCylinders.size(); iIndex++)
        if (oAdd[iIndex])
            oCylinders.push_back(m_oCylinders[iIndex]);
    
    clearCylinders();
    m_oCylinders = oCylinders;

    sortCylinders();

    updateResults( m_iNoOfCylinder2Detect, 2);
    updateResults( m_oCylinders.size(), 3);
};  

/**
 * sort the cylinders and fills the Cylinder pdb
 */
template<class T>
void svt_gacylinder<T>::sortCylinders()
{
    sort( m_oCylinders.rbegin(), m_oCylinders.rend(), svt_tube::lt_score );

    m_oCylindersPdb.clear();
   
    for (unsigned int iIndex=0; iIndex< m_oCylinders.size(); iIndex++)
        m_oCylindersPdb.push_back( m_oCylinders[iIndex].getTube(NULL, true) );
};

/**
 * function to create the thread
 * it assumes that the object that calls it already has the pop initialized 
 */
template<class T>
svt_population<T> svt_gacylinder<T>::execute()
{
   //clear arrays/lists if runs were already executed
    (*this).m_oGA_Array.clear();
    svt_ga<T>::delTabuRegions();
    (*this).setDone(false);
    m_oCylinders.clear();
    m_oCylindersPdb.clear();
    m_oExploredVol.setValue(0.0);
  
    sprintf( m_pRmsdOut," ");

    initAngles();
    initTemplate();
    initKernels();

    vector< svt_gacylinder* > oGA_Array;
    for(unsigned int iThread=0; iThread < (*this).m_iMaxThread; iThread++ )
    {
        svt_gacylinder* pGA_Tmp = (*this).createObject() ;
        pGA_Tmp->setThread( iThread );
        pGA_Tmp->setRun( (*this).m_iRun );
        pGA_Tmp->setMaxGen( (*this).m_iSyncGen );
        oGA_Array.push_back( pGA_Tmp );
    }

    for(unsigned int iThread=0; iThread < (*this).m_iMaxThread; iThread++ )
        (*this).m_oGA_Array.push_back( oGA_Array[iThread]);
    
    long iStartTimeMerge, iStartTime = svt_getElapsedTime();    
    unsigned int iThreadGenerations = 0;

#ifdef __GAFIT_FORK
    vector<pid_t> oPid;
    pid_t iPid;
#endif
    bool bDone = false;
    svt_population<T> oFinalPop, oTotalPop, oTotalTabuPop;  
    char pOut[1024];
    int iCount2Short = 0; //counts how many times too short cylinders were found 
    Real64 fAvgLength;
    
    for(int iParallelRun = 0; iParallelRun <  (*this).m_iMaxGen/(*this).m_iSyncGen && bDone == false; iParallelRun++)
    {
        this->m_iParallelRun = iParallelRun;
        iStartTimeMerge = svt_getElapsedTime();
        
        svtout << "Starting " << (*this).m_iMaxThread << " threads..." << endl;

        (*this).initPopulation( (*this).m_iPopSize );
            
        // start the threads
        for( unsigned int iThread=0; iThread < (*this).m_iMaxThread; iThread++ )
        {
            (*this).m_oGA_Array[iThread]->setCurrGen( 0 );
            ((svt_gacylinder<T>*)(*this).m_oGA_Array[iThread])->setFoundCylinder( true );
            (*this).m_oGA_Array[iThread]->setPopulation( (*this).m_oPop );

            (*this).m_oGA_Array[iThread]->setDone( false );
#ifndef __GAFIT_FORK
            (*this). m_oGA_Array[iThread]->initThread();
#else
            if ( (iPid = fork()) == 0 )
            {
                oPid.push_back( iPid );
                (*this).m_oGA_Array[iThread]->run();
                exit( 0 );
            }
#endif
        }

#ifdef __GAFIT_FORK
        while( oPid.size() < (unsigned int)m_iMaxThread );
#endif

        // make sure threads are really running
        bool bSomeNotRunning = true;
        while(bSomeNotRunning)
        {
            bSomeNotRunning = false;
            for( unsigned int iThread=0; iThread < (*this).m_iMaxThread; iThread++ )
            if (!(*this).m_oGA_Array[iThread]->getIsThreadRunning())
                bSomeNotRunning = true;
        }

        sprintf(pOut,"Waiting for the threads to finish...");
        svtout << pOut << endl;
        
        // wait until they are finished
        bool bAllFinished = false;
        while(!bAllFinished)
        {
            bAllFinished = true;
            for( unsigned int iThread=0; iThread < (*this).m_iMaxThread && bDone == false; iThread++ )
            {
            if ( !(*this).m_oGA_Array[iThread]->getDone() )
                bAllFinished = false;
            else
                if ((*this).m_oGA_Array[iThread]->getCurrGen() < (*this).m_iSyncGen)
                {
                    //svtout << "Thread " << iThread << " finished with stopping criterion (score higher than)!" << endl;
                    bDone = true;
                    for(unsigned int i=0; i < (*this).m_iMaxThread; i++ )
                    if (i!=iThread)
                        (*this).m_oGA_Array[i]->setDone( true );
                    iThreadGenerations += (*this).m_oGA_Array[iThread]->getCurrGen();
                }
            }

            if (!bAllFinished)
            {
                //svtout << "waiting to finish" << endl;
                svt_sleep( 500 );
            }
        }
        
        // make sure threads are really not running anymore
        bool bSomeRunning = true;
        while(bSomeRunning)
        {
            bSomeRunning = false;
            for( unsigned int iThread=0; iThread < (*this).m_iMaxThread; iThread++ )
            if ((*this).m_oGA_Array[iThread]->getIsThreadRunning())
                bSomeRunning = true;
        }
        
        //delete the content of the populations and start again
        oTotalPop.clear(); 
        oTotalTabuPop.clear();
        
        // combine populations
        for( unsigned int iThread=0; iThread < (*this).m_iMaxThread; iThread++ )
        {
            svt_population<T> oTempPop = (*this).m_oGA_Array[iThread]->getPopulation();
            svt_population<T> oTabuTempPop = (*this).m_oGA_Array[iThread]->getTabuRegions();
            
            oTotalPop.insert(oTotalPop.begin(), oTempPop.begin(), oTempPop.end());
            oTotalTabuPop.insert(oTotalTabuPop.begin(), oTabuTempPop.begin(), oTabuTempPop.end());
        }
        
        // keep track of how many generations we have so far
        if (!bDone)
            iThreadGenerations+=(*this).m_iSyncGen;

        sort( oTotalPop.begin(), oTotalPop.end() );
        (*this).m_oGA_Array[0]->penalizeSimilar(oTotalPop, 2.0*(*this).getDistanceThreshold(), (1-2.0*(1-(*this).getDistanceThresholdPenalty())) ); //penalty twice larger than usual
        sort( oTotalPop.begin(), oTotalPop.end() );
        
        sort( oTotalTabuPop.begin(), oTotalTabuPop.end() );
        (*this).m_oGA_Array[0]->penalizeSimilar(oTotalTabuPop, 0.0, 1.0 );
        (*this).m_oGA_Array[0]->discardNullInd(oTotalTabuPop);
        sort( oTotalTabuPop.begin(), oTotalTabuPop.end() );

        (*this).m_oTabus = oTotalTabuPop;
        
        if (!bDone)
        {
            for( unsigned int iThread=0; iThread < (*this).m_iMaxThread; iThread++ )
            {
                    (*this).m_oGA_Array[iThread]->setTabuRegions( oTotalTabuPop );
            if (iThread == 0)
               (*this). m_oGA_Array[iThread]->outputResult( true );
            }
        }

        // discard the cylinders that were found in this parallel run
        fAvgLength = discardCylinders();
        if (fAvgLength < 2)
            iCount2Short++;
        else
            iCount2Short = 0;
    
        if (iCount2Short>=4)
        {
            bDone = true; // stop here as only short helices were found in the last runs
            svtout << "Stop exploration as only short helices were found" << endl;
        }
   
        m_bCanOutputResults = true; 
        sortCylinders();
        updateResults();
   
        if (m_fExploredPercent >= 85)
        {
            bDone = true;
            svtout << "Stopping: more than 85 percent of the map was covered!" << endl;
        }

        if (m_iNoIndInInit/(Real64)this->m_iPopSize  >= 40) // if 20 x more inviduals were created then stop here
        {
            bDone = true;
            svtout << "Stopping: search templates can't be placed within map as space is full with tabu regions!" << endl;
        }

        // found 3 times more than requested
        if (m_iNoOfCylinder2Detect > 0 && m_oCylinders.size()> m_iNoOfCylinder2Detect*3.0) // found 3 times more than requested
        {
            bDone = true;
            svtout << "Stopping: found more than three times the requested " << m_iNoOfCylinder2Detect << " traces!" << endl;
        }

        svtout  << "Recombination of the populations:" << endl;
        svtout << "  Number of generations (per thread): " <<  iThreadGenerations << endl;
        svtout << "  Number of tabu regions  "<<  (int)oTotalTabuPop.size() << endl;
        svtout << "  Partial Elapsed time : " <<  (((svt_getElapsedTime() - iStartTimeMerge)/(1000.0f))/60.0f) << " min" << endl;        
    }

    sortCylinders();
    discardCylinders(false);
    //discardPointsoOnCylinders();
    svtout << "A total of " << m_oCylinders.size() <<  " helices were found" << endl;
    sortCylinders();
    updateResults( m_iNoOfCylinder2Detect );
    updateResults( m_oCylinders.size(), 1 );


    //Output runtime stats
    long iETime = svt_getElapsedTime() - iStartTime;
    svtout << "Number of generations (per thread): " << iThreadGenerations <<  endl;
    svtout << "Elapsed time: " << ((iETime/(1000.0f))/60.0f) << " min" <<  endl; 
    
    //clear the content of the threads
    this->clearThreads();

    return oFinalPop;
}


///////////////////////////////////////////////////////////////////////////////
// Parameters
///////////////////////////////////////////////////////////////////////////////

/**
 * Set resolution of the target map
 * \param fRes the resolution of the target map
 */
template<class T>
void svt_gacylinder<T>::setResolution(Real64 fRes)
{
    m_fRes = fRes;
};
/**
 * Get resolution of the target map
 * \return the resolution of the target map
 */
template<class T>
Real64 svt_gacylinder<T>::getResolution()
{
    return m_fRes;
};

/**
 * Set the masking when computing the correlation
 * \param bMask bool indicating whether to mask
 */
template<class T>
void svt_gacylinder<T>::setMask(bool bMask)
{
    m_bMask = bMask;
};

/**
 * Set the angular step size. In general: The smaller the value, the better the accuracy. In contrast to an exhaustive search, the runtime will also not be longer, if a finer step size
 * is chosen. The only limitation is the memory - as the table needs to be stored, very low numbers might result in an excessive use of memory. Recommended: ~0.5 to 1.0.
 * \param fDelta the angular step size (default 0.5)
 */
template<class T>
void svt_gacylinder<T>::setAngularStepSize( Real64 fDelta )
{
    m_fDelta = fDelta;
};
/**
 * Get the angular step size. In general: The smaller the value, the better the accuracy. In contrast to an exhaustive search, the runtime will also not be longer, if a finer step size
 * is chosen. The only limitation is the memory - as the table needs to be stored, very low numbers might result in an excessive use of memory. Recommended: ~0.5 to 1.0.
 * \return the angular step size (default 0.5)
 */
template<class T>
Real64 svt_gacylinder<T>::getAngularStepSize( )
{
    return m_fDelta;
};

/**
 * Returns the number of angles 
 */
template<class T>
Real64 svt_gacylinder<T>::getAnglesCount()
{
    return m_oAngles.getAngleCount();
};

/**
 * Get angles
 */
template<class T>
svt_vector4<Real64>& svt_gacylinder<T>::getAngle( long unsigned int iIndex)
{
   svt_vector4<Real64> oAngle;
   
   if (iIndex >=0 && iIndex< m_oAngles.getAngleCount())
   {
    oAngle.x( m_oAngles.getPsi  ( iIndex ) );
    oAngle.y( m_oAngles.getTheta( iIndex ) );
    oAngle.z( m_oAngles.getPhi  ( iIndex ) );
   }else
   {
       svtout << "Angle out of boundaries! Exiting.." << endl;
   }
   
   return oAngle;
};


/**
 * Set the ranges for the angular search.
 * \param fPsiFrom   lower limit of the psi angles
 * \param fPsiTo     upper limit of the psi angles
 * \param fThetaFrom lower limit of the theta angles
 * \param fThetaTo   upper limit of the theta angles
 * \param fPhiFrom   lower limit of the phi angles
 * \param fPhiTo     upper limit of the phi angles
 */
template<class T>
void svt_gacylinder<T>::setAngularSearchRange( Real64 fPsiFrom, Real64 fPsiTo, Real64 fThetaFrom, Real64 fThetaTo, Real64 fPhiFrom, Real64 fPhiTo )
{
    m_fPsiFrom   = fPsiFrom;
    m_fPsiTo     = fPsiTo;
    m_fThetaFrom = fThetaFrom;
    m_fThetaTo   = fThetaTo;
    m_fPhiFrom   = fPhiFrom;
    m_fPhiTo     = fPhiTo;
};


/**
 * Set the ranges for the translational search relative to the centers of the units 
 * (if fXFrom = -20 and fXTo = 20 the units moves -20 and 20 A from the current position)
 * \param fXFrom     upper limit of the X
 * \param fXTo       lower limit of the X
 * \param fYFrom     upper limit of the Y
 * \param fYTo       lower limit of the Y
 * \param fZFrom     upper limit of the Z
 * \param fZTo       lower limit of the Z
 */
template<class T>
void svt_gacylinder<T>::setRelativeTranslSearchRange( Real64 fXFrom, Real64 fXTo, Real64 fYFrom, Real64 fYTo, Real64 fZFrom, Real64 fZTo )
{
    
   
    if ( (fXFrom!=fXTo || fYFrom!=fYTo || fZFrom!=m_fZTo) ) // the values were already initialized
    {
    m_fXFrom    = fXFrom + m_fBorder;
    m_fXTo      = fXTo - m_fBorder;
    m_fYFrom    = fYFrom + m_fBorder;
    m_fYTo      = fYTo - m_fBorder;
    m_fZFrom    = fZFrom + m_fBorder;
    m_fZTo      = fZTo - m_fBorder;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Units, Map, PDB files...
///////////////////////////////////////////////////////////////////////////////
/**
 * set the tempate of the cylinder
 * \param oPdb the target structure
 */
template<class T>
void svt_gacylinder<T>::setTemplate( svt_point_cloud_pdb< svt_ga_vec >  &oPdb)
{
    m_oTurn = oPdb;

    for (unsigned int iIndex=0; iIndex<m_oTemplate.size(); iIndex++)
    {
        m_oTurn.getAtom(iIndex)->setName("C");
        m_oTurn.getAtom(iIndex)->setRemoteness('A');
        m_oTurn.getAtom(iIndex)->adjustMass();
    }
    

    svt_ga_vec oCoa = m_oTurn.coa();

    svt_matrix4<Real64> oMat;
    oMat.loadIdentity();
    oMat.setTranslation( -oCoa );

    m_oTurn= oMat*m_oTurn;    
}

/**
 * set target str
 * only for validation purposes 
 * \param oTarStr the target structure
 */
template<class T>
void svt_gacylinder<T>::setTarStr( svt_point_cloud_pdb< svt_ga_vec >  &oTarStr, bool bComputeAxis)
{
    m_oTarStr = oTarStr;

    if (bComputeAxis)
    {
        svt_point_cloud_pdb< svt_ga_vec > oPdbTarCylinderAxis,oPdbTarCylinder;
        //extract residues in the alpha helices
        int iSize = m_oTarStr.size();
        int iModel = 0, iAdded;
        vector<svt_vector4< Real64 > > oVec; 
        svt_vector4<Real64> oAvg; oAvg.x(0.0f); oAvg.y(0.0f); oAvg.z(0.0f);
        bool bIncOnce = true; 
        vector < Real64 > oCylinderInfo;
        Real64 fDistOnAxis,fAvgDistOnAxis = 0.0;
        unsigned int iCountOnAxis = 0;

        //for the center of the axis
        svt_vector4<Real64> oCenter;
        svt_point_cloud_atom oAtom;
        oAtom.setName("CA ");
        oAtom.setRemoteness('A');

        for (int iIndex=0; iIndex<iSize; iIndex++)
        {
            //if in Helix
            if (m_oTarStr.getAtom(iIndex)->getSecStruct()=='H' || 
                m_oTarStr.getAtom(iIndex)->getSecStruct()=='G' || 
                m_oTarStr.getAtom(iIndex)->getSecStruct()=='I'  )  
            { 
                if ( iIndex-1>=0 && // previous atom is still helix but a different type of helix, then add new model 
                     (m_oTarStr.getAtom(iIndex-1)->getSecStruct()=='H' || m_oTarStr.getAtom(iIndex-1)->getSecStruct()=='G' || m_oTarStr.getAtom(iIndex)->getSecStruct()=='I'    )&&
                     m_oTarStr.getAtom(iIndex-1)->getSecStruct() != m_oTarStr.getAtom(iIndex)->getSecStruct() )
                    bIncOnce = true;

                //increase model number && finish with the last helix ; add to list 
                if (bIncOnce)
                {
                    bIncOnce = false; 

                    if ( (int)oPdbTarCylinderAxis.size() >= m_iMinResInHelix - 3) //  the number of points on the axis
                    {
                        m_oTarStrAxes.append( oPdbTarCylinderAxis, iModel );

                        oCenter = oPdbTarCylinderAxis.coa();
                        m_oTarStrAxesCenters.addAtom( oAtom, oCenter );
                        m_oTarStrAxesCenters.getAtom( m_oTarStrAxesCenters.size()-1)->setModel( iModel );

                        iModel++;   
                    }
                    else  // pop the one that is not long enough
                        if (m_oTarStrAxesInfo.size()>0)
                            m_oTarStrAxesInfo.pop_back( ); 
        
                    oVec.clear();
                    oPdbTarCylinderAxis.deleteAllAtoms();
                    
                    //info
                    oCylinderInfo.clear();
                    oCylinderInfo.push_back(iModel); // the model
                    oCylinderInfo.push_back( 3.0/3.6f ); // the length
                    oCylinderInfo.push_back( 0.0f) ; // number of helices that were identified to correspond to
                    oCylinderInfo.push_back( 3.0 );
                    
                    m_oTarStrAxesInfo.push_back( oCylinderInfo );
                }
               
                //extract cylinder 
                oPdbTarCylinder.addAtom(*m_oTarStr.getAtom(iIndex), m_oTarStr[iIndex] );
                oPdbTarCylinder.getAtom( oPdbTarCylinder.size()-1 )->setModel(iModel);
           
                //compute Avg only if it a CA 
                if (m_oTarStr.getAtom(iIndex)->isCA())
                {
                    m_oTarStrCaInHelix.addAtom( *m_oTarStr.getAtom(iIndex), m_oTarStr[iIndex] );

                    oVec.push_back( m_oTarStr[iIndex] );

                    oAvg.x(0.0f); oAvg.y(0.0f); oAvg.z(0.0f); 
                    iAdded = 0;
                    // compute average on the last 4 CA
                    for ( int iIndex = oVec.size()-1; iIndex>=0 && iIndex >= (int)oVec.size()-4; iIndex--)
                    {
                        oAvg+= oVec[iIndex];
                        iAdded++;
                    }
                    // if 4 elements were present in oVec, then add points to pdb
                    if (iAdded == 4)
                    {
                        oAvg /= (Real64)iAdded;
                        oPdbTarCylinderAxis.addAtom( *oPdbTarCylinder.getAtom( oPdbTarCylinder.size()-1 ), oAvg);
                        if (oPdbTarCylinderAxis.size()>1)
                        {
                            fDistOnAxis = oPdbTarCylinderAxis[oPdbTarCylinderAxis.size()-1].distance( oPdbTarCylinderAxis[oPdbTarCylinderAxis.size()-2] );
                            fAvgDistOnAxis += fDistOnAxis;
                            iCountOnAxis ++;
                        }
                        
                        // increase the lenght of helix
                        m_oTarStrAxesInfo[m_oTarStrAxesInfo.size()-1][1] = m_oTarStrAxesInfo[m_oTarStrAxesInfo.size()-1][1] + 1.0/3.6;
                        m_oTarStrAxesInfo[m_oTarStrAxesInfo.size()-1][3] = m_oTarStrAxesInfo[m_oTarStrAxesInfo.size()-1][3] + 1.0;
                    }
                }
            }
            else 
                bIncOnce = true; 
        }

        if (iCountOnAxis>0)
            fAvgDistOnAxis /= (Real64)iCountOnAxis;

        if ((int)oPdbTarCylinderAxis.size() >=   m_iMinResInHelix - 3) //  4 residues
        {
            m_oTarStrAxes.append( oPdbTarCylinderAxis, iModel );

            oCenter = oPdbTarCylinderAxis.coa();
            m_oTarStrAxesCenters.addAtom( oAtom, oCenter );
            m_oTarStrAxesCenters.getAtom( m_oTarStrAxesCenters.size()-1)->setModel( iModel );
        
            iModel++;
        }
        else  // pop the one that is not long enough
        {
            if (m_oTarStrAxesInfo.size()>1)
                m_oTarStrAxesInfo.pop_back( ); 
        } 

        m_oTarStrAxes.calcAtomModels();
        
        m_iNoOfCylinder2Detect = iModel;
        svtout << "Found " << m_iNoOfCylinder2Detect << " models with average distance on the axis of " << fAvgDistOnAxis << endl;

        m_oTarStrAxesVol = *m_oTarStrAxes.blur(1.0, 4.0);

        //
        // Output results
        //
        if (strlen(m_pPath) != 0)
        {
            char pFname[1256];
            sprintf(pFname, "%s/aaa.pdb", m_pPath );
            m_oTarStr.writePDB( pFname );

            sprintf(pFname, "%s/aaa_CaInHelix.pdb", m_pPath );
            //m_oTarStrCaInHelix.writePDB( pFname );
             
            sprintf(pFname, "%s/axes.pdb", m_pPath );
            m_oTarStrAxes.writePDB( pFname );

            sprintf(pFname, "%s/idealHelix.pdb", m_pPath );
            //m_oIdealHelix.writePDB( pFname ); 
         }
    }
};

/**
 * set coarse target
 * \param oCoarseTarget the coarse version of the target structure
 */
template<class T>
void svt_gacylinder<T>::setCoarseTarget( svt_point_cloud_pdb< svt_ga_vec >  &oCoarseTarget)
{
    m_oCoarseTarget = oCoarseTarget;
};


/**
 * set target
 * \param oTar the map
 */
template<class T>
void svt_gacylinder<T>::setTarget( svt_ga_vol &oTar)
{
    m_oTar = oTar;
    
    m_iSizeX = m_oTar.getSizeX();
    m_iSizeY = m_oTar.getSizeY();
    m_iSizeZ = m_oTar.getSizeZ();
    
    m_oSearchSpace.x( m_iSizeX );
    m_oSearchSpace.y( m_iSizeY );
    m_oSearchSpace.z( m_iSizeZ );
    
    m_fOrigX = m_oTar.getGridX();
    m_fOrigY = m_oTar.getGridY();
    m_fOrigZ = m_oTar.getGridZ();
    
    m_fWidth = m_oTar.getWidth();
    
    m_oModelVol.allocate( m_iSizeX, m_iSizeY, m_iSizeZ, 0.0f );
    m_oModelVol.setGrid( m_fOrigX, m_fOrigY, m_fOrigZ );
    m_oModelVol.setWidth( m_fWidth );   
   
    m_fXFrom    = m_fOrigX + m_fBorder;
    m_fXTo  = m_fOrigX+m_iSizeX*m_fWidth - m_fBorder;
    m_fYFrom    = m_fOrigY + m_fBorder;
    m_fYTo  = m_fOrigY+m_iSizeY*m_fWidth - m_fBorder;
    m_fZFrom    = m_fOrigZ + m_fBorder;
    m_fZTo  = m_fOrigZ+m_iSizeZ*m_fWidth - m_fBorder;
  
    if (m_fRes!=0)
    initKernels();

    //set the exlored volume to null
    m_oExploredVol = m_oTar;
    m_oExploredVol.setValue(0.0);

    //
    // Output input
    //
    char pFname[256];
    if (strlen(m_pPath) != 0)
    {
        sprintf(pFname, "%s/aaa.mrc", m_pPath );
        m_oTar.save( pFname );
    }
};

/**
 * init blurring kernels
 */
template<class T>
void svt_gacylinder<T>::initKernels()
{
    m_oKernel.create1DBlurringKernel(m_fWidth, m_fRes/2.0);
};

/**
 * set the accept Move score percentage:e.g. 0.95 means that moves are accepted if withing 0.95 of the original score 
 * \param the score percentage
 */
template<class T>
void svt_gacylinder<T>::setAcceptMoveRatio(Real64 fAcceptMoveRatio)
{
    m_fAcceptMoveRatio = fAcceptMoveRatio;
};

/**
 * get the accept Move score percentage: e.g. 0.95 means that moves are accepted if withing 0.95 of the original score 
 * \param the score percentage
 */
template<class T>
Real64 svt_gacylinder<T>::getAcceptMoveRatio()
{
    return m_fAcceptMoveRatio;
};


/**
 * set the max number of failed Crawls before stoping the search
 * \param iMaxFailedCrawls e.g. 2 indicates 2 times tried before stoping the crawl
 */
template<class T>
void svt_gacylinder<T>::setMaxFailedCrawls( unsigned int iMaxFailedCrawls)
{
    m_iMaxFailedCrawls = iMaxFailedCrawls;
};

/**
 * get the max number of failed Crawls before stoping the search
 * \return iMaxFailedCrawls e.g. 2 indicates 2 times tried before stoping the crawl
 */
template<class T>
unsigned int svt_gacylinder<T>::getMaxFailedCrawls()
{
    return m_iMaxFailedCrawls;
};

/**
 * how many Cylinders shouls be searched for 
 */ 
template<class T>
void svt_gacylinder<T>::setNoOfCylinder2Detect( int iNoOfCylinder2Detect )
{
    m_iNoOfCylinder2Detect = iNoOfCylinder2Detect;
}; 

/**
 * how many Cylinders shouls be searched for 
 */ 
template<class T>
int svt_gacylinder<T>::getNoOfCylinder2Detect( )
{
    return m_iNoOfCylinder2Detect;
}; 

/**
 * was "valid" cylinder found
 */
template<class T>
void svt_gacylinder<T>::setFoundCylinder( bool bFoundCylinder)
{
    m_bFoundCylinder = bFoundCylinder;
}

/**
 *  was "valid" cylinder found
 */
template<class T>
bool svt_gacylinder<T>::wasFoundCylinder()
{
    return m_bFoundCylinder;
};  

/**
 *  set apply Blurring to model  
 * \param state
 */
template<class T>
void svt_gacylinder<T>::setApplyBlurring2Model( bool bBlurring)
{
    m_bApplyBlurring2Model = bBlurring;
};

/**
 * set the radius of the template 
 * \param the radius of the template
 */
template<class T>
void svt_gacylinder<T>::setTemplateRadius( Real64 fTemplateRadius )
{
    m_fTemplateRadius = fTemplateRadius;
}; 

/**
 * get the radius of the template 
 * \param the radius of the template
 */
template<class T>
Real64 svt_gacylinder<T>::getTemplateRadius()
{
    return m_fTemplateRadius;
}; 

/**
 * set the radius of the template 
 * \param the radius of the template
 */
template<class T>
void svt_gacylinder<T>::setSearchTemplateRadius( Real64 fSearchTemplateRadius )
{
    m_fSearchTemplateRadius = fSearchTemplateRadius;
}; 

/**
 * get the radius of the template 
 * \param the radius of the template
 */
template<class T>
Real64 svt_gacylinder<T>::getSearchTemplateRadius()
{
    return m_fSearchTemplateRadius;
}; 

/**
 * set the number of points in one of the circles of the template
 * \param  number of points in one of the circles of the template
 */
template<class T>
void svt_gacylinder<T>::setTemplatePointCount( unsigned int iTemplatePointCount )
{
    m_iTemplatePointCount = iTemplatePointCount;
}; 

/**
 * get the number of points in one of the circles of the template
 * \param  number of points in one of the circles of the template
 */
template<class T>
unsigned int svt_gacylinder<T>::getTemplatePointCount()
{
    return m_iTemplatePointCount;
}; 

/**
 * set the number of copies of the template
 * \param the number of repeats to be set
 */
template<class T>
void svt_gacylinder<T>::setTemplateRepeats( unsigned int iTemplateRepeats )
{
    m_iTemplateRepeats = iTemplateRepeats;
}; 

/**
 * get the number of copies of the circle in the search template
 * \return the number of repeats
 */
template<class T>
unsigned int svt_gacylinder<T>::getSearchTemplateRepeats()
{
    return m_iSearchTemplateRepeats;
}; 

/**
 * set the number of copies of the circle in the search template
 * \param the number of repeats to be set
 */
template<class T>
void svt_gacylinder<T>::setSearchTemplateRepeats( unsigned int iSearchTemplateRepeats )
{
    m_iSearchTemplateRepeats = iSearchTemplateRepeats;
}; 

/**
 * get the number of copies of the template
 * \return the number of repeats
 */
template<class T>
unsigned int svt_gacylinder<T>::getTemplateRepeats()
{
    return m_iTemplateRepeats;
}; 

/**
 * set the distance between two repeats
 * \param the distance between repeats
 */
template<class T>
void svt_gacylinder<T>::setDistBetweenRepeats( Real64 fDistBetweenRepeats )
{
    m_fDistBetweenRepeats = fDistBetweenRepeats;
}; 

/**
 * get the distance between two repeats
 * \return the distance between repeats
 */
template<class T>
Real64 svt_gacylinder<T>::getDistBetweenRepeats()
{
    return m_fDistBetweenRepeats;
};

/**
 * set the size of a crawling step 
 * \param the size of a crawling step
 */
template<class T>
void svt_gacylinder<T>::setCrawlingStepSize( Real64 fCrawlingStepSize )
{
    m_fCrawlingStepSize = fCrawlingStepSize;
    m_fStepsPerTurn = 5.4/m_fCrawlingStepSize;  
}; 

 /**
 * get the size of a crawling step 
 * \return the size of a crawling step
 */
template<class T>
Real64 svt_gacylinder<T>::getCrawlingStepSize( )
{
    return m_fCrawlingStepSize;
};

/**
 * set fit high resolution helix
 */ 
template<class T>
void svt_gacylinder<T>::setFitHelix( bool bFitHelix )
{
    m_bFitHelix = bFitHelix;
};

/**
 * get fit high resolution helix
 */ 
template<class T>
bool svt_gacylinder<T>::getFitHelix()
{
    return m_bFitHelix;
};
/**
 * set the min number of residues in Helix
 * \params the number of residues
 */ 
template<class T>
void svt_gacylinder<T>::setMinResInHelix( unsigned int iMinResInHelix)
{
    m_iMinResInHelix = iMinResInHelix;
};

/**
 * get the min number of residues in Helix
 * \return the number of residues
 */ 
template<class T>
unsigned int svt_gacylinder<T>::getMinResInHelix()
{
    return m_iMinResInHelix;
};

/**
 * are results available and can be outputed
 */ 
template<class T>
bool svt_gacylinder<T>::canOutputResults()
{
    return m_bCanOutputResults;
}; 

/**
 * set anisotropic correction related parameters
 * \param fAni the correction, e.g. fAni=2 will compress the map 2 times
 * \param fOrigZWoAni the origin of the map before ani correction - use for decompression 
 */ 
template<class T>
void svt_gacylinder<T>::setAniCorr( Real64 fAni, Real64 fOrigZWoAni)
{
    m_fAni = fAni;
    m_fOrigZWoAni = fOrigZWoAni;
};

/**
 * get anisotropic correction 
 * \param fAni the correction, e.g. fAni=2 will compress the map 2 times
 */ 
template<class T>
Real64 svt_gacylinder<T>::getAniCorr()
{
    return m_fAni;
};

/**
 * get Origin on Z axis of the map without anisotropic correction 
 */ 
template<class T>
Real64 svt_gacylinder<T>::getOrigZWoAni()
{
    return m_fOrigZWoAni;
};

/**
 * get Origin on Z axis of the map with anisotropic correction 
 */ 
template<class T>
Real64 svt_gacylinder<T>::getOrigZ()
{
    return m_fOrigZ;
};
  
/**
 * Set the correction factor lambda
 */ 
template<class T>
void svt_gacylinder<T>::setLambda(Real64 fLambda)
{
  m_fLambda = fLambda;
};

/**
 * Get the correction factor lambda
 */ 
template<class T>
Real64 svt_gacylinder<T>::getLambda()
{
  return m_fLambda;
};

/**
* set state: are there any new results to be displayed
*/
template<class T>
void svt_gacylinder<T>::setNewResultsFound(bool bNewResults)
{
    m_bNewResults = bNewResults;
    
};

/**
* return state: are there any new results to be displayed
*/
template<class T>
bool svt_gacylinder<T>::getNewResultsFound()
{
    return m_bNewResults;
};


///////////////////////////////////////////////////////////////////////////////
// Routines to calculate the fitness
///////////////////////////////////////////////////////////////////////////////
/**
 * returns the transformation matrix
 */
template<class T>
svt_ga_mat svt_gacylinder<T>::getTrans(T* pInd)
{
    makeValid(pInd);

    //translation
    svt_ga_vec oTransl;
    oTransl.x( m_fXFrom + pInd->getGene(0) * (m_fXTo - m_fXFrom) );
    oTransl.y( m_fYFrom + pInd->getGene(1) * (m_fYTo - m_fYFrom) );
    oTransl.z( m_fZFrom + pInd->getGene(2) * (m_fZTo - m_fZFrom) );

    //Rotation
    Real64 fPsi     = m_oAngles.getPsi  ( (long unsigned int)(pInd->getGene(3) * m_oAngles.getAngleCount()) );
    Real64 fTheta   = m_oAngles.getTheta( (long unsigned int)(pInd->getGene(3) * m_oAngles.getAngleCount()) );
    Real64 fPhi     = m_oAngles.getPhi  ( (long unsigned int)(pInd->getGene(3) * m_oAngles.getAngleCount()) );
  
    svt_matrix4<Real64> oMat;
    oMat.loadIdentity();
    oMat.rotatePTP( fPhi, fTheta, fPsi); 
    oMat.setTranslation( oTransl );

    pInd->setTrans( oMat );

    return oMat;  
}


/**
 * Updates the coordinates of the model based on the genes of the individual
 * attention: it does not update the volume of the model (see update volume)
 */
template<class T>
void svt_gacylinder<T>::updateModel(T* pInd)
{
    svt_matrix4<Real64> oMat;
    oMat.loadIdentity();

    oMat = getTrans( pInd );

    if (m_bRefining)
    {
        if (m_oModel.size()!=m_oTemplate.size())
            m_oModel = m_oTemplate;

        m_oModel = oMat*m_oTemplate;
    }
    else
    {
        if (m_oModel.size()!=m_oBigTemplate.size())
            m_oModel = m_oBigTemplate;

        m_oModel = oMat*m_oBigTemplate;
    }

    pInd->updateCoarsePhenotype(oMat);
};

/**
 * Updates the volume of the model 
 * Attention: it does not update the model - the pdb remains the same
 */
template<class T>
void svt_gacylinder<T>::updateVolume(T* pInd)
{
    updateModel( pInd );

    //svt_ga_mat oMat = getTrans(pInd);
    m_oModelVol.setValue(0.0f);
    m_oModel.projectMass(&m_oModelVol);

    if (m_bApplyBlurring2Model)
        m_oModelVol.convolve1D3D(m_oKernel, false);
};

/**
 * update fitness
 * \param pInd pointer to individual that gets updated
 */
template<class T>
void svt_gacylinder<T>::updateFitness(T* pInd)
{
    svt_ga_mat oMat; 
    updateModel( pInd );

    Real64 fFitness, fSin;
    fFitness = m_oModel.projectMassCorr(&m_oTar, oMat, false);
    if (m_fLambda>1 || m_fLambda < 0) // the set lambda is not between 0 and 1 
        pInd->setFitness( fFitness );
    else
    {
        // code uses Goldstein convention for Euler angles, this is different from tomo paper where Theta is measured from (X,Y) plane
        // hence we replace cos^2 in tomo paper with sin^2 to give lambda correction
        fSin = sin(m_oAngles.getTheta( (long unsigned int)(pInd->getGene(3) * m_oAngles.getAngleCount()) ));
        pInd->setFitness((m_fLambda + (1-m_fLambda)*fSin*fSin)*fFitness );
    }
 

    this->m_iFitnessUpdateCount++;
}

/**
 * add 2 Cylinder
 */
template<class T>
void svt_gacylinder<T>::add2Tube(T* pInd, bool bAddNew, bool bFlip)
{ 
    if (bAddNew)
    {
        svt_tube oCylinder;
        m_oCylinders.push_back( oCylinder );

        //delete all the tabus in the temporary tabu list 
        m_oTabusInCrawl.clear();
    }
     
    if (m_oCylinders.size()>0)
    {
        svt_ga_mat oMat = pInd->getTrans();
        m_oCylinders[m_oCylinders.size()-1].add2Tube(*pInd, bFlip);
        
    }

    //add to tabu list tabu
    m_oTabusInCrawl.push_back( *pInd );
}


/**
 * get the cylinderes
 * \return the vector containing the tubes 
 */
template<class T>
vector<svt_tube> & svt_gacylinder<T>::getCylinders()
{
    return m_oCylinders;
}; 

/**
 * set the cylinderes
 * \param the vector containing the tubes 
 */
template<class T>
void svt_gacylinder<T>::setCylinders(vector<svt_tube> &  oCylinders)
{
    m_oCylinders = oCylinders;
}; 
/**
 * set the cylinderes
 * \param the vector containing the helices 
 */
template<class T>
void svt_gacylinder<T>::setCylindersPdb(vector< svt_point_cloud_pdb<svt_ga_vec> > & oCylinders)
{
    m_oCylindersPdb = oCylinders;
}; 

/**
 * get the cylinderes
 * \return the vector containing the helices 
 */
template<class T>
vector< svt_point_cloud_pdb<svt_ga_vec> > & svt_gacylinder<T>::getCylindersPdb()
{
    return m_oCylindersPdb;
}; 

/**
 * refine an individual; by default it does nothing - overload in the classes that inherit
 * \param the individual that will be refined
 */
template<class T>
void svt_gacylinder<T>::refineInd(T* pInd)
{
    refine(pInd, 0); 
};

/**
 * refine an individual; by default it does nothing - overload in the classes that inherit
 * \param the individual that will be refined
 * \param iteration
 */
template<class T>
void svt_gacylinder<T>::refine(T* pInd, int iIter)
{
    m_bRefining = true;
    updateFitness( pInd );
    
    Real64 fFitness = pInd->getFitness();

    //refine the rotations randomly
    refineRotRandom(pInd);

    //refine the rotation and translation a few times 
    refineGenes(pInd);

    //refine the rotations randomly
    refineRotRandom(pInd);

    //refine the rotation and translation a few times 
    refineGenes(pInd);
    
    add2Tube( pInd, true );

    //refine the length
    refineInLength( pInd );

    //m_bMask = false; 
    m_bRefining = false;
    updateFitness( pInd );

    if (m_oTabusInCrawl.size()>1)
    {
        // found better fitness while crawling and didn't refine already 2 times => keep refining
        if (m_oTabusInCrawl[0].getFitness() > fFitness && iIter<2 && !this->isInTabuReg( &m_oTabusInCrawl[0] ))
        {
            //get better Ind
            pInd->setGenes( m_oTabusInCrawl[0].getGenes() );

            //delete current cylinder
            m_oCylinders.pop_back();
            m_oTabusInCrawl.clear();
            
            refine( pInd, iIter+1 );
            return;
        }

        sort(m_oTabusInCrawl.rbegin(), m_oTabusInCrawl.rend());
        char pOut[256];
        char pFname[265];
        sprintf(pFname, "%s/Tabu%02d.pdb", m_pPath, this->m_iRun );
        svt_point_cloud_pdb<svt_ga_vec> oPdb; 

        for (unsigned int iIndex=0; iIndex< m_oTabusInCrawl.size(); iIndex++)
        {
            this->m_oTabus.push_back( m_oTabusInCrawl[iIndex] );
            /*oPdb = m_oTabusInCrawl[iIndex].getCoarsePhenotype();
            if (strlen(m_pPath)!=0)
                oPdb.writePDB( pFname, true );*/
        } 
        sprintf(pOut, "[%02d-%04d] Added %d tabu regions. ", this->m_iThread, this->m_iGenerations, (int)m_oTabusInCrawl.size()); svtout << pOut << endl;

        m_bFoundCylinder = true;
   }
    else
    {
        m_bFoundCylinder = false; 
        m_oCylinders.pop_back();
    }

    m_oTabusInCrawl.clear();
    sort(this->m_oTabus.begin(), this->m_oTabus.end());
    this->m_oTabuWindow.clear();
    //done in this generatation
    (*this).m_iGenerations = (*this).m_iMaxGen+1; // stop run at the end of the refinement

};

/**
 * refine the translation and rotation of an individual - calls the refinetransl and refineRot a few times
 * \param the individual that will be refined
 */
template<class T>
void svt_gacylinder<T>::refineGenes(T* pInd, svt_vector4<Real64> *pCenter)
{
    //refine the translation
    if (pCenter==NULL)
        refineTransl( pInd );
    else
        refineTranslOnSphere( pInd, pCenter ); 

    //refine rotation
    refineRot( pInd );
};


/**
 * refine the translation of an individual
 * \param the individual that will be refined
 */
template<class T>
void svt_gacylinder<T>::refineTransl(T* pInd)
{
    char    pFname[1024];
    bool    bFound      = false;
    Real64  fX          = 0;
    Real64  fY          = 0;
    Real64  fZ          = 0;
    Real64  fAddGeneX   = 1.0/(8.0*Real64(m_iSizeX)*m_fWidth);//half angstron
    Real64  fAddGeneY   = 1.0/(8.0*Real64(m_iSizeY)*m_fWidth);
    Real64  fAddGeneZ   = 1.0/(8.0*Real64(m_iSizeZ)*m_fWidth);

    T*      pNewInd = new T(*pInd);
    updateFitness( pNewInd );
    Real64  fMax    = pNewInd->getFitness();

    for (int iIndexX= -this->m_iRefinementMaxMutPerGene*2; iIndexX <= this->m_iRefinementMaxMutPerGene*2; iIndexX++)
    {
        pNewInd->setGene(0, pInd->getGene(0)+fAddGeneX*iIndexX);
        for (int iIndexY= -this->m_iRefinementMaxMutPerGene*2; iIndexY <= this->m_iRefinementMaxMutPerGene*2; iIndexY++)
        {
            pNewInd->setGene(1, pInd->getGene(1)+fAddGeneY*iIndexY);
            for (int iIndexZ= -this->m_iRefinementMaxMutPerGene*2; iIndexZ <= this->m_iRefinementMaxMutPerGene*2; iIndexZ++)
            {
                pNewInd->setGene(2, pInd->getGene(2)+fAddGeneZ*iIndexZ);
                    
                if (pNewInd->getGene(0)<=1 && pNewInd->getGene(0)>=0 && 
                    pNewInd->getGene(1)<=1 && pNewInd->getGene(1)>=0 && 
                    pNewInd->getGene(2)<=1 && pNewInd->getGene(2)>=0)                
                {
                    makeValid(pNewInd);
                    updateFitness(pNewInd); 
                    
                    if (fMax < pNewInd->getFitness())
                    {
                        bFound = true;
                        fMax = pNewInd->getFitness();
                        fX = pNewInd->getGene(0);
                        fY = pNewInd->getGene(1);
                        fZ = pNewInd->getGene(2);
                      
                        if (strlen(m_pPath) != 0 )
                        {
                            sprintf(pFname, "%s/ModelTabuRef%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
                            //m_oModel.writePDB( pFname, true );
                        }
                    }
                }
            }
        }
    }

    if (bFound)  // did found a better score
    {
        pInd->setGene(0, fX);
        pInd->setGene(1, fY);
        pInd->setGene(2, fZ);
        updateFitness( pInd );
    }
   
    delete pNewInd; 
}

/**
 * refine the translation of an individual, only allow movements on Spheres
 * \param the individual that will be refined
 */
template<class T>
void svt_gacylinder<T>::refineTranslOnSphere(T* pInd, svt_vector4<Real64> *pCenter)
{
    char    pFname[1024];
    bool    bFound  = false;
    Real64  fX      = 0;
    Real64  fY      = 0;
    Real64  fZ      = 0;
   
    T*      pNewInd = new T(*pInd);
    updateFitness( pNewInd );
    Real64  fMax    = pNewInd->getFitness();

    svt_ga_vec oTransl;
    oTransl.x( m_fXFrom + pNewInd->getGene(0) * (m_fXTo - m_fXFrom) );
    oTransl.y( m_fYFrom + pNewInd->getGene(1) * (m_fYTo - m_fYFrom) );
    oTransl.z( m_fZFrom + pNewInd->getGene(2) * (m_fZTo - m_fZFrom) );

    svt_ga_vec oAxis    = oTransl - (*pCenter);
    Real64 fR           = oAxis.length();
    Real64 fNewR, fNewTheta, fNewPhi;
    svt_ga_vec oNewCenter;
    
    svt_point_cloud_pdb< svt_ga_vec > oPdb, oCenters;
    svt_point_cloud_atom oAtom;
    oPdb = pNewInd->getCoarsePhenotype();
    oCenters.addAtom(oAtom, *pCenter);
    oCenters.addAtom(oAtom, oPdb[0]);

    Real64 fPsi         = m_oAngles.getPsi  ( (long unsigned int)(pNewInd->getGene(3) * m_oAngles.getAngleCount()) );
    Real64 fTheta       = m_oAngles.getTheta( (long unsigned int)(pNewInd->getGene(3) * m_oAngles.getAngleCount()) );
    Real64 fPhi         = m_oAngles.getPhi  ( (long unsigned int)(pNewInd->getGene(3) * m_oAngles.getAngleCount()) );
  
    svt_ga_mat oMat;
    oMat.loadIdentity();
    oMat.rotatePTP( fPhi, fTheta, fPsi); 

    fNewR = fR; // add 0.5 angstrom
    for (fNewTheta = deg2rad(5.00); fNewTheta <= deg2rad(10.0); fNewTheta+= deg2rad(5.00) )
    {
        //fNewTheta = deg2rad(20.0)*iIndexY; // 20 deg
            for (fNewPhi = 0; fNewPhi < deg2rad(360); fNewPhi+=deg2rad(45.0))
        {
            //fNewPhi = deg2rad(45.0)*iIndexZ; // 45 deg

            oNewCenter.x( fNewR * cos( fNewPhi ) * sin( fNewTheta ) );
            oNewCenter.y( fNewR * sin( fNewPhi ) * sin( fNewTheta ) );
            oNewCenter.z( fNewR * cos( fNewTheta ) );
            
            oNewCenter = *pCenter + oMat*oNewCenter;
     
            pNewInd->setGene(0, (oNewCenter.x()-m_fXFrom )/(m_fXTo - m_fXFrom)  ); 
            pNewInd->setGene(1, (oNewCenter.y()-m_fYFrom )/(m_fYTo - m_fYFrom)  ); 
            pNewInd->setGene(2, (oNewCenter.z()-m_fZFrom )/(m_fZTo - m_fZFrom)  ); 
    
            if (pNewInd->getGene(0)<=1 && pNewInd->getGene(0)>=0 && pNewInd->getGene(1)<=1 && pNewInd->getGene(1)>=0 && pNewInd->getGene(2)<=1 && pNewInd->getGene(2)>=0)
            {

                makeValid(pNewInd);

                updateFitness(pNewInd);

                oPdb = pNewInd->getCoarsePhenotype();
                oCenters.addAtom(oAtom, oPdb[0]);

                if (fMax < pNewInd->getFitness())
                {
                    bFound = true;
                    fMax = pNewInd->getFitness();
                    fX = pNewInd->getGene(0);
                    fY = pNewInd->getGene(1);
                    fZ = pNewInd->getGene(2);
                    if (strlen(m_pPath) != 0 )
                    {
                        sprintf(pFname, "%s/ModelTabuRefCylinder%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
                        //m_oModel.writePDB( pFname, true );
                    }
                }
            }     
        }
    }
   
    if (bFound)  // did found a better score
    {
        pInd->setGene(0, fX);
        pInd->setGene(1, fY);
        pInd->setGene(2, fZ);
        updateFitness( pInd );
    }
   
    delete pNewInd; 
}

/**
 * refine the translation of an individual
 * \param the individual that will be refined
 */
template<class T>
void svt_gacylinder<T>::refineRot(T* pInd, Real64 fMaxCorr)
{
    char pFname[1024];
    bool bFound = false;
    long int iMaxIndex  = -1;

    T*      pNewInd = new T(*pInd);
    updateFitness( pNewInd );
    Real64  fMax    = pNewInd->getFitness();

    svt_ga_vec oVec, oNull;
    oVec.x(0.0f); oVec.y(0.0f); oVec.z(1.0f); 
    oNull.x(0.0f); oNull.y(0.0f); oNull.z(0.0f); 

    svt_ga_mat oMat;
    oMat = pInd->getTrans();
    oMat.setTranslation( oNull );

    svt_ga_vec oInitAxis = oVec * oMat;
    svt_ga_vec oCurrAxis;
    Real64 fDotProd;
    int iCount = 0;

    for (long unsigned int iAngle=0; iAngle<m_oAngles.getAngleCount(); iAngle++)
    {
        Real64 fPsi     = m_oAngles.getPsi  ( iAngle );
        Real64 fTheta   = m_oAngles.getTheta( iAngle );
        Real64 fPhi     = m_oAngles.getPhi  ( iAngle );

        oMat.loadIdentity();
        oMat.rotatePTP( fPhi, fTheta, fPsi); 
        oCurrAxis = oVec * oMat;
 
        fDotProd = oInitAxis.x()*oCurrAxis.x() + oInitAxis.y()*oCurrAxis.y() + oInitAxis.z()*oCurrAxis.z();  

        if (fDotProd > fMaxCorr)
        {
            iCount++;
            if (iCount%2 == 0) // compute fitness every 5 angles
            {
            
                pNewInd->setGene( 3, iAngle/(Real64)m_oAngles.getAngleCount() );
                updateFitness( pNewInd );
     
                if (fMax < pNewInd->getFitness())
                {
                    bFound = true;
                    fMax = pNewInd->getFitness();
                    iMaxIndex = iAngle;
                    if (strlen(m_pPath) != 0 )
                    {
                        sprintf(pFname, "%s/ModelTabuRotBetter%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
                        //m_oModel.writePDB( pFname, true );
                    }
                }
            }
        }    
    }

    if (bFound)
    {
    pInd->setGene( 3, iMaxIndex/(Real64)m_oAngles.getAngleCount() );
    updateFitness(pInd);
    }

    delete pNewInd;
};

/**
 * refine the rotation of an individual using random angles
 * \param the individual that will be refined
 */
template<class T>
void svt_gacylinder<T>::refineRotRandom(T* pInd)
{
    char pFname[1024];
    bool bFound = false;
    Real64 fGene = 0;

    T*      pNewInd = new T(*pInd);
    updateFitness( pNewInd );
    Real64  fMax    = pNewInd->getFitness();

    if (strlen(m_pPath) != 0 )
    {
        sprintf(pFname, "%s/ModelTabuRotBetter%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
        //m_oModel.writePDB( pFname, true );
    }

    //randomly sample 10% of the angles
    for (long unsigned int iAngle=0; iAngle<m_oAngles.getAngleCount()/*0.10*/; iAngle++)
    {
        //pNewInd->setGene( 3, svt_genrand() );
        pNewInd->setGene( 3, iAngle/Real64(m_oAngles.getAngleCount()) );
        updateFitness( pNewInd );

        if (fMax < pNewInd->getFitness())
        {
            bFound = true;
            fMax = pNewInd->getFitness();
            fGene = pNewInd->getGene(3);

            if (strlen(m_pPath) != 0 )
            {
                sprintf(pFname, "%s/ModelTabuRotBetter%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
                //m_oModel.writePDB( pFname, true );
            }
        }
    }
 
   if (bFound)
    {
    pInd->setGene( 3, fGene );
    updateFitness(pInd);
        
        if (strlen(m_pPath) != 0 )
        {
            sprintf(pFname, "%s/ModelTabuRotBetter%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
            //m_oModel.writePDB( pFname, true );
        }
 

    }

    delete pNewInd;
};

/**
 * crawl on the tube and search for similar scoring cylinder placements
 * \param the individual that will be refined
 * \param iDirection indicates the direction  +1 for forward and -1 for backwards
 */
template<class T>
void svt_gacylinder<T>::crawl(T* pInd, int iDirection)
{
    //should the elements in the list of accepted step should be flipped
    //by default no, except when changing direction
    //char pOut[1024];
    bool bFlip = false;

    if (iDirection == -1)
        bFlip = true;

    //char pOut[256];
    if (iDirection != 1 && iDirection != -1)
    {
    svtout << "The direction can be either +1 (forward) or -1 (backwards)" << endl;
    return;
    }

    char pFname[1024];
    svt_point_cloud_pdb< svt_vector4<Real64> > oPdb; 
    T*      pNewInd         = new T(*pInd);
    updateFitness( pNewInd );
    Real64  fStartScore         = pNewInd->getFitness();
 
    //
    // Move on the cylinder in the direction indicated by the axis of the start cylinder
    //
    //center of the start cylinder
    svt_ga_vec oTransl;
    oTransl.x( m_fXFrom + pNewInd->getGene(0) * (m_fXTo - m_fXFrom) );
    oTransl.y( m_fYFrom + pNewInd->getGene(1) * (m_fYTo - m_fYFrom) );
    oTransl.z( m_fZFrom + pNewInd->getGene(2) * (m_fZTo - m_fZFrom) );

    svt_vector4<Real64> oOldTrans;

    int     iNoFailureToImprove = 0; // number of steps that failed to improve; counted since last improvement 
    while (iNoFailureToImprove<(int)m_iMaxFailedCrawls)
    {
       
        //translate in the direction of the axes of the previous cylinder
        oPdb = pNewInd->getCoarsePhenotype();   
        if (iDirection==1)
            oOldTrans = oTransl;
        else
            oOldTrans = oTransl + 2*iDirection * m_fCrawlingStepSize * (oPdb[1]-oPdb[0]);

        //oTransl - is the translation of the previous cylinder (before refinement) 
        oTransl = oTransl + iDirection * m_fCrawlingStepSize * (oPdb[1]-oPdb[0]);

        pNewInd->setGene(0, (oTransl.x()-m_fXFrom )/(m_fXTo - m_fXFrom)  ); 
        pNewInd->setGene(1, (oTransl.y()-m_fYFrom )/(m_fYTo - m_fYFrom)  ); 
        pNewInd->setGene(2, (oTransl.z()-m_fZFrom )/(m_fZTo - m_fZFrom)  ); 
         
        if (pNewInd->getGene(0)<=1 && pNewInd->getGene(0)>=0 && 
            pNewInd->getGene(1)<=1 && pNewInd->getGene(1)>=0 && 
            pNewInd->getGene(2)<=1 && pNewInd->getGene(2)>=0)
        {
            //refine rotation and translation
            refineGenes(pNewInd, &oOldTrans);
            updateFitness( pNewInd );

            // is the score good enough - then make tabu
            if (fStartScore*m_fAcceptMoveRatio < pNewInd->getFitness()) //restart the stop counter
                iNoFailureToImprove = 0;
            else //inc the no of failure
                iNoFailureToImprove++;

            //a good step or the first failure then add to cylinder
            if (iNoFailureToImprove<=1)
            {
                if (!bFlip) 
                    add2Tube( pNewInd );
                else
                {
                    add2Tube( pNewInd, false, bFlip );
                    bFlip = false;
                }

                //print
                if (strlen(m_pPath) != 0 )
                {
                    sprintf(pFname, "%s/Model%02d%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread,  (int)this->m_oPop.size() -1 );
                    //m_oModel.writePDB( pFname, true );
                }
            }
            else
            {
                //fail- remove the last two points that were added as they constitue failures 
                if (m_oCylinders.size()>1)
                {
                    m_oCylinders[m_oCylinders.size()-1].pop();
                    m_oCylinders[m_oCylinders.size()-1].pop();
                }
            }
        }
        else // i'M outside map and should stop here
        {
            iNoFailureToImprove = m_iMaxFailedCrawls;
        }
    }
    delete pNewInd;
} 

/**
 * refine the length of an individual
 * \param the individual that will be refined
 */
template<class T>
void svt_gacylinder<T>::refineInLength(T* pInd)
{
    if (strlen(m_pPath) != 0 )
    {
        char pFname[1256];
        updateFitness( pInd );
        sprintf(pFname, "%s/Model%02d%02d99.pdb", m_pPath, this->m_iRun, this->m_iThread );
        //m_oModel.writePDB( pFname, true );
    }

    // crawl forward
    T* pNewInd = new T(*pInd);
    crawl(pNewInd, +1);
    delete pNewInd;

    //crawl backworks
    pNewInd = new T(*pInd);
    crawl(pNewInd, -1);
    delete pNewInd;

}

/**
 * Calculate the full correlation corresponding to a certain individual (with blur)
 */
template<class T>
Real64 svt_gacylinder<T>::getCorrelation()
{
    m_oModelVol.convolve1D3D(m_oKernel, false ); // don't normalize
    return m_oModelVol.correlation(m_oTar, false);
}

/**
 * Calculate the rmsd of the individual towards the target model
 * \return the rmsd 
 */
template<class T>
Real64 svt_gacylinder<T>::getRMSD()
{
    if (m_oTarStr.size()>0)
    {
        Real64 fRMSD = m_oModel.rmsd( m_oTarStr, false, ALL, false );   
        return fRMSD;
    }
    else
        return 0;
    
}
///////////////////////////////////////////////////////////////////////////////
// Mutation
///////////////////////////////////////////////////////////////////////////////

/**
 * custom mutation (can be changed by derived class, default implementation just calls mutationBGA)
 * \param iInd index of individual
 */
template<class T>
void svt_gacylinder<T>::mutationCustom(int iInd)
{    
    Real64 fRand = svt_genrand();
    if (fRand < 0.30)
        svt_ga<T>::mutationCauchy( iInd );
    else
        if (fRand < 0.60)
            svt_ga<T>::mutationMultiCauchy(iInd);
        else
            if (fRand < 0.90)
                svt_ga<T>::mutationRandom(iInd);
            else
                mutationAllCauchy(iInd);
};

/**
 * mutation with a cauchy distribution
 * \param iInd index of individual
 */
template<class T>
void svt_gacylinder<T>::mutationCauchy(int iInd, int iRandIndex, Real64 fRatio)
{
    
    Real64 fNewGene, fIntPart, fRand;
        
    fRand = svt_ranCauchy(0.0, this->m_fMutationOffset*fRatio);

    fRand = modf(fRand, &fIntPart);
    fNewGene = this->m_oNextPop[iInd].getGene( iRandIndex) + fRand;

    //bring the gene back into the 0 - 1 range
    this->m_oNextPop[iInd].setGene( iRandIndex, fNewGene );

    makeValid(&this->m_oNextPop[iInd]);
};

/**
 * mutation
 */
template<class T>
void svt_gacylinder<T>::mutationAllCauchy(int iInd)
{
    for (unsigned int iIndex = 0; iIndex<(unsigned int)this->m_iGenes; iIndex++ )
        mutationCauchy(iInd, iIndex);
}

/**
 * Compute Average Score of all solutions
 */ 
template<class T>
void svt_gacylinder<T>::computeAvgScoreAll()
{
    unsigned int iCount = 0;
    m_fAvgScoreAll = 0;

    for (unsigned int iIndex=0; iIndex < m_oCylinders.size(); iIndex++)
    {
        m_fAvgScoreAll += m_oCylinders[iIndex].getAvgScore()*m_oCylinders[iIndex].size();
        iCount += m_oCylinders[iIndex].size();
    }
    
    if (iCount>0)
        m_fAvgScoreAll /= (Real64)iCount;

}  
///////////////////////////////////////////////////////////////////////////////
// Output statistics, result files, etc
///////////////////////////////////////////////////////////////////////////////

/**
 * output result
 */
template<class T>
void svt_gacylinder<T>::outputResult(bool bTabuAdded)
{
    char pFname[256], pOut[256];
    if (m_iWriteModelInterval != 0 && this->m_iGenerations%m_iWriteModelInterval==0 )
    {
        if ((int)this->m_oPop.size()==this->m_iPopSize)
        { 
            svt_ga<T>::sortPopulation();
            int iSize = 1;// this->m_oPop.size();
            for (int i=0; i < iSize; i++)
            {
                updateModel( &this->m_oPop[ this->m_oPop.size()-i-1 ] );

                sprintf(pOut, "[%02d-%02d-%04d] %8.6f  - ", this->m_iRun, this->m_iThread, this->m_iGenerations, this->m_oPop[ this->m_oPop.size() - i - 1 ].getFitness());
                svtout << pOut;
                this->m_oPop[ this->m_oPop.size()- i - 1 ].printGenes() ;

                if (strlen(m_pPath) != 0 && this->m_iWriteModelInterval != 0 && this->m_iGenerations%this->m_iWriteModelInterval ==0 )
                {
                    sprintf(pFname, "%s/Model%02d%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread, (int)this->m_oPop.size() - i - 1 );
                    //m_oModel.writePDB( pFname, true );
                }
            }
        }
    }

   
    
    svt_ga<T>::sortPopulation();
    int iSize = this->m_oPop.size();
    svt_point_cloud_pdb<svt_ga_vec> oPdb;
    svt_point_cloud_atom oAtom;
    for (int i=0; i < iSize; i++)
    {
    if (this->m_oPop[ this->m_oPop.size()-i-1 ].getOrigin()==TABU)
    {
        svtout << "fitness is 0" << endl;
        oPdb.addAtom(oAtom,this->m_oPop[ this->m_oPop.size()-i-1 ].getCoarsePhenotype()[0] );
     }
    }

    if (strlen(m_pPath) != 0 && oPdb.size()>0)
    {
        sprintf(pFname, "%s/ModelDiscard%02d%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread, this->m_iGenerations );
    //  oPdb.writePDB( pFname, true );
    }

    
    if (bTabuAdded)
    {

/*    updateModel(&this->m_oTabus[ this->m_oTabus.size() - 1] );

      if (strlen(m_pPath) != 0 )
      {
        sprintf(pFname, "%s/Tabu%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
        //m_oModel.writePDB( pFname, true );
      }
*/
/*    updateModel(&this->m_oPop[ this->getBestTabu()] );
      if (m_oTarStr.size() > 0 )
      {
        sprintf(pOut, "[%02d-%02d-%04d] %8.6f  - %8.3f %8.3f %8.3f  ", this->m_iRun, this->m_iThread, this->m_iGenerations, this->getBestTabu().getFitness(), getRMSD(), m_fSpringPotential, m_fMaxDistDev);
        svtout << pOut << endl;
      }

      if (strlen(m_pPath) != 0 )
      {
        sprintf(pFname, "%s/BestTabu%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
      }

*/
    }

}

/**
 * update result
 */
template<class T>
void svt_gacylinder<T>::updateResults(unsigned int iNoOfTubes, int iNum )
{ 
    svt_point_cloud_pdb<svt_ga_vec>  oBadTubes;  
    int iIndex=0;

    svt_tube oCylinder;
    
    char pStatus[256];
    svt_point_cloud_pdb<svt_ga_vec>  oPdb, oAllCylinder, oAllCylinderAniDecorr, oTPCylinder, oRestCylinder, oAllHRCylinder, oPdbHel;
    //iNoOfTubes was not set; then show all
    if (iNoOfTubes==0)
    {
        iNoOfTubes = m_oCylindersPdb.size();
        sprintf(pStatus, "Partial1");
    }
    else
        sprintf(pStatus, "Final1");
       
    Real64 fTurnsOff = 0; 
    Real64 fPercentCovered = 0; 
    unsigned int iFalsePositive=0, iFalseNegative=0, iTruePositive = 0, iTrueNegative =0, iDetectedMoreThanOnce = 0;
    unsigned int iCountCylinder=0;
    char pOut[1256];
    vector<svt_gacylinder_ind> oInds;

    sprintf(pOut, "Index Points Length (in A) Turns  Score\n");
    svtout << pOut ; 

    for (iIndex=0; iIndex < (int)iNoOfTubes && iIndex < (int)m_oCylindersPdb.size(); iIndex++)
    {
        sprintf(pOut, "%3d:  %3d   %6.3f %6.3f %8.6f ", iIndex, m_oCylinders[iIndex].size(), m_oCylinders[iIndex].getLength(), m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn, m_oCylinders[iIndex].getScores()[2]);
        svtout << pOut ;    

    //sprintf(pOut, "%3d: %3d %6d %6.3f %8.6f %8.6f %8.6f %8.6f %5.2f ", iIndex, (int)m_oCylinders[iIndex].getScores()[3], m_oCylindersPdb[iIndex].size(), m_oCylinders[iIndex].getLength(), m_oCylinders[iIndex].getAvgScore(), m_oCylinders[iIndex].getScores()[0],m_oCylinders[iIndex].getScores()[1], m_oCylinders[iIndex].getScores()[2], m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn );
     //   svtout << pOut ;    
        /*sprintf(pOut, "%3d: %3d %6.3f %8.6f %8.6f ", iIndex, m_oCylindersPdb[iIndex].size(), m_oCylinders[iIndex].getLength(),m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn, m_oCylinders[iIndex].getAvgScore() );
        svtout << pOut ;    
        for (int iInd = 1; iInd < 14; iInd++ )
           cout << m_oCylinders[iIndex].getScores()[iInd]  << " " ;
      */
        oPdb = m_oCylindersPdb[iIndex];
        //oInds = m_oCylinders[iIndex].getElements();
        if (oPdb.size()>0) 
        {
            svt_ga_vec oVec;
            vector<Real64> oVecMinDist;
            vector<int> oVecModel;
            for (unsigned int i=0; i< oPdb.size(); i++)
            {
                oVecMinDist.push_back(1e10);
                oVecModel.push_back(-1);
            
                oVec = oPdb[i];
                Real64 fDist;
                for (unsigned int iAtom=0; iAtom< m_oTarStrAxes.size(); iAtom++)
                {   
                    fDist = oVec.distance(m_oTarStrAxes[iAtom]);
                    if (fDist < oVecMinDist[i])
                    {
                        oVecMinDist[i] = fDist;
                        oVecModel[i] = m_oTarStrAxes.getAtom(iAtom)->getModel();
                    }    
                }
        
                if (oVecMinDist[i] > m_fMaxDist4Map)
                    oVecModel[i] = -1;

                //sprintf(pOut, "%3d %8.6f %2d %8.6f %3d %d", iIndex,m_oCylinders[iIndex].getAvgScore(), i, oInds[i].getFitness(), oVecModel[i], oVecModel[i]>0    );
                //svtout << pOut << endl;;
            }
            
            sort(oVecModel.rbegin(),oVecModel.rend());
            int i= oVecModel.size()-1;
            while (i>=0 && oVecModel[i] == -1)
            {
                oVecModel.pop_back();
                i--;
            }

            unsigned int iNum, iMaxNum = 0;
            int iModel = -1; 
            for (unsigned int i=0; i< oVecModel.size(); i++)
            {
                if ( (i==0) || (i>0 && oVecModel[i]!=oVecModel[i-1]) )
                {
                    iNum = 0;
                    for (unsigned int j=0; j< oVecModel.size(); j++)
                        if (oVecModel[j] == oVecModel[i])
                            iNum++;
                    
                    if (iNum > iMaxNum)
                    {
                        iMaxNum = iNum;
                        iModel  = oVecModel[i];
                    }
                }
            }
            if (iMaxNum<4) // not enough points correspond to each other 
                iModel = -1;
    
            Real64 fMinDist = iMaxNum;

            if ( iModel == -1 ) //matching helix not found
            {
                iFalsePositive ++;
                cout << endl;
            }
            else
            {
                int iCylinderInTar = -1;
                for (int i=0; i< (int)m_oTarStrAxesInfo.size(); i++)
                {   
                    if ( m_oTarStrAxesInfo[i][0] == iModel)
                    {
                        m_oTarStrAxesInfo[i][2] =  m_oTarStrAxesInfo[i][2] + 1;
                        iCylinderInTar = i;
                    }
                }
                
                if (iCylinderInTar != -1) // found
                { 
                    fPercentCovered += m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn > m_oTarStrAxesInfo[iCylinderInTar][1] ? 100 :  (100.0*m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn) / m_oTarStrAxesInfo[iCylinderInTar][1] ;
                    fTurnsOff +=  abs(m_oTarStrAxesInfo[iCylinderInTar][1] - m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn);

                    sprintf(pOut," --- %8.3f %8.3f %8.3f %8.3f %4.0f %4.2f ", fMinDist, m_oTarStrAxesInfo[iCylinderInTar][0], m_oTarStrAxesInfo[iCylinderInTar][1], (100.0*m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn )/ m_oTarStrAxesInfo[iCylinderInTar][1], m_oTarStrAxesInfo[iCylinderInTar][3], abs(m_oTarStrAxesInfo[iCylinderInTar][1] - m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn) ); 
                    cout << pOut << endl;
                    iCountCylinder ++; 
                    for (unsigned int iAtom=0; iAtom<oPdb.size(); iAtom++)
                    {
                        oPdb.getAtom(iAtom)->setModel(iIndex);
                        oTPCylinder.addAtom( *oPdb.getAtom(iAtom), oPdb[iAtom] );
                    } 
                }
                else
                    cout << endl;  
           } 
        }
        else
            cout << endl;  

        for (unsigned int iAtom=0; iAtom<oPdb.size(); iAtom++)
        {
            oPdb.getAtom(iAtom)->setModel(iIndex);
            oAllCylinder.addAtom( *oPdb.getAtom(iAtom), oPdb[iAtom] );
            //ani correction : decompress the helices
            if (m_fAni != 1)
            {
                oPdb[iAtom].z( m_fOrigZWoAni + m_fAni*(oPdb[iAtom].z() - m_fOrigZ) );
                oAllCylinderAniDecorr.addAtom( *oPdb.getAtom(iAtom), oPdb[iAtom] );
            }
        }


        oPdbHel = m_oCylinders[iIndex].getHRTube( );
        for (unsigned int iAtom=0; iAtom<oPdbHel.size(); iAtom++)
        {
            oPdbHel.getAtom(iAtom)->setModel(iIndex);
            oAllHRCylinder.addAtom( *oPdbHel.getAtom(iAtom), oPdbHel[iAtom] );
        }

    }
    fPercentCovered /= (Real64)iCountCylinder; //average by the no of helices       
    fTurnsOff /= (Real64)iCountCylinder;

    //compute rates - false positive, false negatives  
    for (unsigned int i=0; i< m_oTarStrAxesInfo.size(); i++)
    {   
        if ( m_oTarStrAxesInfo[i][2] == 0) // cylinder i was not detected
        {
            svtout << "Cylinder "<< i<< " was not detected !" << endl;
            //iFalseNegative ++;
            
            bool bFound = false;
            //search again but this time from the center of the target axis
            unsigned int iCyl;
            for (iCyl=0; iCyl < m_oCylindersPdb.size() && iCyl < iNoOfTubes; iCyl++)
            {
                oPdb = m_oCylindersPdb[iCyl];
                for (unsigned int iAtom=0; iAtom< oPdb.size(); iAtom++)
                {
                    if (m_oTarStrAxesCenters[i].distance( oPdb[iAtom] ) <  m_fMaxDist4Map)
                    {
                        sprintf(pOut, " again---- %3d %6d %4.2f ", iCyl, m_oCylindersPdb[iCyl].size(), m_oCylindersPdb[iCyl].size()/m_fStepsPerTurn);
                        cout << pOut << endl;

                        bFound = true;
                        
                        iAtom = oPdb.size(); // exit with this condition from looking at this helix
                        iCyl = m_oCylindersPdb.size(); // completely exit out of the loop; just interested in the first top scoring helix
                    }
                }
            }
            
            if (bFound && iCyl < iNoOfTubes)
            {
                //iFalseNegative --;
                iTruePositive ++;
            }  
        }

        if ( m_oTarStrAxesInfo[i][2] >= 1) // cylinder i was detected two times
            iTruePositive ++;

        if ( m_oTarStrAxesInfo[i][2] > 1) // cylinder i was detected two times
        {
            svtout << "Cylinder "<< i<< " was detected  two times!" << endl;
            iDetectedMoreThanOnce++;
        }
    }

    svtout << "Remaining Tubes" << endl;
    //the remaning Tubes
    for (iIndex=iNoOfTubes;iIndex < (int)m_oCylindersPdb.size(); iIndex++)
    {
        sprintf(pOut, "%3d:  %3d   %6.3f   %8.6f ", iIndex, m_oCylinders[iIndex].size(), m_oCylinders[iIndex].getLength(), m_oCylinders[iIndex].getScores()[2]);
        svtout << pOut ;    
        //sprintf(pOut, "%3d: %3d %6d %8.6f %8.6f %8.6f %8.6f %5.2f ", iIndex, (int)m_oCylinders[iIndex].getScores()[3], m_oCylindersPdb[iIndex].size(),  m_oCylinders[iIndex].getAvgScore(), m_oCylinders[iIndex].getScores()[0],m_oCylinders[iIndex].getScores()[1], m_oCylinders[iIndex].getScores()[2],  m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn); 
        //svtout << pOut ; 
        //oPdb = m_oCylinders[iIndex].getCylinder(&oTemplate);
       /*sprintf(pOut, "%3d: %3d %6.3f %8.6f %8.6f ", iIndex, m_oCylindersPdb[iIndex].size(), m_oCylinders[iIndex].getLength(),m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn, m_oCylinders[iIndex].getAvgScore() );
        svtout << pOut ;    
        for (int iInd = 1; iInd < 14; iInd++ )
           cout << m_oCylinders[iIndex].getScores()[iInd] << " " ;
    */
        oPdb = m_oCylindersPdb[iIndex];
        if (oPdb.size()>0) 
        {
            svt_ga_vec oVec;
            vector<Real64> oVecMinDist;
            vector<int> oVecModel;
            for (unsigned int i=0; i< oPdb.size(); i++)
            {
                oVecMinDist.push_back(1e10);
                oVecModel.push_back(-1);
            
                oVec = oPdb[i];
                Real64 fDist;
                for (unsigned int iAtom=0; iAtom< m_oTarStrAxes.size(); iAtom++)
                {   
                    fDist = oVec.distance(m_oTarStrAxes[iAtom]);
                    if (fDist < oVecMinDist[i])
                    {
                        oVecMinDist[i] = fDist;
                        oVecModel[i] = m_oTarStrAxes.getAtom(iAtom)->getModel();
                    }    
                }
        
                if (oVecMinDist[i] > m_fMaxDist4Map)
                    oVecModel[i] = -1;
            }
            
            sort(oVecModel.rbegin(),oVecModel.rend());
            int i= oVecModel.size()-1;
            while (i>=0 && oVecModel[i] == -1)
            {
                oVecModel.pop_back();
                i--;
            }

            unsigned int iNum, iMaxNum = 0;
            int iModel = -1; 
            for (unsigned int i=0; i< oVecModel.size(); i++)
            {
                if ( (i==0) || (i>0 && oVecModel[i]!=oVecModel[i-1]) )
                {
                    iNum = 0;
                    for (unsigned int j=0; j< oVecModel.size(); j++)
                        if (oVecModel[j] == oVecModel[i])
                            iNum++;
                    
                    if (iNum > iMaxNum)
                    {
                        iMaxNum = iNum;
                        iModel  = oVecModel[i];
                    }
                }
            }
            if (iMaxNum<4) // not enough points correspond to each other 
                iModel = -1;
    
            Real64 fMinDist = iMaxNum;

            if (iModel == -1) //too far to be considered an cylinder
            {
                iTrueNegative++;
                cout << endl;
            }
            else
            {
                int iCylinderInTar = -1;
                for (int i=0; i< (int)m_oTarStrAxesInfo.size(); i++)
                {   
                    if ( m_oTarStrAxesInfo[i][0] == iModel)
                    {
                        m_oTarStrAxesInfo[i][2] =  m_oTarStrAxesInfo[i][2] + 1;
                        iCylinderInTar = i;
                    }
                }
                
                if (iCylinderInTar != -1) // found
                { 
                    sprintf(pOut," --- %8.3f %8.3f %8.3f %8.3f %4.0f %4.2f", fMinDist, m_oTarStrAxesInfo[iCylinderInTar][0], m_oTarStrAxesInfo[iCylinderInTar][1], (100.0*m_oCylindersPdb[iIndex].size()/m_fStepsPerTurn )/ m_oTarStrAxesInfo[iCylinderInTar][1], m_oTarStrAxesInfo[iCylinderInTar][3], abs(m_oTarStrAxesInfo[iCylinderInTar][1] - m_oCylindersPdb[iIndex].size()/ m_fStepsPerTurn)); 
                    cout << pOut << endl;
                    iFalseNegative++;
                }
                else
                    cout << endl;  
           } 
        }
        else
            cout << endl;  
        

        for (unsigned int iAtom=0; iAtom<oPdb.size(); iAtom++)
        {
            oPdb.getAtom(iAtom)->setModel(iIndex);
            oRestCylinder.addAtom( *oPdb.getAtom(iAtom), oPdb[iAtom] );
        }
    }

    oAllCylinder.calcAtomModels();
    oRestCylinder.calcAtomModels();
    oTPCylinder.calcAtomModels();

    if (strlen(m_pPath)!=0)
    {    
        char pFname[256];
       
        if (m_fAni != 1  )
        {
            sprintf( pFname, "%s/DetectedCylinder%d_%02d.pdb", m_pPath, iNum, this->m_iRun);
            if (oAllCylinderAniDecorr.size() > 0)
                oAllCylinderAniDecorr.writePDB( pFname );
 
            sprintf( pFname, "%s/DetectedCylinderAni%d_%02d.pdb", m_pPath, iNum, this->m_iRun);
            if (oAllCylinder.size()>0)
                oAllCylinder.writePDB( pFname );
        }else
        {
            sprintf( pFname, "%s/DetectedCylinder%d_%02d.pdb", m_pPath, iNum, this->m_iRun);
            if (oAllCylinder.size()>0)
                oAllCylinder.writePDB( pFname );
        }


        
        sprintf( pFname, "%s/DetectedHelix%d_%02d.pdb", m_pPath, iNum, this->m_iRun);
        if (oAllHRCylinder.size()>0)
            oAllHRCylinder.writePDB( pFname );
      
        sprintf( pFname, "%s/DiscardedCylinder%d_%02d.pdb", m_pPath, iNum, this->m_iRun);
        //if (oRestCylinder.size()>0)
        //    oRestCylinder.writePDB( pFname );

        sprintf( pFname, "%s/CorrectTubes%d_%02d.pdb", m_pPath, iNum, this->m_iRun);
      //  if (oTPCylinder.size()>0)
      //      oTPCylinder.writePDB( pFname );

        sprintf( pFname, "%s/BadCylinder%d_%02d.pdb", m_pPath, iNum, this->m_iRun);
        if (oBadTubes.size()>0)
            oBadTubes.writePDB( pFname );
    }

    svt_ga_vol oMapDetectedCylinder;
    if (oTPCylinder.size()>0)
        oMapDetectedCylinder = *oTPCylinder.blur(1.0, 4.0);

    m_fExploredPercent = m_oTar.correlation(m_oExploredVol)*100 ;

    //cout <<  " FName  Re    TH      TP      FN      FP      MoreThanOnce    Cor   Covered" << endl;
    sprintf(pOut, "%s%02d Res: %4.1f %3.1f %2d %2d %2d %2d %2d %2d %2d %2d %2d %5.3f %6.3f %5.3f %6.3f %s", pStatus, iNum, m_fRes, m_fWidth, this->m_iParallelRun,  (int)m_oTarStrAxesInfo.size(), (int) m_iNoOfCylinder2Detect, (int)m_oCylindersPdb.size(), iTruePositive , iFalsePositive, iTrueNegative, iFalseNegative, iDetectedMoreThanOnce, oMapDetectedCylinder.correlation( m_oTarStrAxesVol, false) , fPercentCovered,  fTurnsOff , m_fExploredPercent, m_pRmsdOut  );
    cout << pOut << endl;
    //cout << pStatus << " Res:   " << " " <<  m_fRes << " "  << m_fWidth << " " << this->m_iParallelRun << " " << m_oTarStrAxesInfo.size() << " " <<  m_oCylinders.size() << " "  << iTruePositive << " " << iFalseNegative << " " << iFalsePositive <<  " " << iDetectedMoreThanOnce << " " << oMapDetectedCylinder.correlation( m_oTarStrAxesVol, false) <<  " " << fPercentCovered << " " <<  fTurnsOff << " " << m_fExploredPercent <<  endl;
       
    for (unsigned int iIndex=0; iIndex < m_oTarStrAxesInfo.size(); iIndex++)
        m_oTarStrAxesInfo[iIndex][2] = 0.0;
};
 
/**
 * output the best model
 */
template<class T>
void svt_gacylinder<T>::outputBest()
{
    svt_ga<T>::sortPopulation();

    updateFitness( &this->m_oPop[ this->m_oPop.size() - 1] );
    
    Real64 fRmsd =  m_oModel.rmsd( m_oTarStr, false, ALL,  false );
    char pOut[1024];
    sprintf(pOut,  "Best individual at generation %04d has CC %15.8f - RMSD: %10.8f CC_after_blurring: ", this->m_iGenerations-1, this->m_oPop[ this->m_oPop.size() - 1].getFitness(), fRmsd);
    svtout << pOut;
    
    m_oModelVol.convolve1D3D(m_oKernel, false ); // don't normalize
    Real64 fCC = m_oModelVol.correlation(m_oTar, false);
    
    sprintf(pOut , "%6.5f\n", fCC);
    cout << pOut;
    if (strlen(m_pPath) != 0)
    {
    char pFname[256];
    sprintf(pFname, "%s/BestModel%02d%02d_%04d.pdb", m_pPath,this->m_iRun, this->m_iThread, this->m_iGenerations-1 );
    //m_oModel.writePDB( pFname );
    }
}

/**
 * print results (to cout)
 */
template<class T>
void svt_gacylinder<T>::printResults()
{
    for(int i=this->m_oPop.size()-1; i>=0; i--)
    {
        printf("[%3i] = %1d %1d %8.3f", i, this->m_oPop[i].getOrigin(), this->m_oPop[i].getAge(), this->m_oPop[i].getProp() );
    this->m_oPop[i].printGenes();
    }
}

/**
 * Set the output path path
 * \param pPath pointer to array of char
 */
template<class T>
void svt_gacylinder<T>::setOutputPath( const char *pPath )
{
    strcpy(m_pPath, pPath);
};

/**
 * Get the output path path
 * \param pPath pointer to array of char
 */
template<class T>
const char * svt_gacylinder<T>::getOutputPath( )
{
    return m_pPath;
};

/**
 * How many generations should gacylinder wait until it outputs a new model file (0 turns output off)
 * \param iWriteModelInterval number of generations
 */
template<class T>
void svt_gacylinder<T>::setWriteModelInterval( unsigned int iWriteModelInterval )
{
    m_iWriteModelInterval = iWriteModelInterval;
};
/**
 * How many generations should gacylinder wait until it outputs a new model file (0 turns output off)
 * \return number of generations
 */
template<class T>
unsigned int svt_gacylinder<T>::getWriteModelInterval( )
{
    return m_iWriteModelInterval;
};

/**
 * output the configuration of the program:w
 *
 */
template<class T>
void svt_gacylinder<T>::writeConfiguration(char *pFnameParam)
{
    svt_ga<T>::writeConfiguration(pFnameParam);
    
    FILE* pFileParam = fopen( pFnameParam, "a" );

    fprintf( pFileParam, "Resolution = %f\n",                   m_fRes );    
    fprintf( pFileParam, "VoxelWidth = %f\n",                   m_oTar.getWidth() ); 
    fprintf( pFileParam, "OutputPath = %s\n",                   m_pPath );
    fprintf( pFileParam, "AngularStepSize = %f\n",              getAngularStepSize() );
    fprintf( pFileParam, "PsiFrom = %f\n"  ,                    m_fPsiFrom );
    fprintf( pFileParam, "PsiTo = %f\n"    ,                    m_fPsiTo );
    fprintf( pFileParam, "ThetaFrom = %f\n",                    m_fThetaFrom );
    fprintf( pFileParam, "ThetaTo = %f\n"  ,                    m_fThetaTo );
    fprintf( pFileParam, "PhiFrom = %f\n"  ,                    m_fPhiFrom );
    fprintf( pFileParam, "PhiTo = %f\n"    ,                    m_fPhiTo );    
    fprintf( pFileParam, "XFrom = %f\n"    ,                    m_fXFrom );
    fprintf( pFileParam, "XTo = %f\n"      ,                    m_fXTo );
    fprintf( pFileParam, "YFrom = %f\n"    ,                    m_fYFrom );
    fprintf( pFileParam, "YTo = %f\n"      ,                    m_fYTo );
    fprintf( pFileParam, "ZFrom = %f\n"    ,                    m_fZFrom );
    fprintf( pFileParam, "ZTo = %f\n"      ,                    m_fZTo );
    fprintf( pFileParam, "WriteModelInterval = %i\n",           getWriteModelInterval() );
    fprintf( pFileParam, "NumberOfTraces = %i\n",               m_iNoOfCylinder2Detect );
    fprintf( pFileParam, "MinNumberOfResiduesInHelix = %i\n",   m_iMinResInHelix );
    if (m_bApplyBlurring2Model)
        fprintf( pFileParam, "ApplyBlurring2Model = true\n");
    else
        fprintf( pFileParam, "ApplyBlurring2Model = false\n");
    fprintf( pFileParam, "TemplateRadius = %f\n",           getTemplateRadius());
    fprintf( pFileParam, "SearchTemplateRadius = %f\n",     getSearchTemplateRadius());
    fprintf( pFileParam, "TemplatePointCount = %i\n",       getTemplatePointCount());
    fprintf( pFileParam, "TemplateRepeats = %i\n",          getTemplateRepeats());
    fprintf( pFileParam, "SearchTemplateRepeats = %i\n",    getSearchTemplateRepeats());
    fprintf( pFileParam, "DistBetweenRepeats = %f\n",       getDistBetweenRepeats() );
    fprintf( pFileParam, "CrawlingStepSize = %f\n",         getCrawlingStepSize() );
    fprintf( pFileParam, "AcceptMoveRatio = %f\n",          getAcceptMoveRatio() );
    fprintf( pFileParam, "MaxFailedCrawls = %i\n",          getMaxFailedCrawls() );
    
    if (m_bFitHelix)
        fprintf( pFileParam, "FitHelix = true\n");
    else
        fprintf( pFileParam, "FitHelix = false\n");

    fprintf( pFileParam, "Lambda = %f\n",                   getLambda() );
    
    fclose( pFileParam );
};



///////////////////////////////////////////////////////////////////////////////
// run ga in thread 
///////////////////////////////////////////////////////////////////////////////

/**
 * Write the top scoring solutions to the disk
 * \param oPop the population of solutions 
 * \param iWriteSolutions how many solutions to write
 */
template<class T>
void svt_gacylinder<T>::writeSolutions(svt_population<T> &oPop, unsigned int iWriteSolutions, char *pFilename)
{
    int iSize = oPop.size();
    if (iSize>0 && strlen(m_pPath) != 0 )
    {
    unsigned int iEffectiveWroteSolutions = (int)iWriteSolutions<iSize?iWriteSolutions:iSize;
    svtout << "Output of the " << iEffectiveWroteSolutions << " best individuals:" << endl;
    
    sort( oPop.rbegin(), oPop.rend() );
    oPop.erase( oPop.begin()+iEffectiveWroteSolutions, oPop.end() );
    
    //initPopulation( m_iPopSize, false );
    
    char pStr[2560];
    for(unsigned int i=0;i<iEffectiveWroteSolutions;i++)
    {
        sprintf( pStr, "%s/%s_%02i%03i.pdb", m_pPath, pFilename, this->m_iRun, i+1 );
        
        updateModel( &oPop[i] );
        updateVolume( &oPop[i] );
        m_oModel.writePDB( pStr );
        
       // if (m_oTarStr.size() > 0)
       // sprintf( pStr, "  [%02ld] %s/%s_%02i%02li.pdb - Score: %1.5f CC: %1.6f RMSD: %1.6f \n", oPop.size()-i, m_pPath,pFilename, this->m_iRun, oPop.size()-i, oPop[i].getFitness(), getCorrelation(), getRMSD() );
       // else
       // sprintf( pStr, "  [%02ld] %s/%s_%02i%02li.pdb - Score: %1.5f \n", oPop.size()-i, m_pPath, pFilename, this->m_iRun, oPop.size()-i, oPop[i].getFitness() );
      //  svtout << pStr;
    }
    svtout << endl;

    
    }
}

#endif
