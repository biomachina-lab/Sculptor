#include <svt_tube.h>
#include <svt_powell_cc.h>

///////////////////////////////////////////////////////////////////////////////
// svt_tube class
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_tube::svt_tube():
m_fAvgFitness(0.0),
m_fSumFitness (0.0), 
m_iSize(0),
m_fLength(0),
m_fMapScore(0.0),
m_bWasTubeComputed( false ),
m_bWasFineTubeComputed( false ),
m_fPenalty(0.0),
m_bRecomputeScore (true)
{
};

/**
 * add a new element to tube
 * \param oElem a svt_gacylinder_ind that in considered in this tube
 */
void svt_tube::add2Tube( svt_gacylinder_ind oElem, bool bFlip)
{
    if (m_iSize==0)
        m_oFirstAddedElem =  oElem.getTrans().translation();

    if (bFlip)
    {
        vector<svt_gacylinder_ind> oElements;
        while( m_oElements.size() > 0)
        {
            oElements.push_back( m_oElements[m_oElements.size()-1] );
            m_oElements.pop_back();
        }
        m_oElements = oElements;
    }

    m_oElements.push_back( oElem );
    m_iSize++;
    m_bWasTubeComputed = false;
    m_bWasFineTubeComputed = false;
    
    m_fSumFitness += oElem.getFitness();  
    if (m_iSize>=2)
        m_fAvgFitness = (m_fSumFitness / (Real64)m_iSize);// * sqrt(m_iSize);
    else
        m_fAvgFitness = 0.0f; // do not consider yet

};

/**
 * remove the last element of the tube
 */
void svt_tube::pop( )
{
   if (m_oElements.size()>0)
    { 
        m_fSumFitness -= m_oElements[m_oElements.size()-1].getFitness();    
        m_oElements.pop_back();
        m_iSize--;

        if (m_iSize>=2)
            m_fAvgFitness = m_fSumFitness / (Real64)m_iSize;
        else
            m_fAvgFitness = 0.0f; // do not consider yet
    }
 
};

/**
 * get the list of individuals added to the tube
 * \return the individuals
 */ 
vector<svt_gacylinder_ind > svt_tube::getElements()
{
    return m_oElements;
};

/**
 * set the score
 * \param fScore the score assigned
 */ 
void svt_tube::setMapScore( Real64 fScore)
{
    m_fMapScore = fScore;
};

/**
 * get the score
 * \return the score of the tube
 */ 
Real64 svt_tube::getMapScore()
{
    return m_fMapScore;
};

/**
 * set the score
 * \param fScore the score assigned
 */ 
void svt_tube::setAvgScore( Real64 fScore)
{
    m_fAvgFitness = fScore;
};

/**
 * get the score
 * \return the score of the tube
 */ 
Real64 svt_tube::getAvgScore()
{
    return m_fAvgFitness;
};

/**
 * set the score 
 * \param iIndex - which element in vector score
 * \param fScore - the value
 */
void svt_tube::setScore(int iIndex, Real64 fScore)
{
    while (iIndex>=(int)m_oScores.size())
        m_oScores.push_back(0.0f);

    m_oScores[iIndex] = fScore;
};

/**
 * returns the list of scores
 * \param a vector with the different scores
 */
vector<Real64> svt_tube::getScores()
{
    return m_oScores;
};

/**
 * Compute avg score
 */ 
void svt_tube::computeAvgScore()
{
    m_fSumFitness = 0.0;
    //discard the first two elements and the last two elements as they are the steps that failed
    for (int iIndex=0; iIndex < (int)m_oElements.size(); iIndex++)
    {
        m_fSumFitness += m_oElements[iIndex].getFitness();
    }
    
    if ((int)m_oElements.size()>0)
        m_fAvgFitness = m_fSumFitness/(Real64)m_oElements.size();
    else
        m_fAvgFitness = 0.0;
};

/**
* get the number of points
* \return the number of points
*/ 
unsigned int svt_tube::size()
{
	return m_iSize;
}; 


/**
* get the number of turns
* \return the number of turns
*/ 
Real64 svt_tube::getTurns(Real64 fRatio)
{
 	if (fRatio != 0)
		return m_iSize/(Real64)(fRatio); 
	else
	{
		svtout << "Could not compute the number of turns: fRatio = 0 " << endl;
		return  (Real64)m_iSize;
	}
}; 

/**
 * Compute the length in A0
 */
void svt_tube::computeLength()
{
	//compute points on axis if not yet available
	if (!m_bWasTubeComputed)
		getTube();
	
	m_fLength = 0.0f;
	for (unsigned int i=1; i<m_oTube.size(); i++)
		m_fLength += m_oTube[i-1].distance(  m_oTube[i] );
};

