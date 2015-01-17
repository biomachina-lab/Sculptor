/***************************************************************************
                          sculptor_dlg_gacylinder.cpp
                          -----------------
    begin                : 09.10.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_gacylinder.h>
#include <sculptor_window.h>
#include <QFileDialog>

extern sculptor_window* g_pWindow;
class sculptor_window;
struct sculptor_scene;
/**
 * Constructor
 */
dlg_gacylinder::dlg_gacylinder(QWidget* pParent, svt_node *pDataTopNode)
    : QDialog( pParent ),
     m_pDataTopNode(pDataTopNode)
{
    setupUi(this);
    setDefaultParameters();
    connect( m_pShowExpansionTemplate, SIGNAL( stateChanged(int) ), this, SLOT( createExpansionTemplate() ) );
    connect( m_pShowSearchTemplate, SIGNAL( stateChanged(int) ), this, SLOT( createSearchTemplate() ) );
    connect( m_pDetectHelices, SIGNAL( clicked (bool) ), this, SLOT( setDefaultParameters() ) );
    connect( m_pDetectFilaments, SIGNAL( clicked (bool) ), this, SLOT( setDefaultParameters() ) );
}

/**
 * Destructor
 */
dlg_gacylinder::~dlg_gacylinder()
{
}
/**
 * get the number of helices
 */
int dlg_gacylinder::getObjectCount()
{
    return m_pHelicesCount->value();
};

/**
 * set the number of objects
 */
void dlg_gacylinder::setObjectCount(Real64 fObjectCount)
{
    m_pHelicesCount->setValue(fObjectCount);
};

 /**
 * get the resolution
 */
Real64 dlg_gacylinder::getResolution()
{
    return m_pRes->text().toDouble();
};

/**
 * set the resolution
 */
void dlg_gacylinder::setResolution(Real64 fRes)
{
    QString oStr;
    oStr.setNum(fRes,'g','2');
    m_pRes->setText(oStr);
};


/**
 * get the radius
 */
Real64 dlg_gacylinder::getExpansionTemplateRadius()
{
    return m_pExpansionTemplateRadius->text().toDouble();
};

/**
 * set the radius
 */
void dlg_gacylinder::setExpansionTemplateRadius(Real64 fRadius)
{
    QString oStr;
    oStr.setNum(fRadius);
    m_pExpansionTemplateRadius->setText(oStr);

};

/**
 * get the radius
 */
Real64 dlg_gacylinder::getSearchTemplateRadius()
{
    return m_pSearchTemplateRadius->text().toDouble();
};

/**
 * set the radius
 */
void dlg_gacylinder::setSearchTemplateRadius(Real64 fRadius)
{
    QString oStr;
    oStr.setNum(fRadius);
    m_pSearchTemplateRadius->setText(oStr);
};

/**
 * get the length of the Search Template 
 */
int dlg_gacylinder::getSearchTemplateRepeats()
{
    return m_pSearchTemplateLength->value();
};

/**
 * set the Repeats of the expansion Template
 */
void dlg_gacylinder::setSearchTemplateRepeats(int iRepeats)
{   
    m_pSearchTemplateLength->setValue(iRepeats);
};



/**
 * get the length of the Search Template 
 */
int dlg_gacylinder::getExpansionTemplateRepeats()
{
    return m_pExpansionTemplateLength->value();
};

/**
 * set the Repeats of the expansion Template
 */
void dlg_gacylinder::setExpansionTemplateRepeats(int iRepeats)
{
    m_pExpansionTemplateLength->setValue(iRepeats);
};

/**
 * get the population size
 */
int dlg_gacylinder::getPopSize()
{
    return m_pPopSize->value();
};
/**
 * set the popsize
 */
void dlg_gacylinder::setPopSize( unsigned int iPopSize)
{
    m_pPopSize->setValue(iPopSize); 
}; 

/**
 * get the maximum number of generations
 */
int dlg_gacylinder::getMaxGen()
{
    return m_pMaxGen->value();
};

/**
 * get the number of threads
 */
int dlg_gacylinder::getMaxThread()
{
    return m_pMaxThread->value();
};

/**
 * get the generation when to syncronize
 */
int dlg_gacylinder::getSyncGen()
{
    return m_pSyncGen->value();
};

/**
 * set the generation when to syncronize
 */
