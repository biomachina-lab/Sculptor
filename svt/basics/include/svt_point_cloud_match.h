/***************************************************************************
                          svt_point_cloud_match
                          ---------------------
    begin                : 02/10/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Implementation of point cloud matching functionality of svt_point_cloud
///////////////////////////////////////////////////////////////////////////////
// DO NOT INCLUDE DIRECTLY! File gets included by other header file!
///////////////////////////////////////////////////////////////////////////////

/**
 * matching helper class
 */
template<class T> class _pc_dist
{
protected:

    T m_fScore;
    unsigned int m_iIndex;

public:

    _pc_dist(T fScore, unsigned int iIndex)
    {
	m_fScore = fScore;
	m_iIndex = iIndex;
    };

    inline T getScore() const
    {
	return m_fScore;
    };
    inline unsigned int getIndex() const
    {
	return m_iIndex;
    };

    inline bool operator<(const _pc_dist& rR) const
    {
        return m_fScore < rR.m_fScore;
    };
};

/**
 * Destructor
 */
template<class T>
svt_point_cloud<T>::~svt_point_cloud()
{
};


/**
 * Set tree pruning parameter
 * \param fEpsilon epsilon
 */
template<class T>
inline void svt_point_cloud<T>::setEpsilon( Real64 fEpsilon )
{
    m_fEpsilon = fEpsilon;
};
/**
 * Get tree pruning parameter.
 * \param fEpsilon epsilon
 */
template<class T>
inline Real64 svt_point_cloud<T>::getEpsilon( ) const
{
    return m_fEpsilon;
};
/**
 * set tolorance distance for the anchor determination
 * \param fLambda lambda
 */
template<class T>
inline void svt_point_cloud<T>::setLambda( Real64 fLambda )
{
    m_fLambda = fLambda;
};
/**
 * get tolorance distance for the anchor determination
 * \return lambda
 */
template<class T>
inline Real64 svt_point_cloud<T>::getLambda( ) const
{
    return m_fLambda;
};
/**
 * set nearest neighbor matching zone size
 * \param fGamma gamma
 */
template<class T>
inline void svt_point_cloud<T>::setGamma( Real64 fGamma )
{
    m_fGamma = fGamma;
};
/**
 * get nearest neighbor matching zone size
 * \return gamma
 */
template<class T>
inline Real64 svt_point_cloud<T>::getGamma( ) const
{
    return m_fGamma;
};

/**
 * set the maximal size of the matching zone
 * \param iZoneSize
 */
template<class T>
inline void svt_point_cloud<T>::setZoneSize( unsigned int iZoneSize )
{
    m_iZoneSize = iZoneSize;
};
/**
 * get the maximal size of the matching zone
 * \return maximal size of matching zone
 */
template<class T>
inline unsigned int svt_point_cloud<T>::getZoneSize( ) const
{
    return m_iZoneSize;
};

/**
 * set the maximal number of wildcard matches
 * \param iMaxNoMatch maximal number of wildcard matches
 */
template<class T>
inline void svt_point_cloud<T>::setWildcards( unsigned int iMaxNoMatch )
{
    m_iMaxNoMatch = iMaxNoMatch;
};
/**
 * get the maximal number of wildcard matches
 * \return maximal number of wildcard matches
 */
template<class T>
inline unsigned int svt_point_cloud<T>::getWildcards() const
{
    return m_iMaxNoMatch;
};

/**
 * set the penalty for wildcard matches
 * \param fSkipPenalty penalty for a single wildcard
 */
template<class T>
inline void svt_point_cloud<T>::setSkipPenalty( Real64 fSkipPenalty )
{
    m_fSkipPenalty = fSkipPenalty;
}
/**
 * get the penalty for wildcard matches
 * \return penalty for a single wildcard
 */
template<class T>
inline Real64 svt_point_cloud<T>::getSkipPenalty() const
{
    return m_fSkipPenalty;
}

/**
 * if two solutions are very close only the one with the higher score is considered. The other solutions are removed.
 * \param minimal distance between solutions
 */
template<class T>
inline void svt_point_cloud<T>::setUnique( Real64 fUnique )
{
    m_fUnique = fUnique;
}
/**
 * if two solutions are very close only the one with the higher score is considered. The other solutions are removed.
 * \return minimal distance between solutions
 */
template<class T>
inline Real64 svt_point_cloud<T>::getUnique( ) const
{
    return m_fUnique;
};

/**
 * set the number of runs. Each time a different set of anchor points get selected from the probe structure, beginning with the three points furthest away from each other and the COA.
 * \param iRuns number of runs
 */
template<class T>
inline void svt_point_cloud<T>::setRuns( unsigned int iRuns )
{
    m_iRuns = iRuns;
};
/**
 * get the number of runs. Each time a different set of anchor points get selected from the probe structure, beginning with the three points furthest away from each other and the COA.
 * \return number of runs
 */
template<class T>
inline unsigned int svt_point_cloud<T>::getRuns( ) const
{
    return m_iRuns;
};

/**
 * Full or simple version of the matching algorithm to be performed.
 * \param bSimple if true only a reduced version is performed (faster but less accurate)
 */
template<class T>
inline void svt_point_cloud<T>::setSimple( bool bSimple )
{
    m_bSimple = bSimple;
};
/**
 * Full or simple version of the matching algorithm to be performed.
 * \return if true only a reduced version is performed (faster but less accurate)
 */
template<class T>
inline bool svt_point_cloud<T>::getSimple( ) const
{
    return m_bSimple;
};

/**
 * Set the next point selection scheme to COA
 */
template<class T>
inline void svt_point_cloud<T>::setNextPointCOA(bool bNextPointCOA)
{
    m_bNextPointCOA = bNextPointCOA;
};

/**
 * match two point clouds.
 * This function will perform a full N->M matching based on an achor-based search.
 * \param rPC second point cloud
 * \param rMatch vector of unsigned ints with the indices of the points of second cloud to which the points of this cloud are matched. This vector will get erased and then filled during the matching.
 * \param rMatrices vector of svt_matrix4 objects with the transformations according to the rMatch
 */
