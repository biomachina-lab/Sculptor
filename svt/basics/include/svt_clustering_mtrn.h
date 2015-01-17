/***************************************************************************
                          svt_clustering_mtrn
                          -------------------
    begin                : 02/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_CLUSTERING_MTRN_H
#define __SVT_CLUSTERING_MTRN_H

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
template<class T> class svt_clustering_mtrn : public svt_clustering<T>
{
protected:

    // codebook vectors - mapping space
    vector< svt_point_cloud< T > > m_aOrig;
    // codebook vectors - real space
    vector< svt_point_cloud< T > > m_aLearn;

    // lambda
    double m_fLi;
    double m_fLf;
    // epsilon
    double m_fEi;
    double m_fEf;

    // array of distancies
    vector<_cluster_dist> m_aDistance;

public:

    /**
     * Constructor
     */
    svt_clustering_mtrn() : svt_clustering<T>(),
	m_fLi( 0.2 ),
	m_fLf( 0.02 ),
	m_fEi( 0.1),
	m_fEf( 0.001 )
    {
    };
    virtual ~svt_clustering_mtrn()
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
     * return the index of the codebook vector with the minimal distance to another vector
     * \param rData svt_multivec to which the distance is computed
     * \return index of the codebook vector that is closest to rData
     */
    int getMinDistance(T& rData)
    {
	double fDist = 1.0E10;
	double fTemp;
	int iIndex = -1;
	for(unsigned int i=0; i < this->m_aW.size(); i++)
	{
	    fTemp = this->m_aW[i].distance( rData );
	    if (fTemp < fDist)
	    {
		fDist = fTemp;
		iIndex = i;
	    }
	}

	return iIndex;
    };

    /**
     * add a codebook vector. Each call will add a single codebook vector and initialize it with the provided svt_vector object
     * \param fOrig
     * \param fLearn
     */
    void addSystem(svt_point_cloud<T> oOrig, svt_point_cloud<T> oLearn)
    {
        m_aOrig.push_back( oOrig );
        m_aLearn.push_back( oLearn );
    };

    /**
     * get all codebook vectors
     * \return stl vector with all the svt_vector objects
     */
    svt_point_cloud< T >& getCodebook(unsigned int i)
    {
        return m_aLearn[i];
    };

    /**
     * train the network
     */
    virtual void train(svt_sampled<T>& rObject)
    {
        this->init();

        Real64 fOldLi = m_fLi;

        m_fLi *= m_aOrig[0].size();

	// train
	T oData;
        unsigned int i;

        m_aDistance.clear();
        for(i=0; i<this->m_aOrig[0].size(); i++)
            m_aDistance.push_back(_cluster_dist(i, 0.0));

        // now train
        for(i=0; i< this->m_iMaxstep; i++)
        {
            oData = rObject.sample();
            train( oData );
        }

        m_fLi = fOldLi;
    };

protected:

    /**
     * training, input a new vector from the data and the codebook vectors will get adjusted according to the TRN algo
     * \param fData the data vector
     */
    void train(T& fData)
    {
	unsigned int i,j;

        // find closest cluster
	double fDist = 1.0E10;
	double fMinDist = fDist;
        int iWinner = -1;
        unsigned int iAtom = 0;
        for(i=0; i<this->m_aOrig.size(); i++)
            for(j=0; j<this->m_aOrig[i].size(); j++)
            {
                fDist = this->m_aLearn[i][j].distance( fData );

                if ( fDist < fMinDist )
                {
                    fMinDist = fDist;
                    iWinner = i;
                    iAtom = j;
                }
            }
        if (iWinner == -1)
            return;

        // calculate and sort the distances
        //for(i=0; i<this->m_aOrig[iWinner].size(); i++)
	//    m_aDistance[i] = _cluster_dist(i, this->m_aLearn[iWinner][i].distance( fData ) );
	//sort( m_aDistance.begin(), m_aDistance.end() );

        Real64 fCountMaxstep = (Real64)(this->m_iCount) / (Real64)(this->m_iMaxstep);

	// update lambda and epsilon
	double fEpsilon = m_fEi * pow( m_fEf/m_fEi, fCountMaxstep );
	double fLambda =  m_fLi * pow( m_fLf/m_fLi, fCountMaxstep );

	// update the codebook vectors
	double fFact, fTmp;

        // ok, now update..
        unsigned int iNum = this->m_aLearn[iWinner].size();
        unsigned int iHeight = this->m_aLearn[iWinner][0].height();
        unsigned int iIndex;
	for (i=0; i<iNum; i++)
	{
	    iIndex = i;//m_aDistance[i].getIndex();
            //fFact = fEpsilon * exp(-(Real64)(i+1)/fLambda);

            fFact = fEpsilon * exp(-(this->m_aOrig[iWinner][iAtom].distanceSq( this->m_aOrig[iWinner][iIndex] )) / fLambda);

	    for(j=0; j<iHeight; j++)
	    {
                //fTmp = this->m_aOrig[iWinner][m_aDistance[0].getIndex()][j] - this->m_aOrig[iWinner][iIndex][j];
		fTmp = fData[j] - (this->m_aLearn[iWinner])[iIndex][j];
		(this->m_aLearn[iWinner])[iIndex][j] = this->m_aLearn[iWinner][iIndex][j] + (fFact * fTmp);
	    }
	}

	// increase counter
	this->m_iCount++;
    };
};

#endif
