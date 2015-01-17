/***************************************************************************
                          svt_flexing_spline
                          --------------------------------------
    begin                : 09/29/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/
 
#include <svt_flexing.h>

#ifndef SVT_FLEXING_SPLINE_H
#define SVT_FLEXING_SPLINE_H
/**
 * Flexing using spline interpolation. This is a base class, not to be instanciated.
 * \see svt_flexing_thin_plate_spline, svt_flexing_cubic_spline, 
 * svt_flexing_elastic_body_spline
 */
class DLLEXPORT svt_flexing_spline : public svt_flexing
{
    protected:
	svt_matrix <Real64> m_oKernelsPoints;
	svt_matrix <Real64> m_oDisplacements;
	svt_matrix <Real64> m_oCoefs;
	svt_matrix <Real64> m_oKernel;
	svt_matrix <Real64> m_oDisplacement;
	svt_matrix <Real64> m_oCoef;
	unsigned int m_iKernelSize; // 3 for elastic body; 1 for thin plate spline
	unsigned int m_iDimensions; // 1 for elastic body; 3 for thin plate spline
	
	/**
	 * Values: TPS       1:r (default); 2:r^2*log(r^2); 3:r^3
	 *         EBS
	 * Represents the type of the basis function (the solution of the biharmonical equation)
	 * used by the kernel (is more like the kernel)
	 */
	unsigned int m_iBasisFunctionType;
	
    public:
	/**
	 * Constructor 
	 */
	svt_flexing_spline(const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
			   const svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks,
			   const svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks,
			   bool bFlexAllAtoms, string pDistType, string pDistInGraphSchema,
			   unsigned int iBasisFunctionType = 2);
		
	/**
	 * Destructor
	 */
	~svt_flexing_spline();
	
	/**
	 * Initialize the 2 matrices: m_oKernelsPoints and m_oDisplacements;
	 */ 
	virtual void initKernelsPointsDisplacements();
	
	 /**
	 * Interpolate method
	  */
	virtual svt_point_cloud_pdb< svt_vector4<Real64> >& interpolate();
	
	/**
	 * Compute the positions of the flexed structure...
         */
	virtual void computeFlexedConformation();
	
	/**
	 * Initialize the kernel matrix. This matrix has different values for each couple of codebook vectors
	 * \param iIndexCdbkSrc represent the index of the first codebook 
	 * \param iIndexCdbkDest indicates the index of the second codebook
	 */
	virtual void initKernelCdbk2Cdbk(unsigned int iIndexCdbkSrc, unsigned int iIndexCdbkDest) = 0;
	
	/**
	 * Initialize the kernel matrix. This matrix has different values for each couple of atom - codebook vector
	 * \param iIndexAtom represent the index of the Atom 
	 * \param iIndexCdbk indicates the index of the codebook
	 */
	virtual void initKernelAtom2Cdbk(unsigned int iIndexAtom, unsigned int iIndexCdbk) = 0;
	
	/**
	 * Initialize the displacement of one codebook vector
	 * \param iIndexCdbk represent the index of the first codebook 
	 */
	virtual void initDisplacement(unsigned int iIndexCdbk) = 0;

	/**
	 * Initialize the coefs of one codebook vector
	 * \param iIndexCdbk represent the index of the codebook 
	 */
	virtual void initCoef(unsigned int iIndexCdbk) = 0;
	
	/**
	 * set computed displacement to atoms coordinates
	 * \param oDisplacement computed displacement
	 * \param iIndexAtom represent the index of the atom 
	 */
	virtual void setDisplacement(svt_matrix<Real64> oDisplacement,unsigned int iIndexAtom) = 0; 
	
	/**
	 * Set the type of the Basis Function: TPS    1:r; 2:r^2*log(r^2) (default); 3:r^3
	 *                                     EBS    1:  2: variant (default)
	 */
	virtual void setBasisFunctionType(unsigned int iBasisFunctionType){m_iBasisFunctionType = iBasisFunctionType;}
	
	/**
	 * Include comments/remarks in the output pdb files
	 * \return the comment to add to the pdb structures
	 * \see svt_point_cloud_pdb:addRemark(...)
	 */ 
	virtual string addParameters2Comments();
	
};
#endif