template<class T>
void svt_point_cloud<T>::match( svt_point_cloud<T>& rPC, vector< svt_matchResult<Real64> >& rMatch )
{
    // calculate COA
    T oCOA = coa();
    unsigned int i,j;

    unsigned int iFstIndex = 0;
    unsigned int iSndIndex = 0;
    unsigned int iTrdIndex = 0;

    vector< int > aOldAnchors;
    vector< svt_matrix4<Real64> > aBestSolution;
    vector< int > aBestMatch;

    // some status output
    svtout << endl;
    svtout << "Point-cloud matching..." << endl;
    svtout << "  Subcomponent has " << size() << " points " << endl;
    svtout << "  Gets docked into a structure with " << rPC.size() << " points " << endl;

    // size of the two pointclouds
    unsigned int iNumA = size();

    // sum of all seed atoms evaluated
    unsigned int iSumSeeds = 0;

    // array with all the results from all runs
    vector< svt_matchResult<Real64> > aResults;

    for(unsigned int iRun=0; iRun<m_iRuns; iRun++)
    {
	if (m_iRuns != 1)
	{
	    svtout << endl;
	    svtout << "  Run: " << iRun << endl;
	}

	// calc best anchor of model point cloud
	// 1st: which one is furthest away from the center
	Real64 fLength = 0.0;
	Real64 fDist = 0.0;
	for(i=0; i<iNumA; i++)
	{
	    fDist = (*this)[i].distance(oCOA);

	    if ( fDist > fLength)
	    {
		if (find(aOldAnchors.begin(), aOldAnchors.end(), (int)(i)) == aOldAnchors.end())
		{
		    fLength = fDist;
		    iFstIndex = i;
		}
	    }
	}

	// 2nd: which one is furthest away from the first point?
	Real64 fTmpDist = 0.0;
        fDist = 0.0;
	for(i=0; i<iNumA; i++)
	{
	    fTmpDist = (*this)[i].distance( (*this)[iFstIndex] );

	    if (i != iFstIndex && fTmpDist > fDist)
	    {
		if (find(aOldAnchors.begin(), aOldAnchors.end(), (int)(i)) == aOldAnchors.end())
		{
		    fDist = fTmpDist;
		    iSndIndex = i;
		}
	    }
	}

	// 3rd: which one is furthest away from the first two?
	Real64 fSumDist = 0.0;
	for(i=0; i<iNumA; i++)
	{
	    if (i != iFstIndex && i != iSndIndex)
	    {
		fDist = (*this)[i].distance( (*this)[iFstIndex] ) + (*this)[i].distance( (*this)[iSndIndex] );
		if (fDist > fSumDist)
		{
		    if (find(aOldAnchors.begin(), aOldAnchors.end(), (int)(i)) == aOldAnchors.end())
		    {
			fSumDist = fDist;
			iTrdIndex = i;
		    }
		}
	    }
	}

	svtout << "  First Anchor: " << iFstIndex << endl;
	svtout << "  Second Anchor: " << iSndIndex << endl;
	svtout << "  Third Anchor: " << iTrdIndex << endl;

	// run the matching
	vector< svt_matrix4<Real64> > aMatrix;
        vector< svt_matchResult<Real64> > aMatch;
	match( rPC, aMatch, (unsigned int)(iFstIndex), (unsigned int)(iSndIndex), (unsigned int)(iTrdIndex) );

        // append all the results to the global result array
	for(unsigned int k=0; k<aMatch.size(); k++)
            aResults.push_back( aMatch[k] );

        iSumSeeds += m_iNumSeeds;

	// add the anchors to the old anchors list
        aOldAnchors.push_back( iFstIndex );
        aOldAnchors.push_back( iSndIndex );
        aOldAnchors.push_back( iTrdIndex );
    }

    // analyze the found solutions
    sort( aResults.begin(), aResults.end() );

    // filter
    rMatch.clear();
    vector< svt_matchResult< Real64 > > aFinal;
    vector< svt_vector4<Real64> > aComs;
    svt_vector4<Real64> oTransVec;
    unsigned int iNum = aResults.size();
    for(i=0; i<iNum; i++)
    {
	oTransVec = aResults[i].getMatrix() * oCOA;

	for(j=0; j<aComs.size(); j++)
	{
	    if (aComs[j].distance( oTransVec ) < m_fUnique)
		break;
	}

	if (j >= aComs.size())
	{
	    aComs.push_back( oTransVec );
	    rMatch.push_back( aResults[i] );
	}
    }

    svtout << endl;

    if (rMatch.size() > 0)
	svtout << "  Best Result: " << rMatch[0].getScore() << endl;
    else
	svtout << "  Best Result: No result found!" << endl;

    m_iNumSeeds = iSumSeeds;

    // end
    svtout << "  Done." << endl;
}

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
template<class T>
void svt_point_cloud<T>::match( svt_point_cloud<T>& rPC, vector< svt_matchResult<Real64> >& rMatch, unsigned int iAnchorA, unsigned int iAnchorB, unsigned int iAnchorC )
{
    unsigned int i,j;

    // start time
    //long iStartTime = svt_getToD();

    // empty the match
    rMatch.clear();

    // average nn distance
    Real64 fEpsilon = averageNNDistance( 0.1 );
    setEpsilon( fEpsilon + (1.0 * fEpsilon) );

    // size of the two pointclouds
    unsigned int iNumB = rPC.size();

    // anchors
    unsigned int iFstIndex = iAnchorA;
    unsigned int iSndIndex = iAnchorB;
    unsigned int iTrdIndex = iAnchorC;

    // distances between anchors
    Real64 fDistAB = (*this)[iFstIndex].distance( (*this)[iSndIndex] );
    Real64 fDistBC = (*this)[iSndIndex].distance( (*this)[iTrdIndex] );
    Real64 fDistAC = (*this)[iFstIndex].distance( (*this)[iTrdIndex] );
    vector< svt_vector4<int> > aSeeds;

    // now find appropriate seed anchors in the scene, based on the distances...
    for(i=0; i<iNumB; i++)
	for(j=0; j<iNumB; j++)
	    if (i!=j)
	    {
		if (fabs(rPC[i].distance( rPC[j] ) - fDistAB) < m_fLambda)
		{
		    svt_vector4<int> oVec(i,j,-1);
		    aSeeds.push_back( oVec );

		}
	    }

    m_iNumSeeds = aSeeds.size();
    vector< svt_vector4<int> > aFinalSeeds;

    for(i=0; i<m_iNumSeeds; i++)
	for(j=0; j<iNumB; j++)
	{
	    if (aSeeds[i].x() != (int)(j) && aSeeds[i].y() != (int)(j))
	    {
		if (
		    ( fabs(rPC[j].distance( rPC[ aSeeds[i].x() ] ) - fDistAC) < m_fLambda &&
		      fabs(rPC[j].distance( rPC[ aSeeds[i].y() ] ) - fDistBC) < m_fLambda ) ||
		    ( fabs(rPC[j].distance( rPC[ aSeeds[i].y() ] ) - fDistAC) < m_fLambda &&
		      fabs(rPC[j].distance( rPC[ aSeeds[i].x() ] ) - fDistBC) < m_fLambda )
		   )
		{
		    svt_vector4<int> oVec = aSeeds[i];
		    oVec.z( j );
                    aFinalSeeds.push_back( oVec );
		}
	    }
	}

    m_iNumSeeds = aFinalSeeds.size();

    svtout << "  Number of found anchor matches: " << aFinalSeeds.size() << endl;

    // investigate the quality of the anchor matches
    svt_matrix4<Real64> oMatrix;
    vector<int> aMatches;
    vector< svt_matchResult<Real64> > aResults;
    Real64 fRMSD = 0.0;

    try
    {
        svt_exception::ui()->progressPopup("Match Completion Phase", 0, m_iNumSeeds);

        for(i=0; i<m_iNumSeeds; i++)
        {
            if (i%100 == 0)
            {
                {
                    svt_exception::ui()->progress( i, m_iNumSeeds );
                    //printf("svt_point_cloud  > %10i / %10i - (%05.2f)\r", i, m_iNumSeeds, fRMSD);
                    //fflush(stdout);
                }
            }

	    // run A.1
	    {
		vector<int> aModelAnchor;
		aModelAnchor.push_back( iFstIndex );
		aModelAnchor.push_back( iSndIndex );
		aModelAnchor.push_back( iTrdIndex );
		vector<int> aSceneAnchor;
		aSceneAnchor.push_back( aFinalSeeds[i].x() );
		aSceneAnchor.push_back( aFinalSeeds[i].y() );
		aSceneAnchor.push_back( aFinalSeeds[i].z() );

		oMatrix = optimize( &aModelAnchor, &aSceneAnchor, *this, rPC, true );

		if (aModelAnchor.size() > 3)
		{
		    fRMSD = calcRMSD( *this, rPC, oMatrix, &aModelAnchor, &aSceneAnchor);
		    aResults.push_back( svt_matchResult<Real64>( fRMSD, oMatrix, aModelAnchor, aSceneAnchor ) );
		}
	    }
	    // run A.2
/*	    {
		vector<int> aModelAnchor;
		aModelAnchor.push_back( iFstIndex );
		aModelAnchor.push_back( iSndIndex );
		aModelAnchor.push_back( iTrdIndex );
		vector<int> aSceneAnchor;
		aSceneAnchor.push_back( aFinalSeeds[i].x() );
		aSceneAnchor.push_back( aFinalSeeds[i].z() );
		aSceneAnchor.push_back( aFinalSeeds[i].y() );

		oMatrix = optimize( &aModelAnchor, &aSceneAnchor, *this, rPC, true );

		if (aModelAnchor.size() > 3)
		{
		    fRMSD = calcRMSD( *this, rPC, oMatrix, &aModelAnchor, &aSceneAnchor);
		    aResults.push_back( svt_matchResult<Real64>( fRMSD, oMatrix, aModelAnchor, aSceneAnchor ) );
		}
	    }
	    // run A.3
	    {
		vector<int> aModelAnchor;
		aModelAnchor.push_back( iFstIndex );
		aModelAnchor.push_back( iSndIndex );
		aModelAnchor.push_back( iTrdIndex );
		vector<int> aSceneAnchor;
		aSceneAnchor.push_back( aFinalSeeds[i].y() );
		aSceneAnchor.push_back( aFinalSeeds[i].x() );
		aSceneAnchor.push_back( aFinalSeeds[i].z() );

		oMatrix = optimize( &aModelAnchor, &aSceneAnchor, *this, rPC, true );

		if (aModelAnchor.size() > 3)
		{
		    fRMSD = calcRMSD( *this, rPC, oMatrix, &aModelAnchor, &aSceneAnchor);
		    aResults.push_back( svt_matchResult<Real64>( fRMSD, oMatrix, aModelAnchor, aSceneAnchor ) );
		}
	    }
	    // run A.4
	    {
		vector<int> aModelAnchor;
		aModelAnchor.push_back( iFstIndex );
		aModelAnchor.push_back( iSndIndex );
		aModelAnchor.push_back( iTrdIndex );
		vector<int> aSceneAnchor;
		aSceneAnchor.push_back( aFinalSeeds[i].y() );
		aSceneAnchor.push_back( aFinalSeeds[i].z() );
		aSceneAnchor.push_back( aFinalSeeds[i].x() );

		oMatrix = optimize( &aModelAnchor, &aSceneAnchor, *this, rPC, true );

		if (aModelAnchor.size() > 3)
		{
		    fRMSD = calcRMSD( *this, rPC, oMatrix, &aModelAnchor, &aSceneAnchor);
		    aResults.push_back( svt_matchResult<Real64>( fRMSD, oMatrix, aModelAnchor, aSceneAnchor ) );
		}
	    }
	    // run A.5
	    {
		vector<int> aModelAnchor;
		aModelAnchor.push_back( iFstIndex );
		aModelAnchor.push_back( iSndIndex );
		aModelAnchor.push_back( iTrdIndex );
		vector<int> aSceneAnchor;
		aSceneAnchor.push_back( aFinalSeeds[i].z() );
		aSceneAnchor.push_back( aFinalSeeds[i].x() );
		aSceneAnchor.push_back( aFinalSeeds[i].y() );

		oMatrix = optimize( &aModelAnchor, &aSceneAnchor, *this, rPC, true );

		if (aModelAnchor.size() > 3)
		{
		    fRMSD = calcRMSD( *this, rPC, oMatrix, &aModelAnchor, &aSceneAnchor);
		    aResults.push_back( svt_matchResult<Real64>( fRMSD, oMatrix, aModelAnchor, aSceneAnchor ) );
		}
	    }
	    // run A.6
	    {
		vector<int> aModelAnchor;
		aModelAnchor.push_back( iFstIndex );
		aModelAnchor.push_back( iSndIndex );
		aModelAnchor.push_back( iTrdIndex );
		vector<int> aSceneAnchor;
		aSceneAnchor.push_back( aFinalSeeds[i].z() );
		aSceneAnchor.push_back( aFinalSeeds[i].y() );
		aSceneAnchor.push_back( aFinalSeeds[i].x() );

		oMatrix = optimize( &aModelAnchor, &aSceneAnchor, *this, rPC, true );

		if (aModelAnchor.size() > 3)
		{
		    fRMSD = calcRMSD( *this, rPC, oMatrix, &aModelAnchor, &aSceneAnchor);
		    aResults.push_back( svt_matchResult<Real64>( fRMSD, oMatrix, aModelAnchor, aSceneAnchor ) );
		}
            }
            */

        }
        svt_exception::ui()->progressPopdown();

    } catch (svt_userInterrupt&)
    {
        return;
    }


    // FIXME: Debug code...
    /*cout << endl;
    for (unsigned int i=0; i<aBestModelM.size(); i++)
	printf( "%02i,", aBestModelM[i] );
    cout << endl;

    for (unsigned int i=0; i<aBestSceneM.size(); i++)
	if (aBestSceneM[i] != NOMATCH)
	    printf( "%02i,", aBestSceneM[i] );
	else
	    printf( " - " );
    cout << endl;
    getchar();
    */

    rMatch.clear();

    // elapsed time
    //long iETime = svt_getToD() - iStartTime;
    //unsigned int iHours = (unsigned int)((double)(iETime) / 3.6E9);
    //iETime -= (long)(iHours * 3.6E9);
    //unsigned int iMins = (unsigned int)((double)(iETime) / 6.0E7);
    //iETime -= (long)(iMins * 6.0E7);
    //unsigned int iSecs = (unsigned int)((double)(iETime) / 1.0E6);
    //svtout << "Elapsed time: " << iHours << ":" << iMins << ":" << iSecs << endl;

    if (aResults.size() == 0)
    {
	svtout << "  Error - there was no valid result in this run!" << endl;
        return;
    }

    // sort the resulting matrices
    sort( aResults.begin(), aResults.end() );
    // copy the 10 best results into a matrix array
    for(i=0; i < aResults.size() && i < 1000; i++)
	rMatch.push_back( aResults[i] );

    //svtout << "  Best Result: " << aResults[0].getScore() << endl;
};

