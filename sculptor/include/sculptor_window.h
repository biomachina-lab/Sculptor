/*-*-c++-*-*****************************************************************
                          sculptor_window.h
                          -----------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_WINDOW_H
#define SCULPTOR_WINDOW_H

// qt4 includes
#include <QMainWindow>
#include <QIcon>
#include <QDir>
class QMenu;
class QEvent;
class QCloseEvent;
class QTabWidget;
class QToolButton;
// svt includes
#include <svt_point_cloud_pdb.h>
// sculptor includes
class window_documentlist;
class dlg_about;
class dlg_featuredocking;
class dlg_fetchpdb;
class dlg_flexing;
class dlg_force;
class dlg_gacylinder;
class dlg_gamultifit;
class dlg_lighting;
class dlg_luaeditor;
class dlg_lualist;
class dlg_plot;
class dlg_preferences;
class dlg_refine;
class dlg_rmsd;
class dlg_transform;
class dlg_wait;
class dlg_DPSV_filter;
class sculptor_splitter;
class sculptor_scene;
class sculptor_codebook_displacement;
#include <sculptor_qwtplotcurve.h>
#include <sculptor_document.h>
#include <sculptor_lua.h>
#include <sculptor_gamultifit.h>
#include <sculptor_gacylinder.h>
class ForceDlg;
class sculptor_log;
class sculptor_force_algo;
class scene_document;


/**
 * The main application window.
 * @author Stefan Birmanns
 */
class sculptor_window: public QMainWindow
{
    Q_OBJECT

protected:

    // the sculptor svt scene
    sculptor_scene* m_pScene;

    // the sculptor scene document
    scene_document* m_pSceneDoc;

    // the central widget of QMainWindow
    window_documentlist * m_pDocumentList;

    // previously selected and focused items
    int m_iDocumentListLastSelected;
    int m_iDocumentListLastFocused;

    // X coordinate of last click in document list
    // (useful to decide what to do on double click)
    int m_iDocumentListClickXPos;

    // the force properties dialog
    dlg_force* m_pForceDlg;

    // the force feedback algorithm
    sculptor_force_algo* m_pForceAlgo;

    // transform vars
    dlg_transform* m_pTransformDlg;
    int m_iOldTransX;
    int m_iOldTransY;
    int m_iOldTransZ;
    int m_iOldRotX;
    int m_iOldRotY;
    int m_iOldRotZ;

    // current directory = directory of last loaded doc
    QDir m_oCurrentDir;

    // action for center mode
    QAction* m_pCenterMode;
    // action for visibility
    QAction* m_pView;
    // menu action for toggle property dialog area
    QAction* m_pProperties;
    // menu action for move mode
    QAction* m_pMove;
    // menu action for rendering of codebook vectors
    QAction* m_pRenderFeatureVectors;
    // menu action for automatically adjusting probe/solution color
    QAction* m_pAutoColor;
    // menu action for orthographic projection
    QAction* m_pOrthoMode;
    // toolbar action of orthographic projection
    QAction* m_pOrthoToolbutton;
    // menu action for 3D cursor on/off
    QAction* m_pPipette;
    // menu action for 3D coordinate system on/off
    QAction* m_pCoordinateSystem;
    // menu action for high contrast palette
    QAction* m_pHighContrast;
    bool     m_bHighContrast;
    // menu action capped clip-surfaces
    QAction* m_pCappedClip;
    // menu action for to laplace data docking
    QAction* m_pDockToLaplace;

    // the global lighting dialog
    dlg_lighting* m_pLightingDlg;

    // the Flexible Fitting dialog
    dlg_flexing* m_pFlexFittingDlg;

    // the feature docking dialog
    dlg_featuredocking* m_pFeatureDockingDlg;

    // the Fetch PDB dialog
    dlg_fetchpdb* m_pFetchpdbDlg;

    // the RMSD dialog
    dlg_rmsd* m_pRMSDDlg;

    // the About dialog
    dlg_about* m_pAboutDlg;

    // the Wait dialog
    dlg_wait* m_pWaitDlg;

    // the Preferences dialog
    dlg_preferences* m_pPrefDlg;

    // render displacement between codebook vectors
    QAction* m_pRenderDisplacementArrows;
    sculptor_codebook_displacement* m_pCVDisplacementObj;
    
    // the last refinement settings
    unsigned int m_iRefineMode;

    // powell refinement dialog
    dlg_refine* m_pRefineDlg;

    // target and probe document
    int m_iTargetDoc;
    sculptor_document* m_pTargetDoc;
    int m_iProbeDoc;
    sculptor_document* m_pProbeDoc;

