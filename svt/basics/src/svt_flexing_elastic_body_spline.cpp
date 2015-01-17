/***************************************************************************
                          svt_flexing_elastic_body_spline
                          --------------------------------------
    begin                : 10/01/2006
    last update          : 10/04/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_flexing_elastic_body_spline.h>

#define FLEX cout << svt_trimName("flexing")


/**
 * Constructor
 */
 
 svt_flexing_elastic_body_spline::svt_flexing_elastic_body_spline(
    const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
    const svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks,
    const svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks,
    bool bFlexAllAtoms, string pDistType, string iDistInGraphSchema,
    unsigned int iBasisFunctionType, Real64 fElasticity ): 
    svt_flexing_spline(oOrigConf,oOrigConfCodebooks,oFinalConfCodebooks, bFlexAllAtoms, pDistType, iDistInGraphSchema,iBasisFunctionType)
{

    m_fElasticity = fElasticity;
    
    //constant variables
    m_iKernelSize = 3;
    setBasisFunctionType(iBasisFunctionType);
    
    // choose size
    m_oKernelsPoints.resize(m_iKernelSize*(m_oOrigConfCodebooks.size()+4),
			    m_iKernelSize*(m_oOrigConfCodebooks.size()+4));//3N+12 X 3N+12
    m_oDisplacements.resize(m_iKernelSize*(m_oOrigConfCodebooks.size()+4),1);
    m_oCoefs.resize(m_iKernelSize*(m_oOrigConfCodebooks.size()+4),1);
    m_oKernel.resize(m_iKernelSize,m_iKernelSize);
    m_oDisplacement.resize(m_iKernelSize,1);
    m_oCoef.resize(m_iKernelSize,1);
 
}

/**
 * Destructor
 */
svt_flexing_elastic_body_spline::~svt_flexing_elastic_body_spline()
{
}

/**
 * Initialize the kernel matrix. This matrix has different values for each couple of codebook vectors
 * \param iIndexCdbkSrc represent the index of the first codebook 
 * \param iIndexCdbkDest indicates the index of the second codebook
 */
void svt_flexing_elastic_body_spline::initKernelCdbk2Cdbk(unsigned int iIndexCdbkSrc, unsigned int iIndexCdbkDest)
{
    Real64 fDistSrc2Dest;
    svt_matrix<Real64> oDisp, oTransDisp, oIdentity;
    
    //init constant
    fDistSrc2Dest = m_oOrigConfCodebooks[iIndexCdbkSrc].distance(m_oOrigConfCodebooks[iIndexCdbkDest]);
    
    //init matrices
    oDisp.resize(3,1);
    oTransDisp.resize(1,3);
    oIdentity.resize(3,3);
    
    //erase previous values of the kernel 
    m_oKernel = 0;
    oIdentity = 0;
    
    // init on x
    unsigned int iIndex = 0;
    oDisp[iIndex][0] = m_oOrigConfCodebooks[iIndexCdbkSrc].x()-m_oOrigConfCodebooks[iIndexCdbkDest].x();
    oTransDisp[0][iIndex] = oDisp[iIndex][0];
    oIdentity[iIndex][iIndex] = 1;
    
    //init on y
    iIndex = 1;
    oDisp[iIndex][0] = m_oOrigConfCodebooks[iIndexCdbkSrc].y()-m_oOrigConfCodebooks[iIndexCdbkDest].y();
    oTransDisp[0][iIndex] = oDisp[iIndex][0];
    oIdentity[iIndex][iIndex] = 1;
    
    //init on z
    iIndex = 2;
    oDisp[iIndex][0] = m_oOrigConfCodebooks[iIndexCdbkSrc].z()-m_oOrigConfCodebooks[iIndexCdbkDest].z();
    oTransDisp[0][iIndex] = oDisp[iIndex][0];
    oIdentity[iIndex][iIndex] = 1;
    
    switch(m_iBasisFunctionType)
    {
	case 1:
            // oIdentity is not any more identity 
	    oIdentity *= m_fCnstAlpha;
	    oIdentity *= pow(fDistSrc2Dest,3); 
    
            // oDisp is not any more the displacement
	    oDisp *= oTransDisp;
	    oDisp *= 3*fDistSrc2Dest;
        break;
	case 2:
	    // oIdentity is not any more identity 
	    oIdentity *= m_fCnstAlpha;
	    oIdentity *= fDistSrc2Dest; 
    
            // oDisp is not any more the displacement
	    oDisp *= oTransDisp;
	    if (fDistSrc2Dest == 0) 
	    {
		char pErrorMesage[80];
		sprintf(pErrorMesage,"The two codebook vectors %d and %d are superposed",iIndexCdbkSrc,iIndexCdbkDest);
		m_oKernel = 0;
		throw svt_exception(pErrorMesage);
	    }else
	    {
		oDisp *= 1/fDistSrc2Dest;
	    }
	    break;
	default:
	    // oIdentity is not any more identity 
	    oIdentity *= m_fCnstAlpha;
	    oIdentity *= fDistSrc2Dest; 
    
            // oDisp is not any more the displacement
	    oDisp *= oTransDisp;
	    if (fDistSrc2Dest == 0) 
	    {
		char pErrorMesage[80];
		sprintf(pErrorMesage,"The two codebook vectors %d and %d are superposed",iIndexCdbkSrc,iIndexCdbkDest);
		m_oKernel = 0;
		throw svt_exception(pErrorMesage);
	    }else
	    {
		oDisp *= 1/fDistSrc2Dest;
	    }
	    break;
    }
    
    m_oKernel = oIdentity - oDisp;
 
}

