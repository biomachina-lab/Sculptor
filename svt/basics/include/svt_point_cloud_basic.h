/***************************************************************************
                          svt_point_cloud_basic
                          ---------------------
    begin                : 02/10/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Implementation of basic functionality of svt_point_cloud
///////////////////////////////////////////////////////////////////////////////
// DO NOT INCLUDE DIRECTLY! File gets included by other header file!
///////////////////////////////////////////////////////////////////////////////

/**
 * Get all points in point cloud in an stl vector
 * \return reference to vector of svt_vector4 objects
 */
template<class T>
inline vector< T >& svt_point_cloud<T>::getPoints()
{
    return m_aPoints[m_iTimestep];
};

/**
 * Add a point to point cloud.
 * \param rVec svt_vector4 object
 */
template<class T>
inline void svt_point_cloud<T>::addPoint( T& rVec )
{
    m_aPoints[m_iTimestep].push_back( rVec );
};

/**
 * Add a point to point cloud.
 * \param fX x coord
 * \param fY y coord
 * \param fZ z coord
 */
template<class T>
inline void svt_point_cloud<T>::addPoint( Real64 fX, Real64 fY, Real64 fZ )
{
    T oVec;
    oVec.x( fX );
    oVec.y( fY );
    oVec.z( fZ );
    m_aPoints[m_iTimestep].push_back( oVec );
};

/**
 * Get a point out of point cloud.
 * \param iIndex index of point
 * \return reference to svt_vector4 object
 */
template<class T>
inline T& svt_point_cloud<T>::getPoint( unsigned int iIndex )
{
    return m_aPoints[m_iTimestep][iIndex];
};

/**
 * Replace point in point cloud.
 * \param iIndex index of point
 * \param rVec svt_vector4 object
 */
template<class T>
inline void svt_point_cloud<T>::setPoint( unsigned int iIndex, T& rVec)
{
    m_aPoints[m_iTimestep][iIndex] = rVec;
};

/**
* Delete point in point cloud.
* \param iIndex index of point
*/
template<class T>
inline void svt_point_cloud<T>::delPoint( unsigned int iIndex)
{
    m_aPoints[m_iTimestep].erase(m_aPoints[m_iTimestep].begin()+iIndex);
};
    
    
/**
 * Delete all points in point cloud
 */
template<class T>
inline void svt_point_cloud<T>::delAllPoints()
{
    m_aPoints[m_iTimestep].clear();
};

/**
 * Size of point cloud
 * \return size of pc
 */
template<class T>
inline unsigned int svt_point_cloud<T>::size() const
{
    return m_aPoints[m_iTimestep].size();
};

/**
 * Dereference operator (not range checked!).
 * \param iIndex index of point in point cloud
 */
template<class T>
inline T& svt_point_cloud<T>::operator[](unsigned int iIndex)
{
    return m_aPoints[m_iTimestep][iIndex];
};

/**
 * Calculate the geometric center of the point cloud.
 * \return svt_vector4 with the geometric center
 */
template<class T> 
T svt_point_cloud<T>::geometricCenter()
{
    unsigned int iNum = size();
    unsigned int i=0;
    T oMinVec(  1.0E10,  1.0E10,  1.0E10 );
    T oMaxVec( -1.0E10, -1.0E10, -1.0E10 );

    for(i=0; i<iNum; i++)
    {
	if ((*this)[i].x() < oMinVec.x())
            oMinVec.x( (*this)[i].x() );
	if ((*this)[i].y() < oMinVec.y())
            oMinVec.y( (*this)[i].y() );
	if ((*this)[i].z() < oMinVec.z())
            oMinVec.z( (*this)[i].z() );

	if ((*this)[i].x() > oMaxVec.x())
            oMaxVec.x( (*this)[i].x() );
	if ((*this)[i].y() > oMaxVec.y())
            oMaxVec.y( (*this)[i].y() );
	if ((*this)[i].z() > oMaxVec.z())
            oMaxVec.z( (*this)[i].z() );
    }

    T oTrans = ( oMaxVec - oMinVec ) * 0.5;
    oTrans = oTrans + oMinVec;

    return oTrans;
};