/**
 * Get number of seed/anchor matches evaluated during last matching run.
 */
template<class T>
unsigned int svt_point_cloud<T>::getNumSeeds() const
{
    return m_iNumSeeds;
};

/**
 * Calculate the cross-correlation between this pointcloud and a volumetric object. The idea is to use the points in the pointcloud to sample the volume and to
 * form thereby a reduce cross-correlation coefficient equation.
 * See also:
 * Stefan Birmanns and Willy Wriggers. Interactive Fitting Augmented by Force-Feedback and Virtual Reality. J. Struct. Biol., 2003, Vol. 144, pp. 123-131.
 * \param rPointMat reference to svt_matrix4 object with the initial transformation matrix of the volumetric object
 * \param rVol reference to svt_volume object
 * \param rVolMat reference to svt_matrix4 object with the transformation matrix of the volumetric object
 * \return the cross-correlation
 */
template<class T>
Real64 svt_point_cloud<T>::calcCC(svt_matrix4<Real64>& rPointMat, svt_volume<Real64>& rVol, svt_matrix4<Real64>& rVolMat)
{
    Real64 fCC = 0.0;
    int iVoxelX, iVoxelY, iVoxelZ;
    Real64 fWidth = rVol.getWidth();
    Real64 fVoxelValue = 0.0;
    int iCount = 0;

    // inner = -1, if codebook vector is inner vector from laplace data
    //Real64 fInnerFac = 1; //(inner ? -1. : 1.);

    // calculate the center of the volume data
    T oOrigin;
    oOrigin.x( (fWidth * rVol.getSizeX() ) * 0.5f);
    oOrigin.y( (fWidth * rVol.getSizeY() ) * 0.5f);
    oOrigin.z( (fWidth * rVol.getSizeZ() ) * 0.5f);

    // transformation of point cloud into volumetric data
    svt_matrix4<Real64> oTransMatrix = rVolMat;
    oTransMatrix.invert();
    oTransMatrix *= rPointMat;

    // now calculate the force for each codebook vector
    for(unsigned int i=0;i<size();i++)
    {
	// get original position
	T oVec = (*this)[i];
	// move it into the situs coordinate system
	oVec = oTransMatrix * oVec;
	//oVec += oOrigin;

	// get the index of the voxel lying under the cv
	iVoxelX = (int)(floor(oVec.x() / fWidth));
	iVoxelY = (int)(floor(oVec.y() / fWidth));
	iVoxelZ = (int)(floor(oVec.z() / fWidth));

	// calculate the gradient at the voxel position - negativ weight for inner codebookvectors
	if (iVoxelX < 0 || iVoxelY < 0 || iVoxelZ < 0 || iVoxelX >= (int)(rVol.getSizeX()) || iVoxelY >= (int)(rVol.getSizeY()) || iVoxelZ >= (int)(rVol.getSizeZ()))
	    continue;

	// position of voxel inside the volume
	Real64 fVoxelPosX = iVoxelX * fWidth;
	Real64 fVoxelPosY = iVoxelY * fWidth;
	Real64 fVoxelPosZ = iVoxelZ * fWidth;

	// position of cv inside the voxel
	Real64 fCVXV = (oVec.x() - fVoxelPosX) / fWidth; fCVXV += iVoxelX;
	Real64 fCVYV = (oVec.y() - fVoxelPosY) / fWidth; fCVYV += iVoxelY;
	Real64 fCVZV = (oVec.z() - fVoxelPosZ) / fWidth; fCVZV += iVoxelZ;

        // get interpolated value from volume
	fVoxelValue = rVol.getIntValue( fCVXV, fCVYV, fCVZV );
	if (fVoxelValue != 0.0)
            iCount++;
        fCC += fVoxelValue;
    }

    if (iCount != 0)
	fCC /= (Real64)(iCount);
    else
        fCC = 0;

    return fCC;
}

