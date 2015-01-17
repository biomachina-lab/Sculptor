/***************************************************************************
                          svt_pair_distance_distribution
			  ----------
    begin                : May 23 2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/


#include <svt_pair_distribution.h>

using namespace std;

svt_pair_distribution::svt_pair_distribution():
    m_fSum(0.0f),
    m_bIsNormalized(false)
{
    setWidth( 0.5f );
    setMin( 0.0f );
}

/**
* Constructor 2
*/
svt_pair_distribution::svt_pair_distribution(vector<Real64 > aDensity, vector<Real64 > aBins):
    m_fSum(0.0),
    m_bIsNormalized(false)
{
    unsigned int iSize = aDensity.size();
    for (unsigned int iIndex=0; iIndex < iSize; iIndex++)
    {
        (*this).addDensity( aDensity[iIndex] );
        m_fSum += m_aDensity[ m_aDensity.size()-1 ];
        if (aBins.size()>0)
            (*this).addBin( aBins[iIndex] );
        else
            (*this).addBin( iIndex );
    }
}

svt_pair_distribution::~svt_pair_distribution()
{
}


/**
 * Set the Width of a Bin
 **/
void svt_pair_distribution::setWidth( Real64 fWidth )
{
    m_fWidth = fWidth;
}
     
/**
 * Set Min
 **/
void svt_pair_distribution::setMin( Real64 fMin )
{
    if (m_aBins.size()>0)
       clear();

    m_aBins.push_back( fMin );
    m_aDensity.push_back(0.0f);
    m_fSum = 0.0f;
}

/**
 * The m_aDensity[iIndex] correspunds to the count of atom pairs between m_aBins[iIndex] and m_aBins[iIndex+1]
 **/
void svt_pair_distribution::addPairDistance( Real64 fDistance )
{
    if (!m_bIsNormalized)
    {
        unsigned int iBin;
        
        iBin = (int)( fDistance/ m_fWidth );
        if (m_aBins.size()<=iBin)
        {
            while (m_aBins.size()<=iBin)
            {
                m_aBins.push_back( m_aBins[m_aBins.size()-1] + m_fWidth );
                m_aDensity.push_back(0.0f); 
            }
        }
        
        m_aDensity[iBin]++;
        m_fSum+=1.0f;
    }else
    {
        svtout << "Can't add pair distances for normalized PDFs" << endl;
        return;
    }
};

/**
 * Add one distance in bin
 * \param iBin the bin in which the distance should be added
 **/
void svt_pair_distribution::add2Bin( unsigned int iBin )
{
    if (!m_bIsNormalized)
    {
        if (m_aBins.size()<=iBin)
        {
            while (m_aBins.size()<=iBin)
            {
                m_aBins.push_back( m_aBins[m_aBins.size()-1] + m_fWidth );
                m_aDensity.push_back(0.0f); 
            }
        }
        
        m_aDensity[iBin]++;
        m_fSum+=1.0f;
    }else
    {
        svtout << "Can't add pair distances for normalized PDFs" << endl;
        return;
    }
};

/**
* Add Bin
*/
void svt_pair_distribution::addBin(Real64 fBin)
{
    m_aBins.push_back( fBin );
};

/**
* Add Density
*/
void svt_pair_distribution::addDensity(Real64 fDensity)
{
    m_aDensity.push_back( fDensity );
};

/**
* Add Both Bin and Density
*/
void svt_pair_distribution::add(Real64 fBin, Real64 fDensity)
{
    addBin( fBin );
    addDensity( fDensity );
};

/**
 * Print
 **/ 
void svt_pair_distribution::print(char *pFilename, bool bAppend)
{
    if (pFilename==NULL)
    {
        unsigned int iSize = m_aDensity.size();
        for (unsigned int iIndex=0; iIndex < iSize; iIndex++)
        {
            cout << m_aBins[iIndex] << "\t" << m_aDensity[iIndex] << endl;
        }
        cout << endl;
    }
    else
    {        
        FILE* pFile;
        if (bAppend)
            pFile = fopen(pFilename,"a");
        else
            pFile = fopen(pFilename,"w");
        
        char pLine[1256];
         unsigned int iSize = m_aDensity.size();
        for (unsigned int iIndex=0; iIndex < iSize; iIndex++)
        {
            sprintf(pLine,"%f\t%f\n", m_aBins[iIndex], m_aDensity[iIndex]);
            fprintf(pFile,pLine);
        }
        fclose(pFile);
    }
}

/**
* Get the number of bins 
*/ 
int svt_pair_distribution::size()
{
    return m_aDensity.size();
};

/**
* Get function
*/ 
vector<Real64 > svt_pair_distribution::getPdf()
{
    return m_aDensity;
};

/**
* Get Bins
*/ 
vector<Real64 > svt_pair_distribution::getBins()
{
    return m_aBins;
};