/**
 * Calculate center of atoms (COM with all masses = 1).
 * \return svt_vector4 with the COA
 */
template<class T> 
T svt_point_cloud<T>::coa()
{
    unsigned int iNum = size();
    unsigned int i=0;
    T oCOAVec(  0.0,  0.0,  0.0 );

    for(i=0; i<iNum; i++)
	oCOAVec =  oCOAVec + (*this)[i];

    oCOAVec /= (double) iNum;

    return oCOAVec;
};

/**
 * calculate RMSD between this and another PC. The PCs must be matched already! To get the minimal RMSD please use align() first!
 * \param rPC second point cloud
 */
template<class T> 
Real64 svt_point_cloud<T>::rmsd( svt_point_cloud<T>& rPC )
{
    if (size() != rPC.size())
    {
	svtout << "Cannot calculate rmsd - the point_clouds have different size!" << endl;
        exit(1);
    }

    unsigned int iNumA = size();
    unsigned int i;

    Real64 fRMSD = 0.0;
    Real64 fDist;

    for(i=0; i<iNumA; i++)
    {
	fDist = (*this)[i].distanceSq( rPC[i] );

	fRMSD += fDist;
    }

    fRMSD *= 1.0 / (Real64)(iNumA);
    fRMSD = sqrt( fRMSD );

    return fRMSD;
};

/**
 * calculate RMSD between this and another PC. The points are matched using the nearest neighbor relationship.
 * \param rPC second point cloud
 */
template<class T> 
Real64 svt_point_cloud<T>::rmsd_NN( svt_point_cloud<T>& rPC )
{
    unsigned int iNumA = size();
    unsigned int i;

    Real64 fRMSD = 0.0;
    Real64 fDist;
    unsigned int iNeighbor;

    for(i=0; i<iNumA; i++)
    {
        iNeighbor = rPC.nearestNeighbor( (*this)[i] );
	fDist = (*this)[i].distanceSq( rPC[iNeighbor] );

	fRMSD += fDist;
    }

    fRMSD *= 1.0 / (Real64)(iNumA);
    fRMSD = sqrt( fRMSD );

    return fRMSD;
};

/**
 * Calculate RMSD between this and another PC. The points are matched using the nearest neighbor relationship.
 * All the nearest neighbor distances are calculated and then sorted (slow!) and only the first N-percent are used for the matching and the following RMSD calculation.
 * The idea is that approximately N-percent of the points are outliers which would increase the RMSD significantly, although the overall deviation of the two point clouds is
 * actually small.
 * \param rPC second point cloud
 * \param fPercent percentage of neighbors that should be used for the rmsd calculation (value between 0 and 1!)
 */
template<class T> 
Real64 svt_point_cloud<T>::rmsd_NN_Outliers( svt_point_cloud<T>& rPC, Real64 fPercent, svt_matrix4<Real64>* pMatrix )
{
    unsigned int iNumA = size();
    unsigned int i;

    Real64 fRMSD = 0.0;
    Real64 fDist;
    unsigned int iNeighbor;
    vector< svt_neighbor<Real64> >* pNeighbors = new vector< svt_neighbor<Real64> >;
    svt_point_cloud<T> oTmp;

    if (pMatrix != NULL)
	oTmp = (*pMatrix) * (*this);
    else
        oTmp = (*this);

    for(i=0; i<iNumA; i++)
    {
        iNeighbor = rPC.nearestNeighbor( oTmp[i] );
	fDist = oTmp[i].distanceSq( rPC[iNeighbor] );

        pNeighbors->push_back( svt_neighbor<Real64>( fDist, i, iNeighbor ) );
    }

    sort( pNeighbors->begin(), pNeighbors->end() );

    unsigned int iPercent = (unsigned int)((Real64)(iNumA) * fPercent);

    vector<int> aModelMatch;
    vector<int> aSceneMatch;

    for(i=0; i<iPercent; i++)
    {
	fRMSD += (*pNeighbors)[i].getScore();

	aModelMatch.push_back( (*pNeighbors)[i].getIndexA() );
	aSceneMatch.push_back( (*pNeighbors)[i].getIndexB() );
    }

    fRMSD /= (Real64)(iPercent);
    fRMSD = sqrt( fRMSD );

    if (pMatrix != NULL)
    {
        svt_matrix4<Real64> oOrig = (*pMatrix);
        svt_matrix4<Real64> oMat;
	oMat = oTmp.kearsley( aModelMatch, aSceneMatch, oTmp, rPC );
	(*pMatrix) = oOrig * oMat;
    }

    delete pNeighbors;

    return fRMSD;
}


