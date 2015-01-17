#include <svt_optimizer_beads_cc.h>
/**
 * Constructor
 * \param rPointCloud reference to pointcloud object
 * \param rVolume reference to svt_volume object
 */
svt_optimizer_beads_cc::svt_optimizer_beads_cc(svt_point_cloud_pdb< svt_vector4<Real64> > & rProbePdb, svt_volume<Real64>& rVolume, int iRunIndex) : svt_powell(),
    m_oProbePdb(rProbePdb),
    m_oTarVol( rVolume ),
    m_fResolution( 6.0 ),
    m_bFast( false ),
    m_bBlur( true ),
    m_iRunIndex(iRunIndex),
    m_iNoAtoms(0),
    m_pGaussVol(NULL),
    m_fLinkLen( 3.8 ),
    m_fSpringPotential( 0.0 )
{
    unsigned int i;
    // the point at the begining
    for (unsigned int iIndex=0; iIndex< m_oProbePdb.size(); iIndex++)
    {
	for (i=0; i<3; i++)
	{
	    m_aVector.push_back( 0.0 );
	    m_aDirection.push_back( rVolume.getWidth() * 0.25 );
	}
   }

    m_oModelPdb = m_oProbePdb;
    m_oModelVol = m_oTarVol; 

    setPath("");
};

/**
 * update model
 */
void svt_optimizer_beads_cc::updateModel(vector<Real64> &m_aVector)
{
    svt_vector4<Real64> oVec;
    Real64 fDist;

    m_fSpringPotential = 0.0;
    for(unsigned int iAtom=0; iAtom < m_oProbePdb.size(); iAtom++)
    {
	oVec.x( m_aVector[iAtom*3+0] );
	oVec.y( m_aVector[iAtom*3+1] );
	oVec.z( m_aVector[iAtom*3+2] );

	m_oModelPdb[iAtom] = m_oProbePdb[iAtom] + oVec; 
	if (iAtom > 0)
	{
	    fDist = m_oModelPdb[iAtom-1].distance(m_oModelPdb[iAtom]);
	    m_fSpringPotential += (fDist - m_fLinkLen ) *(fDist - m_fLinkLen );
	}
    }
	
    m_fSpringPotential /= (m_oProbePdb.size());
}



/**
 * get model
 */ 
svt_point_cloud_pdb<svt_vector4<Real64 > > & svt_optimizer_beads_cc::getModel()
{
    return  m_oModelPdb;
}

/**
 * get model
 */ 
void svt_optimizer_beads_cc::outputResult()
{
   /* char pFname[1235];

    if (strlen(m_pPath)>0) 
    {
	sprintf(pFname,"%s/Model%04d_run.pdb",m_pPath, m_iRunIndex);
	m_oModelPdb.writePDB(pFname, true);
    }
*/
}


/**
 * Scoring function, please overload!
 */
Real64 svt_optimizer_beads_cc::score(vector<Real64> m_aVector)
{
    updateModel(m_aVector);
/*
    svtout << " Params:" ;   
    for (unsigned int iIndex=0; iIndex < m_aVector.size(); iIndex++)
	cout << m_aVector[iIndex] << " ";
    cout << endl; 
  */
    if (m_pGaussVol==NULL)
    {
	m_pGaussVol = new svt_volume<Real64>;
	m_pGaussVol->createSitusBlurringKernel(m_oTarVol.getWidth(), m_fResolution);
    }

    //Real64 fScore = getRMSD( );
   Real64 fScore = getCorr( ) + m_fSpringPotential*m_fSpringWeight;


   //cout << "Score: " << fScore << endl;
    return fScore;
}

/**
 * set path
 * \param the path
 */
void svt_optimizer_beads_cc::setPath(const char *pPath)
{
    strcpy(m_pPath, pPath);
};

/**
 * set target Pdb
 * \param the target
 */
void svt_optimizer_beads_cc::setTarPdb(svt_point_cloud_pdb< svt_vector4<Real64> > rTarPdb )
{
    m_oTarPdb = rTarPdb;
};

/**
 * get Msd between model and target only for a atoms considered m_iLine/3+1
 */
Real64 svt_optimizer_beads_cc::getRMSD( )
{
    Real64 fMSD = 0;
    Real64 fError;
    for (unsigned int iIndex = 0; iIndex < m_oProbePdb.size(); iIndex++)
    {
	fError = m_oModelPdb[iIndex].distanceSq(m_oTarPdb[iIndex]);
        fMSD += fError;
    }
   
//    for (unsigned int iIndex = m_iNoAtoms; iIndex < m_oProbePdb.size(); iIndex++)
//	fMSD+= 1000;//m_oError[iIndex];
    
    fMSD /= Real64(m_oProbePdb.size());

    return sqrt(fMSD);
}

/**
 * set current line 
 * \param iLine the line number
 */
void svt_optimizer_beads_cc::setLine(int iLine)
{
    svt_powell::setLine(iLine);
    m_iNoAtoms = m_iLine/2 + 2;
    //m_iNoAtoms = m_iLine/3 + 1;
};

/**
 * get Correlation
 */
Real64 svt_optimizer_beads_cc::getCorr()
{
    m_oModelVol.setValue(0.0);
    m_oModelPdb.projectMass( &m_oModelVol, svt_matrix4<Real64>(), false, m_oProbePdb.size());

    //m_oModelVol.convolve( *m_pGaussVol, false );
    Real64 fScore = -m_oTarVol.correlation( m_oModelVol, false ); 
    
    return fScore;
};
/**
 * set the weight of the spring potentianl 
 */
void svt_optimizer_beads_cc::setSpringWeight(Real64 fSpringWeight)
{
    m_fSpringWeight = fSpringWeight;
};



