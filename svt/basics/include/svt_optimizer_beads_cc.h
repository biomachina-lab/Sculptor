/***************************************************************************
                          svt_optimizer_beads_cc
                          ---------
    begin                : 03/24/2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_OPTIMIZER_BEADS_CC
#define __SVT_OPTIMIZER_BEADS_CC

#include <svt_powell.h>
#include <svt_point_cloud_pdb.h>
#include <string.h>
#include <stdio.h>

//#include <svt_optimizer.h>

/**
 * an optimizer that will refine the placed of a string of beads inside a map
 * \author Mirabela Rusu
 */
class DLLEXPORT svt_optimizer_beads_cc : public svt_powell
{
protected:
    // the initial pdb
    svt_point_cloud_pdb<svt_vector4< Real64 > > m_oProbePdb;

    //the model 
    svt_point_cloud_pdb<svt_vector4<Real64 > > m_oModelPdb;

    // the target pdb
    svt_point_cloud_pdb<svt_vector4< Real64 > > m_oTarPdb;
    
    //the target map
    svt_volume<Real64> m_oTarVol;
    
    //the model map
    svt_volume<Real64> m_oModelVol;

    //the resolution of the target volume
    Real64 m_fResolution;
    bool m_bFast;
    bool m_bBlur;

    //path to results
    char m_pPath[1256];
    
    //index
    int m_iRunIndex;

    //Atoms
    int m_iNoAtoms;

    //gaussian blurring kernel
    svt_volume<Real64> * m_pGaussVol;
	

    // the allowed lenght of one link;
    Real64 m_fLinkLen;


    //spring
    Real64 m_fSpringPotential;
    
    //springWeight
    Real64 m_fSpringWeight;

public:

    /**
     * Constructor
     * \param rPointCloud reference to pointcloud object
     * \param rVolume reference to svt_volume object
     */
    svt_optimizer_beads_cc(svt_point_cloud_pdb< svt_vector4<Real64> > & rProbePdb, svt_volume<Real64>& rVolume, int iRunIndex=0);

    /**
     * Destructor
     */
    virtual ~svt_optimizer_beads_cc()
    {
    };

    /**
     * update model
     */
    void updateModel(vector<Real64> &m_aVector);

    /**
     * get model
     */ 
    svt_point_cloud_pdb<svt_vector4<Real64 > > & getModel();
 
    /**
     * Scoring function, please overload!
     */
    virtual Real64 score(vector<Real64>);

    /**
     * Outputs some sort of results; please overload
     */
    virtual void outputResult();
    
    /**
     * set path
     * \param the path
     */
    void setPath(const char *pPath);

    /**
     * set target Pdb
     * \param the target
     */
    void setTarPdb(svt_point_cloud_pdb< svt_vector4<Real64> > rTarPdb );

    /**
     * get rmsd between model and target only for a atoms considered m_iLine/3+1
     */
    Real64 getRMSD( );
    
    /**
     * set current line 
     * \param iLine the line number
     */
    virtual void setLine(int iLine);

    /**
     * get Correlation
     */
    Real64 getCorr();

    /**
     * set the weight of the spring potentianl 
     */
    void setSpringWeight(Real64 fSpringWeight);


};
#endif