/**
 * calculate hausdorff distance between this and another PC. The PCs must be matched and aligned already!
 * \param rPC second point cloud
 */
template<class T> 
Real64 svt_point_cloud<T>::hausdorff( svt_point_cloud<T>& rPC )
{
    unsigned int iNumA = size();
    unsigned int i;

    Real64 fDist;
    Real64 fHD = 0.0;
    unsigned int iNeighbor;

    for(i=0; i<iNumA; i++)
    {
        iNeighbor = rPC.nearestNeighbor( (*this)[i] );
	fDist = (*this)[i].distance( rPC[iNeighbor] );

	if (fDist > fHD)
	    fHD = fDist;
    }

    return fHD;
};

/**
 * find the nearest neighbor to a query point in the point cloud
 * \param rVec reference to svt_vector4 object - the query point
 * \return index to nearest point in point cloud
 */
template<class T>
unsigned int svt_point_cloud<T>::nearestNeighbor( T& rVec )
{
    unsigned int iNumA = size();

    Real64 fMinDist = 1.0E10;
    unsigned int iMinDist = 0;
    Real64 fDist;

    for(unsigned int i=0; i<iNumA; i++)
    {
	fDist = (*this)[i].distance( rVec );

	if (fDist < fMinDist)
	{
            iMinDist = i;
	    fMinDist = fDist;
	}
    }

    return iMinDist;
};

/**
 * calculate the average nearest neighbor distance
 * in order to reduce the complexity a random test is done and once the average stabilizes, the search is stopped.
 * \param fPrecision if average does not change by more than fPrecision between two iterations the calculation is stopped
 * \return average nearest neighbor distance
 */
template<class T> 
Real64 svt_point_cloud<T>::averageNNDistance(Real64 fPrecision )
{
    unsigned int iNum = size();
    Real64 fNum = (Real64)(iNum);
    unsigned int iTest = 0;
    Real64 fCount = 0.0;
    Real64 fAvg = 0.0;
    Real64 fAvgDiv = 0.0;
    Real64 fAvgDivOld = 1.0E10;
    Real64 fMinDist;
    unsigned int iMinDist;
    Real64 fDist;
    unsigned int iOldTest = 0;

    if (fNum > 0)
    {
        // loop until desired precision is reached...
        while( fabs(fAvgDiv - fAvgDivOld) > fPrecision && fCount < 1000000.0 )
        {
            while (iTest == iOldTest)
                iTest = (unsigned int)(svt_genrand()*fNum);
            iOldTest = iTest;

            // determine nearest neighbor
            fMinDist = 1.0E10;
            iMinDist = 0;

            for(unsigned int i=0; i<iNum; i++)
            {
                fDist = (*this)[i].distance( (*this)[iTest] );

                if (fDist < fMinDist && fDist != 0.0)
                {
                    iMinDist = i;
                    fMinDist = fDist;
                }
            }

            // calculate average
            fAvg += fMinDist;
            fAvgDivOld = fAvgDiv;
            fAvgDiv = fAvg / fCount;
            fCount += 1.0;
        }
    }

    return fAvgDiv;
};

/**
 * calculate the maximum nearest neighbor distance
 * \return maximum nearest neighbor distance
 */
