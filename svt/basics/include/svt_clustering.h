/***************************************************************************
                          svt_clustering
                          --------------
    begin                : 02/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_CLUSTERING_H
#define __SVT_CLUSTERING_H

#include <svt_time.h>
#include <svt_matrix.h>
#include <svt_point_cloud.h>
#include <svt_point_cloud_pdb.h>
#include <svt_exception.h>

/**
 * Global function to cluster an svt_sample object using a svt_clustering object
 */


/**
 * Pure abstract base class of a clustering algorithm. The clustering should work in any dimensional space, therefore the template parameter
 * can specify e.g. a svt_vector3 or svt_vectorn.
 * \author Stefan Birmanns
 */
template<class T> class svt_clustering
{
protected:

    // codebook vectors
    svt_point_cloud< T > m_aW;
    // topology
    svt_matrix< bool > m_oC_Conn;
    svt_matrix< unsigned int > m_oC_Age;

    // number of updates so far
    unsigned int m_iCount;
    // number of maximal updates
    unsigned int m_iMaxstep;

    int CoordPrecisionPDB (double fNum) {
      if (fNum < 9999.9995 && fNum >= -999.9995) return 3;
      else if (fNum < 99999.995 && fNum >= -9999.995) return 2;
      else if (fNum < 999999.95 && fNum >= -99999.95) return 1;
      else if (fNum < 99999999.5 && fNum >= -9999999.5) return 0;
      else {
	svt_exception::ui()->error("Error: PDB coordinate out of range, PDB outout aborted");
        exit(0);
      }
    }

public:

    svt_clustering() :
	m_iCount( 1 ),
	m_iMaxstep( 1000 )
    {
    };
    virtual ~svt_clustering()
    {
    };

    /**
     * initialize the clustering algorithm.
     */
    void init();

    /**
     * train the network
     */
    virtual void train(svt_sampled<T>& rObject) =0;

    /**
     * set the number of steps the algorithm should run
     * \param iMaxstep maximum number of iterations
     */
    inline void setMaxstep( unsigned int iMaxstep );
    /**
     * get the number of steps the algorithm should run
     * \return maximum number of iterations
     */
    inline unsigned int getMaxstep( ) const;

    /**
     * get the current iteration number
     * \return current iteration
     */
    inline unsigned int getStep() const;

    /**
     * add a codebook vector. Each call will add a single codebook vector and initialize it with the provided svt_vector object
     * \param fData
     */
    void addVector(T& fData);

    /**
     * get all codebook vectors
     * \return stl vector with all the svt_vector objects
     */
    inline svt_point_cloud_pdb< T > getCodebook();

    /**
     * delete all codebook vectors
     */
    inline void delCodebook();

    /**
     * get number of codebook vectors
     * \return number of codebook vectors
     */
    inline unsigned int getCodebookSize() const;

    /**
     * get a single codebook vector
     * \param i index of codebook vector
     * \return reference to svt_multivec object
     */
    inline T& getVector(int i) const;

    /**
     * dump all codebook vectors to the console (cout)
     */
    void dump();

    /**
     * Output the topology as csv file. First column gives the index of a codebook vector and then the following numbers are the indices of the vectors it is connected to.
     * \param pFname pointer to char with the filename
     */
    void outputTopology(char* pFname);

    /**
     * Write the topology into a psf file
     * \param pFilename pointer to array of char with the filename
     */
    void writePSF(const char* pFilename);

    /**
     * Output the codebook vectors as a PDB file
     * \param pFname pointer to char with the filename
     */
    void writePDB(char* pFname);

    /**
     * Cluster a data set - the data set must be derived of svt_sampled (e.g. svt_volume).
     * \param iCV number of codebook vectors the routine should use for the clustering
     * \param iRuns how many runs of the clustering should be clustered together?
     * \param rPDB the object derived from svt_sampled that should analyzed.
     */
    svt_point_cloud_pdb< svt_vector4<Real64> > cluster( unsigned int iCV, unsigned int iRuns, svt_sampled< svt_vector4<Real64> >& rPDB );
};

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * initialize the clustering algorithm - connection matrix gets allocated, etc.
 */

template<class T>
inline void svt_clustering<T>::init()
{
    m_oC_Conn.resize( m_aW.size(), m_aW.size(), svt_matrix<bool>::Clear, false );
    m_oC_Age.resize( m_aW.size(), m_aW.size(), svt_matrix<unsigned int>::Clear, 0 );

    m_oC_Conn = false;
    m_oC_Age = 0;

    m_iCount = 1;
};