/**
 * Get Width
 */ 
Real64 svt_pair_distribution::getWidth()
{
    return m_fWidth;
};

/**
 * Get Sum 
*/ 
Real64 svt_pair_distribution::getDistCount()
{
    return m_fSum;
};


/**
 *
 */
bool svt_pair_distribution::getNormalized()
{
    return m_bIsNormalized;
};

/**
 *
 */
void svt_pair_distribution::setNormalized(bool bIsNormalized)
{
    m_bIsNormalized = bIsNormalized;
}

/**
 * Set the distance matrix and (re)compute pdf
 */
void svt_pair_distribution::setDistanceMatrix(svt_matrix<Real64> & oDist)
{
    unsigned int iBin;
    
   //clean data if already existant - set default attributes except m_fWidth; 
    clear();
    m_fSum = 0.0f;
    m_bIsNormalized = false;
    setWidth( m_fWidth );
    setMin( 0.0f );

    unsigned int iHeight = oDist.height();
    unsigned int iWidth  = oDist.width();
    unsigned int iSize;
    
    m_oDist.resize(iHeight, iWidth );
    
    for (unsigned int iIndex1=0; iIndex1 < iHeight; iIndex1++)
    {
        //just the upper corner
        for(unsigned int iIndex2 = iIndex1+1; iIndex2 < iWidth; iIndex2++)
        { 
            m_oDist[iIndex1][iIndex2] = oDist[iIndex1][iIndex2];
            iBin = (int)( oDist[iIndex1][iIndex2] / m_fWidth );
            
            iSize = m_aBins.size();
            if (iSize<=iBin)
            {
                while (iSize<=iBin)
                {
                    m_aBins.push_back( m_aBins[iSize-1] + m_fWidth );
                    m_aDensity.push_back(0.0f); 
                    
                    iSize++;
                }
            }
            m_aDensity[iBin]++;
            m_fSum+=1.0f;
        }
    }
};




/**
* Clear
*/
void svt_pair_distribution::clear()
{
    if (m_aDensity.size()>0)
    {
        m_aBins.clear();
        m_aDensity.clear();
    }
};

/**
* Overload of the = operator
*/
void svt_pair_distribution::operator=(svt_pair_distribution that)
{
    vector<Real64 > aDensity = that.getPdf();
    vector<Real64 > aBins = that.getBins();
    
    m_aDensity.clear();
    m_aBins.clear();
    
    unsigned int iSize = aDensity.size();
    for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
    {
        m_aDensity.push_back(aDensity[iIndex]);
        m_aBins.push_back(aBins[iIndex]);
    }
    
    m_fSum = that.getDistCount();
    m_bIsNormalized = that.getNormalized();
    m_fWidth = that.getWidth();
    
    m_oDist = that.getDistanceMatrix();

};

/**
* Overload of the - operator
*/
svt_pair_distribution svt_pair_distribution::operator-(svt_pair_distribution& A)
{
    vector<Real64 > aDensity, aDensityA;
    vector<Real64 > aBins, aBinsA;
    unsigned int iCount; 

    aDensityA = A.getPdf();
    aBinsA = A.getBins();
    
    iCount = (size() < A.size())?size():A.size();
    
    //loop till the smaller Count
    for (unsigned int iIndex=0; iIndex<iCount; iIndex++)
    {
        if ( abs(m_aBins[iIndex]-aBinsA[iIndex])<0.2 )
        {
            aDensity.push_back( m_aDensity[iIndex] - aDensityA[iIndex]);
            aBins.push_back ( aBinsA[iIndex] );
        }
        else
        {
            svtout <<"ERROR: Bins are different. Please substract Pair Distance Functions with the same bins! " << m_aBins[iIndex] << ":" << aBinsA[iIndex] << endl;
            return svt_pair_distribution();
        }
    }
    
    int iSize = size();
    int iSizeA = A.size();
    if ( iSize < iSizeA)
    {
        
        for (int iIndex=iSize; iIndex < iSizeA; iIndex++)
        {
            aDensity.push_back( 0.0f-aDensityA[iIndex] );
            aBins.push_back ( aBinsA[iIndex] );
        }
    }
    else
    {
        for (int iIndex=iSizeA; iIndex < iSize; iIndex++)
        {
            aDensity.push_back( m_aDensity[iIndex] );
            aBins.push_back ( m_aBins[iIndex] );
        }
    }
    
    m_bIsNormalized = false; 

    return svt_pair_distribution( aDensity, aBins );
};

/**
 * Overload of the - operator
 * \param fValue to multiply with 
 */
void svt_pair_distribution::operator*=(Real64 fValue)
{
    unsigned int iSize = m_aDensity.size();
    for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
    {
        m_aDensity[iIndex]*=fValue;
    }
    
    m_fSum *= fValue;
};