template<class T> 
Real64 svt_point_cloud<T>::maxNNDistance( )
{
    unsigned int iNum = size();
    Real64 fMinDist;
    Real64 fMaxDist = 0.0;
    Real64 fDist;

    // loop until desired precision is reached...
    for (unsigned int i=0; i<iNum; i++)
    {
        // determine nearest neighbor
	fMinDist = 1.0E10;
        
	for(unsigned int j=0; j<iNum; j++)
	{
	    fDist = (*this)[i].distance( (*this)[j] );

	    if (fDist < fMinDist && fDist != 0.0)
	       fMinDist = fDist;
	}
	
        // is this the maximum?
	if (fMinDist > fMaxDist)
	    fMaxDist = fMinDist;
    }

    return fMaxDist;
};

/**
 * Get the minimal coordinates of the point cloud - it will return a vector that has in each dimension the information about the minimal
 * coordinate it has found in the cloud.
 */
template<class T> 
T svt_point_cloud<T>::getMinCoord()
{
    Real64 fMinX = 1.0E10;
    Real64 fMinY = 1.0E10;
    Real64 fMinZ = 1.0E10;

    unsigned int i;

    for(i=0; i<size(); i++)
    {
	if (getPoint(i).x() < fMinX)
            fMinX = getPoint(i).x();
	if (getPoint(i).y() < fMinY)
            fMinY = getPoint(i).y();
	if (getPoint(i).z() < fMinZ)
	    fMinZ = getPoint(i).z();
    }

    T oVec;
    oVec.x( fMinX );
    oVec.y( fMinY );
    oVec.z( fMinZ );

    return oVec;
};

/**
 * Get the maximal coordinates of the point cloud - it will return a vector that has in each dimension the information about the maximal
 * coordinate it has found in the cloud.
 */
template<class T> 
T svt_point_cloud<T>::getMaxCoord()
{
    Real64 fMaxX = -1.0E10;
    Real64 fMaxY = -1.0E10;
    Real64 fMaxZ = -1.0E10;

    unsigned int i;

    for(i=0; i<size(); i++)
    {
	if (getPoint(i).x() > fMaxX)
            fMaxX = getPoint(i).x();
	if (getPoint(i).y() > fMaxY)
            fMaxY = getPoint(i).y();
	if (getPoint(i).z() > fMaxZ)
	    fMaxZ = getPoint(i).z();
    }

    T oVec;
    oVec.x( fMaxX );
    oVec.y( fMaxY );
    oVec.z( fMaxZ );

    return oVec;
};

/**
 * product of matrix and point cloud
 */
template<class T> 
inline svt_point_cloud<T> operator*(const svt_matrix4<Real64>& rM, svt_point_cloud<T>& rPC)
{
    svt_point_cloud<T> oPC;
    T oP;

    for(unsigned int i=0; i<rPC.size(); i++)
    {
        oP = rM * rPC[i];
	oPC.addPoint( oP );
    }

    return oPC;
};

/**
 * sample the object randomly and return a vector that refrects the probability distribution of the object
 */
template<class T>
T svt_point_cloud<T>::sample( )
{
    unsigned int iIndex = (unsigned int)(svt_genrand() * (double)(size()));

    return this->getPoint( iIndex );
};

/**
 * Write pdb file.
 * \param pFilename pointer to array of char with the filename
 * \param bAppend if true, the pdb structure as append at the end of an existing structure file
 */
