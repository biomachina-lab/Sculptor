/***************************************************************************
                          svt_scattering_function
			  ----------
    begin                : August 27 2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/


#include <svt_scattering_function.h>
#include <svt_exception.h>

using namespace std;


/**
 * Constructor
 */
svt_scattering_function::svt_scattering_function():
    m_iCount(0),
    m_fMinX( 0.0f ),
    m_fMaxX( 0.0f ),
    m_fIncX( 0.1f ),
    m_bIsLog( false ),
    m_fWidth( 0.0f ),
    m_bIsSinComputed(false)
{
    m_oParams.push_back(1.0f);
    m_oParams.push_back(0.0f);
};

/**
 * Should the values of the intensity be logarithmic
 */
void svt_scattering_function::setIsLog(bool bIsLog)
{
    if (m_bIsLog==false && bIsLog==true)
    {
        for (unsigned int iIndex=0; iIndex<m_oY.size(); iIndex++)
            if (m_oY[iIndex]>0)
            m_oY[iIndex] = log(m_oY[iIndex]);
    }else
    {
        if (m_bIsLog==true && bIsLog==false)
        {
            for (unsigned int iIndex=0; iIndex<m_oY.size(); iIndex++)
                m_oY[iIndex] = exp(m_oY[iIndex]);
        }
    }
    m_bIsLog = bIsLog;
}

/**
 * Calc the values of the intensity
 */
void svt_scattering_function::calcFunction()
{
    //error checking : were all the variable assigned ?
    if (m_iCount==0)
    {
        svt_exception::ui()->error("Please assign the number of spheres/atoms in the model");
        exit(0); 
    }
    
    //clear the previous content
    m_oY.clear();
    m_oX.clear();
    m_oErr.clear();
    
    
    //Debye formula
    
    vector<Real64> aDensity = m_oPDF.getPdf();
    vector<Real64> aBins = m_oPDF.getBins();
    
     unsigned int iBinCount = m_oPDF.size() ;
    if (iBinCount==0)
    {
        svt_exception::ui()->error("Error: Please assign a pdf");
        exit(0);
    }
    
    
    //first compute the Y (0) - I(0); used as a normalization factor such that all scattering start at 1 by dividing: I(s)/I(0)
    Real64 fYzero=m_iCount;
    for (unsigned int iIndex=0; iIndex < iBinCount; iIndex++ )
    {
        fYzero += 2.0f*aDensity[iIndex];
    }
    
    
    if ( (int)m_oSins.oSin.height() < int((m_fMaxX-m_fMinX)/m_fIncX)+1 || m_oSins.oSin.width()< aBins.size() )
    {
        m_oSins.oSin.resize( int((m_fMaxX-m_fMinX)/m_fIncX)+1,aBins.size(),svt_matrix<Real64>::Save, -10.0f);
    }
    
    Real64 fArg, fY;
    vector<Real64> aSin;
    unsigned int iIndexVec=0;
    for (Real64 fX=m_fMinX; fX <= m_fMaxX; fX+=m_fIncX )
    {
        fY = m_iCount;
        aSin.clear();
        for (unsigned int iIndex=0; iIndex < iBinCount; iIndex++ )
        {
            
            fArg = 2.0f*PI*fX*aBins[iIndex];
            if (!m_bIsSinComputed)
            {
                if (fArg==0.0f)
                    m_oSins.oSin[iIndexVec][iIndex] = 1;
                else
                    m_oSins.oSin[iIndexVec][iIndex] = sin(fArg)/fArg;
                    
                 
            }
            
            if (fArg!=0.0f)
            {
                fY += 2.0f*aDensity[iIndex]*m_oSins.oSin[iIndexVec][iIndex];
            }else
            {
                //cause lim(x->0) (sin(x)/x=1)
                fY += 2.0f*aDensity[iIndex];
            }
        }
                
        //normalize by Y(0) => m_oY(0)=1 
        fY /= fYzero;
        
        if (m_bIsLog)
            m_oY.push_back( log(fY) );
        else 
            m_oY.push_back( fY );
        
        m_oX.push_back( fX );
        m_oErr.push_back( 0.0f );
        
        iIndexVec++;
    }
    
    
    
    m_oSins.aBin = aBins;
    m_oSins.aScatteringVector = m_oX;
};


/**
 * compute the scattering intensities of a PDB 
 * \param oPDB the structure for which to compute the scattering intensities
 */
