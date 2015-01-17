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

#ifndef SENSITUS_FORCE_ALGO_H
#define SENSITUS_FORCE_ALGO_H

// svt includes
#include <svt_basics.h>
#include <svt_matrix4f.h>
#include <svt_vector4f.h>
//#include <GL/gl.h>
#include <svt_matrix4.h>
#include <svt_vector4.h>
#include <svt_adv_force_tool.h>
#include <svt_adv_force_calc.h>
#include <svt_semaphore.h>
#include <svt_point_cloud.h>
// sculptor includes
#include <pdb_document.h>
#include <situs_document.h>
// clib includes
#include <stdio.h>

/**
 * Force Algo
 */
class sculptor_force_algo : public svt_adv_force_calc
{
protected:

    // the force output tool
    svt_adv_force_tool* m_pTool;

    // Flag if force algo is initialized
    bool m_bForceAlgoInitialized;

    // Flag if we have exhaustive search data
    bool m_bExhaustiveSearchDataLoaded;

    // Probe and target documents
    sculptor_document* m_pProbeDoc;
    sculptor_document* m_pTargetDoc;

    // the calculated correlation
    double m_dCorrelation;

    // are we at a local maximum?
    bool m_bAtLocalMaximum;

    // force arrow
    bool  m_bForceArrowVisible;
    float m_fForceArrowScale;

    // the calculated force and torque vectors
    svt_vector4<double> m_oForceVec;
    svt_vector4<double> m_oTorqueVec;
    svt_matrix4<double> m_oTransMatrix;

    // one can turn off one or more force axises
    bool m_bForceTransX;
    bool m_bForceTransY;
    bool m_bForceTransZ;
    bool m_bForceRotX;
    bool m_bForceRotY;
    bool m_bForceRotZ;

    // scaling factors
    float m_fCorrForceScale;
    float m_fStericForceScale;
    float m_fRotForceScale;
    float m_fCentralSphereScale;

    // Steric clashing threshold
    float m_fStericClashThreshold;

    // Steric "good" threshold
    float m_fStericGoodThreshold;

    // Color function of central sphere
    float m_fSphereColorFunctionRed;
    float m_fSphereColorFunctionBlue;
    float m_fSphereColorFunctionGreen;

    // Var to remember the mouse mode
    unsigned int m_iSavedMouseMode;
    // Var to remember if the steric clashing code ever ran
    bool m_bStericClashingRun;

    svt_semaphore m_oAlgoSema;

    unsigned long m_iLastScreenUpdateTime;

public:

    /**
     * Constructor
     */
    sculptor_force_algo(svt_adv_force_tool* pTool);
    virtual ~sculptor_force_algo();

    /**
     * calculate the forces
     */
    virtual void calculateForces();

    /**
     * update the display
     */
    void updateDisplay();

    /**
     * Driver for calculating the actual forces
     */
    void calcForcesDriver();

    /**
     * calculate the forces over codebook vectors
     * \param cv codebook vector
     * \param inner true, if codebook vector is inner vector from laplace data
     */
    void calcForcesOverCV(svt_point_cloud<svt_vector4<Real64> >& rCV, bool inner=false);
    /**
     * calculate the forces by the help of the codebookvectors
     */
    void calcForcesCV();

    /**
     * calculate the forces from Eliquos output
     */
    void calcForcesEli();
    /**
     * rotate pdb node around a axis
     */
    void rotPdbEuler(svt_matrix4<double>& rMat, double psi, double theta, double phi);
    /**
     * Interpolate force
     */
    double interpolateForce(double center, double width,
			    double y1, double y2, double y3,
			    double coord);

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
    svt_vector4<double> calcGradient(int iVoxelX, int iVoxelY, int iVoxelZ, float fCVX, float fCVY, float fCVZ, float fVoxelWidth);
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
    double calcCorrelation(int iVoxelX, int iVoxelY, int iVoxelZ, float fCVX, float fCVY, float fCVZ, float fVoxelWidth);

    /**
     * perform steric clashing calculation
     */
    void calcStericClashing();

    /**
     * Make all individual steric spheres invisible
     * (as opposed to turning off the container)
     */
    void makeStericSpheresInvisible();

    /**
     * Update central sphere color and size
     */
    void updateCentralSphere();

    /**
     * Update central sphere highlight
     */
    void updateCentralSphereHighlight();

    /**
     * Update central text
     */
    void updateCentralText();

    /**
     * Update force arrow
     */
    void updateForceArrow();

    /**
     * Normalize forces to 0-100%
     */
    void normalizeForces();

    /**
     * Initialize the force algo object and start force feedback calculations
     */
    void initialize(sculptor_document* pProbeDoc, sculptor_document* pTargetDoc);
    /**
     * Turn force feedback off again
     */
    void deactivate();
    /**
     * Force feedback currently initialized?
     */
    bool isInitialized();

    /**
     * Reset probe
     */
    void resetProbe();
    /**
     * Reset target
     */
    void resetTarget();

