/***************************************************************************
                          sculptor_force_algo
                          -------------------
    begin                : 17.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_force_algo.h>
#include <sculptor_scene.h>
#include <sculptor_window.h>
// svt includes
#include <svt_opengl.h>
#include <svt_vector4f.h>
#include <svt_init.h>
#include <svt_threads.h>
#include <svt_time.h>
// clib includes
#include <stdio.h>

extern sculptor_window* g_pWindow;

int g_iLastForceUpdate = 0;

/**
 * Constructor
 */
sculptor_force_algo::sculptor_force_algo(svt_adv_force_tool* pTool) : svt_adv_force_calc(),
    m_oTransMatrix(),
    m_oAlgoSema(1)
{
    m_bForceAlgoInitialized = false;

    m_bExhaustiveSearchDataLoaded = false;

    m_pTool = pTool;
    m_pProbeDoc = NULL;
    m_pTargetDoc = NULL;

    m_dCorrelation = 0.0;

    m_bAtLocalMaximum = false;

    m_bForceArrowVisible = true;
    m_fForceArrowScale   = 1.0f;

    m_bForceTransX = true;
    m_bForceTransY = true;
    m_bForceTransZ = true;
    m_bForceRotX = true;
    m_bForceRotY = true;
    m_bForceRotZ = true;

    m_fCorrForceScale = 0.5f;
    m_fStericForceScale = 0.5f;
    m_fRotForceScale = 0.0f;

    m_fCentralSphereScale = 0.07f;

    m_fSphereColorFunctionRed = 0.2f;
    m_fSphereColorFunctionBlue = 0.8f;
    m_fSphereColorFunctionGreen = 0.95f;

    // Steric clashing distance
    m_fStericClashThreshold = 2.25f;

    // "Good" steric distance
    m_fStericGoodThreshold = 7.5f;

    m_bStericClashingRun = false;

    m_iLastScreenUpdateTime = 0;
}
sculptor_force_algo::~sculptor_force_algo()
{
}

///////////////////////////////////////////////////////////////////////////////
// Force Calculation Routines
///////////////////////////////////////////////////////////////////////////////

/**
 * calculate the forces
 */
void sculptor_force_algo::calculateForces()
{
    m_oAlgoSema.P();

    // no probe document?
    if (!m_pProbeDoc || !m_pTool->isButtonPressed() || !isInitialized())
    {
        setForceX(0.0f);       setForceY(0.0f);       setForceZ(0.0f);
        setTorqueForceX(0.0f); setTorqueForceY(0.0f); setTorqueForceZ(0.0f);
        m_oAlgoSema.V();
        return;
    }

    // Perform the actual force calculation
    calcForcesDriver();

    // switch off some force axes
    if (!m_bForceTransX) m_oForceVec.x(0.0f);
    if (!m_bForceTransY) m_oForceVec.y(0.0f);
    if (!m_bForceTransZ) m_oForceVec.z(0.0f);
    if (!m_bForceRotX)   m_oTorqueVec.x(0.0f);
    if (!m_bForceRotY)   m_oTorqueVec.y(0.0f);
    if (!m_bForceRotZ)   m_oTorqueVec.z(0.0f);

    // apply forces to device (if the button is pressed)
    if (m_pTool->isButtonPressed())
    {
        setForceX(m_oForceVec.x());
        setForceY(m_oForceVec.y());
        setForceZ(m_oForceVec.z());
        setTorqueForceX(m_oTorqueVec.x());
        setTorqueForceY(m_oTorqueVec.y());
        setTorqueForceZ(m_oTorqueVec.z());

    } else {

        setForceX(0.0f);
        setForceY(0.0f);
        setForceZ(0.0f);
        setTorqueForceX(0.0f);
        setTorqueForceY(0.0f);
        setTorqueForceZ(0.0f);

    }

    unsigned long iCurrentTime = svt_getToD();

    // Only do screen updates every 35ms
    // Otherwise, we overload the redrawing system end things start to hang...
    if (iCurrentTime - m_iLastScreenUpdateTime > 35)
    {
	// calculate steric clashing
	calcStericClashing();

	// update visual output
	updateCentralSphere();
	updateCentralSphereHighlight();
	updateCentralText();
	updateForceArrow();

	m_iLastScreenUpdateTime = iCurrentTime;
    }

    m_oAlgoSema.V();
}

/**
 * update the display
 */
void sculptor_force_algo::updateDisplay()
{
    // no probe document?
    if (!m_pProbeDoc || !isInitialized()) return;

    calcForcesDriver();
    calcStericClashing();
    updateForceArrow();
    updateCentralSphere();
    updateCentralSphereHighlight();
    updateCentralText();

}

/**
 * Driver for calculating the actual forces
 */
void sculptor_force_algo::calcForcesDriver() 
{
    // reset forces and torques
    m_oForceVec.x( 0.0f ); m_oForceVec.y( 0.0f ); m_oForceVec.z( 0.0f );
    m_oTorqueVec.x( 0.0f ); m_oTorqueVec.y( 0.0f ); m_oTorqueVec.z( 0.0f );

    m_bAtLocalMaximum = false;

    // which type of forces do we calculate?
    if (m_bExhaustiveSearchDataLoaded)
    {
        // based on exhaustive search
        calcForcesEli();

    } else {

	// based on CC at CV positions
	if (   (m_pTargetDoc != NULL)
	    && (((situs_document *) m_pTargetDoc)->gradientCalculated())
	    && (m_pTargetDoc->getType() == SENSITUS_DOC_SITUS)
	    && (m_pProbeDoc->getCodebook().size() > 0)        )
	{
	    calcForcesCV();

	    double fFact = 1.0;
	    fFact = 1.0 / (double)( ((situs_document *)m_pTargetDoc)->getMaxDensity() *
				    m_pProbeDoc->getCodebook().size() );

	    m_oForceVec = m_oForceVec * fFact;
	    m_oTorqueVec = m_oTorqueVec * fFact;
	}

    }

    // apply force scaling from UI
    m_oForceVec  *= (m_fCorrForceScale * 30.0);
    m_oTorqueVec *= (m_fCorrForceScale * 1.0E-6);

    // if the forces are larger than 100%, scale them down to 100 again
    normalizeForces();
}