vector<Real64> svt_scattering_function::calcSAXSIntensity(svt_point_cloud_pdb< svt_vector4<Real64> >& oPDB )
{
    m_oPDF = oPDB.getPairDistribution();
    m_fWidth = m_oPDF.getWidth();
    
    //check if the return variable actually contains any data: if not compute the pdf
    if (m_oPDF.size()==1)
    {
        //error check
        if (m_fWidth==0.0f)
        {
            svt_exception::ui()->error("To compute the scattering intensity, please set the width of the PDF!");
            exit(0);
        }
        
        oPDB.calcPairDistribution(m_fWidth);
        m_oPDF = oPDB.getPairDistribution();
    }
    
    setCount( oPDB.size() );
    
    calcFunction();
    
    return m_oY;
};


/**
 * compute the scattering intensities of a PDF 
 * \param oPDF the pair distance distribution to compute the scattering intensities
 */
vector<Real64> svt_scattering_function::calcSAXSIntensity(svt_pair_distribution& oPDF, unsigned int iAtoms )
{
    m_oPDF = oPDF;
    m_fWidth = m_oPDF.getWidth();
    
    //error check
    if (m_fWidth==0.0f)
    {
        svt_exception::ui()->error("To compute the scattering intensity, please set the width of the PDF!");
        exit(0);
    }
    
    setCount( iAtoms );
    
    calcFunction();
    
    return m_oY;
};

/**
 * Computes the differrence-fitness function between two scattering intensities
 * \param the other scattering intensity function
 * \param type of the score 
 */
Real64 svt_scattering_function::getFitness(svt_scattering_function &oTar, svt_scatteringScoreType type )
{
    vector<Real64> oX, oY;
    
    oX = oTar.x();
    oY = oTar.y();
    //bool bIsLog = oTar.getIsLog();
    
    unsigned int iSize;
    iSize = oX.size()<m_oX.size()?oX.size():m_oX.size(); // the minimum of the two length
    
    //Real64 fLogIsTar, fLogIsModel;
    Real64 fSum=0.0f, fF;
    
   vector<Real64> fCorr = (*this).getCorrelation( oTar );
    
    //FIXME: what happens when the vectors don't have the same number of elements
//     for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
//     {
//         if (oX[iIndex]==m_oX[iIndex] )
//         {
//             if ((!bIsLog && oY[iIndex]>0) && (!m_bIsLog && m_oY[iIndex]>0))
//             {
//                 if (bIsLog)
//                     fLogIsTar = oY[iIndex]; //the log was already applied
//                 else 
//                     fLogIsTar = log(oY[iIndex]);
//                 
//                 if (m_bIsLog)
//                     fLogIsModel = m_oY[iIndex]; //the log was already applied
//                 else 
//                     fLogIsModel =  log(m_oY[iIndex]);
//                 
//                 switch( type )
//                 {
//                     case REC_RS:
//                     case RS:
//                     case REC_RMS:
//                     case RMS_LOG:
//                         fF = (fLogIsTar-fLogIsModel)*(fLogIsTar-fLogIsModel);
//                         break;
//                     case REC_R_FACTOR:
//                     case R_FACTOR:
//                         fF = abs((fLogIsTar-fLogIsModel)/fLogIsTar);
//                         break;
//                     case RMS:
//                         fF = pow(m_oX[iIndex]/m_oX[m_oX.size()-1],3)*(oY[iIndex]-m_oY[iIndex])*(oY[iIndex]-m_oY[iIndex]);
//                         break;
//                 }
//                 
//                 fSum += fF;
//             }
//         }
//         else
//         {
//            svt_exception::ui()->error("Can't compute fitness function because the values on the x axis are different!");
//            exit(0); 
//         }
//     }  
    
   // Real64 fSum=0.0f;
    Real64 fSumLog  = 0.0f;
    Real64 fSumLog2 = 0.0f;
    Real64 fSumAbs  = 0.0f;
    
    Real64 fPow1  = m_oParams[0];
    Real64 fCoef1 = m_oParams[1];
    
   //svtout << fPow1 << fCoef1 << endl;
    
    for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
    {
        if ((oX[iIndex]-m_oX[iIndex])<1e-6 )
        {
            if (fPow1!=0)
                fF = abs(oY[iIndex]-m_oY[iIndex])*pow(m_oX[iIndex]/m_oX[m_oX.size()-1],fPow1);
            else
                fF = abs(oY[iIndex]-m_oY[iIndex]);
            fSumAbs += fF;
            
            if (fPow1!=0)
                fF = (oY[iIndex]-m_oY[iIndex])*(oY[iIndex]-m_oY[iIndex])*pow(m_oX[iIndex]/m_oX[m_oX.size()-1],fPow1);
            else
                fF = (oY[iIndex]-m_oY[iIndex])*(oY[iIndex]-m_oY[iIndex]);
            fSum += fF;
            
            fF = ( log(oY[iIndex])-log(m_oY[iIndex]) )*( log(oY[iIndex])-log(m_oY[iIndex]) );
            fSumLog += fF;
            
            if (log(oY[iIndex])!=0)
            {
                fF = abs( (log(oY[iIndex])-log(m_oY[iIndex])) /log(oY[iIndex]));
                fSumLog2 += fF; 
            }
            
        }
        else
        {
           svt_exception::ui()->error("Can't compute fitness function because the values on the x axis are different!");
           exit(0); 
        }
    }
    
    //cout << type << "\t" << fPow1 << "\t" << fCoef1 << "CC: " << 1.0f-fCorr[0] << " Abs: " << sqrt(fSumAbs/(Real64)m_iCount) << " rms: " << sqrt(fSum/(Real64)m_iCount)  << "\t" << " log: " << sqrt(fSumLog/(Real64)m_iCount)<< " log2: "<<  sqrt(fSumLog2/(Real64)m_iCount) << "\n";
    
    switch( type )
    {
        case IS_RMS://3
            fSum = sqrt(fSum/(Real64)m_iCount);
            break;
        case IS_LOG: //4
            fSum = sqrt(fSumLog/(Real64)m_iCount);
            break;
        case IS_LOG2: //5 
            fSum = sqrt(fSumLog2/(Real64)m_iCount);
            break;
        case IS_ABS://6
            fSum = sqrt(fSumAbs/(Real64)m_iCount);
            break;
        case IS_CORR://7
            fSum = 1.0f-fCorr[0];
            break; 
        case IS_CC_ABS_P3://8
            fSum = fCoef1*(1.0f-fCorr[0])+sqrt(fSumAbs/(Real64)m_iCount);
            break;
    }
    
    //cout << fSum << "\t";

    return fSum;
};

