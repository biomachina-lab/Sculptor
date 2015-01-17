/***************************************************************************
                          svt_gamultifit
                          fit multiple domains into map
                          ---------
    begin                : 01/21/2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_GAMULTIFIT
#define __SVT_GAMULTIFIT

// svt classes
#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_pdb.h>
#include <svt_clustering_trn.h>
#include <svt_volume.h>
#include <svt_gamultifit_ind.h>
#include <svt_ga.h>
#include <svt_stlVector.h>
// clib
#include <math.h>

typedef enum
{
  PDB_RMSD,
  VOL_CC,
  VOL_CC_BLUR_COARSE,
  VOL_CC_BLUR,
  VOL_CC_LAP
} score;

typedef enum
{
  ALL_ATOMS,
  BB_ATOMS,
  CA_ATOMS,
  CUSTOMIZE
} coarse_type;


/**
 * Class to hold a unit defined as a part of a biomolecule 
 * It holds a point cloud and other characteristics
 */
template<class T> class svt_unit
{
protected:
    
    //the point cloud pdb
    svt_point_cloud_pdb< svt_ga_vec > m_oPdb;
    
    //a coarse version of the pdb - can be all CA
    svt_point_cloud_pdb< svt_ga_vec > m_oCoarsePdb;
    
    // the center of the pdb (use coa)
    svt_ga_vec m_oCenter;
    
    //the translational search range
    Real64 m_fXFrom;
    Real64 m_fXTo;
    Real64 m_fYFrom;
    Real64 m_fYTo;
    Real64 m_fZFrom;
    Real64 m_fZTo;
    
public:

    /**
     * add a point cloud pdb 
     * \param oPdb the point cloud 
     */
    void setPdb(svt_point_cloud_pdb< svt_ga_vec > oPdb, svt_point_cloud_pdb< svt_ga_vec > oCoarsePdb);
    
    /**
     * add the coarse point cloud pdb 
     * \param oCoarsePdb the point cloud 
     */
    void setCoarsePdb(svt_point_cloud_pdb< svt_ga_vec > oCoarsePdb);
    
    /**
     * get the point cloud pdb 
     * \return oPdb the point cloud 
     */
    svt_point_cloud_pdb< svt_ga_vec > & getPdb();
    
    /**
     * get the coarse point cloud pdb 
     * \return oCoarsePdb the point cloud 
     */
    svt_point_cloud_pdb< svt_ga_vec > & getCoarsePdb();

    /**
     * Returns the number of atoms in the point cloud pdb
     * \return the number of atoms
     */
    int size();
    /**
     * Returns the number of atoms in the coarse point cloud pdb
     * \return the number of atoms
     */
    int getCoarsePdbSize();
      
    /**
     * Set the ranges for the translational search.
     * \param fXFrom     upper limit of the X
     * \param fXTo       lower limit of the X
     * \param fYFrom     upper limit of the Y
     * \param fYTo       lower limit of the Y
     * \param fZFrom     upper limit of the Z
     * \param fZTo       lower limit of the Z
     * \param bRelative2Center if true it means that From and To ranges are relative to the center of the unit; if false - they are absolute Angstrom distances
     */
    void setTranslSearchRange( Real64 fXFrom, Real64 fXTo, Real64 fYFrom, Real64 fYTo, Real64 fZFrom, Real64 fZTo, bool bRelative2Center = false );
    

    /**
     * get the ranges for the translational search.
     * \param fXFrom     upper limit of the X
     * \param fXTo       lower limit of the X
     * \param fYFrom     upper limit of the Y
     * \param fYTo       lower limit of the Y
     * \param fZFrom     upper limit of the Z
     * \param fZTo       lower limit of the Z
     */
    vector<Real64> getTranslSearchRange( );
    
    /**
     * get Translation
     *   0              fX                1
     *   | -------------\-----------------|
     *   | -------------\-----------------|
     *  m_fXFrom     TranslX            m_fXTo
     *  
     * \param fX value between 0 and 1 representing - see drawing upper
     * \param fY value between 0 and 1 representing - see drawing upper
     * \param fZ value between 0 and 1 representing - see drawing upper
     * \return TranslX, TranslY, Translz translations on x,y and z
     */
    svt_ga_vec getTranslation( Real64 fX, Real64 fY, Real64 fZ );

    /**
     * get genes corresponding to translation
     * \param the translation
     * \return the genes values between 0 and 1
     */
    svt_ga_vec getGenes( Real64 fX, Real64 fY, Real64 fZ );
    
    /**
     * Use the center and the translational range defined to identify the values of the genes and set them to the oInd
     * \param oInd - the individual to which to set the genes
     * \param iUnit - for which unit 
     */ 
    void setGenes(T * oInd, unsigned int iUnit);
};


/**
 * GA-based fitting algorithm
 * \author Stefan Birmanns
 */
