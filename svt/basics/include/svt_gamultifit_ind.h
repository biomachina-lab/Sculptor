/***************************************************************************
                          svt_gamultifit_ind
                          an individual of a gamultifit
                          ---------
    begin                : 01/18/2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_GAMULTIFIT_IND
#define __SVT_GAMULTIFIT_IND


#define svt_ga_vol svt_volume<Real64>
#define svt_ga_vec svt_vector4<Real64>
#define svt_ga_mat svt_matrix4<Real64>

#include <svt_ga_ind.h>
#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_pdb.h>

/**
 * This class generates a nondegenerate set of Euler angles
 * in the specified scan range. For the full sphere, the number
 * of points is almost identical to the spiral method but without
 * the helical slant and the weirdness at the poles. Angle
 * generation for subintervals is far superior and even cases
 * where the range can't be evenly devided by delta are handled
 * gracefully.
 * The method works by dividing the sphere into slices and
 * determining how many psi angles need to be in a slice to re-
 * produce the same spacing as on the equator.
*/
class svt_eulerAngles
{
protected:

    float* m_pAngles;
    unsigned long m_iAngles;

public:

    /**
     * Constructor
     */
    svt_eulerAngles( );

    /**
     * Destructor
     */
    virtual ~svt_eulerAngles( );

    /**
     * Fill the angle table.
     * \param fPsiFrom   lower boundary of the psi angles (in degrees)
     * \param fPsiTo     upper boundary of the psi angles (in degrees)
     * \param fThetaFrom lower boundary of the theta angles (in degrees)
     * \param fThetaTo   upper boundary of the theta angles (in degrees)
     * \param fPhiFrom   lower boundary of the phi angles (in degrees)
     * \param fPhiTo     upper boundary of the phi angles (in degrees)
     * \param fDelta     angular step size in degrees
     */
    void initTable( double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo, double fDelta );
    
    /**
     * remove angles from table
     * \param fPsiFrom   lower boundary of the psi angles (in degrees)
     * \param fPsiTo     upper boundary of the psi angles (in degrees)
     * \param fThetaFrom lower boundary of the theta angles (in degrees)
     * \param fThetaTo   upper boundary of the theta angles (in degrees)
     * \param fPhiFrom   lower boundary of the phi angles (in degrees)
     * \param fPhiTo     upper boundary of the phi angles (in degrees)
     * WARNING: not fully tested
     */
    void removeAngles( double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo );
    
    /**
     * add the oppsite angles: if angle = (psi, theta, phi) 
     * add (-psi, theta, phi) (psi, theta, -phi) (-psi, theta, -phi)
     * eq with (2pi-psi, theta, phi) (psi, theta, 2pi-phi) (2pi-psi, theta, 2pi-phi)
     */
    void addOppositeAngles( );


    /**
     * How many angles do we have stored in total?
     * \return unsigned long with the number of angles
     */
    unsigned long getAngleCount();

    /**
     * The angles follow the common PTP (Goldstein) convention. This function returns the Psi angle.
     * \param iIndex index into the table of angles
     * \return psi angle
     */
    float getPsi( unsigned long iIndex );
    /**
     * The angles follow the common PTP (Goldstein) convention. This function returns the Theta angle.
     * \param iIndex index into the table of angles
     * \return theta angle
     */
    float getTheta( unsigned long iIndex );
    /**
     * The angles follow the common PTP (Goldstein) convention. This function returns the Phi angle.
     * \param iIndex index into the table of angles
     * \return phi angle
     */
    float getPhi( unsigned long iIndex );

    /**
     * searches the angles that are within fAngleRange from the angle indicated by iIndex
     * \param iIndex the reference angle around which to search
     * \param fRange how far away from the reference
     * \return a list of indexes that indicates the angles in the angle list that are close to the angle idicated by iIndex
     */
    vector<long unsigned int> getNeighborAngles( unsigned long int iIndex, Real64 fAngleRange );

protected:

    /**
     * This function precomputes the angle table. It is called automatically in the constructor.
     */
    unsigned long proportionalEulerAngles(double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo, double fDelta);
      
    /**
     * This function precomputes the angle table. It is called automatically in the constructor.
     */
    unsigned long proportionalEulerAnglesThetaOrdering(double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo, double fDelta);
};


/**
 * GA multifit individual
 * \author Mirabela Rusu
 */
class svt_gamultifit_ind : public svt_ga_ind
{
protected:
    // represents the encoded solution in a very simplified reprentation 
    // here we use 4 atoms for each unit (no genes/4) that represent the 
    // Cartesian coordinate systems ( (0,0,0), (1,0,0), (0,1,0), (0,0,1) ) 
    // to which the rotation and translations were applied 
    vector< svt_vector4<Real64> > m_oCoarsePhenotype;

    //wrote on disk
    bool m_bWrote;
    
public:

    /**
     * Constructor
     */
    svt_gamultifit_ind();

    /**
     * Destructor
     */
    virtual ~svt_gamultifit_ind();
    
    /**
     * create the coarse phenotype (equivalent simple pdb) but don't fill yet atomic coordinates
     * \param number of units
     */
    void buildCoarsePhenotype( unsigned int iNoUnits );

    /**
     * update the coarse phenotype for the given unit 
     * \param transformation matrix for that unit
     * \param number of units
     */
    void updateCoarsePhenotype(svt_ga_mat oMat, unsigned int iUnit);


    /**
     * get the coarse phenotype
     */
    svt_point_cloud_pdb< svt_vector4<Real64 > > getCoarsePhenotype();

    /**
     * calculate the distance between two individuals
     * \param rOther reference to the other individual
     * \return vector distance between the two gene-vectors
     */
    virtual Real64 distance( svt_gamultifit_ind& rOther );

    /**
     * set Wrote on disk
     * \param bWrote whether it was already wrote on disk
     */
    void setWrote( bool bWrote );
    
    /**
     * get Wrote on disk
     * \return bWrote whether it was already wrote on disk
     */
    bool getWrote();


};
#endif

