/***************************************************************************
                          svt_powell_cc
			  -------------
    begin                : 07/22/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_POWELL_CC_H
#define SVT_POWELL_CC_H

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_powell.h>
#include <svt_point_cloud.h>
#include <svt_time.h>

/**
 * This is a special powell maximization of the cross-correlation between two
 * EM density maps. In this case we only sample the target map at the feature vector
 * locations in order to have an extremely fast solution.
 */
class DLLEXPORT svt_powell_cc : public svt_powell
{
protected:
       //the vector with the units
    vector< svt_point_cloud_pdb< svt_vector4<Real64> > > m_oUnits;
    
    //the model
    svt_point_cloud_pdb<svt_vector4 <Real64> > m_oModel;
    
    //the model volume
    svt_volume<Real64> m_oModelVol;
     
    //the vector with their transformations
    vector< svt_matrix4<Real64> > m_oUnitsMat;
    
    //the target volume
    svt_volume<Real64>& m_rVolume;    
    
    //the transformation of the target volume
    svt_matrix4<Real64>& m_rVolMat;

    bool m_bLaplacian;
    
    //the resolution of the target volume
    Real64 m_fResolution;
    bool m_bFast;
    bool m_bBlur;

    svt_volume<Real64>* m_pGaussVol;
    svt_volume<Real64>* m_pLaplVol;

public:
    /**
     * Constructor
     * \param rPointCloud reference to pointcloud object
     * \param rPointMat reference to svt_matrix object with the initial transformation
     * \param rVolume reference to svt_volume object
     * \param rVolMat reference to svt_matrix object with the initial transformation
     */
    svt_powell_cc(vector< svt_point_cloud_pdb< svt_vector4<Real64> > > oUnits, vector< svt_matrix4<Real64> > oUnitsMat, svt_volume<Real64>& rVolume, svt_matrix4<Real64>& rVolMat) : svt_powell(),
	m_oUnits(oUnits),
	m_oUnitsMat(oUnitsMat),
	m_rVolume( rVolume ),
	m_rVolMat (rVolMat),
	m_bLaplacian( false ),
	m_fResolution( 6.0 ),
        m_bFast( false ),
        m_bBlur( true ),
        m_pGaussVol( NULL ),
        m_pLaplVol( NULL )
    {
	svt_point_cloud_pdb< svt_vector4< Real64> > oPdb;
	svt_matrix4<Real64> oMat;
	
	unsigned int i;
	for (unsigned int iIndex=0; iIndex< m_oUnits.size(); iIndex++)
	{
	    // initial transformation = id
	    for (i=0; i<6; i++)
		m_aVector.push_back( 0.0 );
	    
	    // initial search dir = x
	    for (i=0; i<3; i++)
		m_aDirection.push_back( rVolume.getWidth() * 0.25 );
	    
	    for (i=0; i<3; i++)
		m_aDirection.push_back( deg2rad( 10.0 ) );
	    
    	    oMat = m_oUnitsMat[iIndex];
	    oPdb = m_oUnits[iIndex];
	    oPdb = oMat*oPdb;
	    
	    if (iIndex==0)
		m_oModel = oPdb;
	    else
		m_oModel.append( oPdb );
	}

	m_oModelVol = m_rVolume;
    };
    
    virtual ~svt_powell_cc()
    {
    };

    
    /**
     * get matrix
     */
    vector< svt_matrix4<Real64> > getTrans()
    {
	// compute matrix
	
	vector<svt_matrix4<Real64> > oMats;
	svt_matrix4<Real64> oMat;
	
	for (unsigned int iUnit=0; iUnit<m_oUnits.size(); iUnit++)
	{
	    oMat = m_oUnitsMat[iUnit];
	    oMat.setTranslation( 0.0, 0.0, 0.0 );
	    oMat.rotatePTP( m_aVector[iUnit*6+3], m_aVector[iUnit*6+4], m_aVector[iUnit*6+5] );
	    oMat.setTranslation(
				m_oUnitsMat[iUnit].translationX() + m_aVector[iUnit*6+0],
				m_oUnitsMat[iUnit].translationY() + m_aVector[iUnit*6+1],
				m_oUnitsMat[iUnit].translationZ() + m_aVector[iUnit*6+2]
			    );
			    
	    oMats.push_back(oMat);
	}
	return oMats;
    };
    