/**
 * calculate the forces over codebook vectors
 * \param cv codebook vector
 * \param inner true, if codebook vector is inner vector from laplace data
 */
void sculptor_force_algo::calcForcesOverCV(svt_point_cloud< svt_vector4<Real64> >& rCV, bool inner) //inner=false default
{
    situs_document* pTargetDoc = NULL;
    pdb_document*   pProbeDoc = NULL;

    if ( (m_pTargetDoc != NULL) &&  m_pTargetDoc->getType() == SENSITUS_DOC_SITUS )
        pTargetDoc = (situs_document *) m_pTargetDoc;

    if (m_pProbeDoc != NULL )
        pProbeDoc = (pdb_document *) m_pProbeDoc;

    if ( (pTargetDoc == NULL) || (pProbeDoc == NULL) ) return;

    svt_vector4<double> oNewForce(0,0,0);
    svt_vector4<double> oOrigin;
    int iVoxelX, iVoxelY, iVoxelZ;
    float fWidth = pTargetDoc->getVoxelWidth() * 1.0E-10f;
    double innerFac;

    // inner = -1, if codebook vector is inner vector from laplace data
    innerFac = (inner ? -1. : 1.);

    // calculate the center of the volume data
    oOrigin.x( (pTargetDoc->getVoxelWidth()*pTargetDoc->getExtX() * 1.0E-2f) * 0.5f);
    oOrigin.y( (pTargetDoc->getVoxelWidth()*pTargetDoc->getExtY() * 1.0E-2f) * 0.5f);
    oOrigin.z( (pTargetDoc->getVoxelWidth()*pTargetDoc->getExtZ() * 1.0E-2f) * 0.5f);

    // now calculate the force for each codebook vector
    for(unsigned int i=0;i<rCV.size();i++)
    {
        // get original position
	svt_vector4<double> oVec = rCV[i];

	// convert to opengl coord
        oVec *= 1.0E-2;
        // move it into the situs coordinate system
        oVec = m_oTransMatrix * oVec;
        oVec += oOrigin;
        oVec *= 1.0E-8;

        // get the index of the voxel lying under the cv
        iVoxelX = (int)(floor(oVec.x() / fWidth));
        iVoxelY = (int)(floor(oVec.y() / fWidth));
        iVoxelZ = (int)(floor(oVec.z() / fWidth));

        // calculate the gradient at the voxel position - negativ weight for inner codebookvectors
        oNewForce = innerFac * calcGradient(iVoxelX, iVoxelY, iVoxelZ, oVec.x(), oVec.y(), oVec.z(), fWidth);
        // add it to the total force
        m_oForceVec += oNewForce;
        // calculate the torque force
	svt_vector4<double> oAtomVec = rCV[i];
        oAtomVec -= rCV.coa();
        oAtomVec *= 1.0E8f;
        oAtomVec = vectorProduct(oAtomVec, oNewForce);
        // add the force to the total torque force
        m_oTorqueVec += oAtomVec;
    }
}
/**
 * calculate the forces by the help of the codebookvectors
 */
void sculptor_force_algo::calcForcesCV()
{
    svt_node* pSVT_PDB = m_pProbeDoc->getNode();
    svt_node* pSVT_SITUS = m_pTargetDoc->getNode();

    if (pSVT_PDB && pSVT_PDB->getTransformation() && pSVT_SITUS && pSVT_SITUS->getTransformation())
    {
        // precalculate the matrix to do the transformation towards the current situs data
        m_oTransMatrix.set(*pSVT_SITUS->getTransformation());
	m_oTransMatrix.invert();
        m_oTransMatrix *= svt_matrix4<double>(*pSVT_PDB->getTransformation());

        // now calculate the force over codebook vectors
        calcForcesOverCV( m_pProbeDoc->getCodebook() );
    }
}

/**
 * calculate the gradient
 * \param iVoxelX the nearest voxel (x index)
 * \param iVoxelY the nearest voxel (y index)
 * \param iVoxelZ the nearest voxel (z index)
 * \param fCVX codebookvector x position
 * \param fCVY codebookvector y position
 * \param fCVZ codebookvector z position
 * \param fVoxelWidth the width (in angstroem) of a voxel
 */
svt_vector4<double> sculptor_force_algo::calcGradient(int iVoxelX, int iVoxelY, int iVoxelZ, float fCVX, float fCVY, float fCVZ, float fVoxelWidth)
{
    float fGradX;
    float fGradY;
    float fGradZ;
    
    float fForceX;
    float fForceY;
    float fForceZ;

    situs_document* pTargetDoc = NULL;
    pdb_document*   pProbeDoc = NULL;

    if ( (m_pTargetDoc != NULL) &&  m_pTargetDoc->getType() == SENSITUS_DOC_SITUS )
        pTargetDoc = (situs_document *) m_pTargetDoc;

    if (m_pProbeDoc != NULL )
        pProbeDoc = (pdb_document *) m_pProbeDoc;

    if ( (pTargetDoc == NULL) || (pProbeDoc == NULL) ) return svt_vector4f(0,0,0);

    // outside situs data?
    if (iVoxelX < 0 || iVoxelY < 0 || iVoxelZ < 0)
        return svt_vector4f(0,0,0);
    if (iVoxelX >= pTargetDoc->getExtX() || 
	iVoxelY >= pTargetDoc->getExtY() || 
	iVoxelZ >= pTargetDoc->getExtZ()    )
        return svt_vector4f(0,0,0);

    // position of voxel inside the volume
    float fVoxelPosX = iVoxelX * fVoxelWidth;
    float fVoxelPosY = iVoxelY * fVoxelWidth;
    float fVoxelPosZ = iVoxelZ * fVoxelWidth;

    // position of cv inside the voxel
    float fCVXV = (fCVX - fVoxelPosX) / fVoxelWidth;
    float fCVYV = (fCVY - fVoxelPosY) / fVoxelWidth;
    float fCVZV = (fCVZ - fVoxelPosZ) / fVoxelWidth;

    // gradient
    fGradX = (pTargetDoc->getGradientX(iVoxelX+1, iVoxelY, iVoxelZ) - pTargetDoc->getGradientX(iVoxelX, iVoxelY, iVoxelZ));
    fGradY = (pTargetDoc->getGradientY(iVoxelX, iVoxelY+1, iVoxelZ) - pTargetDoc->getGradientY(iVoxelX, iVoxelY, iVoxelZ));
    fGradZ = (pTargetDoc->getGradientZ(iVoxelX, iVoxelY, iVoxelZ+1) - pTargetDoc->getGradientZ(iVoxelX, iVoxelY, iVoxelZ));

    // force = linear interpolation of gradient values according to the position of the cv inside the voxel
    fForceX = (fCVXV * fGradX) + pTargetDoc->getGradientX(iVoxelX, iVoxelY, iVoxelZ);
    fForceY = (fCVYV * fGradY) + pTargetDoc->getGradientY(iVoxelX, iVoxelY, iVoxelZ);
    fForceZ = (fCVZV * fGradZ) + pTargetDoc->getGradientZ(iVoxelX, iVoxelY, iVoxelZ);

    return svt_vector4<double>(fForceX, fForceY, fForceZ);
}

