/***************************************************************************
                          svt_flexing_elastic_body_spline
                          --------------------------------------
    begin                : 10/01/2006
    last update          : 10/04/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLEXING_ELASTIC_BODY_SPLINE_H
#define SVT_FLEXING_ELASTIC_BODY_SPLINE_H


#include <svt_flexing_spline.h>


/**
 * Flexing using the physical model of an elastic body . This is the original method implemented by Malcolm Davis.
 */
 
 class DLLEXPORT svt_flexing_elastic_body_spline: public svt_flexing_spline
{
    protected:
	Real64 m_fElasticity;
	Real64 m_fCnstAlpha; // depends on m_fElasticity
    
    public:
	/**
	 * Constructor
	 */
	svt_flexing_elastic_body_spline(const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
					const svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks,
					const svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks,
					bool bFlexAllAtoms, string pDistType, string iDistInGraphSchema,
					unsigned int iBasisFunctionType = 2, Real64 fElasticity = 0.01); //soft rubber
	
	/**
	 * Destructor
	 */
	~svt_flexing_elastic_body_spline();
	
	
	/**
	 * Set the values of the elasticity coefficient (which ranges from 0.01 - soft rubber to 0.49 - hard rubber)
	 * Also computes alpha - a constant directly depending on the elasticity;
	 */
	void setElasticity(Real64 fElasticity);
	
	/////////////////////////////////////////////////////////////////////////////
	//
	//                   VIRTUAL STUFF
	//
	//
	//////////////////////////////////////////////////////////////////////////////
	
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
	virtual void initKernelCdbk2Cdbk(unsigned int iIndexCdbkSrc, unsigned int iIndexCdbkDest);
	
	/**
	 * Initialize the kernel matrix. This matrix has different values for each couple of atom - codebook vector
	 * \param iIndexAtom represent the index of the Atom 
	 * \param iIndexCdbk indicates the index of the codebook
	 */
	virtual void initKernelAtom2Cdbk(unsigned int iIndexAtom, unsigned int iIndexCdbk);
	
	/**
	 * Initialize the displacement of one codebook vector
	 * \param iIndexCdbk represent the index of the first codebook 
	 */
	virtual void initDisplacement(unsigned int iIndexCdbk);
	
	/**
	 * Initialize the coefs of one codebook vector
	 * \param iIndexCdbk represent the index of the first codebook 
	 */
	virtual void initCoef(unsigned int iIndexCdbk);
	
	/**
	 * set computed displacement to atoms coordinates
	 * \param oDisplacement computed displacement
	 * \param iIndexAtom represent the index of the atom 
	 */
	virtual void setDisplacement(svt_matrix<Real64> oDisplacement,unsigned int iIndexAtom);
	
	/**
	 * Set the type of the Basis Function: TPS    1:r; 2:r^2*log(r^2); 3:r^3
	 *                                     EBS      
	 */
	virtual void setBasisFunctionType(unsigned int iBasisFunctionType);
	
	 /**
	 * Include comments/remarks in the output pdb files
	 * \return the comment to add to the pdb structures
	 * \see svt_point_cloud_pdb:addRemark(...)
	  */ 
	virtual string addParameters2Comments();
	
};

#endif