template<class T>
void svt_point_cloud<T>::writePDB(const char* pFilename, bool bAppend)
{
    unsigned int i;
    unsigned int iNum = size();
    FILE* pFile = NULL;

    if (!bAppend)
	pFile = fopen(pFilename, "w");
    else {
	pFile = fopen(pFilename, "a");
        fprintf( pFile, "END\n" );
    }

    int iTS;
    for(iTS = 0; iTS < this->getMaxTimestep(); iTS++)
    {
	if (this->getMaxTimestep() > 0)
            svtout << "  Writing frame " << iTS << endl;

        this->setTimestep( iTS );

	for(i=0; i<iNum; i++)
            fprintf(
                    pFile, "ATOM  %5i %2s%c%c%c%3s %c%4i%c  %8.*f%8.*f%8.*f%6.2f%6.2f %3s  %4s%2s%2s\n",
                    i+1,
                    "QP",
                    'D',
                    'B',
                    ' ',
                    "QPDB",
                    ' ',
                    0,
                    'Q',
                    CoordPrecisionPDB(this->getPoint(i).x()), this->getPoint(i).x(),
                    CoordPrecisionPDB(this->getPoint(i).y()), this->getPoint(i).y(),
                    CoordPrecisionPDB(this->getPoint(i).z()), this->getPoint(i).z(),
                    0.0,
                    0.0,
                    "QPDB",
                    "QPDB",
                    "QPDB",
                    "QPDB"
                   );

        if (this->getMaxTimestep() > 0)
            fprintf( pFile, "END\n");
    }

    if (this->getMaxTimestep() > 0)
        this->setTimestep( 0 );

    fclose(pFile);

};

/**
 * Write CSV file - comma separated values x,y,z. It will write only the coordinates of the current frame.
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
void svt_point_cloud<T>::writeCSV(const char* pFilename)
{
    unsigned int i;
    unsigned int iNum = size();
    FILE* pFile = fopen(pFilename, "w");

    for(i=0; i<iNum; i++)
        fprintf(
                pFile, "%8.3f,%8.3f,%8.3f\n",
                this->getPoint(i).x(),
                this->getPoint(i).y(),
                this->getPoint(i).z()
               );

    fclose(pFile);
};

/**
 * Load a pdb file.
 * \param pointer to array of char with the filename
 */
template<class T>
void svt_point_cloud<T>::loadPDB(const char* pFilename)
{
    // open file
    svt_column_reader oReader( pFilename );
    char* pBuffer;

    // clear all old points
    delAllPoints();

    if (!oReader.eof())
	while(1)
	{
	    // read until next atom record reached
	    while(1)
	    {
		if (oReader.readLine() == false)
		    return;

		pBuffer = oReader.extractString(0,5);

		//is it an ATOM record?
		if (pBuffer[0] == 'A' && pBuffer[1] == 'T' &&  pBuffer[2] == 'O' &&  pBuffer[3] == 'M')
		    break;
		// is it an HETATM record?
		if (pBuffer[0] == 'H' && pBuffer[1] == 'E' &&  pBuffer[2] == 'T' &&  pBuffer[3] == 'A' &&  pBuffer[4] == 'T'&&  pBuffer[5] == 'M')
		    break;
	    }

	    //now parse the coordinates
	    double fX = oReader.extractReal32( 30, 37 );
	    double fY = oReader.extractReal32( 38, 45 );
	    double fZ = oReader.extractReal32( 46, 53 );

	    this->addPoint( fX, fY, fZ );
	}
};

/**
 * set the current timestep
 * \param iTimestep the new timestep
 */
template<class T>
void svt_point_cloud<T>::setTimestep( unsigned int iTimestep )
{
    if ( iTimestep < m_aPoints.size() )
	m_iTimestep = iTimestep;
};

/**
 * get the current timestep
 * \return the timestep
 */
template<class T>
int svt_point_cloud<T>::getTimestep()
{
    return m_iTimestep;
};

/**
 * get the maximum timestep
 * \return the maximum timestep
 */
template<class T>
int svt_point_cloud<T>::getMaxTimestep()
{
    return m_aPoints.size();
};

/**
 * Add a new timestep
 */
template<class T>
void svt_point_cloud<T>::addTimestep()
{
    m_aPoints.push_back( m_aPoints[ m_aPoints.size() -1 ] );
    m_iTimestep = m_aPoints.size()-1;
};

/**
 * Compute the distance matrix between all points in the pointcloud
 * \return svt_matrix object with the distances
 */