/**
 * compute the correlation of two scattering intensities functions
 * \param the other scattering intensity function
 */
vector<Real64> svt_scattering_function::getCorrelation(svt_scattering_function &A)
{
    vector<Real64> oX = A.x();
    vector<Real64> oY = A.y();
    
    unsigned int iSize = m_oY.size();
    
    Real64 fSumX=0.0f, fSumXX=0.0f ;
    Real64 fSumY=0.0f, fSumYY=0.0f;
    Real64 fSumXY=0.0f;
            
    for (unsigned int iIndex=0; iIndex < iSize ; iIndex++)
    {
        fSumX += m_oY[iIndex];
        fSumY += oY[iIndex];
        fSumXY += m_oY[iIndex]*oY[iIndex];
        fSumXX += m_oY[iIndex]*m_oY[iIndex];
        fSumYY += oY[iIndex]*oY[iIndex];
    }
    
    Real64 fCovXY = fSumXY - fSumX*fSumY/Real64(iSize);
    Real64 fVarX  = fSumXX - fSumX*fSumX/Real64(iSize);
    Real64 fVarY  = fSumYY - fSumY*fSumY/Real64(iSize);
    
    vector<Real64> oRes;
    
    // r
    oRes.push_back( fCovXY/sqrt(fVarX*fVarY) ); 
    
    // slope
    oRes.push_back( fCovXY/fVarX );
      
    //intercept
    oRes.push_back( fSumY/Real64(iSize)-oRes[1]*fSumX/Real64(iSize) );
    
    return oRes;

};



/**
 * print the scattering intensities
 */
void svt_scattering_function::print( char *pFname)
{
    if (pFname==NULL)
    {
        for (unsigned int iIndex=0; iIndex<m_oY.size(); iIndex++)
        {
            printf("%10.8f %10.8f %10.8f\n", m_oX[iIndex], m_oY[iIndex], m_oErr[iIndex]);
        }
    }else
    {
        FILE * file;
        file = fopen( pFname, "w" );
        for (unsigned int iIndex=0; iIndex<m_oY.size(); iIndex++)
        {
            fprintf(file, "%10.8f %10.8f %10.8f\n", m_oX[iIndex], m_oY[iIndex],  m_oErr[iIndex] );
        }
        
        fclose( file );
    }
};
    