/**
 * calculate the correlation
 * \param iVoxelX the nearest voxel (x index)
 * \param iVoxelY the nearest voxel (y index)
 * \param iVoxelZ the nearest voxel (z index)
 * \param fCVX codebookvector x position
 * \param fCVY codebookvector y position
 * \param fCVZ codebookvector z position
 * \param fVoxelWidth the width (in angstroem) of a voxel
 */
double sculptor_force_algo::calcCorrelation(int iVoxelX, int iVoxelY, int iVoxelZ, float fCVX, float fCVY, float fCVZ, float fVoxelWidth)
{
    situs_document* pTargetDoc = NULL;

    if ( (m_pTargetDoc != NULL) &&  m_pTargetDoc->getType() == SENSITUS_DOC_SITUS )
        pTargetDoc = (situs_document *) m_pTargetDoc;

    if (pTargetDoc == NULL) return 0.0;

    // outside situs data?
    if (iVoxelX < 0 || iVoxelY < 0 || iVoxelZ < 0)
        return 0;
    if (iVoxelX >= pTargetDoc->getExtX() || iVoxelY >= pTargetDoc->getExtY() || iVoxelZ >= pTargetDoc->getExtZ())
        return 0;

    // position of cv inside the voxel (0..1)
    float fCVXV = fCVX / fVoxelWidth - iVoxelX; 
    float fCVYV = fCVY / fVoxelWidth - iVoxelY;
    float fCVZV = fCVZ / fVoxelWidth - iVoxelZ;

    return (  fCVXV)*(  fCVYV)*(  fCVZV) * pTargetDoc->getAt(iVoxelX+1, iVoxelY+1, iVoxelZ+1) +
	   (  fCVXV)*(  fCVYV)*(1-fCVZV) * pTargetDoc->getAt(iVoxelX+1, iVoxelY+1, iVoxelZ  ) +
	   (  fCVXV)*(1-fCVYV)*(  fCVZV) * pTargetDoc->getAt(iVoxelX+1, iVoxelY  , iVoxelZ+1) +
	   (  fCVXV)*(1-fCVYV)*(1-fCVZV) * pTargetDoc->getAt(iVoxelX+1, iVoxelY  , iVoxelZ  ) +
	   (1-fCVXV)*(  fCVYV)*(  fCVZV) * pTargetDoc->getAt(iVoxelX  , iVoxelY+1, iVoxelZ+1) +
	   (1-fCVXV)*(  fCVYV)*(1-fCVZV) * pTargetDoc->getAt(iVoxelX  , iVoxelY+1, iVoxelZ  ) +
	   (1-fCVXV)*(1-fCVYV)*(  fCVZV) * pTargetDoc->getAt(iVoxelX  , iVoxelY  , iVoxelZ+1) +
	   (1-fCVXV)*(1-fCVYV)*(1-fCVZV) * pTargetDoc->getAt(iVoxelX  , iVoxelY  , iVoxelZ  );
}

/**
 * calculate the forces from Eliquos exhaustive search output
 */