///////////////////////////////////////////////////////////////////////////////
// Private functions
///////////////////////////////////////////////////////////////////////////////

/**
 * sort eigenvectors according to their eigenvalues
 * \param pEigenvectors pointer to svt_matrix with the eigenvectors as column vectors
 * \param pEigenvalues pointer to svt_vector with the eigenvalues
 */
template<class T> 
void svt_point_cloud<T>::eigensort(svt_matrix4<Real64>& rEigenvectors, T& rEigenvalues)
{
    Real64 fP;
    int i,j,k;

    for(i=0; i<4; i++)
    {
	fP = rEigenvalues[k=i];

	for (j=i+1; j<4; j++)
	    if (rEigenvalues[j] >= fP)
		fP = rEigenvalues[k=j];

	if (k != i)
	{
	    rEigenvalues[k] = rEigenvalues[i];
	    rEigenvalues[i] = fP;
	    for (j=0; j<4; j++)
	    {
		fP = rEigenvectors[j][i];
		rEigenvectors[j][i] = rEigenvectors[j][k];
		rEigenvectors[j][k] = fP;
	    }
	}
    }
};

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
 * \param rM vector of ints with the indices of the first (model) vectors that are used
 * \param rS vector of ints with the indices of the second (scene) vectors that are used
 * \param rModel vector of svt_vector4 with the model vectors
 * \param rScene vector of svt_vector4 with the scene vectors
 * \return matrix with optimal transformation of the model into the scene
 */
