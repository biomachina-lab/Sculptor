/***************************************************************************
                          svt_gaflex
                          ---------
    begin                : 06/23/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_gaflex.h>
#include <svt_matrix.h>

// streams
#include <iostream>
using namespace std;

/**
 * center probe and target
 */
void svt_gaflex::centerProbeTarget()
{
    unsigned int i=0;
            
    // first center probe
    svt_vector4<Real64> oMin( 1.0E10,  1.0E10,  1.0E10);
    svt_vector4<Real64> oMax(-1.0E10, -1.0E10, -1.0E10);
    for(i=0; i<m_oProbe.size(); i++)
    {
	if (m_oProbe[i].x() > oMax.x())
            oMax.x( m_oProbe[i].x() );
	if (m_oProbe[i].y() > oMax.y())
            oMax.y( m_oProbe[i].y() );
	if (m_oProbe[i].z() > oMax.z())
	    oMax.z( m_oProbe[i].z() );

	if (m_oProbe[i].x() < oMin.x())
            oMin.x( m_oProbe[i].x() );
	if (m_oProbe[i].y() < oMin.y())
            oMin.y( m_oProbe[i].y() );
	if (m_oProbe[i].z() < oMin.z())
	    oMin.z( m_oProbe[i].z() );
    }
    svt_vector4<Real64> oTrans = ( oMax - oMin ) * 0.5;
    oTrans = oTrans - oMax;
    for(i=0; i<m_oProbe.size(); i++)
    {
	m_oProbe[i].x( m_oProbe[i].x() + oTrans.x());
        m_oProbe[i].y( m_oProbe[i].y() + oTrans.y());
        m_oProbe[i].z( m_oProbe[i].z() + oTrans.z());
    }

    // now center target
    oMin = svt_vector4<Real64>( 1.0E10,  1.0E10,  1.0E10);
    oMax = svt_vector4<Real64>(-1.0E10, -1.0E10, -1.0E10);
    for(i=0; i<m_oTarget.size(); i++)
    {
	if (m_oTarget[i].x() > oMax.x())
            oMax.x( m_oTarget[i].x() );
	if (m_oTarget[i].y() > oMax.y())
            oMax.y( m_oTarget[i].y() );
	if (m_oTarget[i].z() > oMax.z())
	    oMax.z( m_oTarget[i].z() );

	if (m_oTarget[i].x() < oMin.x())
            oMin.x( m_oTarget[i].x() );
	if (m_oTarget[i].y() < oMin.y())
            oMin.y( m_oTarget[i].y() );
	if (m_oTarget[i].z() < oMin.z())
	    oMin.z( m_oTarget[i].z() );
    }
    oTrans = ( oMax - oMin ) * 0.5;
    oTrans = oTrans - oMax;
    for(i=0; i<m_oTarget.size(); i++)
    {
	m_oTarget[i].x( m_oTarget[i].x() + oTrans.x());
        m_oTarget[i].y( m_oTarget[i].y() + oTrans.y());
        m_oTarget[i].z( m_oTarget[i].z() + oTrans.z());
    }
}

/**
 * generate initial population
 * \param iNum number of inds in this population
 */
void svt_gaflex::initPopulation(int iNum)
{
    Real64 fDiscardValue = m_oProbe.maxNNDistance() + m_fConnStretch;
    calcDistMat(fDiscardValue);
    
    m_fSampl = 1.0f / 3.0f;

    svt_ga::initPopulation( iNum );
}

/**
 * update adaptive fitness
 */
void svt_gaflex::updateAdaptive()
{
    m_fSampl /= 3.0;
    
    if (m_fSampl < 0.0001)
    {
	m_fSampl = 0.0;
    }
    
    sortPopulation();
    calcCoordinates( &m_oPop[ m_oPop.size() - 1] );
    for (unsigned int iIndex=0; iIndex < m_oProbe.size(); iIndex++)
        m_oProbe[iIndex] = m_oFlexed[iIndex];
}


/**
 * calculate Coordinate
 * \param pInd pointer to individual
 * \param iIndex the index of the coordinate to be computed
 */
svt_vector4<Real64> svt_gaflex::calcCoordinate(svt_ga_ind* pInd, int iIndex)
{
    svt_vector4<Real64> oVec;
    
    oVec.x( pInd->getGene( iIndex*3+0 ) );
    oVec.y( pInd->getGene( iIndex*3+1 ) );
    oVec.z( pInd->getGene( iIndex*3+2 ) );
    
    if (m_fSampl != 0.0)
    {
        oVec.x( (int)( oVec.x() / m_fSampl ) * m_fSampl + (m_fSampl / 2.0) );
        oVec.y( (int)( oVec.y() / m_fSampl ) * m_fSampl + (m_fSampl / 2.0) );
        oVec.z( (int)( oVec.z() / m_fSampl ) * m_fSampl + (m_fSampl / 2.0) );
    }
    oVec -= 0.5;
    oVec.normalize();
    oVec *= m_oSearchSpace.x();
    oVec += m_oProbe[iIndex];
    
    return oVec;
}