void sculptor_force_algo::calcForcesEli()
{
    situs_document* pTargetDoc = NULL;
    pdb_document*   pProbeDoc = NULL;

    if ( (m_pTargetDoc != NULL) &&  m_pTargetDoc->getType() == SENSITUS_DOC_SITUS )
        pTargetDoc = (situs_document *) m_pTargetDoc;

    if (m_pProbeDoc != NULL )
        pProbeDoc = (pdb_document *) m_pProbeDoc;

    if ( (pTargetDoc == NULL) || (pProbeDoc == NULL) ) return;

    svt_node* pSVT_PDB = pProbeDoc->getNode();
    svt_node* pSVT_SITUS = pTargetDoc->getNode();

    if (pSVT_PDB && pSVT_PDB->getTransformation() && pSVT_SITUS && pSVT_SITUS->getTransformation())
    {

        // Figure out where the Eliquos data is stored
	sculptor_document* pEliDataDoc;
	if (m_pProbeDoc == m_pProbeDoc->getOrigProbeDoc())
	    pEliDataDoc = m_pProbeDoc;
	else
	    pEliDataDoc = m_pProbeDoc->getOrigProbeDoc();

        // get pointers to the Eliquos data
	svt_volume<Real64>* pCrossCorr  = pEliDataDoc->getEliCrossCorrVolume();
	svt_volume<Real64>* pAngleIndex = pEliDataDoc->getEliAngleIndexVolume();
	vector<EULER_ANGLE>* pAngleList = pEliDataDoc->getEliAngleList();

        // precalculate the matrix to do the transformation towards the current situs data
        m_oTransMatrix.set(*pSVT_SITUS->getTransformation());
	m_oTransMatrix.invert();
        m_oTransMatrix *= svt_matrix4<double>(*pSVT_PDB->getTransformation());

	// actual force calculation
	svt_vector4<double> oVec(0,0,0,1);
	svt_vector4<double> oNewForce(0,0,0);
	svt_vector4<double> oOrigin;
	float               fWidth = pCrossCorr->getWidth() * 1.0E-10f;
	int                 iVoxelX, iVoxelY, iVoxelZ;

	// calculate the center of the volume data
	oOrigin.x( (pCrossCorr->getWidth()*pCrossCorr->getSizeX() * 1.0E-2f) * 0.5f);
	oOrigin.y( (pCrossCorr->getWidth()*pCrossCorr->getSizeY() * 1.0E-2f) * 0.5f);
	oOrigin.z( (pCrossCorr->getWidth()*pCrossCorr->getSizeZ() * 1.0E-2f) * 0.5f);

	// convert to opengl coord
        oVec *= 1.0E-2;
        // move it into the situs coordinate system
        oVec = m_oTransMatrix * oVec;
        oVec += oOrigin;
        oVec *= 1.0E-8;

	// shift by half a voxel if the CC map has odd dimensions
	if (   (pCrossCorr->getSizeX() % 2 == 1)
	    && (pCrossCorr->getSizeY() % 2 == 1)
	    && (pCrossCorr->getSizeZ() % 2 == 1) ) oVec -= fWidth/2.0f;
	
        // get the index of the voxel lying under the current position
        iVoxelX = (int)(floor((oVec.x() / fWidth) + 0.5f));
        iVoxelY = (int)(floor((oVec.y() / fWidth) + 0.5f));
        iVoxelZ = (int)(floor((oVec.z() / fWidth) + 0.5f));

	// outside correlation data? 
	if (iVoxelX < 1 || iVoxelY < 1 || iVoxelZ < 1)
	    return;
	if (iVoxelX >= (int) pCrossCorr->getSizeX() - 1 || 
	    iVoxelY >= (int) pCrossCorr->getSizeY() - 1 || 
	    iVoxelZ >= (int) pCrossCorr->getSizeZ() - 1)
	    return;

	// get correlation
	double dCorr = pCrossCorr->getValue(iVoxelX  , iVoxelY  , iVoxelZ  );
	m_dCorrelation = dCorr;

	// calculate forces
	double dCorrX1 = pCrossCorr->getValue(iVoxelX-1, iVoxelY  , iVoxelZ  );
	double dCorrX2 = pCrossCorr->getValue(iVoxelX+1, iVoxelY  , iVoxelZ  );
	double dCorrY1 = pCrossCorr->getValue(iVoxelX  , iVoxelY-1, iVoxelZ  );
	double dCorrY2 = pCrossCorr->getValue(iVoxelX  , iVoxelY+1, iVoxelZ  );
	double dCorrZ1 = pCrossCorr->getValue(iVoxelX  , iVoxelY  , iVoxelZ-1  );
	double dCorrZ2 = pCrossCorr->getValue(iVoxelX  , iVoxelY  , iVoxelZ+1  );

	double centerX = iVoxelX * fWidth;
	double centerY = iVoxelY * fWidth;
	double centerZ = iVoxelZ * fWidth;

	double dScale = 1.0e-10;

	double dForceX = dScale * interpolateForce(centerX, fWidth, dCorrX1, dCorr, dCorrX2, oVec.x());
	double dForceY = dScale * interpolateForce(centerY, fWidth, dCorrY1, dCorr, dCorrY2, oVec.y());
	double dForceZ = dScale * interpolateForce(centerZ, fWidth, dCorrZ1, dCorr, dCorrZ2, oVec.z());

	oNewForce.set(dForceX, dForceY, dForceZ);

        if (atLocalMaximum(pCrossCorr, m_dCorrelation, iVoxelX, iVoxelY, iVoxelZ))
	    m_bAtLocalMaximum = true;
	else
	    m_bAtLocalMaximum = false;

	// svtout << "Corr: " << dCorr 
	//        << " Force: " << dForceX << " " << dForceY << " " << dForceZ << endl;

	// Get the rotation angles
	double dAngleIndex = pAngleIndex->getValue(iVoxelX, iVoxelY, iVoxelZ);
	int    iAngleIndex = (int) floor(dAngleIndex + 0.2);
	double psi         = (*pAngleList)[iAngleIndex].psi;
	double theta       = (*pAngleList)[iAngleIndex].theta;
	double phi         = (*pAngleList)[iAngleIndex].phi;

	unsigned long iCurrentTime = svt_getToD();

	// Only do screen updates every 35ms
	// Otherwise, we overload the redrawing system end things start to hang...
	if (iCurrentTime - m_iLastScreenUpdateTime > 35)
	{
	    m_pProbeDoc->getNode()->getNodeLock()->P();
	    m_pProbeDoc->updateSculptorTransformation();
	    rotPdbEuler(m_pProbeDoc->getTransformation(), psi, theta, phi);
	    m_pProbeDoc->updateSVTTransformation();
	    m_pProbeDoc->getNode()->getNodeLock()->V();
	}

	// write back calculated forces
	m_oForceVec = oNewForce;

    }
}

/**
 * Interpolate force
 */
double sculptor_force_algo::interpolateForce(double center, double width,
					     double y1, double y2, double y3,
					     double coord)
{
    double width2 = width*width;
    double a, b, ycomb, delta;

    if ( ((y2 < y1) && (y2 < y3)) || ((y2 > y1) && (y2 > y3)) )
    {
        // We are close to a minimum, use a simple y = a x^2 formula
	
	delta = coord - center;

	if ( delta > 0.0 )
	{
	    // Right branch

	    a = (y3 - y2) / width2;
	    return 2.0 * a * delta;
	    
        } else {

	    // Left branch

	    a = (y1 - y2) / width2;
	    return 2.0 * a * delta;
	}

    } else {

        // The general case
	
	ycomb  = y1 - 2*y2 + y3;

	a = ycomb / (2*width2);
	b = (width*(-y1+y3) - 2*center*ycomb) / (2*width2);

	// This is the remaining parameter which isn't used for the force interpolation
	// double c = (2*width2*y2 + center*width*(y1-y3) + center*center*ycomb) / (2*width2);

	return 2.0*a*coord + b;
    }
}

/**
 * Determine if current position is a local maximum
 * \return true if local maximum
 */
bool sculptor_force_algo::atLocalMaximum(svt_volume<Real64>* pArray, double dVal,
                                         int iX, int iY, int iZ)
{
    for (int i=-1; i<2; i++)
	for (int j=-1; j<2; j++)
	    for (int k=-1; k<2; k++)
		if (dVal < pArray->getValue(iX+i, iY+j, iZ+k))
		    return false;

    return true;
}

