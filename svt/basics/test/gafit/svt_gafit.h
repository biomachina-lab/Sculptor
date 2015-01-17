/***************************************************************************
                          svt_gafit
                          ---------
    begin                : 06/23/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_GAFIT
#define __SVT_GAFIT

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
 * \author Stefan Birmanns
 */
class svt_gafit : public svt_ga
{
protected:

    // the model object
    svt_point_cloud_pdb<svt_vector4<Real64> > m_oModel;
    // the scene object
    svt_point_cloud_pdb<svt_vector4<Real64> > m_oScene;

    // neighbor density histogram
    vector< vector<int> > m_oModelNDH;
    vector< vector<int> > m_oSceneNDH;

    // the size of the search space
    svt_vector4<Real64> m_oSearchSpace;
    // the vector to uncenter the model
    svt_vector4<Real64> m_oUnCenter;

    // number of samples used for transformation
    float m_fTransSampl;
    float m_fRotSampl;

public:

    /**
     * Constructor
     */
    svt_gafit() : svt_ga( 6 ),
	m_fTransSampl(0.025),
        m_fRotSampl(0.05)
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
    virtual void initPopulation(int iNum);

    /**
     * set model
     * \param oModel the model
     */
    inline void setModel( svt_point_cloud_pdb< svt_vector4<Real64> > &oModel)
    {
	m_oModel = oModel;
    };
    
    /**
     * set scene
     * \param oScene the model
     */
    inline void setScene( svt_point_cloud_pdb< svt_vector4<Real64> > &oScene)
    {
	m_oScene = oScene;
    };

    /**
     * center model and scene
     */
    void centerModelScene();

    /**
     * calculate transformation matrix
     * \param pInd pointer to individual
     */
    svt_matrix4<Real64> calcTransformation(svt_ga_ind* pInd);

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

};

#endif