/**
 * Initialize the kernel matrix. This matrix has different values for each couple of atom - codebook vector
 * \param iIndexAtom represent the index of the Atom 
 * \param iIndexCdbk indicates the index of the codebook
 */
void svt_flexing_elastic_body_spline::initKernelAtom2Cdbk(unsigned int iIndexAtom, unsigned int iIndexCdbk)
{
    Real64 fDistAtom2Cdbk;
    svt_matrix<Real64> oDisp, oTransDisp, oIdentity;
    
    //init constant
    fDistAtom2Cdbk = m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCdbk];
	    
    //init matrices
    oDisp.resize(3,1);
    oTransDisp.resize(1,3);
    oIdentity.resize(3,3);
    
    //erase previous values of the kernel 
    m_oKernel = 0;
    oIdentity = 0;
    
    // init on x
    unsigned int iIndex = 0;
    oDisp[iIndex][0] = m_oOrigConf[iIndexAtom].x()-m_oOrigConfCodebooks[iIndexCdbk].x();
    oTransDisp[0][iIndex] = oDisp[iIndex][0];
    oIdentity[iIndex][iIndex] = 1;
    
    //init on y
    iIndex = 1;
    oDisp[iIndex][0] = m_oOrigConf[iIndexAtom].y()-m_oOrigConfCodebooks[iIndexCdbk].y();
    oTransDisp[0][iIndex] = oDisp[iIndex][0];
    oIdentity[iIndex][iIndex] = 1;
    
    //init on z
    iIndex = 2;
    oDisp[iIndex][0] = m_oOrigConf[iIndexAtom].z()-m_oOrigConfCodebooks[iIndexCdbk].z();
    oTransDisp[0][iIndex] = oDisp[iIndex][0];
    oIdentity[iIndex][iIndex] = 1;
    
    switch(m_iBasisFunctionType)
    {
	case 1:
	    // oIdentity is not any more identity 
	    oIdentity *= m_fCnstAlpha;
	    oIdentity *= pow(fDistAtom2Cdbk,3); 
    
    // oDisp is not any more the displacement
	    oDisp *= oTransDisp;
	    oDisp *= 3*fDistAtom2Cdbk;
	    break;
	case 2:
	    // oIdentity is not any more identity 
	    oIdentity *= m_fCnstAlpha;
	    oIdentity *= fDistAtom2Cdbk; 
    
            // oDisp is not any more the displacement
	    oDisp *= oTransDisp;
    
	    if (fDistAtom2Cdbk == 0) 
	    {
		char pErrorMesage[80];
		sprintf(pErrorMesage,"The Atom %d an the codebook vector %d are superposed",iIndexAtom,iIndexCdbk);
		throw svt_exception(pErrorMesage);
	    }
	    oDisp *= 1/fDistAtom2Cdbk;
	    
	    break;
	default:
	    // oIdentity is not any more identity 
	    oIdentity *= m_fCnstAlpha;
	    oIdentity *= fDistAtom2Cdbk; 
    
            // oDisp is not any more the displacement
	    oDisp *= oTransDisp;
    
	    if (fDistAtom2Cdbk == 0) 
	    {
		char pErrorMesage[80];
		sprintf(pErrorMesage,"The Atom %d an the codebook vector %d are superposed",iIndexAtom,iIndexCdbk);
		throw svt_exception(pErrorMesage);
	    }
	    oDisp *= 1/fDistAtom2Cdbk;
	    break;
    }
    
    
    m_oKernel = oIdentity - oDisp;
}

/**
 * Initialize the displacement of one codebook vector
 * \param iIndexCdbk represent the index of the first codebook 
 */
