/***************************************************************************
                          svt_point_cloud
                          ---------------
    begin                : 04/12/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_POINT_CLOUD_H
#define __SVT_POINT_CLOUD_H

#include <svt_basics.h>
#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_stlVector.h>
#include <svt_time.h>
#include <svt_rnd.h>
#include <svt_volume.h>
#include <svt_sampled.h>
#include <svt_rnd.h>
#include <svt_column_reader.h>
#include <svt_exception.h>
#include <svt_pair_distribution.h>

#define NOMATCH 100000000

/**
 * Helper class
 */
template<class T> class svt_neighbor
{
protected:

    T m_fScore;
    unsigned int m_iIndexA;
    unsigned int m_iIndexB;

public:

    svt_neighbor(T fScore, unsigned int iIndexA, unsigned int iIndexB)
    {
	m_fScore = fScore;
	m_iIndexA = iIndexA;
	m_iIndexB = iIndexB;
    };

    inline T getScore() const
    {
	return m_fScore;
    };
    inline unsigned int getIndexA() const
    {
	return m_iIndexA;
    };
    inline unsigned int getIndexB() const
    {
	return m_iIndexB;
    };

    inline bool operator<(const svt_neighbor<T>& rR) const
    {
        return m_fScore < rR.m_fScore;
    };
};


/**
 * A matching helper class that encapsulates a single result of the matching process
 */
template<class T> class svt_matchResult
{
protected:

    T m_fScore;
    svt_matrix4<T> m_oMatrix;
    vector<int> m_aModelMatch;
    vector<int> m_aSceneMatch;
    vector<int> m_aMatch;

public:

    svt_matchResult(T fScore, svt_matrix4<T> oMatrix, vector<int>& aModelMatch, vector<int>& aSceneMatch)
    {
	m_fScore = fScore;
	m_oMatrix = oMatrix;
        m_aModelMatch = aModelMatch;
        m_aSceneMatch = aSceneMatch;
    };

    inline T getScore() const
    {
	return m_fScore;
    };
    inline void setScore(T fScore)
    {
	m_fScore = fScore;
    };
    inline svt_matrix4<T> getMatrix() const
    {
	return m_oMatrix;
    };
    inline void setMatrix(svt_matrix4<Real64> oMatrix)
    {
	m_oMatrix = oMatrix;
    };
    inline vector<int>& getModelMatch()
    {
	return m_aModelMatch;
    };
    inline vector<int>& getSceneMatch()
    {
	return m_aSceneMatch;
    };
    vector<int>& getMatch()
    {
	if (m_aMatch.size() == 0)
            makeSorted();

        return m_aMatch;
    };
    inline unsigned int compareMatch( svt_matchResult<T>& rOther )
    {
	if (m_aMatch.size() == 0)
	    makeSorted();

        vector<int>& rOtherM = rOther.getMatch();
	unsigned int iNum = m_aMatch.size();
        unsigned int iDiff = 0;

	if (rOtherM.size() != m_aMatch.size())
	{
	    svtout << "ERROR: rOtherM.size(): " << rOtherM.size() << " m_aMatch.size(): " << m_aMatch.size() << endl;
	    exit(1);
	}

	for(unsigned int i=0; i<iNum; i++)
	    if (rOtherM[i] != m_aMatch[i] && rOtherM[i] != NOMATCH && m_aMatch[i] != NOMATCH)
		iDiff++;

        return iDiff;
    };

    inline void printMatch()
    {
	if (m_aMatch.size() == 0)
	    makeSorted();

        svtout;
	for (unsigned int i=0; i<m_aMatch.size(); i++)
	    if (m_aMatch[i] != NOMATCH)
                if (i != m_aMatch.size() -1)
		    printf( "%02i,", m_aMatch[i] );
		else
		    printf( "%02i", m_aMatch[i] );
	    else
		printf( " - " );
	cout << endl;
    };

    inline bool operator<(const svt_matchResult<T>& rR) const
    {
        return m_fScore < rR.m_fScore;
    };

private:

    /**
     * Generate the sorted match
     */
    void makeSorted()
    {
	if (m_aMatch.size() == 0)
	{
            m_aMatch = vector<int>(m_aModelMatch.size());
	    for(unsigned int i=0; i<m_aModelMatch.size(); i++)
                m_aMatch[m_aModelMatch[i]] = m_aSceneMatch[i];
	}
    };
};