/**
 * rotate pdb node by Euler angles
 */
void sculptor_force_algo::rotPdbEuler(svt_matrix4<double>& rMat, double psi, double theta, double phi)
{
    svt_vector4<double> oVec = rMat.translation();
    rMat.loadIdentity();
    rMat.rotatePTP(deg2rad(phi), deg2rad(theta), deg2rad(psi));
    rMat.translate( oVec );
}

/**
 * perform steric clashing calculation
 */
void sculptor_force_algo::calcStericClashing()
{
    if (!m_pProbeDoc) return;

    // Steric clashing turned on?
    if (!g_pWindow->getStericClashing()) return;

    // Do we have the steric clashing data?
    svt_volume<Real64>* pStericClashingData;
    pStericClashingData = g_pWindow->getStericClashingData();
    if (pStericClashingData == NULL)
    {
        makeStericSpheresInvisible();
	return;
    }

    svt_node* pSVT_PDB = m_pProbeDoc->getNode();

    svt_point_cloud< svt_vector4<Real64> > rCV;

    svt_vector4<double> oOrigin;
    int iVoxelX, iVoxelY, iVoxelZ;
    float fWidth = pStericClashingData->getWidth();

    if (pSVT_PDB && pSVT_PDB->getTransformation())
    {
        // get the transformation of the probe doc
        m_oTransMatrix.set(*pSVT_PDB->getTransformation());

	// get codebook
        rCV = m_pProbeDoc->getCodebookSC();
	if (rCV.size() == 0) return;

	// calculate the center of the volume data
	oOrigin.x( pStericClashingData->getGridX() );
	oOrigin.y( pStericClashingData->getGridY() );
	oOrigin.z( pStericClashingData->getGridZ() );

	// now calculate the position of each codebook vector
	Real64 fCurrDistance = 0.0;
	for(unsigned int i=0;i<rCV.size();i++)
	{
	    // get original position
	    svt_vector4<double> oVec = rCV[i];

	    // convert to opengl coord
	    oVec *= 1.0E-2;
	    // move it into the coordinate system of the clashing data
	    oVec = m_oTransMatrix * oVec;
	    oVec *= 1.0E2;
	    oVec -= oOrigin;

	    // get the index of the voxel lying under the cv
	    iVoxelX = (int)(nearbyint(oVec.x() / fWidth));
	    iVoxelY = (int)(nearbyint(oVec.y() / fWidth));
	    iVoxelZ = (int)(nearbyint(oVec.z() / fWidth));

	    // outside steric clashing data? 
	    if (iVoxelX < 1 || iVoxelY < 1 || iVoxelZ < 1)
	    {
		m_pProbeDoc->setStericSpheresVisible( i, false );
		continue;
            }
	    if (iVoxelX >= (int) pStericClashingData->getSizeX() - 1 || 
		iVoxelY >= (int) pStericClashingData->getSizeY() - 1 || 
		iVoxelZ >= (int) pStericClashingData->getSizeZ() - 1)
	    {
		m_pProbeDoc->setStericSpheresVisible( i, false );
		continue;
	    }

	    fCurrDistance = pStericClashingData->getValue(iVoxelX, iVoxelY, iVoxelZ);

	    if ((fCurrDistance != 0.0) && (fCurrDistance < m_fStericGoodThreshold)) {

	        // Set color depending on good or bad contact
	        if (fCurrDistance < m_fStericClashThreshold)
		{
		    m_pProbeDoc->setStericSpheresColor( i, new svt_color(1.0f, 0.0f, 0.0f) );
		} else {
		    m_pProbeDoc->setStericSpheresColor( i, new svt_color(0.0f, 1.0f, 0.0f) );
		}

	        // make CV #i visible
		m_pProbeDoc->setStericSpheresVisible( i, true );

            } else {

	        // make CV #i invisible
		m_pProbeDoc->setStericSpheresVisible( i, false );
            }

	}
    }

    m_bStericClashingRun = true;

    return;
}

/**
 * Make all individual steric spheres invisible
 * (as opposed to turning off the container)
 */
void sculptor_force_algo::makeStericSpheresInvisible()
{
    if (!m_pProbeDoc) return;

    svt_point_cloud< svt_vector4<Real64> > rCV = m_pProbeDoc->getCodebookSC();

    for(unsigned int i=0;i<rCV.size();i++)
    {
	m_pProbeDoc->setStericSpheresVisible( i, false );
    }

    return;
}

/**
 * Update central sphere color and size
 */
void sculptor_force_algo::updateCentralSphere()
{

    if (m_pProbeDoc->getType() == SENSITUS_DOC_PDB)
    {
	if (!m_bExhaustiveSearchDataLoaded)
	{
	    // Regular haptic docking
	    // ((pdb_document*)m_pProbeDoc)->setCenterSphereRadius( 0.6 * m_fCentralSphereScale);
	    // ((pdb_document*)m_pProbeDoc)->setCenterSphereColor( 1.0, 1.0, 1.0 );
	    ((pdb_document*)m_pProbeDoc)->setCenterSphereVisible( false );

	} else {

	    // Eliquos-based haptic docking

	    double dCorrDiff;
	    double dRed;
	    double dGreen;
	    double dBlue;

	    if (m_dCorrelation < m_fSphereColorFunctionRed)
	    {
	        dRed      = 1.0;
	        dGreen    = 1.0;
	        dBlue     = 1.0;
            }
	    if (m_dCorrelation >= m_fSphereColorFunctionRed && m_dCorrelation < m_fSphereColorFunctionBlue)
	    {
	        dCorrDiff = (m_dCorrelation - m_fSphereColorFunctionRed) / m_fSphereColorFunctionBlue;
	        dRed      = 1.0 - dCorrDiff;
	        dGreen    = 0.0;
	        dBlue     = dCorrDiff;
            }
	    if (m_dCorrelation >= m_fSphereColorFunctionBlue)
	    {
	        dCorrDiff = (m_dCorrelation - m_fSphereColorFunctionBlue) /
		            (m_fSphereColorFunctionGreen - m_fSphereColorFunctionBlue);
	        dRed      = 0.0;
	        dGreen    = dCorrDiff;
	        dBlue     = 1.0 - dCorrDiff;
            }

	    ((pdb_document*)m_pProbeDoc)->setCenterSphereColor( dRed, dGreen, dBlue );

	    // if the sphere is bright green, change the highlighting to blue.
	    // Otherwise, highlight local maxima in white
	    if ( (dGreen > 0.95) && (dRed < 0.1) && (dBlue < 0.1) )
                ((pdb_document*) m_pProbeDoc)->setCenterSphereHighlightColor(0.0f, 0.0f, 1.0f);
	    else
                ((pdb_document*) m_pProbeDoc)->setCenterSphereHighlightColor(1.0f, 1.0f, 1.0f);
	}
    }

    return;
}