    // flag if probe/solution color should be automatically adjusted
    bool m_bAutoAdjustProbeSolutionColor;

    // solution set
    bool m_bSolutionSetChanged;

    // action for forces turn on or not
    QAction* m_pForces;

    // action for steric clashes turned on or not
    QAction* m_pStericClashing;

    // Flag if warning for moving a solution was shown
    bool m_bMovingSolutionWarningShown;

    // the main workspace into which the properties dialogues are put
    QWidget* m_pWorkspace;

    // the main splitter
    sculptor_splitter* m_pMainSplitter;      // (main splitter)
    sculptor_splitter* m_pLeftSplitter; // (left splitter)
    sculptor_splitter* m_pRightSplitter;

    // QSplitter* m_pMainSplitter;
    // QSplitter* m_pLeftSplitter;
    // QSplitter* m_pRightSplitter;

    // document type filters for the file dialogs
    QString m_oFilterALL;
    QString m_oFilterPDB;
    QString m_oFilterCBK; // codebook
    QString m_oFilterSCL;
    QString m_oFilterSAR;
    QString m_oFilterLUA;
    QString m_oFilterPSF_PDB;
    QString m_oFilterSITUS;
    QString m_oFilterPSF_DCD;
    QString m_oFilterELI;

    // the recently loaded documents
    vector< string > m_aRecentDN;
    vector< string > m_aRecentFileA;
    vector< string > m_aRecentFileB;
    vector< int > m_aRecentIDs;
    QMenu * m_pRecentMenu;
    QAction * m_pRecentAction;

    // the volume menu
    QMenu * m_pVolumeMenu;
    // the context volume menu for the documentlist
    QMenu * m_pVolumeContextMenu;
    //
    QAction * m_pVolAction_Table;
    QAction * m_pVolAction_Mapexp;
    QAction * m_pVolAction_Hist;
    QAction * m_pVolAction_Occvox;
    QAction * m_pVolAction_ChangeVW;
    QAction * m_pVolAction_ChangeOrig;
    QAction * m_pVolAction_SwapAxes;
    QAction * m_pVolAction_Normalize;
    QAction * m_pVolAction_Threshold;
    QAction * m_pVolAction_Scale;
    QAction * m_pVolAction_Crop;
    QAction * m_pVolAction_Floodfill;
    QAction * m_pVolAction_MultiPointFloodfill;
    QAction * m_pVolAction_Gaussian;
    QAction * m_pVolAction_Laplacian;
    QAction * m_pVolAction_LocalNormalization;
    QAction * m_pVolAction_BilateralFilter;
    QAction * m_pVolAction_DPSV_filter;
    QAction * m_pVolAction_Corr;

    // the structure menu
    QMenu * m_pStructureMenu;
    // the context structure menu for the documentlist
    QMenu * m_pStructureContextMenu;
    //
    QAction * m_pPDBAction_RMSD;
    QAction * m_pPDBAction_Table;
    QAction * m_pPDBAction_Sphere;
    QAction * m_pPDBAction_Blur;
    QAction * m_pPDBAction_Extract;
    QAction * m_pPDBAction_Merge;
    QAction * m_pPDBAction_Symmetry;
    QAction * m_pPDBAction_PDF;
    QAction * m_pPDBAction_DiffPDF;

    // timer for timestep actualisation
    QTimer* m_pDynTimer;
    // remember current speed when playback is stopped
    float m_fDynPreviousSpeed;
    // for dynamics, remember when the last timer timeout occured
    long m_iDynLastTimeout;

    //do write out a movie
    bool m_bMovie;
    QString m_oMovieDir;

    // Steric clashing data volume
    svt_volume<Real64>* m_pStericClashingData;
    bool                m_bStericClashingDataValid;

     //genetic algorithm 
    sculptor_gamultifit<svt_gamultifit_ind>* m_pGA;
     
    // timer for timestep actualisation
    QTimer* m_pUpdateGATimer;

    //Dialog of the GA
    dlg_gamultifit* m_pGADlg;

    //genetic algorithm 
    sculptor_gacylinder<svt_gacylinder_ind>* m_pGACyl;

     //Dialog of the GA
    dlg_gacylinder* m_pGACylDlg;
     
    // timer for timestep actualisation
    QTimer* m_pUpdateGACylTimer;
      
    //the fitness of the best individuals  plotted as a function of generation
    dlg_plot* m_pGAFitnessCurveDlg;
     
    // action for the showing the Fitness curve
    QAction* m_pShowFitnessCurve;
     
