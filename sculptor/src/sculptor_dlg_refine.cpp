/***************************************************************************
                          sculptor_dlg_refine.cpp
                          -----------------
    begin                : 11.01.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_refine.h>

/**
 * Constructor
 */
dlg_refine::dlg_refine(QWidget* pParent)
    : QDialog( pParent )
{
    setupUi(this);

    // if user hits OK button, check if the string has correct PDB ID format
    //connect( m_pCompute, SIGNAL(clicked()), this, SLOT( computeRMSD() ) );
}

/**
 * Destructor
 */
dlg_refine::~dlg_refine()
{
}


/**
 * get Resolution
 * \return the resolution
 */
Real64 dlg_refine::getResolution()
{
    return m_pResolution->text().toDouble();
};

/**
 * get maximum number of iterations
 * \return the number of iterations
 */
Real64 dlg_refine::getMaxIter()
{
    return m_pMaxIter->text().toDouble();
};


/**
 * get the tolerance 
 * \return the tolerance 
 */
Real64 dlg_refine::getTolerance()
{
    return m_pTolerance->text().toDouble();
};

/**
 * should the translation be optimized 
 * \param iAxis = 0 ~ X (default); = 1~y; = 2~z
 */
bool dlg_refine::getOptTrans(int iAxis)
{
    bool bIsChecked;

    switch( iAxis )
    {   
       
        case 1:
            bIsChecked = m_pOptTransY->isChecked();
         break;

        case 2:
            bIsChecked = m_pOptTransZ->isChecked();
         break;

        default:
        case 0:
            bIsChecked = m_pOptTransX->isChecked();
        break;
    }

    return bIsChecked; 
};

/**
 * should the rotation be optimized 
 * \param iAxis = 0 ~ X (default); = 1~y; = 2~z
 */
bool dlg_refine::getOptRot(int iAxis)
{
    bool bIsChecked;
    switch( iAxis )
    {   
        case 1:
            bIsChecked = m_pOptRotY->isChecked();
         break;

        case 2:
            bIsChecked = m_pOptRotZ->isChecked();
         break;

        default:
        case 0:
            bIsChecked = m_pOptRotX->isChecked();
        break;
    }

    return bIsChecked; 
};

/**
 * get cv sampling
 */
bool dlg_refine::getCVSampling()
{
    return m_pCVSampling->isChecked(); 
};

/**
 * set cv sampling
 */
void dlg_refine::setCVSampling( bool bChecked )
{
    m_pCVSampling->setChecked( bChecked );
};

/**
 * get union sphere 
 */
bool dlg_refine::getUnionSphere()
{
    return m_pUnionSphere->isChecked(); 
};

/**
 * set union sphere 
 */
void dlg_refine::setUnionSphere( bool bChecked )
{
    m_pUnionSphere->setChecked( bChecked ); 
};

/**
 * get Correlation
 */
bool dlg_refine::getCorrelation()
{
    return m_pCorrelation->isChecked(); 
};

/**
 * set Correlation
 */
void dlg_refine::setCorrelation( bool bChecked )
{
    m_pCorrelation->setChecked( bChecked );
};

/**
 * get laplace 
 */
bool dlg_refine::getLaplace()
{
    return m_pLaplace->isChecked();
};

/**
 * set laplace 
 */
void dlg_refine::setLaplace( bool bChecked )
{
    m_pLaplace->setChecked( bChecked );
};


