/***************************************************************************
                          svt_pair_distance_distribution
			  ----------
    begin                : May 23 2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PAIR_DISTRIBUTION_H
#define SVT_PAIR_DISTRIBUTION_H

#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_vector4.h>

/**
 * A container class for a pair distance distribution
 * @author Mirabela Rusu
 */
class svt_pair_distribution
{
protected:

   //m_aDensity[i] represents the number of number of atom pairs between m_aBins[i-1] and m_aBins[1] (see below)
   vector<Real64 > m_aDensity;
   
   //bin is defined as distance range
   //m_aDensity[i] represents the density between m_aBins[i-1] and m_aBins[i]
   vector<Real64> m_aBins;
   
   //bin Width
   Real64 m_fWidth;
   
   //total number of distance pairs
   Real64 m_fSum;
   
   //was frequency calculated 
   bool m_bIsNormalized;
   
   //
   svt_matrix<Real64> m_oDist;
           
public:

    /**
     * Constructor
     */
    svt_pair_distribution();
    
    /**
     * Constructor 2
     */
    svt_pair_distribution(vector<Real64 > aDensity, vector<Real64 > aBins);
    
    /**
     * Destructor
     */
    ~svt_pair_distribution();
    
    /**
     * set width of the Bins
     **/
    void setWidth( Real64 fWidth );
     
    /**
     * set 
     **/
    void setMin( Real64 fMin );
     
    /**
     *
     */
    void addPairDistance( Real64 fDistance );
    
    /**
     * Add one distance in bin
     * \param iBin the bin in which the distance should be added
     **/
    void add2Bin( unsigned int iBin );

    
    /**
     * Add Bin
     */
    void addBin(Real64 fBin);
    
    /**
     * Add Density
     */
    void addDensity(Real64 fDensity);
    
    /**
     * Add Both Bin and Density
     */
    void add(Real64 fBin, Real64 fDensity);
    
    /**
     * Print
     */ 
    void print(char *pFilename=NULL, bool bAppend=false);
    
    /**
     * Get the number of bins 
     */ 
    int size();
    
    /**
     * Get function
     */ 
    vector<Real64 > getPdf();
    
    /**
     * Get Bins
     */ 
    vector<Real64 > getBins();
    
    /**
     * Get Width
     */ 
    Real64 getWidth();
    
    /**
     * Get Sum of distances
     */ 
    Real64 getDistCount();
    
    /**
     *
     */
    bool getNormalized();
    
    /**
     *
     */
    void setNormalized(bool bIsNormalized);
    
    /**
     * Set the distance matrix and (re)compute pdf
     */
    void setDistanceMatrix(svt_matrix<Real64> & oDist);
    
    /**
     * Get distance matrix
     */
    svt_matrix<Real64> & getDistanceMatrix(){ return m_oDist;};


    /**
     * Clear
     */
    void clear();
    
    /**
     * Overload of the = operator
     */
   void operator=(svt_pair_distribution that);
    
    /**
     * Overload of the - operator
     * \param svt_pair_distribution the distribution at the right side of the -
     */
    svt_pair_distribution operator-(svt_pair_distribution& A);
    
    /**
     * Overload of the + operator
     * \param svt_pair_distribution the distribution at the right side of the +
     */
    svt_pair_distribution operator+(svt_pair_distribution& A);
    
    /**
     * Overload of the - operator
     * \param fValue to multiply with 
     */
    void operator*=(Real64 fValue);

    /**
     * Another difference: weighted
     * \param svt_pair_distribution the distribution at the right side of the -
     */
    svt_pair_distribution weightedDiff(svt_pair_distribution& A, Real64 fWeightingConst=0.0f);
    
    /**
     * Normalize by deviding the count of distance pairs with the total number of distance pairs
     */
    void normalize();
    
    /**
     * Load PDF from a GNOM file
     */
    void loadGnom(const char* pFilename);
    
    /**
     * get Norm squared
     */
    Real64 getNormSq();
    
    /**
     * get Distance squared
     * \param A the other pair distance distribution
     * \param fConst - constant value; if not null is used in the weighting of the distance square
     */
    Real64 distanceSq(svt_pair_distribution& A, Real64 fConst = 0.0f);
    
    /**
     * Get average = sum of bin values devided by the bin number
     */
    Real64 getAverage();
    
    /**
     * Get sum = sum of bin values
     */
    Real64 getSum();
    
     /**
     * Get deviation from value 
     */
    Real64 getDeviation( Real64 fValue );
    
    /**
     *
     */
    void writeEPS(char* pFname_R_script, char* pFname_img, char*  pFname_ref, char* pFname_target);    
};

#endif