    //
    // Lua scripting support
    //
    dlg_luaeditor* m_pLuaEditor;         // the lua editor dialog
    dlg_lualist* m_pLuaList;             // the script-list dialog
    sculptor_lua m_oLua;                 // the lua interpreter
    vector< string > m_aScriptTitles;    // array with all the titles of the lua scripts
    vector< string > m_aScripts;         // array with all the scripts
    unsigned int m_iCurrentScript;       // which is the current script that is shown in the editor
    QTabWidget* m_pTabArea;

    sculptor_log* m_pLog;

    // the current position: is a point in space
    svt_vector4<Real64> m_oCurrentPosition;

    // were these two areas collapsed before switching to full screen?
    bool m_bTextAreaCollapsed;
    bool m_bDocumentListCollapsed;
    // was the window maximized?
    bool m_bWindowWasMaximized;
    // the geometry before going to fullscreen
    QRect m_oWindowGeometry;

    // should the window state be restored on start? (will be saved in .sculptorrc)
    bool m_bRestoreWindowStateOnStart;
    // save the default window state and size as determined by Qt
    QByteArray m_oDefaultWindowState;
    QSize m_oDefaultWindowSize;

public:

    /**
     * Constructor
     * \param pScene pointer to sculptor_scene
     */
    sculptor_window(sculptor_scene* pScene);

    /**
     * Destructor
     */
    virtual ~sculptor_window();

    /**
     * Set up the window's menus. 
     */
    void createMenus();

    /**
     * get sculptor scene
     * \return pointer to sculptor_scene
     */
    inline sculptor_scene* getScene()
    {
	return m_pScene;
    };

    /**
     * set sculptor scene
     * \return pointer to sculptor_scene
     */
    inline void setScene(sculptor_scene* pScene)
    {
	m_pScene = pScene;
    };

    /**
     * Load a string from .sculptorrc and tries to restore the window state (think dockwidgets)
     */
    void restoreWindowState();

    /**
     * Save a config string to .sculptorrc for the QMainWindow state
     * (dockwidgets/toolbars/window geometry)
     */
    void saveWindowState();

    /**
     * set if the window state should be restored from last session when Sculptor starts
     */
    void setRestoreWindowStateOnStart(bool bRestoreOnStart);

    /**
     * get if the window state should be restored from last session when Sculptor starts
     */
    bool getRestoreWindowStateOnStart();

    /**
     * restore the default window state now. this state is determined when Sculptor starts and held
     * in memory. It is not saved to disk since this can change depending on screen size and
     * settings
     */
    void restoreDefaultWindowState();

    /**
     * add the default document - has to be done after the svt scene was created!
     */
    void addDefaultDoc();

    /**
     * load the information about the recently loaded documents
     */
    void loadRecentList();
    /**
     * save the information about the recently loaded documents
     */
    void saveRecentList();
    /**
     * update the menubar regarding the recently loaded documents
     */
    void updateRecentList();

    /**
     * load a configuration value (int) from the Sculptor configuration file
     * \param pAttribute name of the configuration value that is to be loaded. will be read from the
     * GENERAL_SETTINGS XML node
     * \param iDefault a default value which will be returned if the value was not found in the sculptorrc
     * file. This is a convenience thing to avoid checking for the existence of the value
     */
    int loadConfigValue(const char * pAttribute, int iDefault, const char* pElement = "GENERAL_SETTINGS");

    /**
     * save a configuration value (int) to the Sculptor configuration file
     * \param pAttribute name of the XML attribute that is to be saved. will be saved into the
     * GENERAL_SETTINGS XML node
     * \param iValue the int to be saved
     */
    void saveConfigValue(const char * pAttribute, int iValue, const char* pElement = "GENERAL_SETTINGS");

    /**
     * load a configuration value (float) from the Sculptor configuration file
     * \param pAttribute name of the configuration value that is to be loaded. will be read from the
     * GENERAL_SETTINGS XML node
     * \param fDefault a default value which will be returned if the value was not found in the sculptorrc
     * file. This is a convenience thing to avoid checking for the existence of the value
     */
    Real32 loadConfigValue(const char * pAttribute, Real32 fDefault, const char* pElement = "GENERAL_SETTINGS");

    /**
     * save a configuration value (float) to the Sculptor configuration file
     * \param pAttribute name of the XML attribute that is to be saved. will be saved into the
     * GENERAL_SETTINGS XML node
     * \param fValue the float to be saved
     */
    void saveConfigValue(const char * pAttribute, Real32 fValue, const char* pElement = "GENERAL_SETTINGS");

    /**
     * load a configuration value (char array) from the Sculptor configuration file
     * \param pAttribute name of the configuration value that is to be loaded. will be read from the
     * GENERAL_SETTINGS XML node
     * \param oDefault a default value which will be returned if the value was not found in the sculptorrc
     * file. This is a convenience thing to avoid checking for the existence of the value
     */
    string loadConfigValue(const char * pAttribute, string oDefault, const char* pElement = "GENERAL_SETTINGS");