void svt_flexing_elastic_body_spline::initDisplacement(unsigned int iIndexCdbk)
{
    unsigned int iIndex;
    
    //x
    iIndex = 0;
    m_oDisplacement[iIndex][0] = m_oFinalConfCodebooks[iIndexCdbk].x() - m_oOrigConfCodebooks[iIndexCdbk].x();
    
    //y
    iIndex = 1;
    m_oDisplacement[iIndex][0] = m_oFinalConfCodebooks[iIndexCdbk].y() - m_oOrigConfCodebooks[iIndexCdbk].y();
    
    //z
    iIndex = 2;
    m_oDisplacement[iIndex][0] = m_oFinalConfCodebooks[iIndexCdbk].z() - m_oOrigConfCodebooks[iIndexCdbk].z();
}

/**
 * Initialize the coefs of one codebook vector
 * \param iIndexCdbk represent the index of the first codebook 
 */
void svt_flexing_elastic_body_spline::initCoef(unsigned int iIndexCdbk)
{
    for (unsigned int iIndex=0; iIndex < m_iKernelSize; iIndex++)
	m_oCoef[iIndex][0] = m_oCoefs[iIndexCdbk*m_iKernelSize+iIndex][0];
    
}

/**
 * set computed displacement to atoms coordinates
 * \param oDisplacement computed displacement
 * \param iIndexAtom represent the index of the atom 
 */
void svt_flexing_elastic_body_spline::setDisplacement(svt_matrix<Real64> oDisplacement,unsigned int iIndexAtom)
{
    svt_vector4 <Real64> oDisp(oDisplacement[0][0],oDisplacement[1][0],oDisplacement[2][0]);
    
    m_oFlexedConf[iIndexAtom] += oDisp;
    m_oFlexedConf.getAtom(iIndexAtom)->setTempFact(oDisp.length());
}


 /**
 * Interpolate method
 */
svt_point_cloud_pdb< svt_vector4<Real64> >& svt_flexing_elastic_body_spline::interpolate()
{
    m_pDistType="C";
    svt_flexing_spline::computeAtoms2CodebooksDist();
    svt_flexing_spline::interpolate();
    m_oFlexedConf.addRemark(addParameters2Comments());
    
    return m_oFlexedConf; 
}

/**
 * Compute the positions of the flexed structure...
 */
void svt_flexing_elastic_body_spline::computeFlexedConformation()
{
    svt_flexing_spline::computeFlexedConformation();
}

/**
 * Set the values of the elasticity coefficient (which ranges from 0.01 - soft rubber to 0.49 - hard rubber)
 * Also computes alpha - a constant directly depending on the elasticity;
 */
void svt_flexing_elastic_body_spline::setElasticity(Real64 fElasticity)
{
    m_fElasticity = fElasticity;
    switch(m_iBasisFunctionType)
    {
	case 1:
	    m_fCnstAlpha = 12 * (1 - m_fElasticity) - 1;
	    break;
	case 2:
	    m_fCnstAlpha = 8 * (1 - m_fElasticity) - 1;
	    break;
	default:
	    m_fCnstAlpha = 8 * (1 - m_fElasticity) - 1;
	    break;
    }
}

/**
 * Set the type of the Basis Function: TPS    1:r (default); 2:r^2*log(r^2); 3:r^3
 *                                     EBS      
 */
void svt_flexing_elastic_body_spline::setBasisFunctionType(unsigned int iBasisFunctionType)
{
    svt_flexing_spline::setBasisFunctionType(iBasisFunctionType);
    
    switch(m_iBasisFunctionType)
    {
	case 1:
	    m_fCnstAlpha = 12 * (1 - m_fElasticity) - 1;
	    break;
	case 2:
	    m_fCnstAlpha = 8 * (1 - m_fElasticity) - 1;
	    break;
	default:
	    m_fCnstAlpha = 8 * (1 - m_fElasticity) - 1;
	    break;
    }
}

/**
 * Include comments/remarks in the output pdb files
 * \return the comment to add to the pdb structures
 * \see svt_point_cloud_pdb:addRemark(...)
 */ 
string svt_flexing_elastic_body_spline::addParameters2Comments()
{
    string oRemarks = svt_flexing_spline::addParameters2Comments();
    
    oRemarks = "Flexing Method: Elastic Body Spline with basis function/kernel:";
    switch(m_iBasisFunctionType)
    {
	case 1:
	    oRemarks += "G(x)=(alpha*r(x)^2-3*x*x^T)*r(x); " ;
	    break;
	case 2:
	    oRemarks += "G(x)=alpha*r(x)-x*x^T/r(x); ";
	    break;
	default:
	    oRemarks += "G(x)=alpha*r(x)-x*x^T/r(x); ";
	    break;
    }
     
    oRemarks += "where x is a 3D vector and r(x) is its length.\n";
    
    return oRemarks;
}

