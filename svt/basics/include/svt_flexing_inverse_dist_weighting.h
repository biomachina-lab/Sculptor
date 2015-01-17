/***************************************************************************
                          svt_flexing_inverse_distance_weighting
                          --------------------------------------
    begin                : 09/10/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_flexing.h>
/**
 * Flexing using the inverse distance weighting interpolation
 *  
 * \author Mirabela Rusu
 */


class DLLEXPORT svt_flexing_inverse_dist_weighting : public svt_flexing
{
protected:
    string m_pWeightingMethod;
    Real64 m_fWeightPow;
    Real64 m_fMaxDist2Cdbks4Atom;
    unsigned int m_iNbCdbksOfInfluence;

public:

    /**
     * Constructor of the inverse distance interpolator
     */
    svt_flexing_inverse_dist_weighting(const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
				       const svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks,
				       const svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks,
				       bool bFlexAllAtoms, string pDistType, string iDistInGraphSchema,
				       string pWeightingMethod = "Hybrid", Real64 fWeightPow = 4.0);

    /**
     * Destructor
     */
    ~svt_flexing_inverse_dist_weighting();


    /**
     * Sets the Weighting Method
     * \param sWeightingMethod an integer value
     * \arg 0 pure inverse distance(default value)
     * \arg 1 uses both the distance to the codebook and to the further codebook
     */
    void setWeightingMethod(string pWeightingMethod = "Hybrid" ){m_pWeightingMethod = pWeightingMethod;}

    void setNbCdbksOfInfluence(unsigned int iNbCdbksOfInfluence){m_iNbCdbksOfInfluence = iNbCdbksOfInfluence;}
    
    /**
     * Sets the power of the weight
     * \param fWeightPow an integer value representing the power
     * \arg fWeightPow=4.0 recommended value at it seems to give the best results
     */
    void setWeightPow(Real64 fWeightPow = 4.0){m_fWeightPow = fWeightPow;}

    /**
     * Computs the weight of the iIndexCodebook codebook vector for the atom iIndexAtom
     * \param iIndexCodebook the index of the codebook vector
     * \param iIndexAtom the index of the atom
     * \return a float value representing the weight
     */
    float getCodebookWeight4Atom(unsigned int iIndexCodebook, unsigned int iIndexAtom);

    	
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
     *
     */
    virtual void computeCodebooks2AtomsDist();

    /**
     * Compute the positions of the flexed structure...
     */
    virtual void computeFlexedConformation();
    
     /**
     * Include comments/remarks in the output pdb files
     * \return the comment to add to the pdb structures
     * \see svt_point_cloud_pdb:addRemark(...)
      */ 
    virtual string addParameters2Comments();
};