    /**
     * save a configuration value (char array) to the Sculptor configuration file
     * \param pAttribute name of the XML attribute that is to be saved. will be saved into the
     * GENERAL_SETTINGS XML node
     * \param pValue the char array to be saved
     */
    void saveConfigValue(const char * pAttribute, string pValue, const char* pElement = "GENERAL_SETTINGS");

    /**
     * enable/disable the use of shaders
     */
    void setAllowShaderPrograms(bool bEnabled);

    /**
     * enable/disable ambient occlusion
     */
    void setAllowAmbientOcclusion(bool bEnabled);

public slots:

    /**
     * load a recently loaded document
     */
    void sDocLoadRecent(QAction* pAction);

    /**
     * Create and set a new style
     * \param style string with the style name, e.g. Sculptor or Windows
     */
    void makeStyle(const QString &style);

    /**
     * toggle the area for the document property dialogs if no parameter given
     * \param iShow 1 show the area, 0 hide it
     */
    void togglePropDlgArea();

    /**
     * toggle full screen
     */
    void sToggleFullScreen();

public:

    /**
     * set if document property dialog area (in)visible. this function is called from the
     * documentlist to request an uncollapse (and thus it must not notify the documentlist)
     */
    void setPropDlgAreaVisible(bool bShow);

    /**
     * get if the document property dialog area is visible
     */
    bool getPropDlgAreaVisible();

    /**
     * add a new document (does not change the moved doc, the current doc, and does not show the
     * property dialog)
     * \param pSD pointer to the new sculptor_document
     * \param cFilename QString object with the filename
     * \return index of the new document in the list
     */
    int addDocSimple(sculptor_document* pSD, QString oDisplayname = QString(""));

    /**
     * add a new document. sets the scene as moved document and, makes the document current and shows
     * its property dialog
     * \param pSD pointer to the new sculptor_document
     * \param cFilename QString object with the filename
     * \return index of the new document in the list
     */
    int addDoc(sculptor_document* pSD, QString oDisplayName = QString(""));

    /**
     * query for a specific PDB, calls fetchPDB in svt_point_cloud_pdb to perform fetch
     * \param oPDB QString object with the PDB + extension
     */
    void queryPDB(QString oPDB);

    /**
     * set the moved document.
     * \param iDoc number of the sculptor_document which should be moved around. if iDoc == -1, no document should be moved around.
     */
    void setMovedDoc(int iDoc);

    /**
     * set document visible
     * \param iDoc int number of the sculptor_document which should be set visible
     * \param bVisible boolean indicating visibility status, true by default
     */
    void setViewDoc(int iDoc, bool bVisible=true);

protected:

    /**
     * some custom key press events
     */
    virtual void keyPressEvent(QKeyEvent* pEvent);

    /**
     * loads a set of Eliquos output files.
     * \param oFileA filename of Eliquos output description file
     */
    void loadEliquosDocs(QString oFileA);

public slots:

    // document management

    /**
     * load a document
     * \param oFileA first file to load
     * \param oFileB second file to load
     */
    int loadDoc(QString oFileA, QString oFileB, QString oDisplayName = QString(""));

    /**
     * showFetchpdbDlg displays a dialog box for the user to input a PDBID and choose whether a biological unit should be downloaded.  
     */
    void showFetchpdbDlg();

    /**
     * load a document
     */
    void load();

    /**
     * Save the current state
     * \param oFilename string with the filename
     * \param bWithoutPath without path, filenames are only local (necessary for the sar archives). Default: false
     */
    void saveSCL( string oFilename, bool bWithoutPath=false );

    /**
     * save a document
     */
    void save();

    /**
     * load state file
     */
    void loadSCL();
    /**
     * save state file
     */
    void saveSCL();
    /**
     * Archive the current state
     */
    void saveArchive();
    /**
     * Load an archive file and restore the state
     */
    void loadArchive(QString oFilename);