/**
 * Update central sphere highlight
 */
void sculptor_force_algo::updateCentralSphereHighlight()
{
    if (m_bAtLocalMaximum) 
	((pdb_document*) m_pProbeDoc)->setCenterSphereHighlightVisible( true );
    else
	((pdb_document*) m_pProbeDoc)->setCenterSphereHighlightVisible( false );

    return;
}

/**
 * Update central text
 */
void sculptor_force_algo::updateCentralText()
{

    char pCorrelation[24];

    if (((pdb_document*)m_pProbeDoc)->getCenterTextVisible())
    {
	sprintf(pCorrelation, "%4d",(int) floor(m_dCorrelation*1000.0 + 0.5));
	((pdb_document*)m_pProbeDoc)->setCenterText( pCorrelation );
    }
    
    return;
}

/**
 * Update force arrow
 */
void sculptor_force_algo::updateForceArrow()
{
  if ( !m_pProbeDoc || 
      (m_pProbeDoc->getType() != SENSITUS_DOC_PDB)) return;

  if ( !m_bForceArrowVisible || !m_pProbeDoc->getNode()->getVisible()) 
  {
      ((pdb_document *)m_pProbeDoc)->getForceArrow()->setVisible(false);
      return;
  }

  ((pdb_document *)m_pProbeDoc)->getForceArrow()->setVisible(true);

  // Get some pointers
  svt_matrix4f* pTransf     = ((pdb_document *)m_pProbeDoc)->getForceArrowTransformation();
  svt_node*     pPdbNode    = m_pProbeDoc->getNode();

  svt_matrix4<Real32> mTransformation;
  mTransformation.loadIdentity();

  // Translate the arrow to the current probe position
  Real32 fX = pPdbNode->getPosX();
  Real32 fY = pPdbNode->getPosY();
  Real32 fZ = pPdbNode->getPosZ();

  // The force directions
  Real32 fforceX = m_oForceVec.x();
  Real32 fforceY = m_oForceVec.y();
  Real32 fforceZ = m_oForceVec.z();

  Real32 fnorm   = sqrtf(fforceX*fforceX + fforceY*fforceY + fforceZ*fforceZ);

  // Switch to spherical coords
  Real32 fphi   = atan2f(fforceY, fforceX);
  Real32 ftheta = atan2f(sqrtf(fforceX*fforceX + fforceY*fforceY), fforceZ);

  // Build up the transformation
  mTransformation.scale(m_fForceArrowScale);
  mTransformation.scale(fnorm);
  mTransformation.rotate(1,ftheta);
  mTransformation.rotate(2,fphi);
  mTransformation.translate(fX, fY, fZ);

  // Write transformation back to force arrow node
  mTransformation.copyToMatrix4f(*pTransf);

}

/**
 * Normalize forces to 0-100%
 */
void sculptor_force_algo::normalizeForces()
{

    if (m_oForceVec.x() > 100.0f)
        m_oForceVec.x( 100.0f );
    if (m_oForceVec.y() > 100.0f)
        m_oForceVec.y( 100.0f );
    if (m_oForceVec.z() > 100.0f)
        m_oForceVec.z( 100.0f );
    if (m_oTorqueVec.x() > 100.0f )
        m_oTorqueVec.x( 100.0f );
    if (m_oTorqueVec.y() > 100.0f )
        m_oTorqueVec.y( 100.0f );
    if (m_oTorqueVec.z() > 100.0f )
        m_oTorqueVec.z( 100.0f );
    if (m_oForceVec.x() < -100.0f)
        m_oForceVec.x( -100.0f );
    if (m_oForceVec.y() < -100.0f)
        m_oForceVec.y( -100.0f );
    if (m_oForceVec.z() < -100.0f)
        m_oForceVec.z( -100.0f );
    if (m_oTorqueVec.x() < -100.0f)
        m_oTorqueVec.x( -100.0f );
    if (m_oTorqueVec.y() < -100.0f)
        m_oTorqueVec.y( -100.0f );
    if (m_oTorqueVec.z() < -100.0f)
        m_oTorqueVec.z( -100.0f );

    return;
}

///////////////////////////////////////////////////////////////////////////////
// Sculptor Routines
///////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the force algo object and start force feedback calculations
 */
