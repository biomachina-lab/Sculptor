/***************************************************************************
                          sculptor_window.cpp
			  -------------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_app.h>
#include <sculptor_window.h>
#include <sculptor_window_documentlist.h>
#include <sculptor_document.h>
#include <sculptor_version.h>
#include <sculptor_splitter.h>
#include <sculptor_powell_cc.h>
#include <sculptor_log.h>
#include <sculptor_lua.h>
#include <situs_document.h>
#include <pdb_document.h>
#include <clip_document.h>
#include <sculptor_doc_eraser.h>
#include <sculptor_doc_marker.h>
#include <scene_document.h>
#include <sculptor_scene.h>
#include <sculptor_dlg_about.h>
#include <sculptor_dlg_featuredocking.h>
#include <sculptor_dlg_fetchpdb.h>
#include <sculptor_dlg_flexing.h>
#include <sculptor_dlg_force.h>
#include <sculptor_dlg_gacylinder.h>
#include <sculptor_dlg_gamultifit.h>
#include <sculptor_dlg_lighting.h>
#include <sculptor_dlg_luaeditor.h>
#include <sculptor_dlg_lualist.h>
#include <sculptor_dlg_plot.h>
#include <sculptor_dlg_preferences.h>
#include <sculptor_dlg_refine.h>
#include <sculptor_dlg_rmsd.h>
#include <sculptor_dlg_transform.h>
#include <sculptor_dlg_wait.h>
#include <sculptor_dlg_DPSV_filter.h>
#include <sculptor_qwtplotcurve.h>
#include <sculptor_force_algo.h>
#include <sculptor_codebook_displacement.h>
// svt includes
#include <svt_volume.h>
#include <svt_init.h>
#include <svt_system.h>
#include <svt_force_tool.h>
#include <svt_lua.h>
#include <svt_scenegraph.h>
// qt4 includes
#include <QMenuBar>
#include <QDesktopWidget>
#include <QActionGroup>
#include <QSignalMapper>
#include <QStyleFactory>
#include <QToolBar>
#include <QImage>
#include <QPixmap>
#include <QScrollArea>
#include <QToolButton>
#include <QMessageBox>
#include <QStatusBar>
#include <QEvent>
#include <QCloseEvent>
#include <QList>
#include <QStackedLayout>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
// icon includes
#include <moveSmall.xpm>
#include <coordSys.xpm>
#include <eyeSmall.xpm>
#include <save.xpm>
#include <fileopen.xpm>
#include <zoomIn.xpm>
#include <zoomOut.xpm>
#include <clip-medium.xpm>
#include <eraser-medium.xpm>
#include <marker.xpm>
#include <camera.xpm>
#include <transTool.xpm>
#include <dockTool.xpm>
#include <properties.xpm>
#include <delete.xpm>
#include <top.xpm>
#include <compute_cc.xpm>
#include <mode_points.xpm>
#include <mode_lines.xpm>
#include <mode_vdw.xpm>
#include <mode_cpk.xpm>
#include <mode_licorice.xpm>
#include <mode_tube.xpm>
#include <mode_cartoon.xpm>
//#include <mode_toon.xpm>
#include <colmap_solid.xpm>
#include <colmap_rainbow.xpm>
#include <compute_rmsd.xpm>
#include <ortho.xpm>
#include <force.xpm>
#include <qpdb.xpm>
#include <qfit.xpm>
#include <volCropTool.xpm>
#include <mapExplorerTool.xpm>
#include <vol-orange-medium.xpm>
#include <mol-orange-medium.xpm>
#include <mol-green-medium.xpm>
#include <move-clashing-medium-white.xpm>
#include <global_lighting.xpm>
#include <fetchTool.xpm>
#include <fullscreen.xpm>

#ifdef SVT_SYSTEM_QT
#include <svt_window_qt.h>
#endif

//timer interval
#define TIMER_INTER 10

#ifdef PHANTOM_PATCH
#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>
#endif

extern void setSVT(bool);
extern void svt_redrawWindow(int i);
extern void svt_idle();
extern sculptor_window* g_pWindow;
extern sculptor_app* g_qApp;
extern string QtoStd(QString);
extern QString StdtoQ(string);

/**
 * Constructor
 * \param pScene pointer to sculptor_scene
 */
sculptor_window::sculptor_window(sculptor_scene* pScene)
    : QMainWindow( NULL, Qt::Window ),
      m_pScene( pScene ),
      m_pSceneDoc( NULL ),
      m_pForceDlg( NULL ),
      m_pForceAlgo( NULL ),
      m_pTransformDlg( NULL ),
      m_pLightingDlg( NULL ),
      m_pFlexFittingDlg( NULL ),
      m_pFeatureDockingDlg( NULL ),
      m_pFetchpdbDlg( NULL ),
      m_pRMSDDlg( NULL ),
      m_pAboutDlg( NULL ),
      m_pWaitDlg( NULL ),
      m_pPrefDlg( NULL ),
      m_pCVDisplacementObj( NULL ),
      m_iRefineMode( SCULPTOR_CORRELATION ),
      m_pRefineDlg( NULL ),
      m_iTargetDoc( -1 ),
      m_pTargetDoc( NULL ),
      m_iProbeDoc( -1 ),
      m_pProbeDoc( NULL ),
      m_bAutoAdjustProbeSolutionColor( false ),
      m_pWorkspace( NULL ),
      m_pMainSplitter( NULL ),
      m_pLeftSplitter( NULL ),
      m_pRightSplitter( NULL ),
      m_pRecentMenu( NULL ),
      m_pVolumeMenu( NULL ),
      m_pVolumeContextMenu( NULL ),
      m_pStructureMenu( NULL ),
      m_pStructureContextMenu( NULL ),
      m_fDynPreviousSpeed( 15.0f ),
      m_iDynLastTimeout( 0 ),
      m_bMovie( false ),
      m_pStericClashingData( NULL ),
      m_bStericClashingDataValid( false ),
      m_pGA( NULL ),
      m_pGADlg( NULL ),
      m_pGACyl( NULL ),
      m_pGACylDlg( NULL ),
      m_pGAFitnessCurveDlg( NULL ),
      m_pLuaEditor( NULL ),
      m_pLuaList( NULL ),
      m_oCurrentPosition( svt_vector4<Real64> (0.0f,0.0f,0.0f) ),
      m_bTextAreaCollapsed( false ),
      m_bDocumentListCollapsed( false ),
      m_bWindowWasMaximized( false ),
      m_bRestoreWindowStateOnStart( true )
{
    setAttribute(Qt::WA_DeleteOnClose);
    //resize(800, 600);
    //resize(1024, 768);
    resize(1200, 900);

    // set a nice window icon
    setWindowIcon( QIcon(QPixmap("sculptor.png")) );

    // set up file filters
    m_oFilterALL =     "All supported formats (*.scl *.sar *.pdb *.pdb1 *.situs *.sit *.mrc *.ccp4 *.map *.spi *.SPI *.spider *.SPIDER *.eli)";
    m_oFilterPDB =     "Molecule (*.pdb *.pdb*)";
    m_oFilterPSF_PDB = "Molecule (*.pdb *.psf)";
    m_oFilterPSF_DCD = "Trajectory (*.psf *.dcd)";
    m_oFilterCBK =     "Codebook Vectors (*.pdb)";
    m_oFilterSCL =     "Sculptor state (*.scl)";
    m_oFilterSAR =     "Sculptor archive (*.sar)";
    m_oFilterLUA =     "Sculptor scripts (*.lua)";
    m_oFilterSITUS =   "Volume (*.situs *.sit *.mrc *.ccp4 *.map *.spi *.SPI *.spider *.SPIDER)";
    m_oFilterELI =     "Eliquos exhaustive search (*.eli)";


    // setup the splitters
    // Main splitter - splits between left area (mostly the 3D window) and the right area (mostly the control widgets).
    m_pMainSplitter = new sculptor_splitter( Qt::Horizontal, this );

    // The left splitter (document list and property dialogs)
    m_pLeftSplitter = new sculptor_splitter( Qt::Vertical, m_pMainSplitter );
    
    // The right splitter (3D window and the log window)
    m_pRightSplitter = new sculptor_splitter( Qt::Vertical, m_pMainSplitter );

    // The tab area
    m_pTabArea = new QTabWidget( (QWidget*)m_pRightSplitter );


    // create the document list and the property dialog scrollarea
    m_pDocumentList = new window_documentlist( this, m_pLeftSplitter );
    QWidget* pPropArea = new QWidget(m_pLeftSplitter);
    m_pDocumentList->setPropDialogArea( pPropArea );

    // set main splitter geometry
    QList<int> oSizes;
    oSizes.push_back(350);
    oSizes.push_back(500);
    m_pMainSplitter->setSizes(oSizes);
    m_pMainSplitter->setStretchFactor(1,1);


    // log widget
    m_pLog = new sculptor_log( std::cout, m_pTabArea );
    m_pTabArea->addTab( m_pLog->getLogWidget(), "Log" );
#ifdef WIN32
    svt_redirectCout( m_pLog );
#endif

    // QMainWindow's central widget
    setCentralWidget( m_pMainSplitter );

    // menus
    createMenus();

    // load the information about the recently loaded documents
    loadRecentList();

    // now add the lua gui
    scriptInitGUI();


    // set up timers
    m_pDynTimer = new QTimer( this );
    connect( m_pDynTimer, SIGNAL(timeout()), SLOT(dynTimeout()) );
    
    m_pUpdateGATimer = new QTimer( this );
    connect( m_pUpdateGATimer, SIGNAL(timeout()), SLOT(updateGaResults()) );

    m_pUpdateGACylTimer = new QTimer( this );
    connect( m_pUpdateGACylTimer, SIGNAL(timeout()), SLOT(updateGaCylResults()) );

    // done
    statusBar()->showMessage( QString("Ready"), 2000 );
}

/**
 * Destructor
 */
sculptor_window::~sculptor_window()
{    
    if (m_pPrefDlg != NULL)
	delete (m_pPrefDlg);
    if (m_pLightingDlg != NULL)
	delete (m_pLightingDlg);
    if (m_pFetchpdbDlg != NULL)
        delete (m_pFetchpdbDlg);
    if (m_pFlexFittingDlg != NULL)
	delete (m_pFlexFittingDlg);
    if (m_pRMSDDlg != NULL)
        delete (m_pRMSDDlg);
    if (m_pAboutDlg != NULL)
        delete (m_pAboutDlg);
    if (m_pWaitDlg != NULL)
	delete (m_pWaitDlg);
    if (m_pFeatureDockingDlg != NULL)
        delete (m_pFeatureDockingDlg);
    if (m_pRefineDlg != NULL)
        delete (m_pRefineDlg);
    if (m_pGADlg != NULL)
        delete (m_pGADlg);
    if (m_pGA!=NULL)
        delete (m_pGA);
    if (m_pGACylDlg != NULL)
        delete (m_pGACylDlg);
    if (m_pGACyl!=NULL)
        delete (m_pGACyl);
    if (m_pForces != NULL)
        delete (m_pForces);
    if (m_pStructureContextMenu != NULL)
        delete (m_pStructureContextMenu);
    if (m_pVolumeContextMenu != NULL)
        delete (m_pVolumeContextMenu);
    //delete timers;
    if (m_pDynTimer!=NULL)
    {
        m_pDynTimer->stop();
        delete m_pDynTimer;
    }
    if (m_pUpdateGATimer != NULL)
    {
        m_pUpdateGATimer->stop();
        delete (m_pUpdateGATimer);
    }
    if (m_pUpdateGACylTimer != NULL)
    {
        m_pUpdateGACylTimer->stop();
        delete (m_pUpdateGACylTimer);
    }
}

/**
 * 
 */
void sculptor_window::createMenus()
{
    //
    // file menu and toolbar
    //

    QMenu* pMenu = menuBar()->addMenu( tr("&File") );
    QToolBar* pFileToolbar = addToolBar( tr("&File Tools") );
    pFileToolbar->setObjectName( QString("File_Tools") );
    //
    QAction* pAction = pMenu->addAction( QIcon( fileopen ), tr("&Open"), this, SLOT(load()), QKeySequence(Qt::CTRL+Qt::Key_O) );
    pAction->setToolTip( tr("Open an Existing File From Disk") );
    pFileToolbar->addAction( pAction );
    //
    m_pRecentMenu = pMenu->addMenu( tr("Open &Recent") );
    connect( m_pRecentMenu, SIGNAL(triggered(QAction*)), this, SLOT(sDocLoadRecent(QAction*)) );
    pAction = pMenu->addAction( QIcon( fetchTool ), tr("F&etch PDB"), this, SLOT(showFetchpdbDlg()), QKeySequence(Qt::CTRL+Qt::Key_E) );
    pAction->setToolTip( tr("Fetch a PDB From www.pdb.org") );
    pFileToolbar->addAction( pAction );
    //
    QAction* pSaveAction = pMenu->addAction( QIcon( saveIcon ), tr("&Save As"), this, SLOT(save()), QKeySequence(Qt::CTRL+Qt::Key_S) );
    pSaveAction->setToolTip( tr("Save Current Document to Disk") );
    pFileToolbar->addAction( pSaveAction );
    //
    QAction* pCloseAction = pMenu->addAction( QIcon( deleteIcon ), tr("&Close"), this, SLOT(sDocsRemove()), QKeySequence(Qt::CTRL+Qt::Key_D) );
    pCloseAction->setToolTip( tr("Close the Current Document") );
    //
    pMenu->addSeparator();
    //
    pAction = pMenu->addAction( tr("S&ave State"), this, SLOT( saveSCL() ) );
    pAction->setToolTip( tr("Save Information About the Current State of Sculptor\nUseful to Seamlessly Resume Work on This Computer") );
    pAction = pMenu->addAction( tr("Archive State"), this, SLOT( saveArchive() ) );
    pAction->setToolTip( tr("Save the Current State of Sculptor (Including Opened Documents) Into a Single File on Disk\nFile can be Loaded Sculptor on any Computer") );
    //
    pMenu->addSeparator();
    //
    pMenu->addAction( tr("Preferences"), this, SLOT(sShowPreferencesDialog()) );
    //
    pMenu->addSeparator();
    //
    pAction = pMenu->addAction( tr("&Quit"), this, SLOT( quitNow() ), QKeySequence(Qt::CTRL+Qt::Key_Q) );
    pMenu->insertSeparator(pAction);
    pAction->setToolTip( tr("Exit Sculptor") );

    //
    // edit menu and toolbar
    //

    pMenu = menuBar()->addMenu( tr("&Edit") );
    QToolBar* pEditToolbar = addToolBar( tr("&Edit Tools") );
    pEditToolbar->setObjectName(QString("Edit_Tools"));
    pAction = pMenu->addAction( QIcon(clip_medium), tr("Add &Clipping Plane"), this, SLOT(addClip()) );
    pEditToolbar->addAction( pAction );
    pAction = pMenu->addAction( QIcon(QPixmap(eraser_medium)), tr("&Add Eraser Tool"), this, SLOT(addEraser()) );
    pEditToolbar->addAction( pAction );
    pAction = pMenu->addAction( QIcon(QPixmap(marker)), tr("Add &Marker"), this, SLOT(addMarker()) );
    pEditToolbar->addAction( pAction );
    //
    pMenu -> addSeparator();
    //
    QAction* pTransformAction = pMenu->addAction( QIcon(transTool), "Transformation", this, SLOT(showTransformDlg()) );
    pEditToolbar->addAction( pTransformAction );
    pMenu->addAction( tr("Reset Transformation"), this, SLOT(restoreInitMat()) );
    //
    pMenu -> addSeparator();
    //
    m_pProperties = pMenu->addAction( QIcon( propIcon ), tr("&Property Dialog"), this, SLOT( togglePropDlgArea() ) );
    m_pProperties->setCheckable( TRUE );
    m_pProperties->setChecked( FALSE );
    m_pView = pMenu->addAction( QIcon( eyeSmallIcon ), tr("&Visible"), this, SLOT( toggleViewDoc() ) );
    m_pView->setCheckable( true );
    m_pView->setChecked( true );
    m_pMove = pMenu->addAction( QIcon( moveSmallIcon ), tr("&Move"), this, SLOT( toggleMovedDoc() ) );
    m_pMove->setCheckable( true );
    m_pMove->setChecked( true );
    pMenu->addAction( QIcon( top ), tr("&Top"), this, SLOT( sDocMakeTop() ) );

    //
    // view menu and toolbar
    //

    pMenu = menuBar()->addMenu( tr("&View") );
    QMenu* pToolbarMenu = new QMenu( tr("&Toolbars") );
    pMenu->addMenu(pToolbarMenu);
    QAction* pFullScreenAction = pMenu->addAction( QIcon(QPixmap(view_fullscreen)), tr("&Full Screen"), this, SLOT(sToggleFullScreen()), QKeySequence(Qt::Key_F11) );
    pFullScreenAction->setCheckable(TRUE);
    pFullScreenAction->setChecked(FALSE);
    pMenu->addSeparator();
    QToolBar* pViewToolbar = addToolBar( tr("&View Tools") );
    pViewToolbar->setObjectName( QString("View_Tools") );
    pViewToolbar->addAction( pFullScreenAction );
    //
    //
    QAction* pScreenshot = pMenu->addAction( QIcon(camera), tr("&Save Screenshot"), this, SLOT(saveScreenshot()), QKeySequence(Qt::Key_F9) );
    pFileToolbar->addAction( pScreenshot );
    //
    //pMenu->addAction( tr("&Render HQ image"), this, SLOT(render()) );
    pMenu->addAction( tr("&Export as Wavefront obj file"), this, SLOT(exportOBJ()) );
    //
    QAction * pLighting = pMenu->addAction( QIcon(global_lighting), tr("&Background Color"), this, SLOT(setGlobalLighting()));
    pLighting->setToolTip( tr("Change Background Color and\nGlobal Lighting Options") );
    pViewToolbar->addAction( pLighting );
    //
    pAction = pMenu->addAction( QIcon(zoomInTool),  tr("Zoom In"), this, SLOT(zoomIn()) );
    pMenu->insertSeparator(pAction);
    pViewToolbar->addAction( pAction );
    pAction = pMenu->addAction( QIcon(zoomOutTool), tr("Zoom Out"), this, SLOT(zoomOut()) );
    pViewToolbar->addAction( pAction );
    //
    m_pCenterMode = pMenu->addAction( tr("Center Mode"), this, SLOT(setCenterMode()) );
    pMenu->insertSeparator(m_pCenterMode);
    m_pCenterMode->setCheckable( TRUE );
    m_pScene != NULL ? m_pCenterMode->setChecked( m_pScene->getCenterMode() ) : m_pCenterMode->setChecked( TRUE );
    //
    m_pOrthoMode = pMenu->addAction( QIcon(QPixmap(ortho)), tr("Orthographic Projection"), this, SLOT(toggleOrtho()) );
    m_pOrthoMode->setCheckable( TRUE );
    m_pOrthoMode->setChecked( svt_getOrtho() );
    //
    m_pCoordinateSystem = pMenu->addAction( QIcon(QPixmap(coordSys)), tr("Show 3D Coordinate System"), this, SLOT(toggle3DCoordSystem()));
    m_pCoordinateSystem->setCheckable( TRUE );
    m_pScene != NULL ? m_pCoordinateSystem->setChecked( m_pScene->getCoordinateSystemVisible() ) : m_pCoordinateSystem->setChecked( TRUE );
    //
    m_pPipette = pMenu->addAction( tr("Show 3D Cursor"), this, SLOT(togglePipette()) );
    m_pPipette->setCheckable( TRUE );
    m_pScene != NULL ? m_pPipette->setChecked( m_pScene->getPipetteVisible() ) : m_pPipette->setChecked( FALSE );
    //
    m_pHighContrast = pMenu->addAction( tr("High Contrast Palette"), this, SLOT(toggleHighContrast()) );
    m_pHighContrast->setCheckable( TRUE );
    m_bHighContrast = true;
    m_pHighContrast->setChecked( m_bHighContrast );
    m_pCappedClip = pMenu->addAction( tr("Closed Surfaces at Clip-Planes"), this, SLOT(cappedClip()) );
    m_pCappedClip->setCheckable( true );
    m_pScene != NULL ? m_pCappedClip->setChecked( m_pScene->getCappedClip() ) : m_pCappedClip->setChecked( TRUE );
    //
    if ( svt_getConfig()->getValue("STEREO", false) == true)
        pMenu->addAction("Change Eye Separation", this, SLOT(setEyeSeparation()) );
    //
    pViewToolbar->addAction( m_pOrthoMode );


    //
    //pMenu->insertSeparator();
    //pMenu->addAction( tr("Generate 3DNP screenshots"), this, SLOT(gen3DNP()) );

    // list of all available styles
/*
    QT3FIXME
    pMenu->insertSeparator();
    QMenu* pStyle = pMenu->addMenu( tr("St&yle") );
    pStyle->setCheckable( true );
    QActionGroup *pAG = new QActionGroup( this, 0 );
    pAG->setExclusive( true );
    QSignalMapper *styleMapper = new QSignalMapper( this );
    connect( styleMapper, SIGNAL( mapped( const QString& ) ), this, SLOT( makeStyle( const QString& ) ) );
    QStringList list = QStyleFactory::keys();
    list.sort();
#ifndef QT_NO_STYLE_WINDOWS
    list.insert(list.begin(), tr("Sculptor"));
#endif
    Q3Dict<int> stylesDict( 17, FALSE );
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        QString styleStr = *it;
        QString styleAccel = styleStr;
        if ( stylesDict[styleAccel.left(1)] ) {
            for ( uint i = 0; i < styleAccel.length(); i++ ) {
                if ( !stylesDict[styleAccel.mid( i, 1 )] ) {
                    stylesDict.insert(styleAccel.mid( i, 1 ), (const int *)1);
                    styleAccel = styleAccel.insert( i, '&' );
                    break;
                }
            }
        } else {
            stylesDict.insert(styleAccel.left(1), (const int *)1);
            styleAccel = "&"+styleAccel;
        }
        // QAction *a = new QAction( styleStr, QIcon(), styleAccel, 0, pAG, 0, pAG->isExclusive() );
        // connect( a, SIGNAL( activated() ), styleMapper, SLOT(map()) );
        // styleMapper->setMapping( a, a->text() );
    }
    pAG->addTo(pStyle);
*/

    //
    // dock menu and toolbar
    //

    pMenu = menuBar()->addMenu( tr("&Docking") );
    QToolBar* pDockToolbar = addToolBar( tr("&Docking Tools") );
    pDockToolbar->setObjectName( QString("Dock_Tools") );
    //
    pAction = pMenu->addAction( QIcon(vol_orange_medium), tr("Set &Target Map"), this, SLOT(setTargetDoc()) );
    pDockToolbar->addAction( pAction );
    pAction = pMenu->addAction( QIcon(mol_orange_medium), tr("Set &Probe Molecule"), this, SLOT(setDockDoc()) );
    pDockToolbar->addAction( pAction );
    pAction = pMenu->addAction( QIcon(mol_green_medium), tr("Set &Solution Candidate"), this, SLOT(setSolutionDoc()) );
    pDockToolbar->addAction( pAction );
    //
    //m_pVolAction_Corr = pMenu->addAction( QIcon(compute_cc), tr("&Cross Correlation"), this, SLOT(calcCorrelation()));
    //
    m_pStericClashing = pMenu->addAction( QIcon(move_clashing_medium_white), tr("Steric Clashing"), this, SLOT(toggleStericClashing()) );
    m_pStericClashing->setToolTip( tr("Turn on/off Steric Clashing Visualization") );
    m_pStericClashing->setCheckable( TRUE );
    m_pStericClashing->setChecked( FALSE );
    pDockToolbar->addAction( m_pStericClashing );
    //
    pDockToolbar->addSeparator();
    //
    pAction = m_pAutoColor = pMenu->addAction( tr("Auto Adjust Colors"), this, SLOT(autoColorProbeSolution()));
    pAction->setCheckable( TRUE );
    pAction->setChecked( m_bAutoAdjustProbeSolutionColor );
    //
    pMenu->addSeparator();
    //
    QMenu* pClusterMenu = pMenu->addMenu( QIcon(QPixmap(qpdbTool)), tr("&Feature Extraction") );
    pClusterMenu->addAction( tr("&Neural Gas"), this, SLOT(clustering()) );
    pClusterMenu->addAction( tr("&Laplace Quantization"), this, SLOT(laplaceQuantization()) );
    pClusterMenu->addAction( tr("Atomic Coordinates"), this, SLOT(atomicQuantization()) );
    pClusterMenu->addAction( tr("Flex Feature-Vectors"), this, SLOT(clusteringFromCoords()) );
    pClusterMenu->addSeparator();
    pClusterMenu->addAction( tr("Import Feature-Vectors"), this, SLOT(importCodebook()) );
    pClusterMenu->addAction( tr("Export Feature-Vectors"), this, SLOT(exportCodebook()) );
    //
    m_pRenderFeatureVectors = pMenu->addAction( tr("&Render Feature-Vectors"), this, SLOT(renderCV()) );
    m_pRenderFeatureVectors->setCheckable( TRUE );
    m_pRenderFeatureVectors->setChecked( FALSE );
    //
    m_pRenderDisplacementArrows = pMenu->addAction( tr("Render &Displacements"), this, SLOT(renderDisplacement()) );
    m_pRenderDisplacementArrows->setCheckable( TRUE );
    m_pRenderDisplacementArrows->setChecked( FALSE );
    //
    pMenu->addSeparator();
    //
    QMenu* pHapticMenu = pMenu->addMenu( tr("Haptic Docking") );
    //
    m_pForces = pHapticMenu->addAction( QIcon(QPixmap(forceIcon)), tr("Turn on/off Forces"), this, SLOT(toggleForces()) );
    m_pForces->setCheckable( TRUE );
    m_pForces->setChecked( FALSE );
    //
    pHapticMenu->addSeparator();
    //
    pHapticMenu->addAction( tr("&Force Feedback Properties"), this, SLOT(showForceDlg()) );
    pHapticMenu->addAction( tr("&Motor Temperature"), this, SLOT(showTempDlg()) );
    //
    QAction* pFeatureAction = pDockToolbar->addAction( QIcon(qpdbTool), tr("&Feature Extraction"), this, SLOT(clustering()) );
    pFeatureAction->setMenu( pClusterMenu );
    pAction = pMenu->addAction( QIcon(qfitTool), tr("Feature-Based Docking"), this, SLOT(featureDock()) );
    pDockToolbar->addAction( pAction );
    pMenu->addAction( tr("Feature-Based Flexing"), this, SLOT(showFlexingDlg()) );
    pAction = pMenu->addAction( QIcon(dockTool), tr("Refinement"), this, SLOT(showRefinementDlg()) );
    pDockToolbar->addAction( pAction );
    //
    pDockToolbar->addSeparator();
    //
    m_pPDBAction_RMSD = pDockToolbar->addAction( QIcon(compute_rmsd), "RMSD", this, SLOT(showRmsdDlg()) );
    m_pPDBAction_RMSD->setToolTip( "Compute the RMSD Between Two Structures" );
    m_pVolAction_Corr = pDockToolbar->addAction( QIcon(compute_cc), tr("&Cross Correlation"), this, SLOT(calcCorrelation()));
    m_pVolAction_Corr->setToolTip( "Compute the Cross Correlation Coefficient");
    //
    /*pDockToolbar->addSeparator();
    //
    pDockToolbar->addAction( m_pForces );
    pDockToolbar->addAction(m_pStericClashing );*/
    //
    QMenu* pSimultaneousDocking = pMenu->addMenu( tr("Simultaneous Docking") );
    pSimultaneousDocking->addAction( tr(" Start"), this, SLOT(showGaDlg()), QKeySequence(Qt::CTRL+Qt::Key_G) ); 
    m_pShowFitnessCurve = pSimultaneousDocking->addAction( tr(" Show Fitness Curve "), this, SLOT(showFitnessCurveDlg()), QKeySequence(Qt::CTRL+Qt::Key_C) ); 
    m_pShowFitnessCurve->setCheckable(TRUE);
    m_pShowFitnessCurve->setChecked(FALSE);
    pSimultaneousDocking->addAction( tr(" Stop"), this, SLOT(stopGa()) );
    //
    QMenu* pExtractCylinders = pMenu->addMenu( tr("Volume Tracer") );
    pExtractCylinders->addAction( tr(" Start"), this, SLOT(showGaCylDlg() ), QKeySequence(Qt::CTRL+Qt::Key_T) ); 
    pExtractCylinders->addAction( tr(" Stop"), this, SLOT(stopGaCyl()) );

    //
    // volume menu and toolbar
    //

    m_pVolumeMenu = menuBar()->addMenu( tr("&Volume") );
    //
    m_pVolAction_Table = m_pVolumeMenu->addAction( tr("&Situs Table"), m_pDocumentList, SLOT(currentVolume_showTable()) );
    m_pVolAction_Mapexp = m_pVolumeMenu->addAction( QIcon(mapExplorerTool), "Map Explorer", m_pDocumentList, SLOT(currentVolume_mapExplorer()) );
    m_pVolAction_Mapexp->setToolTip( "2D Visualization of volumetric maps" );
    m_pVolAction_Hist = m_pVolumeMenu->addAction( tr("&Histogram"), m_pDocumentList, SLOT(currentVolume_showHistogramDlg()) );
    m_pVolAction_Occvox = m_pVolumeMenu->addAction( tr("&Occupied Voxels"), m_pDocumentList, SLOT(currentVolume_calcOccupied()) );
    m_pDockToLaplace = m_pVolumeMenu->addAction( tr("Laplace Docking"), m_pDocumentList, SLOT(currentVolume_setDockToLaplace()) );
    //
    m_pVolumeMenu->addSeparator();
    //
    m_pVolAction_ChangeVW = m_pVolumeMenu->addAction( tr("Change &Voxel Size"), m_pDocumentList, SLOT(currentVolume_changeVoxelWidth()) );
    m_pVolAction_ChangeOrig = m_pVolumeMenu->addAction( tr("Change Or&igin"), m_pDocumentList, SLOT(currentVolume_changeOrigin()) );
    m_pVolAction_SwapAxes = m_pVolumeMenu->addAction( tr("Swap &Axes"), m_pDocumentList, SLOT(currentVolume_swapAxis()));
    m_pVolAction_Normalize = m_pVolumeMenu->addAction( tr("&Normalize"), m_pDocumentList, SLOT(currentVolume_normalize()) );
    m_pVolAction_Threshold = m_pVolumeMenu->addAction( tr("Th&reshold"), m_pDocumentList, SLOT(currentVolume_threshold()) );
    m_pVolAction_Scale = m_pVolumeMenu->addAction( tr("&Scale"), m_pDocumentList, SLOT(currentVolume_scale()) );
    m_pVolAction_Crop = m_pVolumeMenu->addAction( QIcon(volCropTool), tr("Crop"), m_pDocumentList, SLOT(currentVolume_crop()) );
    m_pVolAction_Crop->setToolTip( "Crop a volumetric map" );
    m_pVolAction_Floodfill = m_pVolumeMenu->addAction( tr("&Floodfill"), m_pDocumentList, SLOT(currentVolume_floodfill()) );
    m_pVolAction_MultiPointFloodfill = m_pVolumeMenu->addAction( tr("&Multi Point Floodfill"), m_pDocumentList, SLOT(currentVolume_multiPointFloodfill()) );
    m_pVolAction_Gaussian = m_pVolumeMenu->addAction( tr("&Gaussian"), m_pDocumentList, SLOT(currentVolume_gaussian()) );
    m_pVolAction_Laplacian = m_pVolumeMenu->addAction( tr("&Laplacian"), m_pDocumentList, SLOT(currentVolume_laplacian()) );
    m_pVolAction_LocalNormalization = m_pVolumeMenu->addAction( tr("L&ocal Normalization"), m_pDocumentList, SLOT(currentVolume_localNorm()) );
    //m_pVolAction_BilateralFilter = m_pVolumeMenu->addAction( tr("&BilateralFilter"), m_pDocumentList, SLOT(currentVolume_bilateralFilter()) );
    m_pVolAction_DPSV_filter = m_pVolumeMenu->addAction( tr("&DPSV Filter") , m_pDocumentList, SLOT(currentVolume_DPSV_filter()) );
    //
    m_pVolumeMenu->addSeparator();
    //
    m_pVolumeMenu->addAction(m_pVolAction_Corr);
    //
    QToolBar* pVolumeToolbar = addToolBar( tr("V&olume Tools") );
    pVolumeToolbar->setObjectName( QString("Volume_Tools") );
    pVolumeToolbar->addAction( m_pVolAction_Mapexp );
    pVolumeToolbar->addAction( m_pVolAction_Crop );
    //
    // initially, switch this menu off
    m_pVolumeMenu->setEnabled( FALSE );
    //
    // create the context menu for volumes by copying the volume menu and adding save/remove actions
    m_pVolumeContextMenu = new QMenu();
    m_pVolumeContextMenu->addAction(pSaveAction);
    m_pVolumeContextMenu->addAction(pCloseAction);
    m_pVolumeContextMenu->addSeparator();
    m_pVolumeContextMenu->addAction(pFeatureAction);
    m_pVolumeContextMenu->addSeparator();
    m_pVolumeContextMenu->addActions( m_pVolumeMenu->actions() );

    //
    // structure menu and toolbar
    //

    m_pStructureMenu = menuBar()->addMenu( tr("&Structure") );
    QToolBar* pStructureToolbar = addToolBar( tr("&PDB Tools") );
    pStructureToolbar->setObjectName( QString("Structure_Tools") );
    //
    m_pPDBAction_Table = m_pStructureMenu->addAction( tr("&Atom Detail Table"), m_pDocumentList, SLOT( currentPDB_showTable()) );
    m_pPDBAction_Sphere = m_pStructureMenu->addAction( tr("S&phericity"), m_pDocumentList, SLOT( currentPDB_showSphericity()) );
    m_pStructureMenu->addAction( m_pPDBAction_RMSD );
    m_pPDBAction_Blur = m_pStructureMenu->addAction( tr("&Blur"), m_pDocumentList, SLOT( currentPDB_blur()) );
    //
    m_pStructureMenu->addSeparator();
    //
    m_pPDBAction_Extract = m_pStructureMenu->addAction( tr("&Extract...") , m_pDocumentList, SLOT(currentPDB_showExtractDlg()) );
    m_pPDBAction_Merge = m_pStructureMenu->addAction(tr("&Merge"), this, SLOT(mergePdbDocs()) );
    m_pPDBAction_Symmetry = m_pStructureMenu->addAction( tr("Apply Symmetry"), m_pDocumentList, SLOT(currentPDB_showSymmetryDlg()) );
    //
    m_pStructureMenu->addSeparator();
    //
    m_pPDBAction_PDF = m_pStructureMenu->addAction( tr("&PDF"), this, SLOT( showPdfDlg()) );
    m_pPDBAction_DiffPDF = m_pStructureMenu->addAction( tr("&Difference PDF"), this, SLOT( showDiffPdfDlg()) );
    //
    m_pStructureMenu->addSeparator();
    //
    QMenu* pRender = m_pStructureMenu->addMenu( tr("Drawing Mode") );
    //
    pRender->addAction( tr("Off"), this, SLOT( changeRenderModeOFF() ));
    //
    pAction = pStructureToolbar->addAction( QIcon(mode_points), tr("Point Mode"), this, SLOT(changeRenderModePOINT()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to Point Graphics Mode") );
    pRender->addAction( pAction );
    //
    pAction = pStructureToolbar->addAction( QIcon(mode_lines), tr("Line Mode"), this, SLOT(changeRenderModeLINE()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to Line Mode") );
    pRender->addAction( pAction );
    //
    pAction = pStructureToolbar->addAction( QIcon(mode_vdw), tr("VDW Mode"), this, SLOT(changeRenderModeVDW()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to VDW Mode") );
    pRender->addAction( pAction );
    //
    pAction = pStructureToolbar->addAction( QIcon(mode_cpk), tr("CPK Mode"), this, SLOT(changeRenderModeCPK()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to CPK Mode") );
    pRender->addAction( pAction );
    //
    pAction = pStructureToolbar->addAction( QIcon(mode_licorice), tr("Licorice Mode"), this, SLOT(changeRenderModeLICORICE()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to Licorice Mode") );
    pRender->addAction( pAction );
    //
    pAction = pStructureToolbar->addAction( QIcon(mode_tube), tr("Tube Mode"), this, SLOT(changeRenderModeTUBE()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to Tube Mode") );
    pRender->addAction( pAction );
    //
    pAction = pStructureToolbar->addAction( QIcon(mode_cartoon), tr("Cartoon Mode"), this, SLOT(changeRenderModeCARTOON()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to Cartoon Mode") );
    pRender->addAction( pAction );
    //
    //pAction = pStructureToolbar->addAction( QIcon(mode_toon), tr("Toon Mode"), this, SLOT(changeRenderModeTOON()) );
    //pAction->setToolTip( tr("Change Selected PDB Document(s) to Toon Mode") );
    //pRender->addAction( pAction );
    //
    pStructureToolbar->addSeparator();
    //
    QMenu * pColoring = m_pStructureMenu->addMenu( tr("Coloring") );
    //
    pAction = pColoring->addAction( QIcon(colmap_solid), tr("Solid Coloring"), this, SLOT(changeColmapModeSOLID()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to Uniform Coloring") );
    pStructureToolbar->addAction( pAction );
    //
    pAction = pColoring->addAction( QIcon(colmap_rainbow), tr("Solid Rainbow Coloring"), this, SLOT(changeColmapModeRAINBOW()) );
    pAction->setToolTip( tr("Change Selected PDB Document(s) to Rainbow Coloring") );
    pStructureToolbar->addAction( pAction );
    //
    pColoring->addAction( tr("Atom"),      this, SLOT(changeColmapModeTYPE()) );
    pColoring->addAction( tr("Residue"),   this, SLOT(changeColmapModeRESNAME()) );
    pColoring->addAction( tr("Chain"),     this, SLOT(changeColmapModeCHAINID()) );
    pColoring->addAction( tr("Structure"), this, SLOT(changeColmapModeSTRUCTURE()) );
    pColoring->addAction( tr("Segment"),   this, SLOT(changeColmapModeSEGID()) );
    pColoring->addAction( tr("Model"),     this, SLOT(changeColmapModeMODEL()) );
    //
    // initially, switch this menu off
    m_pStructureMenu->setEnabled( FALSE );
    //
    // create the context menu for structures by copying the structure menu and adding save/remove actions
    m_pStructureContextMenu = new QMenu();
    m_pStructureContextMenu->addAction(pSaveAction);
    m_pStructureContextMenu->addAction(pCloseAction);
    m_pStructureContextMenu->addSeparator();
    m_pStructureContextMenu->addAction(pFeatureAction);
    m_pStructureContextMenu->addSeparator();
    m_pStructureContextMenu->addActions( m_pStructureMenu->actions() );

    //
    // help menu
    //

    menuBar()->addSeparator();
    pMenu = menuBar()->addMenu( tr("&Help") );
    pMenu->addAction( tr("&About"), this,SLOT(showAboutDlg()) );

    // as long as we dont have a documentation ready, we better comment this out.
    //pMenu->addSeparator();
    //m_pHelp = pMenu->addAction( QIcon(helpTool), tr("Help"), this, SLOT(help()), QKeySequence(Qt::CTRL+Qt::Key_F1) );
    //pHelpToolbar = addToolBar( tr("Help") );
    //pHelpToolbar->addAction(m_pHelp);


    pToolbarMenu->addAction( pFileToolbar->toggleViewAction() );
    pToolbarMenu->addAction( pEditToolbar->toggleViewAction() );
    pToolbarMenu->addAction( pViewToolbar->toggleViewAction() );
    pToolbarMenu->addAction( pDockToolbar->toggleViewAction() );
    pToolbarMenu->addAction( pVolumeToolbar->toggleViewAction() );
    pToolbarMenu->addAction( pStructureToolbar->toggleViewAction() );
    //pToolbarMenu->addAction( pHelpToolbar->toggleViewAction() );
}

 /**
 * Load a config string from .sculptorrc and try to restore the QMainWindow state
 * (dockwidgets/toolbars/window geometry)
 */
void sculptor_window::restoreWindowState()
{
    QByteArray oState;
    char  cChar;
    char* pChar;
    char* pState = NULL;

    // if not done before, save the current state as default
    if (m_oDefaultWindowState.size() == 0)
    {
        m_oDefaultWindowState = saveState();
        m_oDefaultWindowSize = size();
    }

    // restore window size
    int w, h;
    w = loadConfigValue("WIDTH",  size().width(),  "MAIN_WINDOW");
    h = loadConfigValue("HEIGHT", size().height(), "MAIN_WINDOW");

    resize( QSize(w,h) );

    // now get the window state string
    string oStr = loadConfigValue("STATE", string(""), "MAIN_WINDOW");
    if ( !oStr.empty() )
    {
        pState = new char[oStr.length()+1];
        strcpy(pState, oStr.c_str());
    }

    // and convert it into a QByteArray
    if (pState != NULL)
    {
        pChar = strtok (pState, " ");
        if (pChar == NULL)
            return;

        while (pChar != NULL)
        {
            cChar = (char) atoi(pChar);
            oState.append(cChar);
            pChar = strtok (NULL, " ");
        }

        // let QMainWindow restore state
        restoreState(oState);
    }

    delete[] pState;
}

/**
 * Save a config string to .sculptorrc for the QMainWindow state
 * (dockwidgets/toolbars/window geometry)
 */
void sculptor_window::saveWindowState()
{
    QByteArray oState;
    string oStr;
    char pCh[6];

    saveConfigValue("WIDTH",  size().width(),  "MAIN_WINDOW");
    saveConfigValue("HEIGHT", size().height(), "MAIN_WINDOW");

    // get the window state
    oState = saveState();

    if (oState.size()>0)
    {
        // convert the QByteArray into a string consisting of integers
        for (int i=0; i<oState.size(); ++i)
        {
            sprintf(pCh, "%i ", (int)(oState[i]));
            oStr.append(pCh);
        }
        oStr.append("\0");

        // save the string into the sculptor config file
        saveConfigValue("STATE", oStr, "MAIN_WINDOW");
    }
}

/**
 * add the default document - has to be done after the svt scene was created!
 */
void sculptor_window::addDefaultDoc()
{
#ifdef SVT_SYSTEM_QT
    ((sculptor_splitter*)m_pRightSplitter)->insertWidget( 0, svt_qtGetWidget() );
#endif

    // check if the window state should be restored automatically
    m_bRestoreWindowStateOnStart = (bool)loadConfigValue("RESTORE_WINDOW_STATE", 1, "MAIN_WINDOW");
    if (m_bRestoreWindowStateOnStart)
        restoreWindowState();

    //
    // scene doc
    //

    m_pSceneDoc = new scene_document();
    m_pSceneDoc->setScene(m_pScene);
    addDoc( m_pSceneDoc );

    m_pCappedClip->setChecked( m_pScene->getCappedClip() );

    //
    // force feedback
    //

    m_pForceAlgo = new sculptor_force_algo(m_pScene->getForceTool());
    m_pScene->setForceAlgo(m_pForceAlgo);

    // Init for solution set
    m_bSolutionSetChanged = false;

    // Flag if warning for moving a solution was shown
    m_bMovingSolutionWarningShown = false;

    //
    // fps
    //
    //QTimer *tim = new QTimer( this );
    //connect( tim, SIGNAL(timeout()), SLOT(showFPS()) );
    //tim->start( 750, FALSE );
};

/**
 * update the menubar regarding the recently loaded documents
 */
void sculptor_window::updateRecentList()
{
    if (m_pRecentMenu != NULL)
    {
        m_pRecentMenu->clear();
        m_aRecentIDs.clear();

        for(unsigned int i=0; i < m_aRecentDN.size() && i < 10; i++)
        {
            if (m_aRecentDN[i].length() > 0)
	    {
      		m_pRecentAction = m_pRecentMenu->addAction( StdtoQ(m_aRecentDN[i]) );
		m_pRecentAction->setData( QVariant(i) );
		m_aRecentIDs.push_back( i );
	    }
            else
	    {
                m_pRecentAction = m_pRecentMenu->addAction( StdtoQ(m_aRecentFileA[i]) ); 
		m_pRecentAction->setData( QVariant(i) );
                m_aRecentIDs.push_back( i );
	    }
        }
    }
};

/**
 * load a recently loaded document
 */
void sculptor_window::sDocLoadRecent(QAction* pAction)
{
    int iNewDoc = -1;
    int iAction = pAction->data().toInt();
  
    for(unsigned int i=0; i<m_aRecentIDs.size(); i++)
    {
        if (m_aRecentIDs[i] == iAction)
        {
            if (m_aRecentDN[i].length() > 0)
                iNewDoc = loadDoc( StdtoQ(m_aRecentFileA[i]), StdtoQ(m_aRecentFileB[i]), StdtoQ(m_aRecentDN[i]) );
            else
                iNewDoc = loadDoc( StdtoQ(m_aRecentFileA[i]), StdtoQ(m_aRecentFileB[i]) );
        }
    }

    setDefaultProbeTargetDoc();

    // make the new document current and selected, and show its property dialog
    if (iNewDoc != -1)
    {
        m_pDocumentList->setCurrentItem(iNewDoc);
        m_pDocumentList->showPropDialog();
    }

    // scene becomes moved doc
    setMovedDoc(0);
};

/**
 * load the information about the recently loaded documents
 */
void sculptor_window::loadRecentList()
{
    string oFilename = svt_getHomeDir() + "/.sculptorrc";

    m_aRecentDN.clear();
    m_aRecentFileA.clear();
    m_aRecentFileB.clear();

    // scl file
    TiXmlDocument oXMLDoc;
    if (oXMLDoc.LoadFile(oFilename.c_str()) && oXMLDoc.RootElement() )
    {
        // QT3FIXMEif ( oXMLDoc.RootElement()->Attribute("STYLE") )
        //     g_qApp->makeStyle( QString( oXMLDoc.RootElement()->Attribute("STYLE") ) );

        TiXmlElement* pRecent = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild("RECENT");

        if (pRecent)
        {
            for ( TiXmlElement* pElem = (TiXmlElement*) pRecent->FirstChild(); pElem != 0; pElem = (TiXmlElement*) pElem->NextSibling())
            {
                m_aRecentDN.push_back( string( pElem->Attribute("DISPLAYNAME") ) );
                m_aRecentFileA.push_back( string( pElem->Attribute("SRC") ) );
                m_aRecentFileB.push_back( string( pElem->Attribute("SRCB") ) );
            }
        }

        TiXmlElement* pScripts = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild("SCRIPTS");
        if (pScripts)
        {
            for ( TiXmlElement* pElem = (TiXmlElement*) pScripts->FirstChild(); pElem != 0; pElem = (TiXmlElement*) pElem->NextSibling())
            {
                m_aScriptTitles.push_back( string( pElem->Attribute("TITLE") ) );
                m_aScripts.push_back( string( pElem->Attribute("CODE") ) );
            }
        }

        TiXmlElement* pStereo = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild("STEREO");
        if (pStereo)
        {
            double fEye;
            if (pStereo->Attribute( "EYESEPARATION", &fEye ) )
                svt_setEyeSeparation( fEye );
        }
    }

    updateRecentList();
};

/**
 * save the information about the recently loaded documents
 */
void sculptor_window::saveRecentList()
{
    vector< string > aRecentFileA;
    vector< string > aRecentFileB;
    vector< string > aRecentDN;

    if (m_aRecentFileA.size() > 0)
    {
        for(unsigned int i=0; i < m_aRecentFileA.size(); i++)
        {
            bool bDouble = false;
            if (i > 0)
                for(unsigned int j=0; j < i; j++)
                {
                    if ( m_aRecentFileA[i].compare( m_aRecentFileA[j] ) == 0 && m_aRecentFileB[i].compare( m_aRecentFileB[j] ) == 0 )
                        bDouble = true;
                }

            if (!bDouble)
            {
                aRecentFileA.push_back( m_aRecentFileA[i] );
                aRecentFileB.push_back( m_aRecentFileB[i] );
                aRecentDN.push_back( m_aRecentDN[i] );
            }
        }

        m_aRecentFileA = aRecentFileA;
        m_aRecentFileB = aRecentFileB;
        m_aRecentDN = aRecentDN;
    }

    // rc filename
    string oFilename = svt_getHomeDir() + "/.sculptorrc";
    TiXmlDocument oXMLDoc;
    bool bNewfile;

    // if the file does already exist, load it, check for existence of nodes and modify them;
    // if it does not exist, create it and add the nodes
    bNewfile = !oXMLDoc.LoadFile(oFilename.c_str());
    
    // if we create a new file, add XML declaration
    if (bNewfile)
    {
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "", "");
	oXMLDoc.LinkEndChild( pDecl );
    }


    // sculptor root node
    TiXmlElement* pMain;
    if (bNewfile || oXMLDoc.FirstChild("SCULPTOR") == NULL)
    {
	pMain = new TiXmlElement("SCULPTOR");
	oXMLDoc.LinkEndChild( pMain );
    }
    else
    {
	pMain = (TiXmlElement*)oXMLDoc.FirstChild("SCULPTOR");
    }

    pMain->SetAttribute("VERSION", SCULPTOR_VERSION);
    if (!g_qApp->getStyleName().isEmpty())
	pMain->SetAttribute("STYLE", QtoStd(g_qApp->getStyleName()) );


    // recent file list
    TiXmlElement* pRecent;
    if (bNewfile || pMain->FirstChild("RECENT") == NULL)
    {
	pRecent = new TiXmlElement("RECENT");
	pMain->LinkEndChild( pRecent );
    }
    else
    {
	pRecent = (TiXmlElement*)pMain->FirstChild("RECENT");
	pRecent->Clear();
    }

    TiXmlElement* pDoc = NULL;
    for(unsigned int i=0; i < m_aRecentDN.size() && i < 10; i++)
    {
        pDoc = new TiXmlElement( "LOADED" );

        pDoc->SetAttribute( "SRC",         m_aRecentFileA[i].c_str() );
        pDoc->SetAttribute( "SRCB",        m_aRecentFileB[i].c_str() );
        pDoc->SetAttribute( "DISPLAYNAME", m_aRecentDN[i].c_str()    );

        pRecent->LinkEndChild( pDoc );
    }


    // script list
    TiXmlElement* pScripts;
    if (bNewfile || pMain->FirstChild("SCRIPTS") == NULL)
    {
	pScripts = new TiXmlElement("SCRIPTS");
	pMain->LinkEndChild( pScripts );
    }
    else
    {
	pScripts = (TiXmlElement*)pMain->FirstChild("SCRIPTS");
	pScripts->Clear();
    }

    TiXmlElement* pScript = NULL;
    for(unsigned int i=0; i < m_aScripts.size(); i++)
    {
        pScript = new TiXmlElement( "SCRIPT" );

        pScript->SetAttribute( "TITLE",       m_aScriptTitles[i].c_str() );
        pScript->SetAttribute( "CODE",        m_aScripts[i].c_str() );

        pScripts->LinkEndChild( pScript );
    }


    // stereo parameters
    TiXmlElement* pStereo;
    if (bNewfile || pMain->FirstChild("STEREO") == NULL)
    {
	pStereo = new TiXmlElement("STEREO");
	pMain->LinkEndChild( pStereo );
    }
    else
    {
	pStereo = (TiXmlElement*)pMain->FirstChild("STEREO");
    }

    pStereo->SetDoubleAttribute( "EYESEPARATION", svt_getEyeSeparation() );

    oXMLDoc.SaveFile(oFilename);
};

/**
 *
 */
int sculptor_window::loadConfigValue(const char * pAttribute, int iDefault, const char* pElement)
{
    string oFilename = svt_getHomeDir() + "/.sculptorrc";
    int i;

    // rc file
    TiXmlDocument oXMLDoc;
    if (oXMLDoc.LoadFile(oFilename.c_str()) && oXMLDoc.RootElement())
    {
	TiXmlElement * pGeneralSettings = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild(pElement);

	if (pGeneralSettings != NULL)
	{
	    if (pGeneralSettings->Attribute(pAttribute, &i))
		return i;
	}
    }

    return iDefault;
}

/**
 *
 */
Real32 sculptor_window::loadConfigValue(const char * pAttribute, Real32 fDefault, const char* pElement)
{
    string oFilename = svt_getHomeDir() + "/.sculptorrc";
    double d;

    // rc file
    TiXmlDocument oXMLDoc;
    if (oXMLDoc.LoadFile(oFilename.c_str()) && oXMLDoc.RootElement())
    {
	TiXmlElement * pGeneralSettings = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild(pElement);

	if (pGeneralSettings != NULL)
	{
	    if (pGeneralSettings->Attribute(pAttribute, &d))
		return (Real32) d;
	}
    }

    return fDefault;
}

/**
 *
 */
string sculptor_window::loadConfigValue(const char * pAttribute, string oDefault, const char* pElement)
{
    string oFilename = svt_getHomeDir() + "/.sculptorrc";

    // rc file
    TiXmlDocument oXMLDoc;
    if (oXMLDoc.LoadFile(oFilename.c_str()) && oXMLDoc.RootElement())
    {
	TiXmlElement * pElementNode = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild(pElement);

	if ( pElementNode != NULL && pElementNode->Attribute(pAttribute) )
            oDefault = pElementNode->Attribute(pAttribute);
    }

    return oDefault;
}

/**
 *
 */
void sculptor_window::saveConfigValue(const char * pAttribute, int iValue, const char* pElement)
{
    string oFilename = svt_getHomeDir() + "/.sculptorrc";
    int i = iValue;

    // rc file. if non-existent, create it and add sculptor main node, version and style
    TiXmlDocument oXMLDoc;

    if (!oXMLDoc.LoadFile(oFilename.c_str()))
    {
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "", "");
	oXMLDoc.LinkEndChild( pDecl );
    }
    if (oXMLDoc.FirstChild("SCULPTOR") == NULL)
    {
	// sculptor root node
	TiXmlElement* pMain;
	pMain = new TiXmlElement("SCULPTOR");
	pMain->SetAttribute("VERSION", SCULPTOR_VERSION);
	if (!g_qApp->getStyleName().isEmpty())
	    pMain->SetAttribute("STYLE", QtoStd(g_qApp->getStyleName()) );

	oXMLDoc.LinkEndChild( pMain );
    }

    TiXmlElement * pElementNode = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild(pElement);

    if (pElementNode == NULL)
    {
	pElementNode = new TiXmlElement(pElement);
	oXMLDoc.RootElement()->LinkEndChild(pElementNode);
    }

    pElementNode->SetAttribute(pAttribute, i);

    oXMLDoc.SaveFile(oFilename);
}

/**
 *
 */
void sculptor_window::saveConfigValue(const char * pAttribute, Real32 fValue, const char* pElement)
{
    string oFilename = svt_getHomeDir() + "/.sculptorrc";
    double d = (double) fValue;

    // rc file. if non-existent, create it and add sculptor main node, version and style
    TiXmlDocument oXMLDoc;

    if (!oXMLDoc.LoadFile(oFilename.c_str()))
    {
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "", "");
	oXMLDoc.LinkEndChild( pDecl );
    }
    if (oXMLDoc.FirstChild("SCULPTOR") == NULL)
    {
	// sculptor root node
	TiXmlElement* pMain;
	pMain = new TiXmlElement("SCULPTOR");
	pMain->SetAttribute("VERSION", SCULPTOR_VERSION);
	if (!g_qApp->getStyleName().isEmpty())
	    pMain->SetAttribute("STYLE", QtoStd(g_qApp->getStyleName()) );

	oXMLDoc.LinkEndChild( pMain );
    }

    TiXmlElement * pElementNode = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild(pElement);

    if (pElementNode == NULL)
    {
	pElementNode = new TiXmlElement(pElement);
	oXMLDoc.RootElement()->LinkEndChild(pElementNode);
    }

    pElementNode->SetDoubleAttribute(pAttribute, d);

    oXMLDoc.SaveFile(oFilename);
}

/**
 *
 */
void sculptor_window::saveConfigValue(const char * pAttribute, string oValue, const char* pElement)
{
    string oFilename = svt_getHomeDir() + "/.sculptorrc";

    // rc file. if non-existent, create it and add sculptor main node, version and style
    TiXmlDocument oXMLDoc;

    if (!oXMLDoc.LoadFile(oFilename.c_str()))
    {
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "", "");
	oXMLDoc.LinkEndChild( pDecl );
    }
    if (oXMLDoc.FirstChild("SCULPTOR") == NULL)
    {
	// sculptor root node
	TiXmlElement* pMain;
	pMain = new TiXmlElement("SCULPTOR");
	pMain->SetAttribute("VERSION", SCULPTOR_VERSION);
	if (!g_qApp->getStyleName().isEmpty())
	    pMain->SetAttribute("STYLE", QtoStd(g_qApp->getStyleName()) );

	oXMLDoc.LinkEndChild( pMain );
    }

    TiXmlElement * pElementNode = (TiXmlElement*) oXMLDoc.RootElement()->FirstChild(pElement);

    if (pElementNode == NULL)
    {
	pElementNode = new TiXmlElement(pElement);
	oXMLDoc.RootElement()->LinkEndChild(pElementNode);
    }

    pElementNode->SetAttribute(pAttribute, oValue.c_str());

    oXMLDoc.SaveFile(oFilename);
}

/**
 * Create and set a new style
 * \param style string with the style name, e.g. Sculptor or Windows
 */
void sculptor_window::makeStyle(const QString &style)
{
    g_qApp->makeStyle( style );
    saveRecentList();
};

/**
 * add a new document. sets the scene as moved document and, makes the document current and shows
 * its property dialog
 * \param pSD pointer to the new sculptor_document
 * \param cFilename QString object with the filename
 * \return index of the new document in the list
 */
int sculptor_window::addDoc(sculptor_document* pSD, QString oDisplayName)
{
    // if given, save the displayname in the document
    if ( !oDisplayName.isEmpty() )
        pSD->setDisplayName( QtoStd(oDisplayName) );

    // create new listbox item
    window_documentlistitem* pDLI = new window_documentlistitem(pSD);
    int iNewDoc = m_pDocumentList->addItem( pDLI );

    // make it current (also selects it)
    m_pDocumentList->setCurrentItem(iNewDoc);

    // show the property dialog of the current item
    m_pDocumentList->showPropDialog();

    // scene is the moved doc
    setMovedDoc(0);

    // close intro, show data
    getScene()->switchToData();

    // return the number of the new doc
    return iNewDoc;
}

/**
 * add a new document (does not change the moved doc, the current doc, and does not show the
 * property dialog)
 * \param pSD pointer to the new sculptor_document
 * \param cFilename QString object with the filename
 * \return index of the new document in the list
 */
int sculptor_window::addDocSimple(sculptor_document* pSD, QString oDisplayname)
{
    // set the displayname
    if ( !oDisplayname.isEmpty() )
        pSD->setDisplayName( QtoStd(oDisplayname) );

    // add the new document to the documentlist
    window_documentlistitem* pDLI = new window_documentlistitem(pSD);
    int iNewDoc = m_pDocumentList->addItem( pDLI );

    // return the index of the new doc
    return iNewDoc;
}

/**
 * set the moved document
 * \param iDoc number of the sculptor_document which should be moved around
 */
void sculptor_window::setMovedDoc(int iDoc)
{
    int i;

    if (iDoc == -1)
	iDoc = m_pDocumentList->getCurrentIndex();

    if (iDoc == -1)
	return;

    // situs documents cannot be moved!
    if ( m_pDocumentList->getDocType(iDoc) == SENSITUS_DOC_SITUS )
        return;

    // set all other document's move status to false
    for ( i=0; i<m_pDocumentList->count(); ++i )
	if ( iDoc != i && (*m_pDocumentList)[i]->getMoved() )
	    (*m_pDocumentList)[i]->setMoved(false);

    // now set the move status of the document to true
    disableInput();
    (*m_pDocumentList)[iDoc]->setMoved(true);
    enableInput();

    // reset the transform dialog
    resetTransformDlg();

    // update the state of the menu item Edit->Moved
    updateMenuItems();
}

///////////////////////////////////////////////////////////////////////////////
// document management
///////////////////////////////////////////////////////////////////////////////

/**
 * load a document
 */
int sculptor_window::loadDoc(QString oFileA, QString oFileB, QString oDisplayName)
{
    // the index of the new document. init it to -1, so one can check
    // if a document was loaded successfully
    int iNewDoc = -1;

    // is this an absolute or relative path?
    char oCWD[2048];
    if (getcwd( oCWD, 2047 ) != NULL)
    {
        // do we have an absolute or relative path?
#ifdef WIN32
        if ( oFileA[1] != ':' && oFileA.compare("NONE") != 0)
#else
        if ( oFileA[0] != QDir::separator () && oFileA.compare("NONE") != 0)
#endif
            oFileA = QString(oCWD) + QDir::separator () + oFileA;
#ifdef WIN32
        if ( oFileB.compare("NONE") != 0 && oFileB[1] != ':' )
#else
        if ( oFileB.compare("NONE") != 0 && oFileB[0] != QDir::separator () )
#endif
            oFileB = QString(oCWD) + QDir::separator () + oFileB;
    }

    // convert all separators to the correct form for the current OS
    oFileA = QDir::convertSeparators( oFileA );
    oFileB = QDir::convertSeparators( oFileB );
#ifndef WIN32
    // if not under Windows, the backslashes need to be exchanged (the convertSeparators call above does not seem to be sufficient).
    oFileA.replace("\\", "/");
    oFileB.replace("\\", "/");

    // if not under Windows, the drive letters need to be removed
    if (oFileA[1] == ':')
        oFileA = oFileA.right( oFileA.length() - 2 );
    if (oFileB[1] == ':')
        oFileB = oFileB.right( oFileB.length() - 2 );
#else
    // if under Windows, the slashes need to be exchanged
    oFileA.replace("/", "\\");
    oFileB.replace("/", "\\");
#endif

    // check, if file exists
    if (!QFileInfo(oFileA).exists())
    {
        // statusbar message
        QString s;
        s += "Error: File ";
        s += oFileA;
	s += " does not exist!";
        statusBar()->showMessage( s, 10000 );
        svtout << "Error: File " << QtoStd(oFileA) << " does not exist!" << endl;
        return -1;
    }

    // get path information and set the current directory to this path
    QFileInfo oDirInf( oFileA );
    QString oCurrDir = oDirInf.absolutePath();

    // Sculptor archive
    if ( oFileA.right(3).toLower().compare( QString("sar") )  == 0 )
    {
        loadArchive( oFileA );
        return -1;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // LUA script
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if ( oFileA.right(3).toLower().compare( QString("lua") )  == 0 )
    {
        FILE* pFile = fopen( oFileA.toAscii(), "r" );
        char pBuf[256];
        char *pDummy;
        string oScript;
        while(!feof(pFile))
        {
            pDummy = fgets( pBuf, 256, pFile );
            oScript += pBuf;
        }
        string oName = QtoStd(QFileInfo(oFileA).fileName());
        scriptAdd( oName, oScript );
        fclose( pFile );
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // SITUS / MRC / CCP4 / SPIDER
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if (   oFileA.right(5).toLower().compare( QString("situs") )  == 0
	|| oFileA.right(3).toLower().compare( QString("map") )    == 0 
	|| oFileA.right(3).toLower().compare( QString("sit") )    == 0 
	|| oFileA.right(3).toLower().compare( QString("mrc") )    == 0 
	|| oFileA.right(4).toLower().compare( QString("ccp4") )   == 0
	|| oFileA.right(3).toLower().compare( QString("spi") )    == 0
        || oFileA.right(6).toLower().compare( QString("spider") ) == 0 )
    {

#ifdef PHANTOM_PATCH
	if (   (m_pScene->getForceTool() != NULL)
	    && (strcmp(m_pScene->getForceTool()->getDevice()->getName(),"mouse") != 0) )
	{
	    svtout << "Stopping scheduler" << endl;
	    hdStopScheduler();
	}
#endif

        // situs file
        situs_document* sd = new situs_document( m_pScene->getDataTopNode(), QtoStd(oFileA) );

        if (sd->getExtX() == 0 || sd->getExtY() == 0 || sd->getExtZ() == 0)
        {
            delete sd;
            return -1;
        }

        // document position
        svt_vector4f oDocOrigin = sd->getRealPos();
        oDocOrigin.multScalar( 1.0E8f );
        if (sd->getNode()->getTransformation() == NULL)
            sd->getNode()->setTransformation( new svt_matrix4f() );
        sd->getNode()->getTransformation()->setFromTranslation( oDocOrigin.x(), oDocOrigin.y(), oDocOrigin.z());
        sd->storeInitMat();
        svt_matrix4<Real64> oMat = svt_matrix4<Real64>( *sd->getNode()->getTransformation() );

        // add it to the document lists
        if ( oDisplayName.isEmpty() )
            iNewDoc = addDocSimple( (sculptor_document*)sd );
        else
            iNewDoc = addDocSimple( (sculptor_document*)sd, oDisplayName );

#ifdef PHANTOM_PATCH
	if (   (m_pScene->getForceTool() != NULL)
	    && (strcmp(m_pScene->getForceTool()->getDevice()->getName(),"mouse") != 0) )
	{
	    svtout << "Starting scheduler" << endl;
	    hdStartScheduler();
	}
#endif

    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // PDB + PSF
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if ( (oFileA.right(4).contains( QString("pdb"), Qt::CaseInsensitive )  > 0) && (oFileB.right(3).toLower().compare( QString("psf") ) == 0) )
    {
        // pdb and psf file
	pdb_document* pDoc = new pdb_document(m_pScene->getDataTopNode(), QtoStd(oFileA), SVT_PDB, QtoStd(oFileB), SVT_PSF);

        // add it to the document lists
        if ( oDisplayName.isEmpty() )
            iNewDoc = addDocSimple( (sculptor_document*)pDoc );
        else
            iNewDoc = addDocSimple( (sculptor_document*)pDoc, oDisplayName );
    } 


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // PSF + DCD
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if ( (oFileA.right(3).toLower().compare( QString("psf") ) == 0) && (oFileB.right(3).toLower().compare( QString("dcd") ) == 0) )
    {
        // pdb and dcd file
        pdb_document* pDoc = new pdb_document(m_pScene->getDataTopNode(), QtoStd(oFileA), SVT_PSF, QtoStd(oFileB), SVT_DCD);

        // add it to the document lists
        if ( oDisplayName.isEmpty() )
            iNewDoc = addDocSimple( (sculptor_document*)pDoc );
        else
            iNewDoc = addDocSimple( (sculptor_document*)pDoc, oDisplayName );
    } 


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // PDB
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if ( (oFileA.right(4).contains( QString("pdb"), Qt::CaseInsensitive ) > 0) && (oFileB.right(4).toLower().compare( QString("none") ) == 0) )

    {

#ifdef PHANTOM_PATCH
	if (   (m_pScene->getForceTool() != NULL)
	    && (strcmp(m_pScene->getForceTool()->getDevice()->getName(),"mouse") != 0) )
	{
	    svtout << "Stopping scheduler" << endl;
	    hdStopScheduler();
	}
#endif

        // pdb file
        pdb_document* sd = new pdb_document(m_pScene->getDataTopNode(), QtoStd(oFileA), SVT_PDB);

	// add it to the document lists
        if ( oDisplayName.isEmpty() )
            iNewDoc = addDocSimple( (sculptor_document*)sd );
        else
            iNewDoc = addDocSimple( (sculptor_document*)sd, oDisplayName );

#ifdef PHANTOM_PATCH
	if (   (m_pScene->getForceTool() != NULL)
	    && (strcmp(m_pScene->getForceTool()->getDevice()->getName(),"mouse") != 0) )
	{
	    svtout << "Starting scheduler" << endl;
	    hdStartScheduler();
	}
#endif
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Eliquos exhaustive search output
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if ( (oFileA.right(3).toLower().compare( QString("eli") ) == 0) )
    {
	loadEliquosDocs(oFileA);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // SCL
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if (   (oFileA.right(3).toLower().compare( QString("scl") ) == 0)
        && (oFileB.right(4).toLower().compare( QString("none") ) == 0))
    {
    	// Clean up old document list

	// Clear some global vars
	m_pScene->setMovedNode(NULL);
	disableInput();
	m_pTargetDoc = NULL;
	m_pProbeDoc = NULL;
	if (m_pForceAlgo != NULL)
	{
	    m_pForceAlgo->resetTarget();
	    m_pForceAlgo->resetProbe();
	}
        
        //don't show arrows and deselect the arrows menu item
        if (m_pCVDisplacementObj)
            m_pCVDisplacementObj->setVisible(false);
	m_pRenderDisplacementArrows->setChecked( FALSE );
        
	// Remove all documents except the global scene
	while(m_pDocumentList->count() > 1)
	{
	    window_documentlistitem* pDLI = (*m_pDocumentList)[1];

	    // now remove the node from the scene
	    if ( pDLI->getDoc()->getNode() != NULL )
	    {
		svt_node* pNode = pDLI->getDoc()->getNode();

		if ( pDLI->getDoc()->getType() == SENSITUS_DOC_CLIP)
		    m_pScene->delClipNode( pNode );
		else
		    m_pScene->delDataNode( pNode );
	    }

	    // delete the document from memory
	    sculptor_document* pSD = pDLI->getDoc();

	    // remove document out of the listbox
	    m_pDocumentList->removeItem(1);

	    // and delete from memory
	    if (pSD != NULL)
		delete pSD;
	}

        // Load scl file
        TiXmlDocument dat;
        if ( dat.LoadFile(QtoStd(oFileA).c_str()) )
        {
            // while loading the scene file, the document list should be disabled, otherwise the
            // user can manipulate documents, move them, etc, while they are being loaded.
            m_pDocumentList->setDisabled( TRUE );

            //dat.RootElement() == SCULPTOR
            //dat.RootElement()->FirstChild() == SCENE
            TiXmlElement* pSceneElem = (TiXmlElement*) dat.RootElement()->FirstChild();

            if (pSceneElem)
            {
                int i; // also used as bool
//                 if (pSceneElem->Attribute("WIDTH", &i)) // works only with one normal display windows/canvas
//                     svt_setDisplayWidth(0, i);
//                 if (pSceneElem->Attribute("HEIGHT", &i))
//                     svt_setDisplayHeight(0, i);
                
                int iIdxDocCvDispTail = -1;
                int iIdxDocCvDispHead = -1;
                int iCreateCvDisp;
                

                if (pSceneElem->Attribute( "PIPETTE", &i ) )
		{
                    m_pScene->setPipetteVisible( (bool)i );
		    m_pPipette->setChecked( (bool)i );
		}

                if (pSceneElem->Attribute( "COORDSYSTEM", &i ) )
		{
		    m_pScene->setCoordinateSystemVisible( (bool)i );
		    m_pCoordinateSystem->setChecked( (bool)i );
		}
		
		if (pSceneElem->Attribute( "TEXTFIELD_COLLAPSED", &i ) )
		    if (getMainWidgetArea() != NULL)
		    {
			if (i==1)
			    ((sculptor_splitter *) getMainWidgetArea())->setCollapsed();
			else
			    ((sculptor_splitter *) getMainWidgetArea())->setCollapsed(false);
		    }


                int rCol, gCol, bCol;
                if (pSceneElem->Attribute("BGCOLOR"))
		    if (sscanf(pSceneElem->Attribute("BGCOLOR"), "#%2X%2X%2X", &rCol, &gCol, &bCol) == 3)
			svt_setBackgroundColor(rCol/255., gCol/255., bCol/255.);

                double d;
                if (pSceneElem->Attribute("SCENESCALE", &d))
                    m_pScene->getSceneGraph()->setScale(d);


		// restore the state of the ambient occlusion parameters
                if (pSceneElem->Attribute( "AMBIENT_OCCLUSION_ENABLED", &i ))
                    m_pScene->setAOEnabled( (bool)i );

                if (pSceneElem->Attribute( "AMBIENT_OCCLUSION_RADIUS", &d ))
		    m_pScene->setAOSampleRadiusScaling((Real32)d);

                if (pSceneElem->Attribute( "AMBIENT_OCCLUSION_INTENSITY", &d ))
		    m_pScene->setAOIntensity((Real32)d);

                if (pSceneElem->Attribute( "AMBIENT_OCCLUSION_METHOD", &d ))
		    m_pScene->setAOMethod((Real32)d);


		// restore the state of the fog parameters
		GLfloat glf[4];
		float f[4];
		char str[256];
		//
                if (pSceneElem->Attribute( "FOG_ENABLED", &i ))
                    m_pScene->setFogEnabled( (bool)i );
		//
		
		if (pSceneElem->Attribute("FOG_MODE") != NULL)
		    if (sscanf(pSceneElem->Attribute("FOG_MODE"), "%s", str) == 1)
		    {
			if (     strcmp( str, "GL_LINEAR" ) == 0)
			    glf[0] = GL_LINEAR;
			else if (strcmp( str, "GL_EXP" ) == 0)
			    glf[0] = GL_EXP;
			else if (strcmp( str, "GL_EXP2" ) == 0)
			    glf[0] = GL_EXP2;
			m_pScene->setFogParam(GL_FOG_MODE, glf);
		    }
		//
		if (pSceneElem->Attribute( "GL_FOG_START" , &d ))
		{
		    glf[0] = (GLfloat)d;
		    m_pScene->setFogParam(GL_FOG_START, glf);
		}
                if (pSceneElem->Attribute( "GL_FOG_END", &d ))
		{
		    glf[0] = (GLfloat)d;
		    m_pScene->setFogParam(GL_FOG_END, glf);
		}
                if (pSceneElem->Attribute( "GL_FOG_DENSITY", &d ))
		{
		    glf[0] = (GLfloat)d;
		    m_pScene->setFogParam(GL_FOG_DENSITY, glf);
		}
		if (pSceneElem->Attribute("GL_FOG_COORD_SRC") != NULL)
		    if (sscanf(pSceneElem->Attribute("GL_FOG_COORD_SRC"), "%s", str ) == 1)
		    {
			if (strcmp( str, "GL_FRAGMENT_DEPTH" ) == 0)
			    glf[0] = (GLfloat)GL_FRAGMENT_DEPTH;
			else if (strcmp( str, "GL_FOG_COORD" ) == 0)
			    glf[0] = (GLfloat)GL_FOG_COORD;
			m_pScene->setFogParam(GL_FOG_COORD_SRC, glf);
		    }
		if (pSceneElem->Attribute("GL_FOG_COLOR") != NULL)
		    if (sscanf(pSceneElem->Attribute("GL_FOG_COLOR"), "%f %f %f %f", &f[0], &f[1], &f[2], &f[3] ) == 4)
		    {
			for (i=0; i<4; ++i)
			    glf[i] = (GLfloat)f[i];
			m_pScene->setFogParam(GL_FOG_COLOR, glf);
		    }


                int itemNr, iNewDoc;
                for ( TiXmlElement* pElem = (TiXmlElement*) pSceneElem->FirstChild(); pElem != 0; pElem = (TiXmlElement*) pElem->NextSibling()) 
                {
		    if (strcmp(pElem->Value(), "FORCEDIALOG"))
                    {
			itemNr = m_pDocumentList->count()-1;

                        if (strcmp(pElem->Value(), "CLIP_DOCUMENT") == 0)
                        {
                            iNewDoc = addClip();
			
			    // restore state of clipplane rectangle visibility
                            if ( pElem->Attribute( "RECTANGLE_VISIBLE", &i ) )
                            {
                                clip_document* pClipDoc = (clip_document*)m_pDocumentList->getDoc(iNewDoc);
                                pClipDoc->setRectVisible( (bool)i );
                            }

                        }
                        else if (strcmp(pElem->Value(), "MARKER_DOCUMENT") == 0)
                        {

                            iNewDoc = addMarker();
                            m_pDocumentList->getDoc(iNewDoc)->setState(pElem);

			}
                        else
                        {

                            QString oSRC_A( pElem->Attribute("SRC") );
                            QString oSRC_B( pElem->Attribute("SRCB") );
                            oSRC_A = QDir::convertSeparators( oSRC_A );
                            oSRC_B = QDir::convertSeparators( oSRC_B );
#ifndef WIN32
                            // if not under Windows, the backslashes need to be exchanged (the convertSeparators call above does not seem to be sufficient).
                            oSRC_A.replace("\\", "/");
                            oSRC_B.replace("\\", "/");

                            // if not under Windows, the drive letters need to be removed
                            if (oSRC_A[1] == ':')
                                oSRC_A = oSRC_A.right( oSRC_A.length() - 2 );
                            if (oSRC_B[1] == ':')
                                oSRC_B = oSRC_B.right( oSRC_B.length() - 2 );
#else
                            // if under Windows, the slashes need to be exchanged
                            oSRC_A.replace("/", "\\");
                            oSRC_B.replace("/", "\\");
#endif

                            // does the file indeed exist?
                            QFileInfo oFile_SRC_A( oSRC_A );
                            if (!oFile_SRC_A.exists())
                            {
                                // check in the same directory as the state file
                                QString oBaseName = oFile_SRC_A.fileName();
                                oFile_SRC_A.setFile( oCurrDir + QDir::separator() + oBaseName );

                                // if not OK, it might be a Windows state file under Linux, so the uppercase versions should also be tried...
                                if (!oFile_SRC_A.exists())
                                {
                                    oFile_SRC_A.setFile( oSRC_A );
                                    QString oBaseName = oFile_SRC_A.baseName();
                                    QString oBaseUp = oFile_SRC_A.baseName().toUpper();
                                    QString oBaseDown = oFile_SRC_A.baseName().toLower();
                                    QString oExtension = oFile_SRC_A.completeSuffix();
                                    QString oOldDir = oFile_SRC_A.absolutePath();

                                    // old dir + Filename uppercase
                                    oFile_SRC_A.setFile( oOldDir  + QDir::separator() + oBaseUp + '.' + oExtension );
                                    // old dir + Filename lowercase
                                    if (!oFile_SRC_A.exists())
                                        oFile_SRC_A.setFile( oOldDir  + QDir::separator() + oBaseDown + '.' + oExtension );
                                    // new dir + Filename uppercase
                                    if (!oFile_SRC_A.exists())
                                        oFile_SRC_A.setFile( oCurrDir + QDir::separator() + oBaseUp + '.' + oExtension );
                                    // new dir + Filename lowercase
                                    if (!oFile_SRC_A.exists())
                                        oFile_SRC_A.setFile( oCurrDir + QDir::separator() + oBaseDown + '.' + oExtension );
                                }

                                // if ok, update file information
                                if (oFile_SRC_A.exists())
                                    oSRC_A = oFile_SRC_A.filePath();
                                // if not, prompt user...
                                if (!oFile_SRC_A.exists())
                                {
                                    QString oFilterOLD( QString("File not found - please locate ( ") + oBaseName +QString(" )") );
                                    QStringList oFilterList;
                                    oFilterList << m_oFilterALL;
                                    oFilterList << oFilterOLD;

                                    // open file chooser dialog
                                    QFileDialog oFD( this, oFilterOLD );
                                    oFD.setNameFilters( oFilterList );
                                    oFD.selectNameFilter( oFilterOLD );
                                    oFD.setFileMode( QFileDialog::ExistingFiles );
                                    oFD.setDirectory( oCurrDir );

                                    if ( oFD.exec() == QDialog::Accepted )
                                    {
					QStringList oFiles = oFD.selectedFiles();
					if (!oFiles.isEmpty())
					    oSRC_A = oFiles[0];

                                        oFile_SRC_A.setFile( oSRC_A );
                                        oCurrDir = oFile_SRC_A.absolutePath();
                                    }
                                }

                                // OK, if we now finally have a new path, we have to set the current directory to that path so that the eliquos files can get loaded properly
                                m_oCurrentDir = oCurrDir;
                            }

                            if ( pElem->Attribute("SRCB") != NULL && strlen( pElem->Attribute("SRCB") ) > 0 )
                            {
                                // does the second file indeed exist?
                                QFileInfo oFile_SRC_B( oSRC_B );
                                if (!oFile_SRC_B.exists())
                                {
                                    QString oBaseName = oFile_SRC_B.fileName();
                                    oFile_SRC_B.setFile( oCurrDir + QDir::separator() + oBaseName );

                                    // if ok, update file information
                                    if (oFile_SRC_B.exists())
                                        oSRC_B = oFile_SRC_B.filePath();

                                    // if not, prompt user...
                                    if (!oFile_SRC_B.exists())
                                    {
                                        QString oFilterOLD( QString("File not found - please locate ( ") + oBaseName +QString(" )") );
                                        QStringList oFilterList;
                                        oFilterList << m_oFilterALL;
                                        oFilterList << oFilterOLD;

                                        // open file chooser dialog
                                        QFileDialog oFD( this, oFilterOLD );
                                      //  oFD.setCaption( oFilterOLD );
                                        oFD.setNameFilters( oFilterList );
                                        oFD.selectNameFilter( oFilterOLD );
                                        oFD.setFileMode( QFileDialog::ExistingFiles );
                                        oFD.setDirectory( oCurrDir );

                                        if ( oFD.exec() == QDialog::Accepted )
                                        {
                                           // oSRC_B = oFD.selectedFile();
                                            QStringList oFiles = oFD.selectedFiles();
					    if (!oFiles.isEmpty())
					        oSRC_B = oFiles[0];

					    oFile_SRC_B.setFile( oSRC_B );
                                            oCurrDir = oFile_SRC_B.absolutePath();
                                        }
                                    }

                                    // OK, if we now finally have a new path, we have to set the current directory to that path so that the eliquos files can get loaded properly
                                    m_oCurrentDir = oCurrDir;
                                }

                                loadDoc( oSRC_A, oSRC_B, QString(pElem->Attribute("DISPLAYNAME")));

                            }
                            else
                            {
                                loadDoc( oSRC_A, QString("NONE"), QString(pElem->Attribute("DISPLAYNAME")));
                            }
                        }

			if (itemNr != m_pDocumentList->count()-1) // load was ok
			{
                            if (pElem->Attribute("CVDISPSCR"))
                            {
                                iCreateCvDisp = atoi(pElem->Attribute("CVDISPSCR"));
                                if ( iCreateCvDisp )
                                {
                                    if ( iCreateCvDisp == SCULPTOR_CVDISP_TAIL)
                                    {
                                        iIdxDocCvDispTail = itemNr;

                                    } else {

                                        if ( iCreateCvDisp == SCULPTOR_CVDISP_HEAD)
                                            iIdxDocCvDispHead = itemNr;
                                    }
                                }
                            }
                            itemNr = m_pDocumentList->count()-1;
                            
			    if (pElem->Attribute("VISIBLE", &i))
			    {
				(*m_pDocumentList)[itemNr]->setVisible((bool) i);
			    }
                            m_pDocumentList->getDoc(itemNr)->setState(pElem);
			    m_pRenderFeatureVectors->setChecked( m_pDocumentList->getDoc(itemNr)->getCV_Visible() );
			}
		    }
                }

                if (pSceneElem->Attribute("TRANSFORM"))
                    getScene()->getTopNode()->getTransformation()->setFromString(pSceneElem->Attribute("TRANSFORM"));

                if (pSceneElem->Attribute("TRANSFORMSG"))
                    getScene()->getSceneGraph()->getTransformation()->setFromString(pSceneElem->Attribute("TRANSFORMSG"));

                if (pSceneElem->Attribute( "HIGHCONTRAST", &i ) )
                    m_bHighContrast = !(bool)i;
                toggleHighContrast();

                if (pSceneElem->Attribute( "CAPPEDCLIP", &i ) )
                {
                    m_pScene->setCappedClip( (bool)i );
		            m_pCappedClip->setChecked( m_pScene->getCappedClip() );
                }

                if (pSceneElem->Attribute( "SHOWCVDISP", &i ) )
                {
                 if (i==1)//draw arrows 
                     if (iIdxDocCvDispTail!=-1 && iIdxDocCvDispHead!=-1)
			 createCVDisplacementObj( *m_pDocumentList->getDoc(iIdxDocCvDispTail+1), *m_pDocumentList->getDoc(iIdxDocCvDispHead+1) );
                }

		// restore probe-solution associations (via DocID)
		for (int iProbeDoc=1; iProbeDoc < m_pDocumentList->count(); iProbeDoc++)
		{
		    sculptor_document* pProbeDoc = m_pDocumentList->getDoc(iProbeDoc);
		    if (pProbeDoc->getEliDataLoaded())
		    // We found a probe with exh. search data loaded
		    {
			// Now find all documents which are copies of this probe
			// and add a pointer to the corresponding original probe doc
		        for (int iCopyDoc=1; iCopyDoc < m_pDocumentList->count(); iCopyDoc++)
			{
			    sculptor_document* pCopyDoc = m_pDocumentList->getDoc(iCopyDoc);
			    if ( pCopyDoc->getCopy() && 
				(pCopyDoc->getDocID() == pProbeDoc->getDocID()))
			    {
			        pCopyDoc->setOrigProbeDoc(pProbeDoc);
			    }
			}
		    }
                }

		// restore the state of automatically adjusting probe/solution colors
                if (pSceneElem->Attribute( "AUTOADJUSTCOLORS", &i ) )
		    m_bAutoAdjustProbeSolutionColor = (bool) i;
		m_pAutoColor->setChecked( (bool)i );

                // restore the probe and target doc information
                if (pSceneElem->Attribute( "TARGETDOC", &i ) )
                    if (i!=-1) setTargetDoc( i );
                if (pSceneElem->Attribute( "PROBEDOC", &i ) )
                    if (i!=-1) setDockDoc( i );

		// restore force dialog
		TiXmlElement* pChild;
		if (((pChild = (TiXmlElement*) pSceneElem->FirstChild("FORCEDIALOG")) != NULL) && 
		    (m_pScene->getForceTool() != NULL))
		{
		    double d;
		    int i;
		    if (pChild->Attribute("BUTTONLOGIC", &i)){
			m_pScene->getForceTool()->setButtonLogic((bool) i);
		    }
		    if (pChild->Attribute("FORCEARROW", &i)){
			m_pScene->getForceAlgo()->setForceArrowVisible((bool) i);
		    }
		    if (pChild->Attribute("FORCETRANSLATIONSCALE", &d)){
			m_pScene->setForceTranslationScale(d);
		    }
		    if (pChild->Attribute("CORRFORCESCALE", &d)){
			m_pScene->getForceAlgo()->setCorrForceScale(d);
		    }
		    if (pChild->Attribute("STERICFORCESCALE", &d)){
			m_pScene->getForceAlgo()->setStericForceScale(d);
		    }
		    if (pChild->Attribute("CENTRALSPHERESCALE", &d)){
			m_pScene->getForceAlgo()->setCentralSphereScale(d);
		    }
		    if (pChild->Attribute("FORCEARROWSCALE", &d)){
			m_pScene->getForceAlgo()->setForceArrowScale(d);
		    }
		    if (pChild->Attribute("STERICCLASHTHRESHOLD", &d)){
			m_pScene->getForceAlgo()->setStericClashThreshold(d);
		    }
		    if (pChild->Attribute("STERICGOODTHRESHOLD", &d)){
			m_pScene->getForceAlgo()->setStericGoodThreshold(d);
		    }
		    if (pChild->Attribute("SPHERECOLORFUNCTIONRED", &d)){
			m_pScene->getForceAlgo()->setSphereColorFunctionRed(d);
		    }
		    if (pChild->Attribute("SPHERECOLORFUNCTIONBLUE", &d)){
			m_pScene->getForceAlgo()->setSphereColorFunctionBlue(d);
		    }
		    if (pChild->Attribute("SPHERECOLORFUNCTIONGREEN", &d)){
			m_pScene->getForceAlgo()->setSphereColorFunctionGreen(d);
		    }
		    if (pChild->Attribute("FORCETRANSX", &i)){
			m_pScene->getForceAlgo()->setForceTransX((bool) i);
		    }
		    if (pChild->Attribute("FORCETRANSY", &i)){
			m_pScene->getForceAlgo()->setForceTransY((bool) i);
		    }
		    if (pChild->Attribute("FORCETRANSZ", &i)){
			m_pScene->getForceAlgo()->setForceTransZ((bool) i);
		    }
		    if (pChild->Attribute("FORCEROTX", &i)){
			m_pScene->getForceAlgo()->setForceRotX((bool) i);
		    }
		    if (pChild->Attribute("FORCEROTY", &i)){
			m_pScene->getForceAlgo()->setForceRotY((bool) i);
		    }
		    if (pChild->Attribute("FORCEROTZ", &i)){
			m_pScene->getForceAlgo()->setForceRotZ((bool) i);
		    }
		}

                // restore force flags
                if (pSceneElem->Attribute( "FORCEON", &i ))
		{
		    // This is now only relevant if we have an actual haptic device
		    // Since the default (w/o a haptic device) is automatic thread
		    // starting, I don't think we need to restore this...
		}
                if (pSceneElem->Attribute( "FORCEDIALOGSHOWN", &i ))
		{
		    if ((bool)i)
			showForceDlg();
		}

		// mark steric clashing data dirty
		setSolutionSetChanged(true);

                // restore steric clashing flag
                if (pSceneElem->Attribute( "STERICCLASHINGON", &i ))
		    setStericClashing( (bool)i );

		enableInput();
            }
            else
                svtout << "ERROR: Missing Scene in Sculptor Project File " <<  QtoStd(oFileA) << endl;

            // re-enable the file form...
            m_pDocumentList->setDisabled(FALSE);
        }
        else
            svtout << "ERROR: Failed to load Sculptor Project " <<  QtoStd(oFileA) << endl;

        setMovedDoc( 0 );

    }
    else
    {
        // adjust origin
        if (m_pDocumentList->count() == 2)
            adjustOrigin();
    }

    // adjust scaling of mouse wheel
    svt_vector4<Real32> oMaxVec;
    for (int i=1; i<m_pDocumentList->count(); i++)
        if (oMaxVec.length() < m_pDocumentList->getDoc(i)->getBoundingBox().length())
            oMaxVec = m_pDocumentList->getDoc(i)->getBoundingBox();

    m_pScene->setMouseWheelScale( oMaxVec.length() * 0.1 );

    // statusbar message
    QString s;
    s += "Loaded document ";
    s += oFileA;
    statusBar()->showMessage(s, 2000);

    // get path information and set the current directory to this path
    QFileInfo oFileInf( oFileA );
    m_oCurrentDir = oFileInf.absoluteDir();
    if (oDisplayName.isEmpty())
        oDisplayName = oFileInf.fileName();

    // update the recently loaded files list
    if (!oDisplayName.isEmpty())
        m_aRecentDN.insert( m_aRecentDN.begin(), QtoStd( oDisplayName ) );
    else
        m_aRecentDN.insert( m_aRecentDN.begin(), string( "" ) );

    if (!oFileA.isEmpty())
        m_aRecentFileA.insert( m_aRecentFileA.begin(), QtoStd( oFileA ) );
    else
        m_aRecentFileA.insert( m_aRecentFileA.begin(), string( "" ) );

    if (!oFileB.isEmpty())
        m_aRecentFileB.insert( m_aRecentFileB.begin(), QtoStd( oFileB ) );
    else
        m_aRecentFileB.insert( m_aRecentFileB.begin(), string( "" ) );

    saveRecentList();
    updateRecentList();

    setMovedDoc(0);

    return iNewDoc;
}

/**
 * load Eliquos output files
 */
void sculptor_window::loadEliquosDocs(QString oFileA)
{
    // Eliquos description file
    FILE  *pEliquos_file = fopen(QtoStd(oFileA).c_str(), "r");
    if( pEliquos_file == NULL )
    {
	return;
	// statusbar message
	QString s;
	s += "Error: Failed to read Eliquos description file!";
	s += oFileA;
	statusBar()->showMessage( s, 10000 );
	svtout << "Error: File " << QtoStd(oFileA) << " does not exist!" << endl;
	return;
    }

    svtout << "Opened Eliquos description file " << QtoStd(oFileA) << endl;

    // Read Eliquos file
    char cFilename_probe[256];
    char cFilename_target[256];
    char cFilename_euler_angles[256];
    char cFilename_translations[256];
    char cFilename_rotations[256];
    char cDummy[256];
    int count = 0;
    count += fscanf(pEliquos_file,"%s\n", cFilename_probe);
    count += fscanf(pEliquos_file,"%s\n", cFilename_target);
    count += fscanf(pEliquos_file,"%s\n", cFilename_euler_angles);
    count += fscanf(pEliquos_file,"%s\n", cFilename_translations);
    count += fscanf(pEliquos_file,"%s\n", cFilename_rotations);
    count += fscanf(pEliquos_file,"%s", cDummy);
    count += fscanf(pEliquos_file,"%s", cDummy);
    count += fscanf(pEliquos_file,"%s", cDummy);
    count += fscanf(pEliquos_file,"%s", cDummy);
    count += fscanf(pEliquos_file,"%s", cDummy);
    count += fscanf(pEliquos_file,"%s", cDummy);
    count += fscanf(pEliquos_file,"%s", cDummy);
    count += fscanf(pEliquos_file,"%s", cDummy);

    if (count != 13)
    {
	QMessageBox::warning(this,
                             QString("Loading Exhaustive Search Data"),
                             QString("File is not a valid exhaustive search output file"));
        //statusBar()->showMessage( "Exhaustive search output file corrupted", 2000 );
	return;
    }

    // Print values we just read
    svtout << "Contents of exhausive search description file:" << endl;
    svtout << endl;
    svtout << "  Probe file name           = " << cFilename_probe << endl;
    svtout << "  Target file name          = " << cFilename_target << endl;
    svtout << "  Euler angle file name     = " << cFilename_euler_angles << endl;
    svtout << "  Translation map file name = " << cFilename_translations << endl;
    svtout << "  Rotation map file name    = " << cFilename_rotations << endl;
    svtout << endl;

    // Load top candidates
    count = 0;
    Real64 fCandidateX, fCandidateY, fCandidateZ;
    Real64 fCandidatePhi, fCandidateTheta, fCandidatePsi;
    vector< svt_matrix4<Real64> > oCandidateMatrices;
    svt_vector4<Real64> oVecTransl;
    svt_matrix4<Real64> oMatrix;
    while ( fscanf(pEliquos_file,"%s %lf %lf %lf %lf %lf %lf\n", 
	                cDummy,
	                &fCandidateX, &fCandidateY, &fCandidateZ,
	                &fCandidatePhi, &fCandidateTheta, &fCandidatePsi)
	     != EOF)
    {
        if (strcmp(cDummy, "CANDIDATE") == 0)
	{
	    oVecTransl.set(fCandidateX, fCandidateY, fCandidateZ);
	    oVecTransl *= 1.0E-2;

	    oMatrix.loadIdentity();
	    oMatrix.rotatePTP(deg2rad(fCandidatePsi), deg2rad(fCandidateTheta), deg2rad(fCandidatePhi));
	    oMatrix.translate( oVecTransl );

	    oCandidateMatrices.push_back(oMatrix);

	    count++;
	}
    }

    if (count > 0)
      svtout << "Loaded " << oCandidateMatrices.size() << " candidate solutions" << endl;

    // Close Eliquos file
    fclose(pEliquos_file);
    svtout << "Closed sculptor.eli" << endl;

    // Determine fully qualified filenames of Eliquos files
    QFileInfo oDirInf( oFileA );
    QString oPath = oDirInf.absolutePath() + QDir::separator();

    QString sProbeFile       = oPath + QString(cFilename_probe);
    QString sTargetFile      = oPath + QString(cFilename_target);
    QString sEulerFile       = oPath + QString(cFilename_euler_angles);
    QString sTranslationFile = oPath + QString(cFilename_translations);
    QString sRotationFile    = oPath + QString(cFilename_rotations);

    // Load target document
    // Bail if unsuccessful
    int iOrigNDocs = m_pDocumentList->count();
    loadDoc(sTargetFile, QString("NONE"));
    if (m_pDocumentList->count() == iOrigNDocs)
    {
	QMessageBox::warning(this,
                             QString("Loading Target Volume"),
                             QString("Target volume file " + sProbeFile + " is missing or empty"));
        statusBar()->showMessage( "Could not load target volume.", 2000 );
	return;
    }

    // Determine a document ID used for the whole Eliquos data set
    unsigned long iDocID = svt_getToD();

    // Make it the target document and set the DocID
    // (this works because we just added it to the end of the list ;-)
    setTargetDoc(m_pDocumentList->count()-1);
    m_pDocumentList->getDoc(m_pDocumentList->count()-1)->setDocID(iDocID);

    // Load probe document manually since we need to attach more data to it
    
    pdb_document* sd = new pdb_document(m_pScene->getDataTopNode(), QtoStd(sProbeFile), SVT_PDB);

    // Bail if unsuccessful
    if (((svt_pdb *) sd->getNode())->size() == 0)
    {
        // get rid of the pdb document we just created
	m_pScene->delDataNode( sd->getNode() );
	delete sd;

        // get rid of the map we just loaded
	delDoc(m_pDocumentList->count() - 1);

	QMessageBox::warning(this,
                             QString("Loadind Probe PDB"),
                             QString("Probe PDB file " + sProbeFile + " is missing or empty"));
        statusBar()->showMessage( "Could not load probe structure.", 2000 );
	return;
    }

    // Set the document ID
    sd->setDocID(iDocID);

    // Make it the original probe
    sd->setOrigProbeDoc(sd);

    // Store Eliquos data file names
    sd->setEliCrossCorrFile(sTranslationFile);
    sd->setEliAngleIndexFile(sRotationFile);
    sd->setEliAngleListFile(sEulerFile);

    // Now we read in the actual exhaustive search data
    if (!sd->loadEliCrossCorr())
    {
	m_pScene->delDataNode( sd->getNode() );
	delete sd;
	delDoc(m_pDocumentList->count() - 1);
	return;
    }
    if (!sd->loadEliAngleIndex())
    {
	m_pScene->delDataNode( sd->getNode() );
	delete sd;
	delDoc(m_pDocumentList->count() - 1);
	return;
    }
    if (!sd->loadEliAngleList())
    {
	m_pScene->delDataNode( sd->getNode() );
	delete sd;
	delDoc(m_pDocumentList->count() - 1);
	return;
    }

    // Mark it as a copy, so that switching between copies works
    sd->setCopy(true);
    
    // Set flags
    sd->setEliDataLoaded(true);
    sd->setIGDActive(true);
    
    // document position
    svt_vector4f oDocOrigin = sd->getRealPos();
    oDocOrigin.multScalar( 1.0E8f );
    sd->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
    sd->storeInitMat();
    // add it to the document lists
    int iNewDoc = addDocSimple( (sculptor_document*)sd );
    setDockDoc( iNewDoc );

    // Add candidate solutions to transformation list
    sd->removeAllTransforms();
    svt_vector4<Real64> oVecDocCenter = sd->getCenter();
    oVecDocCenter *= 1.0E-2;
    for (unsigned int i=0; i < oCandidateMatrices.size(); i++)
    {
	sprintf( cDummy, "Solution Candidate %d", i+1);
	sd->addTransform( cDummy, oCandidateMatrices[i].translate(oVecDocCenter));
    }

    sd->updateTransList();

    // statusbar message
    QString s;
    s += "Loaded Eliquos files ";
    s += oFileA;
    statusBar()->showMessage(s, 2000);
}

/**
 * showFetchpdbDlg displays a dialog box for the user to input a PDBID and choose whether a biological unit should be downloaded.  
 */ 
void sculptor_window::showFetchpdbDlg()
{
    // create new dialog if NULL, otherwise clear entries, then show
    if (m_pFetchpdbDlg == NULL)
    {
        m_pFetchpdbDlg = new dlg_fetchpdb(this);
    }
    else
    {
	m_pFetchpdbDlg->clearSelections();
    }

    m_pFetchpdbDlg->show();
}

/**
 * query for a specific PDB, calls fetchPDB in svt_point_cloud_pdb to perform fetchPDB
 * \param oPDB QString object with the PDB + extension
 */
void sculptor_window::queryPDB(QString oPDB)
{
    svt_point_cloud_pdb< svt_vector4<Real64> >* pPdb = new svt_point_cloud_pdb< svt_vector4<Real64> >();

    // if fetching the pdb not successful, return
    if ( !pPdb->fetchPDB(QtoStd(oPDB).c_str()) )
    {
        guiWarning("PDB fetch error", "PDB fetch error", "PDB fetch error");
        delete pPdb;
        return;
    }

    if (pPdb->size() > 0)
    {
        pdb_document* sd = new pdb_document(m_pScene->getDataTopNode(), QtoStd(oPDB), SVT_PDB, "", SVT_NONE, pPdb);
				
	//set changed
	sd->setChanged();

	// add it to the document lists
	addDoc( (sculptor_document*)sd );

        // statusbar message
	QString s;
	s += "Fetched document ";
	s += oPDB;
	statusBar()->showMessage(s, 2000);

	setDefaultProbeTargetDoc();
    }	
    else
    {
        guiWarning("PDB fetch error", QtoStd((oPDB + " is empty.")), "PDB fetch error");
    }

    //adjust origin
    if (m_pDocumentList->count() == 2)
        adjustOrigin();

    // adjust scaling of mouse wheel
    svt_vector4<Real32> oMaxVec;
    for (int i=1; i<m_pDocumentList->count(); i++)
        if (oMaxVec.length() < m_pDocumentList->getDoc(i)->getBoundingBox().length())
            oMaxVec = m_pDocumentList->getDoc(i)->getBoundingBox();

    m_pScene->setMouseWheelScale( oMaxVec.length() * 0.1 );
}

/**
 * load a document
 */
void sculptor_window::load()
{
    int iNewDoc = -1;

    QStringList oFilterList;
    oFilterList << m_oFilterSCL;
    oFilterList << m_oFilterSAR;
    oFilterList << m_oFilterLUA;
    oFilterList << m_oFilterPDB;
    oFilterList << m_oFilterPSF_PDB;
    oFilterList << m_oFilterSITUS;
    oFilterList << m_oFilterPSF_DCD;
    oFilterList << m_oFilterELI;
    oFilterList << m_oFilterALL;
 
    QFileDialog oFD( this );
    oFD.setNameFilters( oFilterList );
    oFD.setAcceptMode( QFileDialog::AcceptOpen );
    oFD.setFileMode( QFileDialog::ExistingFiles );
    oFD.selectFilter( m_oFilterALL );
    oFD.setDirectory( m_oCurrentDir );

    QString oFileA, oFileB;

    if ( oFD.exec() == QDialog::Accepted )
    {
        QStringList oList = oFD.selectedFiles();
        
        if ( oFD.selectedNameFilter() == m_oFilterPSF_PDB )
        {
            if (oList.size() == 2)
            {
                oFileA = oList[0];
                oFileB = oList[1];

                if (oFileA.right(4).contains( QString("pdb"), Qt::CaseInsensitive ) > 0)
		    iNewDoc = loadDoc( oFileA, oFileB );
		else
		    iNewDoc = loadDoc( oFileB, oFileA );
            }
            else
                QMessageBox::warning(this,
                                     QString("Load PDB/PSF Document"),
                                     QString("Please select both pdb and psf files (by holding the Ctrl key and clicking on them) before clicking on OK."));

        }
        else if ( oFD.selectedNameFilter() == m_oFilterPSF_DCD )
        {
            if (oList.size() == 2)
            {
                oFileA = oList[0];
                oFileB = oList[1];

                if ( oFileA.endsWith(".psf") || oFileA.endsWith(".PSF") )
                    iNewDoc = loadDoc( oFileA, oFileB );
                else
                    iNewDoc = loadDoc( oFileB, oFileA );
            }
            else
                QMessageBox::warning(this,
                                     QString("Load PSF/DCD Document"),
                                     QString("Please select both psf and dcd files (by holding the Ctrl key and clicking on them) before clicking on OK.") );

        }
        else 
	{
	    // in all other cases there is no second file to load...
            for( int i=0; i<oList.size(); i++)
            {
#ifdef WIN32
                if (oList[i][1] != ':' )
#else
                if (oList[i][0] != QDir::separator() )
#endif
                    oList[i] = oFD.directory().path() + QDir::separator() + oList[i];

                iNewDoc = loadDoc( oList[i], QString("NONE") );
            }
        }
    }
    else
        statusBar()->showMessage( "Loading aborted", 2000 );

    m_oCurrentDir = oFD.directory();

    setDefaultProbeTargetDoc();

    // make the newly loaded document selected and current, and show its property dialog
    if (iNewDoc != -1)
    {
        m_pDocumentList->setCurrentItem(iNewDoc);
        m_pDocumentList->showPropDialog();
    }

    // scene becomes moved doc
    setMovedDoc(0);
}

/**
 * Save the current state
 * \param oFilename string with the filename
 * \param bWithoutPath without path, filenames are only local (necessary for the sar archives). Default: false
 */
void sculptor_window::saveSCL( string oFilename, bool bWithoutPath )
{
    TiXmlDocument dat;
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    dat.LinkEndChild(decl);

    TiXmlElement* main = new TiXmlElement("SCULPTOR");
    main->SetAttribute("VERSION", SCULPTOR_VERSION);
    dat.LinkEndChild(main);

    TiXmlElement* scene = new TiXmlElement("SCENE");
    main->LinkEndChild(scene);
    //scene->SetAttribute("WIDTH", svt_getDisplayWidth(0)); // only canvas 0
    //scene->SetAttribute("HEIGHT", svt_getDisplayHeight(0)); // only canvas 0
    svt_color* pColor = svt_getBackgroundColor();
    char str[256];
    sprintf(str, "#%02X%02X%02X", (int) (255.*pColor->getR()),
            (int) (255.*pColor->getG()),
            (int) (255.*pColor->getB()));
    scene->SetAttribute("BGCOLOR", str);
    
    scene->SetDoubleAttribute("SCENESCALE", m_pScene->getSceneGraph()->getScaleX());

    if (m_pCVDisplacementObj)
        scene->SetAttribute("SHOWCVDISP", m_pCVDisplacementObj->getVisible());
    scene->SetAttribute("PIPETTE", m_pScene->getPipetteVisible() );
    scene->SetAttribute("COORDSYSTEM", m_pScene->getCoordinateSystemVisible() );
    if (getMainWidgetArea() != NULL)
        scene->SetAttribute("TEXTFIELD_COLLAPSED",  (((sculptor_splitter *) getMainWidgetArea())->getCollapsed()));

    scene->SetAttribute("HIGHCONTRAST", m_bHighContrast );
    scene->SetAttribute("CAPPEDCLIP", m_pScene->getCappedClip() );

    // save the state of automatically adjusting probe/solution colors
    scene->SetAttribute("AUTOADJUSTCOLORS", m_bAutoAdjustProbeSolutionColor );

    // if ambient occlusion was not disabled via command line, save its state.
    // if it was disabled, do nothing here. this will preserve existing the existing setting
    if ( svt_getAllowAmbientOcclusion() )
    {
        // save the state of the ambient occlusion parameters
        if (m_pScene->getAOEnabled())
            scene->SetAttribute("AMBIENT_OCCLUSION_ENABLED", 1 );
        else
    	    scene->SetAttribute("AMBIENT_OCCLUSION_ENABLED", 0 );
    }
    //
    scene->SetDoubleAttribute( "AMBIENT_OCCLUSION_RADIUS", (double)(m_pScene->getAOSampleRadiusScaling()));
    //
    scene->SetDoubleAttribute( "AMBIENT_OCCLUSION_INTENSITY",(double)(m_pScene->getAOIntensity()));
    //
    scene->SetDoubleAttribute( "AMBIENT_OCCLUSION_METHOD",(double)(m_pScene->getAOMethod()));

    // save the state of the fog parameters
    GLfloat f[4];
    if (m_pScene->getFogEnabled())
	scene->SetAttribute("FOG_ENABLED", 1);
    else
	scene->SetAttribute("FOG_ENABLED", 0);
    //
    m_pScene->getFogParam(GL_FOG_MODE, f);
    if (f[0] == GL_LINEAR)
	scene->SetAttribute("FOG_MODE", "GL_LINEAR");
    else if (f[0] == GL_EXP)
	scene->SetAttribute("FOG_MODE", "GL_EXP");
    else if (f[0] == GL_EXP2)
	scene->SetAttribute("FOG_MODE", "GL_EXP2");
    //
    m_pScene->getFogParam(GL_FOG_START, f);
    scene->SetDoubleAttribute("GL_FOG_START", (double)f[0]);
    m_pScene->getFogParam(GL_FOG_END, f);
    scene->SetDoubleAttribute("GL_FOG_END", (double)f[0]);
    m_pScene->getFogParam(GL_FOG_DENSITY, f);
    scene->SetDoubleAttribute("GL_FOG_DENSITY", (double)f[0]);
    m_pScene->getFogParam(GL_FOG_COORD_SRC, f);
    if (f[0] == GL_FRAGMENT_DEPTH)
	scene->SetAttribute("GL_FOG_COORD_SRC", "GL_FRAGMENT_DEPTH");
    else if (f[0] == GL_FOG_COORD)
	scene->SetAttribute("GL_FOG_COORD_SRC", "GL_FOG_COORD");
    //
    m_pScene->getFogParam(GL_FOG_COLOR, f);
    sprintf(str, "%f %f %f %f", (float)f[0], (float)f[1], (float)f[2], (float)f[3]);
    scene->SetAttribute("GL_FOG_COLOR", str);

    // search for probe and target doc
    int iTargetDoc = -1;
    int iProbeDoc = -1;
    int i = 0;
    if (m_pProbeDoc != NULL)
        for(i=0;i<m_pDocumentList->count(); i++)
            if (m_pProbeDoc == m_pDocumentList->getDoc(i))
                iProbeDoc = i;
    if (m_pTargetDoc != NULL)
        for(i=0;i<m_pDocumentList->count(); i++)
            if (m_pTargetDoc == m_pDocumentList->getDoc(i))
                iTargetDoc = i;

    // now save information
    scene->SetAttribute("TARGETDOC", iTargetDoc );
    scene->SetAttribute("PROBEDOC", iProbeDoc );

    svt_matrix4f* pMat = getScene()->getTopNode()->getTransformation();
    sprintf(str, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
            pMat->getAt(0,0), pMat->getAt(0,1), pMat->getAt(0,2), pMat->getAt(0,3),
            pMat->getAt(1,0), pMat->getAt(1,1), pMat->getAt(1,2), pMat->getAt(1,3),
            pMat->getAt(2,0), pMat->getAt(2,1), pMat->getAt(2,2), pMat->getAt(2,3),
            pMat->getAt(3,0), pMat->getAt(3,1), pMat->getAt(3,2), pMat->getAt(3,3));
    scene->SetAttribute("TRANSFORM", str);

    pMat = getScene()->getSceneGraph()->getTransformation();
    sprintf(str, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
            pMat->getAt(0,0), pMat->getAt(0,1), pMat->getAt(0,2), pMat->getAt(0,3),
            pMat->getAt(1,0), pMat->getAt(1,1), pMat->getAt(1,2), pMat->getAt(1,3),
            pMat->getAt(2,0), pMat->getAt(2,1), pMat->getAt(2,2), pMat->getAt(2,3),
            pMat->getAt(3,0), pMat->getAt(3,1), pMat->getAt(3,2), pMat->getAt(3,3));
    scene->SetAttribute("TRANSFORMSG", str);

    // save state of force feedback and steric clashing
    scene->SetAttribute("FORCEON", isForceOn());
    scene->SetAttribute("STERICCLASHINGON", getStericClashing());
    if (m_pForceDlg != NULL)
        scene->SetAttribute("FORCEDIALOGSHOWN", m_pForceDlg->isVisible());
    else
        scene->SetAttribute("FORCEDIALOGSHOWN", false);

    // save state of force dialog
    if (getForceAlgo() != NULL)
    {
        TiXmlElement* pForceDlg = new TiXmlElement("FORCEDIALOG");
        pForceDlg->SetAttribute("BUTTONLOGIC", m_pScene->getForceTool()->getButtonLogic());
        pForceDlg->SetAttribute("FORCEARROW", getForceAlgo()->getForceArrowVisible());
        pForceDlg->SetDoubleAttribute("FORCETRANSLATIONSCALE", m_pScene->getForceTranslationScale());
        pForceDlg->SetDoubleAttribute("CORRFORCESCALE", getForceAlgo()->getCorrForceScale());
        pForceDlg->SetDoubleAttribute("STERICFORCESCALE", getForceAlgo()->getStericForceScale());
        pForceDlg->SetDoubleAttribute("CENTRALSPHERESCALE", getForceAlgo()->getCentralSphereScale());
        pForceDlg->SetDoubleAttribute("FORCEARROWSCALE", getForceAlgo()->getForceArrowScale());
        pForceDlg->SetDoubleAttribute("STERICCLASHTHRESHOLD", getForceAlgo()->getStericClashThreshold());
        pForceDlg->SetDoubleAttribute("STERICGOODTHRESHOLD", getForceAlgo()->getStericGoodThreshold());
        pForceDlg->SetDoubleAttribute("SPHERECOLORFUNCTIONRED", getForceAlgo()->getSphereColorFunctionRed());
        pForceDlg->SetDoubleAttribute("SPHERECOLORFUNCTIONBLUE", getForceAlgo()->getSphereColorFunctionBlue());
        pForceDlg->SetDoubleAttribute("SPHERECOLORFUNCTIONGREEN", getForceAlgo()->getSphereColorFunctionGreen());
        pForceDlg->SetAttribute("FORCETRANSX", getForceAlgo()->getForceTransX());
        pForceDlg->SetAttribute("FORCETRANSY", getForceAlgo()->getForceTransY());
        pForceDlg->SetAttribute("FORCETRANSZ", getForceAlgo()->getForceTransZ());
        pForceDlg->SetAttribute("FORCEROTX", getForceAlgo()->getForceRotX());
        pForceDlg->SetAttribute("FORCEROTY", getForceAlgo()->getForceRotY());
        pForceDlg->SetAttribute("FORCEROTZ", getForceAlgo()->getForceRotZ());
        scene->LinkEndChild(pForceDlg);
    }

    // save the documents
    TiXmlElement* doc = NULL;
    bool isSaveable;
    for(i=1; i < m_pDocumentList->count(); i++)
    {
        isSaveable = true;
        switch (m_pDocumentList->getDocType(i))
        {
        case SENSITUS_DOC_PDB:
            doc = new TiXmlElement("PDB_DOCUMENT");
            break;
        case SENSITUS_DOC_SITUS:
            doc = new TiXmlElement("SITUS_DOCUMENT");
            break;
        case SENSITUS_DOC_CLIP:
            doc = new TiXmlElement("CLIP_DOCUMENT");
	    doc->SetAttribute("RECTANGLE_VISIBLE", ((clip_document*)(m_pDocumentList->getDoc(i)))->getRectVisible());
            break;
        case SENSITUS_DOC_MARKER:
            doc = new TiXmlElement("MARKER_DOCUMENT");
            break;
        default:
            isSaveable = false;
            svtout << "WARNING: " << m_pDocumentList->getDoc(i)->getFileName() << " has not been saved!" << endl;
        }
        if (isSaveable)
        {
            doc->SetAttribute("VISIBLE", m_pDocumentList->getDoc(i)->getVisible());
            if (bWithoutPath)
                doc->SetAttribute("SRC", m_pDocumentList->getDoc(i)->getDisplayName());
            else
                doc->SetAttribute("SRC", m_pDocumentList->getDoc(i)->getFileName()); // check abs,rel and where .scl is stored
            
            string oFName_SRCB = m_pDocumentList->getDoc(i)->getFileName_SRCB();
            if (oFName_SRCB.compare( oFName_SRCB.size()-4,4,"NONE")!=0)
                doc->SetAttribute("SRCB", oFName_SRCB);
            else
                doc->SetAttribute("SRCB", "");

            doc->SetAttribute("DISPLAYNAME", m_pDocumentList->getDoc(i)->getDisplayName());
            doc->SetAttribute("CVDISPSCR", m_pDocumentList->getDoc(i)->getCVDispSrc());

            m_pDocumentList->getDoc(i)->getState(doc);

            scene->LinkEndChild(doc);

        }
    }

    dat.SaveFile(oFilename.c_str());
};

/**
 * save a document
 */
void sculptor_window::save()
{
    unsigned int iSolCount = 0;

    for( int iDoc=0; iDoc<m_pDocumentList->count(); iDoc++ )
    {
        // if the document is a solution, then increase counter
        if (m_pDocumentList->getDoc(iDoc)->getSolution())
            iSolCount++;

        if (m_pDocumentList->getSelected(iDoc))
        {
            window_documentlistitem* pDli = (*m_pDocumentList)[iDoc];
 
            if ( pDli->getDoc() != NULL )
            {
                //
                // Sculptor scene selected, save scl file
                //
                if (iDoc == 0)
                {
                    // is there a document that is not saved yet
                    bool bNotSaved = false;
                    for(int i=1; i < m_pDocumentList->count(); i++)
                        if (m_pDocumentList->getDoc(i)->getChanged())
                            bNotSaved = true;
                    if (bNotSaved)
                    {
                        svt_exception::ui()->info("One or more documents were altered and need to be saved, before the state can be written to disk!");
                        return;
                    }

                    // open file chooser dialog
                    QFileDialog oFD( this );
                    oFD.setNameFilter( m_oFilterSCL );
                    oFD.setAcceptMode( QFileDialog::AcceptSave );

                    oFD.setDirectory( m_oCurrentDir );
                    oFD.setFileMode( QFileDialog::AnyFile );

                    if ( oFD.exec() == QDialog::Accepted )
                    {
                        m_oCurrentDir = oFD.directory();
                        QStringList oFiles = oFD.selectedFiles();
                    
                        if (oFiles.size()!=1)
                        {
                            svt_exception::ui()->info("Please choose the file name!");
                            return;
                        }
                        QString oFile = oFiles.at(0);
                        oFile = QDir::convertSeparators( oFile );

                        // does the new filename have an extension?
                        if (oFile.endsWith( QString(".scl"), Qt::CaseInsensitive) == false)
                            oFile.append( ".scl" );

                        // save the state now
                        saveSCL(string(oFile.toAscii()));

                        // update the recently loaded files list
#ifdef WIN32
                        if ( oFile[1] != ':' )
#else
                        if ( oFile[0] != QDir::separator() )
#endif
                            oFile = oFD.directory().path() + QDir::separator() + oFile;

                        QFileInfo oFileInf( oFile );
                        QString oFileNameOnly = oFileInf.fileName();
                        m_aRecentDN.insert( m_aRecentDN.begin(), QtoStd(oFileNameOnly) );
                        m_aRecentFileA.insert( m_aRecentFileA.begin(), string( oFile.toAscii() ) );
                        m_aRecentFileB.insert( m_aRecentFileB.begin(), string( "NONE" ) );

                        saveRecentList();
                        updateRecentList();
                    }
                    else
                        return;

                }
                else
                {

                    // is the document saveable?
                    if ( !m_pDocumentList->getDoc(iDoc)->getSaveable() )
                        svt_exception::ui()->info("This document cannot be saved.");

                    //is the document locked because used by others
                    if ( m_pDocumentList->getLocked(iDoc) )
                        svt_exception::ui()->info("The document is locked and may not be saved! Please try again after all computations finished.\nATTENTION: Your document was not saved!");

                    // open file chooser dialog
                    QFileDialog oFD( this );
                    oFD.setAcceptMode( QFileDialog::AcceptSave );

                    if ( m_pDocumentList->getDocType(iDoc) == SENSITUS_DOC_PDB )
                        oFD.setNameFilter( m_oFilterPDB );
                    else if ( m_pDocumentList->getDocType(iDoc) == SENSITUS_DOC_SITUS )
                        oFD.setNameFilter( m_oFilterSITUS );

                    // if not a solution, then use previous filename
                    if ( !m_pDocumentList->getSolution(iDoc) || m_pDocumentList->getDocType(iDoc) != SENSITUS_DOC_PDB )
		    {
                        QString oSel = QString( m_pDocumentList->getDoc(iDoc)->getFileName().c_str() );
                        if (oSel.size()==0) // if no filename is assign
                            oSel = QString( m_pDocumentList->getDoc(iDoc)->getDisplayName().c_str() ); 
                        oFD.selectFile( oSel );
		    }
                    else
		    {
                        QFileInfo oFInf( m_pDocumentList->getDoc(iDoc)->getFileName().c_str() );
                        QString oNewName = oFInf.completeBaseName();
                        QString oNumber;
                        oNumber.sprintf("_%i.pdb", iSolCount );
                        oNewName = oFInf.absolutePath() + QDir::separator() + oNewName + oNumber;
                        oFD.selectFile( oNewName );
                    }

                    oFD.setDirectory( m_oCurrentDir );
                    oFD.setFileMode( QFileDialog::AnyFile );

                    if ( oFD.exec() == QDialog::Accepted )
                    {
                        m_oCurrentDir = oFD.directory();
                        QStringList oFiles = oFD.selectedFiles();
                    
                        if (oFiles.size()!=1)
                        {
                            svt_exception::ui()->info("Please choose the file name!");
                            return;
                        }
                        QString oFile = oFiles.at(0);

                        oFile = QDir::convertSeparators( oFile );

                        // does the new filename have an extension?
                        if ( m_pDocumentList->getDocType(iDoc) == SENSITUS_DOC_PDB && oFile.right(4).contains( QString("pdb"), Qt::CaseInsensitive ) == 0 )
 			    oFile.append( ".pdb" );
                        if ( m_pDocumentList->getDocType(iDoc) == SENSITUS_DOC_SITUS && oFile.indexOf( QString(".") ) == -1 )
                            oFile.append( ".situs" );

                        m_pDocumentList->getDoc(iDoc)->save( QtoStd(oFile));
                        // update the recently loaded files list
#ifdef WIN32
                        if ( oFile[1] != ':' )
#else
                        if ( oFile[0] != QDir::separator() )
#endif
                            oFile = oFD.directory().path() + QDir::separator() + oFile;

                        QFileInfo oFileInf( oFile );
                        QString oFileNameOnly = oFileInf.fileName();
                        m_aRecentDN.insert( m_aRecentDN.begin(), oFileNameOnly.toAscii().data() );
                        m_aRecentFileA.insert( m_aRecentFileA.begin(), string( oFile.toAscii().data()) );
                        m_aRecentFileB.insert( m_aRecentFileB.begin(), string( "NONE" ) );

                        saveRecentList();
                        updateRecentList();

                        // update the document list
                        pDli->showDocName();
                    }
		    else
                        return;
                }
            }
        }
    }
}

/**
 * load state file
 */
void sculptor_window::loadSCL()
{
    m_pDocumentList->setCurrentItem( 0 );

    // open file chooser dialog
    QFileDialog oFD(this);
    oFD.setNameFilter( m_oFilterSCL );
    oFD.selectNameFilter( m_oFilterSCL );
    oFD.setViewMode( QFileDialog::List );
    oFD.setDirectory( m_oCurrentDir );

    QString oFile;

    if ( oFD.exec() == QDialog::Accepted )
    {
	QStringList oFiles = oFD.selectedFiles();
        if (!oFiles.isEmpty())
	    oFile = oFiles[0];

	// Load the scene file
        loadDoc( oFile, QString("NONE") );

    } else
        statusBar()->showMessage( "Loading aborted", 2000 );

    m_oCurrentDir = oFD.directory();

}
/**
 * save state file
 */
void sculptor_window::saveSCL()
{
    m_pDocumentList->clearSelection();
    m_pDocumentList->setCurrentItem( 0 );
    save();
}

/**
 * Archive the current state
 */
void sculptor_window::saveArchive()
{
    // first we have to check if all the documents are saved and if the files indeed do exist
    for(int i=1; i<m_pDocumentList->count();i++)
    {
        if ( m_pDocumentList->getDoc(i)->getChanged() )
        {
            QString oError;
            oError.sprintf( "Error: Document %s needs to be saved before the archive can be exported!", m_pDocumentList->getDoc(i)->getDisplayName().c_str() );
            svt_exception::ui()->error( QtoStd(oError).c_str() );
            return;
        }
        if ( m_pDocumentList->getLocked(i) )
        {
            QString oError;
            oError.sprintf( "Error: Document %s is being used, archive cannot be created right now!", m_pDocumentList->getDoc(i)->getDisplayName().c_str() );
            svt_exception::ui()->error( QtoStd(oError).c_str() );
            return;
        }

        QFileInfo oFI( m_pDocumentList->getDoc(i)->getFileName().c_str() );
        if (!oFI.exists())
        {
            QString oError;
            oError.sprintf( "Error: Document %s cannot be found on the disk, please save it to a file, before creating the archive!", m_pDocumentList->getDoc(i)->getDisplayName().c_str() );
            svt_exception::ui()->error( QtoStd(oError).c_str() );
            return;
        }
    }

    // open file chooser dialog
    QFileDialog oFD( this );
    oFD.setNameFilter(m_oFilterSAR);
    oFD.selectNameFilter(m_oFilterSAR);
    oFD.setDirectory(m_oCurrentDir);
    oFD.setFileMode(QFileDialog::AnyFile);
    oFD.setAcceptMode( QFileDialog::AcceptSave );

    if (oFD.exec() == QDialog::Accepted )
    {
        QString oFile;
        m_oCurrentDir = oFD.directory();
        QStringList oSelectedFiles = oFD.selectedFiles();
        if (!oSelectedFiles.isEmpty())
	    oFile = oSelectedFiles[0];
        oFile = QDir::convertSeparators( oFile );

        // does the new filename have an extension?
        if (oFile.endsWith( QString(".sar"), Qt::CaseInsensitive) == false)
            oFile.append( ".sar" );

        // does the file exist already?
        QFileInfo oFileInf( oFile );
        if (oFileInf.exists() )
            if ( !svt_exception::ui()->question("Attention: File exists already - Do you want to overwrite the file?") )
                return;

        // save the bundle now
        vector<string> oFiles;
        for(int i=1; i<m_pDocumentList->count();i++)
        {
            oFiles.push_back( string( m_pDocumentList->getDoc(i)->getFileName()) );
            if (m_pDocumentList->getDoc(i)->getEliDataLoaded())
            {
                oFiles.push_back( string( m_pDocumentList->getDoc(i)->getEliCrossCorrFile().toAscii() ) );
                oFiles.push_back( string( m_pDocumentList->getDoc(i)->getEliAngleIndexFile().toAscii() ) );
                oFiles.push_back( string( m_pDocumentList->getDoc(i)->getEliAngleListFile().toAscii() ) );
            }
        }

        // save the state
        string oStateFile = svt_getHomeDir() + QDir::separator().toLatin1() + string(oFileInf.baseName().toAscii()) + string(".scl");
        saveSCL( oStateFile, true );
        oFiles.push_back( oStateFile );

        // now create bundle
        svt_createArchive( string( oFile.toAscii() ), oFiles );

        // and remove the state again
        QFile::remove( QString(oStateFile.c_str()) );

        // update the recently loaded files list
        m_aRecentDN.insert( m_aRecentDN.begin(), QtoStd(oFileInf.fileName()) );
        m_aRecentFileA.insert( m_aRecentFileA.begin(), string( oFileInf.filePath().toAscii() ) );
        m_aRecentFileB.insert( m_aRecentFileB.begin(), string( "NONE" ) );
        saveRecentList();
        updateRecentList();

    } 
}
/**
 * Load an archive file and restore the state
 */
void sculptor_window::loadArchive(QString oFile)
{
    QFileInfo oFileInf( oFile );

    if (!oFileInf.exists())
    {
        QString oError;
        oError.sprintf( "Error: %s does not exist!", QtoStd(oFile).c_str() );
        svt_exception::ui()->error( QtoStd(oError).c_str() );
    }

    // if there is no file given, then open the file chooser
    if (oFile.length() == 0)
    {
        // open file chooser dialog
        QFileDialog oFD(this);
        oFD.setNameFilter(m_oFilterSAR);
        oFD.selectNameFilter(m_oFilterSAR);
        oFD.setDirectory(m_oCurrentDir);
        oFD.setFileMode(QFileDialog::ExistingFiles);

        if (oFD.exec() == QDialog::Accepted )
        {
            m_oCurrentDir = oFD.directory();
	    QStringList oFiles = oFD.selectedFiles();
	    if (!oFiles.isEmpty())
  	        oFile = oFiles[0];
            oFileInf = QFileInfo( oFile );

        } else

            return;
    }

    // open file chooser dialog for the unpack directory
    QFileDialog oFD_Dir(this,  QString("Please select a directory into which the files can be extracted") );
    oFD_Dir.setDirectory(m_oCurrentDir);
    oFD_Dir.setFileMode( QFileDialog::DirectoryOnly );

    if (oFD_Dir.exec() == QDialog::Accepted )
    {
	string oDir;
	QStringList oFiles = oFD_Dir.selectedFiles();
        if (!oFiles.isEmpty())
            oDir = string(oFiles[0].toAscii()) + QDir::separator().toLatin1();

        // now extract bundle
        svt_extractArchive( string( oFile.toAscii() ), oDir );

        // and open the scene file
        string oStateFile = oDir + string(oFileInf.baseName().toAscii()) + string(".scl");
        loadDoc( oStateFile.c_str(), QString("NONE") );

        // update the recently loaded files list
        m_aRecentDN.insert( m_aRecentDN.begin(), QtoStd(oFileInf.fileName()) );
        m_aRecentFileA.insert( m_aRecentFileA.begin(), string( oFileInf.filePath().toAscii() ) );
        m_aRecentFileB.insert( m_aRecentFileB.begin(), string( "NONE" ) );
        saveRecentList();
        updateRecentList();
    }
}

/**
 * delete the selected document
 */
void sculptor_window::sDocsRemove()
{
    bool bAnything = false;

    for(int i=1; i<m_pDocumentList->count(); ++i)
        if ( m_pDocumentList->getSelected(i) )
            bAnything = true;

    if ( bAnything == false || !svt_exception::ui()->question("Do you really want to close those documents?") )
        return;

    bool bOk;
    bAnything = false;

    for( int i=0; i<m_pDocumentList->count(); ++i )
    {
        if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) != SENSITUS_DOC_SCENE )
	{
	    bOk = delDoc(i);

	    if ( bOk ) //delete was succesful
	    {   
		// we deleted one document, so the document counter needs to be adjusted
		i--;
		bAnything = true;
	    }
	}
    }

    // if a document was deleted, select the scene now
    if (bAnything)
	m_pDocumentList->setCurrentItem( 0 );
}

/**
 * delete a document
 * \return true if successful delete; false if nothing was deleted
 */
bool sculptor_window::delDoc(int iDoc)
{
    //document is used by some other program/optimization
    if ( m_pDocumentList->getLocked(iDoc) )
    {
	svt_exception::ui()->info("The document is locked and cannot be deleted! Please try again later.");
	return false;
    }

    disableInput();

    window_documentlistitem* pDLI = (*m_pDocumentList)[iDoc];

    // check if it contains exhaustive search data
    if (m_pDocumentList->getDoc(iDoc)->getEliDataLoaded())
    {
	// see if there are solutions that depend on the data
	for (int i=0; i<m_pDocumentList->count(); i++)
	    if ( iDoc != i && m_pDocumentList->getDoc(iDoc) == m_pDocumentList->getDoc(i)->getOrigProbeDoc() )
	    {
		svt_exception::ui()->info("One or more solutions depend on this document.\nPlease delete the solutions first.");
		return false;
	    }
    }

    // it is not the moved node any longer
    if (pDLI->getMoved())
    {
	m_pScene->setMovedNode(NULL);
	setMovedDoc(0);
    }

    // is it the target or probe doc?
    bool bWasTarget = false;
    bool bWasProbe  = false;
    if (pDLI->getTarget()) bWasTarget = true;
    if (pDLI->getDock())   bWasProbe  = true;
    
    //check if arrows point from/towards its codebook vectors
    if (m_pCVDisplacementObj != NULL)
    {
	if ( m_pDocumentList->getDoc(iDoc) == m_pCVDisplacementObj->getTail() )
	{
	    m_pCVDisplacementObj->setTail(NULL);
	    m_pRenderDisplacementArrows->setChecked( FALSE );
	}
	else if ( m_pDocumentList->getDoc(iDoc) == m_pCVDisplacementObj->getHead() )
	{
	    m_pRenderDisplacementArrows->setChecked( FALSE );
	    m_pCVDisplacementObj->setHead(NULL);
	}
    }

    // nor is it a solution
    bool bWasSolution = false;
    if (pDLI->getSolution())
	bWasSolution = true;
	

    // get the document
    sculptor_document* pSD = m_pDocumentList->getDoc(iDoc);

    // now remove the node from the scene
    if (m_pDocumentList->getDoc(iDoc)->getNode() != NULL)
    {
	svt_node* pNode = m_pDocumentList->getDoc(iDoc)->getNode();

	if ( m_pDocumentList->getDocType(iDoc) == SENSITUS_DOC_CLIP )
	    m_pScene->delClipNode( pNode );
	else
	    m_pScene->delDataNode( pNode );
    }

    // remove document out of the listbox
    m_pDocumentList->removeItem(iDoc);

    // delete from memory
    if (pSD != NULL)
	delete pSD;

    // adjust origin if the first document was deleted
    if (iDoc == 1)
	adjustOrigin();

    // was it the target doc?
    if (bWasTarget)
    {
	m_pTargetDoc = NULL;
	int j = 0;

	while ( (j < m_pDocumentList->count()) && (m_pDocumentList->getDocType(j) != SENSITUS_DOC_SITUS) )
	    j++;

	if ( j <= m_pDocumentList->count() )
	    setTargetDoc(j);

 	//if the target doc was deleted and there is an eraser, delete the eraser
	for( int k=0; k < m_pDocumentList->count(); ++k)
	{
	    if (m_pDocumentList->getDocType(k) == SENSITUS_DOC_ERASER)
	    {
	        delDoc(k);
		break;
	    }
        }
    }

    // was it the probe doc?
    if (bWasProbe)
    {
	m_pProbeDoc  = NULL;
	int j = 0;
	while( (j <  m_pDocumentList->count()) && (m_pDocumentList->getDocType(j) != SENSITUS_DOC_PDB) )
	    j++;

	if( j <= m_pDocumentList->count() )
	    setDockDoc(j);
    }

    // if the document was a solution, update the solution set
    if (bWasSolution)
	setSolutionSetChanged(true);

    enableInput();

    return true;
}

/**
 * move document up in the list of documents
 */
void sculptor_window::sDocMoveUp()
{
    int i = m_pDocumentList->getCurrentIndex();

    if (i == -1)
	return;

    if ( i >= 2 && i < m_pDocumentList->count() )
	m_pDocumentList->moveItemUp(i);

    adjustOrigin();
    svt_redraw();
}
/**
 * move document down in the list of documents
 */
void sculptor_window::sDocMoveDown()
{
    int i = m_pDocumentList->getCurrentIndex();

    if ( i >= 1 && i < m_pDocumentList->count()-1 )
	m_pDocumentList->moveItemDown(i);

    adjustOrigin();
    svt_redraw();
}

/**
 * move document to the top of the list of documents
 */
void sculptor_window::sDocMakeTop()
{
    int i = m_pDocumentList->getCurrentIndex();

    if ( i >= 2 && i < m_pDocumentList->count() )
	m_pDocumentList->moveItemUpmost(i);

    adjustOrigin();
    svt_redraw();
}

/**
 * get documentlist
 */
window_documentlist* sculptor_window::getDocumentList()
{
    return m_pDocumentList;
}

/**
 * add a clipping plane
 */
int sculptor_window::addClip()
{
    clip_document* pSD = new clip_document( m_pScene->getClipTopNode() );
    svt_vector4f oOrigin = getOrigin();
    oOrigin.multScalar( 1.0E8f );

    pSD->getNode()->setPos( new svt_pos(oOrigin.x(),oOrigin.y(),oOrigin.z()) );

    return addDoc( (sculptor_document*)pSD, "Clipping Plane" );
}

/**
 * display eraser tool
 */
int sculptor_window::addEraser()
{
    if (getTargetDoc() == NULL)
    {
        guiWarning("No Target Document Selected", "Please set a target map to erase.", "Eraser tool error");
	return -1;
    }
    else
    {   
        sculptor_doc_eraser* sd = new sculptor_doc_eraser(m_pScene->getDataTopNode(), (situs_document*)getTargetDoc());
        svt_vector4f oOrigin = getOrigin();
        oOrigin.multScalar( 1.0E8f );

        sd->getNode()->setPos(new svt_pos(oOrigin.x(),oOrigin.y(),oOrigin.z()));

        return addDoc( (sculptor_document*)sd, "Eraser Tool" );
    }
}

/**
 * add a marker 
 */
int sculptor_window::addMarker()
{
    sculptor_doc_marker* pSD = new sculptor_doc_marker( m_pScene->getDataTopNode() );
    svt_vector4f oOrigin = getOrigin();
    oOrigin.multScalar( 1.0E8f );

    pSD->getNode()->setPos( new svt_pos(oOrigin.x(),oOrigin.y(),oOrigin.z()) );

    return addDoc( (sculptor_document*)pSD, "Marker" );
}

/**
 *
 */
void sculptor_window::togglePropDlgArea()
{
    if ( m_pLeftSplitter->getCollapsed() )
    {
        // let the documentlist know that the user requested an uncollapse
        m_pDocumentList->hidePropDlg( false );
        // open the prop dlg area
        m_pLeftSplitter->setCollapsed(false);
        // show the prop dlg
        m_pDocumentList->showPropDialog();
        // check the menu item
        m_pProperties->blockSignals( TRUE );
        m_pProperties->setChecked( TRUE );
        m_pProperties->blockSignals( FALSE );
    }
    else
    {
        // let the documentlist know that the user requested a collapse
        m_pDocumentList->hidePropDlg( true );
        // close the prop dlg area
        m_pLeftSplitter->setCollapsed();
        // uncheck the menu item
        m_pProperties->blockSignals( TRUE );
        m_pProperties->setChecked( FALSE );
        m_pProperties->blockSignals( FALSE );
    }
}

/**
 *
 */
void sculptor_window::sToggleFullScreen()
{
    if ( isFullScreen() )
    {
        if (m_bWindowWasMaximized)
        {
            // FIXME FIXME FIXME
#ifdef LINUX
            // from the Qt doc: "On X11, this function may not work properly with certain window
            // managers." setting the window geometry and then removing the fullscreen state
            // corrects this behavior, but may cause flickering.  do it for now on linux so that the
            // behavior (going from fullscreen to maximized window) is correct
            setGeometry(m_oWindowGeometry);
            setWindowState(windowState() & ~Qt::WindowFullScreen);
#endif
            showMaximized();
        }
        else
        {
            showNormal();
        }

        // restore collapsed state of the splitters
        if (!m_bTextAreaCollapsed)
            m_pRightSplitter->setCollapsed(false);
        if (!m_bDocumentListCollapsed)
            m_pMainSplitter->setCollapsed(false);
    }
    else
    {
        // remember if the splitters were collapsed or not
        m_bTextAreaCollapsed =     m_pRightSplitter->getCollapsed();
        m_bDocumentListCollapsed = m_pMainSplitter->getCollapsed();

        // collapse the splitters if going to fullscreen
        if (!m_bTextAreaCollapsed)
            m_pRightSplitter->setCollapsed();
        if (!m_bDocumentListCollapsed)
            m_pMainSplitter->setCollapsed();
        
        // FIXME FIXME FIXME
        // strangely, isMaximized does not deliver correct answers on X11. a workaround is
        // surprisingly easy: just check if the available screen size is the same like the size of
        // the current window (including frame)
        QDesktopWidget oDW;
        if ( isMaximized() ||
             (   oDW.availableGeometry(this).width()  == frameGeometry().width()
              && oDW.availableGeometry(this).height() == frameGeometry().height()) )
        {
            m_bWindowWasMaximized = true;
            // remember the geometry for the X11 workaround (see above)
            m_oWindowGeometry = geometry();
        }
        else
        {
            m_bWindowWasMaximized = false;
        }

        showFullScreen();
    }
}

/**
 *
 */
void sculptor_window::setPropDlgAreaVisible(bool bShow)
{
    if ( bShow )
    {
        // open the prop dlg area
        m_pLeftSplitter->setCollapsed(false);
        // check the menu item
        m_pProperties->blockSignals( TRUE );
        m_pProperties->setChecked( TRUE );
        m_pProperties->blockSignals( FALSE );
    }
    else
    {
        // close the prop dlg area
        m_pLeftSplitter->setCollapsed();
        // uncheck the menu item
        m_pProperties->blockSignals( TRUE );
        m_pProperties->setChecked( FALSE );
        m_pProperties->blockSignals( FALSE );
    }
}

/*
 *
 */
bool sculptor_window::getPropDlgAreaVisible()
{
    return !(m_pLeftSplitter->getCollapsed());
}


/**
 * toggle the display state of a document
 */
void sculptor_window::toggleViewDoc()
{
    int i;

    for ( i=0; i<m_pDocumentList->count(); ++i)
    {
	if ( m_pDocumentList->getSelected(i) )
	{
	    if ( m_pDocumentList->getVisible(i) )
		setViewDoc( i, false );
	    else
		setViewDoc( i, true );
        }
    }
    updateMenuItems();
}

/**
 * toggle the display state of a document
 */
void sculptor_window::setViewDoc(int iIndex, bool bVisible)
{
    disableInput();

    (*m_pDocumentList)[iIndex]->setVisible( bVisible );

    updateMenuItems();

    enableInput();
}

/**
 * toggle the move state of a document
 */
void sculptor_window::toggleMovedDoc()
{
    int i = m_pDocumentList->getCurrentIndex();

    if (i == -1)
	return;

    if ( (*m_pDocumentList)[i]->getMoved() )
	setMovedDoc( 0 );
    else
	setMovedDoc( i );
}

/**
 * restore the initial transformation matrix of the document
 */
void sculptor_window::restoreInitMat()
{
    window_documentlistitem* pDLI = m_pDocumentList->getCurrentItem();

    if (pDLI != NULL)
        pDLI->getDoc()->resetInitMat();

    adjustOrigin();
}


///////////////////////////////////////////////////////////////////////////////
// view settings
///////////////////////////////////////////////////////////////////////////////

/**
 * this is a timer function called every second to update the current FPS on the statusbar.
 */
void sculptor_window::showFPS()
{
    QString s;

    /*if (svt_getFPS() < 200)
        if (m_pScene->getFUS() > 1)
    	    s.sprintf( "FPS: %i FUS: %i (Cor % 6g)", svt_getFPS(), m_pScene->getFUS(), m_pForceAlgo->getCor());
        else 
    	    s.sprintf( "FPS: %i", svt_getFPS());
    else
    {
        if (m_pScene->getFUS() > 1)
    	    s.sprintf( "FUS: %i (Cor % 6g)", m_pScene->getFUS(), m_pForceAlgo->getCor());
        else 
    	    s.sprintf( "FPS: %i", svt_getFPS());
    }*/

    if (svt_getFPS() > 100)
    {
        statusBar()->clearMessage();
        return;
    }

    s.sprintf( "FPS: %i", svt_getFPS());
    statusBar()->showMessage( s, 2000 );
}

/**
 * save a screenshot
 */
void sculptor_window::saveScreenshot(QString pFilename)
{
#ifndef SVT_SYSTEM_QT

    if (pFilename.isEmpty())
    {
        // open file chooser dialog
        QFileDialog oFD(this);
        QString oFilterTarga("Targa Image File (*.tga)");
        oFD.setNameFilter(oFilterTarga);
        oFD.selectNameFilter(oFilterTarga);
        oFD.setDirectory(m_oCurrentDir);
        oFD.setFileMode(QFileDialog::AnyFile);
        oFD.setAcceptMode( QFileDialog::AcceptSave );

        QString oSel = QString( "screenshot.png" );
        oFD.selectFile( oSel );

        if (oFD.exec() == QDialog::Accepted )
        {
            m_oCurrentDir = oFD.directory();
            QStringList oFiles = oFD.selectedFiles();
            if (!oFiles.isEmpty())
                QString fn = oFiles[0];

            if (fn.endsWith( QString(".tga"), Qt::CaseInsensitive ) == false)
                fn.append( ".tga" );

            m_pScene->saveScreenshot(pFilename);

            return;

        } else
            return;
    }

    m_pScene->saveScreenshot( pFilename );

#else

    if (pFilename.isEmpty())
    {
        // open file chooser dialog
        QFileDialog oFD(this);
        QString oFilterPNG_JPG("Image File (*.png *.jpg)");
        oFD.setNameFilter(oFilterPNG_JPG);
        oFD.selectNameFilter(oFilterPNG_JPG);
        oFD.setDirectory(m_oCurrentDir);
        oFD.setFileMode(QFileDialog::AnyFile);
        oFD.setAcceptMode( QFileDialog::AcceptSave );
        QString oSel = QString( "screenshot.png" );
        oFD.selectFile( oSel );

        if (oFD.exec() == QDialog::Accepted )
        {
	    QString oFilename;
            m_oCurrentDir = oFD.directory();
            QStringList oFiles = oFD.selectedFiles();
            if (!oFiles.isEmpty())
	        oFilename = oFiles[0];
            QString oFormat = "JPEG";

            QImage* pImage = svt_qtGetFramebuffer();
            if (pImage)
            {
                if (oFilename.endsWith( ".png"))
                    oFormat = "PNG";
                if (oFilename.endsWith( ".jpg"))
                    oFormat = "JPEG";

                pImage->save( oFilename, QtoStd(oFormat).c_str(), -1 );
            }
            return;

        } else

            return;

    }

    QString oFilename( pFilename );
    QString oFormat = "JPEG";

    QImage* pImage = svt_qtGetFramebuffer();
    if (pImage)
    {
        if (oFilename.endsWith( ".png"))
            oFormat = "PNG";
        if (oFilename.endsWith( ".jpg"))
            oFormat = "JPEG";

        pImage->save( oFilename, QtoStd(oFormat).c_str(), -1 );
    }

#endif
}

/**
 * Get the current directory
 * \return pointer to array of char with the current directory information
 */
string sculptor_window::getCurrentDir()
{
    QString oPath = m_oCurrentDir.absolutePath();
    string oStr( oPath.toAscii() );

    return oStr;
};
/**
 * Set the current directory
 * \param string with current directory information
 */
void sculptor_window::setCurrentDir(string oStr)
{
    m_oCurrentDir.cd( QString( oStr.c_str() ) );
};


/**
 * render with povray
 */
void sculptor_window::render()
{
    m_pScene->renderPOV();
}

/**
 * export as alias/wavefront
 */
void sculptor_window::exportOBJ()
{
    // open file chooser dialog
    QFileDialog oFD(this);
    QString oFilterOBJ("Wavefront File (*.obj)");
    oFD.setNameFilter(oFilterOBJ);
    oFD.selectNameFilter(oFilterOBJ);
    oFD.setDirectory(m_oCurrentDir);
    oFD.setFileMode(QFileDialog::AnyFile);

    if (oFD.exec() == QDialog::Accepted )
    {
	QString fn;
        m_oCurrentDir = oFD.directory();
        QStringList oFiles = oFD.selectedFiles();
	if (!oFiles.isEmpty())
	    fn = oFiles[0];

        if (fn.endsWith( QString(".obj"), Qt::CaseInsensitive ) == true)
        {
            fn = fn.left( fn.length() - 4 );
        }

	m_pScene->renderOBJ(fn.toAscii());
    }
}

/**
 * zoom in.
 */
void sculptor_window::zoomIn()
{
    if (m_pSceneDoc)
        m_pSceneDoc->zoomIn();
}

/**
 * zoom out.
 */
void sculptor_window::zoomOut()
{
    if (m_pSceneDoc)
        m_pSceneDoc->zoomOut();
}


/**
 * some custom key press events
 */
void sculptor_window::keyPressEvent(QKeyEvent* pEvent)
{
    if (pEvent->key() == Qt::Key_Plus)
        zoomIn();
    else if (pEvent->key() == Qt::Key_Minus)
        zoomOut();
    else if (pEvent->key() == Qt::Key_F8)
        scriptRun();
    else
        QMainWindow::keyPressEvent(pEvent);
}

/**
 * set center mode
 */
void sculptor_window::setCenterMode()
{
    if (m_pCenterMode->isChecked())
    {
        m_pCenterMode->setChecked( false );
        m_pScene->setCenterMode( false );
    }
    else
    {
        m_pCenterMode->setChecked( true );
        m_pScene->setCenterMode(true);
    }
}

/**
 * set background color
 */
void sculptor_window::setBackgroundColor()
{
    svt_color* pColor = svt_getBackgroundColor();
    if (pColor == NULL){
        pColor = new svt_color();
        svt_setBackgroundColor(pColor);
    }

    float fRed = (255.0f*pColor->getR());
    float fGreen = (255.0f*pColor->getG());
    float fBlue = (255.0f*pColor->getB());

    QColor oColor;
    oColor.setRgb(
		  (int)fRed,
                  (int)fGreen,
                  (int)fBlue
                 );

    QColor oCol = QColorDialog::getColor(oColor, 0);
    if (oCol.isValid())
    {
        fRed = (float)oCol.red() / 255.0f;
        fGreen = (float)oCol.green() / 255.0f;
        fBlue = (float)oCol.blue() / 255.0f;

        pColor->setR(fRed);
        pColor->setG(fGreen);
        pColor->setB(fBlue);

	if (m_pLightingDlg != NULL)
	    m_pLightingDlg->updateBGColor();
    }

    svt_redraw();
}

/**
 * adjust origin
 */
void sculptor_window::adjustOrigin()
{
    if (m_pDocumentList->count() < 2)
        return;

    sculptor_document* pDoc = m_pDocumentList->getDoc(1);
    sculptor_document* pDocOrig = m_pDocumentList->getDoc(0);

    if (pDoc)
    {
        svt_node* pNode = m_pScene->getTopNode();
        if (!pNode)
            return;

        svt_matrix4f * pMatrix = pNode->getTransformation();
        if (!pMatrix)
            return;

	pMatrix->setToId();
        svt_vector4f oVec = pDoc->getRealPos();
        oVec.multScalar( 1.0E8f );

        pMatrix->setTranslationX( -oVec.x() );
        pMatrix->setTranslationY( -oVec.y() );
        pMatrix->setTranslationZ( -oVec.z() );

        if ( (*m_pDocumentList)[0]->getMoved() && pDocOrig != NULL)
            m_pScene->setMovedNode(pDocOrig->getNode(), oVec);
    }
}


/**
 * Merge selected PDB documents
 */
void sculptor_window::mergePdbDocs()
{
    vector< pdb_document* > oSelectedPDBs;
    svt_point_cloud_pdb< svt_vector4<Real64> >* rPDB = new svt_point_cloud_pdb< svt_vector4<Real64> >();
    svt_point_cloud_pdb< svt_vector4<Real64> > oTmpPDB;
    pdb_document *pPDB, *pNewDoc;
    svt_matrix4< Real64 > oMat;
    unsigned int i, j;

    //get selected PDB
    for(i=1; i < (unsigned int)m_pDocumentList->count(); i++)
    {
        if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
            oSelectedPDBs.push_back( (pdb_document*)(m_pDocumentList->getDoc(i)) );
    }

    if (oSelectedPDBs.size() < 2)
    {
        QMessageBox::warning(this,
                             QString("Merging PDBs"),
                             QString("Please select two or more pdb files."));
        return;
    }

    svt_exception::ui()->progressPopup("Merging", 0);

    try
    {
        for ( i=0; i < oSelectedPDBs.size(); ++i )
        {
            int iPerc = (int)( (((float)(i)) / ((float)(oSelectedPDBs.size()))) * 100.0f );
            svt_exception::ui()->progress( iPerc );

            pPDB = oSelectedPDBs[i];
            oTmpPDB = pPDB->getPointCloud();

            // translate object and save
            svt_vector4<Real64> oVec;
            svt_matrix4<Real64> oMat( *(pPDB->getNode()->getTransformation()) );

            // adjust coordinates
            for( j=0; j< oTmpPDB.size(); j++)
            {
                oVec = oTmpPDB[j];
                oVec *= 1.0E-2;
                oVec.w( 1.0f );
                oVec = oMat * oVec;                oVec *= 1.0E2;
    
                oTmpPDB[j] = oVec;
                oTmpPDB.getAtom(j)->setPDBIndex( j+1 );
            }

            rPDB->append(oTmpPDB, i + 1 );
        }
        svt_exception::ui()->progressPopdown();
        rPDB->setSecStructAvailable(true);
        rPDB->calcAtomModels();

        pNewDoc = new pdb_document(m_pScene->getDataTopNode(), "", SVT_PDB, "", SVT_NONE, rPDB);
        
        //FIXME get the shortnames of the merged documents and compose a displayname
        addDoc( (sculptor_document*)pNewDoc, "merged_pdb_document.pdb" );

    }
    catch (svt_userInterrupt&)
    {
        //delete 
        rPDB->deleteAllAtoms();
        rPDB->deleteAllBonds();
        
        svt_exception::ui()->progressPopdown();
        return;
    }

    delete rPDB;
};

/**
 * toggle orthographic projection mode
 */
void sculptor_window::toggleOrtho()
{
    svt_setOrtho(!svt_getOrtho());
    m_pOrthoMode->setChecked( svt_getOrtho() );
};

/**
 * toggle 3D coordinate system visibility
 */
void sculptor_window::toggle3DCoordSystem()
{
    m_pScene->setCoordinateSystemVisible(!m_pScene->getCoordinateSystemVisible());
    m_pCoordinateSystem->setChecked( m_pScene->getCoordinateSystemVisible() );
}

/**
 * toggle pipette visibility
 */
void sculptor_window::togglePipette()
{
    m_pScene->setPipetteVisible(!m_pScene->getPipetteVisible());
    m_pPipette->setChecked( m_pScene->getPipetteVisible() );
}

/**
 * high contrast or low contrast palette
 */
void sculptor_window::toggleHighContrast()
{
    m_bHighContrast = !m_bHighContrast;
    int i;

    for( i=1; i<m_pDocumentList->count(); i++ )
    {
        if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB)
        {
            pdb_document* pPDBDoc = (pdb_document*)(m_pDocumentList->getDoc(i));
            pPDBDoc->setHighContrast( m_bHighContrast );
        }
    }
    m_pHighContrast->setChecked( m_bHighContrast );

}

/**
 * Set global OpenGL lighting options
 */
void sculptor_window::setGlobalLighting()
{
    if (m_pLightingDlg == NULL)
	m_pLightingDlg = new dlg_lighting( (QWidget*)this, (svt_scene*) m_pScene );

    m_pLightingDlg->show();
}

/**
 * return a pointer to the lighting dialog
 */
dlg_lighting* sculptor_window::getLightingDlg()
{
    return m_pLightingDlg;
}

/**
 * close the surfaces at clip-planes
 */
void sculptor_window::cappedClip()
{
    m_pScene->setCappedClip( !m_pScene->getCappedClip() );
    m_pCappedClip->setChecked( m_pScene->getCappedClip() );
};

/**
 * set the eye separation for the stereo rendering
 */
void sculptor_window::setEyeSeparation()
{
    bool ok;
    QString oCaption( "Eye Separation" );
    QString oQuestion( "Please enter the new eye separation for the stereoscopic rendering:" );
    double fEye = QInputDialog::getDouble(this,
                                          oCaption,
                                          oQuestion,
                                          svt_getEyeSeparation(), 0.0, 1000.0, 3, &ok);

    if ( ok )
    {
        svt_setEyeSeparation( fEye );
        saveRecentList();
    }
};

/**
 * Generate image series for 3DNP
 */
void sculptor_window::gen3DNP()
{
/*    svt_node* pNode = m_pScene->getMovedNode();

    if (pNode != NULL)
    {
	svt_matrix4f* pMat = pNode->getTransformation();

	svt_matrix4f oMat = *pMat;

	m_pScene->getTopNode()->setTransformation( &oMat );

	char pFname[256];

	svtout << "Starting 3DNP screenshot series..." << endl;

	for(unsigned int i=0; i<50; i++)
	{
	    oMat.postAddRotation( 1, -deg2rad( 7.2 ) );

	    sprintf( pFname, "Row01Shot%02i.tga", i );

	    svtout << "   Generating image number " << i << " - " << pFname << endl;

	    m_pScene->saveScreenshot( pFname );

	    svt_redraw();
	    svt_idle();
	    svt_redrawWindow(1);
	    //svt_redraw_system(1,0,0);
	}

	m_pScene->getTopNode()->setTransformation( pMat );
    }*/
}

///////////////////////////////////////////////////////////////////////////////
// PDB Document Toolbar
///////////////////////////////////////////////////////////////////////////////

/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderMode( int iMode )
{
    bool bNonChanged = true;
    int i;

    for(i=1; i < m_pDocumentList->count(); i++)
    {
	if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	{
	    pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

            pPDB->changeRenderMode( iMode );

            bNonChanged = false;
	}
    }

    if (bNonChanged == true)
	for(int i=1;i < m_pDocumentList->count(); i++)
	{
	    if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	    {
		pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

                pPDB->changeRenderMode( iMode );
	    }
    }
};

/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeOFF( )
{
    changeRenderMode( GM_OFF );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModePOINT( )
{
    changeRenderMode( GM_POINT );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeLINE( )
{
    changeRenderMode( GM_LINE );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeVDW( )
{
    changeRenderMode( GM_VDW );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeCPK( )
{
    changeRenderMode( GM_CPK );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeDOTTED( )
{
    changeRenderMode( GM_DOTTED );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeLICORICE( )
{
    changeRenderMode( GM_LICORIZE );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeTUBE( )
{
    changeRenderMode( GM_NEWTUBE );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeCARTOON( )
{
    changeRenderMode( GM_CARTOON );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void sculptor_window::changeRenderModeTOON( )
{
    changeRenderMode( GM_TOON );
}

/**
 * Select a global coloring mode (for pdb documents only)
 * \param iMode the mode
 */
void sculptor_window::changeColmapMode( int iMode )
{
    bool bNonChanged = true;
    int i;

    for(i=1; i < m_pDocumentList->count(); i++)
    {
	if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	{
	    pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

	    pPDB->setGlobalColmap( iMode, i );

            bNonChanged = false;
	}
    }

    if ( bNonChanged )
	for(i=1; i < m_pDocumentList->count(); i++)
	{
	    if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	    {
		pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

		pPDB->setGlobalColmap( iMode, i );
	    }
	}
};

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeSOLID()
{
    int i;
    int iCount = 0;
    int iPDBCount = 0;
    int iColor = -1;
    pdb_document* pPDB = NULL;

    for(i=1; i<m_pDocumentList->count(); i++)
    {
	if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	{
	    pPDB = (pdb_document*)m_pDocumentList->getDoc(i);
	    iCount++;
        }
        if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB)
        {
            if (iCount == 0)
                pPDB = (pdb_document*)m_pDocumentList->getDoc(i);

            iPDBCount++;
        }
    }

    if (pPDB != NULL && !(iCount == 1 || iPDBCount == 1))
    {
        iColor = pPDB->showColorChooser();
        if (iColor == -1)
            return;
    }

    if (iCount > 1)
	for(i=1; i<m_pDocumentList->count(); i++)
	{
	    if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	    {
		pdb_document* pPDB = (pdb_document*)m_pDocumentList->getDoc(i);

		pPDB->setGlobalColmap( COLMAP_SOLID, (unsigned int)(iColor) );
	    }
	}
    else if (iCount == 1 || iPDBCount == 1)
	pPDB->setGlobalColmapSOLIDINTERACTIVE();

    else {

	for(i=1; i<m_pDocumentList->count(); i++)
	{
	    if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB)
	    {
		pdb_document* pPDB = (pdb_document*)m_pDocumentList->getDoc(i);

		pPDB->setGlobalColmap( COLMAP_SOLID, (unsigned int)(iColor) );
	    }
	}
    }
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeRAINBOW()
{
    int i;
    int iCount = 0;
    int iPDBCount = 0;
    pdb_document* pPDB = NULL;

    for(i=1; i<m_pDocumentList->count(); i++)
    {
	if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	{
	    pPDB = (pdb_document*)m_pDocumentList->getDoc(i);
	    iCount++;
        }
        if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB)
        {
            if (iCount == 0)
                pPDB = (pdb_document*)m_pDocumentList->getDoc(i);

            iPDBCount++;
        }
    }

    if (iCount > 1)
	for(i=1; i<m_pDocumentList->count(); i++)
	{
	    if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	    {
		pdb_document* pPDB = (pdb_document*)m_pDocumentList->getDoc(i);

		pPDB->setGlobalColmap( COLMAP_SOLID, i );
	    }
	}
    else if (iCount == 1 || iPDBCount == 1)
	pPDB->setGlobalColmapSOLIDINTERACTIVE();

    else {

	for(i=1; i<m_pDocumentList->count(); i++)
	{
	    if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB)
	    {
		pdb_document* pPDB = (pdb_document*)m_pDocumentList->getDoc(i);

		pPDB->setGlobalColmap( COLMAP_SOLID, i );
	    }
	}
    }
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeTYPE()
{
    changeColmapMode( COLMAP_TYPE );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeRESNAME()
{
    changeColmapMode( COLMAP_RESNAME );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeCHAINID()
{
    changeColmapMode( COLMAP_CHAINID );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeSEGID()
{
    changeColmapMode( COLMAP_SEGID );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeSTRUCTURE()
{
    changeColmapMode( COLMAP_STRUCTURE );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void sculptor_window::changeColmapModeMODEL()
{
    changeColmapMode( COLMAP_MODEL );
}

///////////////////////////////////////////////////////////////////////////////
// program management
///////////////////////////////////////////////////////////////////////////////

/**
 * a close event has occured (window close button was pressed)
 */
void sculptor_window::closeEvent( QCloseEvent* ce )
{
    // is there a document that is not saved yet
    bool bNotSaved = false;

    for(int i=1; i < m_pDocumentList->count(); i++)
        if (m_pDocumentList->getDoc(i)->getChanged())
            bNotSaved = true;

    if (bNotSaved)
    {
        if ( !svt_exception::ui()->question("One or more documents were altered and need to be saved. Do you really want to quit Sculptor?") )
        {
            ce->ignore();
            return;
        }
    }

    saveWindowState();
    scriptSave();
    ce->accept();
    exit(0);
}

/**
 * quit sculptor
 */
void sculptor_window::quitNow()
{
    // is there a document that is not saved yet
    bool bNotSaved = false;

    for(int i=1; i < m_pDocumentList->count(); i++)
        if (m_pDocumentList->getDoc(i)->getChanged())
            bNotSaved = true;

    if (bNotSaved)
    {
        if ( !svt_exception::ui()->question("One or more documents were altered and need to be saved. Do you really want to quit Sculptor?") )
            return;
    }

    saveWindowState();
    scriptSave();
    svt_exit(0);
}


/**
 * show about dialog
 */
void sculptor_window::showAboutDlg()
{

  if (m_pAboutDlg == NULL)
      m_pAboutDlg = new dlg_about(this);

    m_pAboutDlg->raise();
    m_pAboutDlg->show();

}

/**
 * show the online help
 */
void sculptor_window::help()
{
    // open a help dialog here. probably use html pages and a Qt html viewer
}

///////////////////////////////////////////////////////////////////////////////
// transform dialog
///////////////////////////////////////////////////////////////////////////////

/**
 * show the transform dialog
 */
void sculptor_window::showTransformDlg()
{
    dlg_transform* pDlg = new dlg_transform(this);

    connect(pDlg->m_pDialTransX, SIGNAL(valueChanged(int)), this, SLOT(dialTransXValueChanged(int)) );
    connect(pDlg->m_pDialTransY, SIGNAL(valueChanged(int)), this, SLOT(dialTransYValueChanged(int)) );
    connect(pDlg->m_pDialTransZ, SIGNAL(valueChanged(int)), this, SLOT(dialTransZValueChanged(int)) );

    connect(pDlg->m_pDialRotX, SIGNAL(valueChanged(int)), this, SLOT(dialRotXValueChanged(int)) );
    connect(pDlg->m_pDialRotY, SIGNAL(valueChanged(int)), this, SLOT(dialRotYValueChanged(int)) );
    connect(pDlg->m_pDialRotZ, SIGNAL(valueChanged(int)), this, SLOT(dialRotZValueChanged(int)) );

    connect(pDlg->m_pButtonClose, SIGNAL(clicked()), this, SLOT(closeTransformDlg()) );
    connect(pDlg->m_pButtonReset, SIGNAL(clicked()), this, SLOT(resetTransformDlg()) );

    // dials and spinboxes update each other...
    connect( pDlg->m_pDialTransY, SIGNAL( valueChanged(int) ), pDlg->m_pSpinBoxTransY, SLOT( setValue(int) ) );
    connect( pDlg->m_pDialTransX, SIGNAL( valueChanged(int) ), pDlg->m_pSpinBoxTransX, SLOT( setValue(int) ) );
    connect( pDlg->m_pDialTransZ, SIGNAL( valueChanged(int) ), pDlg->m_pSpinBoxTransZ, SLOT( setValue(int) ) );
    connect( pDlg->m_pDialRotX, SIGNAL( valueChanged(int) ),   pDlg->m_pSpinBoxRotX, SLOT( setValue(int) ) );
    connect( pDlg->m_pDialRotY, SIGNAL( valueChanged(int) ),   pDlg->m_pSpinBoxRotY, SLOT( setValue(int) ) );
    connect( pDlg->m_pDialRotZ, SIGNAL( valueChanged(int) ),   pDlg->m_pSpinBoxRotZ, SLOT( setValue(int) ) );
    connect( pDlg->m_pSpinBoxTransX, SIGNAL( valueChanged(int) ), pDlg->m_pDialTransX, SLOT( setValue(int) ) );
    connect( pDlg->m_pSpinBoxTransY, SIGNAL( valueChanged(int) ), pDlg->m_pDialTransY, SLOT( setValue(int) ) );
    connect( pDlg->m_pSpinBoxTransZ, SIGNAL( valueChanged(int) ), pDlg->m_pDialTransZ, SLOT( setValue(int) ) );
    connect( pDlg->m_pSpinBoxRotX, SIGNAL( valueChanged(int) ), pDlg->m_pDialRotX, SLOT( setValue(int) ) );
    connect( pDlg->m_pSpinBoxRotY, SIGNAL( valueChanged(int) ), pDlg->m_pDialRotY, SLOT( setValue(int) ) );
    connect( pDlg->m_pSpinBoxRotZ, SIGNAL( valueChanged(int) ), pDlg->m_pDialRotZ, SLOT( setValue(int) ) );


    pDlg->show();

    // init vars
    m_pTransformDlg = pDlg;
    resetTransformDlg();

    // Make sure individual dials get enabled or disabled as needed
    disableInput();
    enableInput();
}

/**
 * the user closed the transform dialog
 */
void sculptor_window::closeTransformDlg()
{
    delete (m_pTransformDlg);
    m_pTransformDlg = NULL;
}

/**
 * reset the dialog
 */
void sculptor_window::resetTransformDlg()
{
    // init vars
    m_iOldTransX = 0;
    m_iOldTransY = 0;
    m_iOldTransZ = 0;
    m_iOldRotX = 0;
    m_iOldRotY = 0;
    m_iOldRotZ = 0;

    // set the widgets
    if (m_pTransformDlg != NULL)
    {
        m_pTransformDlg->m_pDialTransX->setValue(0);
        m_pTransformDlg->m_pDialTransY->setValue(0);
        m_pTransformDlg->m_pDialTransZ->setValue(0);
        m_pTransformDlg->m_pDialRotX->setValue(0);
        m_pTransformDlg->m_pDialRotY->setValue(0);
        m_pTransformDlg->m_pDialRotZ->setValue(0);
    }
}

/**
 * the user has changed the x translation dial. One step is equal to 0.1 Angstroem. Turning the knob
 * from the middle to the total left is 1000 steps, is 100 Angstroem.
 */
void sculptor_window::dialTransXValueChanged(int iVal)
{
    // get camera node
    svt_node* pSceneNode = m_pDocumentList->getDoc(0)->getNode();
    // get document node
    svt_node* pNode = m_pScene->getMovedNode();

    float fNewVal = (float)(0.001f*(iVal - m_iOldTransX));

    if (pNode && pNode->getTransformation() && pSceneNode->getTransformation())
    {
        // camera transformation
        svt_matrix4<Real64> oMat_A( *pSceneNode->getTransformation() );
        oMat_A.setTranslation( 0.0, 0.0, 0.0 );
        oMat_A.invert();
        // in the inverse camera space, we need to apply our current transformation
        svt_matrix4<Real64> oMat_B;
        oMat_B.translate((Real32)(fNewVal), 0.0f, 0.0f);
        if (pNode != pSceneNode)
            oMat_B = oMat_A * oMat_B;
        // OK, now let's add that to our document matrix
        svt_matrix4<Real64> oMat_C( *pNode->getTransformation() );
        oMat_C.translate( oMat_B.translationX(), oMat_B.translationY(), oMat_B.translationZ() );
        oMat_C.copyToMatrix4f( *pNode->getTransformation() );
    }

    m_iOldTransX = iVal;

    if (m_pForceAlgo != NULL) 
	m_pForceAlgo->updateDisplay();

    svt_redraw();
}
/**
 * the user has changed the y translation dial. One step is equal to 0.1 Angstroem. Turning the knob from the middle to the total left is 1000 steps, is 100 Angstroem.
 */
void sculptor_window::dialTransYValueChanged(int iVal)
{
    // get camera node
    svt_node* pSceneNode = m_pDocumentList->getDoc(0)->getNode();
    // get document node
    svt_node* pNode = m_pScene->getMovedNode();

    float fNewVal = (float)(0.001f*(iVal - m_iOldTransY));

    if (pNode && pNode->getTransformation() && pSceneNode->getTransformation())
    {
        // camera transformation
        svt_matrix4<Real64> oMat_A( *pSceneNode->getTransformation() );
        oMat_A.setTranslation( 0.0, 0.0, 0.0 );
        oMat_A.invert();
        // in the inverse camera space, we need to apply our current transformation
        svt_matrix4<Real64> oMat_B;
        oMat_B.translate(0.0f, (Real32)(fNewVal), 0.0f);
        if (pNode != pSceneNode)
            oMat_B = oMat_A * oMat_B;
        // OK, now let's add that to our document matrix
        svt_matrix4<Real64> oMat_C( *pNode->getTransformation() );
        oMat_C.translate( oMat_B.translationX(), oMat_B.translationY(), oMat_B.translationZ() );
        oMat_C.copyToMatrix4f( *pNode->getTransformation() );
    }

    m_iOldTransY = iVal;

    if (m_pForceAlgo != NULL) 
	m_pForceAlgo->updateDisplay();

    svt_redraw();
}
/**
 * the user has changed the z translation dial. One step is equal to 0.00001 Angstroem. Turning the knob from the middle to the total left is 1000 steps, is 0.1 Angstroem.
 */
void sculptor_window::dialTransZValueChanged(int iVal)
{
    // get camera node
    svt_node* pSceneNode = m_pDocumentList->getDoc(0)->getNode();
    // get document node
    svt_node* pNode = m_pScene->getMovedNode();

    float fNewVal = (float)(0.001f*(iVal - m_iOldTransZ));

    if (pNode && pNode->getTransformation() && pSceneNode->getTransformation())
    {
        // camera transformation
        svt_matrix4<Real64> oMat_A( *pSceneNode->getTransformation() );
        oMat_A.setTranslation( 0.0, 0.0, 0.0 );
        oMat_A.invert();
        // in the inverse camera space, we need to apply our current transformation
        svt_matrix4<Real64> oMat_B;
        oMat_B.translate(0.0f, 0.0f, (Real32)(fNewVal));
        if (pNode != pSceneNode)
            oMat_B = oMat_A * oMat_B;
        // OK, now let's add that to our document matrix
        svt_matrix4<Real64> oMat_C( *pNode->getTransformation() );
        oMat_C.translate( oMat_B.translationX(), oMat_B.translationY(), oMat_B.translationZ() );
        oMat_C.copyToMatrix4f( *pNode->getTransformation() );
    }

    m_iOldTransZ = iVal;

    if (m_pForceAlgo != NULL) 
	m_pForceAlgo->updateDisplay();

    svt_redraw();
}
/**
 * the user has changed the x rotation dial.
 */
void sculptor_window::dialRotXValueChanged(int iVal)
{
    // get camera node
    svt_node* pSceneNode = m_pDocumentList->getDoc(0)->getNode();
    // get document node
    svt_node* pNode = m_pScene->getMovedNode();

    float fNewVal = (float)((iVal - m_iOldRotX));
    fNewVal = (fNewVal*PI)/180.0f;

    if (pNode && pNode->getTransformation() && pSceneNode->getTransformation())
    {
        svt_matrix4<Real64> oMat_B( *pNode->getTransformation() );
        // if the moved node is not the camera, we have to undo those rotations first
        if ( pNode != pSceneNode )
        {
            // calculate the inverse of the camera transformation
            svt_matrix4<Real64> oMat_InvRot( *pSceneNode->getTransformation() );
            oMat_InvRot.setTranslation( 0.0, 0.0, 0.0 );
            oMat_InvRot.invert();
            // in the inverse camera space we need to apply the rotation
            svt_matrix4<Real64> oMat_Rot( *pSceneNode->getTransformation() );
            oMat_Rot.setTranslation( 0.0, 0.0, 0.0 );
            oMat_Rot.rotate( 0, fNewVal );
            oMat_Rot = oMat_InvRot * oMat_Rot;
            // and now the final rotation needs to be applied to the document
            svt_vector4<Real64> oTrans = oMat_B.translation();
            oMat_B.translate( -oTrans.x(), -oTrans.y(), -oTrans.z() );
            oMat_B = oMat_Rot * oMat_B;
            oMat_B.translate(  oTrans.x(),  oTrans.y(),  oTrans.z() );
        } else
            oMat_B.rotate( 0, fNewVal );

        // OK, now let's add that to our document matrix
        oMat_B.copyToMatrix4f( *pNode->getTransformation() );
    }

    m_iOldRotX = iVal;

    if (m_pForceAlgo != NULL) 
	m_pForceAlgo->updateDisplay();

    svt_redraw();
}
/**
 * the user has changed the y rotation dial.
 */
void sculptor_window::dialRotYValueChanged(int iVal)
{
    // get camera node
    svt_node* pSceneNode = m_pDocumentList->getDoc(0)->getNode();
    // get document node
    svt_node* pNode = m_pScene->getMovedNode();

    float fNewVal = (float)((iVal - m_iOldRotY));
    fNewVal = (fNewVal*PI)/180.0f;

    if (pNode && pNode->getTransformation() && pSceneNode->getTransformation())
    {
        svt_matrix4<Real64> oMat_B( *pNode->getTransformation() );
        // if the moved node is not the camera, we have to undo those rotations first
        if ( pNode != pSceneNode )
        {
            // calculate the inverse of the camera transformation
            svt_matrix4<Real64> oMat_InvRot( *pSceneNode->getTransformation() );
            oMat_InvRot.setTranslation( 0.0, 0.0, 0.0 );
            oMat_InvRot.invert();
            // in the inverse camera space we need to apply the rotation
            svt_matrix4<Real64> oMat_Rot( *pSceneNode->getTransformation() );
            oMat_Rot.setTranslation( 0.0, 0.0, 0.0 );
            oMat_Rot.rotate( 1, fNewVal );
            oMat_Rot = oMat_InvRot * oMat_Rot;
            // and now the final rotation needs to be applied to the document
            svt_vector4<Real64> oTrans = oMat_B.translation();
            oMat_B.translate( -oTrans.x(), -oTrans.y(), -oTrans.z() );
            oMat_B = oMat_Rot * oMat_B;
            oMat_B.translate(  oTrans.x(),  oTrans.y(),  oTrans.z() );
        } else
            oMat_B.rotate( 1, fNewVal );

        // OK, now let's add that to our document matrix
        oMat_B.copyToMatrix4f( *pNode->getTransformation() );
    }

    m_iOldRotY = iVal;

    if (m_pForceAlgo != NULL) 
	m_pForceAlgo->updateDisplay();

    svt_redraw();
}
/**
 * the user has changed the z rotation dial.
 */
void sculptor_window::dialRotZValueChanged(int iVal)
{
    // get camera node
    svt_node* pSceneNode = m_pDocumentList->getDoc(0)->getNode();
    // get document node
    svt_node* pNode = m_pScene->getMovedNode();

    float fNewVal = (float)((iVal - m_iOldRotZ));
    fNewVal = (fNewVal*PI)/180.0f;

    if (pNode && pNode->getTransformation() && pSceneNode->getTransformation())
    {
        svt_matrix4<Real64> oMat_B( *pNode->getTransformation() );
        // if the moved node is not the camera, we have to undo those rotations first
        if ( pNode != pSceneNode )
        {
            // calculate the inverse of the camera transformation
            svt_matrix4<Real64> oMat_InvRot( *pSceneNode->getTransformation() );
            oMat_InvRot.setTranslation( 0.0, 0.0, 0.0 );
            oMat_InvRot.invert();
            // in the inverse camera space we need to apply the rotation
            svt_matrix4<Real64> oMat_Rot( *pSceneNode->getTransformation() );
            oMat_Rot.setTranslation( 0.0, 0.0, 0.0 );
            oMat_Rot.rotate( 2, fNewVal );
            oMat_Rot = oMat_InvRot * oMat_Rot;
            // and now the final rotation needs to be applied to the document
            svt_vector4<Real64> oTrans = oMat_B.translation();
            oMat_B.translate( -oTrans.x(), -oTrans.y(), -oTrans.z() );
            oMat_B = oMat_Rot * oMat_B;
            oMat_B.translate(  oTrans.x(),  oTrans.y(),  oTrans.z() );
        } else
            oMat_B.rotate( 2, fNewVal );

        // OK, now let's add that to our document matrix
        oMat_B.copyToMatrix4f( *pNode->getTransformation() );
    }

    m_iOldRotZ = iVal;

    if (m_pForceAlgo != NULL) 
	m_pForceAlgo->updateDisplay();

    svt_redraw();
}

/**
 * copy the transformation of the current selected document to all other selected docs
 */
void sculptor_window::copyTransform()
{
    int iDoc = m_pDocumentList->getCurrentIndex();

    if (iDoc>= 0 && iDoc < m_pDocumentList->count())
    {
        sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);
        if (!pDoc)
            return;
        svt_node* pNode = pDoc->getNode();
        if (!pNode)
            return;
        svt_matrix4f* pMatrix = pNode->getTransformation();
        if (!pMatrix)
            return;

        for(int i=0; i< m_pDocumentList->count(); i++)
        {
            window_documentlistitem* pDLI = (*m_pDocumentList)[i];

            if (i != iDoc && pDLI && pDLI->getSelected())
            {
                sculptor_document* pDestDoc = m_pDocumentList->getDoc(i);
                if (!pDestDoc)
                    continue;
                svt_node* pDestNode = pDestDoc->getNode();
                if (!pDestNode)
                    continue;
                svt_matrix4f* pDestMatrix = pDestNode->getTransformation();
                if (!pDestMatrix)
                    continue;

                pDestMatrix->setFromMatrix(*pMatrix);
            }
        }
    }

    svt_redraw();
}

///////////////////////////////////////////////////////////////////////////////
// Docking
///////////////////////////////////////////////////////////////////////////////

/**
 * Sets default probe and target document
 * Usually called after a load() or dropLoad() call to ensure probe 
 * and target are set. Will not have any effect if probe or target
 * are already set.
 */
void sculptor_window::setDefaultProbeTargetDoc()
{
    // Find out if probe doc was set, if not, set it to the first PDB document
    if (m_pProbeDoc == NULL)
    {
        for (int iDoc=1; iDoc < m_pDocumentList->count(); iDoc++)
	{
	    sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);

	    if (pDoc->getType() == SENSITUS_DOC_PDB)
	    {
	        setDockDoc(iDoc);
		break;
	    }
	}
    }

    // Find out if target doc was set, if not, set it to the first Situs document
    if (m_pTargetDoc == NULL)
    {
        for (int iDoc=1; iDoc < m_pDocumentList->count(); iDoc++)
	{
	    sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);

	    if (pDoc->getType() == SENSITUS_DOC_SITUS)
	    {
	        setTargetDoc(iDoc);
		break;
	    }
	}
    }
}

/**
 * Checks if probe and target docs are set
 * If only one PDB and one map are loaded, it automatically sets them
 * as probe and target. Otherwise, it asks the user to set them.
 * \return boolean TRUE if everything is ok, FALSE if user action is required
 */
bool sculptor_window::checkProbeTargetDoc()
{

    // Are both probe and target set?
    if (m_pProbeDoc != NULL && m_pTargetDoc != NULL)
    {
        return true;
    }

    // Is the probe document not set?
    if (m_pProbeDoc == NULL)
    {
	int iPotentialDocIndex = 0;
	int iPotentialDocCount = 0;

        for (int iDoc=1; iDoc < m_pDocumentList->count(); iDoc++)
	{
	    sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);

	    if (pDoc->getType() == SENSITUS_DOC_PDB)
	    {
	        iPotentialDocIndex = iDoc;
		iPotentialDocCount++;
	    }
	}

	if (iPotentialDocCount == 1)
	    setDockDoc(iPotentialDocIndex);
    }

    // Is the target document not set?
    if (m_pTargetDoc == NULL)
    {
	int iPotentialDocIndex = 0;
	int iPotentialDocCount = 0;

        for (int iDoc=1; iDoc < m_pDocumentList->count(); iDoc++)
	{
	    sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);

	    if (pDoc->getType() == SENSITUS_DOC_SITUS)
	    {
	        iPotentialDocIndex = iDoc;
		iPotentialDocCount++;
	    }
	}

	if (iPotentialDocCount == 1)
	    setTargetDoc(iPotentialDocIndex);
    }

    // Now, are both probe and target set?
    if (m_pProbeDoc == NULL && m_pTargetDoc == NULL)
    {
        QMessageBox::warning(this,
                             QString("Target/Probe Documents Needed"),
                             QString("Please choose a target map, and a structure to dock into that map"));
        statusBar()->showMessage( "choose target and docking document", 2000 );
        return false; 
    } 

    // Be specific about what is still missing
    if (m_pProbeDoc == NULL)
    {
        QMessageBox::warning(this,
                             QString("Target/Probe Documents Needed"),
                             QString("Please choose a probe structure to dock into the target map"));
        statusBar()->showMessage( "choose probe document", 2000 );
	return false; 
    } 

    if (m_pTargetDoc == NULL)
    {
        QMessageBox::warning(this,
                             QString("Target/Probe Documents Needed"),
                             QString("Please choose a target map for the docking"));
        statusBar()->showMessage( "choose docking target", 2000 );
	return false; 
    } 

    // Everything is ok now
    return true; 

}

/**
 * a new document should be used as target document for the fitting
 * \param iDoc the index of the document
 */
void sculptor_window::setTargetDoc(int iDoc)
{
    window_documentlistitem* pDLI = (*m_pDocumentList)[iDoc];

    // ok, now check some things
    if (iDoc>= 0 && iDoc < m_pDocumentList->count() && pDLI != NULL)
    {
        sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);

        // is the same doc already the dock doc?
        if (pDoc && m_pProbeDoc == pDoc)
        {
            QMessageBox::warning(this,
                                 QString("Set Target Document"),
                                 QString("A document cannot be docked into itself. Choose a different target document."));
            statusBar()->showMessage( "choose another target document", 2000 );
            return;
        }

        // is the document something else than a situs doc? Then something is wrong (target must be a situs doc)
        if (pDoc->getType() != SENSITUS_DOC_SITUS)
        {
            QMessageBox::warning(this,
                                 QString("Set Target Document"),
                                 QString("The target document must be a map!"));
            statusBar()->showMessage( "target doc must be a map", 2000 );
            return;
        }

        m_pTargetDoc = pDoc;
        m_pTargetDoc->setCodebookColor( 0.0, 1.0, 0.0 );

        // now update all the force icons
	updateForceIcons();

        // was this document the moved doc?
        if (pDLI->getMoved())
        {
            setMovedDoc(0);
            return;
        }
    }
}

/**
 * the current selected document should be used as target document for the fitting
 */
void sculptor_window::setTargetDoc()
{
    int iDoc = m_pDocumentList->getCurrentIndex();

    if (iDoc == -1)
	return;

    setTargetDoc(iDoc);
}

/**
 * which document is the target doc?
 * \return pointer to sculptor_document (NULL if no target doc was selected)
 */
sculptor_document* sculptor_window::getTargetDoc()
{
    return m_pTargetDoc;
}

/**
 * which document is the probe doc?
 * \return pointer to sculptor_document (NULL if no probe doc was selected)
 */
sculptor_document* sculptor_window::getProbeDoc()
{
    return m_pProbeDoc;
}

/**
 * toggle forces
 */
void sculptor_window::toggleForces()
{
    disableInput();
    enableInput();
};

/**
 * toggle steric clashes
 */
void sculptor_window::toggleStericClashing()
{
    setStericClashing( m_pStericClashing->isChecked() );
};

/**
 * Get force algo
 */
sculptor_force_algo* sculptor_window::getForceAlgo()
{
    return m_pForceAlgo;
}

/**
 * Enable move or force tool
 */
void sculptor_window::enableInput()
{
    int i;
    int iMovedDoc = 0;
    window_documentlistitem* pDLI; 

    svt_move_tool*      pMoveTool  = m_pScene->getMoveTool();
    svt_adv_force_tool* pForceTool = m_pScene->getForceTool();

    // Figure out which document is being moved

    for(i=0;i<(int)m_pDocumentList->count();i++)
    {
        pDLI = (*m_pDocumentList)[i];
	if ( pDLI->getMoved() )
	    break;
    }

    iMovedDoc = i;

    // Bail if we didn't find a document to move

    if (iMovedDoc >= (int)m_pDocumentList->count()) return;

    // Now select the right tool and perform the appropriate actions
    
    sculptor_document* pDoc = m_pDocumentList->getDoc(iMovedDoc);

    if ((iMovedDoc == 0) || (pDoc->getType() == SENSITUS_DOC_CLIP) || (pDoc->getType() == SENSITUS_DOC_ERASER) || (pDoc->getType() == SENSITUS_DOC_MARKER) )
    {
	// We are moving the scene or a clipping plane or the eraser tool or the marker tool, use the move tool
	
        // Enable move tool
        pMoveTool->setActive(true);

	if (iMovedDoc == 0                                      &&
	    m_pScene->getTopNode()->getTransformation() != NULL &&
	    m_pDocumentList->count() > 1                         )
	{
	    // Moving the scene
	    
	    sculptor_document* pDocOrg = m_pDocumentList->getDoc(1);

	    if (pDocOrg)
	    {
		svt_vector4f oVec = pDocOrg->getRealPos();
		oVec.multScalar( 1.0E8f );
		svt_vector4<Real32> oOrigin( oVec.x(), oVec.y(), oVec.z() );
		m_pScene->setMovedNode(pDoc->getNode(), oOrigin);
	    }

	} else {

	    // Moving a clip plane or eraser tool or the marker tool
	    //
    
	    m_pScene->setMovedNode(pDoc->getNode());
	}

	// Enable all dials in the transformation dialog
	if (m_pTransformDlg != NULL)
	{
	    m_pTransformDlg->m_pDialTransX->setEnabled(true);
	    m_pTransformDlg->m_pDialTransY->setEnabled(true);
	    m_pTransformDlg->m_pDialTransZ->setEnabled(true);
	    m_pTransformDlg->m_pDialRotX->setEnabled(true);
	    m_pTransformDlg->m_pDialRotY->setEnabled(true);
	    m_pTransformDlg->m_pDialRotZ->setEnabled(true);

	    m_pTransformDlg->m_pSpinBoxTransX->setEnabled(true);
	    m_pTransformDlg->m_pSpinBoxTransY->setEnabled(true);
	    m_pTransformDlg->m_pSpinBoxTransZ->setEnabled(true);
	    m_pTransformDlg->m_pSpinBoxRotX->setEnabled(true);
	    m_pTransformDlg->m_pSpinBoxRotY->setEnabled(true);
	    m_pTransformDlg->m_pSpinBoxRotZ->setEnabled(true);
	}

    } else {

	// Trying to move some other document
	
        // Decide between mouse and haptic device

        if (m_pForces->isChecked())
	    pForceTool->switchToLIVE();
	else
	    pForceTool->switchToMouse();

        // Enable force tool
        pForceTool->setActive(true);
        pForceTool->setMoveNode(pDoc->getNode());

	// Make it the probe
	
	if ( (m_pProbeDoc == NULL) || 
	     ((m_pProbeDoc != NULL) && ((int) m_pProbeDoc->getDocID() != iMovedDoc)) )
	    setDockDoc(iMovedDoc);

	// Set to move
	m_pScene->setMovedNode(m_pProbeDoc->getNode());

	// Start thread manually or automatically?
	if (strcmp(pForceTool->getDevice()->getName(),"mouse") != 0)
	{
	    pForceTool->setThreadAutoStart(false);
	    pForceTool->initThread();
	    m_pForces->setChecked( TRUE );
	}
	else
	{
	    pForceTool->setThreadAutoStart(true);
	    pForceTool->exitThread();
	    m_pForces->setChecked( FALSE );
	}

	// Initialize force_algo
	if (m_pForceAlgo != NULL) 
	    m_pForceAlgo->initialize(m_pProbeDoc, m_pTargetDoc);

	// Force display update
	svt_redraw();

	// Enable select dials in the transformation dialog
	if (m_pTransformDlg != NULL)
	{
	    m_pTransformDlg->m_pDialTransX->setEnabled(true);
	    m_pTransformDlg->m_pDialTransY->setEnabled(true);
	    m_pTransformDlg->m_pDialTransZ->setEnabled(true);

	    m_pTransformDlg->m_pSpinBoxTransX->setEnabled(true);
	    m_pTransformDlg->m_pSpinBoxTransY->setEnabled(true);
	    m_pTransformDlg->m_pSpinBoxTransZ->setEnabled(true);

	    if (    (m_pProbeDoc != NULL)
		 && ((int) m_pProbeDoc->getDocID() != iMovedDoc)
		 && (!m_pProbeDoc->getIGDActive())                    )
	    {
		m_pTransformDlg->m_pDialRotX->setEnabled(true);
		m_pTransformDlg->m_pDialRotY->setEnabled(true);
		m_pTransformDlg->m_pDialRotZ->setEnabled(true);
		m_pTransformDlg->m_pSpinBoxRotX->setEnabled(true);
		m_pTransformDlg->m_pSpinBoxRotY->setEnabled(true);
		m_pTransformDlg->m_pSpinBoxRotZ->setEnabled(true);

	    } else {

		// QColor oColor;
		// oColor.setRgb(240, 180, 180);
    
		m_pTransformDlg->m_pDialRotX->setEnabled(false);
		m_pTransformDlg->m_pDialRotY->setEnabled(false);
		m_pTransformDlg->m_pDialRotZ->setEnabled(false);
		m_pTransformDlg->m_pSpinBoxRotX->setEnabled(false);
		m_pTransformDlg->m_pSpinBoxRotY->setEnabled(false);
		m_pTransformDlg->m_pSpinBoxRotZ->setEnabled(false);
		// m_pTransformDlg->m_pDialRotX->setPaletteBackgroundColor(oColor);
		// m_pTransformDlg->m_pDialRotY->setPaletteBackgroundColor(oColor);
		// m_pTransformDlg->m_pDialRotZ->setPaletteBackgroundColor(oColor);
	    }
	}

	// If we are using a haptic device, hook the mouse up to moving the scene
	
	if ((strcmp(pForceTool->getDevice()->getName(),"mouse") != 0))
	{

	    sculptor_document* pDocOrg = m_pDocumentList->getDoc(1);

	    if (pDocOrg)
	    {
		svt_vector4f oVec = pDocOrg->getRealPos();
		oVec.multScalar( 1.0E8f );
		svt_vector4<Real32> oOrigin( oVec.x(), oVec.y(), oVec.z() );
		pMoveTool->setPivotPoint( oOrigin );
		pMoveTool->setMoveNode(m_pDocumentList->getDoc(0)->getNode());
		pMoveTool->setActive(true);
	    }
	}
    }

}

/**
 * Disable move or force tool
 */
void sculptor_window::disableInput()
{
    // Disable both tools
    m_pScene->getMoveTool()->setActive(false);
    m_pScene->getForceTool()->setActive(false);
    m_pScene->getForceTool()->exitThread();
    m_pForces->setChecked( false );

    // Turn off force algo
    if (m_pForceAlgo != NULL) m_pForceAlgo->deactivate();

    // Disable transformation dialog
    if (m_pTransformDlg != NULL)
    {
	m_pTransformDlg->m_pDialTransX->setEnabled(false);
	m_pTransformDlg->m_pDialTransY->setEnabled(false);
	m_pTransformDlg->m_pDialTransZ->setEnabled(false);
	m_pTransformDlg->m_pDialRotX->setEnabled(false);
	m_pTransformDlg->m_pDialRotY->setEnabled(false);
	m_pTransformDlg->m_pDialRotZ->setEnabled(false);

	m_pTransformDlg->m_pSpinBoxTransX->setEnabled(true);
	m_pTransformDlg->m_pSpinBoxTransY->setEnabled(true);
	m_pTransformDlg->m_pSpinBoxTransZ->setEnabled(true);
	m_pTransformDlg->m_pSpinBoxRotX->setEnabled(false);
	m_pTransformDlg->m_pSpinBoxRotY->setEnabled(false);
	m_pTransformDlg->m_pSpinBoxRotZ->setEnabled(false);

	// m_pTransformDlg->m_pDialTransX->setPalette(qApp->palette(m_pTransformDlg->m_pDialTransX));
	// m_pTransformDlg->m_pDialTransY->setPalette(qApp->palette(m_pTransformDlg->m_pDialTransY));
	// m_pTransformDlg->m_pDialTransZ->setPalette(qApp->palette(m_pTransformDlg->m_pDialTransZ));
	// m_pTransformDlg->m_pDialRotX->setPalette(qApp->palette(m_pTransformDlg->m_pDialRotX));
	// m_pTransformDlg->m_pDialRotY->setPalette(qApp->palette(m_pTransformDlg->m_pDialRotY));
	// m_pTransformDlg->m_pDialRotZ->setPalette(qApp->palette(m_pTransformDlg->m_pDialRotZ));
    }

    // Clear moved node pointers
    m_pScene->getMoveTool()->setMoveNode(NULL);
    m_pScene->getForceTool()->setMoveNode(NULL);
    m_pScene->setMovedNode(NULL);
}

/**
 * Set steric clashes active/deactive
 */
void sculptor_window::setStericClashing(bool bStericClashing)
{
    if (bStericClashing)
    {
	if (m_pProbeDoc == NULL)
	{
            QMessageBox::critical(this,
                                  QString("Steric Clashing"),
                                  QString("Please choose a probe molecule for the steric clashing."));
	    return;
	}

	if ( !isStericClashingCodebookValid(m_pProbeDoc) )
	    return;

	disableInput();

	updateStericClashingData();
	m_pProbeDoc->setStericSpheresVisible(true);

	enableInput();
    }
    else
    {
	if (m_pProbeDoc != NULL)
	    m_pProbeDoc->setStericSpheresVisible(false);
    }

    return;
};

/**
 * Check for or generate codebook to use for steric clashing
 */
bool sculptor_window::isStericClashingCodebookValid(sculptor_document* pDoc)
{
    if (pDoc->getCodebookSC().size() > 0) return true;

    ((pdb_document*) pDoc)->generateStericClashingCodebook();

    return true;

}

/**
 * update the steric clashing data
 */
void sculptor_window::updateStericClashingData()
{
    int i;

    // Check if we need to do anything
    if ( !getSolutionSetChanged() || !getStericClashing() )
	return;

    m_bStericClashingDataValid = false;

    // Check for empty solution list
    bool bNoSolutions = true;
    for ( i=0; i<m_pDocumentList->count(); i++ )
    {
        sculptor_document* pCurrDoc = m_pDocumentList->getDoc(i);

	if ( (pCurrDoc->getType() == SENSITUS_DOC_PDB) && pCurrDoc->getSolution() )
	    bNoSolutions = false;
    }

    if (bNoSolutions)
    {
	// this keeps force algo from actually using it
	m_bStericClashingDataValid = false;
	setSolutionSetChanged(false);
	return;
    }

    // Iterate over all solution documents to figure out what size the clashing map needs to be
    
    double dMinX =  99999999.9;
    double dMaxX = -99999999.9;
    double dMinY =  99999999.9;
    double dMaxY = -99999999.9;
    double dMinZ =  99999999.9;
    double dMaxZ = -99999999.9;

    for ( i=0; i<m_pDocumentList->count(); i++ )
    {
        sculptor_document* pCurrDoc = m_pDocumentList->getDoc(i);

	if ( (pCurrDoc->getType() == SENSITUS_DOC_PDB) && pCurrDoc->getSolution() )
	{
	    // get the original min and max extents
	    double dMinXI = ((svt_pdb *) pCurrDoc->getNode())->getMinXCoord();
	    double dMaxXI = ((svt_pdb *) pCurrDoc->getNode())->getMaxXCoord();
	    double dMinYI = ((svt_pdb *) pCurrDoc->getNode())->getMinYCoord();
	    double dMaxYI = ((svt_pdb *) pCurrDoc->getNode())->getMaxYCoord();
	    double dMinZI = ((svt_pdb *) pCurrDoc->getNode())->getMinZCoord();
	    double dMaxZI = ((svt_pdb *) pCurrDoc->getNode())->getMaxZCoord();

	    // calculate the center
	    double dCentX = (dMaxXI + dMinXI) / 2.0;
	    double dCentY = (dMaxYI + dMinYI) / 2.0;
	    double dCentZ = (dMaxZI + dMinZI) / 2.0;

	    // find the radius of the enclosing sphere
            double dRadius = 0.5 *
	                     sqrt(
				   ((dMaxXI - dMinXI) * (dMaxXI - dMinXI)) +
				   ((dMaxYI - dMinYI) * (dMaxYI - dMinYI)) +
				   ((dMaxZI - dMinZI) * (dMaxZI - dMinZI))
			         );

	    // apply the transformation matrix to the center vector
	    svt_vector4<Real64> oVecCent( dCentX, dCentY, dCentZ );
	    svt_matrix4<Real64> oMat( *( ((svt_pdb *) pCurrDoc->getNode())->getTransformation() ) );
	 
	    oVecCent *= 1.0E-2;
	    oVecCent.w( 1.0f );
	    oVecCent = oMat * oVecCent;
	    oVecCent *= 1.0E2;

	    dCentX = oVecCent.x();
	    dCentY = oVecCent.y();
	    dCentZ = oVecCent.z();

	    // adjust boundaries of steric clashing volume based on center and radius
	    if ((dMaxXI + dCentX + dRadius) > dMaxX) dMaxX = dMaxXI + dCentX + dRadius;
	    if ((dMinXI + dCentX - dRadius) < dMinX) dMinX = dMinXI + dCentX - dRadius;
	    if ((dMaxYI + dCentY + dRadius) > dMaxY) dMaxY = dMaxYI + dCentY + dRadius;
	    if ((dMinYI + dCentY - dRadius) < dMinY) dMinY = dMinYI + dCentY - dRadius;
	    if ((dMaxZI + dCentZ + dRadius) > dMaxZ) dMaxZ = dMaxZI + dCentZ + dRadius;
	    if ((dMinZI + dCentZ - dRadius) < dMinZ) dMinZ = dMinZI + dCentZ - dRadius;
	}
    }

    // Now allocate a new steric clashing map with the above boundaries
    // and a voxel size of 3

    const double dVoxelSize = 3.0;

    unsigned int uSizeX = (int) ceil((dMaxX - dMinX) / dVoxelSize);
    unsigned int uSizeY = (int) ceil((dMaxY - dMinY) / dVoxelSize);
    unsigned int uSizeZ = (int) ceil((dMaxZ - dMinZ) / dVoxelSize);

    delete m_pStericClashingData;
    m_pStericClashingData = new svt_volume<Real64>(uSizeX, uSizeY, uSizeZ, 0.0);
    m_pStericClashingData->setGrid(dMinX, dMinY, dMinZ);
    m_pStericClashingData->setWidth(dVoxelSize);

    // Iterate over all documents and project all solutions
    for ( i=0; i<m_pDocumentList->count(); i++ )
    {

        sculptor_document* pCurrDoc = m_pDocumentList->getDoc(i);

	if ( (pCurrDoc->getType() == SENSITUS_DOC_PDB) && pCurrDoc->getSolution() )
	{
	    // Project document
	    svt_matrix4<Real64> oCurrTransform;
            oCurrTransform = *pCurrDoc->getNode()->getTransformation();
            svt_vector4<Real64> oCurrTranslation = 1.0e2 * oCurrTransform.translation();
            oCurrTransform.setTranslation( oCurrTranslation );

	    ((pdb_document *) pCurrDoc)->getPointCloud().projectDistance(m_pStericClashingData,
	                                                                 oCurrTransform,
									 /* Backbone only */ true);
	}
    }

    // Mark data valid
    setSolutionSetChanged(false);
    m_bStericClashingDataValid = true;

    return;
}

/**
 * get steric clashing data
 */
svt_volume<Real64>* sculptor_window::getStericClashingData()
{
    if (m_bStericClashingDataValid)
	return m_pStericClashingData;
    else
        return NULL;
}

/**
 * a new document should be used for the fitting process (the document one will move around)
 * \param iDoc the index of the document
 */
void sculptor_window::setDockDoc(int iDoc)
{
    window_documentlistitem* pDLI = (*m_pDocumentList)[iDoc];

    // ok, now check some things
    if ( iDoc>= 0 && iDoc < m_pDocumentList->count() && pDLI != NULL && pDLI->getDoc() != NULL)
    {
        sculptor_document* pDoc = pDLI->getDoc();

        // is the same doc already the target doc?
        if (pDoc && m_pTargetDoc == pDoc)
        {
            QMessageBox::warning(this,
                                 QString("Setting Docking Document"),
                                 QString("A document cannot be docked into itself!"));
            statusBar()->showMessage( "choose another docking document", 2000 );
            return;
        }

        // is the document something else than a pdb doc?
        if ( pDoc->getType() != SENSITUS_DOC_PDB )
        {
            QMessageBox::warning(this,
                                 QString("Setting Docking Document"),
                                 QString("Docking document must be a PDB!"));
            statusBar()->showMessage( "doc must be a PDB", 2000 );
            return;
        }

	// Ensure we have a steric clashing codebook
	if ( !isStericClashingCodebookValid(pDoc) )
	    return;

	// save the colorscheme of the probe
	bool bSolutionColMapValid = false;
	bool bProbeColMapValid = false;
	int iSolutionColMapMode = 0, iSolutionColMapColor = 0;
	int iProbeColMapMode    = 0, iProbeColMapColor    = 0;

	if ( pDoc->getSolution() )
	{
	    iSolutionColMapMode  = ((pdb_document *) pDoc)->getGlobalColmapMode();
	    iSolutionColMapColor = ((pdb_document *) pDoc)->getGlobalColmapColor();
	    bSolutionColMapValid = true;
	}

	// Turn off steric clashing spheres of the old probe document
	if (m_pProbeDoc != NULL) 
	    m_pProbeDoc->setStericSpheresVisible(false);

	// make the old probe a solution
	if (m_pProbeDoc != NULL && pDoc->getSolution())
	{
	    m_pProbeDoc->setSolution(true);

	    if (m_pProbeDoc->getType() == SENSITUS_DOC_PDB)
	    {
		iProbeColMapMode  = ((pdb_document *) m_pProbeDoc)->getGlobalColmapMode();
		iProbeColMapColor = ((pdb_document *) m_pProbeDoc)->getGlobalColmapColor();
		bProbeColMapValid = true;

		if (m_bAutoAdjustProbeSolutionColor)
		{
		    if (bSolutionColMapValid)
			((pdb_document *) m_pProbeDoc)->setGlobalColmap( iSolutionColMapMode,
									 iSolutionColMapColor);
		    else
			((pdb_document *) m_pProbeDoc)->setGlobalColmap( COLMAP_SOLID, 2);
		}
	    }
	}

	m_pProbeDoc = pDoc;
	m_pProbeDoc->setCodebookColor( 1.0, 0.0, 0.0 );

	// apply 'probe' colorscheme to new probe
	if (m_pProbeDoc->getType() == SENSITUS_DOC_PDB)
	{
	    if (m_bAutoAdjustProbeSolutionColor && bSolutionColMapValid)
		((pdb_document *) m_pProbeDoc)->setGlobalColmap( iProbeColMapMode, iProbeColMapColor);
	}

	// make sure the new probe does not count as a solution
	pDoc->setSolution(false);

	// mark solution set changed
	setSolutionSetChanged(true);

	// update steric clashing data
	updateStericClashingData();

	// find the corresponding target doc if we are using exhaustive search data
	if ( m_pProbeDoc->getEliDataLoaded() ||
	    (m_pProbeDoc->getOrigProbeDoc() && m_pProbeDoc->getOrigProbeDoc()->getEliDataLoaded()))

	    for (int iTargetDoc=1; iTargetDoc < m_pDocumentList->count(); iTargetDoc++)
		if ( (m_pDocumentList->getDoc(iTargetDoc)->getDocID() == m_pProbeDoc->getDocID()) &&
		     (m_pDocumentList->getDocType(iTargetDoc) == SENSITUS_DOC_SITUS      )    )
		    setTargetDoc(iTargetDoc);

        // now update all the force icons
	updateForceIcons();

    }

}
/**
 * the currently selected document should be used for the fitting process (the document one will move around)
 */
void sculptor_window::setDockDoc()
{
    int iDoc = m_pDocumentList->getCurrentIndex();

    if (iDoc == -1)
	return;

    setDockDoc(iDoc);
}

/**
 * is force feedback turned on?
 */
bool sculptor_window::isForceOn()
{
    return m_pForces->isChecked();
}
/**
 * is steric clashing turned on?
 */
bool sculptor_window::getStericClashing()
{
    return m_pStericClashing->isChecked();
}

/**
 * a new document should be used as a solution candidate
 * \param iDoc the index of the document
 */
void sculptor_window::setSolutionDoc(int iDoc)
{
    window_documentlistitem* pDLI = (*m_pDocumentList)[iDoc];

    // ok, now check some things
    if (iDoc>= 0 && iDoc < m_pDocumentList->count() && pDLI != NULL)
    {
        sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);

        // is the same doc already the probe doc?
	// Then just make a new solution
        if (pDoc && m_pProbeDoc == pDoc)
        {
	    makeNewSolution();
            return;
        }

        // is the same doc already the target doc?
        if (pDoc && m_pTargetDoc == pDoc)
        {
            QMessageBox::warning(this,
                                 QString("Setting Solution Document"),
                                 QString("The target document cannot become a solution. Please choose only PDB documents as solution."));
            statusBar()->showMessage( "choose another solution document", 2000 );
            return;
        }

        // is the document something else than a pdb doc? Then something is wrong (solution must be a PDB doc)
        if (pDoc->getType() != SENSITUS_DOC_PDB)
        {
            QMessageBox::warning(this,
                                 QString("Setting Solution Document"),
                                 QString("The document must be a pdb. Please choose only PDB documents as solution."));
            statusBar()->showMessage( "document must be a pdb", 2000 );
            return;
        }

	// toggle the document status
        pDoc->setSolution( !pDoc->getSolution() );

	// mark solution set as dirty
	setSolutionSetChanged(true);
    }

    return;
}
/**
 * the currently selected document should set as a solution document
 */
void sculptor_window::setSolutionDoc()
{

    disableInput();

    for(int i=1;i < m_pDocumentList->count(); i++)
    {
	if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	    setSolutionDoc(i);
    }

    if (getSolutionSetChanged())
    {
	// update steric clashing data
	updateStericClashingData();

	// now update all the force icons
	updateForceIcons();

	m_pDocumentList->repaint();
    }

    enableInput();
}

/**
 * Set solution set changed
 */
void sculptor_window::setSolutionSetChanged(bool bSolutionSetChanged)
{
    m_bSolutionSetChanged = bSolutionSetChanged;

    return;
}
/**
 * Is solution set changed
 */
bool sculptor_window::getSolutionSetChanged()
{
    return m_bSolutionSetChanged;
}

/**
 * make a new solution from the current position of the probe map
 */
void sculptor_window::makeNewSolution()
{
    if (m_pProbeDoc == NULL)
	return;

    disableInput();

    // Copy the probe PDB document
    pdb_document* pNewDoc = new pdb_document(m_pScene->getDataTopNode(), (pdb_document&)(*m_pProbeDoc));

    // Copy the code book
    pNewDoc->setCodebook(m_pProbeDoc->getCodebook());
    pNewDoc->setCV_Visible( false );

    // Copy the steric clashing code book
    pNewDoc->setCodebookSC(m_pProbeDoc->getCodebookSC());
    pNewDoc->setStericSpheresVisible( false );

    // Change coloring of solution (green in both high and low contrast palettes)
    pNewDoc->setGlobalColmap( COLMAP_SOLID, 2);

    // Set the document ID to the one of the probe
    pNewDoc->setDocID(m_pProbeDoc->getDocID());

    // Set the pointer to the original probe which might contain Eliquos data
    if (m_pProbeDoc->getOrigProbeDoc())
        pNewDoc->setOrigProbeDoc(m_pProbeDoc->getOrigProbeDoc());
    else
        pNewDoc->setOrigProbeDoc(m_pProbeDoc);

    // Copy IGD state from probe document
    pNewDoc->setIGDActive( m_pProbeDoc->getIGDActive() );

    // Make it a solution
    pNewDoc->setSolution(true);
    setSolutionSetChanged(true);

    // Mark it as a copy of the probe
    pNewDoc->setCopy(true);

    // Add it to the document list
    addDocSimple( (sculptor_document*)pNewDoc, QString("Solution Candidate") );

    // now update all the force icons
    updateForceIcons();

    enableInput();

    return;
}

/**
 * update all force icons
 */
void sculptor_window::updateForceIcons()
{
    int i;
    for(i=0;i<m_pDocumentList->count();i++)
    {
        sculptor_document* pDoc = m_pDocumentList->getDoc(i);
        window_documentlistitem* pDLI = (*m_pDocumentList)[i];

	pDLI->setTarget(false);
	pDLI->setDock(false);
	pDLI->setSolution(false);
	if (m_pProbeDoc == m_pDocumentList->getDoc(i))
	    pDLI->setDock(true);
	if (m_pTargetDoc == m_pDocumentList->getDoc(i))
	    pDLI->setTarget(true);
	if (pDoc->getSolution())
	    pDLI->setSolution(true);
    }
}

/**
 * feature-based docking docking
 */
void sculptor_window::featureDock()
{

    if ( !checkProbeTargetDoc() ) return;

    svt_point_cloud< svt_vector4<Real64> > oCV_Mono = m_pProbeDoc->getCodebook();
    svt_point_cloud< svt_vector4<Real64> > oCV_Olig = m_pTargetDoc->getCodebook();

    if (oCV_Mono.size()==0)
    {
        svt_exception::ui()->info("Please generate the feature vectors of structure!");
        return;
    }
    
    if (oCV_Olig.size()==0)
    {
        svt_exception::ui()->info("Please generate the feature vectors of target map!");
        return;
    }
    
    
    m_pFeatureDockingDlg = new dlg_featuredocking( this );
    unsigned int i;

    if (m_pFeatureDockingDlg->exec() == QDialog::Accepted)
    {	    
	// calculate transformation of the objects
	svt_matrix4<Real64> oMonoMat;
	if ( m_pProbeDoc->getNode() != NULL)
	    if ( m_pProbeDoc->getNode()->getTransformation() != NULL )
		oMonoMat.set( *m_pProbeDoc->getNode()->getTransformation() );

	for(i=0; i<oCV_Mono.size(); i++)
	    oCV_Mono[i] = oCV_Mono[i] * 1.0E-2;

	svt_matrix4<Real64> oOligMat;
	if ( m_pTargetDoc->getNode() != NULL)
	    if ( m_pTargetDoc->getNode()->getTransformation() != NULL )
		oOligMat.set( *m_pTargetDoc->getNode()->getTransformation() );

	svt_vector4f oVec = m_pTargetDoc->getRealPos();

	for(i=0; i<oCV_Olig.size(); i++)
	    oCV_Olig[i] = oCV_Olig[i] * 1.0E-2;

	oCV_Olig = oOligMat * oCV_Olig;

	vector< svt_matchResult<Real64> > aMatches;
	vector< svt_matchResult<Real64> > aTmpMatches;

	// set parameter for matching algorithm
	oCV_Mono.setLambda( m_pFeatureDockingDlg->getLambda() * 1.0E-2);
	oCV_Mono.setGamma( m_pFeatureDockingDlg->getGamma() * 1.0E-2);
	oCV_Mono.setWildcards( m_pFeatureDockingDlg->getWildcards() );
        oCV_Mono.setSkipPenalty( m_pFeatureDockingDlg->getSkipPenalty() * 1.0E-2);
	oCV_Mono.setRuns( 3 );
	oCV_Mono.setSimple( false );
	oCV_Mono.setZoneSize( m_pFeatureDockingDlg->getMatchingZone() );
        oCV_Mono.setUnique( 0.01 );

	// standard
	aTmpMatches.clear();
	oCV_Mono.setNextPointCOA( false );
        oCV_Mono.match( oCV_Olig, aTmpMatches );
        if ( aTmpMatches.size() > 0 )
        {
            cleanResults( oCV_Mono, oCV_Olig, aTmpMatches, 5000 );
            for(i=0; i<aTmpMatches.size(); i++)
                aMatches.push_back( aTmpMatches[i] );
        }

	aTmpMatches.clear();
	oCV_Mono.setNextPointCOA( true );
        oCV_Mono.match( oCV_Olig, aTmpMatches );
        if (aTmpMatches.size() > 0 )
        {
            cleanResults( oCV_Mono, oCV_Olig, aTmpMatches, 5000 );
            for(i=0; i<aTmpMatches.size(); i++)
                aMatches.push_back( aTmpMatches[i] );
        }

        if (aMatches.size() == 0)
        {
            svt_exception::ui()->info("The matching was not successful, please increase the search tolerance\nand/or the zone width and try again.");
            return;
        }

	cleanResults( oCV_Mono, oCV_Olig, aMatches, m_pFeatureDockingDlg->getMaxSolutions() );

	char pName[256];
	for(i=0; i<aMatches.size(); i++)
        {
	    svt_matrix4< Real64 > oMat = aMatches[i].getMatrix();
            vector<int> pMatch = aMatches[i].getMatch();

            sprintf( pName, "feature vector fit %i", i );

	    m_pProbeDoc->addTransform( pName, oMat, &pMatch, m_pProbeDoc->getCodebookID(), m_pTargetDoc->getCodebookID() );
	}

        // get current document and show property dialog
        sculptor_document* pDoc = m_pProbeDoc;

        if ( pDoc->getType() == SENSITUS_DOC_PDB )
        {
            // show the prop dialog first, so it gets created if it doesn't exist
            m_pDocumentList->showPropDialog( pDoc->getDLItem()->getIndex() );
            // then, pull the Docking tab to the front
            ((pdb_document*)pDoc)->showTransList();
        }
    }

}

/**
 * show the Flexing dialog
 */
void sculptor_window::showFlexingDlg()
{
    sculptor_document *oFirst = NULL;
    sculptor_document *oSecond = NULL;
    
    getDocsFromSelection(&oFirst, &oSecond);

    //
    // Check the validity of the selection
    //
    
    //did select something
    if (oFirst==NULL || oSecond==NULL)
    	return;
    
    // are the codebookvectors already computed
    if ((oFirst->getCodebook()).size()==0)
    {
        svt_exception::ui()->info("Please compute the feature points of the probe molecule!");
	return;
    }
    
    // are the codebookvectors already computed
    if ((oSecond->getCodebook()).size()==0)
    {
        switch (oSecond->getType())
        {
            case SENSITUS_DOC_SITUS:
                 svt_exception::ui()->info("Please compute the feature points of the target map!");
                break;
            case SENSITUS_DOC_PDB:
       	        svt_exception::ui()->info("Please compute the feature points of the second probe molecule!");
                break;
            default:
                svt_exception::ui()->info("Please compute the feature points!");
        }
	return;
    }
    
    bool bUseMatching=false;
    
    if (oFirst->getTransformCount()>0)
    {
        vector<int> pMatch = *(oFirst->getCurrentMatch());

        long int iMatchTargetID = oFirst->getCurrentMatchTargetID();
        long int iMatchProbeID  = oFirst->getCurrentMatchProbeID();
        long int iThisTargetID  = oSecond->getCodebookID();
        long int iThisProbeID   = oFirst->getCodebookID();

        if (pMatch.size()>0 && iMatchTargetID == iThisTargetID && iMatchProbeID == iThisProbeID)
            bUseMatching = true;    //the matching will be used
        else
            bUseMatching = false;
    }else
        bUseMatching = false;

    
    if (!bUseMatching && (oFirst->getCodebook()).size()!=(oSecond->getCodebook()).size())
        svt_exception::ui()->info("The probe and target have different numbers of feature vectors! Please check\nthe feature vectors as they might cause faulty results in the dockingflexing.");

   // error checking and determine dock and terget documents     
    if (m_pFlexFittingDlg == NULL)
        m_pFlexFittingDlg = new dlg_flexing( this );

    m_pFlexFittingDlg->setDocs(oFirst,oSecond);
		     
    //show dialog
    m_pFlexFittingDlg->show();
    m_pFlexFittingDlg->raise();
    
   if (m_pFlexFittingDlg->getShowDisp())
    {
	m_pRenderDisplacementArrows->setChecked( FALSE );
        renderDisplacement();
    }

}

// Simultaneous multibody optimization by GA

/**
 * show the dialog of the Simultaneous MultiBody Optimization by Genetic algorithm
 */ 
void sculptor_window::showGaDlg()
{
    int i;

    if (m_pGA != NULL && m_pGA->getIsThreadRunning()) // exists but running
    {
        QString s;
        s += "The genetic algorithm is still running. Can't start a new run!";
        statusBar()->showMessage( s, 10000 );
        
        svt_exception::ui()->info(QtoStd(s).c_str());
        return;
    }
    
    if (m_pGA != NULL && !m_pGA->getIsThreadRunning()) // exists but not running  
    {
	QString s;
        s += "A genetic algorithm is running. Can't start a new run!";
        statusBar()->showMessage( s, 10000 );
        
        svt_exception::ui()->info(QtoStd(s).c_str());
        return;
    }

    bool bAreDocsLocked = false;

    //get selected files 
    vector<pdb_document*> oSelectedPdbs;
    vector<situs_document*> oSelectedSitus;

    for( i=1; i<m_pDocumentList->count(); i++ )
    {
	if ( m_pDocumentList->getLocked(i) )
	    bAreDocsLocked = true;

        if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB)
	    oSelectedPdbs.push_back( (pdb_document *)m_pDocumentList->getDoc(i) );
        else if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_SITUS)
	    oSelectedSitus.push_back( (situs_document *)m_pDocumentList->getDoc(i) );
    }

    if (oSelectedPdbs.size()==0) // no pdb selected then use probe
        oSelectedPdbs.push_back( (pdb_document *)getProbeDoc() );
    if (oSelectedSitus.size()==0) // no situs selected then use target
        oSelectedSitus.push_back( (situs_document *)getTargetDoc() );
	
    if ( bAreDocsLocked )
    {
        QString s;
        s += "Documents are locked by another running algorithm. Please wait for other calculations to finish before proceeding!";
        //statusBar()->showMessage( s, 10000 );
        
        svt_exception::ui()->info(QtoStd(s).c_str());

	return;
    }
  
    if ( oSelectedPdbs.size() == 0 ) // no units were selected 
    {
        QString s;
        s += "The genetic algorithm could not be started. Please select the atomic structures!";
        statusBar()->showMessage( s, 10000 );
        
        svt_exception::ui()->info(QtoStd(s).c_str());
        return;
    }
    
    if (oSelectedSitus.size() == 0) // no volume was selected 
    {
        QString s;
        s += "The genetic algorithm could not be started. Please select the target volume!";
        statusBar()->showMessage( s, 10000 );
        
        svt_exception::ui()->info(QtoStd(s).c_str());
        return;
    }
    
    if (m_pGADlg == NULL)
    {
        m_pGADlg = new dlg_gamultifit( this );

        // set the m_pShowFitnessCurveDlg (the one form the menu) action in the dialog's checkbox
        m_pGADlg->setShowFitnessCurveAction( m_pShowFitnessCurve );
    }
    else
    {
        m_pGADlg->show();
        m_pGADlg->raise();
    }

    //default values in the dialog
    m_pGADlg->setPopSize( oSelectedPdbs.size() * 100 );
    m_pGADlg->setPopSizeAroundCenters( oSelectedPdbs.size() * 100 );

    //set selected documents
    m_pGADlg->setPdb(oSelectedPdbs);
    m_pGADlg->setSitus(oSelectedSitus);

    if (m_pGADlg->exec() == QDialog::Accepted)
    {
        if (m_pGAFitnessCurveDlg!=NULL)
        {
            delete m_pGAFitnessCurveDlg;
            m_pGAFitnessCurveDlg = NULL;
        }

        startGa( false );

        if ( m_pShowFitnessCurve->isChecked() == TRUE )
            showFitnessCurveDlg();
    }
};

/**
 * Launch the genetic algorithm in new thread 
 * \param bUseDefaultParams if false use the m_pGaDlg to get the params
 */
void sculptor_window::startGa(bool bUseDefaultParams)
{
    try
    {
        //
        // Set/read the parameters
        //
        vector<pdb_document*> oSelectedPdbs = m_pGADlg->getPdb();
        vector<situs_document*> oSelectedSitus = m_pGADlg->getSitus();
   
        //
        // Create GA
        //
        unsigned int iGenes 	= oSelectedPdbs.size()*4;
        m_pGA                   = new sculptor_gamultifit<svt_gamultifit_ind>( iGenes );
 
       if (m_pWaitDlg == NULL)
            m_pWaitDlg = new dlg_wait( 30, this );
        else 
	    m_pWaitDlg->setTimer(30);
    
        m_pWaitDlg->show();


        // Population size
        int iPopSize;
        // Max generations
        int iMaxGen;
        // how often should the model get exported, every generation (=1) or every 10 (=10) or never (=0)
        int iWriteModelInterval;
        // Output path
        string oPath, oNewFolderName;
        // max Threads
        int iMaxThread;
        // resolution
        Real64 fRes;
        // Scoring function
        int eScore;
        
        unsigned int iTabuWindowSize;
        // tabu threshold - see svt_ga.h for details
        Real64 fTabuThreshold;
        // tabu region size - see svt_ga.h for details
        Real64 fTabuRegionSize;
        // sync after how many generations?
        int iSyncGen;
        // cutoff distance
        Real64 fDistanceThreshold;
        // cutoff distance penalty
        Real64 fDistancePenalty;
        //how many individuals should be created around the feature vectors of the map
        int iPopSizeAroundCenters;

        
        if (!bUseDefaultParams && m_pGADlg!=NULL)
        {
            fRes                   = m_pGADlg->getResolution();
            
            iPopSize               = m_pGADlg->getPopSize();
            iMaxGen                = m_pGADlg->getMaxGen();
            
            if (m_pGADlg->getSample())
                iPopSizeAroundCenters = m_pGADlg->getPopSizeAroundCenters();
            else
                iPopSizeAroundCenters = 0;
            
            if (m_pGADlg->getCVScore())
                eScore              = 2;
            else
                eScore 		    = 3; 
                
            iMaxThread 		   = m_pGADlg->getMaxThread();
            iSyncGen               = m_pGADlg->getSyncGen();
            
            //tabu region settings
            iTabuWindowSize 	   = m_pGADlg->getTabuWindowSize();
            fTabuThreshold         = m_pGADlg->getTabuThreshold();
            fTabuRegionSize        = m_pGADlg->getTabuRegionSize();
            
            fDistanceThreshold     = m_pGADlg->getDistanceThreshold();
            fDistancePenalty       = m_pGADlg->getDistancePenalty();
            
            if (m_pGADlg->getOutput())
            {
                oPath =  m_pGADlg->getOutputPath();
                if ( oPath.size()==0 )
                {
                   svt_exception::ui()->info("Please select an output path!");
                   g_pWindow->showGaDlg();
                   return;
                }   
            }    
            
            iWriteModelInterval    = m_pGADlg->getWriteModelInterval(); 
        }
        else
        {
            fRes                   = 10;
            
            iPopSize               = 300;
            iMaxGen                = 1000;
            iPopSizeAroundCenters  = iPopSize;
            
            eScore		   = 2;
            
            iMaxThread 		   = 2;
            iSyncGen               = 100;
            
            //tabu region settings
            iTabuWindowSize 	   = 30;
            fTabuThreshold         = 0.05;
            fTabuRegionSize        = 3.0;
            
            fDistanceThreshold     = 10.0;
            fDistancePenalty       = 0.90;
            
            iWriteModelInterval    = 50;
            oPath                  = "/tmp/mosaec";

            //sprintf( pCmd, "if [ -d %s ]\n then\n rm -r %s\n fi\n mkdir %s\n", oPath.c_str(), oPath.c_str(), oPath.c_str());
            //svt_system( pCmd );

          }


        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setResolution( fRes );
        
        m_pGA->setMaxGen( iMaxGen ); 
        m_pGA->setPopSize( iPopSize );
        if (iPopSizeAroundCenters > 0)
            m_pGA->setPopSizeAroundCenters( iPopSizeAroundCenters );
        
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setMaxThread( iMaxThread );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setSyncGen( iSyncGen );
        
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setScore( (score)eScore );
        if (oPath.size()>0)
            ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setOutputPath( oPath.c_str() );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setWriteModelInterval( iWriteModelInterval );	
            
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setTabuWindowSize( iTabuWindowSize );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setTabuThreshold(  fTabuThreshold );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setTabuRegionSize( fTabuRegionSize );

        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setDistanceThreshold( fDistanceThreshold );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setDistanceThresholdPenalty( fDistancePenalty ); 
     
        // Mutation probability
        Real64 fMP                 = 0.05;
        // Mutation offset
        Real64 fMO                 = 0.05;
        // Crossover probability
        Real64 fCP                 = 0.95;
        // Selective pressure
        Real64 fSP                 = 1.3;
        // angular step size
        Real64 fAngularStepSize    = 1.0;
        
        // all mutate on / off
        bool bMutateAll            = true;
        // logfile
        string oLogFname           = "log";

        vector<svt_point_cloud_pdb< svt_vector4<Real64> > > oUnits, oCoarseUnits;
        svt_point_cloud_pdb< svt_vector4<Real64> > oPdb,  oCoarsePdb;
        svt_volume< Real64 >* pVol = NULL;

        //use target first selected pdb
	// FIXME wouldn't one prefer to use the actual target volume?
        if ( oSelectedSitus.size() > 0 )
            pVol = ((situs_document*)oSelectedSitus[0])->getVolume();

        if (pVol == NULL) // no volume was selected
        {
            svt_exception::ui()->info("The genetic algorithm could not be started. Please select a target volume!");
            return;
        }

        oSelectedSitus[0]->setLocked(true);

        //lock selected 
        unsigned int i;
        for (i = 0; i<oSelectedPdbs.size(); i++)
        {
            oSelectedPdbs[i]->setLocked( true );
            oPdb = ((pdb_document*) oSelectedPdbs[i])->getPointCloud();
            oUnits.push_back( oPdb );
        }

        svt_sgenrand( svt_getToD() );	

        //
        // Create GA
        //
        //process selected files 
        for(i = 0; i < oSelectedPdbs.size(); i++)
        {
            //get the point cloud   
            if (!bUseDefaultParams && m_pGADlg!=NULL && m_pGADlg->getUseComputedCV() )
            {
                oCoarsePdb = ((pdb_document*)oSelectedPdbs[i])->getCodebook( );
                if (oCoarsePdb.size() == 0)
                {
                    svt_exception::ui()->info("Please compute the feature vectors before proceeding!\n(Recommended ratio of 1 feature per 30-50 atoms)");

                    //unlock all
                    oSelectedSitus[0]->setLocked(false);
                    for (i = 0; i<oSelectedPdbs.size(); i++)
                            oSelectedPdbs[i]->setLocked( false );

                    //delete GA
                    delete(m_pGA);
                    m_pGA = NULL;

                    return;
                }
                oCoarseUnits.push_back( oCoarsePdb );
            }
            else
            {	
                if (m_pGADlg->getCVScore())
                {
                    unsigned int iNoAtomsPerCv = 30;
                    unsigned int iNoCv = floor( (Real64)oUnits[i].size()/(Real64)iNoAtomsPerCv+0.5 );
                    Real64 fEi = 0.1;
                    Real64 fEf = 0.001;
                    Real64 fLi = 0.2;
                    Real64 fLf = 0.02;	
                    unsigned int iMaxStep = 100000;
            
                    svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo; 
                    oClusterAlgo.setLambda( fLi, fLf );
                    oClusterAlgo.setEpsilon( fEi, fEf );
                    oClusterAlgo.setMaxstep( iMaxStep );
            
                    oCoarsePdb = oClusterAlgo.cluster(iNoCv, 8, oUnits[i]);
                    for (unsigned int iAtom=0; iAtom < oCoarsePdb.size(); iAtom++)
                            oCoarsePdb.getAtom(iAtom)->setChainID(oUnits[i].getAtom(0)->getChainID());

                    oCoarseUnits.push_back( oCoarsePdb );
                    if (oCoarsePdb.size() == 0)
                    {
                        svt_exception::ui()->info("Please compute the feature vectors before proceeding!\n");

                        //unlock all
                        oSelectedSitus[0]->setLocked(false);
                        for (i = 0; i<oSelectedPdbs.size(); i++)
                                oSelectedPdbs[i]->setLocked( false );

                        //delete GA
                        delete(m_pGA);
                        m_pGA = NULL;

                        return;
                    }
                    // not sure whether the codebook vectors should be added		
                    //((pdb_document*)oSelectedPdbs[i])->setCodebook( oCoarsePdb );
                }
                else
                {
                    oCoarseUnits.push_back( oUnits[i] );
                }

            }
        }
        

        //normally not needed -  but just in case
        if (oUnits.size()==0) // no units were selected 
        {
            svt_exception::ui()->info("The genetic algorithm could not be started. Please select some probe structures!");

            //unlock all
            oSelectedSitus[0]->setLocked(false);
            for (i = 0; i<oSelectedPdbs.size(); i++)
                    oSelectedPdbs[i]->setLocked( false );

            //delete GA
            delete m_pGA;
            m_pGA = NULL;

            return;
        }
        
        if (oCoarseUnits.size()==0) // no codebook were added 
        {
            svt_exception::ui()->info("The genetic algorithm could not be started. No codebook vectors were added!");

            //unlock all
            oSelectedSitus[0]->setLocked(false);
            for (i = 0; i<oSelectedPdbs.size(); i++)
                    oSelectedPdbs[i]->setLocked( false );

            //delete GA
            delete m_pGA;
            m_pGA = NULL;

            return;
        }
        else
        {
            for (unsigned int iIndex=0; iIndex<oCoarseUnits.size();iIndex++)
                if (oCoarseUnits[iIndex].size()==0)
                {
                    svt_exception::ui()->info("The genetic algorithm could not be started. No codebook vectors were added!");

                    //unlock all
                    oSelectedSitus[0]->setLocked(false);
                    for (i = 0; i<oSelectedPdbs.size(); i++)
                            oSelectedPdbs[i]->setLocked( false );

                    //delete GA
                    delete(m_pGA);
                    m_pGA = NULL;

                    return;
                }
        }

        
        svt_volume<Real64> oMap,oOldMap;
        oMap = *pVol;

        // interpolate map
        oOldMap = oMap;
        oMap.interpolate_map( oMap.getWidth() * 2.0  );

        svt_point_cloud_pdb< svt_vector4<Real64> > oCoarseMapPdb;
        if (iPopSizeAroundCenters > 0 && oUnits.size() > 0 )
        {
            unsigned int iNoCv = oUnits.size();
            Real64 fEi = 0.1;
            Real64 fEf = 0.001;
            Real64 fLi = 0.2;
            Real64 fLf = 0.02;
            unsigned int iMaxStep = 100000;

            svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo; 
            oClusterAlgo.setLambda( fLi, fLf );
            oClusterAlgo.setEpsilon( fEi, fEf );
            oClusterAlgo.setMaxstep( iMaxStep );
            
            oCoarseMapPdb = oClusterAlgo.cluster(iNoCv, 8, oOldMap);

            if (oCoarseMapPdb.size() == 0)
            {
                svt_exception::ui()->info("The genetic algorithm could not be started. No codebook vectors were added!\n");

                //unlock all
                oSelectedSitus[0]->setLocked(false);
                for (i = 0; i<oSelectedPdbs.size(); i++)
                        oSelectedPdbs[i]->setLocked( false );

                //delete GA
                delete(m_pGA);
                m_pGA = NULL;

                return;
            }
        }

        // set all parameters
        m_pGA->setReinsertionScheme( REINSERTION_ELITIST_UNIQUE );
        m_pGA->setBestPopSize( 1 );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setUnits( oUnits, oCoarseUnits );
        m_pGA->setDocs( oSelectedPdbs );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setTarget( oMap );	
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setResolution( fRes );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setScore( (score)eScore );
        m_pGA->setMutationProb( fMP );
        m_pGA->setCrossoverProb( fCP );
        m_pGA->setMutationOffset( fMO );
        m_pGA->setSelectivePressure( fSP );

        if (m_pGA->getPopSizeAroundCenters() > 0)
            m_pGA->setCoarseTarget( oCoarseMapPdb ); 
        
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setAngularStepSize( fAngularStepSize );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setMutateAll( bMutateAll );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->setRun( 0 );
        
        m_pUpdateGATimer->setInterval( 1000 ); // update every second
        m_pUpdateGATimer->start();
        
        //set target doc as latest variable and use it to figure out if ga is done initializing: feature calculation etc.
        m_pGA->setTarget( oSelectedSitus[0] );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->initPopulation( iPopSize );
        ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->initThreads();
    }
    catch(svt_exception oError)
    {
	oError.uiMessage();
    }
}

/**
 * Stop the GA 
 */
void sculptor_window::stopGa()
{
    try
    {
        if (m_pGA==NULL)
        {
            QString s;
            s += " No genetic algorithm is running!";
            
            statusBar()->showMessage( s, 10000 );
            svt_exception::ui()->info(QtoStd(s).c_str());
            
            return;
        }

        if ( m_pGA!=NULL && m_pGA->getTarget() == NULL ) // ga got stop before finishing computing the feature vectors
        {
            QString s;
            s += "The genetic algorithm could not be stop at this time. Please try again after the calculation of the feature vectors!";
            
            statusBar()->showMessage( s, 10000 );
            svt_exception::ui()->info(QtoStd(s).c_str());
            
            return;
        }

        m_pGA->setDone(true);
        
        while (m_pGA!=NULL && m_pGA->getIsThreadRunning())
        {
            //printf("DEBUG: still waiting to finish \n");
            svt_sleep(100); // wait
        }
        
        if ( m_pGA!=NULL) // already finished but it was not set to NULL
        {
            // unlock used documents
            vector<pdb_document*> oDocs = m_pGA->getDocs();
            for (unsigned int iUnit=0; iUnit < oDocs.size(); iUnit++)
                oDocs[iUnit]->setLocked( false );
            m_pGA->getTarget()->setLocked( false );
    
            //delete object
            delete(m_pGA);
            m_pGA = NULL;

            //stop timer
            m_pUpdateGATimer->stop();

            //inform user
            QString s;
            s = "Simultaneous docking done!";
            statusBar()->showMessage( s, 10000 );
            return;
        }
        else
        {
            svt_exception::ui()->info("GA was not deleted althought is should had");
            return; 
        }
		
    }catch(svt_exception oError)
    {
	oError.uiMessage();
    }
};


/**
 * update GA results in window
 */
void sculptor_window::updateGaResults()
{
    try
    {
        if (m_pGA == NULL)
                return;
        else
        {
            vector < vector<svt_matrix4<Real64> > > oVec    = ((svt_gamultifit<svt_gamultifit_ind>*)m_pGA)->getBestTransfMat();
            vector < Real64 > oBestPopFitness		    = m_pGA->getBestPopFitness();//get the fitness of the best population just in case the user wants to see it
            
            unsigned int iCountGen			    = m_pGA->getCurrGen()+m_pGA->getParallelRun()*m_pGA->getSyncGen() + 1;
            unsigned int iMaxGen			    = m_pGA->getMaxGen();
            
            QString s;
            if (iCountGen > iMaxGen)
            {
                s.sprintf("MOSAEC done. Generation %d.\n " ,iCountGen);
                statusBar()->showMessage( s, (int)(10000) );//10 seconds
                stopGa();
                return;
            }

            //no update required at this moment: either no data is available  or no new generation was created
            if (oVec.size() == 0)
                 return;
            
            unsigned int iBestPopSize                       = m_pGA->getBestPopSize();
            Real64 fTime                                    = m_pGA->getTimeGen();
    
            s.sprintf("MOSAEC is running. Generation %d of %d (%.2fs/gen).", iCountGen, iMaxGen, fTime);
            statusBar()->showMessage( s, (int)(fTime*1000) );
            
            if (fTime > 0.0)
                m_pUpdateGATimer->setInterval( (int)(fTime*1000) );
            else
                m_pUpdateGATimer->setInterval( 1000 );
            m_pUpdateGATimer->start();
            
            vector<pdb_document*> oDocs                     = m_pGA->getDocs();
            unsigned int iNoUnits                           = oDocs.size();
            sculptor_document* pProbeUnit;
            svt_matrix4< Real64 > oMat;
            char pName[1256];

            for (unsigned int iUnit=0; iUnit < iNoUnits; iUnit++)
            {
                //get the document 
                pProbeUnit = (pdb_document*)oDocs[iUnit];
                
                if (pProbeUnit == NULL)
                {
                    stopGa();
                    svt_exception::ui()->info( "Document was deleted and can't be updated. The MOSAEC run was stopped. " ); 
                    return;
                }
                else 
                {
                    //delete old transformation
                    char pTransfName[1256];
                    for (unsigned int iTransf=0; iTransf < pProbeUnit->getTransformCount(); iTransf++)
                    {
                        strcpy(pTransfName, pProbeUnit->getTransformName( iTransf ));
                        
                        if (pTransfName[0]=='G' && pTransfName[1]=='A' && pTransfName[2]==' ' && pTransfName[3]=='G' && pTransfName[4]=='e' && pTransfName[5]=='n')
                        {
                            pProbeUnit->removeTransform( iTransf );
                            iTransf--; // decrease count cause the list gets shorter and the transformation iTransf is not the same as before erase
                        }
                    }
               
                    for (unsigned int iBest=0; iBest < iBestPopSize; iBest++)
                    {
                        oMat = oVec[iBest][iUnit];
                        
                        //from Angstroem go into display coordinates 
                        oMat[0][3] = oMat[0][3] * 1.0E-2;
                        oMat[1][3] = oMat[1][3] * 1.0E-2;
                        oMat[2][3] = oMat[2][3] * 1.0E-2;
                        
                        sprintf( pName, "GA Generation %d Best %i", iCountGen, iBest);
                        pProbeUnit->addTransform( pName, oMat);
                        ((pdb_document*)pProbeUnit)->updateTransList();
                        
                        if (iBest == 0)
                        {
                            pProbeUnit->applyTransform(); // the best Individual
                            pProbeUnit->setChanged();
                        }
                    }
                }
            }
            
            //****************
            //**** Plot data
            //****************
            
            updateFitnessCurveDlg(oBestPopFitness, iCountGen);
            
            // if the user clicked on one of the close buttons of the fitness curve dialog, then it
            // is hidden now. then don't show the dialog unless they activate the menu action again
            if ( m_pGAFitnessCurveDlg->isHidden() == TRUE )
                m_pShowFitnessCurve->setChecked( FALSE );
            showFitnessCurveDlg();

        }
    }
    catch(svt_exception oError)
    {
	oError.uiMessage();
    }
}


/**
 * add data to fitness plot
 * Attention: does not display the plot; the function showFitnessCurveDlg() displays it or not depending on arguments
 */
void sculptor_window::updateFitnessCurveDlg( vector< Real64 > &oFitness, unsigned int iGen)
{
    int i, iSize;
    unsigned int iNoCurves = 1;


    if (m_pGAFitnessCurveDlg == NULL)
    {
        m_pGAFitnessCurveDlg = new dlg_plot( this, QString("Fitness Curves") );
        m_pGAFitnessCurveDlg->setComment(QString(""));
        m_pGAFitnessCurveDlg->setXAxisTitle(QString("Generation"));
        m_pGAFitnessCurveDlg->setYAxisTitle(QString("Score"));
    }

    vector<sculptor_qwtplotcurve*> aCurves = m_pGAFitnessCurveDlg->getCurves();
    
    if ( aCurves.size() == 0 ) // no curves attached to the plot
    {
        //add curves
        vector <Real64> oX, oY;
        oX.push_back( iGen );
        oY.push_back( 0 );
        svt_palette* pPalette = NULL;
        svt_color *pColor = NULL;
        
        for( i=1; i<m_pDocumentList->count(); i++)
            if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB)
                pPalette = ((pdb_document*)(m_pDocumentList->getDoc(i)))->getHighContrastPalette();

        //create the curves
        for (unsigned int iIndex=0; iIndex < iNoCurves; iIndex++)
        {
	    if (iIndex < oFitness.size())
            {
		if (oFitness[iIndex] > 1e5) // probably value needs to be minimized
		    oY[0] = 1e10-oFitness[iIndex];
		else
		    oY[0] = oFitness[iIndex];
            }
	    else
		oY[0] = 0.0;
            
            sculptor_qwtplotcurve* pCurve = new sculptor_qwtplotcurve( QString("Best"), oX, oY, oX.size() );
            
            pCurve->setStyle(QwtPlotCurve::Steps);

            if (pPalette!=NULL)
                pColor = pPalette->getColor(iIndex % pPalette->size());
            else
                pColor = new svt_color(iIndex/oFitness.size(), iIndex/oFitness.size(), iIndex/oFitness.size());

            // assign color to the curve
            pCurve->setColor(int(255.0f*pColor->getR()), int(255.0f*pColor->getG()), int(255.0f*pColor->getB()));

            // attach the curve to the plot
            m_pGAFitnessCurveDlg->attachCurve(pCurve);
        }
    }
    else // just update curve data
    {
        Real64 fY, fX;
        for (unsigned int iIndex=0; iIndex < iNoCurves; iIndex++)
        {
            fX = iGen;
            if (iIndex < oFitness.size())
                if (oFitness[iIndex] > 1e5) // probably value needs to be minimized
                    fY = 1e10 - oFitness[iIndex];
                else
                    fY = oFitness[iIndex];
            else
                fY = 0.0;
	    
	    //remove data points from curve if already added in previous generations, e.g. if a tabu
	    //is added at gen 90, the gen number is 75
            iSize = aCurves[iIndex]->getSize();

            while( aCurves[iIndex]->getDataX( --iSize ) >= fX )
                aCurves[iIndex]->pop_back();
		
	    aCurves[iIndex]->push_back(fX, fY);
	}
        m_pGAFitnessCurveDlg->sReplot();
    }

    
}

/**
 * shows the Fitness Curve Dialog
 */
void sculptor_window::showFitnessCurveDlg()
{
    if ( m_pShowFitnessCurve->isChecked() )
    { 
        // if there is no current GA, but a previous one created a curve dialog, then reshow the
        // dialog now
        if (m_pGA == NULL)
        {
            if (m_pGAFitnessCurveDlg != NULL && m_pGAFitnessCurveDlg->getCurves().size()>0 )
            {
                m_pGAFitnessCurveDlg->raise();
                m_pGAFitnessCurveDlg->show();
            }
        }
        else
        {
            // there is a GA. create the dialog if it doesn't exist yet, and then show it
            if (m_pGAFitnessCurveDlg == NULL)
            {
                m_pGAFitnessCurveDlg = new dlg_plot(this, QString("Fitness Curves") );
                m_pGAFitnessCurveDlg->setComment(QString(""));
                m_pGAFitnessCurveDlg->setXAxisTitle(QString("Generation"));
                m_pGAFitnessCurveDlg->setYAxisTitle(QString("Score"));
            }
            m_pGAFitnessCurveDlg->sReplot();
            m_pGAFitnessCurveDlg->raise();
            m_pGAFitnessCurveDlg->show();
        }
    }
    else
    {
        if (m_pGAFitnessCurveDlg != NULL) 
            m_pGAFitnessCurveDlg->hide();
    }
}

/**
 * show the dialog of the tube extraction by Genetic algorithm
 */ 
void sculptor_window::showGaCylDlg()
{
    if (m_pGACyl != NULL ) // exists but not running  
    {
	QString s;
        s += "A genetic algorithm is running. Can't start a new run!";
        statusBar()->showMessage( s, 10000 );
        
        svt_exception::ui()->info(QtoStd(s).c_str());
        return;
    }

    vector<sculptor_document*> oDocuments = m_pDocumentList->getDocuments(true, SENSITUS_DOC_SITUS);
    if (oDocuments.size()==0)//no situs document was selected
    {
         svt_exception::ui()->info("Please select one volumetric map!");
         return;
    }


    if (m_pGACylDlg == NULL)
    {
        m_pGACylDlg = new dlg_gacylinder( this, getScene()->getDataTopNode() );
        m_pGACylDlg->setVoxelWidth( ((situs_document*)oDocuments[0])->getVolume()->getWidth() );
        m_pGACylDlg->show();

    }else
    {
        m_pGACylDlg->setVoxelWidth( ((situs_document*)oDocuments[0])->getVolume()->getWidth() );
        m_pGACylDlg->setDefaultParameters();
        m_pGACylDlg->show();
        m_pGACylDlg->raise();
    }
  
    if (m_pGACylDlg->exec() == QDialog::Accepted)
        startGaCyl();
}

/**
 * start the tube extraction
 */
void sculptor_window::startGaCyl()
{
    try
    {
    
        //
        //  the main parameters
        //
        
        Real64 fRes                     = m_pGACylDlg->getResolution();
        int iPopSize                    = m_pGACylDlg->getPopSize();
        int iMaxGen                     = m_pGACylDlg->getMaxGen();// max generations
        int iMaxThread                  = m_pGACylDlg->getMaxThread();
        int iSyncGen                    = m_pGACylDlg->getSyncGen();// sync after how many generations?
        Real64 fAngularStepSize         = 1;
        Real64 fPsiFrom                 = 0.0; // angular search range
        Real64 fPsiTo                   = 360; 
        Real64 fThetaFrom               = 0.0;
        Real64 fThetaTo                 = 180.0 ;
        Real64 fPhiFrom                 = 0.0 ;
        Real64 fPhiTo                   = 0.0 ;
        Real64 fDistanceThreshold       = 5.0; // cutoff distance 
        Real64 fDistancePenalty         = 0.90;  // cutoff distance penalty
        unsigned int iTabuWindowSize    = 35; // size of tabu window - see svt_ga.h for details
        Real64 fTabuThreshold           = 4.0; // size of tabu window - see svt_ga.h for details
        Real64 fTabuRegionSize          = m_pGACylDlg->getTabuRegionSize(); // tabu region size - see svt_ga.h for details
        int iRefinementMaxMutPerGene    = 5; // RefinementMaxMutPerGene
        int iNoOfTubes                  = m_pGACylDlg->getObjectCount();// Number of Tubes to output: if = 0 estimate from target
        bool bApplyBlurring2Model       = false; // shoud the model be burred
        Real64 fExpansionTemplateRadius = m_pGACylDlg->getExpansionTemplateRadius();
        Real64 fSearchTemplateRadius    = m_pGACylDlg->getSearchTemplateRadius();
        unsigned int iTemplatePointCount= 31; // the number of points in the template n-1 are in the circle and one in the center
        int iSearchTemplateRepeats      = m_pGACylDlg->getSearchTemplateRepeats(); //repeates 
        int iExpansionTemplateRepeats   = m_pGACylDlg->getExpansionTemplateRepeats(); //length
        Real64 fDistBetweenRepeats      = m_pGACylDlg->getDistBetweenRepeats(); // number of template copies in the model
        Real64 fCrawlingStep            = m_pGACylDlg->getCrawlingStep(); // distance covered in one crawling step
        Real64 fAcceptMoveRatio         = m_pGACylDlg->getAcceptMoveRatio(); // how much of the original score is allowed to accept a move ~ 0.7*OrigScore
        unsigned int iMaxFailedCrawls   = 2;// the number of failed crawls that are accepted before stopping
        unsigned int iWriteModelInterval= 0;
        Real64 fAni                     = m_pGACylDlg->getAni(); // anisotropic correction for the tomography data

       if (m_pWaitDlg == NULL)
            m_pWaitDlg = new dlg_wait( 15, this );
        else 
	        m_pWaitDlg->setTimer(15);
    
        m_pWaitDlg->show();

        if (fAcceptMoveRatio>1)
           fAcceptMoveRatio /= 100.0;

        //
        // Load the map
        //
        svt_volume<Real64> oMap;
        //get map
        vector<sculptor_document*> oDocuments = m_pDocumentList->getDocuments(true, SENSITUS_DOC_SITUS);
        if (oDocuments.size()>0)
        {
            //assume local normalization was already applied and the negative values were removed
            oMap = *((situs_document*)oDocuments[0])->getVolume();
        } 
        else
        {
            svt_exception::ui()->info("Please select one volumetric map!");
            return;
        }
 
        Real64 fOrigZWoAni = oMap.getGridZ();

        oMap.threshold( 0.0 , oMap.getMaxDensity() );
   
        if (fAni != 1) // some anisotropic correction needed
            oMap.interpolate_map(oMap.getWidth(), oMap.getWidth(), oMap.getWidth()*fAni);

        //
        // Create GA
        //
        unsigned int iGenes = 4;

        // set all parameters
        m_pGACyl = new sculptor_gacylinder<svt_gacylinder_ind> ( iGenes );
        m_pGACyl->setMaxGen( iMaxGen );  
        m_pGACyl->setPopSize( iPopSize );
        m_pGACyl->setTarget( oMap );
        m_pGACyl->setAngularStepSize( fAngularStepSize );
        m_pGACyl->setAngularSearchRange( fPsiFrom, fPsiTo, fThetaFrom, fThetaTo, fPhiFrom, fPhiTo );    
        m_pGACyl->setDistanceThreshold( fDistanceThreshold );
        m_pGACyl->setDistanceThresholdPenalty( fDistancePenalty );
        m_pGACyl->setWriteModelInterval( iWriteModelInterval );
        m_pGACyl->setTabuWindowSize( iTabuWindowSize );
        m_pGACyl->setTabuThreshold(  fTabuThreshold );
        m_pGACyl->setTabuRegionSize( fTabuRegionSize );
        m_pGACyl->setRefinementMaxMutPerGene( iRefinementMaxMutPerGene );
        m_pGACyl->setMaxThread(iMaxThread);
        m_pGACyl->setSyncGen( iSyncGen );
        m_pGACyl->setNoOfCylinder2Detect( iNoOfTubes );
        m_pGACyl->setApplyBlurring2Model( bApplyBlurring2Model );
        m_pGACyl->setResolution( fRes );
        //template is given
        m_pGACyl->setTemplateRadius( fExpansionTemplateRadius );
        m_pGACyl->setSearchTemplateRadius( fSearchTemplateRadius );
        m_pGACyl->setTemplatePointCount( iTemplatePointCount );
        m_pGACyl->setTemplateRepeats( iExpansionTemplateRepeats );
        m_pGACyl->setSearchTemplateRepeats( iSearchTemplateRepeats );
        m_pGACyl->setDistBetweenRepeats ( fDistBetweenRepeats );
        m_pGACyl->setCrawlingStepSize ( fCrawlingStep );
        m_pGACyl->setAcceptMoveRatio( fAcceptMoveRatio );
        m_pGACyl->setMaxFailedCrawls ( iMaxFailedCrawls );
        m_pGACyl->setAniCorr(fAni, fOrigZWoAni );
       
        //
        // Run the genetic algorithm
        //
        svt_population<svt_gacylinder_ind> oTabuPop, oPop;
        m_pGACyl->setMaxThread(iMaxThread);
        m_pGACyl->setMaxGen( iMaxGen );
        m_pGACyl->setSyncGen( iSyncGen );
        m_pGACyl->setDone( false );
 
        //m_pUpdateGACylTimer->setInterval( 100000 ); // update every second
        m_pUpdateGACylTimer->setInterval( 1000 ); // update every 1 s
        m_pUpdateGACylTimer->start();
        svt_exception::ui()->busyPopup("Please wait while Volume Tracer is running...!");
        
        m_pGACyl->initThreads();
    }
    catch(svt_exception oError)
    {
        oError.uiMessage();
    }
};

/**
 * update the results
 */ 
void sculptor_window::updateGaCylResults()
{
   try
   {
    svt_exception::ui()->busyRotate();
    if (this->m_pGACyl != NULL && this->m_pGACyl->getNewResultsFound() )
    {
        this->m_pGACyl->setNewResultsFound(false);
        vector <svt_tube> oTubes;
        oTubes = m_pGACyl->getCylinders();

        if (oTubes.size()==0) // nothing found yet
            return;
  
        pdb_document *pDoc;
        pDoc = m_pGACyl->getResultsDoc();
        pdb_document *pBestDoc;
        pBestDoc = m_pGACyl->getBestResultsDoc();

        if (pDoc == NULL || pBestDoc == NULL ) // not yet create; then create
        {
            char pFileName[1024];
            sprintf (pFileName,"Best%dHelices.pdb", m_pGACyl->getNoOfCylinder2Detect());
    
            pBestDoc = new pdb_document(getScene()->getDataTopNode(), "" , SVT_NONE);
                       
            addDocSimple( (sculptor_document*)pBestDoc, pFileName );
    
            pBestDoc->setChanged();
            pBestDoc->setLocked( true );

            m_pGACyl->setBestResultsDoc( pBestDoc );
 	        
            pDoc = new pdb_document(getScene()->getDataTopNode(), "" , SVT_NONE);
                   
            addDocSimple( (sculptor_document*)pDoc,"AllHelices.pdb" );
            pDoc->setChanged();
            pDoc->setLocked( true );

            m_pGACyl->setResultsDoc( pDoc );
        }
        else
        {
            ((svt_pdb*)pDoc->getNode())->deleteAllAtoms();  
            ((svt_pdb*)pBestDoc->getNode())->deleteAllAtoms();  
        }

        int iNoHelices2Detect =  m_pGACyl->getNoOfCylinder2Detect();
 
        svt_point_cloud_pdb< svt_vector4<Real64 > > oPdb, oCylinders, oBestCylinders;
 
        int iCount;
        Real64 fAni = m_pGACyl->getAniCorr();
        Real64 fOrigZ =  m_pGACyl->getOrigZ();
        Real64 fOrigZWoAni =  m_pGACyl->getOrigZWoAni();
        unsigned int iSize = oTubes.size();
        for (unsigned int iIndex=0; iIndex < iSize; iIndex++)    
        { 
            oPdb = oTubes[iIndex].getTube(NULL, true);
            iCount = 0;
            for (unsigned int iAtom=0; iAtom<oPdb.size(); iAtom++)
            {
                iCount++;
                //oPdb.getAtom(iAtom)->setChainID( iIndex );
                oPdb.getAtom(iAtom)->setModel(iIndex);
                oPdb.getAtom(iAtom)->setChainID('A');
 
                if (fAni != 1)
                    oPdb[iAtom].z( fOrigZWoAni + fAni*(oPdb[iAtom].z() - fOrigZ) );
                oPdb.getAtom(iAtom)->setPDBIndex( iCount );
                oPdb.getAtom(iAtom)->setResidueSeq(iCount);
                oPdb.getAtom(iAtom)->setOrdResidueSeq(iCount);
                oCylinders.addAtom( *oPdb.getAtom(iAtom), oPdb[iAtom] );
                if ((int)iIndex<iNoHelices2Detect)
                    oBestCylinders.addAtom( *oPdb.getAtom(iAtom), oPdb[iAtom] );
            }
        }
        oCylinders.calcAtomChainIDs();
        oCylinders.calcAtomModels();
        oCylinders.calcOrdinalChainIDs();
        oBestCylinders.calcAtomModels();
        oBestCylinders.calcAtomChainIDs();
        oBestCylinders.calcOrdinalChainIDs();

        svt_vector4< Real64> oCenter = oCylinders.coa();

        svt_matrix4 <Real64> oTranslation;
        oTranslation.loadIdentity();
        oTranslation.setTranslation(-oCylinders.coa());
        pDoc->setCenter( oCylinders.coa() );
        pBestDoc->setCenter( oCylinders.coa() );

        for( unsigned int i=0; i<oCylinders.size(); i++)
        {
            oCylinders[i] = oTranslation * oCylinders[i];
            ((svt_pdb*)pDoc->getNode())->addAtom( *oCylinders.getAtom(i), oCylinders[i] );
        }
        ((svt_pdb*)pDoc->getNode())->calcAllLists();
        ((svt_pdb*)pDoc->getNode())->calcAtomModels();

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

        ((svt_pdb*)pDoc->getNode())->drawGL();
        pDoc->setGlobalColmap(COLMAP_MODEL,0);
        ((svt_pdb*)pDoc->getNode())->setDisplayMode( SVT_PDB_CARTOON );
        ((svt_pdb*)pDoc->getNode())->setCartoonParam( CARTOON_SHEET_HELIX_PROFILE_CORNERS, 8);
        ((svt_pdb*)pDoc->getNode())->setCartoonParam( CARTOON_TUBE_DIAMETER, m_pGACyl->getSearchTemplateRadius() * 2);
        ((svt_pdb*)pDoc->getNode())->rebuildDL();

        //best document
        for( unsigned int i=0; i<oBestCylinders.size(); i++)
        {
            oBestCylinders[i] = oTranslation * oBestCylinders[i];
            ((svt_pdb*)pBestDoc->getNode())->addAtom( *oBestCylinders.getAtom(i), oBestCylinders[i] );
        }
        ((svt_pdb*)pBestDoc->getNode())->calcAllLists();
        ((svt_pdb*)pBestDoc->getNode())->calcAtomModels();

        // install new transformation
        ((svt_pdb*)pBestDoc->getNode())->setPos( new svt_pos(
                                    (1.0E-2) * oCenter.x(),
                                    (1.0E-2) * oCenter.y(),
                                    (1.0E-2) * oCenter.z()
                                   ) );

        pBestDoc->storeInitMat();

        oDocOrigin = pDoc->getRealPos();
        oDocOrigin.multScalar( 1.0E8f );
        pBestDoc->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
        pBestDoc->storeInitMat();

        ((svt_pdb*)pBestDoc->getNode())->drawGL();
        pBestDoc->setGlobalColmap(COLMAP_MODEL,0);
        ((svt_pdb*)pBestDoc->getNode())->setDisplayMode( SVT_PDB_CARTOON );
        ((svt_pdb*)pBestDoc->getNode())->setCartoonParam( CARTOON_TUBE_DIAMETER, m_pGACyl->getSearchTemplateRadius() * 2);
        ((svt_pdb*)pBestDoc->getNode())->setCartoonParam( CARTOON_SHEET_HELIX_PROFILE_CORNERS, 8);

        ((svt_pdb*)pBestDoc->getNode())->rebuildDL();

        statusBar()->showMessage( "Please wait while H/F Trac is running ... ", (int)(60000) );//60 seconds

        svt_redraw();
 
        svt_sleep(10);
        QString s;
        if (!m_pGACyl->getIsThreadRunning())
        {
            s.sprintf("Helix detection done. \n");
            statusBar()->showMessage( s, (int)(10000) );//10 seconds
            stopGaCyl();
            return;
        }

   }
   }catch (svt_userInterrupt&)
   {
       stopGaCyl();
   }
}

/**
 * stops the tube extraction
 */
void sculptor_window::stopGaCyl()
{
   try
    {
        if (m_pGACyl==NULL)
        {
            QString s;
            s += " No genetic algorithm is running!";
            
            statusBar()->showMessage( s, 10000 );
            svt_exception::ui()->info(QtoStd(s).c_str());
            
            return;
        }
        svt_exception::ui()->busyPopdown();

        m_pGACyl->setDone(true);

        svt_exception::ui()->busyPopup("Please wait while the algorithm is stopping!");
        while (m_pGACyl!=NULL && m_pGACyl->getIsThreadRunning())
        {
            svt_sleep(1000); // wait
            svt_exception::ui()->busyRotate();
            m_pGACyl->setDone(true);
        }
        svt_exception::ui()->busyPopdown();
        
        if ( m_pGACyl!=NULL) // already finished but it was not set to NULL
        {
            // unlock used documents
            pdb_document* pDoc = m_pGACyl->getResultsDoc();
            if (pDoc!=NULL)
                pDoc->setLocked( false );

            pDoc = m_pGACyl->getBestResultsDoc();
            if (pDoc!=NULL)
                pDoc->setLocked( false );

    
            //delete object
            delete(m_pGACyl);
            m_pGACyl = NULL;

            //stop timer
            m_pUpdateGACylTimer->stop();

            //inform user
            QString s;
            s = "Helix extraction done!";
            statusBar()->showMessage( s, 10000 );
            return;
        }
        else
        {
            svt_exception::ui()->info("GA was not deleted althought is should had");
            return; 
        }
		
    }catch(svt_exception oError)
    {
	oError.uiMessage();
    }

}

/**
 * get a pointer to the ga timer
 */
QTimer*  sculptor_window::getGaCylTimer()
{
    return m_pUpdateGACylTimer;
}; 



/**
* Shows PDF function of selected PDBs:
*/
void sculptor_window::showPdfDlg()
{
    svt_color* pColor;
    svt_palette* pPalette;
    
    //get selected PDBs
    vector<sculptor_document*> oSelPdbDoc = m_pDocumentList->getDocuments(true, SENSITUS_DOC_PDB);

    if ( oSelPdbDoc.size()==0 )
    {
        svt_exception::ui()->info("Please select at least one pdb document.");
        return;
    }
    
    // construct a new plot
    dlg_plot* pDlg = new dlg_plot( this, QString("Pair Distance Distribution") );

    pPalette = ((pdb_document*)oSelPdbDoc[0])->getHighContrastPalette();
    unsigned int iStatus, iSumStatus = 0;
    
    for (unsigned int i=0; i<oSelPdbDoc.size();i++)
    {
        if (pPalette!=NULL)
            pColor = pPalette->getColor(i % pPalette->size());
        else
            pColor = new svt_color( int(20.0*float(i)/float(oSelPdbDoc.size())),
                                    int(20.0*float(i)/float(oSelPdbDoc.size())),
                                    int(200.0*float(i)/float(oSelPdbDoc.size())));
            
        iStatus = ((pdb_document*)oSelPdbDoc[i])->addPdf2Plot(pDlg, *pColor);
        iSumStatus += iStatus;
    }
   
    if (iSumStatus>0)
    {
        pDlg->setXAxisTitle(QString("R"));
        pDlg->setYAxisTitle(QString("P(R)"));
    
        pDlg->raise();
        pDlg->show();
    }
};

/**
* Shows the difference between PDF function of selected PDBs: first selected PDB is the reference
* The reference will be substracted from all the other selected PDB
*/
void sculptor_window::showDiffPdfDlg()
{
    QString oName;
    svt_pair_distribution oPDFr, oPDF;
    svt_pair_distribution oPDFd;
    svt_palette* pPalette;
    pdb_document *oPDBr, *oPDB;
    
    vector<sculptor_document*> oSelPdbDoc = m_pDocumentList->getDocuments(true, SENSITUS_DOC_PDB);
    
    if ( oSelPdbDoc.size() == 0 )
    {
        svt_exception::ui()->info("Please select at least two pdb documents.");
        return;
    }
    
    // the first selected PDB is the reference; the reference will be substracted from the other selected PDB
    oPDBr = ((pdb_document *)oSelPdbDoc[0]);
    oPDFr = oPDBr->getPointCloud().getPairDistribution(); 

    if (oPDFr.size()<=1)
    {
        oPDBr->getPointCloud().calcPairDistribution();
        oPDFr = oPDBr->getPointCloud().getPairDistribution(); 

        if (oPDFr.size()>1) // cause user has push da cancel button
        {
            oPDBr->getPointCloud().normalizePairDistribution(); 
            oPDFr = oPDBr->getPointCloud().getPairDistribution(); 
        }
    }
 
    pPalette = oPDBr->getHighContrastPalette();
    dlg_plot* pDlg = new dlg_plot( this, QString("Difference Pair Distance Distribution") );

    for (unsigned int i=1; i<oSelPdbDoc.size();i++)
    {
        oPDB = ((pdb_document *)oSelPdbDoc[i]);
        oPDB->getPointCloud().setTimestep(0);
        
        oPDF = oPDB->getPointCloud().getPairDistribution();
        if (oPDF.size()<=1)
        {
            oPDB->getPointCloud().calcPairDistribution();
            oPDF = oPDB->getPointCloud().getPairDistribution();

            if (oPDF.size()>0) // cause user has push da cancel button
            {
                oPDB->getPointCloud().normalizePairDistribution();
                oPDF = oPDB->getPointCloud().getPairDistribution();
            }
        }
        
        if (oPDFr.size()>0 && oPDF.size()>0) // cause user has push da cancel button
        {
            oPDFd = oPDF - oPDFr;
            
            oName.sprintf("%s_%s ",
                          oSelPdbDoc[i]->getDisplayName().c_str(),
                          oSelPdbDoc[0]->getDisplayName().c_str() );
            sculptor_qwtplotcurve* pCurve = new sculptor_qwtplotcurve(oName, oPDFd.getBins(), oPDFd.getPdf(), (int)oPDFd.size());
        
            pCurve->setStyle(QwtPlotCurve::Lines);
            
           //set color 
            if (pPalette!=NULL)
            {
                svt_color* oColor = pPalette->getColor(i % pPalette->size());
                pCurve->setColor(int(oColor->getR()*255), int(oColor->getG()*255), int(oColor->getB()*255) );
            }
            else
                pCurve->setColor( int(20.0*float(i)/float(oSelPdbDoc.size())), int(20.0*float(i)/float(oSelPdbDoc.size())), int(200.0*float(i)/float(oSelPdbDoc.size())));
            
            pDlg->attachCurve(pCurve);
        }
        else
        {
            return;
        }
    }

    pDlg->setXAxisTitle(QString("R"));
    pDlg->setYAxisTitle(QString("P(R)"));

    pDlg->show();
}



/**
 * show the properties dialog for the force feedback
 */
void sculptor_window::showForceDlg()
{
    if (m_pForceDlg != NULL)
    {
        m_pForceDlg->show();
        m_pForceDlg->raise();
	return;
    }

    m_pForceDlg = new dlg_force(this);

    m_pForceDlg->show();
}

/**
 * toggle the properties dialog for the force feedback
 */
void sculptor_window::toggleForceDlg()
{
    if (m_pForceDlg == NULL)
    {
        showForceDlg();
	return;
    }

    if (m_pForceDlg != NULL)
    {
        if (m_pForceDlg->isVisible())
	{
	    m_pForceDlg->hide();
	} else {
	    m_pForceDlg->show();
	    m_pForceDlg->raise();
	}
    }

    return;
}

/**
 * show the motor temperatures dialog for the force feedback
 */
void sculptor_window::showTempDlg()
{
/*
    TempMotorsDlg* pDlg = new TempMotorsDlg(m_pScene, this);

    pDlg->show();
*/
}

/**
 * set force tool center mode
 * \param bCenter if true the center mode for the force tool is enabled
 */
void sculptor_window::setForceCenterMode(bool bCenter)
{
    m_pScene->setForceCenterMode(bCenter);
}

/**
 * show the refinement dialog
 */
void sculptor_window::showRefinementDlg()
{
    //
    // Get the target
    //
    if (getTargetDoc() == NULL || getTargetDoc()->getType() != SENSITUS_DOC_SITUS)
    {
	svt_exception::ui()->info("Please select a target map first!");
        return;
    }

    //
    // Is there a probe molecule?
    //
    bool bFoundPDB = false;
    for(int i=1;i < m_pDocumentList->count(); i++)
    {
        if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
            bFoundPDB = true;
    }
    if (!bFoundPDB && getProbeDoc() == NULL)
    {
	svt_exception::ui()->info("Please select an atomic model first!");
        return;
    }

    //
    // OK, we seem to have everything, so let's get going...
    //
    bool bAreDocsLocked = false;

    situs_document* pSitus = (situs_document*)(getTargetDoc());
    if (getTargetDoc()->getLocked())
        bAreDocsLocked = true;

    if (pSitus == NULL)
        return;

    //
    // get the probes
    //
    pdb_document* pPDBDoc;
    vector<unsigned long> oDocIDs;
    
    // refinement dialog
    if (m_pRefineDlg == NULL)
    {
        m_pRefineDlg = new dlg_refine( this );

        switch( m_iRefineMode )
        {
        case SCULPTOR_CVSAMPLING:
            m_pRefineDlg->setCVSampling( true );
            break;

        case SCULPTOR_UNIONSPHERE:
            m_pRefineDlg->setUnionSphere( true );
            break;

        case SCULPTOR_LAPLACIAN:
            m_pRefineDlg->setLaplace( true );
            break;

        default:
        case SCULPTOR_CORRELATION:
            m_pRefineDlg->setCorrelation( true );
            break;
        };
    }else
    {
        m_pRefineDlg->show();
        m_pRefineDlg->raise();
    };

    m_pRefineDlg->exec();

    if ( m_pRefineDlg->result() == QDialog::Accepted )
    {
	//
	//the volume
	//
	svt_volume<Real64>* pMap = pSitus->getVolume();
	svt_volume<Real64>  oLaplMap;
	svt_matrix4< Real64 > oVolMat;
        oVolMat.set( *pSitus->getNode()->getTransformation() );
        
	oVolMat.setTranslationX( oVolMat.translationX() * 1.0E2 );
	oVolMat.setTranslationY( oVolMat.translationY() * 1.0E2 );
	oVolMat.setTranslationZ( oVolMat.translationZ() * 1.0E2 );

	// resolution
	Real64 fRes = 10.0;
	fRes = m_pRefineDlg->getResolution();

	// set the parameters for the different modes
	if ( m_pRefineDlg->getUnionSphere() )
		m_iRefineMode = SCULPTOR_UNIONSPHERE;

	if ( m_pRefineDlg->getLaplace() )
	{
		m_iRefineMode = SCULPTOR_LAPLACIAN;

                pMap = pSitus->getLaplaceVol();
                oLaplMap.deepCopy( *pMap );
                oLaplMap.applyLaplacian();
                oLaplMap.setWidth( pMap->getWidth() );
	};

	if ( m_pRefineDlg->getCorrelation() )
		m_iRefineMode = SCULPTOR_CORRELATION;

	//
	//get the pdbs
	//
	svt_point_cloud_pdb< svt_vector4<Real64> >* pStruct = NULL;	
	vector< svt_point_cloud_pdb< svt_vector4<Real64> > > oPDBs;
	
	svt_matrix4< Real64 > oPDBMat;
	vector<svt_matrix4< Real64 > > oPDBMats;
		    
	vector<double> aDirection;
	
	//get all the selected PDBs because a multibody refinement will be done
	vector< sculptor_document* > oPowDocs;
	for(int i=1;i < m_pDocumentList->count(); i++)
	{
	    if ( m_pDocumentList->getLocked(i) ) 
		bAreDocsLocked = true;	

	    if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	    {
		oPowDocs.push_back( m_pDocumentList->getDoc(i) );
	    }
	}

	if (oPowDocs.size()==0) // nothing was selected - use doc document
	{
	    oPowDocs.push_back( getProbeDoc() );
	    if (getProbeDoc()->getLocked()) 
		bAreDocsLocked = true;
	}

	if (bAreDocsLocked)
	{
	    QString s;
	    s += "The Powell refinment may not be executed while other algorithms are running. Please wait for the other calculation to finish before proceeding!";
	    statusBar()->showMessage( s, 10000 );

	    svt_exception::ui()->info(QtoStd(s).c_str());

	    return;
	}

	//lock map
	pSitus->setLocked( true ); 
	for (unsigned int iUnit=0; iUnit<oPowDocs.size();iUnit++)
	{
	    //look the document now; unlock when powell is done
	    oPowDocs[iUnit]->setLocked( true );
	
	    // Warn user if refining a document with active IGD and turn IGD off
	    if (oPowDocs[iUnit]->getIGDActive())
	    {
	        if(!oPowDocs[iUnit]->getIGDWarningShown())
		    	svt_exception::ui()->info("Warning: IGD will be turned off.\nRe-activate via double click on move icon.");
	        oPowDocs[iUnit]->setIGDWarningShown( true );
	        oPowDocs[iUnit]->setIGDActive(false);
	    }

	    //get PDB document
	    pPDBDoc =  (pdb_document*)oPowDocs[iUnit];
	    oDocIDs.push_back( pPDBDoc->getDocID() );
		
	    // get PDB
	    pStruct = &pPDBDoc->getPointCloud();
	    
	    if ( m_pRefineDlg->getCVSampling() )
	    {
	        m_iRefineMode = SCULPTOR_CVSAMPLING;
	        if (pPDBDoc->getCodebook().size() == 0)
	        {
		    svt_exception::ui()->error("No feature-vectors calculated for this structure. To use this option, \n please determine the codebook-vectors first (menu item: Docking->Clustering).");
		    return;
		}
	        pStruct = new svt_point_cloud_pdb< svt_vector4<Real64> >( pPDBDoc->getCodebook() );
	    }
	
	    oPDBs.push_back(*pStruct);
		
	    // create transformation matrix
	    if (pPDBDoc->getNode()->getTransformation() == NULL)
	        pPDBDoc->getNode()->setTransformation( new svt_matrix4f() );

	    oPDBMat = svt_matrix4<Real64>( *pPDBDoc->getNode()->getTransformation() );
		

	    oPDBMat.setTranslationX( oPDBMat.translationX() * 1.0E2 );
	    oPDBMat.setTranslationY( oPDBMat.translationY() * 1.0E2 );
	    oPDBMat.setTranslationZ( oPDBMat.translationZ() * 1.0E2 );
	
	    oPDBMats.push_back( oPDBMat );
		
	    // limit the optimizations if the users wishes to
	    if (m_pRefineDlg->getOptTrans(0)) // translation on X axis
	        aDirection.push_back( pSitus->getVoxelWidth() * 0.25 );
	    else
	        aDirection.push_back( 0 );

	    if (m_pRefineDlg->getOptTrans(1)) // translation on Y axis
	        aDirection.push_back( pSitus->getVoxelWidth() * 0.25 );
	    else
		aDirection.push_back( 0 );
    
	    if (m_pRefineDlg->getOptTrans(2)) // translation on Z axis
	        aDirection.push_back( pSitus->getVoxelWidth() * 0.25 );
	    else
	        aDirection.push_back( 0 );

	    if (m_pRefineDlg->getOptRot(0)) // Rotation on X axis
	        aDirection.push_back( deg2rad( 10.0 ) );
	    else
	        aDirection.push_back( 0 );

	    if (m_pRefineDlg->getOptRot(1)) // Rotation on Y axis
	        aDirection.push_back( deg2rad( 10.0 ) );
	    else
	        aDirection.push_back( 0 );

	    if (m_pRefineDlg->getOptRot(2)) // Rotation on Z axis
	        aDirection.push_back( deg2rad( 10.0 ) );
	    else
	        aDirection.push_back( 0 );
	}	
		
        // create powell object
        sculptor_powell_cc* pPowell;
        if ( !m_pRefineDlg->getLaplace() )
            pPowell = new sculptor_powell_cc(oPDBs, oPDBMats, *pMap, oVolMat );
        else
            pPowell = new sculptor_powell_cc(oPDBs, oPDBMats, oLaplMap, oVolMat );
	pPowell->setDocs( oPowDocs );
        pPowell->setTolerance( m_pRefineDlg->getTolerance() );
        pPowell->setDirection( aDirection );
	pPowell->setMaxIter( m_pRefineDlg->getMaxIter() );
	pPowell->setMaxIter2Minimize( 10 );
	pPowell->setLaplacian( m_pRefineDlg->getLaplace() );
	pPowell->setResolution( fRes );
	pPowell->setFast( false );
	pPowell->setBlur( true );

	if ( m_pRefineDlg->getUnionSphere() )
	{
		pPowell->setFast( false );
		pPowell->setBlur( false );
	};
	if ( m_pRefineDlg->getCVSampling() )
	{
		pPowell->setFast( true );
		pPowell->setBlur( false );
	};

        long int iTime = svt_getToD();
	pPowell->maximize();

        long int iElapsedTime = svt_getToD() - iTime;
    
        svtout << "Elapsed time: " << iElapsedTime/(Real64)1000.0 << " s " << iElapsedTime/(Real64)60000.0 << " min  " << iElapsedTime/(Real64)3600000.0 << " h " << endl;  
	
	//sets the transforms to the documents
	pPowell->setPowellTransf();
	//svt_redraw();

	//unlock
	pSitus->setLocked( false );
	for (unsigned int iUnit=0; iUnit<oPowDocs.size();iUnit++)
	    oPowDocs[iUnit]->setLocked( false );

        if ( m_iRefineMode == SCULPTOR_CVSAMPLING )
            delete pStruct;

    }
};

/**
 * automatically color probe and target structures
 */
void sculptor_window::autoColorProbeSolution()
{
    m_bAutoAdjustProbeSolutionColor = ! m_bAutoAdjustProbeSolutionColor;
    m_pAutoColor->setChecked( m_bAutoAdjustProbeSolutionColor );
}

/**
 * Laplace quantization
 */
void sculptor_window::laplaceQuantization()
{
    // get current documentlistitem
    window_documentlistitem* pDLI = m_pDocumentList->getCurrentItem();

    if (pDLI == NULL)
	return;

    if ( !pDLI->getSelected() )
    {
        QMessageBox::warning(this,
                             QString("Feature Extraction"),
                             QString("Please select a document to perform the clustering on."));
        statusBar()->showMessage( "select document to cluster", 2000 );
	return;
    }

    sculptor_document* pDoc = pDLI->getDoc();

    if (pDoc != NULL)
    {
	pDoc->showLaplaceQuantizationDlgOptions();
	m_pRenderFeatureVectors->setChecked( pDoc->getCV_Visible() );
    }
};

/**
 * Atomic Laplace quantization
 */
void sculptor_window::atomicQuantization()
{
    // get current documentlistitem
    window_documentlistitem* pDLI = m_pDocumentList->getCurrentItem();

    if (pDLI == NULL)
	return;

    if ( !pDLI->getSelected() )
    {
        QMessageBox::warning(this,
                             QString("Laplca Quantization"),
                             QString("Please select a document to perform the clustering on."));
        statusBar()->showMessage( "select document to cluster", 2000 );
	return;
    }

    sculptor_document* pDoc = pDLI->getDoc();
      
    if ( pDoc != NULL && pDoc->getType() == SENSITUS_DOC_PDB )
    {
	((pdb_document*)pDoc)->showAtomicQuantizationDlg();
	m_pRenderFeatureVectors->setChecked( pDoc->getCV_Visible() );
    }
};

/**
 * Show clustering dialog
 */
void sculptor_window::clustering()
{
    // get current documentlistitem
    window_documentlistitem* pDLI = m_pDocumentList->getCurrentItem();

    if (pDLI == NULL)
	return;

    if ( !pDLI->getSelected() )
    {
        QMessageBox::warning(this,
                             QString("Feature Extraction"),
                             QString("Please select a document to perform the clustering on."));
        statusBar()->showMessage( "select document to cluster", 2000 );
	return;
    }

    sculptor_document* pDoc = pDLI->getDoc();

    if (pDoc != NULL)
    {
	pDoc->showClusteringDlgOptions();
	m_pRenderFeatureVectors->setChecked( pDoc->getCV_Visible() );
    }
};

/**
 * FAM: The first selected document gives the initial coordinates for the 
 * clustering of the second selected document (vol/pdb)
 */
void sculptor_window::clusteringFromCoords()
{
    if ( !checkProbeTargetDoc() ) return;

    sculptor_document* pPdb = m_pProbeDoc;
    sculptor_document* pSitus = m_pTargetDoc;
    
    // ? codebook vectors already generated for the first selected document
    if ( (pPdb->getCodebook() ).size()==0)
    {
	svt_exception::ui()->info("Please generate/import the feature vectors of the pdb structure.");
	return;
    }
    
    // ? overwrite codebook vectors if they exist
    if ((pSitus->getCodebook()).size()!=0)
	if ( !svt_exception::ui()->question("The current feature vectors of the document will be lost.\nDo you wish to proceed?"))
	    return;
    
    
    svt_vector4f oOrigin = getOrigin();

    //added new so that codebook information is not written over before accessing in clustering method (due to intro of intermediate class for dialog)
    svt_point_cloud_pdb< svt_vector4<Real64> >* oFirstDocCV = new svt_point_cloud_pdb< svt_vector4<Real64> >();

    pPdb->getCodebookRealPos(*oFirstDocCV, oOrigin);
    
    pSitus->showClusteringDlgOptions( oFirstDocCV );
    m_pRenderFeatureVectors->setChecked( pSitus->getCV_Visible() );       
}

/**
 * Enable or disable rendering of the codebook
 */
void sculptor_window::renderCV()
{
    for(int i=0;i < m_pDocumentList->count(); i++)
    {
	if ( m_pDocumentList->getSelected(i) )
	{
            sculptor_document* pDoc = m_pDocumentList->getDoc(i);

            if (pDoc != NULL)
            {
                pDoc->setCV_Visible( !pDoc->getCV_Visible() );

                if (i == m_pDocumentList->getCurrentIndex())
		    m_pRenderFeatureVectors->setChecked( pDoc->getCV_Visible() );
            }
        }
    }
    updateMenuItems();
};

/**
 * Copy feature vectors between two documents
 */
void sculptor_window::copyCV()
{
    int iDoc = m_pDocumentList->getCurrentIndex();

    if (iDoc>= 0 && iDoc < m_pDocumentList->count())
    {
        sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);
        if (!pDoc)
            return;
        svt_point_cloud< svt_vector4<Real64> > rVecA = pDoc->getCodebook();

	for(int i=0; i< m_pDocumentList->count(); i++)
        {
            window_documentlistitem* pDLI = (*m_pDocumentList)[i];

            if (i != iDoc && pDLI && pDLI->getSelected())
            {
                sculptor_document* pDestDoc = m_pDocumentList->getDoc(i);
                if (!pDestDoc)
		    continue;
                pDestDoc->setCodebook( rVecA );
            }
        }
    }

    svt_redraw();
};


/**
 * create codebook displacement object
 */
void sculptor_window::createCVDisplacementObj(sculptor_document &oDockDoc, sculptor_document &oTargetDoc)
{
    
    //get the selected documents codebook vectors	
    svt_vector4f oOrigin = getOrigin();

    if (m_pCVDisplacementObj==NULL)
   	m_pCVDisplacementObj = new sculptor_codebook_displacement(&oDockDoc, &oTargetDoc, oOrigin, m_pScene->getDataTopNode());
    else
    {
	m_pCVDisplacementObj->setOrigin(oOrigin);
	m_pCVDisplacementObj->setTail(&oDockDoc);
	m_pCVDisplacementObj->setHead(&oTargetDoc);
    }
    m_pCVDisplacementObj->setAjustDimensions(false);

    //remove the CVDispSrc from all the other documents
    for (int i=0; i<m_pDocumentList->count(); i++)
        m_pDocumentList->getDoc(i)->setCVDispSrc( SCULPTOR_CVDISP_NONE );
    
    oDockDoc.setCVDispSrc( SCULPTOR_CVDISP_TAIL );
    oTargetDoc.setCVDispSrc( SCULPTOR_CVDISP_HEAD );
    
    m_pRenderDisplacementArrows->setChecked( TRUE );
}

/**
 * Update the Codebook displacement representation : position, color etc...
 */
void sculptor_window::updateCVDisplacementObj()
{
    if (m_pCVDisplacementObj!=NULL)
    {
        //nice dark green : 0.01,0.54,0.25
        svt_color * pColor = new svt_color(0.75,0,0);
        m_pCVDisplacementObj->setSphereColor(pColor);
        m_pCVDisplacementObj->setCylinderColor(pColor);
        m_pCVDisplacementObj->setConeColor(pColor);
        
        m_pCVDisplacementObj->drawGL();
    }
}

/**
 * Enable and disable rendering of the displacement
 */

void sculptor_window::renderDisplacement( bool bFromFlexing)
{
    if (bFromFlexing)
       m_pRenderDisplacementArrows->setChecked(m_pFlexFittingDlg->getShowDisp() ); 
    
    // delete the previous arrows 
    if (m_pCVDisplacementObj!=NULL)
    {
        m_pCVDisplacementObj->setVisible(false);
	m_pCVDisplacementObj->setTail(NULL);
	m_pCVDisplacementObj->setHead(NULL);
    }

    //if click on the check item - then delete and uncheck item
    if (m_pRenderDisplacementArrows->isChecked() )
    {
        if (m_pFlexFittingDlg!=NULL)
            m_pFlexFittingDlg->setShowDisp(true);
    }
    else
    {
        if (m_pFlexFittingDlg!=NULL)
            m_pFlexFittingDlg->setShowDisp(false);
        return;
    }
   
    //error checking 
    sculptor_document *oDockDoc=NULL;
    sculptor_document *oTargetDoc=NULL;
    if (m_pFlexFittingDlg!=NULL && m_pFlexFittingDlg->isVisible() && m_pFlexFittingDlg->getDockDoc() != NULL && m_pFlexFittingDlg->getTargetDoc() !=NULL)
    {// if the flexing dialog was already opened 
        oDockDoc = m_pFlexFittingDlg->getDockDoc();
        oTargetDoc =  m_pFlexFittingDlg->getTargetDoc();
        m_pFlexFittingDlg->raise();
    }
    else
        getDocsFromSelection(&oDockDoc, &oTargetDoc);
    
    //check if two documents have been selected 
    if (oDockDoc == NULL || oTargetDoc == NULL)
    {
	m_pRenderDisplacementArrows->setChecked( FALSE );
        svt_exception::ui()->info("Please select two documents with computed feature vectors.");
	return;
    }
    
    //check if the CV are computed
    if (oDockDoc->getCodebook().size()==0)
    {
	m_pRenderDisplacementArrows->setChecked( FALSE );
        svt_exception::ui()->info("Please compute the feature vectors of the first selected pdb document.");
        return;
    }
    
    if (oTargetDoc->getCodebook().size()==0)
    {
	m_pRenderDisplacementArrows->setChecked( FALSE );
        svt_exception::ui()->info("Please compute the feature vectors of the situs map (if any) or of the second selected pdb document!");
        return;
    }
    
    //show the arrows
    if (m_pRenderDisplacementArrows->isChecked())
    {
	createCVDisplacementObj(*oDockDoc, *oTargetDoc); 
	updateCVDisplacementObj();
	m_pCVDisplacementObj->setVisible(true);
    }
}

/**
 * Compute RMSD of the corresponding CV
 * \param oFirst a sculptor document with computed CV and eventually a match
 * \param oSecond a sculptor document with computed CV
 * \return the value of the RMSD between the CV of the two documents considering the match
 */
Real64 sculptor_window::computeCvRMSD(sculptor_document &oFirst, sculptor_document &oSecond)
{
    vector<int> oMatch;
    long int iMatchTargetID = 0;
    long int iMatchProbeID = 0;
    long int iThisTargetID = oSecond.getCodebookID();
    long int iThisProbeID = oFirst.getCodebookID();
    
    //get match
    if (oFirst.getType() == SENSITUS_DOC_PDB && (oFirst.getTransformCount()>0) )
    {
        oMatch = *( oFirst.getCurrentMatch() );
        iMatchTargetID = oFirst.getCurrentMatchTargetID();
        iMatchProbeID = oFirst.getCurrentMatchProbeID();
    }

    //printf("DEBUG: IDs: (%li,%li) = (%li,%li)\n", iMatchProbeID, iMatchTargetID, iThisProbeID, iThisTargetID );

    if (iMatchProbeID != iThisProbeID || iMatchTargetID != iThisTargetID )
        oMatch.clear();

    // calculate origin
    sculptor_document* pDocOrg = m_pDocumentList->getDoc(1);
    svt_vector4<Real64> oOrigin;
    if (pDocOrg)
    {
        svt_vector4f oVec = pDocOrg->getRealPos();
        oVec.multScalar( 1.0E8f );
        oOrigin = oVec;
    }
    
    //get CV
    svt_point_cloud_pdb<svt_vector4<Real64> > oCvFirst;
    oFirst.getCodebookRealPos(oCvFirst, oOrigin );
    
    svt_point_cloud_pdb<svt_vector4<Real64> > oCvSecond;
    oSecond.getCodebookRealPos(oCvSecond, oOrigin);
    
    //Cv of the second with the match applied
    svt_point_cloud_pdb<svt_vector4<Real64> > oCvSecondMatched;
    
    // if not available match
    if (oMatch.size()==0)
    {
        oCvSecondMatched = oCvSecond;
        
         unsigned int iCVCountProbe = oCvFirst.size();
         unsigned int iCVCountTarget = oCvSecondMatched.size();
            
        if (iCVCountProbe>iCVCountTarget)
            for (unsigned int iIndexCV = iCVCountProbe-1; iIndexCV >= iCVCountTarget; iIndexCV--)
                oCvFirst.delPoint(iIndexCV);
        else // iCVCountProbe < iCVCountTarget
            for (unsigned int iIndexCV = iCVCountTarget-1; iIndexCV >= iCVCountProbe; iIndexCV--)
                oCvSecondMatched.delPoint(iIndexCV);
    }
    else
    {
        for (unsigned int iIndex=0; iIndex<oMatch.size(); iIndex++)
            oCvSecondMatched.addPoint( oCvSecond[ oMatch[iIndex] ] );
    }
    
    Real64 fRMSD;
    fRMSD = oCvFirst.rmsd( oCvSecondMatched );
    
    return fRMSD;
};



/**
 * Cleanup the result stack, remove abiguous results.
 */
void sculptor_window::cleanResults( svt_point_cloud< svt_vector4<Real64> >& /*oCV_Mono*/, svt_point_cloud< svt_vector4<Real64> >& /*oCV_Olig*/, vector< svt_matchResult< Real64 > >& rResults, unsigned int iMaxCount )
{
    svt_point_cloud< svt_vector4<Real64> > oTransCV;
    unsigned int iNum,i,j;

    iNum = rResults.size();

    // re-sort
    /*Real64 fCVRMSD = 1.0E10;
    for(i=0; i<iNum; i++)
    {
        svt_matrix4<Real64> oMatrix = rResults[i].getMatrix();

	oTransCV = rResults[i].getMatrix() * oCV_Mono;
	fCVRMSD = oTransCV.rmsd_NN( oCV_Olig );
	rResults[i].setScore( fCVRMSD );
    }
    sort( rResults.begin(), rResults.end() );*/

    // filter
    vector< svt_matchResult< Real64 > > aFinal;
    vector< svt_vector4<Real64> > aComs;
    svt_vector4<Real64> oTransVec;
    for(i=0; i<iNum; i++)
    {
        oTransVec = rResults[i].getMatrix().translation();

	for(j=0; j<aComs.size(); j++)
	{
	    if (aComs[j].distance( oTransVec ) < 0.01)
		break;
	}

	if (j >= aComs.size() && aFinal.size() < iMaxCount)
	{
	    aComs.push_back( oTransVec );
	    aFinal.push_back( rResults[i] );
	}
    }

    rResults.clear();
    for(i=0; i<aFinal.size(); i++)
        rResults.push_back( aFinal[i] );
};

/**
 * Creates a new pdb_document to hold the flexed structure
 *\param pFilename a string holding the filename of the original structure: the structure to be flexed
 *\param rPDB contains the coordinates obtained during flexing
 *\param oDocOrigin the realPos(origin) of the initial file
 *\return int value represents the index of the new created file in the list of documents  
 */
int sculptor_window::createDocForFlexedStruct(const char * pFilename,
                                              svt_point_cloud_pdb< svt_vector4<Real64> >* rPDB,
                                              bool bDynamic)
{
    // a new document is created to hold the flexed structure
    pdb_document* pDoc = new pdb_document(m_pScene->getDataTopNode(), string(pFilename), 0, "", 0, rPDB);
    
    pDoc->setDynamic(bDynamic);
    
    svt_vector4f oDocOrigin = pDoc->getRealPos();
    oDocOrigin.multScalar( 1.0E8f );
    pDoc->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
    pDoc->storeInitMat();
    pDoc->setChanged();
    
    // add it to the document lists
    int iNewDoc = addDoc( (sculptor_document*)pDoc );
    svtout << "Document \"" << pDoc->getDisplayName() <<  "\" has been created to hold the flexed structure." << endl;
    
    return iNewDoc; 
};

/**
 * get the origin of the system
 */
svt_vector4f sculptor_window::getOrigin()
{ 
    int iOrDoc = -1;
    int i;
    svt_vector4f oOrigin;

    // get the origin. this is the position of the first situs or pdb file in the list
    for ( i=0; i<m_pDocumentList->count(); i++)
	if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB || m_pDocumentList->getDocType(i) == SENSITUS_DOC_SITUS)
    {
	iOrDoc = i;
	i = m_pDocumentList->count();
    }

    if (iOrDoc != -1)
	oOrigin = m_pDocumentList->getDoc(iOrDoc)->getRealPos();
    else
	oOrigin = svt_vector4f(0.0f,0.0f,0.0f);

    return oOrigin;
};

/**
 * based on the number of selected files, the flexing documents (Dock, target) will be determined :
 * 1) no or one selected file: use the documents indicated by the Dock and TArget iconds
 * 2) 2 files selected: (one pdb and one situs-flex) or (two pdb - morph)
 * \param oFirst - the structure that will be morphed
 * \param oSecond - the target doc - target codebook vectors 
 */
void sculptor_window::getDocsFromSelection(sculptor_document **oFirst, sculptor_document **oSecond)
{
    unsigned int iCountSelectedPDBDoc=0;
    unsigned int iCountSelectedSitusDoc=0;
    
    for(int i=1;i < m_pDocumentList->count(); i++)
    {
	if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
	{
	    if (*oFirst==NULL)
	       *oFirst = m_pDocumentList->getDoc(i);
	    else if (*oSecond==NULL) 
	       *oSecond = m_pDocumentList->getDoc(i);
	    
            iCountSelectedPDBDoc++;
        }
	else if ( m_pDocumentList->getSelected(i) && m_pDocumentList->getDocType(i) == SENSITUS_DOC_SITUS )
	{
	    if (*oSecond==NULL)
		*oSecond = m_pDocumentList->getDoc(i);
	    iCountSelectedSitusDoc++;
	}
    }
    
    if (iCountSelectedPDBDoc + iCountSelectedSitusDoc <= 1) 
    {
	// since no situs, pdb file has been selected use docDoc and targetDoc
	if ( !checkProbeTargetDoc() )
	    return;

	*oFirst = m_pProbeDoc;
	*oSecond = m_pTargetDoc;
    }
    else 
    {
	if (iCountSelectedPDBDoc==1 && iCountSelectedSitusDoc==1)
	    statusBar()->showMessage("Two selected files: pdb and situs ",2000);
	else if (iCountSelectedPDBDoc==2 && iCountSelectedSitusDoc==0)
	    statusBar()->showMessage("Two selected pdb files ", 2000);
	else
	{
	    svt_exception::ui()->info("Confusing Selection!\nPlease indicate the input by:\n   1) Choosing the probe molecule and the target map OR\n   2) Selecting the probe molecule and target map OR\n   3) Select two probe molecules");
	    //statusBar()->showMessage( "Confusing selection!", 2000 ); // come on, with the dialog box we don't need the status bar message
            *oFirst=NULL;
            *oSecond=NULL;
	}
    }
} 


/**
 * calculate the correlation coefficient
 */
void sculptor_window::calcCorrelation()
{
    situs_document* pFirst = NULL;
    situs_document* pSecond = NULL;
    QString oString;

    for(int i=1; i < m_pDocumentList->count() && pSecond == NULL; i++)
    {
        if (m_pDocumentList->getSelected(i))
        {
            if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB )
            {
                svt_exception::ui()->info("The cross correlation coefficient can be computed only between two density maps.\nIn the 'Structure' menu, use 'Blur' to convert PDB atomic coordinates into density values.");
                return;
            }
            else if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_SITUS )
            {
                if (pFirst == NULL)
                    pFirst = (situs_document*)m_pDocumentList->getDoc(i);
                else if (pSecond == NULL)
                    pSecond = (situs_document*)m_pDocumentList->getDoc(i);
            }
        }
    }

    // ok, now calculate the cc
    if (pFirst != NULL && pSecond != NULL && pFirst != pSecond)
    {
	//get volumes
        svt_volume<Real64 > *oVOLFirst = pFirst->getVolume();
        svt_volume<Real64 > *oVOLSecond = pSecond->getVolume();

	Real64 fOccupiedVoxelsFirst  = oVOLFirst->getOccupied( EPSILON );
	Real64 fOccupiedVoxelsSecond = oVOLSecond->getOccupied( EPSILON );

	//compute correlation from the small to the large volume (first volume used as a mask)
	Real64 fCC;
	if (fOccupiedVoxelsFirst < fOccupiedVoxelsSecond)
	    fCC = oVOLFirst->correlation(*oVOLSecond);
	else
	    fCC = oVOLSecond->correlation(*oVOLFirst);


	if (fCC > -1.0e9) // could be negative if error occured
	{
	    oString.sprintf("CC between the two volumes: %5.6f", fCC);
	    svt_exception::ui()->info(QtoStd(oString).c_str());
	}
    } 
    else
	svt_exception::ui()->info("Please select two density maps!");
};

/**
 * show the rmsd dialog 
 */
void sculptor_window::showRmsdDlg()
{
    if (m_pRMSDDlg == NULL)
        m_pRMSDDlg = new dlg_rmsd(this);
    
    m_pRMSDDlg->updateUi();
    m_pRMSDDlg->show();
};

/**
 * Export codebook
 */
void sculptor_window::exportCodebook()
{
    int iDoc = m_pDocumentList->getCurrentIndex();

    if (iDoc> 0 && iDoc < m_pDocumentList->count())
    {
        // get the selected document
        sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);
        if (!pDoc)
            return;

        // open file chooser dialog
        QFileDialog oFD(this);
        oFD.setNameFilter( m_oFilterCBK );
        oFD.selectNameFilter( m_oFilterCBK );
        oFD.setFileMode( QFileDialog::AnyFile );
        oFD.setViewMode( QFileDialog::List );

        oFD.setDirectory( m_oCurrentDir );

        QString oFileA, oFileB;

        if ( oFD.exec() == QDialog::Accepted )
        {
	    QStringList oFiles = oFD.selectedFiles();
	    if (!oFiles.isEmpty())
                oFileA = oFiles[0];

            pDoc->exportCodebook( oFileA, getOrigin() );
        }

    }
    else
    {
	QMessageBox::information(this,
                                 QString("Export Codebook"),
                                 QString("Please select a document from where you would like to export the codebook vectors!") );
    }
};

/**
 * Import codebook
 */
void sculptor_window::importCodebook()
{
    int iDoc = m_pDocumentList->getCurrentIndex();

    if (iDoc> 0 && iDoc < m_pDocumentList->count())
    {
        // get the selected document
        sculptor_document* pDoc = m_pDocumentList->getDoc(iDoc);
        if (!pDoc)
            return;

        // open file chooser dialog
        QFileDialog oFD(this);
        oFD.setNameFilter( m_oFilterCBK );
        oFD.selectNameFilter( m_oFilterCBK );
        oFD.setFileMode( QFileDialog::ExistingFile );
        oFD.setViewMode( QFileDialog::List );

        oFD.setDirectory( m_oCurrentDir );

        QString oFileA, oFileB;

        if ( oFD.exec() == QDialog::Accepted )
        {
	    QStringList oFiles = oFD.selectedFiles();
            if (!oFiles.isEmpty())
                oFileA = oFiles[0];

            pDoc->importCodebook( oFileA, getOrigin() );

            m_oCurrentDir = oFD.directory();

        }

    }
    else
    {
	QMessageBox::information(this,
                                 QString("Import Codebook"),
                                 QString("Please select a document for which you would like to import the codebook vectors!") );
    }
};


/**
 * Get the main area into which the 3D widget is placed
 */
QWidget* sculptor_window::getMainWidgetArea()
{
    return m_pRightSplitter;
};

/**
 * set the current Position
 * \param svt_vector4 with the current postion
 */ 
void sculptor_window::setCurrentPosition( svt_vector4<Real64> oPos )
{
    m_oCurrentPosition = oPos;
};

/**
 * get the current Position
 * \return svt_vector4 with the current postion
 */ 
svt_vector4<Real64> sculptor_window::getCurrentPosition()
{
    return m_oCurrentPosition;
};


///////////////////////////////////////////////////////////////////////////////
// molecular dynamics
///////////////////////////////////////////////////////////////////////////////

/**
 * get the loop Option of the first document ( with multiple frames )
 */
int sculptor_window::dynGetLoop()
{
    for(int i=1;i < m_pDocumentList->count(); i++)
    {
        if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
        {
            pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

            return ((svt_pdb*)pPDB->getNode())->getLoop();
        }
    }
    return 0;
}

/**
 * set the loop Option of the first document ( with multiple frames )
 */
void sculptor_window::dynSetLoop(int iLoop)
{
    for(int i=1;i < m_pDocumentList->count(); i++)
    {
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
        {
            pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

            ((svt_pdb*)pPDB->getNode())->setLoop( iLoop );
            pPDB->updatePropDlgDynLoop( iLoop );
        }
    }
}

/**
 * get the speed to all "dynamic" documents
 */
float sculptor_window::dynGetSpeed()
{
    float fMinSpeed = 1.0E10f;
    float fSpeed;

    for (int i=1; i<m_pDocumentList->count(); i++)
    {
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
        {
            pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));
            fSpeed = ((svt_pdb*)pPDB->getNode())->getSpeed( );

            if (fSpeed < fMinSpeed)
                fMinSpeed = fSpeed;
        }
    }

    return fMinSpeed;
}

/**
 * set the speed to all "dynamic" documents
 */
void sculptor_window::dynSetSpeed(float fSpeed)
{
    for (int i=1; i<m_pDocumentList->count(); i++)
    {
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
        {
            pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

            ((svt_pdb*)pPDB->getNode())->setSpeed( fSpeed );

            if (fSpeed != 0.0f)
                pPDB->updatePropDlgDynTPS( (int)fabs(fSpeed) );
        }
    }
}

/**
 * set the Timestep to iTimestep; if iTimestep larger that the maxTimestep then just set the modulo
 * \param iTimestep the Timestep
 */
void sculptor_window::dynSetTimestep(int iTimestep)
{
    int iMaxTimestep, iDocTimestep;

    for(int i=1;i < m_pDocumentList->count(); i++)
    {
        if (m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
        {
            pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

            iMaxTimestep = pPDB->getPointCloud().getMaxTimestep();

            if (iTimestep>=iMaxTimestep)
                iDocTimestep = iTimestep % iMaxTimestep;
            else
                iDocTimestep = iTimestep;

            pPDB->getPointCloud().setTimestep( iDocTimestep );
            pPDB->updatePropDlgDynTimestep(iDocTimestep, iMaxTimestep);
            ((svt_pdb*)pPDB->getNode())->rebuildDL();
        }
    }
}

/**
 * get the max of the current Timestep of all documents
 * \return the largest getTimestep
 */
int sculptor_window::dynGetTimestep()
{
    int iMaxTimestep = 0;
    int iMaxTimestepDoc;

    for(int i=1;i < m_pDocumentList->count(); i++)
    {
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
        {
            pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

            iMaxTimestepDoc = pPDB->getPointCloud().getTimestep();

            if (iMaxTimestepDoc > iMaxTimestep)
                iMaxTimestep = iMaxTimestepDoc;
        }
    }
    return iMaxTimestep;
}

/**
 * get maxTimestep of all documents
 * \return the largest maxTimestep
 */
int sculptor_window::dynGetMaxTimestep()
{
    int iMaxTimestep = 0;
    int iMaxTimestepDoc;

    for(int i=1;i < m_pDocumentList->count(); i++)
    {
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
        {
            pdb_document* pPDB = (pdb_document*)(m_pDocumentList->getDoc(i));

            iMaxTimestepDoc = pPDB->getPointCloud().getMaxTimestep();

            if (iMaxTimestepDoc > iMaxTimestep)
                iMaxTimestep = iMaxTimestepDoc;
        }
    }
    return iMaxTimestep;
}

/**
 *
 */
void sculptor_window::dynForward()
{
    // convenient: when at the end, restart from the beginning automatically (unless we rock)
    if ( dynGetLoop() != SVT_DYNAMICS_ROCK && dynGetTimestep() == dynGetMaxTimestep()-1 )
        dynSetTimestep(0);

    float fSpeed = dynGetSpeed();

    if (fSpeed == 0.0f)
        dynSetSpeed( fabs(m_fDynPreviousSpeed) );
    else
        dynSetSpeed( fabs(fSpeed) );

    // remember the time the timer was activated
    m_iDynLastTimeout = svt_getToD();
    m_pDynTimer->start( TIMER_INTER );

    // update the buttons in prop_pdb for all pdb documents that have a dynamic data set
    for(int i=1;i < m_pDocumentList->count(); i++)
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
            ((pdb_document*)(m_pDocumentList->getDoc(i)))->updatePropDlgDynButtons(SVT_DYNAMICS_FORWARD);
}

/**
 *
 */
void sculptor_window::dynBackward()
{
    // convenient: when at the beginning, restart from the end automatically (unless we rock)
    if ( dynGetLoop() != SVT_DYNAMICS_ROCK && dynGetTimestep() == 0 )
        dynSetTimestep(dynGetMaxTimestep()-1);

    float fSpeed = dynGetSpeed();

    if (fSpeed == 0.0f)
        dynSetSpeed( -fabs(m_fDynPreviousSpeed) );
    else
        dynSetSpeed( -fabs(fSpeed) );

    // remember the time the timer was activated
    m_iDynLastTimeout = svt_getToD();
    m_pDynTimer->start( TIMER_INTER );

    // update the buttons in prop_pdb for all pdb documents that have a dynamic data set
    for(int i=1;i < m_pDocumentList->count(); i++)
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
            ((pdb_document*)(m_pDocumentList->getDoc(i)))->updatePropDlgDynButtons(SVT_DYNAMICS_BACKWARD);
}

/**
 *
 */
void sculptor_window::dynStop()
{
    // remember the current playback speed
    if ( dynGetSpeed() != 0.0f )
        m_fDynPreviousSpeed = dynGetSpeed();

    dynSetSpeed( 0.0f );
    m_pDynTimer->stop();

    // update the buttons in prop_pdb for all pdb documents that have a dynamic data set
    for(int i=1;i < m_pDocumentList->count(); i++)
        if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
            ((pdb_document*)(m_pDocumentList->getDoc(i)))->updatePropDlgDynButtons(SVT_DYNAMICS_STOPPED);
}

/**
 *
 */
void sculptor_window::dynToStart()
{
    dynStop();
    dynSetTimestep( 0 );
}
/**
 *
 */
void sculptor_window::dynToEnd()
{
    dynStop();
    dynSetTimestep( dynGetMaxTimestep() - 1 );
}

/**
 * adjust playback-speed. check first if we are currently playing backwards (user clicked on play
 * backwards or structure is currently rocking back). this makes sure to not change the current
 * playback direction
 * \param iTPS timesteps per second
 */
void sculptor_window::dynSetTPS(int iTPS)
{
    if ( dynGetSpeed() < 0.0f )
        dynSetSpeed( (float)(-iTPS) );
    else
        dynSetSpeed( (float)iTPS );
}

/**
 * timer
 */
void sculptor_window::dynTimeout( )
{
    int iMaxTimestep = dynGetMaxTimestep()-1;
    int iLoop = dynGetLoop();
    Real64 fSpeed = dynGetSpeed();

    if (m_bMovie)
    {
        //
        // write current frame
        //
        int iCurr = dynGetTimestep();

        char pFname[256];
        if ( iLoop == SVT_DYNAMICS_ROCK && fSpeed < 0)
            sprintf( pFname, "frame_%03i.tga", (iMaxTimestep - iCurr) + iMaxTimestep+1 );
        else
            sprintf( pFname, "frame_%03i.tga", iCurr );

        svtout << "  Writing " << pFname << endl;

        QString oFname = m_oMovieDir + QString(pFname);

        saveScreenshot( oFname );

        //
        // next timestep
        //
        if ( iLoop == SVT_DYNAMICS_ROCK )
        {
            if ( fSpeed > 0.0 )
                ++iCurr;
            else
                --iCurr;

            if ( iCurr > iMaxTimestep )
            {
                dynSetSpeed( -fSpeed );
                iCurr = iMaxTimestep;
            }

            if ( iCurr < 0 )
            {
                m_bMovie = false;

                // update the buttons in prop_pdb for all pdb documents that have a dynamic data set
                for(int i=1;i < m_pDocumentList->count(); i++)
                    if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
                        ((pdb_document*)(m_pDocumentList->getDoc(i)))->updatePropDlgDynMovieButton(false);

                svtout << "done." << endl;
                iCurr = 0;
            }
            dynSetTimestep( iCurr );
        }
        else
        {
            ++iCurr;

            if ( iCurr > iMaxTimestep )
            {
                m_bMovie = false;

                // uncheck movie export buttons
                for(int i=1;i < m_pDocumentList->count(); i++)
                    if ( m_pDocumentList->getDocType(i) == SENSITUS_DOC_PDB && ((pdb_document*)m_pDocumentList->getDoc(i))->getDynamic() )
                        ((pdb_document*)(m_pDocumentList->getDoc(i)))->updatePropDlgDynMovieButton(false);

                svtout << "done." << endl;
            }
            else
            {
                dynSetTimestep( iCurr );
            }
        }
        return;
    }

    // not dynamic or speed set to zero - nothing to do
    if (iMaxTimestep == 0 || fSpeed == 0.0f)
        return;
 

    // check if "the time has come" for a new frame. return if not
    //
    float fDiff = (float)(svt_getToD() - m_iDynLastTimeout) / 1000.0f;
    //
    if ( fabs(fDiff * fSpeed ) >= 1.0f )
        m_iDynLastTimeout = svt_getToD();
    else
        return;


    // set new timestep
    int iCurrent = dynGetTimestep();
    iCurrent += (int)(fDiff * fSpeed);


    // wrap around/stop playback
    if ( iCurrent < 0 )
    {
        switch( iLoop )
        {
            case SVT_DYNAMICS_LOOP:
                iCurrent = iMaxTimestep;
                break;

            case SVT_DYNAMICS_ROCK:
                dynSetSpeed( fabs(fSpeed) );
                //iCurrent -= (int)(fDiff * (fSpeed));
                iCurrent = 0;
                break;

            default:
                dynStop();
                return;
        }
    }
    else if ( iCurrent > iMaxTimestep )
    {
        switch( iLoop )
        {
            case SVT_DYNAMICS_LOOP:
                iCurrent = 0;
                break;

            case SVT_DYNAMICS_ROCK:
                dynSetSpeed( -fSpeed );
                //iCurrent -= (int)(fDiff * (fSpeed));
                iCurrent = iMaxTimestep;
                break;

            default:
                dynStop();
                return;
        }
    }

    m_iDynLastTimeout = svt_getToD();
    dynSetTimestep( iCurrent );
}

/**
 * Export a movie
 */
void sculptor_window::dynMovie()
{
    QFileDialog oFD(NULL);
    oFD.setFileMode( QFileDialog::DirectoryOnly );
    oFD.setOption(QFileDialog::ShowDirsOnly, true);

    if ( oFD.exec() != QDialog::Accepted )
    {
        // uncheck movie export button
        ((pdb_document*)(m_pDocumentList->getCurrentItem()->getDoc()))->updatePropDlgDynMovieButton(false);
        return;
    }

    QString s = oFD.directory().absolutePath();

    if ( s.isEmpty() )
        return;

    float fSpeed = dynGetSpeed();
    m_oMovieDir = s + QDir::separator();
    m_bMovie = true;

    if ( dynGetLoop() == SVT_DYNAMICS_ROCK )
        dynSetSpeed( fabs( fSpeed ) );

    svtout << "Will export movie frames... ";
};

/**
 * Open the lua editor dialog
 */
void sculptor_window::scriptInitGUI()
{
    // no scripts installed yet? OK, then fill in some default stuff
    if (m_aScriptTitles.size() == 0)
        scriptAddDefault();

    if (m_pLuaEditor == NULL)
    {
        m_pLuaEditor = new dlg_luaeditor( m_pTabArea );
        m_pLuaList   = new dlg_lualist  ( m_pTabArea );
        sculptor_lua_syntax* pSyntax;
        pSyntax = new sculptor_lua_syntax( m_pLuaEditor->getEditor() );
        m_pTabArea->addTab( m_pLuaList, "Script List" );
        m_pTabArea->addTab( m_pLuaEditor, "Script Editor" );

        for (unsigned int i=0; i<m_aScripts.size(); i++)
        {
            m_pLuaEditor->addScriptName( StdtoQ(m_aScriptTitles[i]) );
            m_pLuaList  ->addScriptName( StdtoQ(m_aScriptTitles[i]) );
        }

        m_iCurrentScript = 0;
        m_pLuaEditor->setCurrentScript( m_iCurrentScript, StdtoQ(m_aScripts[m_iCurrentScript]) );
        m_pLuaList  ->setCurrentScript( m_iCurrentScript );

        m_pLuaEditor->initConnections(this);
        m_pLuaList  ->initConnections(this);
    }

    if ( m_oLua.isRunning() )
    {
        m_pLuaEditor->showScriptRunning();
        m_pLuaList  ->showScriptRunning();
    }
    else
    {
        m_pLuaEditor->showScriptRunning(false);
        m_pLuaList  ->showScriptRunning(false);
    }
};

/**
 * Add a simple default script
 */
void sculptor_window::scriptAddDefault()
{
    m_aScriptTitles.push_back( string("New Script") );
    m_aScripts.     push_back( string("-- fill in your lua code here\n") );
}

/**
 * Run the lua code
 */
void sculptor_window::scriptRun()
{
    if (m_pLuaEditor != NULL)
    {
        getScene()->switchToData();

        scriptSave();

        m_pLuaEditor->showScriptRunning();
        m_pLuaList  ->showScriptRunning();

        if (m_pTabArea->currentWidget() == m_pLuaList)
            scriptSelected( m_pLuaList->getCurrentScript() );

        m_pTabArea->setCurrentIndex( 0 );
        m_oLua.init();
        m_oLua.runScript( QtoStd(m_pLuaEditor->getScriptText()).c_str() );

        m_pLuaEditor->showScriptRunning(false);
        m_pLuaList  ->showScriptRunning(false);
    }
};

/**
 * Add a lua script (for example by loading it from the command-line
 * \param oName stl string with the name of the lua script
 * \param oScript stl string with a lua script
 */
void sculptor_window::scriptAdd( string oName, string oScript )
{
    // if a script with that name already exists, then replace the code
    for(unsigned int i=0; i<m_aScriptTitles.size(); i++)
    {
        if ( m_aScriptTitles[i] == oName )
        {
            m_iCurrentScript = i;
            m_pLuaEditor->setCurrentScript( m_iCurrentScript, StdtoQ(m_aScripts[m_iCurrentScript]) );
            m_pLuaList  ->setCurrentScript( m_iCurrentScript );
            scriptRun();

            return;
        }
    }

    m_aScriptTitles.push_back( oName );
    m_aScripts.push_back( oScript );

    scriptSelected( m_aScripts.size()-1 );

    scriptRun();
};

/**
 * Stop the lua interpreter
 */
void sculptor_window::scriptStop()
{
    m_oLua.stopScript();

    if (m_pLuaEditor != NULL)
    {
        m_pLuaEditor->showScriptRunning(false);
        m_pLuaList  ->showScriptRunning(false);
    }
};

/**
 * Save the current scripts
 */
void sculptor_window::scriptSave()
{
    // save the current script and its name
    m_aScripts[m_iCurrentScript]      = QtoStd(m_pLuaEditor->getScriptText());
    m_aScriptTitles[m_iCurrentScript] = QtoStd(m_pLuaEditor->getScriptName());

    // update the script name in the script list
    //m_pLuaEditor->changeScriptName( m_pLuaEditor->getScriptText(), m_iCurrentScript );

    saveRecentList();
};

/**
 * Another script was selected by the user, so save the current one and then make the new one current
 */
void sculptor_window::scriptSelected(int iIndex)
{
    // before switching, save the old script and its name
    m_aScripts[m_iCurrentScript]      = QtoStd(m_pLuaEditor->getScriptText());
    m_aScriptTitles[m_iCurrentScript] = QtoStd(m_pLuaEditor->getScriptName());
    m_pLuaEditor->updateScriptListName( m_iCurrentScript );

    m_iCurrentScript = iIndex;

    m_pLuaEditor->setCurrentScript( m_iCurrentScript, StdtoQ(m_aScripts[m_iCurrentScript]) );
    m_pLuaList  ->setCurrentScript( m_iCurrentScript );
};

/**
 * User would like to add a new script
 */
void sculptor_window::scriptAdded()
{
    // add new default script
    scriptAddDefault();

    // store the new script
    m_pLuaEditor->addScriptName( StdtoQ(m_aScriptTitles[m_aScripts.size()-1]) );
    m_pLuaList  ->addScriptName( StdtoQ(m_aScriptTitles[m_aScripts.size()-1]) );

    // now select the new script
    scriptSelected( m_aScriptTitles.size()-1 );
};

/**
 * User would like to remove the current script
 */
void sculptor_window::scriptRemoved()
{
    // remove script
    m_aScriptTitles.erase( m_aScriptTitles.begin()+m_iCurrentScript );
    m_aScripts.     erase( m_aScripts.begin()     +m_iCurrentScript );

    // remove script from editor and list
    m_pLuaEditor->removeScriptName( m_iCurrentScript );
    m_pLuaList  ->removeScriptName( m_iCurrentScript );


    // script was removed, so put the next script's name in that place

    if (m_aScripts.size() == 0)
    {
        // create default script if last one was removed
        scriptAddDefault();
        m_pLuaEditor->addScriptName( StdtoQ(m_aScriptTitles[m_aScripts.size()-1]) );
        m_pLuaList  ->addScriptName( StdtoQ(m_aScriptTitles[m_aScripts.size()-1]) );
    }

    if (m_iCurrentScript >= m_aScripts.size())
        m_iCurrentScript = m_aScripts.size() - 1;

    m_pLuaEditor->setCurrentScript( m_iCurrentScript,
                                    StdtoQ(m_aScripts[m_iCurrentScript]),
                                    StdtoQ(m_aScriptTitles[m_iCurrentScript]) );
    m_pLuaList  ->setCurrentScript( m_iCurrentScript );

};

/**
 *
 */
void sculptor_window::scriptDown()
{
    if (m_iCurrentScript == m_aScriptTitles.size()-1)
        return;

    m_aScripts[m_iCurrentScript]      = QtoStd(m_pLuaEditor->getScriptText());
    m_aScriptTitles[m_iCurrentScript] = QtoStd(m_pLuaEditor->getScriptName());

    string oStr = m_aScriptTitles[m_iCurrentScript+1];
    m_aScriptTitles[m_iCurrentScript+1] = m_aScriptTitles[m_iCurrentScript];
    m_aScriptTitles[m_iCurrentScript]   = oStr;

    oStr = m_aScripts[m_iCurrentScript+1];
    m_aScripts[m_iCurrentScript+1] = m_aScripts[m_iCurrentScript];
    m_aScripts[m_iCurrentScript]   = oStr;

    m_pLuaList  ->scriptDown( m_iCurrentScript );
    m_pLuaEditor->scriptDown( m_iCurrentScript, StdtoQ(m_aScripts[m_iCurrentScript+1]) );

    ++m_iCurrentScript;
}

/**
 *
 */
void sculptor_window::scriptUp()
{
    if (m_iCurrentScript == 0)
        return;

    m_aScripts[m_iCurrentScript]      = QtoStd(m_pLuaEditor->getScriptText());
    m_aScriptTitles[m_iCurrentScript] = QtoStd(m_pLuaEditor->getScriptName());

    string oStr = m_aScriptTitles[m_iCurrentScript-1];
    m_aScriptTitles[m_iCurrentScript-1] = m_aScriptTitles[m_iCurrentScript];
    m_aScriptTitles[m_iCurrentScript] = oStr;

    oStr = m_aScripts[m_iCurrentScript-1];
    m_aScripts[m_iCurrentScript-1] = m_aScripts[m_iCurrentScript];
    m_aScripts[m_iCurrentScript]   = oStr;

    m_pLuaList  ->scriptUp( m_iCurrentScript );
    m_pLuaEditor->scriptUp( m_iCurrentScript, StdtoQ(m_aScripts[m_iCurrentScript-1]) );

    --m_iCurrentScript;
}

/**
 *
 */
void sculptor_window::scriptSetName(int iIndex, QString oName)
{
    m_aScriptTitles[iIndex] = QtoStd(oName);
    m_pLuaEditor->setScriptName(iIndex, oName);
}

/**
 * User would like to change the name of the current script
 */
void sculptor_window::scriptChangeName()
{
    m_aScriptTitles[m_iCurrentScript] = QtoStd(m_pLuaEditor->getScriptName());
    m_pLuaEditor->updateScriptListName( m_iCurrentScript );
    m_pLuaList  ->updateScriptListName( m_iCurrentScript, m_pLuaEditor->getScriptName() );
};

/**
 * Clear the log window
 */
void sculptor_window::clearLog()
{
    m_pLog->clear();
};

///////////////////////////////////////////////////////////////////////////////
// Abstract GUI Functions.
// This is a group of functions that open standard dialogs like file-open dlgs,
// message-boxes, etc.
///////////////////////////////////////////////////////////////////////////////

/**
 * Opens a file-open-dialog.
 * \param oMessage stl string with the title of the file dialog
 * \return returns the file that was selected by the user or an empty string in case the user cancelled the operation
 */
string sculptor_window::guiFileOpenDlg(string oMessage)
{
    // open file chooser dialog
    QFileDialog oFD(this, QString( oMessage.c_str() ));
    oFD.setNameFilter( m_oFilterALL );
    oFD.setFileMode( QFileDialog::ExistingFiles );
    oFD.setDirectory( m_oCurrentDir );

    string oReturn;

    QString oFileName;
    QFileInfo oFileInfo;
    if ( oFD.exec() == QDialog::Accepted )
    {
	QStringList oFiles = oFD.selectedFiles();
	if (!oFiles.isEmpty())
            oFileName = oFiles[0];
        oFileInfo.setFile( oFileName );
        m_oCurrentDir = oFileInfo.absoluteDir();

        oReturn = QtoStd(oFileName);
    }

    return oReturn;
};
/**
 * Opens a file-save-dialog.
 * \param oMessage stl string with the title of the file dialog
 * \return returns the file that was selected by the user or an empty string in case the user cancelled the operation
 */
string sculptor_window::guiFileSaveDlg(string oMessage)
{
    // open file chooser dialog
    QFileDialog oFD(this, QString( oMessage.c_str() ));
    oFD.setNameFilter( m_oFilterALL );
    oFD.setFileMode( QFileDialog::AnyFile);
    oFD.setDirectory( m_oCurrentDir );

    string oReturn;

    QString oFileName;
    QFileInfo oFileInfo;
    if ( oFD.exec() == QDialog::Accepted )
    {
	QStringList oFiles = oFD.selectedFiles();
	if (!oFiles.isEmpty())
	    oFileName = oFiles[0];
        oFileInfo.setFile( oFileName );
        m_oCurrentDir = oFileInfo.absoluteDir();

        oReturn = QtoStd(oFileName);
    }

    return oReturn;
};

/**
 * Opens a warning message box dialog.
 * \param oCaption   stl string with the caption of the dialog
 * \param oMessage   stl string with the actual message
 * \param oStatusbar stl string with the statusbar message
 */
void sculptor_window::guiWarning(string oCaption, string oMessage, string oStatusbar)
{
    QMessageBox::warning(this, QString( oCaption.c_str() ),
                         QString( oMessage.c_str() ));
    statusBar()->showMessage( oStatusbar.c_str(), 2000 );
}

/**
 * Opens an info message box dialog.
 * \param oCaption   stl string with the caption of the dialog
 * \param oMessage   stl string with the actual message
 * \param oStatusbar stl string with the statusbar message
 */
 void sculptor_window::guiInfo(string oCaption, string oMessage, string oStatusbar)
{
    QMessageBox::information(this, QString( oCaption.c_str() ),
                         QString( oMessage.c_str() ));
    statusBar()->showMessage( oStatusbar.c_str(), 2000 );
}

/**
 * Opens an info message box dialog that asks the user a yes/no question.
 * \param oCaption   stl string with the caption of the dialog
 * \param oMessage   stl string with the actual message
 * \return boolean (true if the user answered yes, false if no)
 */
bool sculptor_window::guiYesNo(string oCaption, string oMessage)
{
    if (QMessageBox::question(this, QString( oCaption.c_str() ), QString( oMessage.c_str() ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No  )
        return false;
    else
        return true;
}

/**
 * Opens an info message box dialog that asks the user to input a string.
 * \param oCaption   stl string with the caption of the dialog
 * \param oMessage   stl string with the actual message
 * \param oDefault   stl string with the default answer, given if the user cancels the dialog
 * \return string with the answer, empty if the user pressed cancel.
 */
string sculptor_window::guiQuestion(string oCaption, string oMessage, string oDefault)
{
    string oAnswer = oDefault;
    bool bOK;
    QString oText = QInputDialog::getText(0, QString(oCaption.c_str()), QString(oMessage.c_str()), QLineEdit::Normal, QString(oDefault.c_str()), &bOK );
    if ( bOK && !oText.isEmpty() )
        oAnswer = QtoStd(oText);

    return oAnswer;
};

/**
 * Opens an info message box dialog that asks the user to input a value.
 * \param oCaption   stl string with the caption of the dialog
 * \param oMessage   stl string with the actual message
 * \param fDefault   default value, if the user cancels the dialog
 * \return double with the answer, 0 if the user pressed cancel.
 */
Real64 sculptor_window::guiGetValue(string oCaption, string oMessage, Real64 fDefault)
{
    Real64 fAnswer = fDefault;
    bool bOK;
    QString oDefault; oDefault.sprintf("%f", fDefault);
    QString oText = QInputDialog::getText(0, QString(oCaption.c_str()), QString(oMessage.c_str()), QLineEdit::Normal, QString(oDefault), &bOK );
    if ( bOK && !oText.isEmpty() )
        fAnswer = oText.toDouble();

    return fAnswer;
}

/**
 *  trigger the ga timer
 */
void triggerGaCylTimer()
{
	  svt_wakeupWindow(1);
    g_pWindow->getGaCylTimer()->start( 1 );
    svt_sleep(2000);
    g_pWindow->getGaCylTimer()->start( 100000 );
}

/*
 *
 */
void sculptor_window::popupVolumeMenu(QPoint oPoint)
{
    m_pVolumeContextMenu->popup(oPoint);
}

/*
 *
 */
void sculptor_window::popupStructureMenu(QPoint oPoint)
{
    m_pStructureContextMenu->popup(oPoint);
}

/*
 *
 */
void sculptor_window::updateMenuItems()
{
    window_documentlistitem* pDLI = m_pDocumentList->getCurrentItem();

    if (pDLI == NULL)
	return;

    m_pView->blockSignals( TRUE );
    m_pView->setChecked( pDLI->getVisible() );
    m_pView->blockSignals( FALSE );

    m_pMove->blockSignals( TRUE );
    m_pMove->setChecked( pDLI->getMoved() );
    m_pMove->blockSignals( FALSE );

    m_pRenderFeatureVectors->blockSignals( TRUE );
    m_pRenderFeatureVectors->setChecked( pDLI->getDoc()->getCV_Visible() );
    m_pRenderFeatureVectors->blockSignals( FALSE );
}

/**
 *
 */
void sculptor_window::updateMenus(bool bSceneSelected, int iPDBsSelected, int iVolsSelected)
{
    //DEBUG//printf(" updateMenus[scene: %i pdbs: %i vols: %i]", (int)bSceneSelected, iPDBsSelected, iVolsSelected);
    if (bSceneSelected)
    {
        // if scene selected, disable volume and structure menues
        m_pStructureMenu->setEnabled( FALSE );
        m_pVolumeMenu->setEnabled( FALSE );

        // check/uncheck menu item according to state of current document
        updateMenuItems();
    }
    else
    {
        // enable/disable menus and their menu actions depending on the number of docs selected

	if ( iPDBsSelected > 0 && iVolsSelected > 0 )
	{
	    updateMenuItems();

	    m_pVolumeMenu->setEnabled( FALSE );
	    m_pStructureMenu->setEnabled( FALSE );
	}
	else if (iPDBsSelected > 0)
	{
	    m_pVolumeMenu->setEnabled( FALSE );

	    m_pPDBAction_Table             ->setEnabled( iPDBsSelected == 1 );
	    m_pPDBAction_Sphere            ->setEnabled( iPDBsSelected == 1 );
	    m_pPDBAction_RMSD              ->setEnabled( iPDBsSelected == 2 );
	    m_pPDBAction_Blur              ->setEnabled( iPDBsSelected == 1 );
	    m_pPDBAction_Extract           ->setEnabled( iPDBsSelected == 1 );
	    m_pPDBAction_Symmetry          ->setEnabled( iPDBsSelected == 1 );
	    m_pPDBAction_Merge             ->setEnabled( iPDBsSelected  > 1 );
	    m_pPDBAction_PDF               ->setEnabled( iPDBsSelected  > 0 );
	    m_pPDBAction_DiffPDF           ->setEnabled( iPDBsSelected  > 1 );

	    m_pStructureMenu->setEnabled( TRUE );
	}
	else if (iVolsSelected > 0)
	{
	    m_pStructureMenu->setEnabled( FALSE );

	    m_pVolAction_Table             ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Mapexp            ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Hist              ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Occvox            ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_ChangeVW          ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_ChangeOrig        ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_SwapAxes          ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Normalize         ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Threshold         ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Scale             ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Crop              ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Floodfill         ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_MultiPointFloodfill->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Gaussian          ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Laplacian         ->setEnabled( iVolsSelected == 1 );
	    m_pVolAction_Corr              ->setEnabled( iVolsSelected == 2 );
	    m_pVolAction_DPSV_filter       ->setEnabled( iVolsSelected == 1 );
	    //m_pVolAction_BilateralFilter   ->setEnabled( iVolsSelected == 1 );
	    m_pPDBAction_RMSD              ->setEnabled( FALSE );
	    m_pVolumeMenu                  ->setEnabled( TRUE );
	}
    }
}

/**
 *
 */
bool sculptor_window::getDockToLaplace()
{
    return m_pDockToLaplace->isChecked();
}

/**
 *
 */
void sculptor_window::sShowPreferencesDialog()
{
    if (m_pPrefDlg == NULL)
        m_pPrefDlg = new dlg_preferences(this);

    m_pPrefDlg->raise();
    m_pPrefDlg->show();
}

/**
 *
 */
void sculptor_window::setRestoreWindowStateOnStart(bool bRestoreWindowStateOnStart)
{
    m_bRestoreWindowStateOnStart = bRestoreWindowStateOnStart;

    // save this setting to disc
    saveConfigValue("RESTORE_WINDOW_STATE", (int)m_bRestoreWindowStateOnStart, "MAIN_WINDOW");
}

/**
 *
 */
bool sculptor_window::getRestoreWindowStateOnStart()
{
    return m_bRestoreWindowStateOnStart;
}

/**
 *
 */
void sculptor_window::restoreDefaultWindowState()
{
    if (m_oDefaultWindowState.size() > 0)
    {
        restoreState(m_oDefaultWindowState);
        resize(m_oDefaultWindowSize);
    }
}

/**
 *
 */
void sculptor_window::setAllowShaderPrograms(bool bEnabled)
{
    svt_setAllowShaderPrograms(bEnabled);

    saveConfigValue("ALLOW_SHADER_PROGRAMS", (int)bEnabled);

    vector<sculptor_document*> oPDBDocs = m_pDocumentList->getDocuments(false, SENSITUS_DOC_PDB);
    if ( oPDBDocs.size() > 0 )
        for ( vector<sculptor_document*>::iterator itr = oPDBDocs.begin(); itr != oPDBDocs.end(); ++itr )
            ((pdb_document*)(*itr))->setAllowShaderPrograms();

    vector<sculptor_document*> oSITUSDocs = m_pDocumentList->getDocuments(false, SENSITUS_DOC_SITUS);
    if ( oSITUSDocs.size() > 0 )
        for ( vector<sculptor_document*>::iterator itr = oSITUSDocs.begin(); itr != oSITUSDocs.end(); ++itr )
            ((situs_document*)(*itr))->setAllowShaderPrograms();

    getScene()->setAllowAmbientOcclusion(bEnabled);

    if ( m_pLightingDlg != NULL )
        m_pLightingDlg->setAOGroupBoxEnabled(bEnabled);
}

/**
 *
 */
void sculptor_window::setAllowAmbientOcclusion(bool bEnabled)
{
    // save this setting to disc
    saveConfigValue("ALLOW_AMBIENT_OCCLUSION", (int)bEnabled);

    svt_setAllowAmbientOcclusion(bEnabled);

    sculptor_scene* pScene = getScene();
    if ( pScene != NULL )
        pScene->setAllowAmbientOcclusion(bEnabled);
}
