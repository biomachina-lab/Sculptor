/***************************************************************************
                          svt_clustering_trn
                          ------------------
    begin                : 02/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_CLUSTERING_TRN_H
#define __SVT_CLUSTERING_TRN_H

#include <svt_types.h>
#include <svt_multivec.h>
#include <svt_stlVector.h>
#include <svt_cmath.h>

#include <svt_clustering.h>

/**
 * distance sorting helper class
 */
class _cluster_dist
{
protected:

    double m_fDist;
    int m_iIndex;

public:

    _cluster_dist(int iIndex, double fDist)
    {
	m_iIndex = iIndex;
	m_fDist = fDist;
    };

    inline double getDist() const
    {
	return m_fDist;
    };
    inline int getIndex() const
    {
	return m_iIndex;
    };

    inline bool operator<(const _cluster_dist& rD) const
    {
        return m_fDist < rD.m_fDist;
    };

};

/**
 * Topology Representing Networks
 * see Thomas Martinetz, Klaus Schulten, Neural Networks Vol. 7, No.3 pp. 507-522, 1994
 * \author Stefan Birmanns
 */
template<class T> class svt_clustering_trn : public svt_clustering<T>
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
    svt_clustering_trn() : svt_clustering<T>(),
	m_fLi( 0.2 ),
	m_fLf( 0.02 ),
	m_fEi( 0.1),
	m_fEf( 0.001 ),
	m_fTf( 10.0 ),
	m_fTi( 1.0 ),
	m_bConnOnline( true )
    {
    };
    virtual ~svt_clustering_trn()
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
     * set the t parameters
     * \param fTi the t_i parameter (see paper)
     * \param fTf the t_f parameter (see paper)
     */
    inline void setT(double fTi, double fTf)
    {
	m_fTi = fTi;
	m_fTf = fTf;
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

        m_fLi *= this->m_aW.size();
        m_fTi *= this->m_iMaxstep;
        m_fTf *= this->m_iMaxstep;

	// train
	T oData;
        unsigned int i;

        m_aDistance.clear();
	for(i=0; i<this->m_aW.size(); i++)
	    m_aDistance.push_back(_cluster_dist(i, 0.0));

        // calculate the average training time and update the busy indicator every second
        long iTime = 0;

        try {
            svt_exception::ui()->busyPopup("Feature Point Extraction");

            for(i=0; i< this->m_iMaxstep; i++)
            {
                oData = rObject.sample();
                train( oData );

                if (svt_getToD() - iTime > 1000)
                {
                    int iPerc = (int)((((float)(i) / (float)(this->m_iMaxstep)) * 100.0));
                    svt_exception::ui()->busyRotate( iPerc );
                    iTime = svt_getToD();
                }
            }

        }
        catch (svt_userInterrupt& ) {

            throw svt_userInterrupt();
        }

        svt_exception::ui()->busyPopdown();

        m_fTi /= this->m_iMaxstep;
        m_fTf /= this->m_iMaxstep;
        m_fLi /= this->m_aW.size();
    };

protected:

    /**
     * training, input a new vector from the data and the codebook vectors will get adjusted according to the TRN algo
     * \param fData the data vector
     */
    void train(T& fData)
    {
	unsigned int i,j;

	// calculate and sort the distances
	for(i=0; i < this->m_aW.size(); i++)
	    m_aDistance[i] = _cluster_dist(i, this->m_aW[i].distance( fData ));
	sort( m_aDistance.begin(), m_aDistance.end() );

        Real64 fCountMaxstep = (Real64)(this->m_iCount) / (Real64)(this->m_iMaxstep);

	// connections?
	if (m_bConnOnline && m_aDistance.size()>1)
        {
            // calculate maximal age
            double fT = m_fTi * pow( m_fTf/m_fTi, fCountMaxstep);
            // which node is winner, which is second winner?
	    unsigned int iWinner = m_aDistance[0].getIndex();
            unsigned int iSecond = m_aDistance[1].getIndex();

            // refresh edge between winner and second winner
	    this->m_oC_Conn[iWinner][iSecond] = true;
            this->m_oC_Age[iWinner][iSecond] = 0;
            // increase ages of all other edges and remove edges with age > maximal age
	    for(i=0; i<this->m_aW.size(); i++)
	    {
		if (i != iSecond)
		{
		    this->m_oC_Age[iWinner][i] += 1;
		    if ((double)(this->m_oC_Age[iWinner][i]) > fT)
			this->m_oC_Conn[iWinner][i] = false;
		}
	    }
        }

	// update lambda and epsilon
	double fEpsilon = m_fEi * pow( m_fEf/m_fEi, fCountMaxstep );
	double fLambda =  m_fLi * pow( m_fLf/m_fLi, fCountMaxstep );

	// update the codebook vectors
	unsigned int iIndex;
	double fFact, fTmp;

        // ok, now update..
        unsigned int iNum = this->m_aW.size();
	unsigned int iHeight;
	for (i=0; i<iNum; i++)
	{
	    iIndex = m_aDistance[i].getIndex();
            fFact = fEpsilon * exp(-(Real64)(i)/fLambda);

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

public:

    /**
     * train the network to form connections - this has to be done after the main training, it is assumed the codebook vectors are already in their final positions
     */
    void calcConnections(svt_sampled<T>& rObject)
    {
        this->m_oC_Conn.resize( this->m_aW.size(), this->m_aW.size(), svt_matrix<bool>::Clear, false );
        this->m_oC_Age.resize( this->m_aW.size(), this->m_aW.size(), svt_matrix<unsigned int>::Clear, 0 );

        this->m_oC_Conn = false;
        this->m_oC_Age = 0;
	unsigned int i;
        unsigned int iNum = this->m_aW.size();
        T fData;
        m_fTi *= this->m_iMaxstep;
        m_fTf *= this->m_iMaxstep;

        for(this->m_iCount=0; this->m_iCount< this->m_iMaxstep; this->m_iCount++)
        {
	    fData = rObject.sample();

            // calculate and sort the distances
            for(i=0; i < iNum; i++)
                m_aDistance[i] = _cluster_dist(i, this->m_aW[i].distance( fData ));
            sort( m_aDistance.begin(), m_aDistance.end() );

            Real64 fCountMaxstep = (Real64)(this->m_iCount) / (Real64)(this->m_iMaxstep);

            // calculate maximal age
            double fT = m_fTi * pow( m_fTf/m_fTi, fCountMaxstep);
            // which node is winner, which is second winner?
            unsigned int iWinner = m_aDistance[0].getIndex();
            unsigned int iSecond = m_aDistance[1].getIndex();

            // refresh edge between winner and second winner
            this->m_oC_Conn[iWinner][iSecond] = true;
            this->m_oC_Age[iWinner][iSecond] = 0;
            // increase ages of all other edges and remove edges with age > maximal age
            for(i=0; i<iNum; i++)
            {
                if (i != iSecond)
                {
                    this->m_oC_Age[iWinner][i] += 1;
                    if ((double)(this->m_oC_Age[iWinner][i]) > fT)
                        this->m_oC_Conn[iWinner][i] = false;
                }
            }
        }

        m_fTi /= this->m_iMaxstep;
        m_fTf /= this->m_iMaxstep;
    };
};

#endif
