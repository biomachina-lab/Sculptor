/***************************************************************************
                          svt_gafit
                          ---------
    begin                : 06/23/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_gafit.h>
#include <svt_matrix.h>

// streams
#include <iostream>
using namespace std;

/**
 * center model and scene
 */
void svt_gafit::centerModelScene()
{
    unsigned int i=0;
            
    // first center model
    svt_vector4<Real64> oMin( 1.0E10,  1.0E10,  1.0E10);
    svt_vector4<Real64> oMax(-1.0E10, -1.0E10, -1.0E10);
    for(i=0; i<m_oModel.size(); i++)
    {
	if (m_oModel[i].x() > oMax.x())
            oMax.x( m_oModel[i].x() );
	if (m_oModel[i].y() > oMax.y())
            oMax.y( m_oModel[i].y() );
	if (m_oModel[i].z() > oMax.z())
	    oMax.z( m_oModel[i].z() );

	if (m_oModel[i].x() < oMin.x())
            oMin.x( m_oModel[i].x() );
	if (m_oModel[i].y() < oMin.y())
            oMin.y( m_oModel[i].y() );
	if (m_oModel[i].z() < oMin.z())
	    oMin.z( m_oModel[i].z() );
    }
    svt_vector4<Real64> oTrans = ( oMax - oMin ) * 0.5;
    oTrans = oTrans - oMax;
    for(i=0; i<m_oModel.size(); i++)
    {
	m_oModel[i].x( m_oModel[i].x() + oTrans.x());
        m_oModel[i].y( m_oModel[i].y() + oTrans.y());
        m_oModel[i].z( m_oModel[i].z() + oTrans.z());
    }

    m_oUnCenter = oTrans;

    // now center scene
    oMin = svt_vector4<Real64>( 1.0E10,  1.0E10,  1.0E10);
    oMax = svt_vector4<Real64>(-1.0E10, -1.0E10, -1.0E10);
    for(i=0; i<m_oScene.size(); i++)
    {
	if (m_oScene[i].x() > oMax.x())
            oMax.x( m_oScene[i].x() );
	if (m_oScene[i].y() > oMax.y())
            oMax.y( m_oScene[i].y() );
	if (m_oScene[i].z() > oMax.z())
	    oMax.z( m_oScene[i].z() );

	if (m_oScene[i].x() < oMin.x())
            oMin.x( m_oScene[i].x() );
	if (m_oScene[i].y() < oMin.y())
            oMin.y( m_oScene[i].y() );
	if (m_oScene[i].z() < oMin.z())
	    oMin.z( m_oScene[i].z() );
    }
    oTrans = ( oMax - oMin ) * 0.5;
    oTrans = oTrans - oMax;
    for(i=0; i<m_oScene.size(); i++)
    {
	m_oScene[i].x( m_oScene[i].x() + oTrans.x());
        m_oScene[i].y( m_oScene[i].y() + oTrans.y());
        m_oScene[i].z( m_oScene[i].z() + oTrans.z());
    }

    m_oSearchSpace = ( oMax - oMin );

}

/**
 * generate initial population
 * \param iNum number of inds in this population
 */
void svt_gafit::initPopulation(int iNum)
{
    svt_ga::initPopulation( iNum );

    m_fTransSampl = 1.0 / 3.0;
    m_fRotSampl = 1.0 / 3.0;

//     m_fRotSampl = 0.0;
//     m_fTransSampl = 0.0;
// 
//     m_fTransSampl /= 3.0;
//     m_fRotSampl /= 3.0;
// 
//     m_fTransSampl /= 3.0;
//     m_fRotSampl /= 3.0;

    //m_fTransSampl = 0.0;
    //m_fRotSampl = 0.0;
}

/**
 * update adaptive fitness
 */
void svt_gafit::updateAdaptive()
{
    m_fTransSampl /= 3.0;
    m_fRotSampl /= 3.0;

    if (m_fTransSampl < 0.0001)
    {
	m_fTransSampl = 0.0;
	m_fRotSampl = 0.0;
    }

//     cout << endl;
//     cout << m_iGenerations << " - " << m_fTransSampl << endl;
//     cout << endl;
}

/**
 * calculate transformation matrix
 * \param pInd pointer to individual
 */