template<class T> 
svt_matrix4<Real64> svt_point_cloud<T>::kearsley(vector< int >& rMM, vector< int >& rSM, svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene)
{
    unsigned int i;
    unsigned int iCount = rMM.size();

    vector< int > rM;
    vector< int > rS;

    for(i=0; i<iCount; i++)
    {
	if (rSM[i] != NOMATCH)
	{
	    rM.push_back( rMM[i] );
	    rS.push_back( rSM[i] );
	}
    }
    iCount = rM.size();

    // precomp.: calc COA for both model and scene and transformation
    T oModelCOA;
    for(i=0; i<iCount; i++)
	    oModelCOA = oModelCOA + rModel[rM[i]];
    oModelCOA = oModelCOA / (double)(iCount);

    T oSceneCOA;
    for(i=0; i<iCount; i++)
	oSceneCOA = oSceneCOA + rScene[rS[i]];
    oSceneCOA = oSceneCOA / (double)(iCount);

    // first step: setup Q matrix (see kearsley acta cryst a45)

    // distance plus and minus
    T oDP;
    T oDM;

    // setup matrix
    svt_matrix4<Real64> oQ;
    // as svt_matrix creates an identity matrix, we have to set the diagonal elements to 0
    oQ[0][0] = 0.0;
    oQ[1][1] = 0.0;
    oQ[2][2] = 0.0;
    oQ[3][3] = 0.0;

    // now construct matrix
    for(i=0; i<iCount; i++)
    {
        oDM = (rModel[rM[i]] - oModelCOA) - (rScene[rS[i]] - oSceneCOA);
        oDP = (rModel[rM[i]] - oModelCOA) + (rScene[rS[i]] - oSceneCOA);

        oQ[0][0] += (oDM.x()*oDM.x()) + (oDM.y()*oDM.y()) + (oDM.z()*oDM.z());
	oQ[1][1] += (oDM.x()*oDM.x()) + (oDP.y()*oDP.y()) + (oDP.z()*oDP.z());
	oQ[2][2] += (oDP.x()*oDP.x()) + (oDM.y()*oDM.y()) + (oDP.z()*oDP.z());
	oQ[3][3] += (oDP.x()*oDP.x()) + (oDP.y()*oDP.y()) + (oDM.z()*oDM.z());

	oQ[0][1] += (oDP.y()*oDM.z()) - (oDM.y()*oDP.z());
	oQ[0][2] += (oDM.x()*oDP.z()) - (oDP.x()*oDM.z());
	oQ[0][3] += (oDP.x()*oDM.y()) - (oDM.x()*oDP.y());

        oQ[1][0] += (oDP.y()*oDM.z()) - (oDM.y()*oDP.z());
	oQ[1][2] += (oDM.x()*oDM.y()) - (oDP.x()*oDP.y());
	oQ[1][3] += (oDM.x()*oDM.z()) - (oDP.x()*oDP.z());

	oQ[2][0] += (oDM.x()*oDP.z()) - (oDP.x()*oDM.z());
	oQ[2][1] += (oDM.x()*oDM.y()) - (oDP.x()*oDP.y());
	oQ[2][3] += (oDM.y()*oDM.z()) - (oDP.y()*oDP.z());

	oQ[3][0] += (oDP.x()*oDM.y()) - (oDM.x()*oDP.y());
	oQ[3][1] += (oDM.x()*oDM.z()) - (oDP.x()*oDP.z());
	oQ[3][2] += (oDM.y()*oDM.z()) - (oDP.y()*oDP.z());
    }

    // second step: solve matrix using Jacobi transformation
    svt_matrix4<Real64> oEigenvectors;
    T oEigenvalues;
    oQ.jacobi(oEigenvectors, oEigenvalues);

    // third step: sort the eigenvectors according to their eigenvalues
    eigensort(oEigenvectors, oEigenvalues);

    // forth step: generate transformation matrix
    svt_matrix4<Real64> oFinal;
    svt_matrix4<Real64> oRot;

    oFinal[0][3] -= oModelCOA[0];
    oFinal[1][3] -= oModelCOA[1];
    oFinal[2][3] -= oModelCOA[2];

    oRot[0][0] = (oEigenvectors[0][3]*oEigenvectors[0][3]) + (oEigenvectors[1][3]*oEigenvectors[1][3]) - (oEigenvectors[2][3]*oEigenvectors[2][3]) - (oEigenvectors[3][3]*oEigenvectors[3][3]);
    oRot[1][0] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[2][3] + oEigenvectors[0][3] * oEigenvectors[3][3]);
    oRot[2][0] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[3][3] - oEigenvectors[0][3] * oEigenvectors[2][3]);

    oRot[0][1] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[2][3] - oEigenvectors[0][3] * oEigenvectors[3][3]);
    oRot[1][1] = oEigenvectors[0][3]*oEigenvectors[0][3] + oEigenvectors[2][3]*oEigenvectors[2][3] - oEigenvectors[1][3]*oEigenvectors[1][3] - oEigenvectors[3][3]*oEigenvectors[3][3];
    oRot[2][1] = 2.0 * (oEigenvectors[2][3] * oEigenvectors[3][3] + oEigenvectors[0][3] * oEigenvectors[1][3]);

    oRot[0][2] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[3][3] + oEigenvectors[0][3] * oEigenvectors[2][3]);
    oRot[1][2] = 2.0 * (oEigenvectors[2][3] * oEigenvectors[3][3] - oEigenvectors[0][3] * oEigenvectors[1][3]);
    oRot[2][2] = oEigenvectors[0][3]*oEigenvectors[0][3] + oEigenvectors[3][3]*oEigenvectors[3][3] - oEigenvectors[1][3]*oEigenvectors[1][3] - oEigenvectors[2][3]*oEigenvectors[2][3];

    oFinal = oRot * oFinal;

    oFinal[0][3] += oSceneCOA[0];
    oFinal[1][3] += oSceneCOA[1];
    oFinal[2][3] += oSceneCOA[2];

    return oFinal;
};

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
template<class T> 
svt_matrix4<Real64> svt_point_cloud<T>::kearsley(svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene)
{
    unsigned int i;
    unsigned int iCount = rModel.size();

    // precomp.: calc COA for both model and scene and transformation
    T oModelCOA;
    for(i=0; i<iCount; i++)
	    oModelCOA = oModelCOA + rModel[i];
    oModelCOA = oModelCOA / (double)(iCount);

    T oSceneCOA;
    for(i=0; i<iCount; i++)
	oSceneCOA = oSceneCOA + rScene[i];
    oSceneCOA = oSceneCOA / (double)(iCount);

    T oTransCOA;
    oTransCOA = oSceneCOA - oModelCOA;

    // first step: setup Q matrix (see kearsley acta cryst a45)

    // distance plus and minus
    T oDP;
    T oDM;

    // setup matrix
    svt_matrix4<Real64> oQ;
    // as svt_matrix creates an identity matrix, we have to set the diagonal elements to 0
    oQ[0][0] = 0.0;
    oQ[1][1] = 0.0;
    oQ[2][2] = 0.0;
    oQ[3][3] = 0.0;

    // now construct matrix
    for(i=0; i<iCount; i++)
    {
	oDM = (rModel[i] - oModelCOA) - (rScene[i] - oSceneCOA);
	oDP = (rModel[i] - oModelCOA) + (rScene[i] - oSceneCOA);

	oQ[0][0] += (oDM.x()*oDM.x()) + (oDM.y()*oDM.y()) + (oDM.z()*oDM.z());
	oQ[1][1] += (oDM.x()*oDM.x()) + (oDP.y()*oDP.y()) + (oDP.z()*oDP.z());
	oQ[2][2] += (oDP.x()*oDP.x()) + (oDM.y()*oDM.y()) + (oDP.z()*oDP.z());
	oQ[3][3] += (oDP.x()*oDP.x()) + (oDP.y()*oDP.y()) + (oDM.z()*oDM.z());

	oQ[0][1] += (oDP.y()*oDM.z()) - (oDM.y()*oDP.z());
	oQ[0][2] += (oDM.x()*oDP.z()) - (oDP.x()*oDM.z());
	oQ[0][3] += (oDP.x()*oDM.y()) - (oDM.x()*oDP.y());

	oQ[1][0] += (oDP.y()*oDM.z()) - (oDM.y()*oDP.z());
	oQ[1][2] += (oDM.x()*oDM.y()) - (oDP.x()*oDP.y());
	oQ[1][3] += (oDM.x()*oDM.z()) - (oDP.x()*oDP.z());

	oQ[2][0] += (oDM.x()*oDP.z()) - (oDP.x()*oDM.z());
	oQ[2][1] += (oDM.x()*oDM.y()) - (oDP.x()*oDP.y());
	oQ[2][3] += (oDM.y()*oDM.z()) - (oDP.y()*oDP.z());

	oQ[3][0] += (oDP.x()*oDM.y()) - (oDM.x()*oDP.y());
	oQ[3][1] += (oDM.x()*oDM.z()) - (oDP.x()*oDP.z());
	oQ[3][2] += (oDM.y()*oDM.z()) - (oDP.y()*oDP.z());
    }

    // second step: solve matrix using Jacobi transformation
    svt_matrix4<Real64> oEigenvectors;
    T oEigenvalues;
    oQ.jacobi(oEigenvectors, oEigenvalues);

    // third step: sort the eigenvectors according to their eigenvalues
    eigensort(oEigenvectors, oEigenvalues);

    // forth step: generate transformation matrix
    svt_matrix4<Real64> oFinal;
    svt_matrix4<Real64> oRot;

    oFinal[0][3] -= oModelCOA[0];
    oFinal[1][3] -= oModelCOA[1];
    oFinal[2][3] -= oModelCOA[2];

    oRot[0][0] = (oEigenvectors[0][3]*oEigenvectors[0][3]) + (oEigenvectors[1][3]*oEigenvectors[1][3]) - (oEigenvectors[2][3]*oEigenvectors[2][3]) - (oEigenvectors[3][3]*oEigenvectors[3][3]);
    oRot[1][0] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[2][3] + oEigenvectors[0][3] * oEigenvectors[3][3]);
    oRot[2][0] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[3][3] - oEigenvectors[0][3] * oEigenvectors[2][3]);

    oRot[0][1] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[2][3] - oEigenvectors[0][3] * oEigenvectors[3][3]);
    oRot[1][1] = oEigenvectors[0][3]*oEigenvectors[0][3] + oEigenvectors[2][3]*oEigenvectors[2][3] - oEigenvectors[1][3]*oEigenvectors[1][3] - oEigenvectors[3][3]*oEigenvectors[3][3];
    oRot[2][1] = 2.0 * (oEigenvectors[2][3] * oEigenvectors[3][3] + oEigenvectors[0][3] * oEigenvectors[1][3]);

    oRot[0][2] = 2.0 * (oEigenvectors[1][3] * oEigenvectors[3][3] + oEigenvectors[0][3] * oEigenvectors[2][3]);
    oRot[1][2] = 2.0 * (oEigenvectors[2][3] * oEigenvectors[3][3] - oEigenvectors[0][3] * oEigenvectors[1][3]);
    oRot[2][2] = oEigenvectors[0][3]*oEigenvectors[0][3] + oEigenvectors[3][3]*oEigenvectors[3][3] - oEigenvectors[1][3]*oEigenvectors[1][3] - oEigenvectors[2][3]*oEigenvectors[2][3];

    oFinal = oRot * oFinal;

    oFinal[0][3] += oSceneCOA[0];
    oFinal[1][3] += oSceneCOA[1];
    oFinal[2][3] += oSceneCOA[2];

    return oFinal;
};