void sculptor_force_algo::initialize(sculptor_document* pProbeDocInp, sculptor_document* pTargetDocInp)
{
    m_oAlgoSema.P();

    m_pProbeDoc  = pProbeDocInp;
    m_pTargetDoc = pTargetDocInp;

    pdb_document*   pProbeDoc  = (pdb_document *) pProbeDocInp;
    situs_document* pTargetDoc = (situs_document *) pTargetDocInp;

    // Currently, the probe can only be a PDB structure
    if ( (m_pProbeDoc != NULL) && (m_pProbeDoc->getType() == SENSITUS_DOC_PDB) )
        pProbeDoc = (pdb_document *) pProbeDoc;

    // Currently, the target can only be a map
    if ( (m_pTargetDoc != NULL) &&  (m_pTargetDoc->getType() == SENSITUS_DOC_SITUS) )
        pTargetDoc = (situs_document *) pTargetDoc;

    // Bail if we don't have a probe document
    if ( pProbeDoc == NULL )
    {
	m_bForceAlgoInitialized = false;
	m_oAlgoSema.V();
        return;
    }

    bool bWasLoadedBefore  = m_bExhaustiveSearchDataLoaded;

    m_bExhaustiveSearchDataLoaded = pProbeDoc->getIGDActive()
                                    && (pProbeDoc->getEliDataLoaded() || 
                                        (pProbeDoc->getOrigProbeDoc() &&
				         pProbeDoc->getOrigProbeDoc()->getEliDataLoaded()));

    // Calculate gradient map for traditional haptic docking if we have a haptic device
    if ( pTargetDoc != NULL && pProbeDoc != NULL
	&& strcmp(m_pTool->getDevice()->getName(),"mouse") != 0
       	&& !m_bExhaustiveSearchDataLoaded
       	&& !pTargetDoc->gradientCalculated())
        pTargetDoc->calcGradientMap();

    // Change some defaults when doing IGD
    if (m_bExhaustiveSearchDataLoaded)
    {
        // Only remember the previous mouse mode state, if the previous document was not an eliquous document
        if (!bWasLoadedBefore)
            m_iSavedMouseMode = svt_getMouseMode();

        // Switch mouse mode to translation
        if (strcmp(m_pTool->getDevice()->getName(),"mouse") == 0)
            svt_setMouseMode(SVT_MOUSE_TRANS);

        // Turn off all but translations in the move tool
        m_pTool->setTranslationOnly(true);

	// Set initial central sphere radius
	pProbeDoc->setCenterSphereRadius( m_fCentralSphereScale );

	// Visibility of central sphere and text
	pProbeDoc->setCenterSphereVisible( true );
	pProbeDoc->setCenterTextVisible( true );

    } else {

	pProbeDoc->setCenterSphereRadius( 0.6 * m_fCentralSphereScale );

	// Visibility of central sphere and text
	pProbeDoc->setCenterSphereVisible( false );
	pProbeDoc->setCenterTextVisible( false );

    }

    // Make the force arrow visible
    if (m_bExhaustiveSearchDataLoaded)
	pProbeDoc->setForceArrowVisible(true);

    // Steric clashing
    if (g_pWindow->getStericClashing())
	m_pProbeDoc->setStericSpheresVisible( true );

    // We've done all the initialization
    m_bForceAlgoInitialized = true;

    // Update the display
    updateDisplay();

    m_oAlgoSema.V();
};

/**
 * Turn force feedback off again
 */
void sculptor_force_algo::deactivate()
{
    if (m_bForceAlgoInitialized == false) return;

    if (m_pProbeDoc != NULL && m_bExhaustiveSearchDataLoaded)
    {
        // Reset mouse mode
        svt_setMouseMode(m_iSavedMouseMode);

        // Turn on translations and rotations in the move tool
        m_pTool->setTranslationOnly(false);
    }

    if (m_pProbeDoc != NULL && m_pProbeDoc->getType() == SENSITUS_DOC_PDB)
    {
        ((pdb_document*)m_pProbeDoc)->setCenterSphereVisible( false );
        ((pdb_document*)m_pProbeDoc)->setCenterSphereHighlightVisible( false );
        ((pdb_document*)m_pProbeDoc)->setCenterTextVisible( false );
	((pdb_document*)m_pProbeDoc)->setForceArrowVisible(false);
    }

    m_pTool->setActive( false );

    m_bExhaustiveSearchDataLoaded = false;

    m_bForceAlgoInitialized = false;
};

/**
 * Force feedback currently initialized?
 */
bool sculptor_force_algo::isInitialized()
{
    return m_bForceAlgoInitialized;
}

/**
 * Reset probe
 */
void sculptor_force_algo::resetProbe()
{
    m_pProbeDoc = NULL;
    return;
}
/**
 * Reset target
 */
void sculptor_force_algo::resetTarget()
{
    m_pTargetDoc = NULL;
    return;
}

/**
 * Get current correlation
 */
double sculptor_force_algo::getCorrelation()
{
    if ( (m_pProbeDoc == NULL) || (m_pTargetDoc == NULL) )
    {
        return 0.0;
    } else {
	return m_dCorrelation;
    }
}

/**
 * en/disable the translational forces along the x axis
 */
void sculptor_force_algo::setForceTransX(bool bForceTransX)
{
    m_oAlgoSema.P();
    m_bForceTransX = bForceTransX;
    m_oAlgoSema.V();
}
/**
 * get en/disable state of the translational forces along the x axis
 */
bool sculptor_force_algo::getForceTransX()
{
    return m_bForceTransX;
}

/**
 * en/disable the translational forces along the y axis
 */
void sculptor_force_algo::setForceTransY(bool bForceTransY)
{
    m_oAlgoSema.P();
    m_bForceTransY = bForceTransY;
    m_oAlgoSema.V();
}
/**
 * get en/disable state of the translational forces along the y axis
 */
bool sculptor_force_algo::getForceTransY()
{
    return m_bForceTransY;
}

/**
 * en/disable the translational forces along the z axis
 */
void sculptor_force_algo::setForceTransZ(bool bForceTransZ)
{
    m_oAlgoSema.P();
    m_bForceTransZ = bForceTransZ;
    m_oAlgoSema.V();
}
/**
 * get en/disable state of the translational forces along the z axis
 */
bool sculptor_force_algo::getForceTransZ()
{
    return m_bForceTransZ;
}

/**
 * en/disable the rotational forces along the x axis
 */
void sculptor_force_algo::setForceRotX(bool bForceRotX)
{
    m_oAlgoSema.P();
    m_bForceRotX = bForceRotX;
    m_oAlgoSema.V();
}
/**
 * get en/disable state of the rotational forces along the x axis
 */
bool sculptor_force_algo::getForceRotX()
{
    return m_bForceRotX;
}

/**
 * en/disable the rotational forces along the y axis
 */
void sculptor_force_algo::setForceRotY(bool bForceRotY)
{
    m_oAlgoSema.P();
    m_bForceRotY = bForceRotY;
    m_oAlgoSema.V();
}
/**
 * get en/disable state of the rotational forces along the y axis
 */
bool sculptor_force_algo::getForceRotY()
{
    return m_bForceRotY;
}

/**
 * en/disable the rotational forces along the z axis
 */