/**
 * calculate Coordinates 
 * \param pInd pointer to individual
 */
void svt_gaflex::calcCoordinates(svt_ga_ind* pInd)
{
    svt_vector4<Real64> oVec;
    
    m_oFlexed.delAllPoints();
     for (unsigned int iIndex=0; iIndex < (unsigned int)pInd->getGeneCount()/3; iIndex++ )
    {
        oVec = calcCoordinate(pInd, iIndex);
        m_oFlexed.addPoint( oVec );
    }
}

/**
 * update fitness
 * \param pInd pointer to individual that gets updated
 */
void svt_gaflex::updateFitness(svt_ga_ind* pInd)
{
    double fFitness = 0.0;
    
    calcCoordinates( pInd );
    
    //should the validity of the solution be checked 
    if (m_bCheck)
    {
        if (isValid(pInd)) // if valid 
            fFitness = m_oFlexed.rmsd( m_oTarget );
        else
            fFitness = 1.0E10;
    }else
        fFitness = m_oFlexed.rmsd( m_oTarget );
    
    fFitness = 1.0E10 - fFitness;
    if (fFitness < 0.0)
        fFitness = 0.0;

    pInd->setFitness( fFitness );
}

/**
 * output result
 */
void svt_gaflex::outputResult()
{
    sortPopulation();
    
    char pFname[256];
    
    sprintf(pFname, "%s/target.pdb", m_pPath);
    m_oTarget.writePDB( pFname );
    
//     sprintf(pFname, "%s/probe.pdb", m_pPath);
//     m_oProbe.writePDB( pFname );
    
    // output the top 10 solutions
    svt_point_cloud_pdb< svt_vector4<Real64> > oProbe;
    svt_matrix<Real64> oMat;
    for(int i=0; i<5; i++)
    {
	calcCoordinates( &m_oPop[ m_oPop.size() - i - 1] );
        
        sprintf(pFname, "%s/result%d.pdb", m_pPath, i);
        if (m_iGenerations==1)
            m_oFlexed.writePDB( pFname );
        else 
            m_oFlexed.writePDB( pFname, true );

    }
}

/**
 * print results (to cout)
 */
void svt_gaflex::printResults()
{
    for(unsigned int i=0; i<m_oPop.size(); i++)
    {
        printf("[%2i] = ", i);
	m_oPop[i].printGenes();
    }
}

/**
 * is valid
 */
bool svt_gaflex::isValid(svt_ga_ind* pInd)
{
    
    svt_vector4<Real64> oVec1, oVec2;
    Real64 fDist, fValue;
    
    for (int iIndex1=0; iIndex1 < pInd->getGeneCount()/3; iIndex1++)
    {
        for (int iIndex2= iIndex1+1; iIndex2 < pInd->getGeneCount()/3; iIndex2++)
        {        
            if ( m_oDistMat[iIndex1][iIndex2]>0 )
            {
                oVec1 = calcCoordinate(pInd, iIndex1);
                oVec2 = calcCoordinate(pInd, iIndex2);
                
                fDist = oVec1.distance( oVec2 );
                fValue = abs(m_oDistMat[iIndex1][iIndex2]-fDist);
                
                if ( fValue > m_fConnStretch)
                    return false;
            }
        }
    }
    
    return true;
};

/**
 * Compute the Skeleton matrix
 */
void svt_gaflex::calcDistMat(Real64 fDiscardValue)
{
   m_oDistMat.resize( m_oProbe.size(), m_oProbe.size() );
   Real64 fDist;
  
    for(unsigned int iIndex1=0; iIndex1 < m_oProbe.size(); iIndex1++)
    {
        for(unsigned int iIndex2=iIndex1+1; iIndex2 < m_oProbe.size(); iIndex2++)
        {
            fDist = m_oProbe[iIndex1].distance( m_oProbe[iIndex2] );
            if (fDiscardValue!=0 && fDist>fDiscardValue)
                fDist = 0;
            m_oDistMat[iIndex1][iIndex2] = fDist;
            m_oDistMat[iIndex2][iIndex1] = fDist;
        }
        m_oDistMat[iIndex1][iIndex1] = 0;
    }
    
    svtout << m_oDistMat << endl;
}
