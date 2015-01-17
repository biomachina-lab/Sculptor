/***************************************************************************
                          svt_flexing_thin_plate_spline
                          --------------------------------------
    begin                : 10/01/2006
    last update          : 10/01/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_flexing_thin_plate_spline.h>

#define FLEX cout << svt_trimName("flexing")


/**
 * Constructor
 */
 
 svt_flexing_thin_plate_spline::svt_flexing_thin_plate_spline(
        const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
	const svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks,
	const svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks,
	bool bFlexAllAtoms, string pDistType, string iDistInGraphSchema,
	unsigned int iBasisFunctionType ): svt_flexing_spline(oOrigConf,oOrigConfCodebooks,oFinalConfCodebooks, bFlexAllAtoms, pDistType, iDistInGraphSchema, iBasisFunctionType)
{
    unsigned int iDim = 3;
    m_iKernelSize = 1;
    
    
    m_oKernelsPoints.resize(m_iKernelSize*(m_oOrigConfCodebooks.size()+4),
			    m_iKernelSize*(m_oOrigConfCodebooks.size()+4));//3N+12 X 3N+12
    m_oDisplacements.resize(m_iKernelSize*(m_oOrigConfCodebooks.size()+4),iDim);
    m_oCoefs.resize(m_iKernelSize*(m_oOrigConfCodebooks.size()+4),iDim);
    
    m_oKernel.resize(m_iKernelSize,m_iKernelSize);
    m_oDisplacement.resize(m_iKernelSize,iDim);
    m_oCoef.resize(m_iKernelSize,iDim);
}

/**
 * Destructor
 */
svt_flexing_thin_plate_spline::~svt_flexing_thin_plate_spline()
{
}

/**
 * Initialize the kernel matrix. This matrix has different values for each couple of codebook vectors
 * \param iIndexCdbkSrc represent the index of the first codebook 
 * \param iIndexCdbkDest indicates the index of the second codebook
 */
void svt_flexing_thin_plate_spline::initKernelCdbk2Cdbk(unsigned int iIndexCdbkSrc, unsigned int iIndexCdbkDest)
{
    Real64 fDistSrc2Dest;
    
    switch (toupper(m_pDistType[0]))
    {
	case ('C'):
	    fDistSrc2Dest = m_oOrigConfCodebooks[iIndexCdbkSrc].distance(m_oOrigConfCodebooks[iIndexCdbkDest]);
	    break;
	case ('G'):
 	    fDistSrc2Dest = ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).getGraphDists()[iIndexCdbkSrc][iIndexCdbkDest];    
	    break;
	default:
	    fDistSrc2Dest = m_oOrigConfCodebooks[iIndexCdbkSrc].distance(m_oOrigConfCodebooks[iIndexCdbkDest]);
	    break;
	    
    }
    
    m_oKernel = 0;
    for (unsigned int iIndex = 0; iIndex < m_iKernelSize; iIndex++) m_oKernel[iIndex][iIndex] =  basisFunction(fDistSrc2Dest);
}

/**
 * Initialize the kernel matrix. This matrix has different values for each couple of atom - codebook vector
 * \param iIndexAtom represent the index of the Atom 
 * \param iIndexCdbk indicates the index of the codebook
 */
void svt_flexing_thin_plate_spline::initKernelAtom2Cdbk(unsigned int iIndexAtom, unsigned int iIndexCdbk)
{
    Real64 fDistAtom2Cdbk;
    m_oKernel.resize(m_iKernelSize,m_iKernelSize);
    
    //init constant
    fDistAtom2Cdbk = m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCdbk];
    m_oKernel = 0;
    for (unsigned int iIndex = 0; iIndex < m_iKernelSize; iIndex++) m_oKernel[iIndex][iIndex] =  basisFunction(fDistAtom2Cdbk);
}

/**
 * Initialize the displacement of one codebook vector
 * \param iIndexCdbk represent the index of the first codebook 
 */
