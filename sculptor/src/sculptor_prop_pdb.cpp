/***************************************************************************
			  sculptor_prop_pdb.cpp
			  -------------------
    begin                : 10/11/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor include
#include <sculptor_prop_pdb.h>
#include <sculptor_window.h>
#include <pdb_document.h>
#include <pdb_mode.h>
//qt4 includes
#include <QListWidget>
#include <QString>
// svt includes
#include <svt_pdb.h>
// xpm includes
#include <raise.xpm>
#include <lower.xpm>
#include <dynStart.xpm>
#include <dynPrev.xpm>
#include <dynStop.xpm>
#include <dynNext.xpm>
#include <dynEnd.xpm>

extern sculptor_window* g_pWindow;

/**
 *
 */
prop_pdb::prop_pdb( QWidget *parent, pdb_document* pDoc )
    : QWidget( parent ),
      m_pDoc( pDoc )
{
    setupUi(this);


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // graphics tab
    //
    m_pComboGraphicsMode->addItem( QString("Off"),      QVariant(GM_OFF) );
    m_pComboGraphicsMode->addItem( QString("Point"),    QVariant(GM_POINT) );
    m_pComboGraphicsMode->addItem( QString("Line"),     QVariant(GM_LINE) );
    m_pComboGraphicsMode->addItem( QString("CPK"),      QVariant(GM_CPK) );
    m_pComboGraphicsMode->addItem( QString("VDW"),      QVariant(GM_VDW) );
    m_pComboGraphicsMode->addItem( QString("Dotted"),   QVariant(GM_DOTTED) );
    m_pComboGraphicsMode->addItem( QString("Licorize"), QVariant(GM_LICORIZE) );
    m_pComboGraphicsMode->addItem( QString("Tube"),     QVariant(GM_TUBE) );
    m_pComboGraphicsMode->addItem( QString("New Tube"), QVariant(GM_NEWTUBE) );
    m_pComboGraphicsMode->addItem( QString("Cartoon"),  QVariant(GM_CARTOON) );
    m_pComboGraphicsMode->addItem( QString("Surface"),  QVariant(GM_SURF) );
    //m_pComboGraphicsMode->addItem(QString("Toon"),      QVariant(GM_TOON);
    m_pComboGraphicsMode->setCurrentIndex(2);

    m_pComboAtomType->addItem( QString("All"),           QVariant(MODE_ALL) );
    m_pComboAtomType->addItem( QString("Index"),         QVariant(MODE_INDEX) );
    m_pComboAtomType->addItem( QString("Name"),          QVariant(MODE_TYPE) );
    m_pComboAtomType->addItem( QString("Residue Name"),  QVariant(MODE_RESNAME) );
    m_pComboAtomType->addItem( QString("Residue Seq."),  QVariant(MODE_RESSEQ) );
    m_pComboAtomType->addItem( QString("Chain ID"),      QVariant(MODE_CHAINID) );
    m_pComboAtomType->addItem( QString("Segment ID"),    QVariant(MODE_SEGID) );
    m_pComboAtomType->addItem( QString("Model"),         QVariant(MODE_MODEL) );
    m_pComboAtomType->addItem( QString("Nucleotides"),   QVariant(MODE_NUCLEOTIDE) );
    m_pComboAtomType->addItem( QString("Het Atoms"),     QVariant(MODE_HET) );
    m_pComboAtomType->addItem( QString("Water Atoms"),   QVariant(MODE_WATER) );

    m_pComboColorMap->addItem( QString("Name"),          QVariant(COLMAP_TYPE) );
    m_pComboColorMap->addItem( QString("Resname"),       QVariant(COLMAP_RESNAME) );
    m_pComboColorMap->addItem( QString("Chain ID"),      QVariant(COLMAP_CHAINID) );
    m_pComboColorMap->addItem( QString("Segment ID"),    QVariant(COLMAP_SEGID) );
    m_pComboColorMap->addItem( QString("Solid"),         QVariant(COLMAP_SOLID) );
    m_pComboColorMap->addItem( QString("Structure"),     QVariant(COLMAP_STRUCTURE) );
    m_pComboColorMap->addItem( QString("Model"),         QVariant(COLMAP_MODEL) );
    m_pComboColorMap->addItem( QString("Beta"),          QVariant(COLMAP_BETA) );

    m_pComboTextLabel->addItem( QString("Off"),          QVariant(LABEL_OFF) );
    m_pComboTextLabel->addItem( QString("Index"),        QVariant(LABEL_INDEX) );
    m_pComboTextLabel->addItem( QString("Name"),         QVariant(LABEL_TYPE) );
    m_pComboTextLabel->addItem( QString("Residue Name"), QVariant(LABEL_RESNAME) );
    m_pComboTextLabel->addItem( QString("Residue Seq."), QVariant(LABEL_RESID) );
    m_pComboTextLabel->addItem( QString("Model"),        QVariant(LABEL_MODEL) );
    m_pComboTextLabel->addItem( QString("Chain ID"),     QVariant(LABEL_CHAINID) );

    m_pAutoApply->setChecked( m_pDoc->getAutoApply() );

    connect( m_pButtonApply,      SIGNAL(clicked()),      m_pDoc, SLOT(sApply()) );
    connect( m_pButtonAdd,        SIGNAL(clicked()),      m_pDoc, SLOT(sAddGraphicsMode()) );
    connect( m_pButtonDelete,     SIGNAL(clicked()),      m_pDoc, SLOT(sDelGraphicsMode()) );
    connect( m_pButtonInsert,     SIGNAL(clicked()),      m_pDoc, SLOT(sInsGraphicsMode()) );
    connect( m_pComboAtomType,    SIGNAL(activated(int)), this,   SLOT(sChangeSelectionMode(int)) );
    connect( m_pComboAtomName,    SIGNAL(activated(int)), this,   SLOT(sChangeSelectionName(int)) );
    connect( m_pComboAtomTo,      SIGNAL(activated(int)), this,   SLOT(sChangeSelectionName(int)) );
    connect( m_pComboGraphicsMode,SIGNAL(activated(int)), this,   SLOT(sChangeGM(int)) );
    connect( m_pComboColorMap,    SIGNAL(activated(int)), this,   SLOT(sChangeColorMap(int)) );
    connect( m_pComboTextLabel,   SIGNAL(activated(int)), this,   SLOT(sChangeTextLabel(int)) );
    connect( m_pButtonAdjMode,    SIGNAL(clicked()),      m_pDoc, SLOT(sAdjGraphicsMode()) );
    connect( m_pButtonAdjColor,   SIGNAL(clicked()),      m_pDoc, SLOT(sAdjColor()) );
    connect( m_pAutoApply,        SIGNAL(toggled(bool)),  m_pDoc, SLOT(sAutoApply(bool)) );

    connect( m_pModeList,         SIGNAL(itemSelectionChanged()), this, SLOT(sUpdatePdbModeButtons()) );


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // dynamics tab
    //
    if ( m_pDoc->getDynamic() )
    {
        // fill in current data
        updateDynTPS();
        updateDynTimestep();
        updateDynLoop();
        updateDynButtons();

        // set icons to the buttons
        QIcon oIcon = QIcon( QPixmap(dynStart_xpm) );
        m_pDynToStart->setIcon( oIcon );

        oIcon = QIcon( QPixmap(dynPrev_xpm) );
        m_pDynBackward->setIcon( oIcon );

        oIcon = QIcon( QPixmap(dynStop_xpm) );
        m_pDynStop->setIcon( oIcon );

        oIcon = QIcon( QPixmap(dynNext_xpm) );
        m_pDynForward->setIcon( oIcon );

        oIcon = QIcon( QPixmap(dynEnd_xpm) );
        m_pDynToEnd->setIcon( oIcon );

        // and the connections
        connect(m_pDynForward,       SIGNAL(clicked()),         g_pWindow, SLOT(dynForward()) );
        connect(m_pDynBackward,      SIGNAL(clicked()),         g_pWindow, SLOT(dynBackward()) );
        connect(m_pDynStop,          SIGNAL(clicked()),         g_pWindow, SLOT(dynStop()) );
        connect(m_pDynToStart,       SIGNAL(clicked()),         g_pWindow, SLOT(dynToStart()) );
        connect(m_pDynToEnd,         SIGNAL(clicked()),         g_pWindow, SLOT(dynToEnd()) );
        connect(m_pMovieButton,      SIGNAL(clicked()),         g_pWindow, SLOT(dynMovie()) );

        connect(m_pSpinboxTPS,       SIGNAL(editingFinished()), this,      SLOT(sSpinboxTPSChanged()) );
        connect(m_pSpinboxTPS,       SIGNAL(valueChanged(int)), this,      SLOT(sSpinboxTPSChanged()) );
        connect(m_pSliderTPS,        SIGNAL(valueChanged(int)), this,      SLOT(sSliderTPSChanged(int)) );

        connect(m_pSpinboxTimestep,  SIGNAL(editingFinished()), this,      SLOT(sSpinboxTimestepChanged()) );
        connect(m_pSpinboxTimestep,  SIGNAL(valueChanged(int)), this,      SLOT(sSpinboxTimestepChanged()) );
        connect(m_pSliderTimestep,   SIGNAL(valueChanged(int)), this,      SLOT(sSliderTimestepChanged(int)) );

        m_pPlaybackMode->setId(m_pDynOnce, SVT_DYNAMICS_ONCE);
        m_pPlaybackMode->setId(m_pDynLoop, SVT_DYNAMICS_LOOP);
        m_pPlaybackMode->setId(m_pDynRock, SVT_DYNAMICS_ROCK);
        connect(m_pPlaybackMode, SIGNAL(buttonClicked(int)), g_pWindow, SLOT(dynSetLoop(int)) );
    }
    else
    {
        m_pTabs->setTabEnabled( 1, FALSE );
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // docking tab
    //
    connect( m_pAdd,             SIGNAL(clicked()),        m_pDoc, SLOT(sTransAdd()) );
    connect( m_pRemove,          SIGNAL(clicked()),        m_pDoc, SLOT(sTransDel()) );
    connect( m_pUp,              SIGNAL(clicked()),        m_pDoc, SLOT(sTransUp()) );
    m_pUp->setIcon( QIcon(QPixmap(raiseIcon)) );
    connect( m_pDown,            SIGNAL(clicked()),        m_pDoc, SLOT(sTransDown()) );
    m_pDown->setIcon( QIcon(QPixmap(lowerIcon)) );
    connect( m_pRename,          SIGNAL(clicked()),        m_pDoc, SLOT(sTransRename()) );
    connect( m_pClear,           SIGNAL(clicked()),        m_pDoc, SLOT(sTransClear()) );
    connect( m_pListTransform,   SIGNAL(itemClicked(QListWidgetItem*)), m_pDoc, SLOT(sTransApply(QListWidgetItem*)) );
}

/**
 *
 */
prop_pdb::~prop_pdb()
{
}

/**
 *
 */
void prop_pdb::transformAdd(const char * pName)
{
    m_pListTransform->addItem( QString(pName) );
}

/**
 *
 */
void prop_pdb::transformRemove(int iIndex)
{
    QListWidgetItem* pItem = m_pListTransform->takeItem( iIndex );
    delete pItem;
}

/**
 *
 */
void prop_pdb::transformUp(int iIndex)
{
    QListWidgetItem* pItem = m_pListTransform->takeItem( iIndex );
    m_pListTransform->insertItem( iIndex-1, pItem );
}

/**
 *
 */
void prop_pdb::transformDown(int iIndex)
{
    QListWidgetItem* pItem = m_pListTransform->takeItem( iIndex );
    m_pListTransform->insertItem( iIndex+1, pItem );
}

/**
 *
 */
int prop_pdb::getTransformCount()
{
    return m_pListTransform->count();
}

/**
 *
 */
int prop_pdb::getCurrentTransform()
{
    return m_pListTransform->currentRow();
}

/**
 *
 */
void prop_pdb::setTransformText(int iIndex, QString oText)
{
    m_pListTransform->item(iIndex)->setText(oText);
}

/**
 *
 */
QString prop_pdb::getTransformText(int iIndex)
{
    return m_pListTransform->item(iIndex)->text();
}

/**
 *
 */
void prop_pdb::transformsClear()
{
    m_pListTransform->clear();
}

/**
 *
 */
void prop_pdb::pdbModeAdd(QString oStr)
{
    m_pModeList->addItem( oStr );
}

/**
 *
 */
int prop_pdb::getCurrentPdbModeIndex()
{
    return m_pModeList->currentRow();
}

/**
 *
 */
void prop_pdb::setCurrentPdbModeIndex(int iIndex)
{
    m_pModeList->blockSignals( TRUE );
    m_pModeList->setCurrentRow(iIndex);
    m_pModeList->blockSignals( FALSE );
}

/**
 *
 */
void prop_pdb::setPdbMode(int iIndex, QString oStr)
{
    m_pModeList->item(iIndex)->setText(oStr);
}

/**
 *
 */
void prop_pdb::pdbModeRemove(int iIndex)
{
    QListWidgetItem* pItem = m_pModeList->takeItem( iIndex);
    delete pItem;
}

/**
 *
 */
void prop_pdb::pdbModesClear()
{
    m_pModeList->clear();
}

/**
 *
 */
int prop_pdb::getGM_AtomType()
{
    return m_pComboAtomType->itemData( m_pComboAtomType->currentIndex() ).toInt();
}

/**
 *
 */
int prop_pdb::getGM_AtomName()
{
    return m_pComboAtomName->itemData( m_pComboAtomName->currentIndex() ).toInt();
}


/**
 *
 */
int prop_pdb::getGM_AtomTo()
{
    return m_pComboAtomTo->itemData( m_pComboAtomTo->currentIndex() ).toInt();
}

/**
 *
 */
int prop_pdb::getGM_GraphicsMode()
{
    return m_pComboGraphicsMode->itemData( m_pComboGraphicsMode->currentIndex() ).toInt();
}

/**
 *
 */
int prop_pdb::getGM_ColorMap()
{
    return m_pComboColorMap->itemData( m_pComboColorMap->currentIndex() ).toInt();
}

/**
 *
 */
int prop_pdb::getGM_TextLabel()
{
    return m_pComboTextLabel->itemData( m_pComboTextLabel->currentIndex() ).toInt();
}

/**
 *
 */
void prop_pdb::updateTabInformation()
{
    svt_pdb* pPdbNode = (svt_pdb*)m_pDoc->getNode();

    QString s;
    s.sprintf( "%i", pPdbNode->size() );
    m_pNumAtoms->setText(s);

    s.sprintf( "%i", pPdbNode->getNumCAAtoms() );
    m_pNumCAAtoms->setText(s);

    s.sprintf( "%i", pPdbNode->getNumBBAtoms() );
    m_pNumBBAtoms->setText(s);

    s.sprintf( "%i", pPdbNode->getBondsNum() );
    m_pNumBonds->setText(s);

    vector<unsigned int > oModels = pPdbNode->getAtomModels();
    s.sprintf( "%i",  int(oModels.size()) );
    m_pNumModels->setText(s);

    vector<char> oChains = pPdbNode->getAtomChainIDs();
    s.sprintf( "%i", int(oChains.size()) );
    m_pNumChains->setText(s);

    s.sprintf( "%i", pPdbNode->getMaxTimestep());
    m_pNumTimesteps->setText(s);

    svt_vector4< Real64 > oCenter = m_pDoc->getCenter();

    s.sprintf( "%8.3f", oCenter.x() );
    m_pCOA_X->setText(s);

    s.sprintf( "%8.3f", oCenter.y() );
    m_pCOA_Y->setText(s);

    s.sprintf( "%8.3f", oCenter.z() );
    m_pCOA_Z->setText(s);
}

/**
 *
 */
void prop_pdb::updateTabDocking()
{

}

void prop_pdb::showDockingTab()
{
    m_pTabs->setCurrentIndex(2);
}

/**
 * the user changed the color map combo box
 * this is a QT slot.
 */
void prop_pdb::sChangeColorMap(int)
{
    updatePdbMode();
    sUpdatePdbModeButtons();
}
/**
 * the user changed the text label combo box
 * this is a QT slot.
 */
void prop_pdb::sChangeTextLabel(int)
{
    updatePdbMode();
};

/**
 * the user changed the graphics mode combo box
 * this is a QT slot.
 */
void prop_pdb::sChangeGM(int)
{
    updatePdbMode();
    sUpdatePdbModeButtons();
}

/**
 * the user has clicked on the selection name combo
 * this is a QT slot.
 */
void prop_pdb::sChangeSelectionName(int)
{
    updatePdbMode();
    sUpdatePdbModeButtons();
}

/**
 * the user has clicked on the selection mode combo
 * this is a QT slot.
 */
void prop_pdb::sChangeSelectionMode(int m)
{
    // get the current mode
    pdb_mode* pMode = m_pDoc->getPdbMode( getCurrentPdbModeIndex() );
    // set the selection name to 0.
    if (pMode)
    {
        pMode->setSelectionMode(m);
        pMode->setSelectionName(0);
        pMode->setSelectionTo(0);

        sUpdatePdbModeButtons();
        updatePdbMode();
    }
}

/**
 * according to the current graphics mode settings some buttons must be disabled or enabled. This is done here.
 */
void prop_pdb::sUpdatePdbModeButtons()
{
    

    // get the current mode
    pdb_mode* pMode = m_pDoc->getPdbMode( getCurrentPdbModeIndex() );
    svt_pdb* pPdbNode = (svt_pdb*) m_pDoc->getNode();

    // now change all buttons according to this mode
    if (pMode)
    {
        // set the selection mode, name, graphics mode and colormap mode
        m_pComboAtomType->setCurrentIndex(pMode->getSelectionMode());
        m_pComboGraphicsMode->setCurrentIndex(pMode->getGraphicsMode());
        m_pComboColorMap->setCurrentIndex(pMode->getColmapType());
        m_pComboTextLabel->setCurrentIndex(pMode->getLabelMode());

        // now disable or enable the sphere scaling line edit
        switch(pMode->getGraphicsMode())
        {
            case GM_VDW:
            case GM_CPK:
            case GM_DOTTED:
            case GM_TUBE:
            case GM_NEWTUBE:
            case GM_CARTOON:
            case GM_SURF:
                //case GM_TOON:
                m_pButtonAdjMode->setEnabled( TRUE );
                break;
            default:
                m_pButtonAdjMode->setDisabled( TRUE );
                break;
        }

        // disable or enable the colomapping adjust button
        switch(pMode->getColmapType())
        {
            case COLMAP_SEGID:
                m_pButtonAdjColor->setDisabled( TRUE );
                break;
            default:
                m_pButtonAdjColor->setEnabled( TRUE );
                break;
        }

        // change the other buttons according to the current selection type
        vector<const char*> array;
        vector<char> arrayC;
        vector<int> arrayI;
        vector<unsigned int> arrayUI;
        QString s;
        unsigned int i;

        switch(pMode->getSelectionMode())
        {
            case MODE_ALL:
                m_pComboAtomName->setDisabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_TYPE:
                array = pPdbNode->getAtomNames();
                m_pComboAtomName->clear();
                for(i=0; i<array.size(); i++)
                    m_pComboAtomName->addItem( QString(array[i]), QVariant(i) );
                m_pComboAtomName->setEnabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_RESNAME:
                array = pPdbNode->getAtomResnames();
                m_pComboAtomName->clear();
                for(i=0;i<array.size();i++)
                    m_pComboAtomName->addItem( QString(array[i]), QVariant(i) );
                m_pComboAtomName->setEnabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_INDEX:
                m_pComboAtomName->clear();
                m_pComboAtomTo->clear();
                for(i=0;i<pPdbNode->size();i++)
                {
                    s.sprintf("%i", i+1);
                    m_pComboAtomName->addItem( s, QVariant(i) );
                    m_pComboAtomTo->addItem( s, QVariant(i) );
                }
                m_pComboAtomName->setEnabled( TRUE );
                m_pComboAtomTo->setEnabled( TRUE );
                m_pLabelFrom->setText("From:");
                m_pLabelTo->setText("To:");
                break;

            case MODE_CHAINID:
                arrayC = pPdbNode->getAtomChainIDs();
                m_pComboAtomName->clear();
                for(i=0;i<arrayC.size();i++)
                {
                    s.sprintf("%c",arrayC[i]);
                    m_pComboAtomName->addItem( s, QVariant(i) );
                }
                m_pComboAtomName->setEnabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_SEGID:
                array = pPdbNode->getAtomSegmentIDs();
                m_pComboAtomName->clear();
                for(i=0;i<array.size();i++)
                    m_pComboAtomName->addItem( QString(array[i]), QVariant(i) );
                m_pComboAtomName->setEnabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_RESSEQ:
                arrayI = pPdbNode->getAtomResidueSeqs();
                m_pComboAtomName->clear();
                m_pComboAtomTo->clear();
                for(i=0;i<arrayI.size();i++)
                {
                    s.sprintf("%i", arrayI[i]);
                    m_pComboAtomName->addItem( s, QVariant(i) );
                    m_pComboAtomTo->addItem( s, QVariant(i) );
                }
                m_pComboAtomName->setEnabled( TRUE );
                m_pComboAtomTo->setEnabled( TRUE );
                m_pLabelFrom->setText("From:");
                m_pLabelTo->setText("To:");
                break;

            case MODE_MODEL:
                arrayUI = pPdbNode->getAtomModels();
                m_pComboAtomName->clear();
                for(i=0;i<arrayUI.size();i++)
                {
                    s.sprintf("%i", arrayUI[i]);
                    m_pComboAtomName->addItem( s, QVariant(i) );
                }
                m_pComboAtomName->setEnabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_NUCLEOTIDE:
                m_pComboAtomName->setDisabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_HET:
                m_pComboAtomName->setDisabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;

            case MODE_WATER:
                m_pComboAtomName->setDisabled( TRUE );
                m_pComboAtomTo->setDisabled( TRUE );
                m_pLabelFrom->setText("");
                m_pLabelTo->setText("");
                break;
        }

        m_pComboAtomName->setCurrentIndex(pMode->getSelectionName());
        m_pComboAtomTo->setCurrentIndex(pMode->getSelectionTo());
    }
}

void prop_pdb::updatePdbMode()
{
    int iIndex = getCurrentPdbModeIndex();
    pdb_mode* pMode = m_pDoc->getPdbMode( iIndex );

    if (pMode)
    {
        pMode->setSelectionMode( getGM_AtomType() );
        pMode->setSelectionName( getGM_AtomName() );
        pMode->setSelectionTo(   getGM_AtomTo() );
        pMode->setGraphicsMode(  getGM_GraphicsMode() );
        pMode->setColmapType(    getGM_ColorMap() );
        pMode->setLabelMode(     getGM_TextLabel() );

        setPdbMode( iIndex, m_pDoc->calcModeString(iIndex) );

        m_pDoc->updatePdbMode();
    }
}

/**
 *
 */
int prop_pdb::getCurrentColorMap()
{
    return m_pComboColorMap->itemData( m_pComboColorMap->currentIndex() ).toInt();
}

///////////////////////////////////////////////////////////////////////////////
// molecular dynamics
///////////////////////////////////////////////////////////////////////////////

/**
 * update playback-speed with the current value
 * \param iTPS curent timesteps per second
 */
void prop_pdb::updateDynTPS(int iTPS)
{
    if (iTPS == -1 )
        iTPS = (int)(g_pWindow->dynGetSpeed());

    // do not set back to zero after stop playing dynamics, would look annoying
    // if (iTPS == 0)
    //     return;

    m_pSpinboxTPS->blockSignals( TRUE );
    m_pSliderTPS ->blockSignals( TRUE );

    m_pSpinboxTPS->setValue( iTPS );
    m_pSliderTPS ->setValue( iTPS );

    m_pSpinboxTPS->blockSignals( FALSE );
    m_pSliderTPS ->blockSignals( FALSE );
}

/**
 * update timestep with the current value
 * \param iTimestep curent timestep
 */
void prop_pdb::updateDynTimestep(int iTimestep, int iMaxTimestep)
{
    if (iTimestep == -1 )
    {
        iTimestep  = g_pWindow->dynGetTimestep();
        iMaxTimestep = g_pWindow->dynGetMaxTimestep();
    }

    m_pSpinboxTimestep->blockSignals( TRUE );
    m_pSliderTimestep ->blockSignals( TRUE );

    m_pSpinboxTimestep->setValue( iTimestep );
    m_pSliderTimestep ->setValue( iTimestep );
    m_pSpinboxTimestep->setMaximum( iMaxTimestep );
    m_pSliderTimestep ->setMaximum( iMaxTimestep );

    m_pSpinboxTimestep->blockSignals( FALSE );
    m_pSliderTimestep ->blockSignals( FALSE );
}


/**
 * update loop mode with the current value
 * \param iLoop curent loop mode
 */
void prop_pdb::updateDynLoop(int iLoop)
{
    if (iLoop == -1)
        iLoop = g_pWindow->dynGetLoop();

    switch (iLoop)
    {
        case SVT_DYNAMICS_ONCE:
            m_pDynOnce->blockSignals( TRUE );
            m_pDynOnce->setChecked( TRUE );
            m_pDynOnce->blockSignals( FALSE );
            break;

        case SVT_DYNAMICS_LOOP:
            m_pDynLoop->blockSignals( TRUE );
            m_pDynLoop->setChecked( TRUE );
            m_pDynLoop->blockSignals( FALSE );
            break;

        case SVT_DYNAMICS_ROCK:
            m_pDynRock->blockSignals( TRUE );
            m_pDynRock->setChecked( TRUE );
            m_pDynRock->blockSignals( FALSE );
            break;
    }
}

/**
 *
 */
void prop_pdb::updateDynButtons(int iPlaying)
{
    if (iPlaying == -1)
        iPlaying = SVT_DYNAMICS_STOPPED;

    m_pPlaybackMode->blockSignals(TRUE);
    m_pDynBackward->blockSignals(TRUE);
    m_pDynStop    ->blockSignals(TRUE);
    m_pDynForward ->blockSignals(TRUE);

    switch (iPlaying)
    {
        case SVT_DYNAMICS_FORWARD:
            m_pDynForward ->setChecked(TRUE);
            m_pDynStop    ->setChecked(FALSE);
            m_pDynBackward->setChecked(FALSE);
            break;

        case SVT_DYNAMICS_STOPPED:
            m_pDynForward ->setChecked(FALSE);
            m_pDynStop    ->setChecked(TRUE);
            m_pDynBackward->setChecked(FALSE);
            break;

        case SVT_DYNAMICS_BACKWARD:
            m_pDynForward ->setChecked(FALSE);
            m_pDynStop    ->setChecked(FALSE);
            m_pDynBackward->setChecked(TRUE);
            break;
    }

    m_pDynBackward->blockSignals(FALSE);
    m_pDynStop    ->blockSignals(FALSE);
    m_pDynForward ->blockSignals(FALSE);
    m_pPlaybackMode->blockSignals(FALSE);
}

/**
 *
 */
void prop_pdb::updateDynMovieButton(bool bActive)
{
    m_pMovieButton->blockSignals(TRUE);
    m_pMovieButton->setChecked(bActive);
    m_pMovieButton->blockSignals(FALSE);
}

/**
 * set timesteps per second from the slider
 */
void prop_pdb::sSliderTPSChanged(int iTPS)
{
    // update the spinbox
    m_pSpinboxTPS->blockSignals(TRUE);
    m_pSpinboxTPS->setValue(iTPS);
    m_pSpinboxTPS->blockSignals(FALSE);
    // and set the value
    g_pWindow->dynSetTPS( iTPS );
}

/**
 * set timesteps per second from the spinbox
 */
void prop_pdb::sSpinboxTPSChanged()
{
    // update the slider
    m_pSliderTPS->blockSignals(TRUE);
    m_pSliderTPS->setValue( m_pSpinboxTPS->value() );
    m_pSliderTPS->blockSignals(FALSE);
    // and set the value
    g_pWindow->dynSetTPS( m_pSpinboxTPS->value() );
}


/**
 * set timestep from the slider
 */
void prop_pdb::sSliderTimestepChanged(int iTimestep)
{
    // update the spinbox
    m_pSpinboxTimestep->blockSignals(TRUE);
    m_pSpinboxTimestep->setValue(iTimestep);
    m_pSpinboxTimestep->blockSignals(FALSE);
    // and set the value
    g_pWindow->dynSetTimestep( iTimestep );
}

/**
 * set timestep from the spinbox
 */
void prop_pdb::sSpinboxTimestepChanged()
{
    // update the slider
    m_pSliderTimestep->blockSignals(TRUE);
    m_pSliderTimestep->setValue( m_pSpinboxTimestep->value() );
    m_pSliderTimestep->blockSignals(FALSE);
    // and set the value
    g_pWindow->dynSetTimestep( m_pSpinboxTimestep->value() );
}