/**
* get the length = sum distances between points
* assumes that the points are in order = point 0 is closest 1 and point 2 follows point 1... etc
* \return the length of the tube 
*/ 
Real64 svt_tube::getLength()
{
	if (m_fLength == 0.0 || !m_bWasTubeComputed)
		computeLength();

	return m_fLength;
}; 

/**
 * get the first element
 */  
svt_vector4<Real64> svt_tube::getFirstAddedElem()
{
    return m_oFirstAddedElem;
};

/**
 * set penalty
 * \param the penalty
 */ 
void svt_tube::setPenalty(Real64 fPenalty)
{
    m_fPenalty = fPenalty;
};

/**
 * increase penalty
 * \param the penalty
 */ 
void svt_tube::addPenalty( Real64 fPenalty)
{
    m_fPenalty += fPenalty;
};

/**
 * get penalty
 * \return the penalty
 */ 
Real64 svt_tube::getPenalty()
{
    return m_fPenalty;
};

/**
 * get curvature
 */
Real64 svt_tube::getCurvature()
{
    return m_fCurvature;
}

/**
 * overload < operator
 * \param that another svt_tube element
 */ 
bool svt_tube::operator<(const svt_tube& that) const
{
    return m_fAvgFitness < that.m_fAvgFitness;
};

/**
 * overload < operator using the max density of the map
 * \param that another svt_tube element
 */ 
bool svt_tube::lt_mapScore(svt_tube first, svt_tube second) 
{
    return first.m_fMapScore < second.m_fMapScore;
};

/**
 * overload < operator using the max density of the map
 * \param that another svt_tube element
 */ 
bool svt_tube::lt_length(svt_tube first, svt_tube second) 
{
    return first.m_iSize < second.m_iSize;
};


/**
 * overload < operator using the max density of the map
 * \param that another svt_tube element
 */ 
bool svt_tube::lt_wPenatly(svt_tube first, svt_tube second) 
{
    return first.m_fAvgFitness - first.getPenalty() < second.m_fAvgFitness - second.getPenalty();
};

/**
 * overload < operator using the max density of the map
 * \param that another svt_tube element
 */ 
bool  svt_tube::lt_score(svt_tube first, svt_tube second)
{
 if ((int)(first.m_oScores.size())>2 && (int)(second.m_oScores.size())>2)
        return first.m_oScores[2]< second.m_oScores[2];
    else
        return true;
};   



/**
 * get tube as
 * \return the pdb tube
 */
svt_point_cloud_pdb<svt_ga_vec> svt_tube::getTube(svt_point_cloud_pdb<svt_ga_vec> *pTemplate, bool bFine) 
{
    //svtout << m_bWasTubeComputed << " " << m_bWasFineTubeComputed  << " " << bFine << endl;

    //was already computed once just return dont recompute
    if (m_bWasTubeComputed && pTemplate==NULL && !bFine)
        return m_oTube;

    // fine pdb was already computed once just return dont recompute
    if (m_bWasFineTubeComputed && pTemplate==NULL && bFine)
        return m_oFineTube;

    svt_point_cloud_pdb<svt_ga_vec> oPdb;
    svt_point_cloud_atom oAtom;
    oAtom.setName(" C");
    oAtom.setRemoteness('A');
    oAtom.setResname("GLY");
    svt_ga_vec oVec, oNull;
    svt_point_cloud_pdb< svt_ga_vec> oTemplatePdb;
    oNull.x(0.0); oNull.y(0.0);oNull.z(0.0);
    svt_ga_mat oMat;

    for (unsigned int iIndex=0; iIndex<m_oElements.size(); iIndex++)
    {
        oMat = m_oElements[iIndex].getTrans();

        if (pTemplate == NULL)
        {
            oAtom.setResidueSeq(oPdb.size());
            oVec = oMat*oNull;
            oPdb.addAtom(oAtom, oVec);
        }
        else
        {
            oTemplatePdb = oMat*(*pTemplate);
            oPdb.append(oTemplatePdb);
        }
    }
    m_oTube = oPdb;
    m_bWasTubeComputed = true;
 
    if (pTemplate == NULL)
    {
        if (bFine  && !m_bWasFineTubeComputed) // not yet computed 
         {
            Real64 fNewX, fNewY, fNewZ, fM, fDist;
            int iCountIntermediates, iCount = 0;
            svt_vector4<Real64> oVec1, oVec2, oVec;

            for (unsigned int iIndex = 0; iIndex < oPdb.size()-1; iIndex++)
            {
              iCount++;
              oAtom.setResidueSeq(iCount);
              oAtom.setOrdResidueSeq(iCount);
              oAtom.setPDBIndex( iCount );
              m_oFineTube.addAtom(oAtom, oPdb[iIndex]);

              fDist = oPdb[iIndex].distance(oPdb[iIndex+1]);
              if (fDist > 6.0) // should create intermediates
              {
                  iCountIntermediates = floor(fDist/4.0); //how many points should have beween 
                  for (int iPoint = 1; iPoint < iCountIntermediates; iPoint++)
                  {
                      // cout << iIndex << ":" << iPoint << endl; 
                      oVec1 = oPdb[iIndex];
                      oVec2 = oPdb[iIndex+1];

                      fM = 1.0/iCountIntermediates*iPoint;
                      fNewX =  oVec1.x() + fM*(oVec2.x()-oVec1.x());
                      fNewY =  oVec1.y() + fM*(oVec2.y()-oVec1.y());
                      fNewZ =  oVec1.z() + fM*(oVec2.z()-oVec1.z());
                        
                      oVec.x(fNewX); 
                      oVec.y(fNewY); 
                      oVec.z(fNewZ);
                      iCount++;
                      oAtom.setResidueSeq(iCount);
                      oAtom.setResidueSeq(iCount);
                      oAtom.setOrdResidueSeq(iCount);
                      oAtom.setPDBIndex( iCount );
                      m_oFineTube.addAtom(oAtom, oVec);
                  } 
              }
            }
            iCount++;
            oAtom.setResidueSeq(iCount);
            oAtom.setOrdResidueSeq(iCount);
            oAtom.setPDBIndex( iCount );
            m_oFineTube.addAtom(oAtom, oPdb[oPdb.size()-1]);
            m_bWasFineTubeComputed = true;
        }
    }

    //compute the lenght in A
    computeLength();

    if (!bFine)
      return m_oTube;
   else
      return m_oFineTube;
};

