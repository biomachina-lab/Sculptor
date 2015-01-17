/***************************************************************************
                          svt_powell_saxs
			  -------------
    begin                : 05/28/2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_POWELL_SAXS_H
#define SVT_POWELL_SAXS_H

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_powell.h>
#include <svt_point_cloud.h>
#include <svt_time.h>
#include <svt_pair_distribution.h>
#include <svt_flexing_inverse_dist_weighting.h>

/**
 * This is a special powell maximization of the pair distribution distance between
 * two SAXS profiles
 */
class DLLEXPORT svt_powell_saxs : public svt_powell
{
protected:
    
    svt_point_cloud_pdb< svt_vector4<Real64> >& m_rPointCloud;
    svt_point_cloud< svt_vector4<Real64> >& m_rCv;
    vector<unsigned int>* m_pSelCv;
    svt_pair_distribution &m_rPdf;
    
    Real64 m_fInitialDirection;
    
    //filename to output flex model
    char m_pFNameM[1256];
    char m_pFNameCv[1256];
    
    //PDFs
    svt_pair_distribution m_oPDF, m_oNotSelAtomPDF, m_oSel2NotSelAtomPDF, m_oNotSel2SelAtomPDF, m_oSelAtomPDF;

public:
    /**
     * Constructor
     * \param rPointCloud reference to pointcloud object
     * \param rCv reference to codebook vectors
     * \param aSelCv vector containing the CV selected for optimization: not all vectors optimized.
     * \param rPdf reference to pdf
     */
    svt_powell_saxs(svt_point_cloud_pdb< svt_vector4<Real64> >& rPointCloud, svt_point_cloud< svt_vector4<Real64> >& rCv, svt_pair_distribution &rPdf, char* pFName, char* pFNameCv,  vector<unsigned int>* pSelCv=NULL) : svt_powell(),
	m_rPointCloud( rPointCloud ),
        m_rCv( rCv ),
        m_pSelCv( pSelCv ),
        m_rPdf( rPdf ),
        m_fInitialDirection( 0.01 )
    {
        unsigned int iIndex;
        
        if (pSelCv==NULL) // add all Cv
        {
            for (iIndex=0; iIndex < m_rCv.size(); iIndex++)
            {
                // initial transformation = id 
                m_aVector.push_back( 0.0 ); //x
                m_aVector.push_back( 0.0 ); //y
                m_aVector.push_back( 0.0 ); //z
                
                // initial search dir = x
                m_aDirection.push_back( m_fInitialDirection );
                m_aDirection.push_back( m_fInitialDirection );
                m_aDirection.push_back( m_fInitialDirection );
            }
        }
        else
        {
            for (iIndex=0; iIndex < m_pSelCv->size(); iIndex++)
            {
                if ((*m_pSelCv)[iIndex]>0 && (*m_pSelCv)[iIndex]< m_rCv.size())
                {
                    // initial transformation = id 
                    m_aVector.push_back( 0.0 ); //x
                    m_aVector.push_back( 0.0 ); //y
                    m_aVector.push_back( 0.0 ); //z
                    
                    // initial search dir = x
                    m_aDirection.push_back( m_fInitialDirection );
                    m_aDirection.push_back( m_fInitialDirection );
                    m_aDirection.push_back( m_fInitialDirection );
                }
                else
                {
                    svtout << "Error: Wrong selection of the Cv!" << endl;
                    return;
                }
            }
        }
        
        if (pFName!=NULL)
        {
            strcpy( m_pFNameM, pFName);
            strcpy( m_pFNameCv, pFNameCv);
            
            m_rPointCloud.writePDB( m_pFNameM );
            m_rCv.writePDB( m_pFNameCv );
        }
        
        m_oNotSelAtomPDF = rPointCloud.getSelectionPDF( m_rPdf.getWidth(), false, false);
        
        //FIXME: here check how many atoms are selected: if non selected then select all;
    };
    
    virtual ~svt_powell_saxs()
    {
    };

    /**
     * Scoring function, calculates Mean Square deviation of PDF difference
     */
    virtual double score(vector<double> aVec)
    {
        Real64 fMSD = 0;
        int iTime = svt_getElapsedTime();
        
        // 1) flex the structure
        svt_point_cloud< svt_vector4<Real64> > oCvNew;
        oCvNew = m_rCv;
        
        svt_vector4<Real64> *oVec;
        for (unsigned int iIndex=0; iIndex < (*m_pSelCv).size(); iIndex++)
        {
            oVec = new svt_vector4<Real64>( m_rCv[ (*m_pSelCv)[iIndex] ].x()+aVec[iIndex*3], 
                                            m_rCv[ (*m_pSelCv)[iIndex] ].y()+aVec[iIndex*3+1], 
                                            m_rCv[ (*m_pSelCv)[iIndex] ].z()+aVec[iIndex*3+2] );
            oCvNew.setPoint( (*m_pSelCv)[iIndex], *oVec );
        }
        
        svt_flexing_inverse_dist_weighting* oInterpol = new svt_flexing_inverse_dist_weighting( m_rPointCloud, m_rCv, oCvNew, false,"Cartesian","none","Pure", 4.0);
        svt_point_cloud_pdb< svt_vector4<Real64> >& oFlexModel  = oInterpol->interpolate();
        oFlexModel.writePDB( m_pFNameM, true );
        oCvNew.writePDB( m_pFNameCv, true );
        
        // 2) get PDF
        m_oSelAtomPDF = oFlexModel.getSelectionPDF( m_rPdf.getWidth(), true, true );
        m_oSel2NotSelAtomPDF = oFlexModel.getSelectionPDF( m_rPdf.getWidth(), true, false );
        m_oNotSel2SelAtomPDF = oFlexModel.getSelectionPDF( m_rPdf.getWidth(), false, true );
        
        m_oPDF = m_oNotSelAtomPDF + m_oSelAtomPDF + m_oSel2NotSelAtomPDF + m_oNotSel2SelAtomPDF;
        m_oPDF.normalize();
        
        // 3) get score
        fMSD = m_rPdf.distanceSq( m_oPDF );
        svtout << "Time: " << (svt_getElapsedTime()-iTime)/1000.0 << " Score: " << fMSD << endl;
        
        
        delete oInterpol;
        delete oVec;
        return fMSD;
    };

};

#endif