    /**
     * Normalize the euler angles
     */
    void normalizeEulerAngles(unsigned int iUnit, vector<double> &aVec)
    {
        double fNewPsi, fNewTheta, fNewPhi;

        double fPsi   = rad2deg( aVec[iUnit*6+3] );
        double fTheta = rad2deg( aVec[iUnit*6+4] );
        double fPhi   = rad2deg( aVec[iUnit*6+5] );
		

        if ( (fPsi   >= 0 && fPsi   <= 360.0) &&
            (fTheta >= 0 && fTheta <= 180.0) &&
            (fPhi   >= 0 && fPhi   <= 360.0)    )
            return;

        /* bring fPsi, fTheta, fPhi, within 2 PI of reference */
        if (fPsi >= 0) fNewPsi = fmod(fPsi,360.0);
        else fNewPsi = 360.0 - fmod(-fPsi,360.0);

        if (fTheta >= 0) fNewTheta = fmod(fTheta,360.0);
        else fNewTheta = 360.0 - fmod(-fTheta,360.0);

        if (fPhi >= 0) fNewPhi = fmod(fPhi,360.0);
        else fNewPhi = 360.0 - fmod(-fPhi,360.0);

        /* if fTheta is not within PI, we use invariant transformations */
        /* and attempt to map to above intervals */
        if (fNewTheta > 180.0) { /* fTheta overflow */
            /* fTheta -> 2 PI - fTheta */
            if (fNewTheta >= 0) fTheta = fmod(fNewTheta,360.0);
            else fTheta = 360.0 - fmod(-fNewTheta,360.0);
            fNewTheta -= 2 * fTheta;

            /* remap to [0, 2 PI] interval */
            fTheta = fNewTheta;
            if (fTheta >= 0) fNewTheta = fmod(fTheta,360.0);
            else fNewTheta = 360.0 - fmod(-fTheta,360.0);

            /* we have flipped fTheta so we need to flip fPsi and fPhi as well
             */
            /* to keep rot-matrix invariant */
            /* fPsi -> fPsi + PI */
            if (fNewPsi > 180.0)
                fNewPsi -= 180.0;
            else
                fNewPsi += 180.0;

            /* fPhi -> fPhi + PI */
            if (fNewPhi > 180.0) fNewPhi -= 180.0;
            else fNewPhi += 180.0;
        }
		
        aVec[iUnit*6+3] = deg2rad( fNewPsi );
        aVec[iUnit*6+4] = deg2rad( fNewTheta ) ;
        aVec[iUnit*6+5] = deg2rad( fNewPhi );
    };
    