    /**
     * Get current correlation
     */
    double getCorrelation();

    /**
     * en/disable the translational forces along the x axis
     */
    void setForceTransX(bool bForceTransX);
    /**
     * get en/disable state of the translational forces along the x axis
     */
    bool getForceTransX();
    /**
     * en/disable the translational forces along the y axis
     */
    void setForceTransY(bool bForceTransY);
    /**
     * get en/disable state of the translational forces along the y axis
     */
    bool getForceTransY();
    /**
     * en/disable the translational forces along the z axis
     */
    void setForceTransZ(bool bForceTransZ);
    /**
     * get en/disable state of the translational forces along the z axis
     */
    bool getForceTransZ();

    /**
     * en/disable the rotational forces along the x axis
     */
    void setForceRotX(bool bForceRotX);
    /**
     * get en/disable state of the rotational forces along the x axis
     */
    bool getForceRotX();
    /**
     * en/disable the rotational forces along the y axis
     */
    void setForceRotY(bool bForceRotY);
    /**
     * get en/disable state of the rotational forces along the y axis
     */
    bool getForceRotY();
    /**
     * en/disable the rotational forces along the z axis
     */
    void setForceRotZ(bool bForceRotZ);
    /**
     * get an/disable state of the rotational forces along the z axis
     */
    bool getForceRotZ();

    /**
     * scale the correlation forces
     * \param fScale scale factor (0.0f .. 1.0f)
     */
    void setCorrForceScale(float fScale);
    /**
     * get the scale for the correlation forces
     * \return scale factor (0.0f .. 1.0f)
     */
    float getCorrForceScale();
    /**
     * scale the steric forces
     * \param fScale scale factor (0.0f .. 1.0f)
     */
    void setStericForceScale(float fScale);
    /**
     * get the scale for the steric forces
     * \return scale factor (0.0f .. 1.0f)
     */
    float getStericForceScale();
    /**
     * scale the central sphere diameter
     * \param fScale scale factor (0.0f .. 1.0f)
     */
    void setCentralSphereScale(float fScale);
    /**
     * get the scale for central sphere
     * \return scale factor (0.0f .. 1.0f)
     */
    float getCentralSphereScale();
    /**
     * scale the force arrow
     * \param fScale scale factor (0.0f .. 1.0f)
     */
    void setForceArrowScale(float fScale);
    /**
     * get the scale for the force arrow
     * \return scale factor (0.0f .. 1.0f)
     */
    float getForceArrowScale();
    /**
     * set force arrow visibility
     * \param bVisible
     */
    void setForceArrowVisible(bool bVisible);
    /**
     * get the force arrow visibility
     * \return bVisible
     */
    bool getForceArrowVisible();
    /**
     * Set the threshold for steric clashing
     * \param fThreshold (0.0f .. 20.0f)
     */
    void setStericClashThreshold(float fThreshold);
    /**
     * get the steric clashing threshold
     * \return threshold (0.0f .. 20.0f)
     */
    float getStericClashThreshold();
    /**
     * Set the threshold for "good" contacts
     * \param fThreshold (0.0f .. 20.0f)
     */
    void setStericGoodThreshold(float fThreshold);
    /**
     * get the "good" contact threshold
     * \return threshold (0.0f .. 20.0f)
     */
    float getStericGoodThreshold();

    /**
     * scale the rotational forces
     * \param fScale scale factor (0.0f .. 1.0f)
     */
    void setRotForceScale(float fScale);
    /**
     * get the rotational scale for the forces
     * \return scale factor (0.0f .. 1.0f)
     */
    float getRotForceScale();

    /**
     * set the sphere color function RED
     * \param fSphereColorFunctionRed (0.0f .. 1.0f)
     */
    void setSphereColorFunctionRed(float fSphereColorFunctionRed);
    /**
     * get the sphere color function RED
     * \return SphereColorFunctionRed (0.0f .. 1.0f)
     */
    float getSphereColorFunctionRed();
    /**
     * set the sphere color function BLUE
     * \param fSphereColorFunctionRed (0.0f .. 1.0f)
     */
    void setSphereColorFunctionBlue(float fSphereColorFunctionBlue);
    /**
     * get the sphere color function BLUE
     * \return SphereColorFunctionBlue (0.0f .. 1.0f)
     */
    float getSphereColorFunctionBlue();
    /**
     * set the sphere color function GREEN
     * \param fSphereColorFunctionGreen (0.0f .. 1.0f)
     */
    void setSphereColorFunctionGreen(float fSphereColorFunctionRed);
    /**
     * get the sphere color function GREEN
     * \return SphereColorFunctionGreen (0.0f .. 1.0f)
     */
    float getSphereColorFunctionGreen();

private:

    /**
     * Determine if current position is a local maximum
     * \return true if local maximum
     */
    bool atLocalMaximum(svt_volume<Real64>* pArray, double dVal, int iX, int iY, int iZ);

};

#endif