template<class T>
svt_matrix<Real64> svt_point_cloud<T>::getDistanceMat()
{
    unsigned int iSize = size();
    svt_matrix<Real64> oDist(iSize, iSize);
    Real64 fDist;
    for (unsigned int iIndex1=0; iIndex1 < iSize; iIndex1++)
    {
        for (unsigned int iIndex2=0; iIndex2 < iSize; iIndex2++)
        {
            fDist = (*this)[iIndex1].distance( (*this)[iIndex2] );
            oDist[iIndex1][iIndex2] = fDist;
        }
    }

    return oDist;
}

/**
* Calculate the Pair Distribution Function
* \param fWidth is width of a bin (held constant between the range Dmin and Dmax)
*/
template<class T> void svt_point_cloud<T>::calcPairDistribution(Real64 fWidth, bool bShowProgress)
{
    m_oPDF.setWidth( fWidth );
    m_oPDF.setMin( 0.0f );
    
    Real64 fDist;
    
    //FIXME here don't use function to addDistances cause function calls are expensive; 
    //but compute the bin and create vector here
    try
    {
        unsigned int iCount=0;
        if (bShowProgress)
            svt_exception::ui()->progressPopup("Compute PDF", 0, (size()-1)*((size()-1))/2 );
        
        unsigned int iSize = size();
        svt_matrix<Real64> oDist(iSize, iSize);
        for (unsigned int iIndex1=0; iIndex1 < iSize; iIndex1++)
        {
            for (unsigned int iIndex2=iIndex1+1; iIndex2 < iSize; iIndex2++)
            {
                iCount++;
                
                fDist =(*this)[iIndex1].distance( (*this)[iIndex2] );
                oDist[iIndex1][iIndex2] = fDist;

                if ((bShowProgress) && (iCount % 1000000 == 0))
                    svt_exception::ui()->progress( iCount, (size()-1)*((size()-1))/2 );
            
            }
        }
        
        m_oPDF.setDistanceMatrix( oDist );
                
    } 
    catch (svt_userInterrupt&)
    {
        m_oPDF.clear();
    }
    
    if (bShowProgress)
        svt_exception::ui()->progressPopdown();
};

/**
* Calculate the Pair Distribution Function - old code
* \param fWidth is width of a bin (held constant between the range Dmin and Dmax)
*/
template<class T> void svt_point_cloud<T>::calcPairDistribution2(Real64 fWidth, bool bShowProgress)
{
    m_oPDF.setWidth( fWidth );
    m_oPDF.setMin( 0.0f );
    
    Real64 fDist;
    
    //FIXME here don't use function to addDistances cause function calls are expensive; 
    //but compute the bin and create vector here
    try
    {
        unsigned int iCount=0;
        if (bShowProgress)
            svt_exception::ui()->progressPopup("Compute PDF", 0, (size()-1)*((size()-1))/2 );
        
        unsigned int iSize = size();
        for (unsigned int iIndex1=0; iIndex1 < iSize; iIndex1++)
        {
            for (unsigned int iIndex2=iIndex1+1; iIndex2 < iSize; iIndex2++)
            {
                iCount++;
                
                fDist =(*this)[iIndex1].distance( (*this)[iIndex2] );
                
                m_oPDF.addPairDistance( fDist );
                
                if ((bShowProgress) && (iCount % 1000000 == 0))
                    svt_exception::ui()->progress( iCount, (size()-1)*((size()-1))/2 );
            
            }
        }
    } 
    catch (svt_userInterrupt&)
    {
        m_oPDF.clear();
    }
    
    if (bShowProgress)
        svt_exception::ui()->progressPopdown();
};

/**
 * return the pairDistribution
 */
template<class T> inline svt_pair_distribution svt_point_cloud<T>::getPairDistribution()
{
    return m_oPDF;
};


/**
 * set a  pairDistribution
 */
template<class T> inline void svt_point_cloud<T>::setPairDistribution(svt_pair_distribution & oPDF)
{
    m_oPDF = oPDF;
};

/**
 * Normalize pairDistribution
 */
template<class T>
void svt_point_cloud<T>::normalizePairDistribution()
{
    m_oPDF.normalize();
};