    /**
     * delete the selected document(s)
     */
    void sDocsRemove();
    /**
     * delete a document
     * \return true if successful delete; false if nothing was deleted
     */
    bool delDoc(int iDoc);
    /**
     * move document up in the list of documents
     */
    void sDocMoveUp();
    /**
     * move document down in the list of documents
     */
    void sDocMoveDown();
    /**
     * move document to the top of the list of documents
     */
    void sDocMakeTop();

    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderMode( int );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeOFF( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModePOINT( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeLINE( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeVDW( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeCPK( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeDOTTED( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeLICORICE( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeTUBE( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeCARTOON( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeTOON( );

    /**
     * Select a global coloring mode (for pdb documents only)
     * \param iMode the mode
     */
    void changeColmapMode( int iMode );
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeSOLID();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeRAINBOW();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeTYPE();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeRESNAME();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeCHAINID();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeSEGID();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeSTRUCTURE();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeMODEL();

public:

    /**
     * get documentlist
     */
    window_documentlist* getDocumentList();

    /**
     * Sets default probe and target document
     * Usually called after a load() or dropLoad() call to ensure probe 
     * and target are set. Will not have any effect if probe or target
     * are already set.
     */
    void setDefaultProbeTargetDoc();
    /**
     * Checks if probe and target docs are set
     * If only one PDB and one map are loaded, it automatically sets them
     * as probe and target. Otherwise, it asks the user to set them.
     * \return boolean TRUE if everything is ok, FALSE if user action is required
     */
    bool checkProbeTargetDoc();
    /**
     * which document is the target doc?
     * \return pointer to sculptor_document (NULL if no target doc was selected)
     */
    sculptor_document* getTargetDoc();
    /**
     * which document is the probe doc?
     * \return pointer to sculptor_document (NULL if no probe doc was selected)
     */
    sculptor_document* getProbeDoc();

    /**
     * Set solution set changed
     */
    void setSolutionSetChanged(bool bSolutionSetChanged);
    /**
     * Is solution set changed
     */
    bool getSolutionSetChanged();
    /**
     * make a new solution from the current position of the probe map
     */
    void makeNewSolution();

    /**
     * adjust origin
     */
    void adjustOrigin();
    

public slots:

    /**
     * Merge selected PDB documents
     */
    void mergePdbDocs();

    /**
     * save a screenshot
     */
    void saveScreenshot(QString pFilename =QString());

    /**
     * Get the current directory
     * \return string object with the current directory information
     */
    string getCurrentDir();
    /**
     * Set the current directory
     * \param string with current directory information
     */
    void setCurrentDir(string oStr);

    /**
     * add a clipping plane
     */
    int addClip();
  
    /**
     * add eraser tool
     */
    int addEraser();

    /**
     * add a marker 
     */
    int addMarker();

    /**
     * toggle the display state of a document
     */
    void toggleViewDoc();

    /**
     * toggle the move state of a document
     */
    void toggleMovedDoc();

    /**
     * restore the initial transformation matrix of the document
     */
    void restoreInitMat();

    // view settings

    /**
     * this is a timer function called every second to update the current FPS on the statusbar.
     */
    void showFPS();
    /**
     * render with povray
     */
    void render();
    /**
     * export as alias/wavefront
     */
    void exportOBJ();
    /**
     * zoom in.
     */
    void zoomIn();
    /**
     * zoom out.
     */
    void zoomOut();
    
    /**
     * set center mode
     */
    void setCenterMode();
    /**
     * set background color
     */
    void setBackgroundColor();

    /**
     * toggle orthographic projection mode
     */
    void toggleOrtho();

    /**
     * toggle pipette visibility
     */
    void togglePipette();

    /**
     * toggle 3D coordinate system visibility
     */
    void toggle3DCoordSystem();

    /**
     * high contrast or low contrast palette
     */
    void toggleHighContrast();
    
    /**
     * Set global OpenGL lighting options
     */
    void setGlobalLighting();

    /**
     * return a pointer to the lighting dialog
     */
    dlg_lighting* getLightingDlg();

    /**
     * close the surfaces at clip-planes
     */
    void cappedClip();

    /**
     * set the eye separation for the stereo rendering
     */
    void setEyeSeparation();

    /**
     * Generate image series for 3DNP
     */
    void gen3DNP();

    // program management

    /**
     * a close event has occured (window close button was pressed)
     */
    void closeEvent( QCloseEvent* );
    /**
     * quit sculptor
     */
    void quitNow();
    /**
     * show about dialog
     */
    void showAboutDlg();
    /**
     * show the online help
     */
    void help();

    // transform

    /**
     * show the transform dialog
     */
    void showTransformDlg();
    /**
     * reset the dialog
     */
    void resetTransformDlg();
    /**
     * the user closed the transform dialog
     */
    void closeTransformDlg();
    /**
     * the user has changed the x translation dial.
     */
    void dialTransXValueChanged(int iVal);
    /**
     * the user has changed the y translation dial.
     */
    void dialTransYValueChanged(int iVal);
    /**
     * the user has changed the z translation dial.
     */
    void dialTransZValueChanged(int iVal);
    /**
     * the user has changed the x rotation dial.
     */
    void dialRotXValueChanged(int iVal);
    /**
     * the user has changed the y rotation dial.
     */
    void dialRotYValueChanged(int iVal);
    /**
     * the user has changed the z rotation dial.
     */
    void dialRotZValueChanged(int iVal);

    /**
     * copy the transformation of the current selected document to all other selected docs
     */
    void copyTransform();

   
    // force

    /**
     * a new document should be used as target document for the fitting
     * \param iDoc the index of the document
     */
    void setTargetDoc(int iDoc);
    /**
     * the currently selected document should be used as target document for the fitting
     */
    void setTargetDoc();

    /**
     * a new document should be used for the fitting process (the document one will move around)
     * \param iDoc the index of the document
     */
    void setDockDoc(int iDoc);
    /**
     * the currently selected document should be used for the fitting process (the document one will move around)
     */
    void setDockDoc();
    /**
     * a new document should be used as a solution candidate
     * \param iDoc the index of the document
     */
    void setSolutionDoc(int iDoc);
    /**
     * the currently selected document should set as a solution document
     */
    void setSolutionDoc();
    /**
     * automatically color probe and target structures
     */
    void autoColorProbeSolution();
    /**
     * update all force icons
     */
    void updateForceIcons();

    /**
     * Get force algo
     */
    sculptor_force_algo* getForceAlgo();
    /**
     * toggle forces
     */
    void toggleForces();
    /**
     * is force feedback turned on?
     */
    bool isForceOn();

    /**
     * Enable move or force tool
     */
    void enableInput();
    /**
     * Disable move or force tool
     */
    void disableInput();

    /**
     * toggle steric clashes
     */
    void toggleStericClashing();
    /**
     * is steric clashing turned on?
     */
    bool getStericClashing();
    /**
     * Set steric clashes active/deactive
     */
    void setStericClashing(bool bStericClashing);
    /**
     * Check for or generate codebook to use for steric clashing
     */
    bool isStericClashingCodebookValid(sculptor_document* pDoc);

    /**
     * update the steric clashing data
     */
    void updateStericClashingData();

    /**
     * get steric clashing data
     */
    svt_volume<Real64>* getStericClashingData();

    /**
     * feature-based docking docking
     */
    void featureDock();  
    
    // Simultaneous multibody optimization by GA
     
    /**
     * show the dialog of the Simultaneous MultiBody Optimization by Genetic algorithm
     */ 
    void showGaDlg();
        
    /**
     * Launch the genetic algorithm in new thread 
     * \param bUseDefaultParams if false use the m_pGaDlg to get the params
     */
    void startGa(bool bUseDefaultParams = true);
    
    /**
     * Stop genetic algorithm in new thread 
     */
    void stopGa();
     
    /**
     * update GA results in window
     */
    void updateGaResults();
   
    /**
     * add data to fitness plot
     * Attention: does not display the plot; the function showFitnessCurveDlg() displays it or not depending on arguments
     */
    void updateFitnessCurveDlg( vector< Real64 > &oFitness, unsigned int iGen );
 
    /**
    * sets the visibility of the Fitness Curve Dialog
    */
    void showFitnessCurveDlg();
 
    /**
     * show the dialog of the tube extraction by Genetic algorithm
     */ 
    void showGaCylDlg();
     
    /**
     * start the tube extraction
     */
    void startGaCyl();
  
    /**
     * update the results
     */ 
    void updateGaCylResults();

    /**
     * stops the tube extraction
     */
    void stopGaCyl();

    /**
     * get a pointer to the ga timer
     */
    QTimer * getGaCylTimer(); 

    // Flexible Fitting by Interpolation methods
    /**
     * show the properties dialog for the force feedback
     */
    void showForceDlg();
    /**
     * toggle the properties dialog for the force feedback
     */
    void toggleForceDlg();

    /**
     * show the motor temperatures dialog for the force feedback
     */
    void showTempDlg();

    /**
     * show the refinement dialog
     */
    void showRefinementDlg();

    /**
     * set force tool center mode
     * \param bCenter if true the center mode for the force tool is enabled
     */
    void setForceCenterMode(bool bCenter);

    /**
     * show the Flexing dialog
     */
    void showFlexingDlg();
  
    //PDF
    
    /**
     * Shows PDF function of selected PDBs:
     */
    void showPdfDlg();
    
    /**
     * Shows the difference between PDF function of selected PDBs: first selected PDB is the reference
     * The reference will be substracted from all the other selected PDD
     */
    void showDiffPdfDlg();
    
    // clustering

    /**
     * Show clustering dialog
     */
    void clustering();

    /**
     * Laplace quantization
     */
    void laplaceQuantization();
    
    /**
     * Atomic quantization
     */
    void atomicQuantization();
    
    /**
    * FAM: The first selected document gives the initial coordinates for the 
    * clustering of the second selected document (vol/pdb)
    */
    void clusteringFromCoords();
    
    /**
     * Enable or disable rendering of the codebook
     */
    void renderCV();

    /**
     * Copy feature vectors between two documents
     */
    void copyCV();
    
    /**
     * Enable and disable rendering of the displacement
     * \param is the render displacement called form the flexible fitting - by default NO
     */
    void renderDisplacement(bool bFromFlexing=false);
    
    /**
     * create codebook displacement object
     * \param oDockDoc 
     * \param oTargetDoc 
     */
    void createCVDisplacementObj(sculptor_document &oDockDoc, sculptor_document &oTargetDoc);
    
    /**
     * Update the Codebook displacement representation : position, color etc...
     */
    void updateCVDisplacementObj();
    
    /**
     * Compute RMSD of the corresponding CV
     * \param oFirst a sculptor document with computed CV and eventually a match
     * \param oSecond a sculptor document with computed CV
     * \return the value of the RMSD between the CV of the two documents considering the match
     */
    Real64 computeCvRMSD(sculptor_document &oFirst, sculptor_document &oSecond);

    /**
     * Cleanup the result stack, remove abiguous results.
     */
    void cleanResults( svt_point_cloud< svt_vector4<Real64> >& oCV_Mono, svt_point_cloud< svt_vector4<Real64> >& oCV_Olig, vector< svt_matchResult< Real64 > >& rResults, unsigned int iMaxCount );
    
   /**
     * Creates a new pdb_document to hold the flexed structure
     *\param pFilename a string holding the filename of the original structure: the structure to be flexed
     *\param rPDB contains the coordinates obtained during flexing
     *\param bDynamic is the molecule dynamic?
     *\return int value represents the index of the new created file in the list of documents  
    */
    int createDocForFlexedStruct(const char * pFilename, svt_point_cloud_pdb<svt_vector4<Real64> >* rPDB, bool bDynamic = false);

    /**
     * get the origin of the system
     */
    svt_vector4f getOrigin();

    /**
     * based on the number of selected files, the flexing documents (Dock, target) will be determined :
     * 1) no or one selected file: use the documents indicated by the Dock and TArget iconds
     * 2) 2 files selected: (one pdb and one situs-flex) or (two pdb - morph)
     * \param oFirst - the structure that will be morphed
     * \param oSecond - the target doc - target codebook vectors 
     */
    void getDocsFromSelection(sculptor_document **oFirst, sculptor_document **oSecond);   

    /**
     * calculate the correlation coefficient
     */
    void calcCorrelation();

    /**
     * show the rmsd dialog 
     */
    void showRmsdDlg();

    /**
     * Export codebook
     */
    void exportCodebook();
    /**
     * Import codebook
     */
    void importCodebook();

    /**
     * Get the main area into which the 3D widget is placed
     */
    QWidget* getMainWidgetArea();

    /**
     * set the current Position
     * \param svt_vector4 with the current postion
     */ 
    void setCurrentPosition( svt_vector4<Real64> oPos );

    /**
     * get the current Position
     * \return svt_vector4 with the current postion
     */ 
    svt_vector4<Real64> getCurrentPosition();


    /**
     * \name Methods for Protein Dynamics
     */
    //@{

public slots:

    /**
     * play forward
     */
    void dynForward();
    /**
     * play backward
     */
    void dynBackward();
    /**
     * stop playback
     */
    void dynStop();
    /**
     * jump to start
     */
    void dynToStart();
    /**
     * jump to end
     */
    void dynToEnd();
     /**
     * Export a movie
     */
    void dynMovie();
    /**
     * timer
     */
    void dynTimeout();
    /**
    * set the loop Option of the first document ( with multiple frames )
    */
    void dynSetLoop(int iLoop);

public:

    /**
     * adjust playback-speed. check first if we are currently playing backwards (user clicked on play
     * backwards or structure is currently rocking back). this makes sure to not change the current
     * playback direction
     * \param iTPS timesteps per second
     */
    void dynSetTPS( int iTPS );
    /**
    * get the loop Option of the first document ( with multiple frames )
    */
    int dynGetLoop();
    /**
     * get the min speed to all "dynamic" documents 
     */
    float dynGetSpeed();
    /**
     * set the speed to all "dynamic" documents 
     */
    void dynSetSpeed(float fSpeed);
    /**
     * set the Timestep; if iTimestep larger that the maxTimestep then just set the modulo 
     * \param iTimestep the Timestep 
     */
    void dynSetTimestep( int iTimestep );
    /**
     * get maxTimestep of all documents 
     * \return the largest maxTimestep
     */
    int dynGetMaxTimestep();
    /**
    * get the max of the current Timestep of all documents 
    * \return the largest getTimestep
    */
    int dynGetTimestep();
    //@}

    /**
     * \name Methods for Lua Scripts
     */
    //@{

public slots:

    /**
     * Open the lua editor dialog
     */
    void scriptInitGUI();

    /**
     * Add a simple default script
     */
    void scriptAddDefault();

    /**
     * Run the lua code
     */
    void scriptRun();

    /**
     * Stop the lua interpreter
     */
    void scriptStop();

    /**
     *  User would like to move a script down in the list
     */
    void scriptDown();

    /**
     *  User would like to move a script up in the list
     */
    void scriptUp();

    /**
     * Add a lua script (for example by loading it from the command-line
     * \param oName stl string with the name of the lua script
     * \param oScript stl string with a lua script
     */
    void scriptAdd( string oName, string oScript );

    /**
     * Another script was selected by the user
     */
    void scriptSelected(int iIndex);

    /**
     * User would like to add a new script (pressed on new button)
     */
    void scriptAdded();

    /**
     * User would like to remove the current script
     */
    void scriptRemoved();

    /**
     * User would like to change the name of the current script
     */
    void scriptChangeName();

    /**
     * User has renamed a script in the list, so update arrays and the editor
     */
    void scriptSetName(int iIndex, QString oName);

    /**
     * Save the current scripts
     */
    void scriptSave();

    /**
     * Clear the log window
     */
    void clearLog();

    //@}

    /**
     * Show the preferences dialog
     */
    void sShowPreferencesDialog();

public:

    /**
     * Opens a file-open-dialog.
     * \param oMessage stl string with the title of the file dialog
     * \return returns the file that was selected by the user or an empty string in case the user cancelled the operation
     */
    string guiFileOpenDlg(string oMessage);
    /**
     * Opens a file-save-dialog.
     * \param oMessage stl string with the title of the file dialog
     * \return returns the file that was selected by the user or an empty string in case the user cancelled the operation
     */
    string guiFileSaveDlg(string oMessage);

    /**
     * Opens a warning message box dialog.
     * \param oCaption   stl string with the caption of the dialog
     * \param oMessage   stl string with the actual message
     * \param oStatusbar stl string with the statusbar message
     */
    void guiWarning(string oCaption, string oMessage, string oStatusbar);

    /**
     * Opens an info message box dialog.
     * \param oCaption   stl string with the caption of the dialog
     * \param oMessage   stl string with the actual message
     * \param oStatusbar stl string with the statusbar message
     */
    void guiInfo(string oCaption, string oMessage, string oStatusbar);

    /**
     * Opens an info message box dialog that asks the user a yes/no question.
     * \param oCaption   stl string with the caption of the dialog
     * \param oMessage   stl string with the actual message
     * \return boolean (true if the user answered yes, false if no)
     */
    bool guiYesNo(string oCaption, string oMessage);

    /**
     * Opens an info message box dialog that asks the user to input a string.
     * \param oCaption   stl string with the caption of the dialog
     * \param oMessage   stl string with the actual message
     * \param oDefault   stl string with the default answer, given if the user cancels the dialog
     * \return string with the answer, empty if the user pressed cancel.
     */
    string guiQuestion(string oCaption, string oMessage, string oDefault);

    /**
     * Opens an info message box dialog that asks the user to input a value.
     * \param oCaption   stl string with the caption of the dialog
     * \param oMessage   stl string with the actual message
     * \param fDefault   default value, if the user cancels the dialog
     * \return double with the answer, 0 if the user pressed cancel.
     */
    Real64 guiGetValue(string oCaption, string oMessage, Real64 fDefault = 0.0);

    /**
     * popup the volume context menu
     * \param QPoint* the location where to popup
     */
    void popupVolumeMenu(QPoint oPoint);

    /**
     * popup the structure context menu
     * \param QPoint* the location where to popup
     */
    void popupStructureMenu(QPoint oPoint);

    /*
     * actions in the menus (and in toolbars) get checked/unchecked depending on state of current
     * document
     */
    void updateMenuItems();

    /**
     * enable/disable menus and their menu actions depending on the number and types selected docs
     * \param bSceneSelected bool to indicate if the scene is selected
     * \param iPDBsSelected int that counts the selected pdb documents
     * \param iVolsSelected int that counts the selected volume documents
     */
    void updateMenus(bool bSceneSelected, int iPDBsSelected, int iVolsSelected);

    /**
     * get if the Laplace docking menu action is checked
     */
    bool getDockToLaplace();
};

#endif
