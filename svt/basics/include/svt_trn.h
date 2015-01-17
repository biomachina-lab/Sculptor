/***************************************************************************
                          svt_trn
                          -------
    begin                : 07/05/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_TRN
#define __SVT_TRN

#include <svt_types.h>
#include <svt_multivec.h>
#include <svt_stlVector.h>
#include <svt_cmath.h>

/**
 * distance sorting helper class
 */
class _dist
{
protected:

    double m_fDist;
    int m_iIndex;

public:

    _dist(int iIndex, double fDist)
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

    inline bool operator<(const _dist& rD) const
    {
        return m_fDist < rD.m_fDist;
    };

};

/**
 * connection helper struct for online topology calculation
 */
typedef struct
{
    vector<bool> aConn;
    vector<int> aAge;
} connection;

/**
 * Topology Representing Networks
 * see Thomas Martinetz, Klaus Schulten, Neural Networks Vol. 7, No.3 pp. 507-522, 1994
 * \author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_trn
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
    // wrng
    double m_fK;

    // number of updates so far
    int m_iCount;
    // number of maximal updates
    int m_iMaxstep;

    // codebook vectors
    vector< T > m_aW;
    // topology
    vector< connection > m_aC;

    // should the connections get calculated online (like in TRN) or do you want to do it later, in a postprocessing step (default: true = online)
    bool m_bConnOnline;

public:

    /**
     * Constructor
     */
    svt_trn() :
	m_fLi( 0.2 ),
	m_fLf( 0.02 ),
	m_fEi( 0.1),
	m_fEf( 0.001 ),
	m_fTf( 2.0 ),
	m_fTi( 0.1 ),
	m_fK( 0.0 ),
	m_iCount( 1 ),
	m_iMaxstep( 1000 ),
	m_bConnOnline( true )
    {
    };

    /**
     * set the epsilon parameters
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
     * set the kappa parameters
     * \param fK the kappa parameter (see WRNG paper)
     */
    inline void setKappa(double fK)
    {
        m_fK = fK;
    };
    /**
     * get the kappa parameters
     * \return the kappa parameter (see WRNG paper)
     */
    inline double getKappa() const
    {
        return m_fK;
    };

    /**
     * set the number of steps the algorithm should run
     * \param iMaxstep maximum number of iterations
     */
    inline void setMaxstep( int iMaxstep )
    {
	m_iMaxstep = iMaxstep;
    };
    /**
     * get the number of steps the algorithm should run
     * \return maximum number of iterations
     */
    inline int getMaxstep(  )
    {
	return m_iMaxstep;
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
     * get the current iteration number
     * \return current iteration
     */
    inline int getStep()
    {
	return m_iCount;
    };

    /**
     * add a codebook vector. Each call will add a single codebook vector and initialize it with the provided svt_vector object
     * \param fData
     */
    void addVector(T& fData)
    {
	m_aW.push_back( fData );
    };

    /**
     * initialize TRN algorithm
     */
    void init()
    {
	for(unsigned int i=0; i<m_aW.size(); i++)
	{
	    connection oConn;
	    for(unsigned int j=0; j<m_aW.size(); j++)
	    {
		oConn.aConn.push_back( false );
		oConn.aAge.push_back( 0 );
	    }
	    m_aC.push_back( oConn );
	}

	m_fTf *= m_aW.size();
	m_fTi *= m_aW.size();

	m_iCount = 1;
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
	for(unsigned int i=0; i < m_aW.size(); i++)
	{
	    fTemp = m_aW[i].distance( rData );
	    if (fTemp < fDist)
	    {
		fDist = fTemp;
		iIndex = i;
	    }
	}

	return iIndex;
    };

    /**
     * training, input a new vector from the data and the codebook vectors will get adjusted according to the TRN algo
     * \param fData the data vector
     */
    void train(T& fData)
    {
	unsigned int i,j;

	// calculate and sort the distances
	vector<_dist> aDistance;
	for(i=0; i < m_aW.size(); i++)
	    aDistance.push_back(_dist(i, m_aW[i].distance( fData )));
	sort( aDistance.begin(), aDistance.end() );

	//for(i=0; i < m_aW.size(); i++)
	//    cout << aDistance[i].getDist() << endl;

        //getchar();

	// connections?
	if (m_bConnOnline)
	{
	    double fT = m_fTi * exp(((double)(m_iCount)/(double)(m_iMaxstep)) * log(m_fTf/m_fTi));
	    unsigned int iWinner = aDistance[0].getIndex();
	    unsigned int iSecond = aDistance[1].getIndex();
	    m_aC[iWinner].aConn[iSecond] = true;
	    m_aC[iWinner].aAge[iSecond] = 0;
	    for(i=0; i<m_aW.size(); i++)
	    {
		if (i != iSecond)
		{
		    m_aC[iWinner].aAge[i] += 1;
		    if (m_aC[iWinner].aAge[i] > fT)
			m_aC[iWinner].aConn[i] = false;
		}
	    }
	}

	// update lambda and epsilon
	double fEpsilon = m_fEi * exp(((double)(m_iCount)/(double)(m_iMaxstep)) * log(m_fEf/m_fEi));
	double fLambda =  m_fLi * exp(((double)(m_iCount)/(double)(m_iMaxstep)) * log(m_fLf/m_fLi));

	// update the codebook vectors
	unsigned int iIndex;
	double fFact, fFactOE, fTmp;
	// additional sum for the winner relaxing ng algorithm
	static T oSum(m_aW[0].height());
	if (m_fK != 0.0)
	    for(j=0; j<m_aW[0].height(); j++)
		oSum[j] = 0;

	// ok, now update..
	for (i=m_aW.size()-1; i>0; i--)
	{
	    iIndex = aDistance[i].getIndex();
	    fFactOE = exp(-(double)(i)/fLambda);
	    fFact = fEpsilon * fFactOE;

	    if (fFactOE < 1.0E-10)
		fFactOE = 0;

	    for(j=0; j<m_aW[iIndex].height(); j++)
	    {
		fTmp = fData[j] - m_aW[iIndex][j];

		m_aW[iIndex][j] = m_aW[iIndex][j] + (fFact * fTmp);

		if (m_fK != 0.0 && fFactOE != 0.0)
		    oSum[j] = oSum[j] + (fFactOE * fTmp);
	    }
	}
	// ok, the winner is a special case, becase of the WRNG algorithm (not used if m_fK == 0.0)
	iIndex = aDistance[i].getIndex();
	fFactOE = 1.0;
	fFact = fEpsilon * fFactOE;
	if (m_fK != 0.0 && fFactOE != 0.0)
	{
	    for(j=0; j<m_aW[iIndex].height(); j++)
		oSum[j] = oSum[j] + (fFactOE * (fData[j] - m_aW[iIndex][j]));
	}
	for(j=0; j<m_aW[iIndex].height(); j++)
	{
	    m_aW[iIndex][j] = m_aW[iIndex][j] +
		(fFact * (fData[j] - m_aW[iIndex][j])) +
		((m_fK * (fData[j] - m_aW[iIndex][j])) - (m_fK * oSum[j]));
	}

	// increase counter
	m_iCount++;
    };

    /**
     * get all codebook vectors
     * \return stl vector with all the svt_vector objects
     */
    inline vector<T> getCodebook() const
    {
	return m_aW;
    };

    /**
     * get number of codebook vectors
     * \return number of codebook vectors
     */
    inline unsigned int getCodebookSize() const
    {
	return m_aW.size();
    };

    /**
     * get a single codebook vector
     * \param i index of codebook vector
     * \return reference to svt_multivec object
     */
    inline T& getVector(int i) const
    {
	return (T&)m_aW[i];
    };

    /**
     * calculate connections in a postprocessing step. You have to call this function for every data vector!!
     * \param fData svt_multivec data vector
     */
    void calcConnections(T& fData)
    {
	unsigned int i;

	// calculate and sort the distances
	vector<_dist> aDistance;
	for(i=0; i < m_aW.size(); i++)
	    aDistance.push_back(_dist(i, m_aW[i].distance( fData )));
	sort( aDistance.begin(), aDistance.end() );

	int iWinner = aDistance[0].getIndex();
	int iSecond = aDistance[1].getIndex();
	m_aC[iWinner].aConn[iSecond] = true;
	m_aC[iWinner].aAge[iSecond] = 0;
    };

    /**
     * dump all codebook vectors to the console (cout)
     */
    void dump()
    {
	unsigned int i,j;

	for (i=0; i<m_aW.size(); i++)
	    m_aW[i].dump();

	for (i=0; i<m_aW.size(); i++)
	{
	    cout << i << " connected to: ";
	    for (j=0; j<m_aW.size(); j++)
		if (m_aC[i].aConn[j] == true)
		    cout << j << " ";
	    cout << endl;
	}
    };

    /**
     * Output the topology as csv file. First column gives the index of a codebook vector and then the following numbers are the indices of the vectors it is connected to.
     * \param pFname pointer to char with the filename
     */
    void outputTopology(char* pFname)
    {
	FILE* pFile = fopen(pFname, "w");

	if (pFile == NULL)
	    exit(1);

	for(unsigned int i=0; i<m_aW.size(); i++)
	{
	    // output index of current vector
	    fprintf( pFile, "%i", i );

	    // and now the indices of the other vectors it is connected to
	    for(unsigned int j=0; j<m_aW.size(); j++)
		if (m_aC[i].aConn[j] == true)
		    fprintf( pFile, ", %i", j );

	    fprintf( pFile, "\n" );
	}

	fclose( pFile );
    };

    /**
     * Output the codebook vectors as a PDB file
     * \param pFname pointer to char with the filename
     */
    void outputPDB(char* pFname)
    {
	FILE* pFile = fopen(pFname, "w");

	if (pFile == NULL)
	    exit(1);

	// now transform the model and output the vectors as pdb file
	T oVec( 2 );
	for(unsigned int i=0; i<m_aW.size(); i++)
	{
	    fprintf(pFile, "ATOM  %5i %-4s%c%3s %c%4i%c   %8.3f%8.3f%8.3f%6.2f%6.2f %3s  %4s%2s%2s\n",
		    0,
		    "QPDB",
		    'Q',
		    "001",
		    'A',
		    0,
		    'Q',
		    m_aW[i][0],
		    m_aW[i][1],
		    0.0f,
		    0.0f,
		    0.0f,
		    "QPDB",
		    "QPDB",
		    "QPDB",
		    "QPDB"
		   );
	}

	fclose(pFile);
    };
};

#endif