/**
 * Overload of the + operator
 * careful that the pdf are not normalized 
 */
svt_pair_distribution svt_pair_distribution::operator+(svt_pair_distribution& A)
{
    vector<Real64 > aDensity, aDensityA;
    vector<Real64 > aBins, aBinsA;
    unsigned int iCount; 

    aDensityA = A.getPdf();
    aBinsA = A.getBins();
    
    iCount = (size() < A.size())?size():A.size();
    
    //loop till the smaller Count
    for (unsigned int iIndex=0; iIndex<iCount; iIndex++)
    {
        if (m_aBins[iIndex]==aBinsA[iIndex])
        {
            aDensity.push_back( m_aDensity[iIndex] + aDensityA[iIndex]);
            aBins.push_back ( aBinsA[iIndex] );
        }
        else
        {
            svtout <<"ERROR: Bins are different. Please add Pair Distance Functions with same bins!" << endl;
            return svt_pair_distribution();
        }
    }
    
    int iSize = size();
    int iSizeA = A.size();
    if ( iSize < iSizeA)
    {
        
        for (int iIndex=iSize; iIndex < iSizeA; iIndex++)
        {
            aDensity.push_back( 0.0f+aDensityA[iIndex] );
            aBins.push_back ( aBinsA[iIndex] );
        }
    }
    else
    {
        for (int iIndex=iSizeA; iIndex < iSize; iIndex++)
        {
            aDensity.push_back( m_aDensity[iIndex] );
            aBins.push_back ( m_aBins[iIndex] );
        }
    }
    
    m_bIsNormalized = false; 
    
    return svt_pair_distribution( aDensity, aBins );

};


/**UpiU
 * Another difference: weighted
 * \param svt_pair_distribution the distribution at the right side of the -
 */
svt_pair_distribution svt_pair_distribution::weightedDiff(svt_pair_distribution& A, Real64 fConst)
{
    vector<Real64 > aDensity, aDensityA;
    vector<Real64 > aBins, aBinsA;
    unsigned int iCount; 
    Real64 fDistS, fWeightedMSD;

    aDensityA = A.getPdf();
    aBinsA = A.getBins();
    
    iCount = (size() < A.size())?size():A.size();
    
    //loop till the smaller Count
    for (unsigned int iIndex=0; iIndex<iCount; iIndex++)
    {
        if ( abs(m_aBins[iIndex]-aBinsA[iIndex])<0.2 )
        {
            fDistS = (m_aDensity[iIndex] - aDensityA[iIndex])*(m_aDensity[iIndex] - aDensityA[iIndex]);
            if (fDistS>0)
                fWeightedMSD = exp( - (fConst*fConst*aDensityA[iIndex]*aDensityA[iIndex])/fDistS )*fDistS ;
            else 
               fWeightedMSD = 0;
            
            aDensity.push_back( fWeightedMSD );
            aBins.push_back ( aBinsA[iIndex] );
        }
        else
        {
            svtout <<"ERROR: Bins are different. Please substract Pair Distance Functions with the same bins!" << endl;
            return svt_pair_distribution();
        }
    }
    
    int iSize = size();
    int iSizeA = A.size();
    if ( iSize < iSizeA )
    {
        for (int iIndex=iSize; iIndex < iSizeA; iIndex++)
        {
            fDistS = (0 - aDensityA[iIndex])*(0 - aDensityA[iIndex]);
            if (fDistS>0)
                fWeightedMSD = exp( - (fConst*fConst*aDensityA[iIndex]*aDensityA[iIndex])/fDistS )*fDistS ;
            else 
               fWeightedMSD = 0;
            
            aDensity.push_back( fWeightedMSD );
            aBins.push_back ( aBinsA[iIndex] );
        }
    }
    else
    {
        for (int iIndex=iSizeA; iIndex < iSize; iIndex++)
        {
            
            fDistS = (m_aDensity[iIndex])*(m_aDensity[iIndex]);
            if (fDistS>0)
                fWeightedMSD = fDistS ;
            else 
               fWeightedMSD = 0;
            aDensity.push_back( fWeightedMSD );
            aBins.push_back ( m_aBins[iIndex] );
        }
    }
    
    m_bIsNormalized = false; 

    return svt_pair_distribution( aDensity, aBins );
};

/**
 * Normalize by deviding the count of distance pairs with the total number of distance pairs
 */
void svt_pair_distribution::normalize()
{
    if (m_bIsNormalized)
        return;
   
    m_bIsNormalized = true; 
    
    unsigned int iSize = m_aDensity.size();
    if (m_fSum==0.0)
    {
        m_fSum = 0.0;
        for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
            m_fSum += m_aDensity[iIndex];
    }
            
   
    for (unsigned int iIndex=0; iIndex< iSize; iIndex++)
        m_aDensity[iIndex] = m_aDensity[iIndex] / m_fSum;

};