template<class T> class svt_gamultifit : public svt_ga<T>
{
protected:

    // the units to be fitted
    vector< svt_unit<T> > m_oUnits;
    
    //individual corresponding to the units as loaded 
    T m_oInitialInd;

    
    //target volume
    svt_ga_vol m_oTar;
    
    //the target structure for validation purposes only
    svt_point_cloud_pdb<svt_ga_vec> m_oTarStr;
    
    //the target structure for validation purposes only
    svt_point_cloud_pdb<svt_ga_vec> m_oCoarseTarget;

    //the model
    svt_point_cloud_pdb<svt_ga_vec> m_oModel;
    // coarse model
    svt_point_cloud_pdb<svt_ga_vec> m_oCoarseModel;
    
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
    
    //no units
    unsigned int m_iNoUnits;

    //the number of individuals around the center
    unsigned int m_iPopSizeAroundCenters;

    // the size of the search spacetransClear()
    svt_ga_vec m_oSearchSpace;
    
    //transClear()
    svt_point_cloud_pdb< svt_ga_vec > m_oGenesInRealSpace, m_oRotation;
    
    // the scoring function to be used
    score m_eScore;
    
    //the kernel for the blurring of the model
    svt_ga_vol m_oKernel,m_oKernelCoarse;

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

    // how many generations should gamultifit wait until it outputs a new model file (0 turns output off)
    unsigned int m_iWriteModelInterval;
    
    // the generation at the last call of getBestTransfMat; required for update display in Sculptor - to update display only when after new generation was created
    int m_iGenerationOfLastGetBest;
    
    vector<int> m_oCountFramesWrote;
    
    //proportion input structure:
    Real64 m_fPropInputStr;
        
public:

    /**
     * Constructor
     */
    svt_gamultifit(unsigned int iGenes);
    
    /**
     * Destructor
     */
    virtual ~svt_gamultifit();
    
    /**
    * set the translation range for each unit
    */
    void setTranslSearchRange();


    /**
     * generate initial population
     * \param iNum number of inds in this population
     */
    virtual void initPopulation(int iNum, bool bOutputInput = false);

    /**
     * create new invididuals around the points given in 
     * \param oPdb
     */
    void initPopulationAroundPoints( svt_point_cloud_pdb<svt_vector4<Real64 > >& oPdb);
    
    /**
     * Create an object 
     */
    virtual svt_gamultifit<T>* createObject();

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
     * Set scoring function
     * \param eScore the scoring function
     */
    void setScore(score eScore);
    /**
     * Get scoring function
     * \return the scoring function
     */
    score getScore();

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
     * set units
     * \param oUnits the units
     */
    void setUnits( vector<svt_point_cloud_pdb< svt_ga_vec > > &oUnits, vector<svt_point_cloud_pdb< svt_ga_vec > > &oCoarseUnits);
    
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
    void setTarStr( svt_point_cloud_pdb< svt_ga_vec >  &oTarStr);
    
    /**
     * set coarse target
     * \param oCoarseTarget the coarse version of the target structure
     */
    void setCoarseTarget( svt_point_cloud_pdb< svt_ga_vec >  &oCoarseTarget);
    
    /**
     * set proportion of individuals in the initial population that are obtained from the input structure 
     * method useful for refinement and restart of the GA
     * \param fPropInputStr value from 0 to 1 ; 0.1 means 10% population is obtained by mutating and adding once the input Stru/individual
     */
    void setPropInputStr( Real64 fPropInputStr );
    
    /**
     * get proportion of individuals in the initial population that are obtained from the input structure 
     * \return value from 0 to 1 ; 0.1 means 10% population is obtained by mutating and adding once the input Stru/individual
     */
    Real64 getPropInputStr();
    
    /**
     * set the number of individuals created around the feature vectors of the map
     * \param iPopSizeAroundCenters
     */
    void setPopSizeAroundCenters( unsigned int iPopSizeAroundCenters );
    
    /**
     * get the number of individuals created around the feature vectors of the map
     * \return iPopSizeAroundCenters
     */
    unsigned int getPopSizeAroundCenters();


    /**
     * Get model
     * \return the model that was last generated by calcTransformation
     */
    inline svt_point_cloud_pdb<svt_ga_vec >&  getModel(){return m_oModel;}
    
    /**
     * Get coarse model
     * \return the model that was last generated by calcTransformation
     */
    inline svt_point_cloud_pdb<svt_ga_vec >&  getCoarseModel(){return m_oCoarseModel;}

    
    /**
     * Get model volume
     * \return the volume of the model that was last generated by calcTransformation
     */
    inline svt_ga_vol&  getModelVol(){return m_oModelVol;}
    
    /**
     * init kernels
     */
    void initKernels();

    //
    // Fitness function
    //

    /**
     * calculate transformation matrix
     * \param pInd pointer to individual
     */
    svt_ga_mat calcTransformation(T* pInd, unsigned int iUnit);
        
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
    void updateVolume(T* pInd, bool bCoarse=true);
    
    /**
     * Updates the coordinates of the Coarse model based on the genes of the individual
     */
    Real64 updateCoarseModel(T* pInd);
    
    /**
     * compute the leonard-jones function for both repulsive and attractive part
     * here one can change the 3.3 - distance from which the leonard-jones potential is cutoff * ATTENTION: DON"T FORGET TO ALSO CHANGE THE VALUES OF 0.7726424
     * the 3.8 - equilibrium distance for which the penalty is min * ATTENTION: DON"T FORGET TO ALSO CHANGE THE VALUES OF 0.7726424
     * \param fX given X
     */
    Real64 getLeonardJonesContribution(Real64 fX);
    
    /**
     * compute overlap using only the (1/d)^12 from leonard-jones potential
     * \param fX given X
     */
    Real64 getOverlap(Real64 fX);
    
    /**
     * add the mass to the particular voxel
     */
    void incAtModelVol(int iX, int iY, int iZ, Real64 fValue);//, svt_volume<char>& rMask, unsigned int iUnit);
    
    /**
     * update fitness
     * \param pInd pointer to individual that gets updated
     */
    virtual void updateFitness(T* pInd);

    /**
     * Calculate the full correlation corresponding to the current individual (with blur)
     */
    Real64 getCorrelation();

    /**
    * Calculate the rmsd of the individual towards the target model
    * \param pInd the individual of which RMSD is computed
    * \return the rmsd 
    */
    Real64 getRMSD(T* pInd);

    
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

    //
    // Transposition
    //

    /**
     * flips two genes between the same individual
     * \param rParentA reference to object
     * \param pNewIndA pointer to new ind
     */
    virtual void transpositionUniform(T& rParentA, T* pNewIndA );

    //
    // Output statistics, result files, etc
    //

    /**
     * output result
     */
    virtual void outputResult(bool bTabuAdded = false);
    
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
     * compute the location of the center and orientation of z vector when rotated following genes
     */
    void writePopGenesInRealSpace();
    
    /**
     * write niche results
     */
    void outputNicheResults();


    /**
     * Set the output path path
     * \param pPath pointer to array of char
     */
    void setOutputPath( const char *pPath );

    /**
     * How many generations should gamultifit wait until it outputs a new model file (0 turns output off)
     * \param iWriteModelInterval number of generations
     */
    void setWriteModelInterval( unsigned int iWriteModelInterval );
    /**
     * How many generations should gamultifit wait until it outputs a new model file (0 turns output off)
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
     * get the transformation matrixes of the best individual
     */
    vector< vector<svt_ga_mat> > getBestTransfMat();

    /**
     * Write the top scoring solutions to the disk
     * \param oPop the population of solutions 
     * \param iWriteSolutions how many solutions to write
     */
    void writeSolutions(svt_population<T> &oPop, unsigned int iWriteSolutions, char *pFileName);

};

///////////////////////////////////////////////////////////////////////////////
// svt_unit class
///////////////////////////////////////////////////////////////////////////////


/**
 * add a point cloud pdb 
 * \param oPdb the point cloud 
 */
template<class T>
void svt_unit<T>::setPdb(svt_point_cloud_pdb< svt_ga_vec > oPdb, svt_point_cloud_pdb< svt_ga_vec > oCoarsePdb )
{
    m_oPdb = oPdb;

    m_oCenter = m_oPdb.coa();
        
    svt_ga_mat oTranslation;
    oTranslation.loadIdentity();
    oTranslation.setTranslation(-m_oCenter);
    m_oPdb = oTranslation*m_oPdb;
    
    // Coarse Pdb
    m_oCoarsePdb = oTranslation*oCoarsePdb;
    
    svtout << "Coarse Atoms added: " << m_oCoarsePdb.size() << endl;
};

/**
 * add the coarse point cloud pdb 
 * \param oCoarsePdb the point cloud 
 */
template<class T>
void svt_unit<T>::setCoarsePdb(svt_point_cloud_pdb< svt_ga_vec > oCoarsePdb)
{
    
    m_oCoarsePdb = oCoarsePdb;
}

/**
 * get the point cloud pdb 
 * \return oPdb the point cloud 
 */
template<class T>
svt_point_cloud_pdb< svt_ga_vec > & svt_unit<T>::getPdb()
{
    return m_oPdb;
};


/**
 * get the coarse point cloud pdb 
 * \return oCoarsePdb the point cloud 
 */
template<class T>
svt_point_cloud_pdb< svt_ga_vec > & svt_unit<T>::getCoarsePdb()
{
    return m_oCoarsePdb;
};

/**
 * Returns the number of atoms in the point cloud pdb
 * \return the number of atoms
 */
template<class T>
int svt_unit<T>::size()
{
    return m_oPdb.size();
};

/**
 * Returns the number of atoms in the coarse point cloud pdb
 * \return the number of atoms
 */
template<class T>
int svt_unit<T>::getCoarsePdbSize()
{
    return m_oCoarsePdb.size();
};

/**
 * Set the ranges for the translational search.
 * \param fXFrom     upper limit of the X
 * \param fXTo       lower limit of the X
 * \param fYFrom     upper limit of the Y
 * \param fYTo       lower limit of the Y
 * \param fZFrom     upper limit of the Z
 * \param fZTo       lower limit of the Z
 */
template<class T>
void svt_unit<T>::setTranslSearchRange( Real64 fXFrom, Real64 fXTo, Real64 fYFrom, Real64 fYTo, Real64 fZFrom, Real64 fZTo, bool bRelative2Center )
{
    
    if (bRelative2Center)
    {
        m_fXFrom    = m_oCenter.x() + fXFrom;
        m_fXTo      = m_oCenter.x() + fXTo;
        m_fYFrom    = m_oCenter.y() + fYFrom;
        m_fYTo      = m_oCenter.y() + fYTo;
        m_fZFrom    = m_oCenter.z() + fZFrom;
        m_fZTo      = m_oCenter.z() + fZTo;
    }
    else
    {
        m_fXFrom    = fXFrom;
        m_fXTo      = fXTo;
        m_fYFrom    = fYFrom;
        m_fYTo      = fYTo;
        m_fZFrom    = fZFrom;
        m_fZTo      = fZTo;
    }
};

/**
 * get the ranges for the translational search.
 */
template<class T>
vector<Real64> svt_unit<T>::getTranslSearchRange()
{
    vector <Real64> oTranslSearchRange;
    
    oTranslSearchRange.push_back(m_fXFrom);
    oTranslSearchRange.push_back(m_fXTo);
    oTranslSearchRange.push_back(m_fYFrom);
    oTranslSearchRange.push_back(m_fYTo);
    oTranslSearchRange.push_back(m_fZFrom);
    oTranslSearchRange.push_back(m_fZTo);
    
    return oTranslSearchRange;
};

