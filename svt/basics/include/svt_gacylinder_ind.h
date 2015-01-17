/***************************************************************************
                          svt_gacylinder_ind
                          an individual of a gacylinder
                          ---------
    begin                : 01/18/2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_GACYLINDER_IND
#define __SVT_GACYLINDER_IND


#define svt_ga_vol svt_volume<Real64>
#define svt_ga_vec svt_vector4<Real64>
#define svt_ga_mat svt_matrix4<Real64>

#include <svt_ga_ind.h>
#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_pdb.h>
/**
 * \author Mirabela Rusu
 */
class svt_gacylinder_identifySse_ind : public svt_ga_ind
{
protected:
    // the number of ones in the individual
    int m_iSseCount;

    // the number of intersections
    unsigned int m_iIntersectionCount;

    // the mapping with the target helices
    vector<int> m_oMappingID;

    // a vector with the cut cylinders stored as pdbs
    vector< svt_point_cloud_pdb<svt_ga_vec> > m_oCylinders;
public:
    /**
     * the constructor
     */ 
    svt_gacylinder_identifySse_ind();
    /**
     * the destructor
     */ 
    virtual ~svt_gacylinder_identifySse_ind();

    /**
     * set the number of sse
     */
    void setSseCount(int iSseCount);
    /**
     * get the number of sse
     */
    int getSseCount();
    /**
     * set the number of intersections
     */
    void setIntersectionCount(int iIntersectionCount);
    /**
     * get the number of intersections
     */
    int getIntersectionCount();
    /**
     * set the mapping
     */
    void setMapping(vector<int>& oMappingID);
    /**
     * get the mapping
     */
    vector<int>& getMapping();
    /**
     * set the cylinders
     */
    void setCylinders(vector< svt_point_cloud_pdb<svt_ga_vec> >& oCylinders);
    /**
     * get the cylinders
     */
    vector< svt_point_cloud_pdb<svt_ga_vec> >&  getCylinders();
    /**
     * get the cylinders into a pdb model
     */
    void getCylinders2Model( svt_point_cloud_pdb<svt_ga_vec> * pModel);
    /**
     * compute the number of Sse in the individual
     */
    void computeSseCount();  
    /**
     * print genes to cout
     */
    virtual void printGenes();

};

/**
 * GA multifit individual
 * \author Mirabela Rusu
 */
class svt_gacylinder_ind : public svt_ga_ind
{
protected:
    // represents the encoded solution in a very simplified reprentation 
    // here we use 4 atoms for each unit (no genes/4) that represent the 
    // Cartesian coordinate systems ( (0,0,0), (1,0,0), (0,1,0), (0,0,1) ) 
    // to which the rotation and translations were applied 
    vector< svt_vector4<Real64> > m_oCoarsePhenotype;

    //wrote on disk
    bool m_bWrote;

    // the number of turns in the cylinder - the cylinder has 2*m_iTurns+1 circles inside the cylinder
    unsigned int m_iTurns;

    // the height of a turn
    Real64 m_fHeightTurn;

    // the transformation matrix that was applied to original vector
    svt_ga_mat m_oTrans;

    //fitness of the top part
    Real64 m_fFitnessTop;
 
    //fitness of the bottom part
    Real64 m_fFitnessBot;
 
    
public:

    /**
     * Constructor
     */
    svt_gacylinder_ind();

    /**
     * destructor
     */
    ~svt_gacylinder_ind(){};
    
    /**
     * create the coarse phenotype (equivalent simple pdb) but don't fill yet atomic coordinates
     * \param number of points 
     */
    void buildCoarsePhenotype();

    /**
     * update the coarse phenotype for the given unit 
     * \param transformation matrix for that unit
     * \param number of units
     */
    void updateCoarsePhenotype(svt_ga_mat oMat);


    /**
     * get the coarse phenotype
     */
    svt_point_cloud_pdb< svt_vector4<Real64 > > getCoarsePhenotype();

    /**
     * calculate the distance between two individuals
     * \param rOther reference to the other individual
     * \return vector distance between the two gene-vectors
     */
    virtual Real64 distance( svt_gacylinder_ind& rOther );

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

    /**
     * get the number of turns
     * \return the number of turns
     */
    unsigned int getTurns();

    /**
     * get the number of turns
     * \param the number of turns
     */
    void setTurns( unsigned int iTurns);
 
    /**
     * get the height of a turn
     * \return the height
     */
    Real64 getHeightTurn();
    
    /**
     * set the height of a turn
     * \param the height
     */
    void setHeightTurn( Real64 fHeightTurn);

    /**
     * get the Transformation
     * \return the transformation
     */	
    svt_ga_mat getTrans();

    /**
     * set the Transformation
     * \param the transformation
     */	
    void setTrans(svt_ga_mat& rTrans);


    /**
     * set Fitness Top
     * \param the new fitness
     */ 
    void setFitnessTop( Real64 fFitness);

    /**
     * get Fitness Top
     * \return the new fitness
     */ 
    Real64 getFitnessTop();

    /**
     * set Fitness Bot
     * \param the new fitness
     */ 
    void setFitnessBot( Real64 fFitness);

    /**
     * get Fitness Top
     * \return the new fitness
     */ 
    Real64 getFitnessBot();

};
#endif