/**
 * Load PDF from a GNOM file
 */
void svt_pair_distribution::loadGnom(const char* pFilename)
{
    svtout << "Load pair distribution function from Gnom file " << endl;
    
    clear();
    
    m_fSum = 0.0;
    m_bIsNormalized = false;
    
//    svtout << "Clear all" << endl;
    
    FILE *pFile = fopen(pFilename, "r");
    if( pFile == NULL )
    {
        char pFname[2560];
        sprintf(pFname, "Can't open GNOM file: \"%s\"!", pFilename );
        svt_exception::ui()->error(pFname);
    }

    // do not read all GNOM file content but just the PDF
    char pLable1[1256];
    char pLable2[1256];
    char pLine[1256];
        
    
    while (fgets(pLine, 1256, pFile)!=NULL)
    {
        sscanf(pLine, "%s %s", pLable1, pLable2);
        if (strcmp(pLable1,"R")==0 && strcmp(pLable2,"P(R)")==0)
            break;
    }
    
    //read empty line
    fgets(pLine, 1256, pFile);
    
    Real64 fBin,fDensity;
    while (fgets(pLine, 1256, pFile)!=NULL && sscanf(pLine, "%le %le", &fBin, &fDensity)==2) // not end of file and 2 numbers could be read from file
    {
 //       svtout << fBin << "\t" << fDensity << endl;
        m_aBins.push_back( fBin );
        m_aDensity.push_back( fDensity );
    }
    
    fclose(pFile);
    
    if (m_aBins.size()>=2)
        m_fWidth = m_aBins[1]-m_aBins[0];
};

/**
 * get Norm squared
 * \param fConst - constant value; if not null is used in the weighting of the distance square
 */
Real64 svt_pair_distribution::getNormSq()
{
    Real64 fNormSq = 0;
    
    unsigned int iSize = m_aDensity.size();
    for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
        fNormSq += m_aDensity[iIndex]*m_aDensity[iIndex];

    return fNormSq;    
};

/**
 * get Distance squared
 * \param A the other pair distance distribution
 * \param fConst - constant value; if not null is used in the weighting of the distance square
 */
Real64 svt_pair_distribution::distanceSq(svt_pair_distribution& A,  Real64 fConst)
{
    Real64 fDistSq;
    svt_pair_distribution oDiff;
    
    if (fConst==0.0f)
    {
        oDiff = (*this) - A;
        fDistSq = oDiff.getNormSq();
    }
    else
    {
        oDiff = (*this).weightedDiff(A, fConst);
        fDistSq = oDiff.getAverage()*oDiff.size();
    }
    
    
    return fDistSq;
};

/**
 * Get average = sum of bin values devided by the bin number
 */
Real64 svt_pair_distribution::getAverage()
{
    Real64 fSum=0.0f;
    
    unsigned int iSize = m_aDensity.size();
    for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
        fSum += m_aDensity[iIndex];
    
    return fSum/(Real64)m_aDensity.size();
};

/**
 * Get sum = sum of bin values
 */
Real64 svt_pair_distribution::getSum()
{
    Real64 fSum=0.0f;
    
    unsigned int iSize = m_aDensity.size();
    for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
        fSum += m_aDensity[iIndex];
    
    return fSum;
};

/**
 * Get deviation from value 
 */
Real64 svt_pair_distribution::getDeviation( Real64 fValue )
{

    Real64 fSum = 0.0f;
    unsigned int iSize = m_aDensity.size();
    for (unsigned int iIndex=0; iIndex < iSize; iIndex++)
    {
        fSum += abs(m_aDensity[iIndex] - fValue);
    }
    
    fSum /= m_aDensity.size();
    
    return fSum;
};

void svt_pair_distribution::writeEPS(char* pFname_R_script, char* pFname_img, char*  pFname_ref, char* pFname_target)
{
    FILE * pFile;
    char pCmdLn[1256];
    
    pFile = fopen(pFname_R_script,"w");
    sprintf(pCmdLn,"postscript(\"%s\");\ntab_orig=read.table(\"%s\");\ntab=read.table(\"%s\");\n", pFname_img, pFname_ref, pFname_target);
    fprintf(pFile,pCmdLn);
    fprintf(pFile,"plot(tab_orig$V1,tab_orig$V2,t=\"l\",xlab=\"R\", ylab=\" PDF(target) - PDF(model)\",col=2,lwd=2);\nlines(tab$V1,tab$V2,col=3,lwd=2);\nabline(h=0);\n");
    sprintf(pCmdLn,"dev.off();\nquit(\"yes\");");
    fprintf(pFile,pCmdLn);
    fclose(pFile);
    
    sprintf(pCmdLn,"R CMD BATCH %s",pFname_R_script);
    svt_system(pCmdLn);

}
