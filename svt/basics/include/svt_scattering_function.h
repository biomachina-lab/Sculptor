/***************************************************************************
                          svt_scattering_function
			  ----------
    begin                : August 27 2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SCATTERING_FUNCTION_H
#define SVT_SCATTERING_FUNCTION_H

#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_pair_distribution.h>
#include <svt_point_cloud_pdb.h>
#include <svt_vector4.h>

typedef enum
{
    IS_RMS,//3
    IS_LOG, //4
    IS_LOG2, //5
    IS_ABS, //6
    IS_CORR, //7
    IS_CC_ABS_P3 //8
} svt_scatteringScoreType;

typedef struct
{
    vector<Real64> aBin;
    vector<Real64> aScatteringVector;
    svt_matrix<Real64> oSin;
    
    void print()
    {
        unsigned int iSizeBins = aBin.size();
        unsigned int iIndexBin;
        for (iIndexBin=0; iIndexBin<iSizeBins; iIndexBin++)
        {
            cout << aBin[iIndexBin] << "\t";
        }
        cout << endl;
        
        unsigned int iSizeVector = aScatteringVector.size();
        unsigned int iIndexVec;
        for(iIndexVec=0; iIndexVec< iSizeVector; iIndexVec++)
        {
            cout << aScatteringVector[iIndexVec] << "\t";
        }
        cout << endl;
        
        svtout << oSin << endl;
    };
} preComputedSins;

/**
 * A container class for the saxs scattering intensity function
 * @author Mirabela Rusu
 */
class svt_scattering_function
{
protected:
    
    // the pair distance distribution
    svt_pair_distribution m_oPDF;
    
    //The number of atoms/beads/spheres in the model
    unsigned int m_iCount;
    
    // description of the x axis: min, max and increments
    Real64 m_fMinX, m_fMaxX, m_fIncX;
    
    //values 
    vector<Real64> m_oX, m_oY, m_oErr;
    
    //use logarithm scale
    bool m_bIsLog;
    
    //the bin increment for the pdf calculation
    Real64 m_fWidth;
    
    //parameters for the scoring functions
    vector<Real64> m_oParams;
    
    // were the sin already computed
    bool m_bIsSinComputed;
    
    //The vector of sines
    preComputedSins m_oSins;

public:
    /**
     * Constructor
     */
    svt_scattering_function();
    
    /**
     * Destructor
     */
    ~svt_scattering_function(){};
    
    /**
     * Set PDF
     */
    inline void setPDF(svt_pair_distribution oPDF){m_oPDF = oPDF;};
    
    /**
     * Set MinX
     */
    inline void setMinX(Real64 fMinX){m_fMinX = fMinX;};
     
    /**
     * Get MinX
     */
    inline Real64 getMinX(){return m_fMinX;};

    /**
     * Set MaxX
     */
    inline void setMaxX(Real64 fMaxX){m_fMaxX = fMaxX;};
    
    /**
     * Get MaxX
     */
    inline Real64 getMaxX(){return m_fMaxX;};

    /**
     * Set IncX
     */
    inline void setIncrementX(Real64 fIncX){m_fIncX = fIncX;};
    
    /**
     * get IncX
     */
    inline Real64 getIncrementX(){ return m_fIncX;};
    
    /**
     * Get MaxY
     */
    inline Real64 getMaxY();
    
    /**
     * Set the atom count = the number of atoms in the pdb
     */
    inline void setCount(unsigned int iCount){m_iCount = iCount;};
    
    /**
     * get the atom count = the number of atoms in the pdb
     */
    inline unsigned int getCount(){return m_iCount;};

    
    /**
     * Should the values of the intensity be logarithmic
     */
    inline void setIsLog(bool bIsLog);
    
    /**
     * get is log
     */
    inline bool getIsLog(){return m_bIsLog;};
    
    /**
     * set the width of the histogram in case it has to be computed here
     */
    inline void setPDFWidth(Real64 fWidth){m_fWidth = fWidth;};
    
    /**
     * set the parameters used in the fitness function
     */
    inline void setParams(vector<Real64> oParams){m_oParams = oParams;};
    
    /**
     * get the parameters used in the fitness function
     */
    inline vector<Real64> getParams(){return m_oParams;};
    
    /**
     * get Intensities
     */
    inline vector<Real64> y(){return m_oY;};
    
    
    /**
     * get x
     */
    inline vector<Real64> x(){return m_oX;};
    
    /**
     * get err
     */
    inline vector<Real64> err(){return m_oErr;};
    
     /**
     * get the parameters used in the fitness function
     */
    inline preComputedSins getSin(){return m_oSins;};
    
    /**
     * get the parameters used in the fitness function
     */
    inline void setSin(preComputedSins oSins)
    {
        m_bIsSinComputed = true;
        m_oSins.aBin=oSins.aBin;
        m_oSins.aScatteringVector=oSins.aScatteringVector;
        m_oSins.oSin=oSins.oSin;
    };
    
    
    /**
     * Calc the values of the intensity
     */
    void calcFunction();

    /**
     * compute the scattering intensities of a PDB 
     * \param oPDB the structure for which to compute the scattering intensities
     */
    vector<Real64> calcSAXSIntensity(svt_point_cloud_pdb< svt_vector4<Real64> >& oPDB );
     
    /**
     * compute the scattering intensities of a PDF 
     * \param oPDF the pair distance distribution to compute the scattering intensities
     */
    vector<Real64> calcSAXSIntensity(svt_pair_distribution& oPDF, unsigned int iAtoms );
    
    /**
     * Computes the differrence-fitness function between two scattering intensities
     * \param the other scattering intensity function
     * \param type of the score 
     */
    Real64 getFitness(svt_scattering_function &A, svt_scatteringScoreType );
    
    /**
     * compute the correlation of two scattering intensities functions
     * \param the other scattering intensity function
     */
    vector<Real64> getCorrelation(svt_scattering_function &A);
    
    /**
     * Normalize = devide intensities by max intensity
     */
    void normalize();
    
    /**
     * Overload of the = operator
     */
    void operator=(svt_scattering_function that);
    
    /**
     * 
     */
    unsigned int size(){return m_oY.size();}
    
    /**
     * print the scattering intensities
     */
    void print(char *pFname=NULL);
    
    /**
     * Load data from Gnom file
     * \param PFname the name of the gnom file
     */
    void loadGnom(char* pFname);
    
    /**
     * Load data from file normalized with R
     * first column is the row number, second is the scattering angles and the third is the scattering intensities
     * \param PFname the name of the gnom file
     */
    void loadR(char* pFname);

};

#endif
