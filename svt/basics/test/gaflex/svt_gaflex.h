/***************************************************************************
                          svt_gaflex
                          ---------
    begin                : 07/01/2008
    author               : Mirabela Rusu    
    email                : sculptor@biomachina.org
***************************************************************************/

#ifndef __SVT_GAFLEX
#define __SVT_GAFLEX

// internal classes
#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_pdb.h>
#include <svt_ga.h>

#include <math.h>

// stl
#include <vector>
using namespace std;

/**
 * GA-based fitting algorithm
 * \author Mirabela Rusu
 */
class svt_gaflex : public svt_ga
{
protected:

    // input data
    svt_point_cloud_pdb<svt_vector4<Real64> > m_oProbe;
    svt_point_cloud_pdb<svt_vector4<Real64> > m_oTarget;
    svt_point_cloud<svt_vector4<Real64> > m_oFlexed;
    
    // the size of the search space:  
    svt_vector4<Real64> m_oSearchSpace;
    
    // Sampling
    Real64 m_fSampl;
    
    // skeleton matrix 
    svt_matrix<Real64> m_oDistMat;
    
    // the longitudinal stretch of the skeleton connections
    Real64 m_fConnStretch;

public:

    /**
     * Constructor
     */
    svt_gaflex( int m_iGenes ) : svt_ga( m_iGenes ),
	m_fSampl(0.025)
    {
    };

    /**
     * update adaptive fitness
     */
    virtual void updateAdaptive();
    /**
     * generate initial population
     * \param iNum number of inds in this population
     */
    virtual void initPopulation(int iNum );

    /**
     * set probe
     * \param oProbe the probe
     */
    inline void setProbe( svt_point_cloud_pdb< svt_vector4<Real64> > &oProbe)
    {
	m_oProbe = oProbe;
    };
    
    /**
     * set target
     * \param oTarget the probe
     */
    inline void setTarget( svt_point_cloud_pdb< svt_vector4<Real64> > &oTarget)
    {
	m_oTarget = oTarget;
    };
    
    /**
     * set the longitudinal stretch of the skeleton bonds
     * \param fConnStretch the stretch
     */
    inline void setConnStretch( Real64 fConnStretch )
    {
	m_fConnStretch = fConnStretch;
    };

    /**
     * center probe and target
     */
    void centerProbeTarget();
    
    /**
     * set search space 
     * \param fRadiusSearchSpace 
     */
    inline void setSearchSpace( Real64 fRadius )
    {
        m_oSearchSpace = fRadius;
    };
    
    /**
     * calculate Coordinate
     * \param pInd pointer to individual
     * \param iIndex the index of the coordinate to be computed
     */
    svt_vector4<Real64> calcCoordinate(svt_ga_ind* pInd, int iIndex);
    
    /**
     * calculate coordinates
     * \param pInd pointer to individual
     */
    void calcCoordinates(svt_ga_ind* pInd);

    /**
     * update fitness
     * \param pInd pointer to individual that gets updated
     */
    virtual void updateFitness(svt_ga_ind* pInd);

    /**
     * output result
     */
    virtual void outputResult();

    /**
     * print results (to cout)
     */
    void printResults();
    
    /**
     * 
     */
    virtual bool isValid(svt_ga_ind* pInd);
        
    /**
    * Compute the Skeleton matrix
    */
    void calcDistMat(Real64 fDiscardValue = 0);

};

#endif