/**
 * Overload of the = operator
 */
void svt_scattering_function::operator=(svt_scattering_function that)
{
    vector<Real64> oX = that.x();
    vector<Real64> oY = that.y();
    vector<Real64> oErr = that.err();
    
    m_oX.clear();
    m_oY.clear();
    m_oErr.clear();
    
    for (unsigned int iIndex=0; iIndex < oY.size(); iIndex++)
    {
        m_oX.push_back(oX[iIndex]);
        m_oY.push_back(oY[iIndex]);
        m_oErr.push_back(oErr[iIndex]);
    }
    
    m_fMinX = that.getMinX();
    m_fMaxX = that.getMaxX();
    m_fIncX = that.getIncrementX();
    m_iCount = that.getCount();
    
    m_bIsLog = that.getIsLog();
    
    m_oParams = that.getParams();
};



/**
 * Load data from Gnom file
 * \param PFname the name of the gnom file
 */
void svt_scattering_function::loadGnom(char* pFilename)
{
    svtout << "Load scattering intensities from Gnom file " << endl;
    
    m_oY.clear();
    m_oX.clear();
    m_oErr.clear();
    
    FILE *pFile = fopen(pFilename, "r");
    if( pFile == NULL )
    { 
        char pFname[2560];
        sprintf(pFname, "Can't open GNOM file: \"%s\"!", pFilename );
        svt_exception::ui()->error(pFname);
    }
    
    char pLine[1256];
    
    Real64 fS,fI, fError;
    while (fgets(pLine, 1256, pFile)!=NULL && sscanf(pLine, "%le %le %le", &fS, &fI, &fError )==3) // not end of file and 3 numbers could be read from file
    {
        m_oX.push_back( fS );
        m_oY.push_back( fI );
        m_oErr.push_back( fError );
    }
    
    fclose(pFile);
    
    m_fMinX = m_oX[0];
    m_fMaxX = m_oX[m_oX.size()-1];
    
    //suppose uniform S
    if (m_oX.size() > 2)
        m_fIncX = m_oX[1]-m_oX[0];
};

/**
 * Load data from file normalized with R
 * first column is the row number, second is the scattering angles and the third is the scattering intensities
 * \param PFname the name of the gnom file
 */
void svt_scattering_function::loadR(char* pFilename)
{
    
    svtout << "Load scattering intensities fromfile " << endl;
    
    m_oY.clear();
    m_oX.clear();
    m_oErr.clear();
    
    FILE *pFile = fopen(pFilename, "r");
    if( pFile == NULL )
    { 
        char pFname[2560];
        sprintf(pFname, "Can't open file: \"%s\"!", pFilename );
        svt_exception::ui()->error(pFname);
    }
    
    char pLine[1256];
    
    Real64 fS,fI, fError;
    while (fgets(pLine, 1256, pFile)!=NULL && sscanf(pLine, "%le %le %le", &fError, &fS, &fI )==3) // not end of file and 3 numbers could be read from file
    {
        m_oX.push_back( fS );
        m_oY.push_back( fI );
        m_oErr.push_back( fError );
    }
    
    fclose(pFile);
    
    m_fMinX = m_oX[0];
    m_fMaxX = m_oX[m_oX.size()-1];
    
    //suppose uniform S
    if (m_oX.size() > 2)
        m_fIncX = m_oX[1]-m_oX[0];

};

/**
 * Get MaxY
 */
Real64 svt_scattering_function::getMaxY()
{
    unsigned int iSize = m_oY.size();
    Real64 fMax = 0.0f;
    for (unsigned int iIndex = 0; iIndex < iSize; iIndex++)
    {
        if (m_oY[iIndex]>fMax)
            fMax = m_oY[iIndex];
    }
    
    return fMax;
};

/**
 * Normalize = devide intensities by max intensity
 */
void svt_scattering_function::normalize()
{
    Real64 fMax = getMaxY();
    
    unsigned int iSize = m_oY.size();

    if (fMax != 0.0f)
    {
        for (unsigned int iIndex = 0; iIndex < iSize; iIndex++)
            m_oY[iIndex]=m_oY[iIndex]/fMax;
    }
    else
    {
        svtout << "Can't normalize by a null value (max=0)! Please verify your data." << endl;
        exit(0);
    }
};