/**
 * set the number of steps the algorithm should run
 * \param iMaxstep maximum number of iterations
 */
template<class T>
inline void svt_clustering<T>::setMaxstep( unsigned int iMaxstep )
{
    m_iMaxstep = iMaxstep;
};
/**
 * get the number of steps the algorithm should run
 * \return maximum number of iterations
 */
template<class T>
inline unsigned int svt_clustering<T>::getMaxstep() const
{
    return m_iMaxstep;
};

/**
 * get the current iteration number
 * \return current iteration
 */
template<class T>
inline unsigned int svt_clustering<T>::getStep() const
{
    return m_iCount;
};

/**
 * add a codebook vector. Each call will add a single codebook vector and initialize it with the provided svt_vector object
 * \param fData
 */
template<class T>
void svt_clustering<T>::addVector(T& fData)
{
    m_aW.addPoint( fData );
};

/**
 * get all codebook vectors
 * \return stl vector with all the svt_vector objects
 */
template<class T>
inline svt_point_cloud_pdb< T > svt_clustering<T>::getCodebook()
{
    svt_point_cloud_pdb<T> oCodebook;
    svt_point_cloud_atom oAtom;
    unsigned int iCV = m_aW.size();
    unsigned int i,j;

    for(unsigned int i=0; i<iCV; i++)
    {
        oCodebook.addAtom( oAtom, m_aW[i] );
    }

    for(i=0; i<iCV; i++)
        for(j=i+1; j<iCV; j++)
            if (m_oC_Conn[i][j])
                oCodebook.addBond( i, j );

    return oCodebook;
};

/**
 * delete all codebook vectors
 */
template<class T>
inline void svt_clustering<T>::delCodebook()
{
    m_oC_Conn = false;
    m_oC_Age = 0;
    m_aW.delAllPoints();
};

/**
 * get number of codebook vectors
 * \return number of codebook vectors
 */
template<class T>
inline unsigned int svt_clustering<T>::getCodebookSize() const
{
    return m_aW.size();
};

/**
 * get a single codebook vector
 * \param i index of codebook vector
 * \return reference to svt_multivec object
 */
template<class T>
inline T& svt_clustering<T>::getVector(int i) const
{
    return (T&)m_aW[i];
};

/**
 * dump all codebook vectors to the console (cout)
 */
template<class T>
void svt_clustering<T>::dump()
{
    unsigned int i,j;

    for (i=0; i<m_aW.size(); i++)
        m_aW[i].dump();

    for (i=0; i<m_aW.size(); i++)
    {
        svtout << i << " connected to: ";
        for (j=0; j<m_aW.size(); j++)
            if (m_oC_Conn[i][j] == true)
                cout << j << " ";
        cout << endl;
    }
};

/**
 * Output the topology as csv file. First column gives the index of a codebook vector and then the following numbers are the indices of the vectors it is connected to.
 * \param pFname pointer to char with the filename
 */
template<class T>
void svt_clustering<T>::outputTopology(char* pFname)
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
            if (m_oC_Conn[i][j] == true)
                fprintf( pFile, ", %i", j );

        fprintf( pFile, "\n" );
    }

    fclose( pFile );
};

/**
 * Write the topology into a psf file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
void svt_clustering<T>::writePSF(const char* pFilename)
{
    unsigned int i,j,k = 0;

    FILE* pFout = fopen(pFilename, "w");

    fprintf(pFout, "PSF \n");
    fprintf(pFout, " \n");
    fprintf(pFout, "       1 !NTITLE\n");
    fprintf(pFout, " REMARK Connectivity file created by SVT/Sculptor - Attention: Atm, only the connectivity information is valid in this file!\n");
    fprintf(pFout, " \n");
    fprintf(pFout, "%8d !NATOM\n", this->m_aW.size());

    for( i=0; i<this->m_aW.size(); i++ )
	fprintf(pFout, "%8d %4s %-4d %4s QVOL QVOL   0.000000       0.00000           0\n", i, "QPDB", 0, "QPDB" );

    //fprintf(pFout, "%8d QVOL %-4d QVOL QVOL QVOL   0.000000       0.00000           0\n", i, i+1);

    fprintf(pFout, " \n");

    unsigned int iBonds = 0;
    for (i=0; i<m_aW.size(); i++)
	for (j=0; j<m_aW.size(); j++)
	    if (j < i && m_oC_Conn[i][j] == true)
                iBonds++;

    fprintf(pFout, "%8d !NBOND: bonds\n", iBonds);

    for (i=0; i<m_aW.size(); i++)
	for (j=0; j<m_aW.size(); j++)
	{
	    if (j<i && m_oC_Conn[i][j] == true)
	    {
		if (k == 4)
		{
		    k = 0;
		    fprintf(pFout, " \n");
		}
		k++;

		fprintf( pFout, "%8d%8d", i+1, j+1 );
	    }
	}
    if (k > 0) fprintf(pFout, "\n");

    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NTHETA: angles\n");
    fprintf(pFout, "\n");   
    fprintf(pFout, "       0 !NPHI: dihedrals\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NIMPHI: impropers\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NDON: donors\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NACC: acceptors\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NNB\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0       0 !NGRP\n");
    fprintf(pFout, "\n");

    fclose (pFout);

    svtout << "Connectivity data written to PSF file " << pFilename << endl;

};


/**
 * Output the codebook vectors as a PDB file
 * \param pFname pointer to char with the filename
 */
