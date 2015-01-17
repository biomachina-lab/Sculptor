/***************************************************************************
                          svt_powell_vol
			  -------------
    begin                : 04/20/2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_POWELL_VOL_H
#define SVT_POWELL_VOL_H

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
class DLLEXPORT svt_powell_vol : public svt_powell
{
protected:    
    //the target volume
    svt_volume<Real64>& m_rTar;    

    //the probe volume
    svt_volume<Real64>& m_rProbe;    

    //an initial matrix 
    svt_matrix4<Real64>& m_rProbeTrans;

    //the model volume
    svt_volume<Real64> m_oModelVol;
     
public:
    /**
     * Constructor
     * \param rVolume reference to svt_volume object
     * \param rProbe reference to svt_volume object
     */
    svt_powell_vol(svt_volume<Real64>& rTar, svt_volume<Real64>& rProbe, svt_matrix4<Real64> rProbeTrans) : svt_powell(),
        m_rTar( rTar ),
        m_rProbe( rProbe ),
        m_rProbeTrans( rProbeTrans )
    {
        int i;
        // initial transformation = id
        for (i=0; i<6; i++)
            m_aVector.push_back( 0.0 );
        
        // initial search dir = x
        for (i=0; i<3; i++)
            m_aDirection.push_back( rTar.getWidth() * 0.25 );
        
        for (i=0; i<3; i++)
            m_aDirection.push_back( deg2rad( 10.0 ) );
	    
    	m_oModelVol = m_rTar;
    };
    
    virtual ~svt_powell_vol() {};

    /**
     * get matrix
     */
    svt_volume<Real64>& getVolume()
    {
        svtout << m_aVector[0] << " " <<  m_aVector[1] << " " << m_aVector[2] << " " << m_aVector[3] << " " << m_aVector[4] << " " << m_aVector[5] << " " << endl;
	return m_oModelVol;
    };
    
    /**
     * Normalize the euler angles
     */
    void normalizeEulerAngles(vector<double> &aVec)
    {
        double fNewPsi, fNewTheta, fNewPhi;

        double fPsi   = rad2deg( aVec[3] );
        double fTheta = rad2deg( aVec[4] );
        double fPhi   = rad2deg( aVec[5] );
		

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
		
        aVec[3] = deg2rad( fNewPsi );
        aVec[4] = deg2rad( fNewTheta ) ;
        aVec[5] = deg2rad( fNewPhi );
    };
    
    /**
     * Scoring function, calculates CC
     */
    virtual double score(vector<double> &aVec)
    {
        // compute matrix 
	svt_matrix4<Real64> oMat;
        oMat.rotatePTP( aVec[3], aVec[4], aVec[5] );
        oMat.setTranslation(aVec[0],aVec[1], aVec[2]);

        //apply transformation matrix
        m_oModelVol.setValue(0.0);
        m_rProbe.applyTransformation(m_oModelVol, m_rProbeTrans*oMat);	
        Real64 fCorr = -m_rTar.correlation(m_oModelVol);

        normalizeEulerAngles( aVec );

        return fCorr;
    };
};

#endif
