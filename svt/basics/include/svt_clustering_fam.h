/***************************************************************************
                          svt_clustering_fam
                          -------------------
    begin                : 02/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_CLUSTERING_FAM_H
#define __SVT_CLUSTERING_FAM_H

#include <svt_types.h>
#include <svt_multivec.h>
#include <svt_stlVector.h>
#include <svt_cmath.h>

#include <svt_clustering.h>
#include <svt_clustering_trn.h>

/**
 * Topology Representing Networks
 * see Thomas Martinetz, Klaus Schulten, Neural Networks Vol. 7, No.3 pp. 507-522, 1994
 * \author Stefan Birmanns
 */
template<class T> class svt_clustering_fam : public svt_clustering<T>
{
protected:

    // lambda
    double m_fLi;
    double m_fLf;
    // epsilon
    double m_fEi;
    double m_fEf;
    // correction
    double m_fCi;
    double m_fCf;

    // array of distancies
    vector<_cluster_dist> m_aDistance;

    // distance matrix
    svt_matrix<Real64> m_aGraphDistMat;
    svt_matrix<Real64> m_aRealDistMat;

public:

    /**
     * Constructor
     */
    svt_clustering_fam() : svt_clustering<T>(),
	m_fLi( 0.2 ),
	m_fLf( 0.02 ),
	m_fEi( 0.1),
        m_fEf( 0.001 )
    {
    };
    virtual ~svt_clustering_fam()
    {
    };

    /**
     * Set the epsilon parameters -
     * \param fEi the e_i parameter (see paper)
     * \param fEf the e_f parameter (see paper)
     */
    inline void setEpsilon(double fEi, double fEf)
    {
	m_fEi = fEi;
	m_fEf = fEf;
    };

    /**
     * set the lambda parameters
     * \param fLi the l_i parameter (see paper)
     * \param fLf the l_f parameter (see paper)
     */
    inline void setLambda(double fLi, double fLf)
    {
	m_fLi = fLi;
	m_fLf = fLf;
    };

    /**
     * Set the mapping space system
     * \param rOrig
     */
    void setSystem(svt_point_cloud_pdb<T>& rOrig)
    {
        this->m_aW = rOrig;

        this->init();

        unsigned int iCV = this->m_aW.size();
        unsigned int i,j;

        for(i=0; i<iCV; i++)
            for(j=i+1; j<iCV; j++)
                if (rOrig.isBond(i,j))
                    this->m_oC_Conn[i][j] = true;

        rOrig.computePSFGraphDistMat();
        m_aGraphDistMat = rOrig.getGraphDists();

        m_aRealDistMat = rOrig.getDistanceMat();

        for(i=0; i<iCV; i++)
            for(j=i+1; j<iCV; j++)
                if (this->m_oC_Conn[i][j] == false)
                {
                    m_aRealDistMat[i][j] = 0;
                    m_aRealDistMat[j][i] = 0;
                }
    };

    /**
     * train the network
     */
    virtual void train(svt_sampled<T>& rObject)
    {
        Real64 fOldLi = m_fLi;

        m_fLi *= this->m_aW.size();

        //svt_point_cloud<T> oTmp;
        //oTmp = this->m_aW;

	// train
	T oData;
        unsigned int i;

        // now train
        try
        {
            svt_exception::ui()->progressPopup("Training Network", 0, this->m_iMaxstep);
            for(i=0; i< this->m_iMaxstep; i++)
            {
                oData = rObject.sample();
                train( oData );

                if (i%1000 == 0)
                {
                    svt_exception::ui()->progress( i, this->m_iMaxstep);

                    //    oTmp.addTimestep();
                    //    for(j=0;j<this->m_aW.size();j++)
                    //        oTmp[j] = this->m_aW[j];

                }
            }
            svt_exception::ui()->progressPopdown();

        } catch (svt_userInterrupt&)
        {
            svt_exception::ui()->progressPopdown();
	}

        //oTmp.writePDB("debug.pdb");

        //svt_point_cloud_pdb<T> oCodebook = this->getCodebook();
        //svt_matrix<Real64> aResultMat = oCodebook.getDistanceMat();
        //Real64 fSum = 0.0;
        //unsigned int j;
        //for(i=0; i<aResultMat.width();i++)
        //    for(j=0; j<aResultMat.height(); j++)
        //        if (m_aRealDistMat[j][i] != 0.0)
        //            fSum += fabs(aResultMat[j][i] - m_aGraphDistMat[j][i]);
        //printf("DEBUG: fDiff: %f\n", fSum );

        m_fLi = fOldLi;
    };

protected:

    /**
     * training, input a new vector from the data and the codebook vectors will get adjusted according to the TRN algo
     * \param fData the data vector
     */
    void train(T& fData)
    {
	unsigned int i,j,k;

        // find closest cluster
	double fDist = 1.0E10;
	double fMinDist = fDist;
        int iWinner = -1;
        for(i=0; i<this->m_aW.size(); i++)
        {
            fDist = this->m_aW[i].distance( fData );

            if ( fDist < fMinDist )
            {
                fMinDist = fDist;
                iWinner = i;
            }
        }
        if ( iWinner == -1 || fMinDist > 12.0 )
            return;

	// update lambda and epsilon
        Real64 fCountMaxstep = (Real64)(this->m_iCount) / (Real64)(this->m_iMaxstep);
	Real64 fEpsilon = m_fEi * pow( m_fEf/m_fEi, fCountMaxstep );
	Real64 fLambda  = m_fLi * pow( m_fLf/m_fLi, fCountMaxstep );

	// update the codebook vectors
	Real64 fFact, fTmp;

        // ok, now update..
        unsigned int iNum = this->m_aW.size();
        unsigned int iHeight = this->m_aW[0].height();
	for (i=0; i<iNum; i++)
        {
            // calculate the direction of the update, depending on the stimulus and the distances to the other nodes
            T fForce;

            for (k=0; k<iNum; k++)
            {
                if (k!=i)
                {
                    fDist = this->m_aRealDistMat[k][i];
                    if (fDist > 0.0)
                    {
                        fDist -= this->m_aW[i].distance( this->m_aW[k] );

                        T fDiffVec = this->m_aW[i] - this->m_aW[k];
                        fDiffVec.normalize();

                        for(j=0; j<iHeight; j++)
                        {
                            fForce[j] = fForce[j] + fDist * fDiffVec[j];
                        }
                    }
                }
            }

            fFact = fEpsilon * exp(-this->m_aGraphDistMat[iWinner][i] / fLambda);

	    for(j=0; j<iHeight; j++)
	    {
		fTmp = fData[j] - this->m_aW[i][j];
		this->m_aW[i][j] = this->m_aW[i][j] + (fEpsilon*fForce[j]) + (fFact * fTmp);
	    }
	}

	// increase counter
	this->m_iCount++;
    };
};

#endif