template<class T>
void svt_clustering<T>::writePDB(char* pFname)
{
    FILE* pFile = fopen(pFname, "w");

    if (pFile == NULL)
        exit(1);

    // now transform the model and output the vectors as pdb file
    T oVec( 2 );
    for(unsigned int i=0; i<m_aW.size(); i++)
    {
        fprintf(pFile, "ATOM  %5i %-4s%c%3s %c%4i%c   %8.*f%8.*f%8.*f%6.2f%6.2f %3s  %4s%2s%2s\n",
                0,
                "QPDB",
                'Q',
                "001",
                'A',
                0,
                'Q',
                CoordPrecisionPDB(m_aW[i].x()), m_aW[i].x(),
                CoordPrecisionPDB(m_aW[i].y()), m_aW[i].y(),
                CoordPrecisionPDB(m_aW[i].z()), m_aW[i].z(),
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

/**
 * Cluster a data set - the data set must be derived of svt_sampled (e.g. svt_volume).
 * \param iCV number of codebook vectors the routine should use for the clustering
 * \param iRuns how many runs of the clustering should be clustered together?
 * \param rPDB the object derived from svt_sampled that should analyzed.
 */
template<class T>
svt_point_cloud_pdb< svt_vector4<Real64> > svt_clustering<T>::cluster( unsigned int iCV, unsigned int iRuns, svt_sampled< svt_vector4<Real64> >& rPDB )
{
    vector< svt_point_cloud< svt_vector4< Real64 > > > oVectors;
    svt_point_cloud< svt_vector4<Real64> > oCVMAP;
    unsigned int i,j,k;
    Real64 fMaxClusterVar = 0.0;
    Real64* aClusterVar = new Real64[iCV];
    vector< svt_matrix< bool > > aConns;

    // run the clustering algo
    svt_exception::ui()->progressPopup("Training Network", 0);
    for(k=0; k<iRuns; k++)
    {
	delCodebook();

	for(i=0; i<iCV; i++)
	{
	    svt_vector4<Real64> oVec = rPDB.sample();
	    addVector( oVec );
	}

        try
        {
	    train( rPDB );

            int iPerc = (int)((((float)(k) / (float)(iRuns)) * 100.0));
            svt_exception::ui()->progress( iPerc);

        } catch (svt_userInterrupt&)
        {
            delCodebook();
            svt_exception::ui()->progressPopdown();
            svt_point_cloud_pdb< svt_vector4<Real64> > oEmpty;
            return oEmpty;
	}

	oCVMAP = getCodebook();
        oVectors.push_back( oCVMAP );
        aConns.push_back( m_oC_Conn );
    }
    svt_exception::ui()->progressPopdown();

    Real64 fAvgClusterVar = 0.0;

    oCVMAP = oVectors[ (unsigned int)((svt_genrand() * (Real64)(iRuns))) ];
    // how are the clusters populated?
    vector< vector< unsigned int > > iClusterPop;
    // where did every cv ended up being?
    vector< vector< unsigned int > > aClusterCV;

    Real64 fDiff = 10.0;
    Real64 fOldDiff = 0.0;
    unsigned int iR;
    unsigned int iClusterSize;
    svt_vector4<Real64> oVec;
    unsigned int iMinInd = 0;

    // compute cluster centers
    while(fabs(fDiff-fOldDiff) > 1.0E-20)
    {
        // initialize the cluster population vector
        iClusterPop.clear();
        aClusterCV.clear();
	for(j=0; j<iCV; j++)
	{
	    vector< unsigned int > iCluster;
	    iClusterPop.push_back( iCluster );
	}

	// add every codebook vector in every run to a cluster
	for(i=0; i<iRuns; i++)
        {
	    vector< unsigned int > aCluster;
            aClusterCV.push_back( aCluster );

	    for(j=0; j<iCV; j++)
	    {
		iMinInd = oCVMAP.nearestNeighbor( oVectors[i][j] );

                iClusterPop[iMinInd].push_back(i*iCV+j);
                aClusterCV[i].push_back(iMinInd);
            }
	}

        // compute the average position = cluster center for every cluster
	fOldDiff = fDiff;
	fDiff = 0.0;
	for(j=0; j<iCV; j++)
	{
	    iClusterSize = iClusterPop[j].size();

	    oVec.set(0.0);
	    for(i=0; i<iClusterSize; i++)
	    {
		iR = (unsigned int)(((Real64)(iClusterPop[j][i]) / (Real64)(iCV)));

		oVec = oVec + oVectors[iR][iClusterPop[j][i]-(iR*iCV)];
	    }
	    oVec = oVec / (Real64)(iClusterSize);

	    fDiff += oVec.distance( oCVMAP[j] );

	    oCVMAP[j] = oVec;
	}
    }

    // compute variability within clusters: average rmsd between cluster centers and cv's within cluster
    Real64 fClusterVar;
    fAvgClusterVar = 0.0;
    fMaxClusterVar = 0.0;
    Real64 fDist = 0.0;

    for(j=0; j<iCV; j++)
    {
	fClusterVar = 0.0;

	iClusterSize = iClusterPop[j].size();

	if (iClusterSize == 0)
	{
	    cout << endl;
	    svtout << "Warning! Cluster does not contain any data points!" << endl;
	}

	// calculate average
	for(i=0; i<iClusterSize; i++)
	{
	    iR = (unsigned int)(((Real64)(iClusterPop[j][i]) / (Real64)(iCV)));
	    fDist = oCVMAP[j].distanceSq( oVectors[iR][iClusterPop[j][i]-(iR*iCV)] );
	    fClusterVar += fDist;
	}
	fClusterVar = fClusterVar / (Real64)(iClusterSize);
	fClusterVar = sqrt( fClusterVar );
	aClusterVar[j] = fClusterVar;

        // some stats
	if (fClusterVar > fMaxClusterVar)
	    fMaxClusterVar = fClusterVar;

	fAvgClusterVar += fClusterVar;

    }
    fAvgClusterVar /= iCV;

    char pInfo[256];
    sprintf(pInfo, "Max/Avg Variance: %2.3f/%2.3f", fMaxClusterVar, fAvgClusterVar );
    //svt_exception::ui()->info(pInfo);
    svtout << "Cluster-centers: " << pInfo << endl;

    svt_point_cloud_pdb< svt_vector4<Real64> > oResult;
    svt_point_cloud_atom oAtom;
    for(j=0; j<iCV; j++)
    {
	oAtom.setTempFact( aClusterVar[j] );

	//type information
	oAtom.setName( "QP" );
	oAtom.setRemoteness( 'D' );
	oAtom.setBranch( 'B' );
	//alternate location identifier information
	oAtom.setAltLoc( ' ' );
	//resname information
	oAtom.setResname( "QPDB" );
	//chain id
	oAtom.setChainID( ' ' );
	// residue sequence number
	oAtom.setResidueSeq( j );
	// iCode
	oAtom.setICode( ' ' );
	// fOccupancy
	oAtom.setOccupancy( 0.0 );
	// note
	//oAtom.setNote( "   " );
	// segment id
	//oAtom.setSegmentID( "    " );
	// element symbol
	oAtom.setElement( "QP" );
	// charge
	oAtom.setCharge( "DB" );

	oResult.addAtom( oAtom, oCVMAP[j] );
    }

    // bonds
    unsigned int iI, iJ;
    for(i=0; i<iCV; i++)
        for(j=i+1; j<iCV; j++)
        {
            iI = aClusterCV[iRuns-1][i];
            iJ = aClusterCV[iRuns-1][j];

            if (aConns[iRuns-1][iI][iJ])
                oResult.addBond( i, j );
        }

    delete [] aClusterVar;

    for(j=0; j<iCV; j++)
        this->m_aW[j] = oCVMAP[j];

    return oResult;
};

#endif
