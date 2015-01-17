/***************************************************************************
                          svt_flexing_thin_plate_spline
                          --------------------------------------
    begin                : 10/01/2006
    last update          : 10/04/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_flexing_spline.h>


/**
 * Flexing using the physical model of an elastic body . This is the original method implemented by Malcolm Davis.
 */
 
 class DLLEXPORT svt_flexing_thin_plate_spline: public svt_flexing_spline
{
    protected:
	
    public:
	/**
	 * Constructor
	 */
	svt_flexing_thin_plate_spline(const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
			              const svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks,
				      const svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks,
				      bool bFlexAllAtoms,  string pDistType, string iDistInGraphSchema,
				      unsigned int iBasisFunctionType = 2);
	
	/**
	 * Destructor
	 */
	~svt_flexing_thin_plate_spline();
	
	/**
	 * Computes the value of the basis Function based on its type
	 * \see setBasisFunctionType  
	 * \param fX usually the argument is a distance/radius
	 * \return the value of the basis function
	 */
	Real64 basisFunction(Real64 fX);
	
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
	 * Include comments/remarks in the output pdb files
	 * \return the comment to add to the pdb structures
	 * \see svt_point_cloud_pdb:addRemark(...)
	 */ 
	virtual string addParameters2Comments();
};