/**
 * optimize_simple - subroutine for the match() procedure
 */
template<class T> 
svt_matrix4<Real64> svt_point_cloud<T>::optimize_simple( vector<int>* pModelMatch, vector<int>* pSceneMatch, svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, bool)
{
    svtout << "SIMPLE!" << endl;

    svt_matrix4<Real64> oMatrix = kearsley( *pModelMatch, *pSceneMatch, rModel, rScene );

    // transform...
    unsigned int i,j;
    unsigned int iIndex;
    svt_vector4<Real64> oVec;

    for(i=0; i<rModel.size(); i++)
    {
	// is vector i unmatched?
	for(j=0; j<pModelMatch->size(); j++)
	    if ((*pModelMatch)[j] == (int)(i))
		break;

	// yes...
	if (j == pModelMatch->size())
	{
	    oVec = oMatrix * rModel[i];

	    iIndex = rScene.nearestNeighbor( oVec );

	    // is vector iIndex unmatched?
	    for(j=0; j<pSceneMatch->size(); j++)
		if ((*pSceneMatch)[j] == (int)(iIndex))
		    break;

	    // yes...
	    if (j == pSceneMatch->size() )
	    {
		pModelMatch->push_back( i );
		pSceneMatch->push_back( iIndex );
	    }
	}
    }

    oMatrix = kearsley( *pModelMatch, *pSceneMatch, rModel, rScene );

    return oMatrix;
};

/**
 * optimize - subroutine for the match() procedure
 */