void dlg_gacylinder::setSyncGen(int iSyncGen)
{
    m_pSyncGen->setValue(iSyncGen);
};

/**
 * get the crawling step
 */
Real64 dlg_gacylinder::getCrawlingStep()
{
    return m_pCrawlingStep->text().toDouble();
};

/**
 * set the crawling step
 */
void dlg_gacylinder::setCrawlingStep(Real64 fCrawlingStep)
{
    QString oStr;
    oStr.setNum(fCrawlingStep);
    m_pCrawlingStep->setText(oStr);
};

 /**
 * get the accept move ratio
 */
Real64 dlg_gacylinder::getAcceptMoveRatio()
{
    return m_pAcceptMoveRatio->text().toDouble();
};

/**
 * set the accept move ratio
 */
void dlg_gacylinder::setAcceptMoveRatio(Real64 fRatio)
{
    QString oStr;
    oStr.setNum(fRatio,'g','2');
    m_pAcceptMoveRatio->setText(oStr);
};

 /**
 * get the tabu regions size
 */
Real64 dlg_gacylinder::getTabuRegionSize()
{
    return m_pTabuRegionSize->text().toDouble();
};
 
/**
 * set the tabu regions size
 */
void dlg_gacylinder::setTabuRegionSize(Real64 fTabuRegionSize)
{   
    QString oStr;
    oStr.setNum(fTabuRegionSize,'g','2');
    m_pTabuRegionSize->setText(oStr);
};

/**
 * get voxel width 
 */
Real64 dlg_gacylinder::getVoxelWidth()
{
    return m_pVoxelWidth->text( ).toDouble();
};

/**
 * set voxel width 
 */
void dlg_gacylinder::setVoxelWidth(Real64 fVoxelWidth)
{
    QString oWidth;
    oWidth.setNum(fVoxelWidth,'g',8); 
    m_pVoxelWidth->setText( oWidth );
};

/**
 * get the anisotropic correction factor
 */
Real64 dlg_gacylinder::getAni()
{
    return m_pAni->text().toDouble();
};

/**
 * set the anisotropic correction factor
 */
void dlg_gacylinder::setAni(Real64 fAni)
{
    QString oStr;
    oStr.setNum(fAni,'g',8); 
    m_pAni->setText( oStr );
};

/*
 * get lambda for correction
 */
Real64 dlg_gacylinder::getLambda()
{
    return m_pLambda->text().toDouble();
};

/*
 * set lambda for correction
 */
void dlg_gacylinder::setLambda( Real64 fLambda )
{
    QString oStr;
    oStr.setNum(fLambda,'g',8); 
    m_pLambda->setText( oStr );
};

/**
 * get distance between repeats 
 */
Real64 dlg_gacylinder::getDistBetweenRepeats()
{
    return m_pDistanceBetweenRepeats->text().toDouble();

};

/**
 * set distance between repeats 
 */
void dlg_gacylinder::setDistBetweenRepeats(Real64 fDist)
{
    QString oStr;
    oStr.setNum(fDist,'g',8); 
    m_pDistanceBetweenRepeats->setText( oStr );

};


/**
 * Create document with the expansion template
 */
void dlg_gacylinder::createExpansionTemplate()
{
    if (m_pShowExpansionTemplate->isChecked())
    {
        svt_point_cloud_pdb< svt_vector4<Real64> > oTemplate;
        oTemplate.makeCylinder(31, getExpansionTemplateRadius(), getExpansionTemplateRepeats(), getDistBetweenRepeats());
        oTemplate.setSecStructAvailable( true );

        pdb_document *pDoc = new pdb_document(m_pDataTopNode, "" , SVT_NONE, "",SVT_NONE, &oTemplate);
        g_pWindow->addDocSimple( (sculptor_document*)pDoc, "Expansion Template" );

        svt_vector4< Real64> oCenter = oTemplate.coa();

        // install new transformation
        ((svt_pdb*)pDoc->getNode())->setPos( new svt_pos(
                                    (1.0E-2) * oCenter.x(),
                                    (1.0E-2) * oCenter.y(),
                                    (1.0E-2) * oCenter.z()
                                   ) );

        pDoc->storeInitMat();

        svt_vector4f oDocOrigin = pDoc->getRealPos();
        oDocOrigin.multScalar( 1.0E8f );
        pDoc->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
        pDoc->storeInitMat();
    }
}