/**
 * get Translation
 *   0              fX                1
 *   | -------------\-----------------|
 *   | -------------\-----------------|
 *  m_fXFrom     TranslX            m_fXTo
 *  
 * \param fX value between 0 and 1 representing - see drawing upper
 * \param fY value between 0 and 1 representing - see drawing upper
 * \param fZ value between 0 and 1 representing - see drawing upper
 * \return TranslX, TranslY, Translz translations on x,y and z
 */
template<class T>
svt_ga_vec svt_unit<T>::getTranslation( Real64 fX, Real64 fY, Real64 fZ )
{
    svt_ga_vec oVec;
    
    oVec.x( m_fXFrom + fX * (m_fXTo - m_fXFrom) );
    oVec.y( m_fYFrom + fY * (m_fYTo - m_fYFrom) );
    oVec.z( m_fZFrom + fZ * (m_fZTo - m_fZFrom) );
    
    return oVec;
};

/**
 * get genes corresponding to translation
 * \param the translation
 * \return the genes values between 0 and 1
 */
template<class T>
svt_ga_vec svt_unit<T>::getGenes( Real64 fX, Real64 fY, Real64 fZ )
{
    svt_ga_vec oVec;

    oVec.x( (fX - m_fXFrom)/(m_fXTo - m_fXFrom) );
    oVec.y( (fY - m_fYFrom)/(m_fYTo - m_fYFrom) );
    oVec.z( (fZ - m_fZFrom)/(m_fZTo - m_fZFrom) );
    
    return oVec;
};



/**
 * Use the center and the translational range defined to identify the values of the genes and set them to the oInd
 * \param oInd - the individual to which to set the genes
 * \param iUnit - for which unit 
 */ 