/**
 * get the direction of the tube
 */ 
svt_ga_vec svt_tube::getDirection()
{
    m_oDirection.x( 0.0f ); 
    m_oDirection.y( 0.0f ); 
    m_oDirection.z( 0.0f );
    
    svt_ga_vec oNull, oPrev, oVec;
    oNull.x(0.0); oNull.y(0.0);oNull.z(0.0);
    svt_ga_mat oMat;

    if (m_oElements.size()>0)
    {
        oMat = m_oElements[0].getTrans();
        oPrev = oMat*oNull;
    }

    unsigned int iCount = 0;
    for (unsigned int iIndex=1; iIndex<m_oElements.size(); iIndex++)
    {
        iCount++;
        oMat = m_oElements[iIndex].getTrans();
        oVec = oMat*oNull;

        m_oDirection += (oVec - oPrev);
    
        oPrev = oVec;
    }

    if (iCount != 0)
    {
        m_oDirection *= (1.0/(Real64(iCount)));
        m_oDirection.normalize();
    }

    return m_oDirection;
};

/**
 * print the tube
 */  
void svt_tube::print()
{
    svtout << "Tube of " << m_iSize << "  with avg fitness of " << m_fAvgFitness << endl; 

    for (unsigned int iIndex=0; iIndex < m_oElements.size(); iIndex++)
    {
        svtout;
        m_oElements[iIndex].printGenes();
    }
};

/**
 * compute the volume underneath the tube
 */
void svt_tube::fillExplored(svt_ga_vol & rVol, svt_ga_vol *pVol)
{
    svt_point_cloud_pdb< svt_vector4<Real64> > oPdb;
    oPdb = getTube();

/*
    svt_ga_vol oExtractedVol;
    oExtractedVol = rVol;
    oExtractedVol.setValue(0.0);

    svt_ga_mat oMat;

    oPdb.projectMass( &oExtractedVol,oMat, false );
*/
    svt_ga_vec oVecVol;
    Real64 fWidth = rVol.getWidth();
    Real64 fGridX = rVol.getGridX();
    Real64 fGridY = rVol.getGridY();
    Real64 fGridZ = rVol.getGridZ();
    Real64 fDist, fDensity;
    
    for (unsigned int iX=0; iX<rVol.getSizeX() - 1; iX++)
    {
        oVecVol.x( fGridX+iX*fWidth + fWidth/2.0 );
        for (unsigned int iY=0; iY<rVol.getSizeY() - 1; iY++)
        {
            oVecVol.y( fGridY+iY*fWidth  + fWidth/2.0 );
            for (unsigned int iZ=0; iZ<rVol.getSizeZ() - 1; iZ++)
            {
                oVecVol.z( fGridZ+iZ*fWidth  + fWidth/2.0 );
                for (unsigned int iIndex=0; iIndex< oPdb.size(); iIndex++)
                {
                    fDist = oPdb[iIndex].distance( oVecVol );
                    if (fDist <= 6.0/2.0 )
                    {
                        fDensity = rVol.at(iX, iY, iZ) ;
                        pVol->setAt(iX, iY, iZ, fDensity );
                        //oExtractedVol.setAt(iX, iY, iZ, fDensity );
                    }
                }
            }
        }
    }

    //create blurring kernel
//    svt_ga_vol oKernel;
//    oKernel.create1DBlurringKernel(1.0, 5.0);
 //   oExtractedVol.convolve1D3D(oKernel, false);
     
    //get the matrix
  //  m_fMapScore =  oExtractedVol.correlation(rVol);

};

