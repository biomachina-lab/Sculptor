/***************************************************************************
                          sculptor_dlg_gamultifit.cpp
                          -----------------
    begin                : 09.10.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_window.h>
#include <sculptor_dlg_gamultifit.h>
// qt4 includes
#include <QFileDialog>

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);

/**
 * Constructor
 */
dlg_gamultifit::dlg_gamultifit(QWidget* pParent)
    : QDialog( pParent )
{
    setupUi(this);

    connect( m_pSelectOutputDir, SIGNAL(clicked()), this, SLOT(selectGaOutputDir()) );

    // grey out some inputs when a checkbox disables the corresponding functionality
    connect( m_pCVScore, SIGNAL(toggled(bool)), m_pUseComputedCV, SLOT(setEnabled(bool)) );
    connect( m_pSample, SIGNAL(toggled(bool)), m_pPopAroundCenters, SLOT(setEnabled(bool)) );
    connect( m_pOutput, SIGNAL(toggled(bool)), m_pLocation, SLOT(setEnabled(bool)) );
    connect( m_pOutput, SIGNAL(toggled(bool)), m_pOutputPath, SLOT(setEnabled(bool)) );
    connect( m_pOutput, SIGNAL(toggled(bool)), m_pSelectOutputDir, SLOT(setEnabled(bool)) );
    connect( m_pOutput, SIGNAL(toggled(bool)), m_pPopSizeLabel_2, SLOT(setEnabled(bool)) );
    connect( m_pOutput, SIGNAL(toggled(bool)), m_pWriteModelInterval, SLOT(setEnabled(bool)) );
}

/**
 * Destructor
 */
dlg_gamultifit::~dlg_gamultifit()
{
}


/**
* set pdb used in mosaec
* \param a list of documents
*/ 
void dlg_gamultifit::setPdb( vector<pdb_document *> &oDocs )
{
    m_oPDBs = oDocs;
};

/**
 * get pdb used in mosaec
 * \return a list of documents
 */ 
vector<pdb_document *>  dlg_gamultifit::getPdb( )
{
    return m_oPDBs;
};
/**
* set situs used in mosaec
* \param a list of documents
*/ 
void dlg_gamultifit::setSitus( vector<situs_document *> &oDocs )
{
    m_oSITUSs = oDocs;
};

/**
* get situs used in mosaec
* \return a list of documents
*/ 
vector<situs_document *>  dlg_gamultifit::getSitus( )
{
    return m_oSITUSs;
};
/**
 * set the popsize
 */
void dlg_gamultifit::setPopSize( unsigned int iPopSize)
{
    m_pPopSize->setValue(iPopSize); 
}; 

/**
 * set the popsize
 */
void dlg_gamultifit::setPopSizeAroundCenters( unsigned int iPopSize)
{
    m_pPopAroundCenters->setValue( iPopSize );
};

/**
 * set the m_pShowFitnessCurve action (the one from the main window's menu) in the dialog's
 * checkbox
 * \param QAction pointer pShowFitnessCurveDlg
 */
void dlg_gamultifit::setShowFitnessCurveAction( QAction* pShowFitnessCurveDlg )
{
    // set the checkbox according to the QAction's state
    m_pShowFitnessCurve->setChecked( pShowFitnessCurveDlg->isChecked() );

    // this should be sufficient, replacing the following 2 connects. why does it not work? FIXME
    //m_pShowFitnessCurve->addAction(pShowFitnessCurveDlg);

    connect(pShowFitnessCurveDlg, SIGNAL(triggered(bool)), m_pShowFitnessCurve, SLOT(setChecked(bool)));
    connect(m_pShowFitnessCurve,  SIGNAL(toggled(bool)), pShowFitnessCurveDlg, SLOT(setChecked(bool)));
}

/**
 * get the UseComputedCV
 * \return the m_pUseComputedCV checked
 */ 
bool dlg_gamultifit::getUseComputedCV()
{
    return m_pUseComputedCV->isChecked();
};


/**
 * get the CVScore
 *\return if CVScore checked
 */ 
bool dlg_gamultifit::getCVScore()
{
    return m_pCVScore->isChecked();
};

/**
 *Select the output directory for the GA results
 */
void dlg_gamultifit::selectGaOutputDir()
{
    QFileDialog oFD(NULL);
    oFD.setViewMode( QFileDialog::List );
    //oFD.setFileMode( QFileDialog::DirectoryOnly );
    oFD.setOption(QFileDialog::ShowDirsOnly, true);

    if ( oFD.exec() == QDialog::Accepted )
    {
        QString s = oFD.directory().absolutePath();

        if ( !s.isEmpty() )
        {
            m_pOutputPath->setText( s );
            m_pOutput->setChecked( true );
        }
    }
}
 
/**
 * get the distance penalty
 */
Real64 dlg_gamultifit::getResolution()
{
    return m_pResolution->text().toDouble();
};
/**
 * get the population size
 */
int dlg_gamultifit::getPopSize()
{
    return m_pPopSize->value();
};

 /**
 * get weather one should sample around CV
 */ 
bool dlg_gamultifit::getSample()
{
    return m_pSample->isChecked();
};
/**
 * get the popsize
 */
unsigned int dlg_gamultifit::getPopSizeAroundCenters()
{
    return m_pPopAroundCenters->value();
};

/**
 * get the maximum number of generations
 */
int dlg_gamultifit::getMaxGen()
{
    return m_pMaxGen->value();
};

/**
 * get the number of threads
 */
int dlg_gamultifit::getMaxThread()
{
    return m_pMaxThread->value();
};

/**
 * get the generation when to syncronize
 */
int dlg_gamultifit::getSyncGen()
{
    return m_pSyncGen->value();
};

/**
 * get the size of the tabu window
 */
Real64 dlg_gamultifit::getTabuWindowSize()
{
    return m_pTabuWindowSize->text().toDouble();
};

 /**
 * get the threshold of the tabu regions
 */
Real64 dlg_gamultifit::getTabuThreshold()
{
    return m_pTabuThreshold->text().toDouble();
};

 /**
 * get the tabu regions size
 */
Real64 dlg_gamultifit::getTabuRegionSize()
{
    return m_pTabuRegionSize->text().toDouble();
};

 /**
 * get the distance Threshold
 */
Real64 dlg_gamultifit::getDistanceThreshold()
{
    return m_pDistanceThreshold->text().toDouble();
};

 /**
 * get the distance penalty
 */
Real64 dlg_gamultifit::getDistancePenalty()
{
    return m_pDistancePenalty->text().toDouble();
};
/**
 * get wether to output to disk
 */ 
bool dlg_gamultifit::getOutput()
{
    return m_pOutput->isChecked();
};

/**
 * get the output path
 */ 
string dlg_gamultifit::getOutputPath()
{
    return QtoStd(m_pOutputPath->text());
};   

/**
 * get the WriteModelInterval 
 */
int dlg_gamultifit::getWriteModelInterval()
{
    return m_pWriteModelInterval->value();
};