template<class T> 
svt_matrix4<Real64> svt_point_cloud<T>::optimize( vector<int>* pModelMatch, vector<int>* pSceneMatch, svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, bool bInit )
{
    static Real64 fBestRMSD = 1.0E10;
    static vector<int> aBestModelMatch;
    static vector<int> aBestSceneMatch;
    static svt_matrix4<Real64> oBestMatrix;
    static unsigned int iNumM; 
    static unsigned int iNumS;
    static unsigned int iLevel;
    static vector< _pc_dist<Real64> > aDistToCOA;

    if (bInit == true)
    {
	fBestRMSD = 1.0E10;
	iNumM = rModel.size();
	iNumS = rScene.size();
        iLevel = 0;

        if (m_bNextPointCOA == true)
	{
	    // calculate distances of all model vectors to COA
	    T oCOA = rModel.coa();
	    aDistToCOA.clear();
	    for(unsigned int i=0; i<iNumM; i++)
		aDistToCOA.push_back( _pc_dist<Real64>( rModel[i].distance( oCOA ), i) );

	    sort( aDistToCOA.begin(), aDistToCOA.end() );
	}

    } else
        iLevel++;

    // FIXME: Debug output
    //svtout << "DEBUG: iLevel:" << iLevel << endl;
    //getchar();

    // find an unmatched model vector...
    T oVec;
    unsigned int iIndex=0, j=0, k=0;

    if (pModelMatch->size() < rModel.size())
    {
        // get optimal transformation for the old match
	svt_matrix4<Real64> oMatrix = kearsley( *pModelMatch, *pSceneMatch, rModel, rScene );

	// look for an unmatched point by moving towards the COA...
	if (m_bNextPointCOA)
	{
	    unsigned int i;
	    for(i=aDistToCOA.size(); i>0; i--)
		if (find(pModelMatch->begin(), pModelMatch->end(), (int)(aDistToCOA[i-1].getIndex()) ) == pModelMatch->end())
		    break;

	    iIndex = aDistToCOA[i-1].getIndex();

	} else {

	    // look for an unmatched point that is closest to a scenepoint
	    Real64 fMinDist = 1.0E10;
	    Real64 fDist = 0.0;
	    for(unsigned int i=0; i<rModel.size(); i++)
	    {
		// which model point is unmatched?
		if (find(pModelMatch->begin(), pModelMatch->end(), (int)(i) ) == pModelMatch->end())
		{
		    // now loop over the scene...
		    for(k=0; k<rScene.size(); k++)
		    {
			// which scene point is unmatched?
			if (find(pSceneMatch->begin(), pSceneMatch->end(), (int)(k) ) == pSceneMatch->end())
			{
			    // calculate distance to unmatched model vector i
			    fDist = rModel[i].distance( rScene[k] );

			    // closest so far?
			    if ( fDist < fMinDist)
			    {
				fMinDist = fDist;
				iIndex = i;
			    }
			}
		    }
		}
	    }
	}

	// FIXME: Debug output
	/*cout << endl;
	for (unsigned int i=0; i<pModelMatch->size(); i++)
	    printf( "%03i,", (*pModelMatch)[i] );
        printf("%03i", iIndex );
	cout << endl;

	for (unsigned int i=0; i<pSceneMatch->size(); i++)
	    if ((*pSceneMatch)[i] != NOMATCH)
		printf( "%03i,", (*pSceneMatch)[i] );
	    else
		printf( "--- " );
	cout << endl;

        svtout << oMatrix << endl;

	svt_point_cloud< svt_vector4<Real64> > oWrite;
	for(unsigned int iI=0; iI<rModel.size(); iI++)
	{
            oVec = oMatrix * rModel[iI];
	    oWrite.addPoint( oVec );
	}
        oWrite.writePDB( "optimize.pdb" );
	getchar();*/

	// transform
	oVec = oMatrix * rModel[iIndex];

	// now lets find the nearest neighbors in the scene...
	vector< _pc_dist<Real64> > aNeighbors;
	Real64 fDist = 0.0;
	for(j=0; j<iNumS; j++)
	{
            // is this scene point unmatched?
	    for(k=0; k<pSceneMatch->size(); k++)
	    {
		//cout << "pSceneMatch[" << k << "]:" << (*pSceneMatch)[k] << endl;

		if ((*pSceneMatch)[k] == (int)j)
		    break;
	    }
            // yes...
	    if (k >= pSceneMatch->size())
	    {
		fDist = rScene[j].distance( oVec );

		//cout << j << " - fDist: " << fDist << " m_fGamma: " << m_fGamma << endl;

		if (fDist < m_fGamma)
		    aNeighbors.push_back(_pc_dist<Real64>(fDist, j));
	    }
	}

	// test all the points
	unsigned int iNumN = aNeighbors.size();
	sort(aNeighbors.begin(), aNeighbors.end());
	unsigned int iCount = 0;

        // do we have potential matching points?
        if (iNumN > 0)
	{
	    for(j=0; j<iNumN; j++)
	    {
		if (iCount < m_iZoneSize)
		{
		    // FIXME: Debug...
		    //svtout << "DEBUG: Add Match..." << endl;

		    vector<int>* pTmpMatchModel = new vector<int>; *pTmpMatchModel = *pModelMatch;
		    vector<int>* pTmpMatchScene = new vector<int>; *pTmpMatchScene = *pSceneMatch;
		    pTmpMatchModel->push_back( iIndex );
		    pTmpMatchScene->push_back( aNeighbors[j].getIndex() );
		    optimize( pTmpMatchModel, pTmpMatchScene, rModel, rScene, false );
		    delete pTmpMatchModel;
		    delete pTmpMatchScene;
		    iCount++;
		}
	    }
            // no, no potential matches, so we can try a NOMATCH, but not too many...
	}
	if (iNumN == 0 || aNeighbors[0].getScore() > 3.0)
	{
	    // as we sort by nearest neighbor distance, it is strange that we did not find any more neighbors.
	    /*if (rModel.size() - pModelMatch->size() < m_iMaxNoMatch)
	    {
		svt_matrix4<Real64> oMatrix = kearsley( *pModelMatch, *pSceneMatch, rModel, rScene );
		Real64 fRMSD = calcRMSD( rModel, rScene, oMatrix, pModelMatch, pSceneMatch );

                // fill the rest with NOMATCH
		for(i=0; i<rModel.size(); i++)
		{
		    // which model point is unmatched?
		    if (find(pModelMatch->begin(), pModelMatch->end(), (int)(i) ) == pModelMatch->end())
		    {
			pModelMatch->push_back( iIndex );
			pSceneMatch->push_back( NOMATCH );
		    }
		}

		// do we have a winner?
		if (fRMSD < fBestRMSD)
		{
		    fBestRMSD = fRMSD;
		    oBestMatrix = oMatrix;
		    aBestModelMatch = *pModelMatch;
		    aBestSceneMatch = *pSceneMatch;
		}
	    };*/

	    unsigned int iNoMatch = 0;
	    for(j=0; j<pSceneMatch->size(); j++)
		if ((*pSceneMatch)[j] == NOMATCH)
                    iNoMatch++;

            // if there are no more than iMaxNoMatch blind matches, try them
	    if (iNoMatch < m_iMaxNoMatch)
	    {
		// FIXME: Debug...
		//svtout << "DEBUG: Add NoMatch..." << endl;

		vector<int>* pTmpMatchModel = new vector<int>; *pTmpMatchModel = *pModelMatch;
		vector<int>* pTmpMatchScene = new vector<int>; *pTmpMatchScene = *pSceneMatch;
		pTmpMatchModel->push_back( iIndex );
		pTmpMatchScene->push_back( NOMATCH );
		optimize( pTmpMatchModel, pTmpMatchScene, rModel, rScene, false );
		delete pTmpMatchModel;
		delete pTmpMatchScene;

		/*} else {
		// FIXME: Debug...
		svtout << "DEBUG: No more wildcards available" << endl;
		}*/
	    }
	}

    // no unmatched vector left, so lets evaluate this solution...
    } else {

	if (pModelMatch->size() == rModel.size())
	{
	    //svtout << "DEBUG: OK, complete!" << endl;

	    svt_matrix4<Real64> oMatrix = kearsley( *pModelMatch, *pSceneMatch, rModel, rScene );
            Real64 fRMSD = 0.0;
	    if (m_iMaxNoMatch == 0)
	    {
		fRMSD = calcRMSD( rModel, rScene, oMatrix, pModelMatch, pSceneMatch );

	    } else {

		Real64 fPercent = (Real64)(rModel.size()) / (Real64)(m_iMaxNoMatch);
		if (fPercent > 1.0)
                    fPercent = 1.0;
                fRMSD = rModel.rmsd_NN_Outliers( rScene, fPercent );
	    }

	    //svtout << "DEBUG: fRMSD: " << fRMSD << endl;

	    // do we have a winner?
	    if (fRMSD < fBestRMSD)
	    {
		fBestRMSD = fRMSD;
		oBestMatrix = oMatrix;
		aBestModelMatch = *pModelMatch;
		aBestSceneMatch = *pSceneMatch;

		unsigned int iNoMatch = 0;
		for(j=0; j<pSceneMatch->size(); j++)
		    if ((*pSceneMatch)[j] == NOMATCH)
			iNoMatch++;
	    }
	} 

    }

    // OK, do we exit this now? Then copy the best match...
    if (bInit == true)
    {
	if (aBestModelMatch.size() == rModel.size())
	{
	    //svtout << "DEBUG: End of recursion, plus matching complete!" << endl;

	    /*svt_matrix4<Real64> oMatrix = kearsley( *pModelMatch, *pSceneMatch, rModel, rScene );
	    Real64 fRMSD = calcRMSD( rModel, rScene, oMatrix, pModelMatch, pSceneMatch );

	    svtout << "DEBUG: fRMSD: " << fRMSD << endl;
	    svtout << "DEBUG: fBestRMSD: " << fBestRMSD << endl;

	    // do we have a winner?
	    if (fRMSD < fBestRMSD)
	    {
		svtout << "DEBUG: Winnerrrrr!" << endl;

		fBestRMSD = fRMSD;
		oBestMatrix = oMatrix;
		aBestModelMatch = *pModelMatch;
		aBestSceneMatch = *pSceneMatch;

		unsigned int iNoMatch = 0;
		for(j=0; j<pSceneMatch->size(); j++)
		    if ((*pSceneMatch)[j] == NOMATCH)
			iNoMatch++;

		if (iNoMatch != 0)
		    svtout << endl << "Wildcards used: " << iNoMatch << endl;

	    }*/
	    (*pModelMatch) = aBestModelMatch;
	    (*pSceneMatch) = aBestSceneMatch;
	}
    }

    return oBestMatrix;
};

