/***************************************************************************
                          svt_clustering_gng
                          ------------------
    begin                : 02/25/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_CLUSTERING_GNG_H
#define __SVT_CLUSTERING_GNG_H

#include <svt_types.h>
#include <svt_multivec.h>
#include <svt_stlVector.h>
#include <svt_cmath.h>

#include <svt_clustering.h>

/**
 * Growing Neural Gas
 * see Bernd Fritzke
 * \author Stefan Birmanns
 */
template<class T> class svt_clustering_gng : public svt_clustering<T>
{
protected:

    Real64 m_fEpsilonB; // learning rate for the Best
    Real64 m_fEpsilonN; // learning rate for the Next best

    unsigned int m_iMaxNode; // maximum number of nodes

    vector< Real64 > m_aError; // accumulated error
    vector< unsigned int > m_aWin; // time since last win
    vector< bool > m_aDead; // list of dead cells

    Real64 m_fAlpha; // see paper
    Real64 m_fBeta; // see paper

    unsigned int m_iAlphaMax; // maximum age of edge

public:

    /**
     * Constructor
     */
    svt_clustering_gng(unsigned int iMaxNode) : svt_clustering<T>(),
	m_fEpsilonB( 0.05 ),
	m_fEpsilonN( 0.006 ),
	m_iMaxNode( iMaxNode ),
	m_aError( 0 ),
        m_aDead( 0 ),
	m_fAlpha( 0.5 ),
	m_fBeta( 0.0005 ),
        m_iAlphaMax( 100 )
    {
    };
    virtual ~svt_clustering_gng()
    {
    };

    /**
     * set the epsilon parameters
     * \param fEpsilonB the e_b parameter (see paper)
     * \param fEpsilonN the e_n parameter (see paper)
     */
    inline void setEpsilon(double fEpsilonB, double fEpsilonN)
    {
	this->m_fEpsilonB = fEpsilonB;
	this->m_fEpsilonN = fEpsilonN;
    };

    /**
     * set the alpha and beta parameters
     * \param fAlpha alpha parameter (see paper)
     * \param fBeta beta parameter (see paper)
     */
    inline void setAlphaBeta(double fAlpha, double fBeta)
    {
	this->m_fAlpha = fAlpha;
	this->m_fBeta = fBeta;
    };

    /**
     * set the alpha_max parameter
     * \param iAlphaMax alpha_max parameter (see paper)
     */
    inline void setAlphaMax(unsigned int iAlphaMax)
    {
	this->m_iAlphaMax = iAlphaMax;
    };

    /**
     * Cleanup dead cells
     */
    inline void cleanupDeadCells()
    {
        // are there actually dead cells?
        bool bDead = false;

	for(unsigned int i=0; i<this->m_aW.size(); i++)
	    if (this->m_aDead[i] == true)
		bDead = true;

	if (bDead == false)
            return;

        vector< unsigned int > aIndices;

	for(unsigned int i=0; i<this->m_aW.size(); i++)
	{
	    if (this->m_aDead[i] == false)
                aIndices.push_back(i);
	}

	svt_matrix<bool> oC_Conn(aIndices.size(), aIndices.size());
	svt_matrix<unsigned int> oC_Age(aIndices.size(), aIndices.size());

	for(unsigned int i=0; i<aIndices.size(); i++)
	    for(unsigned int j=0; j<aIndices.size(); j++)
	    {
		oC_Conn[i][j] = this->m_oC_Conn[aIndices[i]][aIndices[j]];
		oC_Age[i][j]  = this->m_oC_Age[aIndices[i]][aIndices[j]];
	    }

        this->m_oC_Conn = oC_Conn;
	this->m_oC_Age = oC_Age;

        vector< bool > aDead;
        vector< Real64 > aError;
        vector< unsigned int > aWin;
        svt_point_cloud< T > aW;

 	for(unsigned int i=0; i<aIndices.size(); i++)
	{
            aError.push_back( m_aError[aIndices[i]] );
            aW.addPoint( this->m_aW[aIndices[i]] );
            aDead.push_back( false );
            aWin.push_back( this->m_aWin[aIndices[i]] );
	}

	this->m_aDead = aDead;
        this->m_aError = aError;
        this->m_aWin = aWin;
        this->m_aW = aW;

    };

    /**
     * train the network
     */
    virtual void train(svt_sampled<T>& rObject)
    {
        T oTmp;

        this->m_aW.delAllPoints();

	// add two vectors
        oTmp = rObject.sample(); addVector( oTmp );
        oTmp = rObject.sample(); addVector( oTmp );

        // initialize error vector
        this->m_aError.clear();
        this->m_aError.push_back( 0.0 );
	this->m_aError.push_back( 0.0 );
	// initialize dead cell vector
        this->m_aDead.clear();
        this->m_aDead.push_back( false );
        this->m_aDead.push_back( false );
        // initialize win vector
        this->m_aWin.clear();
        this->m_aWin.push_back( 0 );
        this->m_aWin.push_back( 0 );

        // allocate connection matrix
        this->init();

	// train
	T oData;
        unsigned int i;

        // iterate until number of nodes reached
	while( this->m_aW.size() <= this->m_iMaxNode )
        {
            // reset the number of wins per node
            for(i=0; i < this->m_aW.size(); i++)
                m_aWin[i] = 0;

            this->m_iCount = 0;

	    while( this->m_iCount < this->m_iMaxstep)
	    {
		oData = rObject.sample();
		train( oData );
	    }

            unsigned int iNum = this->m_aW.size();

            // remove all nodes without connections
            double fThres = 2; //( (double)(this->m_iMaxstep) / (double)(iNum) ) * 0.01;
            for(i=0; i<iNum; i++)
                if ((double)(this->m_aWin[i]) < fThres)
                    this->m_aDead[i] = true;
                else
                    this->m_aDead[i] = false;

	    // cleanup all the dead cells
	    cleanupDeadCells();

            if (this->m_aW.size() == iNum)
            {
                //svtout << "Finished iteration with " << this->m_aW.size() << " nodes." << endl;

                //char oStr[256];
                //sprintf(oStr, "iter_%i.pdb", this->m_aW.size() );
                //this->writePDB( oStr );
            }

            // are we done here?
            if (this->m_aW.size() == this->m_iMaxNode)
                return;

	    // which are the nodes with the highest and second highest accumulated error?
	    unsigned int iHighest = 0;
	    unsigned int iSecHigh = 1;
	    Real64 fHighest = 0.0;
            Real64 fSecHigh = 0.0;
	    for(i=0; i<this->m_aW.size(); i++)
	    {
		if (this->m_aError[i] > fHighest)
		{
		    fHighest = this->m_aError[i];
		    iHighest = i;
		}
	    }
	    for(i=0; i<this->m_aW.size(); i++)
	    {
		if (this->m_aError[i] > fSecHigh && i != iHighest)
		{
		    fSecHigh = this->m_aError[i];
		    iSecHigh = i;
		}
	    }

	    // add new node
            T nVec = (this->m_aW[iHighest] - this->m_aW[iSecHigh]) * 0.5;
	    addVector( nVec );
            unsigned int iNew = this->m_aW.size() - 1;

            // connect new node
	    this->m_oC_Conn.resize( this->m_aW.size(), this->m_aW.size(), svt_matrix<bool>::Save );
	    this->m_oC_Conn[iHighest][iNew] = true;
	    this->m_oC_Age.resize( this->m_aW.size(), this->m_aW.size(), svt_matrix<unsigned int>::Save );
	    this->m_oC_Age[iHighest][iNew] = 0;

	    // remove connections between old nodes
	    this->m_oC_Conn[iHighest][iSecHigh] = false;
	    this->m_oC_Conn[iSecHigh][iHighest] = false;

	    // reduce error of highest and sechigh
            this->m_aError[iHighest] -= (this->m_fAlpha * this->m_aError[iHighest]);
	    this->m_aError[iSecHigh] -= (this->m_fAlpha * this->m_aError[iSecHigh]);
	    this->m_aError.push_back( ( this->m_aError[iHighest] + this->m_aError[iSecHigh] ) * 0.5 );
            this->m_aDead.push_back( false );
            this->m_aWin.push_back( 0 );

	    // reduce error of all nodes and set win to 0
            for(i=0; i<this->m_aW.size(); i++)
            {
                this->m_aError[i] -= (this->m_fBeta * this->m_aError[i]);
                this->m_aWin[i] = 0;
            }
	}
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
        Real64 fDist = 0.0;
	Real64 fMinDist = 1.0E10;
	Real64 fSecMinDist = 1.0E10;
	unsigned int iMinDist = 0;
        unsigned int iSecMinDist = 0;
        unsigned int iNum = this->m_aW.size();
	for(i=0; i < iNum; i++)
	{
	    fDist = this->m_aW[i].distance( fData );

	    if (fDist < fMinDist && m_aDead[i] == false)
	    {
		fMinDist = fDist;
		iMinDist = i;
	    }
	    if (fDist < fSecMinDist && fDist > fMinDist && m_aDead[i] == false)
	    {
		fSecMinDist = fDist;
		iSecMinDist = i;
	    }
	}

        // reset the last win counter
        m_aWin[iMinDist] += 1;

	// add connection
        this->m_oC_Conn[iMinDist][iSecMinDist] = true;
        this->m_oC_Age[iMinDist][iSecMinDist] = 0;
        this->m_oC_Conn[iSecMinDist][iMinDist] = true;
        this->m_oC_Age[iSecMinDist][iMinDist] = 0;

	// adjust error of winner
        this->m_aError[iMinDist] += fMinDist;

        // ok, now update winner and second best node
	double fTmp;
        unsigned int iHeight = this->m_aW[iMinDist].height();

        // winner
	for(j=0; j<iHeight; j++)
	{
	    fTmp = fData[j] - this->m_aW[iMinDist][j];
	    this->m_aW[iMinDist][j] = this->m_aW[iMinDist][j] + (this->m_fEpsilonB * fTmp);
	}

        // second best
	for(j=0; j<iHeight; j++)
	{
	    fTmp = fData[j] - this->m_aW[iSecMinDist][j];
	    this->m_aW[iSecMinDist][j] = this->m_aW[iSecMinDist][j] + (this->m_fEpsilonN * fTmp);
	}

	// increase age of all connections emanating from winner and remove all edges older than alpha_max
        for(i=0; i<iNum; i++)
        {
	    if (this->m_oC_Conn[iMinDist][i] == true)
	    {
		this->m_oC_Age[iMinDist][i]++;
		this->m_oC_Age[i][iMinDist]++;

                if (this->m_oC_Age[iMinDist][i] > this->m_iAlphaMax)
                {
                    this->m_oC_Conn[iMinDist][i] = false;
                    this->m_oC_Conn[i][iMinDist] = false;
                }
            }
        }

	// increase counter
	this->m_iCount++;
    };
};

#endif