    /**
     * Scoring function, calculates CC
     */
    virtual double score(vector<double> &aVec)
    {
	svt_point_cloud_pdb< svt_vector4<Real64> > oUnit;
 	unsigned int iNoAtomsUpdated=0, iSize;
 	Real64 fXa, fYa,fZa;
	svt_matrix4<Real64> oMat;
	
	if (!m_bFast)
	    m_oModelVol.setValue( 0.0f );
	
	//create the model
	for (unsigned int iUnit=0; iUnit < m_oUnits.size(); iUnit++)
	{
	    oMat = m_oUnitsMat[iUnit];
	    
	    // compute matrix 
	    oMat.setTranslation( 0.0, 0.0, 0.0 );
	    oMat.rotatePTP( aVec[iUnit*6+3], aVec[iUnit*6+4], aVec[iUnit*6+5] );
	    
	    oMat.setTranslation(m_oUnitsMat[iUnit].translationX() + aVec[iUnit*6+0],
				m_oUnitsMat[iUnit].translationY() + aVec[iUnit*6+1],
				m_oUnitsMat[iUnit].translationZ() + aVec[iUnit*6+2]  );
	    
	    if (m_bFast) // use CV - then compute model
	    {				
		oUnit = m_oUnits[iUnit];
		iSize = oUnit.size();
	        for (unsigned int iAtom=0; iAtom<iSize; iAtom++)
		{
		    fXa =  oMat[0][0]*oUnit[iAtom].x() + oMat[0][1]*oUnit[iAtom].y() + oMat[0][2]*oUnit[iAtom].z() + oMat[0][3]*oUnit[iAtom].w();
		    fYa =  oMat[1][0]*oUnit[iAtom].x() + oMat[1][1]*oUnit[iAtom].y() + oMat[1][2]*oUnit[iAtom].z() + oMat[1][3]*oUnit[iAtom].w();
		    fZa =  oMat[2][0]*oUnit[iAtom].x() + oMat[2][1]*oUnit[iAtom].y() + oMat[2][2]*oUnit[iAtom].z() + oMat[2][3]*oUnit[iAtom].w();
            
		    m_oModel[iNoAtomsUpdated+iAtom].x( fXa );
		    m_oModel[iNoAtomsUpdated+iAtom].y( fYa );
		    m_oModel[iNoAtomsUpdated+iAtom].z( fZa );
		}
		iNoAtomsUpdated += iSize;
	    }else{ // compute only the model
		m_oUnits[iUnit].projectMass(&m_oModelVol, oMat, false);
	    }

	    normalizeEulerAngles( iUnit, aVec );
	}

	Real64 fCorr = 0.0;	
	if (m_bFast)
	{
	    Real64 fGridX = m_rVolume.getGridX();
	    Real64 fGridY = m_rVolume.getGridY();
            Real64 fGridZ = m_rVolume.getGridZ();
	    Real64 fWidth = m_rVolume.getWidth();

	    int iVoxelX, iVoxelY, iVoxelZ;
            svt_vector4<Real64> oVec;

	    // now calculate the force for each codebook vector
	    for(unsigned int i=0; i<m_oModel.size();i++)
	    {
		// get original position
		oVec = m_oModel[i];

                oVec.x( oVec.x() - fGridX );
                oVec.y( oVec.y() - fGridY );
                oVec.z( oVec.z() - fGridZ );

		// get the index of the voxel lying under the cv
		iVoxelX = (int)(floor(oVec.x() / fWidth));
		iVoxelY = (int)(floor(oVec.y() / fWidth));
		iVoxelZ = (int)(floor(oVec.z() / fWidth));

                fCorr += (m_rVolume.getValue( iVoxelX, iVoxelY, iVoxelZ ) * (m_oModel.atom(i).getMass()));
	    }

	    fCorr /= m_oModel.size();
            fCorr = -fCorr;

	} else {

            if (m_bBlur)
            {
                if ( m_pGaussVol == NULL)
                {
                    m_pGaussVol = new svt_volume<Real64>;
                    // 1D kernel
                    m_pGaussVol->create1DBlurringKernel(m_rVolume.getWidth(), m_fResolution);
                    // 3D kernel
                    //m_pGaussVol->createSitusBlurringKernel(m_rVolume.getWidth(), m_fResolution);
                }
                if (m_bLaplacian && m_pLaplVol == NULL)
                {
                    m_pLaplVol = new svt_volume<Real64>;
                    m_pLaplVol->createLaplacian();
                }

                // 1D kernel convolution
                m_oModelVol.convolve1D3D( *m_pGaussVol, false );
                // 3D kernel convolution
                //m_oModelVol.convolve( *m_pGaussVol, false );

                // if in laplacian mode, we still have to convolve with the laplacian kernel
                if (m_bLaplacian)
		    m_oModelVol.convolve( *m_pLaplVol, false);
		
            }
            fCorr = -m_rVolume.correlation(m_oModelVol, false);
	}

        return fCorr;
    };

    /**
     * Set laplcian mode.
     * \param bLaplacian if true, a laplacian filter is applied before calculating the correlation in the scoring function
     */
    void setLaplacian( bool bLaplacian )
    {
	m_bLaplacian = bLaplacian;
    };
    /**
     * Get laplcian mode.
     * \return if true, a laplacian filter is applied before calculating the correlation in the scoring function
     */
    bool getLaplacian()
    {
	return m_bLaplacian;
    };

    /**
     * Set fast union sphere mode.
     * \param bFast if true, only a unoin sphere model is evaluated.
     */
    void setFast( bool bFast )
    {
	m_bFast = bFast;
    };
    /**
     * Get fast union sphere mode.
     * \return if true, only a union sphere model is evaluated.
     */
    bool getFast( ) const
    {
	return m_bFast;
    };

    /**
     * Set blur mode.
     * \param bBlur if false, only the point coordinate of the atom is projected onto the lattice.
     */
    void setBlur( bool bBlur )
    {
	m_bBlur = bBlur;
    };
    /**
     * Get blur mode.
     * \return if false, only the point coordinate of the atom is projected onto the lattice.
     */
    bool getBlur( ) const
    {
	return m_bBlur;
    };

    /**
     * Set resolution of target map.
     * \param fResolution resolution of target map.
     */
    void setResolution( Real64 fResolution )
    {
        m_fResolution = fResolution;
    };
    /**
     * Get resolution of target map.
     * \return resolution of target map.
     */
    Real64 getResolution( )
    {
        return m_fResolution;
    };
};

#endif
