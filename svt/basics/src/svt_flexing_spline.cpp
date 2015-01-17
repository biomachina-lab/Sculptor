/***************************************************************************
                          svt_flexing_spline
                          --------------------------------------
    begin                : 09/29/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_flexing_spline.h>
#include <svt_iostream.h>
#include <svt_point_cloud.h>
#include <svt_vector4.h>
#include <svt_matrix.h>
#include <math.h>

#define FLEX cout << svt_trimName("flexing")

/**
 * Constructor
 */
svt_flexing_spline::svt_flexing_spline(
    const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
    const svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks, 
    const svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks,
    bool bFlexAllAtoms, string pDistType, string pDistInGraphSchema,
    unsigned int iBasisFunctionType) : svt_flexing(oOrigConf,oOrigConfCodebooks,oFinalConfCodebooks, bFlexAllAtoms, pDistType, pDistInGraphSchema)
{
    m_iBasisFunctionType = iBasisFunctionType;
}


/**
 * Destructor
 */
svt_flexing_spline::~svt_flexing_spline()
{
    
}

 /**
 * Interpolate method
 */
svt_point_cloud_pdb< svt_vector4<Real64> >& svt_flexing_spline::interpolate()
{
    // init matrices
    try 
    {
	initKernelsPointsDisplacements();
	
	//solve the system by first computing the inverse 
	m_oKernelsPoints.gaussjordan();
	
	m_oCoefs = m_oKernelsPoints;
	m_oCoefs *= m_oDisplacements; //* codebooks displacements
        
	//compute the displacements and the new positions
	computeFlexedConformation();
	
    }
    catch(svt_exception oError) 
    {
	oError.uiMessage();
    }
    
    return m_oFlexedConf; 
    
}

/**
 * Compute the positions of the flexed structure...
 */
void svt_flexing_spline::computeFlexedConformation()
{
    svt_matrix <Real64> oDisplacement(m_oDisplacement.height(),m_oDisplacement.width());
    unsigned int iIndex;
    unsigned int iIndexCA;

    for (unsigned int iIndexAtom = 0; iIndexAtom < m_oFlexedConf.size(); iIndexAtom++)
    {
	oDisplacement = 0;
	iIndexCA = m_oMoveLike[iIndexAtom];
	
	for (unsigned int iIndexCdbk = 0; iIndexCdbk < m_oOrigConfCodebooks.size(); iIndexCdbk++)
	{
	    initKernelAtom2Cdbk(iIndexCA, iIndexCdbk);
	    initCoef(iIndexCdbk);
	    m_oKernel *= m_oCoef;
	    oDisplacement += m_oKernel;
	}
	
	// affine transformation
	//x
	iIndex = 0;
	initCoef(m_oOrigConfCodebooks.size()+iIndex);
	m_oCoef *= m_oOrigConf[iIndexCA].x();
	oDisplacement += m_oCoef;
	
	//y
	iIndex = 1;
	initCoef(m_oOrigConfCodebooks.size()+iIndex);
	m_oCoef *= m_oOrigConf[iIndexCA].y();
	oDisplacement += m_oCoef; 
	
	//z
	iIndex = 2;
	initCoef(m_oOrigConfCodebooks.size()+iIndex);
	m_oCoef *= m_oOrigConf[iIndexCA].z();
	oDisplacement += m_oCoef; 
	
	//free term
	iIndex = 3;
	initCoef(m_oOrigConfCodebooks.size()+iIndex);
	oDisplacement += m_oCoef; 
	
	setDisplacement(oDisplacement,iIndexAtom);
    }
}

void svt_flexing_spline::initKernelsPointsDisplacements()
{
    svt_matrix <Real64> oPoint;
    svt_matrix <Real64> oDisplacement;
    unsigned int iIndex;
    
    //init the identity
    oPoint.resize(m_iKernelSize,m_iKernelSize);
    m_oKernel.resize(m_iKernelSize,m_iKernelSize);
    m_oDisplacements = 0;
    
    oPoint = 0;
    m_oKernelsPoints = 0;
    m_oDisplacements = 0;
    
    //compute Fill the matrix element by element
    for(unsigned int iIndexCdbk1=0; iIndexCdbk1 < m_oOrigConfCodebooks.size();iIndexCdbk1++)
    {
	//init the kernels
	for(unsigned int iIndexCdbk2=iIndexCdbk1+1; iIndexCdbk2 < m_oOrigConfCodebooks.size();iIndexCdbk2++)
	{
	    initKernelCdbk2Cdbk(iIndexCdbk1,iIndexCdbk2);
	    m_oKernelsPoints.fill(m_oKernel,iIndexCdbk1*m_oKernel.width(),iIndexCdbk2*m_oKernel.width());
	    m_oKernelsPoints.fill(m_oKernel,iIndexCdbk2*m_oKernel.width(),iIndexCdbk1*m_oKernel.width());
	}
	
	//init the points
	//  x
	oPoint = 0;
	for (iIndex = 0; iIndex < m_iKernelSize; iIndex++) oPoint[iIndex][iIndex] = m_oOrigConfCodebooks[iIndexCdbk1].x();
	iIndex = 0;
	m_oKernelsPoints.fill(oPoint,iIndexCdbk1*m_oKernel.width(),(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width());
	m_oKernelsPoints.fill(oPoint,(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width(),iIndexCdbk1*m_oKernel.width());
	
	//  y
	oPoint = 0;
	for (iIndex = 0; iIndex < m_iKernelSize; iIndex++) oPoint[iIndex][iIndex] = m_oOrigConfCodebooks[iIndexCdbk1].y();
	iIndex = 1;
	m_oKernelsPoints.fill(oPoint,iIndexCdbk1*m_oKernel.width(),(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width());
	m_oKernelsPoints.fill(oPoint,(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width(),iIndexCdbk1*m_oKernel.width());
	
	//  z
	oPoint = 0;
	for (iIndex = 0; iIndex < m_iKernelSize; iIndex++) oPoint[iIndex][iIndex] = m_oOrigConfCodebooks[iIndexCdbk1].z();
	iIndex = 2;
	m_oKernelsPoints.fill(oPoint,iIndexCdbk1*m_oKernel.width(),(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width());
	m_oKernelsPoints.fill(oPoint,(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width(),iIndexCdbk1*m_oKernel.width());
	
	// Identity
	oPoint = 0;
	for (iIndex = 0; iIndex < m_iKernelSize; iIndex++) oPoint[iIndex][iIndex] = 1;
	iIndex = 3;
	m_oKernelsPoints.fill(oPoint,iIndexCdbk1*m_oKernel.width(),(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width());
	m_oKernelsPoints.fill(oPoint,(m_oOrigConfCodebooks.size()+iIndex)*m_oKernel.width(),iIndexCdbk1*m_oKernel.width());
	
	
	//init displacements
	initDisplacement(iIndexCdbk1);
	m_oDisplacements.fill(m_oDisplacement,iIndexCdbk1*m_oKernel.width(),0);
    }
   
}

/**
 * Include comments/remarks in the output pdb files
 * \return the comment to add to the pdb structures
 * \see svt_point_cloud_pdb:addRemark(...)
 */ 
 string svt_flexing_spline::addParameters2Comments()
 {
     string oRemarks = svt_flexing::addParameters2Comments();
     char oRemark[1000];
     
     sprintf(oRemark,"Spline based interpolation; kernel size %d. ", m_iKernelSize);
     oRemarks += oRemark;
     
     return oRemarks;
 }