/**
 * Create document with the search template
 */
void dlg_gacylinder::createSearchTemplate()
{
    if (m_pShowSearchTemplate->isChecked())
    {
        svt_point_cloud_pdb< svt_vector4<Real64> > oTemplate;
        oTemplate.makeCylinder(31, getSearchTemplateRadius(), getSearchTemplateRepeats(), getDistBetweenRepeats());
        oTemplate.setSecStructAvailable( true );

        pdb_document *pDoc = new pdb_document(m_pDataTopNode, "" , SVT_NONE, "",SVT_NONE, &oTemplate);
        g_pWindow->addDocSimple( (sculptor_document*)pDoc, "Search Template" );

        svt_vector4< Real64> oCenter = oTemplate.coa();

        // install new transformation
        ((svt_pdb*)pDoc->getNode())->setPos( new svt_pos(
                                    (1.0E-2) * oCenter.x(),
                                    (1.0E-2) * oCenter.y(),
                                    (1.0E-2) * oCenter.z()
                                   ) );

        pDoc->storeInitMat();

        svt_vector4f oDocOrigin = pDoc->getRealPos();
        oDocOrigin.multScalar( 1.0E8f );
        pDoc->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
        pDoc->storeInitMat();
    }
 };

/**
 * Switch between tracing helices and filopodia
 */
void dlg_gacylinder::setDefaultParameters()
{
    if (m_pDetectHelices->isChecked())
    {
        svtout << "Detect Helices" << endl;
        m_iObjectCount              = 20;
        m_fRes                      = 8.0;
        m_fAni                      = 1.0;
        m_fAcceptedMoveRatio        = 70.0;
        m_fExpansionTemplateRadius  = 1.0;
        m_fSearchTemplateRadius     = 2.0;
        m_iExpansionTemplateRepeats = 8;
        m_iSearchTemplateRepeats    = 20;
        m_iPopSize                  = 100;
        m_iSyncGen                  = 100;
        m_fCrawlingStep             = 1.4;
        m_fTabuRegionSize           = 6;
        m_fDistBetweenRepeats       = 1;
        m_fLamdba                   = 1;
    }

    if (m_pDetectFilaments->isChecked())
    {
        svtout << "Detect Filaments" << endl;
      
        m_iObjectCount              = 100;
        m_fRes                      = 50.0;
        m_fAni                      = 2.0;
        m_fAcceptedMoveRatio        = 80.0;
        m_fExpansionTemplateRadius  = 12.0;
        m_fSearchTemplateRadius     = 24.0;
        m_iExpansionTemplateRepeats = 20;
        m_iSearchTemplateRepeats    = 50;
        m_iPopSize                  = 200;
        m_iSyncGen                  = 200;
        m_fCrawlingStep             = 50;
        m_fTabuRegionSize           = 60;
        m_fDistBetweenRepeats       = 10;
        m_fLamdba                   = 0.75;
   }

    setObjectCount              ( m_iObjectCount );
    setResolution               ( m_fRes );
    setAni                      ( m_fAni );
    setAcceptMoveRatio          ( m_fAcceptedMoveRatio );
    setExpansionTemplateRadius  ( m_fExpansionTemplateRadius );
    setSearchTemplateRadius     ( m_fSearchTemplateRadius );
    setExpansionTemplateRepeats ( m_iExpansionTemplateRepeats);
    setSearchTemplateRepeats    ( m_iSearchTemplateRepeats);
    setPopSize                  ( m_iPopSize);
    setSyncGen                  ( m_iSyncGen );
    setCrawlingStep             ( m_fCrawlingStep);
    setTabuRegionSize           ( m_fTabuRegionSize);
    setDistBetweenRepeats       ( m_fDistBetweenRepeats );
    setLambda                   ( m_fLamdba );

    if (getExpansionTemplateRadius()/getVoxelWidth()>5)
    {
        m_pVoxelWidthLabel->setStyleSheet("QLabel { color : red; }");
        svt_exception::ui()->info("The size of the expansion seems to be inconsistant with the voxel dimensions. This may happen if the voxel size in other unit that Angstrom. Please make sure that your input parameters are consistent!");
    }
    else
        m_pVoxelWidthLabel->setStyleSheet("QLabel { color : black; }");

};