/**
 * A point cloud class.
 * \author Stefan Birmanns
 */
template<class T> class svt_point_cloud : public svt_sampled< T >
{
protected:

    vector< vector< T > > m_aPoints;

    // tree pruning parameter
    Real64 m_fEpsilon;
    // tolorance distance
    Real64 m_fLambda;
    // ranking distance
    Real64 m_fGamma;
    // size of the matching zone
    unsigned int m_iZoneSize;
    // number of runs with different anchor points
    unsigned int m_iRuns;

    // maximum number of wildcard matches
    unsigned int m_iMaxNoMatch;

    // number of seed/anchor matches evaluated during last matching run
    unsigned int m_iNumSeeds;

    // in case of simple==true only a very simple version of the matching algorithm is performed...
    bool m_bSimple;

    // uniqueness rmsd threshold for the solutions
    Real64 m_fUnique;

    // next point selection scheme
    bool m_bNextPointCOA;

    // timestep information for time-varying pointclouds
    unsigned int m_iTimestep;

    // penalty for the wildcards in the matching algo
    Real64 m_fSkipPenalty;
    
    //pair distribution function
    svt_pair_distribution m_oPDF;

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

    /**
     * Constructor
     */
    svt_point_cloud() :
        m_aPoints( 1 ),
	m_fEpsilon( 30.0 ),
	m_fLambda( 5.0 ),
	m_fGamma( 1.0 ),
	m_iZoneSize( 3 ),
	m_iRuns( 1 ),
        m_iMaxNoMatch( 3 ),
	m_iNumSeeds( 0 ),
	m_bSimple( false ),
	m_fUnique( 1.0 ),
	m_bNextPointCOA( false ),
	m_iTimestep( 0 ),
	m_fSkipPenalty( 1.0 )
    {
    };
    
    ~svt_point_cloud();
            
    /**
     * Get all points in point cloud in an stl vector.
     * \return reference to vector of svt_vector4 objects
     */
    inline vector< T >& getPoints();

    /**
     * Add a point to point cloud.
     * \param rVec svt_vector4 object
     */
    inline void addPoint( T& rVec );
    /**
     * Add a point to point cloud.
     * \param fX x coord
     * \param fY y coord
     * \param fZ z coord
     */
    inline void addPoint( Real64 fX, Real64 fY, Real64 fZ );

    /**
     * Get a point out of point cloud.
     * \param iIndex index of point
     * \return reference to svt_vector4 object
     */
    inline T& getPoint( unsigned int iIndex );

    /**
     * Replace point in point cloud.
     * \param iIndex index of point
     * \param rVec svt_vector4 object
     */
    inline void setPoint( unsigned int iIndex, T& rVec);

    /**
     * Delete point in point cloud.
     * \param iIndex index of point
     */
    inline void delPoint( unsigned int iIndex);
    
    /**
     * Delete all points in point cloud
     */
    inline void delAllPoints();

    /**
     * Size of point cloud.
     * \return size of pc
     */
    inline unsigned int size() const;

    /**
     * Dereference operator (not range checked!).
     * \param iIndex index of point in point cloud
     */
    inline T& operator[](unsigned int iIndex);

    /**
     * Calculate center of atoms (COA with all masses = 1).
     * \return svt_vector4 with the COA
     */
    T coa();

    /**
     * Calculate the geometric center of the point cloud.
     * \return svt_vector4 with the geometric center
     */
    T geometricCenter();

    /**
     * calculate RMSD between this and another PC. The PCs must be matched already! To get the minimal RMSD please use align() first!
     * \param rPC second point cloud
     */
    Real64 rmsd( svt_point_cloud<T>& rPC );
    /**
     * calculate RMSD between this and another PC. The points are matched using the nearest neighbor relationship.
     * \param rPC second point cloud
     */
    Real64 rmsd_NN( svt_point_cloud<T>& rPC );
    /**
     * Calculate RMSD between this and another PC. The points are matched using the nearest neighbor relationship.
     * All the nearest neighbor distances are calculated and then sorted (slow!) and only the first N-percent are used for the matching and the following RMSD calculation.
     * The idea is that approximately N-percent of the points are outliers which would increase the RMSD significantly, although the overall deviation of the two point clouds is
     * actually small.
     * \param rPC second point cloud
     * \param fPercent percentage of neighbors that should be used for the rmsd calculation
     * \param rMatrix reference to svt_matrix4 object
     */
    Real64 rmsd_NN_Outliers( svt_point_cloud<T>& rPC, Real64 fPercent, svt_matrix4<Real64>* pMatrix =NULL );

    /**
     * calculate hausdorff distance between this and another PC. The PCs must be matched and aligned already!
     * \param rPC second point cloud
     */
    Real64 hausdorff( svt_point_cloud<T>& rPC );

    /**
     * find the nearest neighbor to a query point in the point cloud
     * \param rVec reference to svt_vector4 object - the query point
     * \return index to nearest point in point cloud
     */
    unsigned int nearestNeighbor( T& rVec );

    /**
     * calculate the average nearest neighbor distance
     * in order to reduce the complexity a random test is done and once the average stabilizes, the search is stopped.
     * \param fPrecision if average does not change by more than fPrecision between two iterations the calculation is stopped
     * \return average nearest neighbor distance
     */
    Real64 averageNNDistance(Real64 fPrecision );

    /**
     * calculate the maximum nearest neighbor distance
     * \return maximum nearest neighbor distance
     */
    Real64 maxNNDistance( );

    /**
     * Align point cloud with another point cloud.
     * This function does not perform a matching procedure, it assumes that the ith point in the first
     * cloud is associated to the ith point in the second point cloud. It aligns both clouds so that they have a minimal RMSD.
     * \param rPC second point cloud
     */
    //void align( svt_point_cloud<T>& rPC );

    /**
     * Get the minimal coordinates of the point cloud - it will return a vector that has in each dimension the information about the minimal
     * coordinate it has found in the cloud.
     */
    T getMinCoord();
    /**
     * Get the maximal coordinates of the point cloud - it will return a vector that has in each dimension the information about the maximal
     * coordinate it has found in the cloud.
     */
    T getMaxCoord();

    //
    // clustering
    //

    /**
     * sample the object randomly and return a vector that refrects the probability distribution of the object
     */
    T sample( );

    //
    // file i/o
    //

    /**
     * Write pdb file.
     * \param pFilename pointer to array of char with the filename
     * \param bAppend if true, the pdb structure as append at the end of an existing structure file
     */
    void writePDB(const char* pFilename, bool bAppend = false);
    /**
     * Load a pdb file.
     * \param pointer to array of char with the filename
     */
    void loadPDB(const char* pFilename);

    /**
     * Write CSV file - comma separated values x,y,z. It will write only the coordinates of the current frame.
     * \param pFilename pointer to array of char with the filename
     */
    void writeCSV(const char* pFilename);

    //
    // matching functions
    //

    /**
     * Set tree pruning parameter
     * \param fEpsilon epsilon
     */
    void setEpsilon( Real64 fEpsilon );
    /**
     * Get tree pruning parameter.
     * \param fEpsilon epsilon
     */
    Real64 getEpsilon( ) const;
    /**
     * set tolorance distance for the anchor determination
     * \param fLambda lambda
     */
    void setLambda( Real64 fLambda );
    /**
     * get tolorance distance for the anchor determination
     * \return lambda
     */
    Real64 getLambda( ) const;
    /**
     * set nearest neighbor matching zone size
     * \param fGamma gamma
     */
    void setGamma( Real64 fGamma );
    /**
     * get nearest neighbor matching zone size
     * \return gamma
     */
    Real64 getGamma( ) const;

    /**
     * set the maximal size of the matching zone
     * \param iZoneSize
     */
    inline void setZoneSize( unsigned int iZoneSize );
    /**
     * get the maximal size of the matching zone
     * \return maximal size of matching zone
     */
    inline unsigned int getZoneSize( ) const;

    /**
     * set the maximal number of wildcard matches
     * \param iMaxNoMatch maximal number of wildcard matches
     */
    inline void setWildcards( unsigned int iMaxNoMatch );
    /**
     * get the maximal number of wildcard matches
     * \return maximal number of wildcard matches
     */
    inline unsigned int getWildcards() const;

    /**
     * set the penalty for wildcard matches
     * \param fSkipPenalty penalty for a single wildcard
     */
    inline void setSkipPenalty( Real64 fSkipPenalty );
    /**
     * get the penalty for wildcard matches
     * \return penalty for a single wildcard
     */
    inline Real64 getSkipPenalty() const;

    /**
     * if two solutions are very close only the one with the higher score is considered. The other solutions are removed.
     * \param minimal distance between solutions
     */
    inline void setUnique( Real64 fUnique );
    /**
     * if two solutions are very close only the one with the higher score is considered. The other solutions are removed.
     * \return minimal distance between solutions
     */
    inline Real64 getUnique( ) const;

    /**
     * set the number of runs. Each time a different set of anchor points get selected from the probe structure, beginning with the three points furthest away from each other and the COA.
     * \param iRuns number of runs
     */
    inline void setRuns( unsigned int iRuns );
    /**
     * get the number of runs. Each time a different set of anchor points get selected from the probe structure, beginning with the three points furthest away from each other and the COA.
     * \return number of runs
     */
    inline unsigned int getRuns( ) const;

    /**
     * Full or simple version of the matching algorithm to be performed.
     * \param bSimple if true only a reduced version is performed (faster but less accurate)
     */
    inline void setSimple( bool bSimple );
    /**
     * Full or simple version of the matching algorithm to be performed.
     * \return if true only a reduced version is performed (faster but less accurate)
     */
    inline bool getSimple( ) const;

    /**
     * Set the next point selection scheme to COA
     */
    inline void setNextPointCOA(bool bNextPointCOA);

    /**
     * match two point clouds.
     * This function will perform a full N->M matching based on an achor-based search.
     * \param rPC second point cloud
     * \param pMatch vector of unsigned ints with the indices of the points of second cloud to which the points of this cloud are matched. This vector will get erased and then filled during the matching.
     * \return copy of this cloud matched to rPC
     */
    void match( svt_point_cloud<T>& rPC, vector< svt_matchResult<Real64> >& rMatch);

    /**
     * match two point clouds.
     * This function will perform a full N->M matching based on an achor-based search.
     * \param rPC second point cloud
     * \param pMatch vector of unsigned ints with the indices of the points of second cloud to which the points of this cloud are matched. This vector will get erased and then filled during the matching.
     * \param iAnchorA index of first anchor point (if not provided the routine will find a set of appropriate anchor points)
     * \param iAnchorB index of second anchor point (if not provided the routine will find a set of appropriate anchor points)
     * \param iAnchorC index of third anchor point (if not provided the routine will find a set of appropriate anchor points)
     * \return copy of this cloud matched to rPC
     */
    void match( svt_point_cloud<T>& rPC, vector< svt_matchResult<Real64> >& rMatch, unsigned int iAnchorA, unsigned int iAnchorB, unsigned int iAnchorC );

    /**
     * Get number of seed/anchor matches evaluated during last matching run.
     */
    unsigned int getNumSeeds() const;

    /**
     * Calculate the cross-correlation between this pointcloud and a volumetric object. The idea is to use the points in the pointcloud to sample the volume and to
     * form thereby ad reduce cross-correlation coefficient equation.
     * See also:
     * Stefan Birmanns and Willy Wriggers. Interactive Fitting Augmented by Force-Feedback and Virtual Reality. J. Struct. Biol., 2003, Vol. 144, pp. 123-131.
     * \param rPointMat reference to svt_matrix4 object with the transformation matrix of the pointcloud object
     * \param rVol reference to svt_volume object
     * \param rVolMat reference to svt_matrix4 object with the transformation matrix of the volumetric object
     * \return the cross-correlation
     */
    Real64 calcCC(svt_matrix4<Real64>& rPointMat, svt_volume<Real64>& rVol, svt_matrix4<Real64>& rVolMat);

private:

    /**
     * sort eigenvectors according to their eigenvalues
     * \param pEigenvectors pointer to svt_matrix with the eigenvectors as column vectors
     * \param pEigenvalues pointer to svt_vector with the eigenvalues
     */
    void eigensort(svt_matrix4<Real64>& rEigenvectors, T& rEigenvalues);

public:

    /**
     * least-squares fit of a model point set to a scene point set
     * \param rM vector of ints with the indices of the first (model) vectors that are used
     * \param rS vector of ints with the indices of the second (scene) vectors that are used
     * \param rModel vector of svt_vector4 with the model vectors
     * \param rScene vector of svt_vector4 with the scene vectors
     * \return matrix with optimal transformation of the model into the scene
     */
    svt_matrix4<Real64> kearsley( vector< int >& rM, vector< int >& rS, svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene );

    /**
     * Least-squares fit of a model point set to a scene point set.
     *
     * The method is based on Kearsley's quaternion method. The method by Kabsch produces
     * in some cases wrong results, e.g. try to align the following pdb onto itself:
     *
     * ATOM   1421  CA  ASP   195      23.907   2.453  21.715  1.00  4.10
     * ATOM   1627  CA  GLU   221      20.828   3.631  18.679  1.00  3.82
     * ATOM    295  CA  GLY    44      48.823  29.146  -1.483  1.00 22.85
     * ATOM    313  CA  GLY    47      42.862  22.490   3.028  1.00  8.42
     *
     * This should of course yield the identity operator and an RMSD of
     * 0.0 A, but the Kabsch routine actually yields ... 1.368 A, plus an
     * operator that isn't anywhere near identity !
     * Found on the O mailing list.
     *
     * \param rModel vector of svt_vector4 with the model vectors
     * \param rScene vector of svt_vector4 with the scene vectors
     * \return matrix with optimal transformation of the model into the scene
     */
    svt_matrix4<Real64> kearsley(svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene);

    //private:

    /**
     * optimize_simple - subroutine for the match() procedure
     */
    svt_matrix4<Real64> optimize_simple( vector<int>* pModelMatch, vector<int>* pSceneMatch, svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, bool bInit );
    /**
     * optimize - subroutine for the match() procedure
     */
    svt_matrix4<Real64> optimize( vector<int>* pModelMatch, vector<int>* pSceneMatch, svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, bool bInit );

    /**
     * internal convenience function - no real difference with hausdorff - only here we don't need to create/copy a svt_point_cloud
     */
    Real64 calcHausdorff( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix);
    /**
     * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
     */
    Real64 calcRMSD( svt_point_cloud<T>& rModel, svt_matrix4<Real64> oMatA, svt_matrix4<Real64> oMatB);
    /**
     * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
     */
    Real64 calcRMSD( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix);
    /**
     * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
     * plus here we take the matching into account! If NOMATCH this point will not used
     */
    Real64 calcRMSD( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix, vector<int>* pModelMatch);
    /**
     * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
     * plus here we take the matching into account! If NOMATCH this point will not be used.
     */
    Real64 calcRMSD( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix, vector<int>* pModelMatch, vector<int>* pSceneMatch);
    
    /**
     * Calculate the Pair Distribution Function - improved version without multiple function calls
     * \param fWidth is width of a bin (held constant between the range Dmin and Dmax)
     */
    void calcPairDistribution(Real64 fWidth = 1.0, bool bShowProgress=true);
    
    /**
     * Calculate the Pair Distribution Function
     * \param fWidth is width of a bin (held constant between the range Dmin and Dmax)
     */
    void calcPairDistribution2(Real64 fWidth = 1.0, bool bShowProgress=true);

    
     /**
     * return the pairDistribution
     */
    inline svt_pair_distribution getPairDistribution();
    
    /**
     * return the pairDistribution
     */
    inline void setPairDistribution(svt_pair_distribution & oPDF);

    
    /**
     * Normalize pairDistribution
     */
    void normalizePairDistribution();

    /**
     * Compute the distance matrix between all points in the pointcloud
     * \return svt_matrix object with the distances
     */
    svt_matrix<Real64> getDistanceMat();

    /**
     * \name Timesteps for time-varying point clouds
     */
    //@{

    /**
     * set the current timestep
     * \param iTimestep the new timestep
     */
    void setTimestep( unsigned int iTimestep );
    /**
     * get the current timestep
     * \return the timestep
     */
    int getTimestep();
    /**
     * get the maximum timestep
     * \return the maximum timestep
     */
    int getMaxTimestep();
    /**
     * Add a new timestep.
     */
    void addTimestep();

    //@}
};

// include the basic definitions
#include <svt_point_cloud_basic.h>
// include the match definitions
#include <svt_point_cloud_match.h>

#endif
