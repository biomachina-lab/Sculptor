/***************************************************************************
                          svt_clustering_irank
                          ------------------
    begin                : 02/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_CLUSTERING_IRANK_H
#define __SVT_CLUSTERING_IRANK_H

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
template<class T> class svt_clustering_irank : public svt_clustering<T>
{
protected:

    // lambda
    double m_fLi;
    double m_fLf;
    // epsilon
    double m_fEi;
    double m_fEf;
    // time
    double m_fTf;
    double m_fTi;

    // should the connections get calculated online (like in TRN) or do you want to do it later, in a postprocessing step (default: true = online)
    bool m_bConnOnline;

    // array of distancies
    vector<_cluster_dist> m_aDistance;

public:

    /**
     * Constructor
     */
    svt_clustering_irank() : svt_clustering<T>(),
	m_fLi( 0.2 ),
	m_fLf( 0.02 ),
	m_fEi( 0.1),
	m_fEf( 0.001 ),
	m_fTf( 2.0 ),
	m_fTi( 0.1 ),
	m_bConnOnline( true )
    {
    };
    virtual ~svt_clustering_irank()
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
     * should the connections get calculated online (like in TRN) or do you want to do it later, in a postprocessing step
     * \param bConnOnline bool variable, if true then the calculations are done online
     */
    inline void setConnOnline(bool bConnOnline)
    {
	m_bConnOnline = bConnOnline;
    };

    /**
     * should the connections get calculated online (like in TRN) or do you want to do it later, in a postprocessing step
     * \return if true then the calculations are done online
     */
    inline bool getConnOnline() const
    {
	return m_bConnOnline;
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
     * train the network
     */
    virtual void train(svt_sampled<T>& rObject)
    {
        this->init();

        Real64 fOldTf = m_fTf;
        Real64 fOldTi = m_fTi;
        Real64 fOldLi = m_fLi;

	m_fTf *= this->m_aW.size();
	m_fTi *= this->m_aW.size();
        m_fLi *= this->m_aW.size();

	// train
	T oData;
        unsigned int i;

        m_aDistance.clear();
	for(i=0; i<this->m_aW.size(); i++)
	    m_aDistance.push_back(_cluster_dist(i, 0.0));

	for(i=0; i< this->m_iMaxstep; i++)
	{
	    oData = rObject.sample();

	    train( oData );
	}

	m_fTf = fOldTf;
        m_fTi = fOldTi;
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
        unsigned int iNum = this->m_aW.size();

	// calculate and sort the distances
	Real64 fMinDist = 1.0E10;
	Real64 fMaxDist = 0.0;
        Real64 fDist = 0.0;
	for(i=0; i < iNum; i++)
	{
            fDist = this->m_aW[i].distance( fData );
	    m_aDistance[i] = _cluster_dist(i, fDist);
	    if (fDist > fMaxDist)
		fMaxDist = fDist;
	    if (fDist < fMinDist)
		fMinDist = fDist;
	}

        Real64 fCountMaxstep = (Real64)(this->m_iCount) / (Real64)(this->m_iMaxstep);

	// update lambda and epsilon
	double fEpsilon = m_fEi * pow( m_fEf/m_fEi, fCountMaxstep );
	double fLambda =  (m_fLi * pow( m_fLf/m_fLi, fCountMaxstep )) / (Real64)(iNum-1);

	// update the codebook vectors
	unsigned int iIndex;
	double fFact, fTmp;

        // ok, now update..
	unsigned int iHeight;
        Real64 fM = 0.0;
	for (i=0; i<iNum; i++)
	{
	    iIndex = m_aDistance[i].getIndex();
	    fM = (m_aDistance[i].getDist() - fMinDist) / (fMaxDist - fMinDist);

	    fFact = fEpsilon * exp(-fM/fLambda);

            iHeight = this->m_aW[iIndex].height();
	    for(j=0; j<iHeight; j++)
	    {
		fTmp = fData[j] - this->m_aW[iIndex][j];
		this->m_aW[iIndex][j] = this->m_aW[iIndex][j] + (fFact * fTmp);
	    }
	}

	// increase counter
	this->m_iCount++;
    };

    /**
     * calculate connections in a postprocessing step. You have to call this function for every data vector!!
     * \param fData svt_multivec data vector
     */
    void calcConnections(T& fData)
    {
	unsigned int i;

	// calculate and sort the distances
	for(i=0; i < this->m_aW.size(); i++)
	    m_aDistance[i] = _cluster_dist(i, this->m_aW[i].distance( fData ));
	sort( m_aDistance.begin(), m_aDistance.end() );

	int iWinner = m_aDistance[0].getIndex();
	int iSecond = m_aDistance[1].getIndex();
	this->m_aC[iWinner].aConn[iSecond] = true;
	this->m_aC[iWinner].aAge[iSecond] = 0;
    };

};

#endif