svt_matrix4<Real64> svt_gafit::calcTransformation(svt_ga_ind* pInd)
{
    double fX = pInd->getGene(0);
    double fY = pInd->getGene(1);
    double fZ = pInd->getGene(2);

    // adaptive sampling
    //float fTenPerc = (float)(m_iMaxGen) / 10.0f;
    //int iTenPerc = (int)((float)(m_iGenerations) / fTenPerc) +1;
    //m_fTransSampl = 1.0 / pow(2.0, iTenPerc);

    //cout << m_fTransSampl << ": (" << fX << "," << fY << "," << fZ << ") - ";

    if (m_fTransSampl != 0.0)
    {
	int iX = (int)( fX / m_fTransSampl );
	fX = (iX * m_fTransSampl) + (m_fTransSampl / 2.0);
	int iY = (int)( fY / m_fTransSampl );
	fY = (iY * m_fTransSampl) + (m_fTransSampl / 2.0);
	int iZ = (int)( fZ / m_fTransSampl );
	fZ = (iZ * m_fTransSampl) + (m_fTransSampl / 2.0);

	//cout << "(" << fX << "," << fY << "," << fZ << ")" << endl;
	//getchar();
    }

    fX = (fX - 0.5) * m_oSearchSpace.x();
    fY = (fY - 0.5) * m_oSearchSpace.y();
    fZ = (fZ - 0.5) * m_oSearchSpace.z();

    double fRX = pInd->getGene(3);
    double fRY = pInd->getGene(4);
    double fRZ = pInd->getGene(5);

    if (m_fRotSampl != 0.0)
    {
	int iRX = (int)( fRX / m_fRotSampl );
	fRX = (iRX * m_fRotSampl) + (m_fRotSampl / 2.0);
	int iRY = (int)( fRY / m_fRotSampl );
	fRY = (iRY * m_fRotSampl) + (m_fRotSampl / 2.0);
	int iRZ = (int)( fRZ / m_fRotSampl );
	fRZ = (iRZ * m_fRotSampl) + (m_fRotSampl / 2.0);
    }

    fRX *= 360.0;
    fRY *= 360.0;
    fRZ *= 360.0;

    svt_matrix4<Real64> oMat;
    oMat.rotate( 1.0, 0.0, 0.0, fRX );
    oMat.rotate( 0.0, 1.0, 0.0, fRY );
    oMat.rotate( 0.0, 0.0, 1.0, fRZ );
    oMat[0][3] = fX;
    oMat[1][3] = fY;
    oMat[2][3] = fZ;

    return oMat;
}

/**
 * update fitness
 * \param pInd pointer to individual that gets updated
 */
void svt_gafit::updateFitness(svt_ga_ind* pInd)
{
    // first calculate transformation matrix using the binary encoded parameters in the individual
    svt_matrix<Real64>  oMat = calcTransformation( pInd );

    // now calculate the nearest neighbor for each transformed model vector in the scene
    double fFitness = 0.0;
    
    svt_point_cloud_pdb< svt_vector4<Real64 > > oModel = oMat * m_oModel;
    
    fFitness = oModel.rmsd( m_oScene );

    fFitness = 1.0E10 - fFitness;
    if (fFitness < 0.0)
        fFitness = 0.0;

    pInd->setFitness( fFitness );
}

/**
 * output result
 */
void svt_gafit::outputResult()
{
    sortPopulation();
    
    char pFname[256];
    
    sprintf(pFname, "%s/scene.pdb", m_pPath);
    m_oScene.writePDB( pFname );
    
    sprintf(pFname, "%s/model.pdb", m_pPath);
    m_oModel.writePDB( pFname );
    
    
    // output the top 10 solutions
    svt_point_cloud_pdb< svt_vector4<Real64> > oModel;
    svt_matrix<Real64> oMat;
    for(int i=0; i<5; i++)
    {
	oMat = calcTransformation( &m_oPop[ m_oPop.size() - i - 1] );
        oModel = oMat * m_oModel;
        
        sprintf(pFname, "%s/result%d.pdb", m_pPath, i);
        if (m_iGenerations==1)
            oModel.writePDB( pFname );
        else 
            oModel.writePDB( pFname, true );

    }
}

/**
 * print results (to cout)
 */
void svt_gafit::printResults()
{
    for(unsigned int i=0; i<m_oPop.size(); i++)
    {
        printf("[%2i] = ", i);
	m_oPop[i].printGenes();
    }
}