void svt_flexing_thin_plate_spline::initDisplacement(unsigned int iIndexCdbk)
{
    unsigned int iIndex;
    
    //x
    iIndex = 0;
    m_oDisplacement[0][iIndex] = m_oFinalConfCodebooks[iIndexCdbk].x() - m_oOrigConfCodebooks[iIndexCdbk].x();
    
    //y
    iIndex = 1;
    m_oDisplacement[0][iIndex] = m_oFinalConfCodebooks[iIndexCdbk].y() - m_oOrigConfCodebooks[iIndexCdbk].y();
    
    //z
    iIndex = 2;
    m_oDisplacement[0][iIndex] = m_oFinalConfCodebooks[iIndexCdbk].z() - m_oOrigConfCodebooks[iIndexCdbk].z();
}

/**
 * Initialize the coefs of one codebook vector
 * \param iIndexCdbk represent the index of the first codebook 
 */
void svt_flexing_thin_plate_spline::initCoef(unsigned int iIndexCdbk)
{
    for (unsigned int iIndex=0; iIndex < m_oCoef.width(); iIndex++)
	m_oCoef[0][iIndex] = m_oCoefs[iIndexCdbk][iIndex];
    
}

/**
 * set computed displacement to atoms coordinates
 * \param oDisplacement computed displacement
 * \param iIndexAtom represent the index of the atom 
 */
void svt_flexing_thin_plate_spline::setDisplacement(svt_matrix<Real64> oDisplacement,unsigned int iIndexAtom)
{
    svt_vector4 <Real64> oDisp(oDisplacement[0][0],oDisplacement[0][1],oDisplacement[0][2]);
    
    m_oFlexedConf[iIndexAtom] += oDisp;
    m_oFlexedConf.getAtom(iIndexAtom)->setTempFact(oDisp.length());
}


 /**
 * Interpolate method
 */
svt_point_cloud_pdb< svt_vector4<Real64> >& svt_flexing_thin_plate_spline::interpolate()
{
    computeAtoms2CodebooksDist();
    svt_flexing_spline::interpolate();
    m_oFlexedConf.addRemark(addParameters2Comments());
    
    return m_oFlexedConf; 
}

/**
 * Compute the positions of the flexed structure...
 */
void svt_flexing_thin_plate_spline::computeFlexedConformation()
{
    svt_flexing_spline::computeFlexedConformation();
}

/**
 * Computes the value of the basis Function based on its type
 * \see setBasisFunctionType  
 * \param fX usually the argument is a distance/radius
 * \return the value of the basis function
 */
 Real64 svt_flexing_thin_plate_spline::basisFunction(Real64 fX)
{
    Real64 fY;
    
    switch(m_iBasisFunctionType)
    {
	case 1:
	    fY = fX;
	    break;
	case 2:
	    fY = pow(fX,2)*log(pow(fX,2));
	    break;
	case 3:
	    fY = pow(fX,3);
	    break;
	default:
	    fY = pow(fX,2)*log(pow(fX,2));
	    break;
    }
    
    return fY;
}

/**
 * Include comments/remarks in the output pdb files
 * \return the comment to add to the pdb structures
 * \see svt_point_cloud_pdb:addRemark(...)
 */ 
string svt_flexing_thin_plate_spline::addParameters2Comments()
{
    string oRemarks = svt_flexing_spline::addParameters2Comments();
    
    oRemarks = "Flexing Method: Thin Plate Spline with basis function/kernel: u(r)=";
    switch(m_iBasisFunctionType)
    {
	case 1:
	    oRemarks += "r." ;
	    break;
	case 2:
	    oRemarks += "r^2*log(r^2).";
	    break;
	case 3:
	    oRemarks += "r^3.";
	    break;
	default:
	    oRemarks += "r^2*log(r^2).";
	    break;
    }
     
    oRemarks += "\n";
    
    return oRemarks;
}
