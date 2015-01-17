/***************************************************************************
                          svt_tube
                          a tube
                          ---------
    begin                : 04/26/2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_HELIX_IND
#define __SVT_HELIX_IND

#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_pdb.h>
#include <svt_gacylinder_ind.h>

/**
 *  a cylinder
 * \author Mirabela Rusu
 */
class svt_tube
{
protected:
    // the gacylinder ind that create the cylinder
    vector<svt_gacylinder_ind > m_oElements; 

    //another score
    Real64 m_fAvgFitness;

    //another score
    Real64 m_fSumFitness;

    //the length of the tube - it represents the number of step/turns that were added to the tube
    unsigned int m_iSize;

    //the length of the tube in A
    Real64 m_fLength; //

    // the score based on the map
    Real64 m_fMapScore;
    
    //default NO: once the tube is computed then true
    bool m_bWasTubeComputed;

    //default NO: once the tube is computed then true
    bool m_bWasFineTubeComputed;

    // the points on the axis 
    svt_point_cloud_pdb<svt_ga_vec> m_oTube;

    // the points on the axis 
    svt_point_cloud_pdb<svt_ga_vec> m_oFineTube;

    // the hr tube
    svt_point_cloud_pdb<svt_ga_vec> m_oHRTube;

    //
    svt_vector4<Real64> m_oFirstAddedElem;

    //penatly - score that is substracted from the original score to penalize for characteristics (e.g. axes crossing)
    Real64 m_fPenalty;

    // curvature of the axis
    Real64 m_fCurvature;

    //should the score be recomputed
    bool m_bRecomputeScore;

    //scores
    vector<Real64> m_oScores;

    //
    svt_ga_vec m_oDirection;
public:

    /**
     * Constructor
     */
    svt_tube();

    /**
     * destructor
     */
     virtual ~svt_tube(){};

    /**
     * add a new element to tube
     * \param oElem a svt_gacylinder_ind that in considered in this cylinder
     */
    void add2Tube(svt_gacylinder_ind oElem, bool bFlip = false);

    /**
     * remove the last element of the tube
     */
    void pop( );

    /**
     * get the list of individuals added to the tube
     * \return the individuals
     */ 
    vector<svt_gacylinder_ind > getElements();

    /**
     * set the map score
     * \param fScore the score assigned
     */ 
    void setMapScore( Real64 fScore);

    /**
     * get the map score
     * \return the score of the tube
     */ 
    Real64 getMapScore(); 

    /**
     * set the score
     * \param fScore the score assigned
     */ 
    void setAvgScore( Real64 fScore);

    /**
     * get the score
     * \return the score of the tube
     */ 
    Real64 getAvgScore(); 

    /**
     * Compute avg score
     */ 
    void computeAvgScore();

    /**
    * set the score 
    * \param iIndex - which element in vector score
    * \param fScore - the value
    */
    void setScore(int iIndex, Real64 fScore);

    /**
    * returns the list of scores
    * \param a vector with the different scores
    */
    vector<Real64> getScores() ;
 
	/**
     * get the number of points
     * \return the number of points
     */ 
    unsigned int size(); 

	/**
     * get the number of turns
     * \return the number of turns
     */ 
    Real64 getTurns(Real64 fRatio = 1.0); 

	/**
	 * Compute the length in A0
	 */
	void computeLength();

    /**
     * get the length = sum distances between points
     * assumes that the points are in order = point 0 is closest 1 and point 2 follows point 1... etc
     * \return the length of the tube 
     */ 
    Real64 getLength(); 

    /**
     * get the first element
     */  
    svt_vector4<Real64> getFirstAddedElem();

    /**
     * set penalty
     * \param the penalty
     */ 
    void setPenalty( Real64 fPenalty);

    /**
     * increase penalty
     * \param the penalty
     */ 
    void addPenalty( Real64 fPenalty);

    /**
     * get penalty
     * \return the penalty
     */ 
    Real64 getPenalty();

    /**
     * get curvature
     */
    Real64 getCurvature();

    /**
     * overload < operator
     * \param that another svt_tube element
     */ 
    bool operator<(const svt_tube& that) const;

    /**
     * overload < operator using the max density of the map
     * \param that another svt_tube element
     */ 
    static bool lt_mapScore(svt_tube first, svt_tube second); 

    /**
     * overload < operator using the max density of the map
     * \param that another svt_tube element
     */ 
    static bool lt_length(svt_tube first, svt_tube second); 


    /**
     * overload < operator using the max density of the map
     * \param that another svt_tube element
     */ 
    static bool lt_wPenatly(svt_tube first, svt_tube second); 

    /**
     * overload < operator using the max density of the map
     * \param that another svt_tube element
     */ 
    static bool lt_score(svt_tube first, svt_tube second);   

    /**
     * get tube as
     * \param a template, e.g. circle
     * \param should return the fine version
     * \return the pdb tube
     */
    svt_point_cloud_pdb<svt_ga_vec> getTube(svt_point_cloud_pdb<svt_ga_vec> *pTemplate=NULL, bool bFine = false);
 
    /**
     * get the direction of the tube
     */ 
    svt_ga_vec getDirection();

    /**
     * print the tube
     */  
    void print();
   
    /**
     * compute the volume underneath the tube
     */
    void fillExplored(svt_ga_vol & rVol, svt_ga_vol *pVol);

    /**
     *get the high resolution version of the tube
     */ 
    svt_point_cloud_pdb<svt_ga_vec> getHRTube();

    /**
     * creates a highresolution version of the helix
     */
    void createHighResTube( svt_ga_vol & rVol, svt_point_cloud_pdb<svt_ga_vec> *pTemplate);

    /**
     * Estimate curvature of the axis
     */
    void estimate_curvature();

    /**
     * discard the points that are at the ends of the tube is their score is < fScore
     */
    void discardPointsAtEnds( Real64 fScore); 
};
#endif