/**
 *get the high resolution version of the tube
 */ 
svt_point_cloud_pdb<svt_ga_vec> svt_tube::getHRTube()
{
    return m_oHRTube;
};

/**
 * creates a highresolution version of the helix
 */
void svt_tube::createHighResTube( svt_ga_vol & rVol, svt_point_cloud_pdb<svt_ga_vec> *pTemplate )
{
    svt_point_cloud_pdb< svt_ga_vec > oHelix, oBestHelix;
    Real64 fBestCorr, fCorr;

    svt_ga_mat oMat, oConstRot;
    oConstRot.loadIdentity();

    int iStep = 5;
    Real64 fAngleStep = 1.5, fAngleMin, fAngleMax, fAngleBest;

    vector<svt_point_cloud_pdb <svt_ga_vec> > oPdbs;
    for (int iIndex = m_oElements.size()-1; iIndex >= 0; iIndex -= iStep)
    {
        // refine rotation
        if (iIndex == (int)m_oElements.size()-1)
        {
            fAngleMin = -180;
            fAngleMax = 180;
        }
        else
        {
            fAngleMin = -45;
            fAngleMax = 45;
        }

        oMat.loadIdentity(); 
        oMat.rotate(2, deg2rad(fAngleMin));
        
        fBestCorr = pTemplate->projectMassCorr(&rVol, m_oElements[iIndex].getTrans()*oConstRot*oMat, false );
        fAngleBest = fAngleMin;
        for (Real64 fAngle=fAngleMin+fAngleStep; fAngle < fAngleMax; fAngle += fAngleStep )
        {
            oMat.loadIdentity();
            oMat.rotate(2, deg2rad(fAngle));

            fCorr = pTemplate->projectMassCorr(&rVol, m_oElements[iIndex].getTrans()*oConstRot*oMat, false );
            if (fCorr > fBestCorr)
            {
                fBestCorr = fCorr;
                fAngleBest = fAngle;
            }
        }

        oMat.loadIdentity();
        oMat.rotate(2, deg2rad(fAngleBest));

        oPdbs.push_back(  m_oElements[iIndex].getTrans()*oConstRot*oMat*(*pTemplate) );
        oConstRot = oMat * oConstRot;
             
        oConstRot.rotate(2, deg2rad(130));
    }

    for (unsigned int iIndex = 0; iIndex < oPdbs.size(); iIndex ++)
    {
	oHelix = oPdbs[iIndex];
	if (iIndex==0)
	    m_oHRTube =  oHelix;
	else
	    m_oHRTube.append(oHelix);    
    }
}; 

/**
 * Estimate curvature of the axis
 */
void svt_tube::estimate_curvature()
{
    svt_point_cloud_pdb<svt_ga_vec> oPdb = getTube();
    svt_ga_vec oRef, oDiff; 

    m_fCurvature = 0.0;
    if (oPdb.size()>=3)
    {
        oRef = oPdb[1] - oPdb[0];

        for (unsigned int iIndex = 2; iIndex<oPdb.size(); iIndex++)
        {
            oDiff = oPdb[iIndex] - oPdb[iIndex-1]; 
            m_fCurvature += oRef.distance(oDiff);
            oRef = oDiff;
        }

        m_fCurvature /= (Real64)(oPdb.size()-2);
    } 
};

/**
 * discard the points that are at the ends of the tube is their score is < fScore
 */
void svt_tube::discardPointsAtEnds( Real64 fScore)
{
    print();
    
    //at the end
    while (m_oElements.size()>0 && m_oElements[m_oElements.size()-1].getFitness() < fScore )
        m_oElements.pop_back();

    //at the begining
    while (m_oElements.size()>0 && m_oElements[0].getFitness() < fScore )
        m_oElements.erase(m_oElements.begin());

    //recompute Scores
    m_iSize = m_oElements.size(); 
    computeAvgScore();

    print();
}; 

