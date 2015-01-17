/***************************************************************************
                          svt_trn
                          -------
    begin                : 07/05/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_trn.h>
#include <svt_iostream.h>
#include <svt_stlAlgorithm.h>

/**
 * Constructor
 */
svt_trn::svt_trn() :
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
 * add a codebook vector. Each call will add a single codebook vector and initialize it with the provided svt_vector object
 * \param fData
 */
void svt_trn::addVector(svt_multivec& fData)
{
    m_aW.push_back( fData );
}

/**
 * initialize TRN algorithm
 */
void svt_trn::init()
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
}

/**
 * return the index of the codebook vector with the minimal distance to another vector
 * \param rData svt_multivec to which the distance is computed
 * \return index of the codebook vector that is closest to fData
 */
int svt_trn::getMinDistance(svt_multivec& rData)
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
void svt_trn::train(svt_multivec& fData)
{
    unsigned int i,j;

    // calculate and sort the distances
    vector<_dist> aDistance;
    for(i=0; i < m_aW.size(); i++)
        aDistance.push_back(_dist(i, m_aW[i].distance( fData )));
    sort( aDistance.begin(), aDistance.end() );

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
    static svt_multivec oSum(m_aW[0].getDimension());
    if (m_fK != 0.0)
	for(j=0; j<m_aW[0].getDimension(); j++)
	    oSum.setCoord(j,0);

    // ok, now update..
    for (i=m_aW.size()-1; i>0; i--)
    {
	iIndex = aDistance[i].getIndex();
	fFactOE = exp(-(double)(i)/fLambda);
	fFact = fEpsilon * fFactOE;

	if (fFactOE < 1.0E-10)
	    fFactOE = 0;

	for(j=0; j<m_aW[iIndex].getDimension(); j++)
	{
	    fTmp = (fData.getCoord(j) - m_aW[iIndex].getCoord(j));

	    m_aW[iIndex].setCoord(j, m_aW[iIndex].getCoord(j) + (fFact * fTmp) );

	    if (m_fK != 0.0 && fFactOE != 0.0)
		oSum.setCoord(j, oSum.getCoord(j) + (fFactOE * fTmp) );
	}
    }
    // ok, the winner is a special case, becase of the WRNG algorithm (not used if m_fK == 0.0)
    iIndex = aDistance[i].getIndex();
    fFactOE = 1.0;
    fFact = fEpsilon * fFactOE;
    if (m_fK != 0.0 && fFactOE != 0.0)
    {
	for(j=0; j<m_aW[iIndex].getDimension(); j++)
	    oSum.setCoord(j, oSum.getCoord(j) + (fFactOE * (fData.getCoord(j) - m_aW[iIndex].getCoord(j))) );
    }
    for(j=0; j<m_aW[iIndex].getDimension(); j++)
    {
	m_aW[iIndex].setCoord(j,
			      m_aW[iIndex].getCoord(j) +
			      (fFact * (fData.getCoord(j) - m_aW[iIndex].getCoord(j))) +
                              ((m_fK * (fData.getCoord(j) - m_aW[iIndex].getCoord(j))) - (m_fK * oSum.getCoord(j)))
			     );
    }

    // increase counter
    m_iCount++;
}

/**
 * calculate connections in a postprocessing step. You have to call this function for every data vector!!
 * \param fData svt_multivec data vector
 */
void svt_trn::calcConnections(svt_multivec& fData)
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
}

/**
 * dump all codebook vectors to the console
 */
void svt_trn::dump()
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
}

/**
 * Output the topology as csv file. First column gives the index of a codebook vector and then the following numbers are the indices of the vectors it is connected to.
 * \param pFname pointer to char with the filename
 */
void svt_trn::outputTopology(char* pFname)
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
}

/**
 * Output the codebook vectors as a PDB file
 * \param pFname pointer to char with the filename
 */
void svt_trn::outputPDB(char* pFname)
{
    FILE* pFile = fopen(pFname, "w");

    if (pFile == NULL)
	exit(1);

    // now transform the model and output the vectors as pdb file
    svt_multivec oVec( 2 );
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
		m_aW[i].getCoord(0),
		m_aW[i].getCoord(1),
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
}