/**
 * internal convenience function - no real difference with hausdorff - only here we don't need to create/copy a svt_point_cloud
 */
template<class T> 
Real64 svt_point_cloud<T>::calcHausdorff( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix)
{
    unsigned int iNumA = rModel.size();
    unsigned int iNumB = rScene.size();
    unsigned int i,j;

    Real64 fMinDist;
    Real64 fHD = 0.0;
    T oVec;
    int iIndexA = 0, iIndexB = 0;
    int iMinIndex = 0;

    for(i=0; i<iNumA; i++)
    {
	oVec = rMatrix * rModel[i];
	fMinDist = 1.0E10;
	iMinIndex = -1;

	for(j=0; j<iNumB; j++)
	{
	    if (oVec.distance( rScene[j] ) < fMinDist)
	    {
		fMinDist = oVec.distance( rScene[j] );
		iMinIndex = j;
	    }
	}

	if (fMinDist > fHD)
	{
	    fHD = fMinDist;
	    iIndexA = i;
	    iIndexB = iMinIndex;
	}
    }

    return fHD;
};

/**
 * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
 */
template<class T>
Real64 svt_point_cloud<T>::calcRMSD( svt_point_cloud<T>& rModel, svt_matrix4<Real64> oMatA, svt_matrix4<Real64> oMatB)
{
    unsigned int iNum = rModel.size();
    unsigned int i,j;

    Real64 fMinDist;
    Real64 fDist;
    Real64 fRMSD = 0.0;
    T oVecA;
    T oVecB;

    for(i=0; i<iNum; i++)
    {
	oVecA = oMatA * rModel[i];
	fMinDist = 1.0E10;

	for(j=0; j<iNum; j++)
	{
            oVecB = oMatB * rModel[j];
            fDist = oVecA.distanceSq( oVecB );

	    if ( fDist < fMinDist)
		fMinDist = fDist;
	}

	fRMSD += fMinDist;
    }

    fRMSD /= iNum;
    fRMSD = sqrt( fRMSD );

    return fRMSD;
};

/**
 * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
 */
template<class T>
Real64 svt_point_cloud<T>::calcRMSD( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix)
{
    unsigned int iNumA = rModel.size();
    unsigned int iNumB = rScene.size();
    unsigned int i,j;

    Real64 fMinDist;
    Real64 fRMSD = 0.0;
    T oVec;

    for(i=0; i<iNumA; i++)
    {
	oVec = rMatrix * rModel[i];
	fMinDist = 1.0E10;

	for(j=0; j<iNumB; j++)
	{
	    if (oVec.distanceSq( rScene[j] ) < fMinDist)
		fMinDist = oVec.distanceSq( rScene[j] );
	}

	fRMSD += fMinDist;
    }

    fRMSD /= iNumA;
    fRMSD = sqrt( fRMSD );

    return fRMSD;
};

/**
 * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
 * plus here we take the matching into account! If NOMATCH this point will not be used.
 */
template<class T>
Real64 svt_point_cloud<T>::calcRMSD( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix, vector<int>* pSceneMatch)
{
    unsigned int iNumA = rModel.size();
    unsigned int iNumB = rScene.size();
    unsigned int i,j;

    if (iNumA != (*pSceneMatch).size())
    {
	svtout << "DEBUG: calcRMSD: pSceneMatch does not contain enough point matches!!!" << endl;
	exit(1);
    }

    Real64 fMinDist;
    Real64 fRMSD = 0.0;
    int iCount = 0;
    T oVec;

    for(i=0; i<iNumA; i++)
    {
	if ((*pSceneMatch)[i] != NOMATCH)
	{

	    oVec = rMatrix * rModel[i];
	    fMinDist = 1.0E10;

	    for(j=0; j<iNumB; j++)
	    {
		if (oVec.distanceSq( rScene[j] ) < fMinDist)
		    fMinDist = oVec.distanceSq( rScene[j] );
	    }

	    fRMSD += fMinDist;
	    iCount++;

	} else

            fRMSD += m_fSkipPenalty;
    }

    fRMSD /= iCount;
    fRMSD = sqrt( fRMSD );

    return fRMSD;
};

/**
 * internal convenience function - no real difference with rmsd - only here we don't need to create/copy a svt_point_cloud
 * plus here we take the matching into account! If NOMATCH this point will not be used.
 */
template<class T>
Real64 svt_point_cloud<T>::calcRMSD( svt_point_cloud<T>& rModel, svt_point_cloud<T>& rScene, svt_matrix4<Real64>& rMatrix, vector<int>* pModelMatch, vector<int>* pSceneMatch)
{

    /**svt_point_cloud<T> oTmp = rMatrix * rModel;

    return rScene.rmsd_NN( oTmp );*/

    unsigned int iNum = pModelMatch->size();
    unsigned int i;

    if (iNum != (*pSceneMatch).size())
    {
	svtout << "DEBUG: calcRMSD: pSceneMatch does not contain enough point matches!!!" << endl;
	exit(1);
    }

    Real64 fRMSD = 0.0;
    int iCount = 0;
    T oVec;

    for(i=0; i<iNum; i++)
    {
	if ((*pSceneMatch)[i] != NOMATCH)
	{
	    oVec = rMatrix * rModel[(*pModelMatch)[i]];
	    fRMSD += oVec.distanceSq( rScene[(*pSceneMatch)[i]] );
            iCount++;
	}
    }

    fRMSD /= iCount;
    fRMSD = sqrt( fRMSD );

    return fRMSD;
};