void sculptor_force_algo::setForceRotZ(bool bForceRotZ)
{
    m_oAlgoSema.P();
    m_bForceRotZ = bForceRotZ;
    m_oAlgoSema.V();
}
/**
 * get en/disable state of the rotational forces along the z axis
 */
bool sculptor_force_algo::getForceRotZ()
{
    return m_bForceRotZ;
}

/**
 * scale the correlation forces
 * \param fScale scale factor (0.0f .. 1.0f)
 */
void sculptor_force_algo::setCorrForceScale(float fScale)
{
    m_oAlgoSema.P();
    m_fCorrForceScale = fScale;
    m_oAlgoSema.V();
}
/**
 * get the scale for the correlation forces
 * \return scale factor (0.0f .. 1.0f)
 */
float sculptor_force_algo::getCorrForceScale()
{
    return m_fCorrForceScale;
}

/**
 * scale the steric forces
 * \param fScale scale factor (0.0f .. 1.0f)
 */
void sculptor_force_algo::setStericForceScale(float fScale)
{
    m_oAlgoSema.P();
    m_fStericForceScale = fScale;
    m_oAlgoSema.V();
}
/**
 * get the scale for the steric forces
 * \return scale factor (0.0f .. 1.0f)
 */
float sculptor_force_algo::getStericForceScale()
{
    return m_fStericForceScale;
}

/**
 * scale the rotational forces
 * \param fScale scale factor (0.0f .. 1.0f)
 */
void sculptor_force_algo::setRotForceScale(float fScale)
{
    m_oAlgoSema.P();
    m_fRotForceScale = fScale;
    m_oAlgoSema.V();
}
/**
 * get the rotational scale for the forces
 * \return scale factor (0.0f .. 1.0f)
 */
float sculptor_force_algo::getRotForceScale()
{
    return m_fRotForceScale;
}

/**
 * scale the central sphere diameter
 * \param fScale scale factor (0.0f .. 1.0f)
 */
void sculptor_force_algo::setCentralSphereScale(float fScale)
{
    m_oAlgoSema.P();
    m_fCentralSphereScale = fScale;
    if (m_pProbeDoc != NULL) 
	((pdb_document*)m_pProbeDoc)->setCenterSphereRadius( m_fCentralSphereScale );
    m_oAlgoSema.V();
    return;
}

/**
 * get the scale for central sphere
 * \return scale factor (0.0f .. 1.0f)
 */
float sculptor_force_algo::getCentralSphereScale()
{
    return m_fCentralSphereScale;
}

/**
 * scale the force arrow
 * \param fScale scale factor (0.0f .. 1.0f)
 */
void sculptor_force_algo::setForceArrowScale(float fScale)
{
    m_oAlgoSema.P();
    m_fForceArrowScale = fScale;
    updateForceArrow();
    m_oAlgoSema.V();
    svt_redraw();
}

/**
 * get the scale for the force arrow
 * \return scale factor
 */
float sculptor_force_algo::getForceArrowScale()
{
    return m_fForceArrowScale;
}

/**
 * set force arrow visibility
 * \param bVisible
 */
void sculptor_force_algo::setForceArrowVisible(bool bVisible)
{
    m_oAlgoSema.P();
    m_bForceArrowVisible = bVisible; 
    updateForceArrow();
    m_oAlgoSema.V();
    svt_redraw();
}

/**
 * get the force arrow visibility
 * \return bVisible
 */
bool sculptor_force_algo::getForceArrowVisible()
{
    return m_bForceArrowVisible;
}

/**
 * Set the threshold for steric clashing
 * \param fThreshold (0.0f .. 20.0f)
 */
void sculptor_force_algo::setStericClashThreshold(float fThreshold)
{
    m_oAlgoSema.P();
    m_fStericClashThreshold = fThreshold;
    m_oAlgoSema.V();
    return;
}

/**
 * get the threshold for steric clashing
 * \return threshold (0.0f .. 20.0f)
 */
float sculptor_force_algo::getStericClashThreshold()
{
    return m_fStericClashThreshold;
}

/**
 * Set the threshold for good contacts
 * \param fThreshold (0.0f .. 20.0f)
 */
void sculptor_force_algo::setStericGoodThreshold(float fThreshold)
{
    m_oAlgoSema.P();
    m_fStericGoodThreshold = fThreshold;
    m_oAlgoSema.V();
    return;
}

/**
 * get the scale for good contacts
 * \return threshold (0.0f .. 20.0f)
 */
float sculptor_force_algo::getStericGoodThreshold()
{
    return m_fStericGoodThreshold;
}

/**
 * set the sphere color function RED
 * \param fSphereColorFunctionRed (0.0f .. 1.0f)
 */
void sculptor_force_algo::setSphereColorFunctionRed(float fSphereColorFunctionRed)
{
    m_fSphereColorFunctionRed = fSphereColorFunctionRed;
    return;
}
/**
 * get the sphere color function RED
 * \return SphereColorFunctionRed (0.0f .. 1.0f)
 */
float sculptor_force_algo::getSphereColorFunctionRed()
{
    return m_fSphereColorFunctionRed;
}
/**
 * set the sphere color function BLUE
 * \param fSphereColorFunctionRed (0.0f .. 1.0f)
 */
void sculptor_force_algo::setSphereColorFunctionBlue(float fSphereColorFunctionBlue)
{
    m_fSphereColorFunctionBlue = fSphereColorFunctionBlue;
    return;
}
/**
 * get the sphere color function BLUE
 * \return SphereColorFunctionBlue (0.0f .. 1.0f)
 */
float sculptor_force_algo::getSphereColorFunctionBlue()
{
    return m_fSphereColorFunctionBlue;
}
/**
 * set the sphere color function GREEN
 * \param fSphereColorFunctionGreen (0.0f .. 1.0f)
 */
void sculptor_force_algo::setSphereColorFunctionGreen(float fSphereColorFunctionGreen)
{
    m_fSphereColorFunctionGreen = fSphereColorFunctionGreen;
    return;
}
/**
 * get the sphere color function GREEN
 * \return SphereColorFunctionGreen (0.0f .. 1.0f)
 */
float sculptor_force_algo::getSphereColorFunctionGreen()
{
    return m_fSphereColorFunctionGreen;
}