template<class T>
void svt_unit<T>::setGenes(T* oInd, unsigned int iUnit)
{
    if (oInd->getGeneCount() > (int)iUnit*4+3)
    {
	oInd->setGene(iUnit*4+0, (m_oCenter.x() - m_fXFrom)/(m_fXTo - m_fXFrom) );
	oInd->setGene(iUnit*4+1, (m_oCenter.y() - m_fYFrom)/(m_fYTo - m_fYFrom) );
	oInd->setGene(iUnit*4+2, (m_oCenter.z() - m_fZFrom)/(m_fZTo - m_fZFrom) );
	oInd->setGene(iUnit*4+3, 0.0 );
    }
    else
    {
	svtout << "Can't set genes because the individual was not yet set" << endl;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
template<class T>
svt_eulerAngles svt_gamultifit<T>::m_oAngles;

/**
 * Constructor
 */
template<class T> 
svt_gamultifit<T>::svt_gamultifit(unsigned int iGenes) : svt_ga<T>( iGenes ),
    m_fWidth( 0.0f ),
    m_fRes( 0.0f ),
    m_iPopSizeAroundCenters( 0.0 ),
    m_eScore( VOL_CC_BLUR_COARSE ),
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
    m_fZTo( 0.0f ),
    m_fPropInputStr(0.0)
{ 
    strcpy(m_pPath, "");
};


/**
 * Destructor
 */
template<class T>
svt_gamultifit<T>::~svt_gamultifit() {};

/**
 * set the translation range for each unit
 */
template<class T>
void svt_gamultifit<T>::setTranslSearchRange()
{
     if ( (m_fXFrom!=m_fXTo || m_fYFrom!=m_fYTo || m_fZFrom!=m_fZTo) && m_oUnits.size() > 0) // the values were already initialized
     {
	for (int iGene=0; iGene<this->m_iGenes; iGene++)
	    m_oInitialInd.addGene(0.0);
	    
	Real64 fBorder=15; //don't consider 15A  (on all directions) when defining the search space
	for (unsigned int iUnit = 0; iUnit<m_iNoUnits; iUnit++)
	{
	    m_oUnits[iUnit].setTranslSearchRange( m_fXFrom + fBorder, m_fXTo - fBorder,
						  m_fYFrom + fBorder, m_fYTo - fBorder, 
						  m_fZFrom + fBorder, m_fZTo - fBorder);

	    m_oUnits[iUnit].setGenes(&m_oInitialInd, iUnit);
	}
	makeValid(&m_oInitialInd);
    }

}

/**
 * generate initial population
 * \param iNum number of inds in this population
 */
template<class T>
void svt_gamultifit<T>::initPopulation(int iNum, bool bOutputInput)
{
    setTranslSearchRange();
    
    if (m_fPropInputStr <= 1.0 && m_fPropInputStr > 0.0 )
    {
	svt_ga<T>::initPopulation(iNum*(1.0-m_fPropInputStr));
	
	    //add the initial individual
	this->m_oPop.push_back(m_oInitialInd);
    
	//add mutations of the original individual for 10% of the ga
	T oInd;
	for (int iIndex=0; iIndex < (int)iNum*m_fPropInputStr-1.0 && (int)(this->m_oPop).size() < iNum; iIndex++)
	{
	    oInd = m_oInitialInd;
	    svt_ga<T>::mutationCauchy(&oInd);
	    makeValid (&oInd);
	    this->m_oPop.push_back(oInd);	    
	}
	
	//set the right size

    }
    else
	svt_ga<T>::initPopulation(iNum);
   
    if (m_oCoarseTarget.size()>0)
	initPopulationAroundPoints( m_oCoarseTarget ); 

     
    
    if (strlen(m_pPath) != 0 && bOutputInput)
    {
	// output the files
	char pFname[1256];
	if (strcmp(m_pPath,"")!=0 && m_fWidth!=0 && this->m_iThread == 0)
	{
	    sprintf(pFname, "%s/target.situs", m_pPath );
	    m_oTar.saveSitus( pFname );
	}
	
	if (m_oTarStr.size()>0 && this->m_iThread == 0)
	{
	    sprintf(pFname, "%s/target.pdb", m_pPath );
	    m_oTarStr.writePDB( pFname );
	}
    }
    
    // create bluring kernel
    initKernels();
	
    // create the angle table
    if (m_oAngles.getAngleCount() <= 0)
    {
        svtout << "Create angular search table (P/T/P, Delta, #Angles, memory): ( " << m_fPsiFrom << " - " << m_fPsiTo << " / " << m_fThetaFrom << " - " << m_fThetaTo << " / " << m_fPhiFrom << " - " << m_fPhiTo << ", " << m_fDelta << ", ";

        if (m_fPsiTo<360 && m_fThetaTo < 180 && m_fPhiTo < 360) // refinement is applied
        {
            m_oAngles.initTable( 0.0, 360.0, 0.0, m_fThetaTo, 0.0,360.0, m_fDelta );
            m_oAngles.removeAngles( m_fPsiTo, 360.0-m_fPsiTo,m_fThetaTo, 180, m_fPhiTo, 360.0-m_fPhiTo);
        }
        else
            m_oAngles.initTable( m_fPsiFrom, m_fPsiTo, m_fThetaFrom, m_fThetaTo, m_fPhiFrom, m_fPhiTo, m_fDelta );

        cout << m_oAngles.getAngleCount() << ", " << floor((m_oAngles.getAngleCount() * sizeof(float) * 3) / 1048576) << "mb )" << endl;
    }
    
    for (unsigned int iIndex = 0; iIndex < 50; iIndex++ )
	m_oCountFramesWrote.push_back(0); 
}

/**
 * create new invididuals around the points given in 
 * \param oPdb
 */
template<class T>
void svt_gamultifit<T>::initPopulationAroundPoints(svt_point_cloud_pdb<svt_vector4<Real64 > >& oPdb)
{
    vector<bool> oUsed;
    unsigned int iUnit, iPoint, iSelPoint;
    svt_vector4<Real64> oGenes;

    for (unsigned int iIndex = 0; iIndex < m_iPopSizeAroundCenters; iIndex++)
    {
	oUsed.clear();
	for (iPoint=0; iPoint < oPdb.size(); iPoint++)
	   oUsed.push_back(false);

	T oInd;
	for (iUnit=0; iUnit < m_iNoUnits; iUnit++)
	{
	    //get the point
	    do {iSelPoint = floor( svt_genrand() * oPdb.size() );} while(oUsed[iSelPoint]);
	    oUsed[iSelPoint] = true;

	    //get corresponding genes	
	    oGenes = m_oUnits[iUnit].getGenes(	oPdb[iSelPoint].x()+(svt_genrand()-0.5)*10.0,
						oPdb[iSelPoint].y()+(svt_genrand()-0.5)*10.0, 
						oPdb[iSelPoint].z()+(svt_genrand()-0.5)*10.0);
	    //set genes
	    oInd.addGene( oGenes.x() );
	    oInd.addGene( oGenes.y() );
	    oInd.addGene( oGenes.z() ); 
	    oInd.addGene( svt_genrand() );
	}
	
	oInd.setOrigin( AROUND_CENTER );
	makeValid( &oInd );
	this->m_oPop.push_back( oInd );
    }
};

/**
 * Create an object 
 */
template<class T>
svt_gamultifit<T>* svt_gamultifit<T>::createObject()
{
    return new svt_gamultifit<T>( *this );
};


///////////////////////////////////////////////////////////////////////////////
// Parameters
///////////////////////////////////////////////////////////////////////////////

/**
 * Set resolution of the target map
 * \param fRes the resolution of the target map
 */
template<class T>
void svt_gamultifit<T>::setResolution(Real64 fRes)
{
    m_fRes = fRes;
};
/**
 * Get resolution of the target map
 * \return the resolution of the target map
 */
template<class T>
Real64 svt_gamultifit<T>::getResolution()
{
    return m_fRes;
};

/**
 * Set scoring function
 * \param eScore the scoring function
 */
template<class T>
void svt_gamultifit<T>::setScore(score eScore) 
{  
    m_eScore = eScore;
    
    if (m_fWidth!=0 && m_fRes!=0)//volume was initialized and resolution was set
	initKernels();
    
    if (this->m_oPop.size()>0) // score changed during run; need to update all fitness
	for (unsigned int i=0; i<this->m_oPop.size(); i++)
	    updateFitness( &this->m_oPop[i] );
};

/**
 * Get scoring function
 * \return the scoring function
 */
template<class T>
score svt_gamultifit<T>::getScore()
{  
    return m_eScore;
};

/**
 * Set the angular step size. In general: The smaller the value, the better the accuracy. In contrast to an exhaustive search, the runtime will also not be longer, if a finer step size
 * is chosen. The only limitation is the memory - as the table needs to be stored, very low numbers might result in an excessive use of memory. Recommended: ~0.5 to 1.0.
 * \param fDelta the angular step size (default 0.5)
 */
template<class T>
void svt_gamultifit<T>::setAngularStepSize( Real64 fDelta )
{
    m_fDelta = fDelta;
};
/**
 * Get the angular step size. In general: The smaller the value, the better the accuracy. In contrast to an exhaustive search, the runtime will also not be longer, if a finer step size
 * is chosen. The only limitation is the memory - as the table needs to be stored, very low numbers might result in an excessive use of memory. Recommended: ~0.5 to 1.0.
 * \return the angular step size (default 0.5)
 */
template<class T>
Real64 svt_gamultifit<T>::getAngularStepSize( )
{
    return m_fDelta;
};

/**
 * Returns the number of angles 
 */
template<class T>
Real64 svt_gamultifit<T>::getAnglesCount()
{
    return m_oAngles.getAngleCount();
};

/**
 * Get angles
 */
template<class T>
svt_vector4<Real64>& svt_gamultifit<T>::getAngle( long unsigned int iIndex)
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
void svt_gamultifit<T>::setAngularSearchRange( Real64 fPsiFrom, Real64 fPsiTo, Real64 fThetaFrom, Real64 fThetaTo, Real64 fPhiFrom, Real64 fPhiTo )
{
    m_fPsiFrom   = fPsiFrom;
    m_fPsiTo     = fPsiTo;
    m_fThetaFrom = fThetaFrom;
    m_fThetaTo   = fThetaTo;
    m_fPhiFrom   = fPhiFrom;
    m_fPhiTo     = fPhiTo;

    m_oAngles.initTable( m_fPsiFrom, m_fPsiTo, m_fThetaFrom, m_fThetaTo, m_fPhiFrom, m_fPhiTo, m_fDelta );

    svtout << "Create angular search table (P/T/P, Delta, #Angles, memory): ( " << m_fPsiFrom << " - " << m_fPsiTo << " / " << m_fThetaFrom << " - " << m_fThetaTo << " / " << m_fPhiFrom << " - " << m_fPhiTo << ", " << m_fDelta << ", ";
    cout << m_oAngles.getAngleCount() << ", " << floor((m_oAngles.getAngleCount() * sizeof(float) * 3) / 1048576) << "mb )" << endl;
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
void svt_gamultifit<T>::setRelativeTranslSearchRange( Real64 fXFrom, Real64 fXTo, Real64 fYFrom, Real64 fYTo, Real64 fZFrom, Real64 fZTo )
{
    
    
    if ( (fXFrom!=fXTo || fYFrom!=fYTo || fZFrom!=m_fZTo) ) // the values were already initialized
    {
	m_fXFrom    = fXFrom;
	m_fXTo      = fXTo;
	m_fYFrom    = fYFrom;
	m_fYTo      = fYTo;
	m_fZFrom    = fZFrom;
	m_fZTo      = fZTo;
	setTranslSearchRange();
	
    }
};

///////////////////////////////////////////////////////////////////////////////
// Units, Map, PDB files...
///////////////////////////////////////////////////////////////////////////////

/**
 * set units
 * \param oUnits the units
 */
template<class T>
void svt_gamultifit<T>::setUnits( vector<svt_point_cloud_pdb< svt_ga_vec > > &oUnits, vector<svt_point_cloud_pdb< svt_ga_vec > > &oCoarseUnits)
{
    svt_unit<T> oUnit;
    m_iNoUnits = oUnits.size();
    m_oUnits.clear();
    for (unsigned int iUnit=0; iUnit < m_iNoUnits; iUnit++)
    {
        oUnit.setPdb( oUnits[iUnit], oCoarseUnits[iUnit] );
        m_oUnits.push_back( oUnit );
        
        
        //initialize the model
        if (iUnit==0)
            m_oModel = oUnits[iUnit];
        else
            m_oModel.append( oUnits[iUnit] );
        
        //Update Coarse Model
        if (iUnit==0)
            m_oCoarseModel = oUnit.getCoarsePdb();
        else
            m_oCoarseModel.append( oUnit.getCoarsePdb() );
        
    }
    svtout << m_iNoUnits << " units were added." << endl;

    setTranslSearchRange();
    
};

/**
 * set target str
 * only for validation purposes 
 * \param oTarStr the target structure
 */
template<class T>
void svt_gamultifit<T>::setTarStr( svt_point_cloud_pdb< svt_ga_vec >  &oTarStr)
{
    m_oTarStr = oTarStr;
};

/**
 * set coarse target
 * \param oCoarseTarget the coarse version of the target structure
 */
template<class T>
void svt_gamultifit<T>::setCoarseTarget( svt_point_cloud_pdb< svt_ga_vec >  &oCoarseTarget)
{
    m_oCoarseTarget = oCoarseTarget;
};


/**
 * set target
 * \param oTar the map
 */
template<class T>
void svt_gamultifit<T>::setTarget( svt_ga_vol &oTar)
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
    
    m_fXFrom 	= m_fOrigX;
    m_fXTo 	= m_fOrigX+m_iSizeX*m_fWidth;
    m_fYFrom 	= m_fOrigY;
    m_fYTo 	= m_fOrigY+m_iSizeY*m_fWidth;
    m_fZFrom 	= m_fOrigZ;
    m_fZTo 	= m_fOrigZ+m_iSizeZ*m_fWidth;
    setTranslSearchRange();
    
    if (strlen(m_pPath) != 0)
    {
	// output the files
	char pFname[1256];
	if (strcmp(m_pPath,"")!=0 && m_fWidth!=0 && this->m_iThread == 0)
	{
	    sprintf(pFname, "%s/target.situs", m_pPath );
	    m_oTar.saveSitus( pFname );
	}
    }
    
    if (m_fRes!=0)
	initKernels();
	
};

/**
 * set proportion of individuals in the initial population that are obtained from the input structure 
 * method useful for refinement and restart of the GA
 * \param fPropInputStr value from 0 to 1 ; 0.1 means 10% population is obtained by mutating and adding once the input Stru/individual
 */
template<class T>
void svt_gamultifit<T>::setPropInputStr( Real64 fPropInputStr )
{
    m_fPropInputStr = fPropInputStr ;
};

/**
 * get proportion of individuals in the initial population that are obtained from the input structure 
 * \return value from 0 to 1 ; 0.1 means 10% population is obtained by mutating and adding once the input Stru/individual
 */
template<class T>
Real64 svt_gamultifit<T>::getPropInputStr()
{
    return m_fPropInputStr;
};


/**
 * set the number of individuals created around the feature vectors of the map
 * \param iPopSizeAroundCenter 
 */
template<class T>
void svt_gamultifit<T>::setPopSizeAroundCenters( unsigned int iPopSizeAroundCenters )
{
    m_iPopSizeAroundCenters = iPopSizeAroundCenters; 
};

/**
 * get the number of individuals created around the feature vectors of the map
 * \return iPopSizeAroundCenters
 */
template<class T>
unsigned int svt_gamultifit<T>::getPopSizeAroundCenters()
{
    return m_iPopSizeAroundCenters;
};



/**
 * init blurring kernels
 */
template<class T>
void svt_gamultifit<T>::initKernels()
{
    switch( m_eScore )
    {
	case VOL_CC:
	case VOL_CC_BLUR_COARSE:
        case VOL_CC_BLUR:
            // 3D kernels
	    //m_oKernel.createSitusBlurringKernel(m_fWidth, m_fRes);
            //m_oKernelCoarse.createSitusBlurringKernel(m_fWidth/2.0, m_fRes/2.5);
            // 1D kernels
	    m_oKernel.create1DBlurringKernel(m_fWidth, m_fRes);
	    m_oKernelCoarse.create1DBlurringKernel(m_fWidth/2.0, m_fRes/2.5);
	    break;
	case VOL_CC_LAP:
	    m_oKernel.createLaplacianOfGaussian(m_fWidth, m_fRes);
	    break;
	default:
	    break;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Routines to calculate the fitness
///////////////////////////////////////////////////////////////////////////////

/**
 * calculate transformation matrix - use quaternions
 * \param pInd pointer to individual
 */
template<class T>
svt_ga_mat svt_gamultifit<T>::calcTransformation(T* pInd, unsigned int iUnit)
{  
    svt_ga_mat oMat;
    
    makeValid( pInd );

    Real64 fX = pInd->getGene(iUnit*4+0);
    Real64 fY = pInd->getGene(iUnit*4+1);
    Real64 fZ = pInd->getGene(iUnit*4+2);
    
    svt_ga_vec oTransl = m_oUnits[iUnit].getTranslation(fX, fY, fZ);
    
    //old code had phi and psi interchanged
    Real32 fRX = m_oAngles.getPhi  ( (long unsigned int)(pInd->getGene(iUnit*4+3) * m_oAngles.getAngleCount()) );
    Real32 fRY = m_oAngles.getTheta( (long unsigned int)(pInd->getGene(iUnit*4+3) * m_oAngles.getAngleCount()) );
    Real32 fRZ = m_oAngles.getPsi  ( (long unsigned int)(pInd->getGene(iUnit*4+3) * m_oAngles.getAngleCount()) );

    oMat.loadIdentity();
    
    oMat.rotatePTP( fRX, fRY, fRZ );
    oMat.translate( oTransl );

    //update the coarse phenotype as well
    pInd->updateCoarsePhenotype(oMat, iUnit);
    
    return oMat;
}

/**
 * Updates the coordinates of the model based on the genes of the individual
 * attention: it does not update the volume of the model (see update volume)
 */
template<class T>
void svt_gamultifit<T>::updateModel(T* pInd)
{
    unsigned int iSize;
    unsigned int iNoAtomsUpdated=0;
    svt_ga_mat oMat;

    Real64 fXa, fYa,fZa;
    
    for (unsigned int iUnit=0; iUnit<m_iNoUnits; iUnit++)
    {
        oMat = calcTransformation(pInd, iUnit);
        
        iSize = m_oUnits[iUnit].size();
	svt_point_cloud_pdb<svt_ga_vec>& oUnit = m_oUnits[iUnit].getPdb();
	
        for (unsigned int iAtom = 0; iAtom < iSize; iAtom++ )
        {
	    fXa =  oMat[0][0]*oUnit[iAtom].x() + oMat[0][1]*oUnit[iAtom].y() + oMat[0][2]*oUnit[iAtom].z() + oMat[0][3]*oUnit[iAtom].w();
	    fYa =  oMat[1][0]*oUnit[iAtom].x() + oMat[1][1]*oUnit[iAtom].y() + oMat[1][2]*oUnit[iAtom].z() + oMat[1][3]*oUnit[iAtom].w();
	    fZa =  oMat[2][0]*oUnit[iAtom].x() + oMat[2][1]*oUnit[iAtom].y() + oMat[2][2]*oUnit[iAtom].z() + oMat[2][3]*oUnit[iAtom].w();
            
	    m_oModel[iNoAtomsUpdated+iAtom].x( fXa );
	    m_oModel[iNoAtomsUpdated+iAtom].y( fYa );
	    m_oModel[iNoAtomsUpdated+iAtom].z( fZa );
        }
        iNoAtomsUpdated += iSize;
    }
    
};

/**
 * Updates the volume of the model 
 * Attention: it does not update the model - the pdb remains the same
 */
template<class T>
void svt_gamultifit<T>::updateVolume(T* pInd, bool bCoarse)
{
    svt_ga_mat oMat;
    
    // set every voxel in modelvol to 0
    m_oModelVol.setValue(0.0f);

    for (unsigned int iUnit=0; iUnit<m_iNoUnits; iUnit++)
    {
        oMat = calcTransformation(pInd, iUnit);
	
	if (bCoarse)
	    m_oUnits[iUnit].getCoarsePdb().projectMass(&m_oModelVol, oMat);
	else
	    m_oUnits[iUnit].getPdb().projectMass(&m_oModelVol, oMat);
	
	
    }
    
};

/**
 * Updates the coordinates of the Coarse model based on the genes of the individual
 */
template<class T>
Real64 svt_gamultifit<T>::updateCoarseModel(T* pInd)
{
    unsigned int iSize;
    unsigned int iNoAtomsUpdated=0;
    svt_ga_mat oMat;
    Real64 fXa, fYa,fZa, fPenalty=0;
    
    svt_point_cloud_pdb< svt_ga_vec > oCoarsePdb;
    for (unsigned int iUnit=0; iUnit<m_iNoUnits; iUnit++)
    {
        oMat = calcTransformation(pInd, iUnit);
        
        oCoarsePdb = m_oUnits[iUnit].getCoarsePdb();
        iSize = oCoarsePdb.size();
        
        for (unsigned int iAtom = 0; iAtom < iSize; iAtom++ )
        {
            // update coordinate
	    fXa =  oMat[0][0]*oCoarsePdb[iAtom].x() + oMat[0][1]*oCoarsePdb[iAtom].y() + oMat[0][2]*oCoarsePdb[iAtom].z() + oMat[0][3]*oCoarsePdb[iAtom].w();
	    fYa =  oMat[1][0]*oCoarsePdb[iAtom].x() + oMat[1][1]*oCoarsePdb[iAtom].y() + oMat[1][2]*oCoarsePdb[iAtom].z() + oMat[1][3]*oCoarsePdb[iAtom].w();
	    fZa =  oMat[2][0]*oCoarsePdb[iAtom].x() + oMat[2][1]*oCoarsePdb[iAtom].y() + oMat[2][2]*oCoarsePdb[iAtom].z() + oMat[2][3]*oCoarsePdb[iAtom].w();
            
	    m_oCoarseModel[iNoAtomsUpdated+iAtom].x( fXa );
	    m_oCoarseModel[iNoAtomsUpdated+iAtom].y( fYa );
	    m_oCoarseModel[iNoAtomsUpdated+iAtom].z( fZa );
        }
        iNoAtomsUpdated += iSize;

    }

    return fPenalty;
};

/**
 * compute the leonard-jones function for both repulsive and attractive part
 * here one can change the 3.2 - distance from which the leonard-jones potential is cutoff * ATTENTION: DON"T FORGET TO ALSO CHANGE THE VALUES OF 4.724488
 * the 3.8 - equilibrium distance for which the penalty is min * ATTENTION: DON"T FORGET TO ALSO CHANGE THE VALUES OF 4.724488
 * \param fX given X
 */
template<class T>
Real64 svt_gamultifit<T>::getLeonardJonesContribution(Real64 fX)
{
    Real64 fY;
    
    if (fX < 3.1) 
    {
        fY = 4.724488; // (3.8/3.1)^12 - 2*(3.8/3.1)^6 
    }
    else
    {
        Real64 fPow = 3.8/fX;
        
        fPow = pow(fPow,6);
        fY = fPow*fPow - 2.0f*fPow;
    }
    return fY;
};

/**
 * compute overlap using only the (1/d)^12 from leonard-jones potential
 * \param fX given X
 */
template<class T>
Real64 svt_gamultifit<T>::getOverlap(Real64 fX)
{
    Real64 fY;
    
    if (fX < 3.1) 
    {
        fY = 11.50967; // (3.8/3.1)^12 - 2*(3.8/3.1)^6 
    }
    else
    {
        Real64 fPow = 3.8/fX;
        
        fPow = pow(fPow,6);
        fY = fPow*fPow;
    }
    return fY;
};

/**
 * update fitness
 * \param pInd pointer to individual that gets updated
 */
template<class T>
void svt_gamultifit<T>::updateFitness(T* pInd)
{
    Real64 fFitness = 1e10;

    switch (m_eScore)
    {
    case PDB_RMSD:
        updateModel( pInd );
        fFitness = 1e10-m_oModel.rmsd( m_oTarStr, false, ALL, false ); //rmsd
        break;
    case VOL_CC:
	updateVolume( pInd );
	fFitness = m_oModelVol.correlation(m_oTar, false );
	break;
    case VOL_CC_BLUR_COARSE:
        updateVolume( pInd );
        // 1D kernel convolution
        m_oModelVol.convolve1D3D(m_oKernelCoarse, false ); // don't normalize
        // 3D kernel convolution
        //m_oModelVol.convolve(m_oKernelCoarse, false ); // don't normalize
        fFitness = m_oModelVol.correlation(m_oTar, false);
	break;
    case VOL_CC_BLUR:
	updateVolume( pInd );
        // 1D kernel convolution
        m_oModelVol.convolve1D3D(m_oKernel, false ); // don't normalize
        // 3D kernel convolution
        //m_oModelVol.convolve(m_oKernel, false ); // don't normalize
        fFitness = m_oModelVol.correlation(m_oTar, false);
	break;
    case VOL_CC_LAP:
        updateVolume( pInd );
        m_oModelVol.convolve(m_oKernel, false ); // don't normalize
        fFitness = m_oModelVol.correlation(m_oTar, false);
        break;
    }

    pInd->setFitness( fFitness );

    this->m_iFitnessUpdateCount++;
}

/**
 * Calculate the full correlation corresponding to a certain individual (with blur)
 */
template<class T>
Real64 svt_gamultifit<T>::getCorrelation()
{
    m_oModelVol.convolve1D3D(m_oKernel, false ); // don't normalize
    return m_oModelVol.correlation(m_oTar, false);
}

/**
 * Calculate the rmsd of the individual towards the target model
 * \param pInd the individual of which RMSD is computed
 * \return the rmsd 
 */
template<class T>
Real64 svt_gamultifit<T>::getRMSD(T* pInd)
{
    Real64 fPartialRmsd, fRmsd = 0;
    svt_matrix4<Real64> oMat;
    
    if (m_oTarStr.size() != 0)
    {
	svt_point_cloud_pdb< svt_vector4<Real64> > oPdb, oUnit;
	for (unsigned int iUnit=0; iUnit<m_iNoUnits; iUnit++)
	{
	    oMat = calcTransformation(pInd, iUnit);
	    oUnit = m_oUnits[iUnit].getPdb();
	    oPdb = oMat*oUnit;
	    
	    fPartialRmsd = oPdb.rmsd( m_oTarStr, false, ALL, false );
	    if (fPartialRmsd == 1.0e10)
		return -1;
	    fRmsd += fPartialRmsd*fPartialRmsd;
	}
    
    }
    
    fRmsd = sqrt(fRmsd/m_iNoUnits);
    
    return fRmsd;
    
}

/**
 * Calculate the rmsd of the individual towards the target model
 * \return the rmsd 
 */
template<class T>
Real64 svt_gamultifit<T>::getRMSD()
{
    if (m_oTarStr.size()>0)
	return m_oModel.rmsd( m_oTarStr, false, ALL, false );
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
void svt_gamultifit<T>::mutationCustom(int iInd)
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
void svt_gamultifit<T>::mutationCauchy(int iInd, int iRandIndex, Real64 fRatio)
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
void svt_gamultifit<T>::mutationAllCauchy(int iInd)
{
    for (unsigned int iIndex = 0; iIndex<(unsigned int)this->m_iGenes; iIndex++ )
        mutationCauchy(iInd, iIndex);
}

///////////////////////////////////////////////////////////////////////////////
// Transposition
///////////////////////////////////////////////////////////////////////////////

/**
 * flips two genes between the same individual
 * \param rParentA reference to object
 * \param pNewIndA pointer to new ind
 */
template<class T>
void svt_gamultifit<T>::transpositionUniform(T& rParentA, T* pNewIndA )
{    
    if (m_iNoUnits == 1)
    {
	(*pNewIndA) = rParentA;
	return;
    }
    
    Real64 fRand;
    int iPoint1, iPoint2;
    
    fRand = svt_genrand();
    iPoint1 = (int)(fRand * (Real64)(m_iNoUnits));
    
    do 
    {
	fRand = svt_genrand();
	iPoint2 = (int)(fRand * (Real64)(m_iNoUnits));
    } while (iPoint1 == iPoint2);

    if (iPoint1 > iPoint2)
    {
	int iTemp = iPoint1;
	iPoint1 = iPoint2;
	iPoint2 = iTemp;
    }

    for(int j=0;j<(int)m_iNoUnits;j++)
    {
        if (j == iPoint1)
        {
            pNewIndA->addGene( rParentA.getGene(iPoint2*4+0) );
            pNewIndA->addGene( rParentA.getGene(iPoint2*4+1) );
            pNewIndA->addGene( rParentA.getGene(iPoint2*4+2) );
            pNewIndA->addGene( rParentA.getGene(iPoint2*4+3) );
        }else
        {
            if (j == iPoint2)
            {
                pNewIndA->addGene( rParentA.getGene(iPoint1*4+0) );
                pNewIndA->addGene( rParentA.getGene(iPoint1*4+1) );
                pNewIndA->addGene( rParentA.getGene(iPoint1*4+2) );
                pNewIndA->addGene( rParentA.getGene(iPoint1*4+3) );
            }else
            {
                pNewIndA->addGene( rParentA.getGene(j*4+0) );
                pNewIndA->addGene( rParentA.getGene(j*4+1) );
                pNewIndA->addGene( rParentA.getGene(j*4+2) );
                pNewIndA->addGene( rParentA.getGene(j*4+3) );
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
// Output statistics, result files, etc
///////////////////////////////////////////////////////////////////////////////

/**
 * output result
 */
template<class T>
void svt_gamultifit<T>::outputResult(bool bTabuAdded)
{
    char pFname[256], pOut[1024];
    Real64 fRmsd = -1.0, fRmsd2 = -1.0;

    if (m_iWriteModelInterval != 0 && this->m_iGenerations%m_iWriteModelInterval==0 )
    {
	svt_ga<T>::sortPopulation();
	
	svt_matrix<Real64> oMat;
	fRmsd = -1.0;
	fRmsd2 = -1.0;
	
	for(unsigned int i=0; i<this->m_oBestPop.size(); i++)
	{
	    updateModel( &this->m_oBestPop[ i ] );

	    if (m_oTarStr.size() > 0)
		fRmsd2 = m_oModel.rmsd( m_oTarStr, false, ALL,  false );
	    else 
		fRmsd2 = -1;
	    
	    fRmsd =  getRMSD(&this->m_oBestPop[ i ]);
	    
	    updateVolume( &this->m_oBestPop[ i ], false);
	    m_oModelVol.convolve1D3D(m_oKernel, false ); // don't normalize
	    Real64 fCC = m_oModelVol.correlation(m_oTar, false);
	    
  
	    if (m_oTarStr.size() > 0)
		sprintf(pOut, "[%02d-%02d-%04d] %8.6f %8.6f - %8.3f %8.3f\n", this->m_iRun, this->m_iThread, this->m_iGenerations, this->m_oBestPop[ i ].getFitness(), fCC, fRmsd, fRmsd2);
	    else 
		sprintf(pOut, "[%02d-%02d-%04d] %8.6f %8.6f\n", this->m_iRun, this->m_iThread, this->m_iGenerations, this->m_oBestPop[ i ].getFitness(), fCC);
	    svtout << pOut ;
	    
	    if (this->m_fStopScore > 0.0 && fCC > this->m_fStopScore)
	    {
		this->m_bDone = true;
		this->m_oTabus.push_back( this->m_oBestPop[ i ] );
	    }
		
 	    if (strlen(m_pPath) != 0 && this->m_iWriteModelInterval != 0 && this->m_iGenerations%this->m_iWriteModelInterval ==0 )
 	    {
 		sprintf(pFname, "%s/Model%02d%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread, i );
		//m_oModel.writePDB( pFname, true );
 	    }
	}
    }
    
    if (strlen(m_pPath) != 0 && bTabuAdded && this->m_oTabus.size()>0 )
    {
	sort(this->m_oTabus.begin(), this->m_oTabus.end());

	if (! (this->m_oTabus[ this->m_oTabus.size() - 1].getWrote()) ) // if this solution was not yet wrote on disk then write
	{
	    updateModel( &this->m_oTabus[ this->m_oTabus.size() - 1] );

	    char pFname[256]; 
	    sprintf(pFname, "%s/Tabu%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
	    m_oModel.writePDB( pFname, true );
	    this->m_oTabus[ this->m_oTabus.size() - 1].setWrote( true );

	    updateVolume( &this->m_oTabus[ this->m_oTabus.size() - 1], false );
	    m_oModelVol.convolve1D3D(m_oKernel, false ); // don't normalize
	    Real64 fCC = m_oModelVol.correlation(m_oTar, false);
      
	    if (this->m_fStopScore > 0.0 && fCC > this->m_fStopScore)
		this->m_bDone = true;
      
	    Real64 fRmsd = -1.0, fRmsd2 = -1.0;
	    if (m_oTarStr.size() > 0)
	    {
		if (m_oTarStr.size() > 0)
		    fRmsd2 = m_oModel.rmsd( m_oTarStr, false, ALL,  false );
		else 
		    fRmsd2 = -1;
		fRmsd =  getRMSD(&this->m_oBestPop[ 0 ]);
	    }	
	    
	    sprintf(pFname, "%s/TabuScores_%02d.tab", m_pPath, this->m_iRun );
	    FILE* pFile = fopen( pFname, "at" );
	    if (m_oTarStr.size() > 0)
		fprintf(pFile,"[%02d-%02d-%04d] %02d %8.6f %8.6f - %8.3f %8.3f\n", this->m_iRun, this->m_iThread, this->m_iGenerations, m_oCountFramesWrote[this->m_iThread], this->m_oTabus[ this->m_oTabus.size() - 1].getFitness(), fCC, fRmsd, fRmsd2);
	    else
		fprintf(pFile,"[%02d-%02d-%04d] %02d %8.6f %8.6f\n", this->m_iRun, this->m_iThread, this->m_iGenerations, m_oCountFramesWrote[this->m_iThread], this->m_oTabus[ this->m_oTabus.size() - 1].getFitness(), fCC);
	    fclose( pFile );
	    
	    m_oCountFramesWrote[this->m_iThread]++;
	}
    }


/*   
    if ( strlen(m_pPath) != 0 && !bTabuAdded && m_iWriteModelInterval != 0 && this->m_iGenerations%m_iWriteModelInterval==0 && this->m_iMaxGen-this->m_iGenerations >= 15 )
    {
            
        sprintf(pFname, "%s/Scores_%02d%02d.tab", m_pPath, this->m_iRun, this->m_iThread );
        FILE* pFile = fopen( pFname, "at" );
        svt_ga<T>::sortPopulation();
        fprintf(pFile,"%02d %02d %04d %8.6f ", this->m_iRun, this->m_iThread, this->m_iGenerations, this->m_oPop[ this->m_oPop.size()-1].getFitness() );
        if (this->m_oTabus.size() > 0 )
            fprintf(pFile,"%8.6f", this->m_oTabus[ this->m_oTabus.size()-1].getFitness() );
        else
            fprintf(pFile,"%8.6f", 0.0f );

        updateModel(&this->m_oPop[ this->m_oPop.size()-1] );
        Real64 fRmsd, fRmsd2;
	if (m_oTarStr.size() > 0)
	    fRmsd2 = m_oModel.rmsd( m_oTarStr, false, ALL,  false );
	else 
	    fRmsd2 = -1;
	    
	fRmsd =  getRMSD(& this->m_oPop[ this->m_oPop.size()-1]);
        fprintf(pFile, " %8.3f %8.3f", fRmsd, fRmsd2 ); 
       
        if (this->m_oTabus.size() > 0 )
        {


        updateModel(&this->m_oTabus[ this->m_oTabus.size()-1] );
	if (m_oTarStr.size() > 0)
	    fRmsd2 = m_oModel.rmsd( m_oTarStr, false, ALL,  false );
	else 
	    fRmsd2 = -1;
	    
	fRmsd =  getRMSD(&this->m_oTabus[ this->m_oTabus.size()-1] );
        fprintf(pFile, " %8.3f %8.3f\n", fRmsd, fRmsd2 );
        }
        else
            fprintf(pFile, " %8.3f %8.3f\n", 0.0, 0.0 );
        

        fclose( pFile );
    }
	
*/
}

/**
 * output the best model
 */
template<class T>
void svt_gamultifit<T>::outputBest()
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
	m_oModel.writePDB( pFname );
    }
}

/**
 * print results (to cout)
 */
template<class T>
void svt_gamultifit<T>::printResults()
{
    for(int i=this->m_oPop.size()-1; i>=0; i--)
    {
        printf("[%3i] = %1d %1d %8.3f", i, this->m_oPop[i].getOrigin(), this->m_oPop[i].getAge(), this->m_oPop[i].getProp() );
	this->m_oPop[i].printGenes();
    }
}

/**
 * compute the location of the center and orientation of z vector when rotated following genes
 */
template<class T>
void svt_gamultifit<T>::writePopGenesInRealSpace()
{
    char pFname[1256];
    unsigned int iSize = this->m_oPop.size();
    svt_ga_mat oMat;
    svt_ga_vec oVec, oNull, oProd;
    oVec.x(0.0);    
    oVec.y(0.0);
    oVec.z(3.0);
    oNull.x(0.0);
    oNull.y(0.0);
    oNull.z(0.0);
    
    m_oGenesInRealSpace.deleteAllAtoms();
    m_oRotation.deleteAllAtoms();
    
    for(unsigned int i=0; i<iSize; i++)
    {
        for (unsigned int iUnit=0; iUnit<m_iNoUnits; iUnit++)
        {
            oMat = calcTransformation( &this->m_oPop[ this->m_oPop.size() - i - 1], iUnit); 
            
            oProd = oMat*oNull;
            m_oGenesInRealSpace.addAtom(*m_oUnits[iUnit].getPdb().getAtom(0), oProd );
            
            oMat.setTranslation(oNull);
            oProd = oMat*oVec;
            m_oRotation.addAtom(*m_oUnits[iUnit].getPdb().getAtom(0), oProd );
        }
    }
    
    if (strlen(m_pPath) != 0)
    {
	sprintf(pFname, "%s/GinRS%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
	m_oGenesInRealSpace.writePDB( pFname, true );
	
	sprintf(pFname, "%s/Rot%02d%02d.pdb", m_pPath, this->m_iRun, this->m_iThread );
	m_oRotation.writePDB( pFname, true );
    }
};


/**
 * write niche results
 */
template<class T>
void svt_gamultifit<T>::outputNicheResults()
{
    vector<int> oCountsPerNiche;
    Real64 fRmsd;    
    char pOut[1024], pFname[1024];
    
    for (int iIndex = 0; iIndex < this->m_iPopSize; iIndex++)
	oCountsPerNiche.push_back(0);;
	
    if (this->m_iReinsertionScheme==REINSERTION_SHARING)
    {

	for (int iIndex =  this->m_iPopSize-1; iIndex >=0 ; iIndex--)
	{
	    if (this->m_oPop[iIndex].getNiche()>0)
	    {
		oCountsPerNiche[ this->m_oPop[iIndex].getNiche() ]++;
		
		if (oCountsPerNiche[this->m_oPop[iIndex].getNiche()] == 1 ) // first individual of the niche
		{
		    updateFitness( &this->m_oPop[iIndex]);
		    fRmsd =  m_oModel.rmsd( m_oTarStr, false, ALL,  false );

		    sprintf(pOut, "[%05d-%04d] %10.6f - ",iIndex, this->m_iGenerations,fRmsd);
		    cout << pOut; this->m_oPop[ iIndex ].printGenes();
		    
		    if (this->m_oPop[iIndex].getNiche()<=10 && this->m_iGenerations == svt_ga<T>::getMaxGen()-2 && strlen(m_pPath) != 0)
		    {
			sprintf(pFname, "%s/Niche%02d.pdb", m_pPath, this->m_oPop[iIndex].getNiche() );
			m_oModel.writePDB( pFname );
		    }
		}
		    
		
	    }
	}
    
	for (int iIndex = 1; iIndex < this->m_iPopSize; iIndex++)
	{
	    if (oCountsPerNiche[ iIndex ] > 0 )
		cout << oCountsPerNiche[ iIndex ] << " ";
	    else 
		iIndex = this->m_iPopSize;
	}
    cout << endl;
   }
    
};

/**
 * Set the output path path
 * \param pPath pointer to array of char
 */
template<class T>
void svt_gamultifit<T>::setOutputPath( const char *pPath )
{
    strcpy(m_pPath, pPath);
};

/**
 * How many generations should gamultifit wait until it outputs a new model file (0 turns output off)
 * \param iWriteModelInterval number of generations
 */
template<class T>
void svt_gamultifit<T>::setWriteModelInterval( unsigned int iWriteModelInterval )
{
    m_iWriteModelInterval = iWriteModelInterval;
};
/**
 * How many generations should gamultifit wait until it outputs a new model file (0 turns output off)
 * \return number of generations
 */
template<class T>
unsigned int svt_gamultifit<T>::getWriteModelInterval( )
{
    return m_iWriteModelInterval;
};

/**
 * output the configuration of the program:w
 *
 */
template<class T>
void svt_gamultifit<T>::writeConfiguration(char *pFnameParam)
{
    svt_ga<T>::writeConfiguration(pFnameParam);
    
    FILE* pFileParam = fopen( pFnameParam, "a" );

    fprintf( pFileParam, "ScoreFunction = %d\n",            getScore() );
    fprintf( pFileParam, "Resolution = %f\n",               getResolution() );
    fprintf( pFileParam, "OutputPath = %s\n",               m_pPath );
    fprintf( pFileParam, "AngularStepSize = %f\n",          getAngularStepSize() );
    fprintf( pFileParam, "PsiFrom = %f\n"  ,                m_fPsiFrom );
    fprintf( pFileParam, "PsiTo = %f\n"    ,                m_fPsiTo );
    fprintf( pFileParam, "ThetaFrom = %f\n",                m_fThetaFrom );
    fprintf( pFileParam, "ThetaTo = %f\n"  ,                m_fThetaTo );
    fprintf( pFileParam, "PhiFrom = %f\n"  ,                m_fPhiFrom );
    fprintf( pFileParam, "PhiTo = %f\n"    ,                m_fPhiTo );    
    fprintf( pFileParam, "XFrom = %f\n"    ,                m_fXFrom );
    fprintf( pFileParam, "XTo = %f\n"      ,                m_fXTo );
    fprintf( pFileParam, "YFrom = %f\n"    ,                m_fYFrom );
    fprintf( pFileParam, "YTo = %f\n"      ,                m_fYTo );
    fprintf( pFileParam, "ZFrom = %f\n"    ,                m_fZFrom );
    fprintf( pFileParam, "ZTo = %f\n"      ,                m_fZTo );
    fprintf( pFileParam, "WriteModelInterval = %i\n",       getWriteModelInterval() );
    fprintf( pFileParam, "ProportionInputStr = %f\n"	,   getPropInputStr());
    fprintf( pFileParam, "PopSizeAroundCenters = %d\n"	,   getPopSizeAroundCenters());

    
    fclose( pFileParam );
};



///////////////////////////////////////////////////////////////////////////////
// run ga in thread 
///////////////////////////////////////////////////////////////////////////////

/**
 * get the transformation matrixes of the best individual
 */
template<class T>
vector< vector<svt_ga_mat> > svt_gamultifit<T>::getBestTransfMat()
{
    vector < vector<svt_ga_mat> >  oAllVec;
    svt_population<T> oPop, oTabuPop;
    T oInd;

    this->m_fTimeGen = 0;
    this->m_iGenerations = this->m_iSyncGen+1;
    for(unsigned int iThread=0; iThread < this->m_oGA_Array.size(); iThread++ )
    {
	if (this->m_oGA_Array[iThread]->getTimeGen() > this->m_fTimeGen)
	    this->m_fTimeGen = this->m_oGA_Array[iThread]->getTimeGen();
	
	if (this->m_oGA_Array[iThread]->getCurrGen() < this->m_iGenerations)
	    this->m_iGenerations = this->m_oGA_Array[iThread]->getCurrGen();

        oInd = this->m_oGA_Array[iThread]->getBest();
	if (oInd.getGeneCount() > 0)
	    oPop.push_back( oInd );
	
        oInd = this->m_oGA_Array[iThread]->getBestTabu();
	if (oInd.getGeneCount() > 0)
	    oTabuPop.push_back( oInd );
    }
    
    if ( this->m_iGenerations == 0 || this->m_iGenerations == m_iGenerationOfLastGetBest) // population was not yet been initialized or no generation has been created
        return oAllVec;
    
    sort(oPop.begin(), oPop.end());
    sort(oTabuPop.begin(), oTabuPop.end());
    
    this->m_oBestPop.clear();
    
    if (oPop.size()>0)
	this->m_oBestPop.push_back( oPop[ oPop.size() - 1] );
    if (oTabuPop.size()>0)
	this->m_oBestPop.push_back( oTabuPop[ oTabuPop.size() - 1] );
    
    if (this->m_oBestPop.size()==0)
	return oAllVec;
 
    sort( this->m_oBestPop.rbegin(), this->m_oBestPop.rend());
    this->m_iBestPopSize = this->m_oBestPop.size();
    
    vector<svt_ga_mat> oVec;
    svt_ga_mat oMat;
    
    for (unsigned int iIndex=0; iIndex < this->m_iBestPopSize; iIndex++)
    {
        oVec.clear();
        
        for (unsigned int iUnit=0; iUnit<m_iNoUnits; iUnit++)
        {
            oMat = calcTransformation(&this->m_oBestPop[ iIndex ], iUnit);
            oVec.push_back(oMat);
        }
       oAllVec.push_back( oVec );
    }
    m_iGenerationOfLastGetBest = this->m_iGenerations;
    return oAllVec;
};

/**
 * Write the top scoring solutions to the disk
 * \param oPop the population of solutions 
 * \param iWriteSolutions how many solutions to write
 */
template<class T>
void svt_gamultifit<T>::writeSolutions(svt_population<T> &oPop, unsigned int iWriteSolutions, char *pFilename)
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
	    sprintf( pStr, "%s/%s_%02i%02i.pdb", m_pPath, pFilename, this->m_iRun, i+1 );
	    
	    updateModel( &oPop[i] );
	    updateVolume( &oPop[i] , false);
	    m_oModel.writePDB( pStr );
	    
	    if (m_oTarStr.size() > 0)
		sprintf( pStr, "  [%02ld] %s/%s_%02i%02li.pdb - Score: %1.5f CC: %1.6f RMSD: %1.6f %1.6f\n", (long unsigned)(oPop.size()-i), m_pPath,pFilename, this->m_iRun, (long unsigned)(oPop.size()-i), oPop[i].getFitness() , getCorrelation(), getRMSD(), getRMSD(&oPop[i]));
	    else
		sprintf( pStr, "  [%02ld] %s/%s_%02i%02li.pdb - Score: %1.5f CC: %1.6f\n", (long unsigned)(oPop.size()-i), m_pPath, pFilename, this->m_iRun, (long unsigned)(oPop.size()-i), oPop[i].getFitness() , getCorrelation() );
	    svtout << pStr;
	}
	svtout << endl;

	
    }
}

#endif
