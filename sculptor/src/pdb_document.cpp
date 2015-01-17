/***************************************************************************
                          pdb_document.cpp
                          ----------------
    begin                : 27.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <pdb_document.h>
#include <situs_document.h>
#include <sculptor_window.h>
#include <sculptor_window_documentlist.h>
#include <sculptor_scene.h>
#include <sculptor_prop_pdb.h>
#include <sculptor_dlg_rmsd.h>
#include <sculptor_dlg_colmap.h>
#include <sculptor_dlg_colmapsecstruct.h>
#include <sculptor_dlg_colorselect.h>
#include <sculptor_dlg_extract.h>
#include <sculptor_dlg_cartoon.h>
#include <sculptor_dlg_plot.h>
#include <sculptor_dlg_pdbvdw.h>
#include <sculptor_dlg_atomtable.h>
#include <sculptor_dlg_atomclustering.h>
#include <sculptor_dlg_newtube.h>
#include <sculptor_dlg_symmetry.h>
#include <sculptor_dlg_toon.h>
// svt_includes
#include <svt_core.h>
#include <svt_pdb.h>
#include <svt_const.h>
#include <svt_array.h>
#include <svt_pdb.h>
#include <svt_atom.h>
#include <svt_bond.h>
#include <svt_init.h>
#include <svt_types.h>
#include <svt_time.h>
#include <svt_string.h>
// qt4 includes
#include <QProgressDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QTableView>
#include <QString>
// stdlib FIXME
#include <time.h>
#include <ctype.h>

#ifdef WIN32
#define srand48 srand
#define drand48() (((float)(rand()))/(float)(RAND_MAX))
#endif

svt_palette* pdb_document::s_pHighContrastPalette = NULL;
svt_palette* pdb_document::s_pLowContrastPalette = NULL;

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);
extern QString StdtoQ(string);

/**
 * Constructor
 * \param pGraphNode the node in the svt_scenegraph, pdb_document will add its svt_nodes to
 * \param oFile pointer to an char array with the filename of the first file
 * \param iType file-type of the first file (e.g. SVT_NONE, SVT_PDB, SVT_PSF, SVT_DCD, ...)
 * \param oFile_SRCB pointer to an char array with the filename of the second file
 * \param iType_SRCB file-type of the second file (e.g. SVT_PDB, SVT_PSF, SVT_DCD, ...)
 */
pdb_document::pdb_document(svt_node* pGraphNode,
                           string oFile,      int iType,
                           string oFile_SRCB, int iType_SRCB,
                           svt_point_cloud_pdb< svt_vector4<Real64 > >* rPDB)
    : sculptor_document( pGraphNode, oFile, oFile_SRCB ),
      m_pPdbPropDlg( NULL ),
      m_pAtomTable( NULL ),
      m_pColmapDlg( NULL ),
      m_pColmapDlgSecStruct( NULL ),
      m_pExtractDlg( NULL ),
      m_pColorSelectDlg( NULL ),
      m_pSymmetryDlg( NULL ),
      m_pCartoonDlg( NULL ),
      m_pNewTubeDlg( NULL ),
      m_pVDWDlg( NULL ),
      //m_pToonDlg( NULL ),
      m_pAtomClusteringDlg( NULL ),
      m_bAutoApply( true )
{

    if (rPDB == NULL)
    {
        // create new pdb node and load file
        m_pPdbNode = new sculptor_svt_pdb( (char*)oFile.c_str(), iType, (char*)oFile_SRCB.c_str(), iType_SRCB );
    }
    else
    {
        m_pPdbNode = new sculptor_svt_pdb(*rPDB);
    }

    // store the center of the structure
    m_oCenter = m_pPdbNode->coa();

    svtout  << "Center of atoms: " << m_oCenter << endl;

    // install default colormap
    m_pPdbNode->setAtomColmap(&m_cTypeColmap);

    // center the structure
    svt_matrix4 <Real64> oTranslation;
    oTranslation.loadIdentity();
    oTranslation.setTranslation(-m_oCenter);

    int iCurrentTimestep = m_pPdbNode->getTimestep();
    for(int iTS=0; iTS<m_pPdbNode->getMaxTimestep(); iTS++)
    {
        m_pPdbNode->setTimestep( iTS );

        for( unsigned int i=0; i<m_pPdbNode->size(); i++)
        {
            (*m_pPdbNode)[i] = oTranslation * (*m_pPdbNode)[i];
        }
    }
    m_pPdbNode->setTimestep( iCurrentTimestep );
    m_pPdbNode->setName("(pdb_document)m_pPdbNode");

    // install new transformation
    m_pPdbNode->setPos( new svt_pos((1.0E-2) * m_oCenter.x(),
                                    (1.0E-2) * m_oCenter.y(),
                                    (1.0E-2) * m_oCenter.z()) );

    // save transformation
    storeInitMat();

    // initialize the display modes
    m_pPdbNode->setDisplayMode( SVT_PDB_LINE );
    m_aModes.addElement( pdb_mode(MODE_ALL, 0, GM_LINE, COLMAP_TYPE) );

    // install in scenegraph
    m_pGraphNode->addAtHead(m_pPdbNode);

    // do some init stuff (all constructors do this)
    init();
}

/**
 * 'Copy' Constructor
 * \param pGraphNode the node in the svt_scenegraph to which pdb_document will add its svt_nodes to
 * \param rOrig reference to the PDB document to be copied
 */
pdb_document::pdb_document(svt_node* pGraphNode, pdb_document& rOrig)
    : sculptor_document( pGraphNode, rOrig.getFileName(), rOrig.getFileName_SRCB() ),
      m_pPdbPropDlg( NULL ),
      m_pAtomTable( NULL ),
      m_pColmapDlg( NULL ),
      m_pColmapDlgSecStruct( NULL ),
      m_pExtractDlg( NULL ),
      m_pColorSelectDlg( NULL ),      
      m_pSymmetryDlg( NULL ),
      m_pCartoonDlg( NULL ),
      m_pNewTubeDlg( NULL ),
      m_pVDWDlg( NULL ),
      //m_pToonDlg( NULL ),
      m_pAtomClusteringDlg( NULL ),
      m_bAutoApply( true )
{
    m_pPdbNode = new sculptor_svt_pdb((svt_pdb&) (*rOrig.getNode()));

    // copy the center of the structure
    m_oCenter = rOrig.getCenter();

    // copy transformation
    svt_matrix4f* pTransformation = new svt_matrix4f;
    *pTransformation = *(rOrig.getNode()->getTransformation());
    m_pPdbNode->setTransformation(pTransformation);

    m_aModes = rOrig.getModes();

    // install in scenegraph
    m_pGraphNode->addAtHead(m_pPdbNode);

    // do some init stuff (all constructors do this)
    init();
}

/**
 *
 */
pdb_document::~pdb_document()
{
    // as the force arrow was added to the main scenegraph and not to the pdb_document node, we have to delete it manually
    m_pGraphNode->del(m_pForceArrow);

    if (m_pNewTubeDlg != NULL)
        delete (m_pNewTubeDlg);
    if (m_pCartoonDlg != NULL)
        delete (m_pCartoonDlg);
//     if (m_pToonDlg != NULL)
//      delete (m_pToonDlg);
    if (m_pAtomTable != NULL)
	delete (m_pAtomTable);
    if (m_pColmapDlg != NULL)
	delete (m_pColmapDlg);
    if (m_pColmapDlgSecStruct != NULL)
        delete (m_pColmapDlgSecStruct);
    if (m_pVDWDlg != NULL)
        delete (m_pVDWDlg);
    if (m_pExtractDlg != NULL)
        delete (m_pExtractDlg);
    if (m_pColorSelectDlg != NULL)
        delete (m_pColorSelectDlg);
    if (m_pAtomClusteringDlg != NULL)
        delete (m_pAtomClusteringDlg);
}

/**
 *
 */
void pdb_document::init()
{
    // dynamic or static?
    if ( m_pPdbNode->getMaxTimestep() > 1)
        m_bDynamic = true;
    else
        m_bDynamic = false;

    // initialize the two palettes
    if (s_pLowContrastPalette == NULL)
    {
        s_pLowContrastPalette = new svt_palette;
        s_pLowContrastPalette->addColor( svt_color( 197.0f/255.0f, 250.0f/255.0f, 246.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 172.0f/255.0f, 224.0f/255.0f, 241.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color(  65.0f/255.0f, 190.0f/255.0f, 232.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color(   0.0f/255.0f, 114.0f/255.0f, 188.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color(   0.0f/255.0f, 127.0f/255.0f, 141.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color(   0.0f/255.0f, 169.0f/255.0f, 157.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color(   2.0f/255.0f, 137.0f/255.0f,  64.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color(  97.0f/255.0f, 205.0f/255.0f,  88.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 160.0f/255.0f, 255.0f/255.0f, 134.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 255.0f/255.0f, 245.0f/255.0f, 109.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 252.0f/255.0f, 175.0f/255.0f,  23.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 237.0f/255.0f,  27.0f/255.0f,  47.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 183.0f/255.0f,  39.0f/255.0f, 191.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 195.0f/255.0f, 192.0f/255.0f, 218.0f/255.0f) );
        s_pLowContrastPalette->addColor( svt_color( 242.0f/255.0f, 244.0f/255.0f, 245.0f/255.0f) );
    }
    if (s_pHighContrastPalette == NULL)
    {
        s_pHighContrastPalette = new svt_palette;
        s_pHighContrastPalette->addColor( svt_color(  65.0f/255.0f, 190.0f/255.0f, 232.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 237.0f/255.0f,  27.0f/255.0f,  47.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(  97.0f/255.0f, 205.0f/255.0f,  88.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 252.0f/255.0f, 175.0f/255.0f,  23.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   0.0f/255.0f, 127.0f/255.0f, 141.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 183.0f/255.0f,  39.0f/255.0f, 191.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 255.0f/255.0f, 245.0f/255.0f, 109.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 172.0f/255.0f, 224.0f/255.0f, 241.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 160.0f/255.0f, 255.0f/255.0f, 134.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   0.0f/255.0f, 169.0f/255.0f, 157.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 242.0f/255.0f, 244.0f/255.0f, 245.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   0.0f/255.0f, 114.0f/255.0f, 188.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   2.0f/255.0f, 137.0f/255.0f,  64.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 195.0f/255.0f, 192.0f/255.0f, 218.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 197.0f/255.0f, 250.0f/255.0f, 246.0f/255.0f) );
    }

    m_cResnameColmap.  setPalette( s_pHighContrastPalette );
    m_cChainIDColmap.  setPalette( s_pHighContrastPalette );
    m_cSegIDColmap.    setPalette( s_pHighContrastPalette );
    m_cSolidColmap.    setPalette( s_pHighContrastPalette );
    m_cStructureColmap.setPalette( s_pHighContrastPalette );
    m_cModelColmap.    setPalette( s_pHighContrastPalette );
    m_cBetaColmap.     setPalette( s_pHighContrastPalette );
    m_cBetaColmap.     setMaxBeta( m_pPdbNode->getMaxTempFact() );
    m_cBetaColmap.     setMinBeta( m_pPdbNode->getMinTempFact() );
    m_bHighContrast = true;

    // add the center sphere
    m_pCenterSphere = new svt_sphere();
    m_pCenterSphere->setVisible( false );
    m_pCenterSphere->setProperties( new svt_properties() );
    m_pCenterSphere->getProperties()->setColor( new svt_color() );
    m_pCenterSphere->setName("(pdb_document)m_pCenterSphere");
    m_pPdbNode->add( m_pCenterSphere );

    // add text above central sphere
    m_pCenterText = new svt_textnode();
    m_pCenterText->setVisible( false );
    m_pCenterText->setFGColor( 0.0f, 1.0f, 0.0f );
    m_pCenterText->setCentered( true );
    m_pCenterText->setOffset( 0.2f, 0.2f, 0.4f );
    m_pCenterText->setName("(pdb_document)m_pCenterText");
    m_pCenterSphere->add( m_pCenterText );

    // add a sphere to highlight local maxima
    m_pCenterSphereHighlight = new svt_sphere();
    m_pCenterSphereHighlight->setVisible( false );
    m_pCenterSphereHighlight->setProperties( new svt_properties() );
    m_pCenterSphereHighlight->getProperties()->setWireframe( true );
    m_pCenterSphereHighlight->getProperties()->setSolid( false );
    m_pCenterSphereHighlight->getProperties()->setWireframeColor( new svt_color(1.0f, 1.0f, 1.0f) );
    m_pCenterSphereHighlight->setName("(pdb_document)m_pCenterSphereHighlight");
    m_pPdbNode->add( m_pCenterSphereHighlight );

    // add the force arrow
    m_pForceArrow = new svt_arrow();
    m_pForceArrow->setVisible( false );
    m_pForceArrow->setHeight( 0.3f );
    m_pForceArrow->setProperties( new svt_properties() );
    m_pForceArrow->getProperties()->setColor( new svt_color() );
    m_pForceArrow->setTransformation( new svt_matrix4f );
    m_pForceArrow->setName("(pdb_document)m_pForceArrow");

    // force arrow gets added on the same level as the pdb node, not as a child
    // Necessary to keep it from rotating with the structure.
    m_pGraphNode->add(m_pForceArrow);
}

/**
 * get the svt_node the document is providing
 */
svt_node* pdb_document::getNode()
{
    return m_pPdbNode;
}

///////////////////////////////////////////////////////////////////////////////
// properties dlg
///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
QWidget* pdb_document::createPropDlg(QWidget* parent)
{
    // if the dialog already exists, just return it
    if ( m_pPdbPropDlg != NULL)
    {
        return (QWidget*)m_pPdbPropDlg;
    }

    QString oText;
    m_pPdbPropDlg = new prop_pdb( parent, this );

    // fill in all the current transformations
    for( unsigned int i=0; i<getTransformCount(); i++)
        m_pPdbPropDlg->transformAdd( getTransformName(i) );

    // now rebuild mode list
    rebuildModeList();

    // update the buttons in the prop dialog
    m_pPdbPropDlg->sUpdatePdbModeButtons();
    m_pPdbPropDlg->updateTabDocking();
    m_pPdbPropDlg->updateTabInformation();

    return (QWidget*)m_pPdbPropDlg;
}

/**
 *
 */
QWidget* pdb_document::getPropDlg()
{
    return (QWidget*)m_pPdbPropDlg;
}

/**
 * Remove all existing modes and add a new global mode
 * \param iGraphicsMode new global graphics mode
 */
void pdb_document::setGlobalMode( int iGraphicsMode )
{
    // get previous coloring mode
    int iColmap = m_aModes[0].getColmapType();

    // remove all modes
    m_aModes.delAllElements();
    pdb_mode oNewMode( MODE_ALL, 0, iGraphicsMode, iColmap );
    m_aModes.addElement( oNewMode );

    // update graphics rendering
    while(m_pPdbNode->getNodeLock()->tryLock())
        svt_sleep(10);
    m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
    m_pPdbNode->blockWidget( m_pPdbPropDlg );

    for(int i=0;i<m_aModes.numElements();i++)
        applyPdbMode(m_aModes[i]);

    m_pPdbNode->rebuildDL();
    m_pPdbNode->getNodeLock()->V();
}

/**
 * Change the coloring for the first mode in the stack
 * \param iColmapMode new global coloring mode
 */
void pdb_document::setGlobalColmap( int iColmapMode, unsigned int iColor )
{
    if (iColmapMode == COLMAP_SOLID)
        m_cSolidColmap.changeMapping( 0, iColor );

    m_aModes[0].setColmapType( iColmapMode );

    // update graphics rendering
    m_pPdbNode->getNodeLock()->P();
    m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
    m_pPdbNode->blockWidget( m_pPdbPropDlg );

    for(int i=0;i<m_aModes.numElements();i++)
        applyPdbMode(m_aModes[i]);

    m_pPdbNode->rebuildDL();
    m_pPdbNode->getNodeLock()->V();

    if (m_pDLItem != NULL)
        m_pDLItem->showColor( iColmapMode == COLMAP_SOLID );
}

/**
 * Get the global color mapping mode
 */
int pdb_document::getGlobalColmapMode()
{
    return m_aModes[0].getColmapType();
}

/**
 * Get the global color mapping color
 */
int pdb_document::getGlobalColmapColor()
{
    return m_cSolidColmap.getMapping(0);
}

/**
 * Show color chooser dialog
 */
int pdb_document::showColorChooser()
{
    // ask for color
    svt_palette* pPalette  = m_cSolidColmap.getPalette();
    unsigned int iSelected = m_cSolidColmap.getMapping( NULL );
    
    if ( m_pColorSelectDlg == NULL )
        m_pColorSelectDlg = new dlg_colorselect( g_pWindow, pPalette );
    
    m_pColorSelectDlg->setCurrent( iSelected );
   
    if (m_pColorSelectDlg->exec() == QDialog::Accepted)
        return m_pColorSelectDlg->getCurrent();
    else
        return -1;
};

/**
 * Change the coloring for the first mode in the stack - to solid and ask interactively for the color itself
 */
void pdb_document::setGlobalColmapSOLIDINTERACTIVE()
{
    unsigned int i;
    int iSelected = showColorChooser();

    if (iSelected >= 0)
    {
        // set first colmap to solid
        m_aModes[0].setColmapType( COLMAP_SOLID );

        m_cSolidColmap.changeMapping(NULL, iSelected );

        svt_setStop(true);
        m_pPdbNode->rebuildDL();
        svt_setStop(false);
    }

    // update graphics rendering
    m_pPdbNode->getNodeLock()->P();
    m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
    m_pPdbNode->blockWidget( m_pPdbPropDlg );

    for(i=0;i<(unsigned int)m_aModes.numElements();i++)
        applyPdbMode(m_aModes[i]);

    m_pPdbNode->rebuildDL();
    m_pPdbNode->getNodeLock()->V();

    if ( m_pDLItem != NULL )
        m_pDLItem->showColor(true);
}


/**
 *set m_pDynamic
 */
void pdb_document::setDynamic(bool bDynamic)
{
    m_bDynamic = bDynamic;
}

/**
 *get m_pDynamic
 */
bool pdb_document::getDynamic()
{
    return m_bDynamic;
}

///////////////////////////////////////////////////////////////////////////////
// Slots
///////////////////////////////////////////////////////////////////////////////

void pdb_document::setSphereScale(Real32 fSphereScale)
{
    int i = m_pPdbPropDlg->getCurrentPdbModeIndex();

    m_aModes[i].setSphereScale(fSphereScale);
    applyPdbMode(m_aModes[i]);
}

Real32 pdb_document::getSphereScale()
{
    int i = m_pPdbPropDlg->getCurrentPdbModeIndex();

    return m_aModes[i].getSphereScale();
}


/// the user wants to adjust the graphics mode
void pdb_document::sAdjGraphicsMode()
{
    bool ok = false;
    double dValue;
    QString oStr;


    int i = m_pPdbPropDlg->getCurrentPdbModeIndex();

    switch( m_aModes[i].getGraphicsMode() )
    {
        case GM_VDW:
        {
            if (m_pVDWDlg == NULL)
                m_pVDWDlg = new dlg_pdbvdw(g_pWindow, m_pPdbNode, this);

            oStr = QString( "VDW Settings (%1)" ).arg( getNameShort().c_str() );
            m_pVDWDlg->setWindowTitle(oStr);

            m_pVDWDlg->raise();
            m_pVDWDlg->show();
            break;
        }

        case GM_CPK:

        case GM_DOTTED:
        {
            dValue = QInputDialog::getDouble( g_pWindow, QString("Make an input"), QString("Sphere Scaling"), m_aModes[i].getSphereScale(), (double)0.0f, (double)10000.0f, 2, &ok);
            if ( ok  )
            {
                m_aModes[i].setSphereScale(dValue);
                applyPdbMode(m_aModes[i]);
            }
            break;
        }

        case GM_TUBE:
        {
            dValue = QInputDialog::getDouble( g_pWindow, QString("Make an input"), QString("Tube Diameter Scaling"), m_aModes[i].getTubeDiameter(), (double)0.0f, (double)10000.0f, 2, &ok);
            if ( ok  )
            {
                m_aModes[i].setTubeDiameter(dValue);
                applyPdbMode(m_aModes[i]);
            }
            break;
        }

        case GM_NEWTUBE:
        {
            if (m_pNewTubeDlg == NULL)
            {
                m_pNewTubeDlg = new dlg_newtube(g_pWindow, m_pPdbNode);

                oStr = QString( "Tube Settings (%1)" ).arg( getNameShort().c_str() );
                m_pNewTubeDlg->setWindowTitle(oStr);

                if (m_pCartoonDlg != NULL)
                {
                    m_pCartoonDlg->setNewTubePropDlg(m_pNewTubeDlg);
                    m_pNewTubeDlg->setCartoonPropDlg(m_pCartoonDlg);
                }
            }

            m_pNewTubeDlg->raise();
            m_pNewTubeDlg->show();
            break;
        }

        case GM_CARTOON:
        {
            if (m_pCartoonDlg == NULL)
            {
                m_pCartoonDlg = new dlg_cartoon(g_pWindow, this);

                oStr = QString( "Cartoon Settings (%1)" ).arg( getNameShort().c_str() );
                m_pCartoonDlg->setWindowTitle(oStr);

                if (m_pColmapDlgSecStruct != NULL)
                {
                    m_pColmapDlgSecStruct->setCartoonPropDlg(m_pCartoonDlg);
                    m_pCartoonDlg->setColorDlgSecStruct(m_pColmapDlgSecStruct);
                }
                if (m_pNewTubeDlg != NULL)
                {
                    m_pCartoonDlg->setNewTubePropDlg(m_pNewTubeDlg);
                    m_pNewTubeDlg->setCartoonPropDlg(m_pCartoonDlg);
                }
            }

            m_pCartoonDlg->raise();
            m_pCartoonDlg->show();
            break;
        }

        case GM_SURF:
        {
            m_aModes[i].setProbeRadius( g_pWindow->guiGetValue("Probe Radius", "Probe Radius", m_aModes[i].getProbeRadius()) );
            applyPdbMode(m_aModes[i]);
            break;
        }

//     case GM_TOON:
//     {
//      if (m_pToonDlg == NULL)
//      {
//          m_pToonDlg = new dlg_toon(g_pWindow, m_pPdbNode);
//      }
//      oStr.sprintf( "Toon Settings (%s)", getNameShort().c_str() );
//      m_pToonDlg->setWindowTitle(oStr);
//      m_pToonDlg->raise();
//      m_pToonDlg->show();
//      break;
//     }
    }
}

/**
 * the user wants to adjust the color settings
 */
void pdb_document::sAdjColor()
{
    int colmap = m_pPdbPropDlg->getCurrentColorMap();
    vector<const char*> array;
    vector<char> arrayC;
    vector<unsigned int> arrayI;
    QString oStr;
    unsigned int i;

    int j = m_pPdbPropDlg->getCurrentPdbModeIndex();

    if (j < 0 || j > m_aModes.numElements())
        return;

    if ((colmap != COLMAP_STRUCTURE) && (colmap != COLMAP_SOLID))
    {
	if (m_pColmapDlg == NULL)
            m_pColmapDlg = new dlg_colmap(g_pWindow, this);
	else 
	    m_pColmapDlg->resetDlg();
    }

    switch(colmap)
    {

        case COLMAP_TYPE:
        {   
            m_pColmapDlg->setLabelName(QString("Atom Name:"));
            array = m_pPdbNode->getAtomNames();
            for(i=0;i<array.size();i++)
		m_pColmapDlg->addColorNameItem(i, array[i]);
            m_pColmapDlg->show();
            break;
        }

        case COLMAP_RESNAME:
        {
            m_pColmapDlg->setLabelName(QString("Residue Name:"));
            array = m_pPdbNode->getAtomResnames();
            for(i=0;i<array.size();i++)
                m_pColmapDlg->addColorNameItem(i, array[i]);
            m_pColmapDlg->show();
            break;
        }

        case COLMAP_STRUCTURE:
        {
            if (m_pColmapDlgSecStruct == NULL)
	    {
	        m_pColmapDlgSecStruct = new dlg_colmapsecstruct(g_pWindow, this);
	       
                m_pColmapDlgSecStruct->setLabelName(QString("Sec. Structure:"));
                arrayC.push_back('H');
                arrayC.push_back('S');
                arrayC.push_back('C');
                
		for(i=0;i<arrayC.size();i++)
                {
                    oStr.sprintf("%c", arrayC[i]);
                    m_pColmapDlgSecStruct->addColorNameItem(i, oStr);
                }
               
                if (m_pCartoonDlg != NULL)
                {
                    m_pCartoonDlg->setColorDlgSecStruct(m_pColmapDlgSecStruct);
                    m_pColmapDlgSecStruct->setCartoonPropDlg(m_pCartoonDlg);
                }
	    }
            
            m_pColmapDlgSecStruct->raise();
            m_pColmapDlgSecStruct->show();
            break;
        }

        case COLMAP_CHAINID:
        {
            m_pColmapDlg->setLabelName(QString("Chain ID:"));
            arrayC = m_pPdbNode->getAtomChainIDs();
            for(i=0;i<arrayC.size();i++)
            {
                oStr.sprintf("%c", arrayC[i]);
                m_pColmapDlg->addColorNameItem(i, oStr);
            }
            m_pColmapDlg->show();
            break;
        }

        case COLMAP_MODEL:
        {
            m_pColmapDlg->setLabelName(QString("Model:"));
            arrayI = m_pPdbNode->getAtomModels();
            for(i=0;i<arrayI.size();i++)
            {
                oStr.sprintf("%i", arrayI[i]);
                m_pColmapDlg->addColorNameItem(i, oStr);
            }
            m_pColmapDlg->show();
            break;
        }

        case COLMAP_SOLID:
        {
            svt_palette* pPalette = m_cSolidColmap.getPalette();
            unsigned int iSelected = m_cSolidColmap.getMapping( NULL );

	    if ( m_pColorSelectDlg == NULL )
		m_pColorSelectDlg = new dlg_colorselect( g_pWindow, pPalette );
    
            m_pColorSelectDlg->setCurrent( iSelected );

            if (m_pColorSelectDlg->exec() == QDialog::Accepted)
            {
                m_cSolidColmap.changeMapping(NULL, m_pColorSelectDlg->getCurrent() );
                m_pPdbNode->rebuildDL();
            }
            break;
        }
    }

    if (m_pDLItem != NULL)
        m_pDLItem->showColor( colmap == COLMAP_SOLID );
}


/**
 * the user has clicked on change color in the colormap dlg
 */
void pdb_document::buttonChangeColor(unsigned int iName)
{
    int i = m_pPdbPropDlg->getCurrentPdbModeIndex();

    if (i < 0 || i > m_aModes.numElements())
        return;

    int colmap = m_pPdbPropDlg->getCurrentColorMap();
   // unsigned int name;
    vector<const char*> array;
    vector<char> arrayC;
    vector<unsigned int> arrayI;
    char oStr[256];
    const char* pStr =NULL;
    svt_atom_colmap* pColmap = NULL;

    switch(colmap)
    {
        case COLMAP_TYPE:
            pColmap = &m_cTypeColmap;
            array = m_pPdbNode->getAtomNames();
           // name = iName;
            pStr = array[iName];
            break;
        case COLMAP_RESNAME:
            pColmap = &m_cResnameColmap;
            array = m_pPdbNode->getAtomResnames();
           // name = iName;
            pStr = array[iName];
            break;
        case COLMAP_CHAINID:
            pColmap = &m_cChainIDColmap;
            arrayC = m_pPdbNode->getAtomChainIDs();
           // name = iName;
            if (iName < arrayC.size())
                sprintf(oStr,"%c", arrayC[iName]);
            pStr = oStr;
            break;
        case COLMAP_MODEL:
            pColmap = &m_cModelColmap;
            arrayI = m_pPdbNode->getAtomModels();
           // name = iName;
            if (iName < arrayI.size())
                sprintf(oStr,"%i", arrayI[iName]);
            pStr = oStr;
            break;
        case COLMAP_STRUCTURE:
            pColmap = &m_cStructureColmap;
            arrayC.push_back('H');
            arrayC.push_back('E');
            arrayC.push_back('T');
          //  name = iName;
            if (iName < arrayC.size())
                sprintf(oStr,"%c", arrayC[iName]);
            pStr = oStr;
            break;
    }

    if (m_pDLItem != NULL)
        m_pDLItem->showColor( colmap == COLMAP_SOLID );

    svt_palette* pPalette = pColmap->getPalette();
    unsigned int iSelected = pColmap->getMapping( pStr );

    if ( m_pColorSelectDlg == NULL )
        m_pColorSelectDlg = new dlg_colorselect( g_pWindow, pPalette );
    
    m_pColorSelectDlg->setCurrent( iSelected );

    if (m_pColorSelectDlg->exec() == QDialog::Accepted)
    {
        pColmap->changeMapping(pStr, m_pColorSelectDlg->getCurrent() );
        m_pPdbNode->rebuildDL();
    }
}


/**
 * the user has change the state of the auto apply check box
 */
void pdb_document::sAutoApply( bool bAutoApply )
{
    m_bAutoApply = bAutoApply;
};

/**
 *
 */
bool pdb_document::getAutoApply()
{
    return m_bAutoApply;
};


/**
 * the user has changed the modes and want to update the scene now (to make the changes visible).
 * this is a QT slot.
 */
void pdb_document::sApply()
{
    m_pPdbNode->getNodeLock()->P();
    m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
    m_pPdbNode->blockWidget( m_pPdbPropDlg );

    for(int i=0; i<m_aModes.numElements(); ++i)
        applyPdbMode( m_aModes[i] );

    m_pPdbNode->rebuildDL();
    m_pPdbNode->getNodeLock()->V();
}

/**
 * the user wants to add a graphics mode
 * this is a QT slot.
 */
void pdb_document::sAddGraphicsMode()
{
    // copy the current selected mode to the new one
    pdb_mode *pMode = getPdbMode();
    if (pMode)
    {
        pdb_mode m = *pMode;
        m_aModes.addElement(m);
        rebuildModeList();

        // the new mode should be the selected mode
        int i = m_aModes.numElements()-1;
        m_pPdbPropDlg->setCurrentPdbModeIndex(i);
    }

    // automatically update
    if (m_bAutoApply == true)
    {
        m_pPdbNode->getNodeLock()->P();
        m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
        m_pPdbNode->blockWidget( m_pPdbPropDlg );

        for(int i=0;i<m_aModes.numElements();i++)
            applyPdbMode(m_aModes[i]);

        m_pPdbNode->rebuildDL();
        m_pPdbNode->getNodeLock()->V();
    }
}

/**
 * the user wants to insert a graphics mode
 * this is a QT slot.
 */
void pdb_document::sInsGraphicsMode()
{
    int curr_mode = m_pPdbPropDlg->getCurrentPdbModeIndex();

    // copy the current selected mode to the new one
    pdb_mode *pMode = getPdbMode();

    if (pMode)
    {
        pdb_mode m = *pMode;
        m_aModes.insElement(m, curr_mode);
        rebuildModeList();

        // the new mode should be the selected mode
        m_pPdbPropDlg->setCurrentPdbModeIndex(curr_mode);
    }

    // automatically update
    if (m_bAutoApply == true)
    {
        m_pPdbNode->getNodeLock()->P();
        m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
        m_pPdbNode->blockWidget( m_pPdbPropDlg );

        for(int i=0;i<m_aModes.numElements();i++)
            applyPdbMode(m_aModes[i]);

        m_pPdbNode->rebuildDL();
        m_pPdbNode->getNodeLock()->V();
    }
}

/// the user wants to delete a graphics mode
void pdb_document::sDelGraphicsMode()
{
    int i = m_pPdbPropDlg->getCurrentPdbModeIndex();

    if (i < 0 || i > m_aModes.numElements() || m_aModes.numElements() == 1)
        return;

    m_pPdbPropDlg->pdbModeRemove(i);
    m_aModes.delElement(i);

    // automatically update
    if (m_bAutoApply == true)
    {
        m_pPdbNode->getNodeLock()->P();
        m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
        m_pPdbNode->blockWidget( m_pPdbPropDlg );

        for(int i=0;i<m_aModes.numElements();i++)
            applyPdbMode(m_aModes[i]);

        m_pPdbNode->rebuildDL();
        m_pPdbNode->getNodeLock()->V();
    }
}

///////////////////////////////////////////////////////////////////////////////
// private helper functions
///////////////////////////////////////////////////////////////////////////////

/// rebuild the complete mode list, according to the current internal mode array
void pdb_document::rebuildModeList()
{
    m_pPdbPropDlg->pdbModesClear();

    // now insert all nodes of the document
    int i;
    for( i=0; i<m_aModes.numElements(); ++i)
        m_pPdbPropDlg->pdbModeAdd( calcModeString(i) );

    m_pPdbPropDlg->setCurrentPdbModeIndex(0);
}

/// apply changes to the graphics mode
void pdb_document::applyPdbMode(pdb_mode& m)
{
    int iAtom = -1;
    svt_atom* pAtom = NULL;
    svt_atom* pOldAtom = NULL;
    vector<const char*> array;
    vector<char> arrayC;
    vector<int> arrayI;
    vector<unsigned int> arrayUI;
    int i;

    // let's block the properties dialog from the sculptor_svt_pdb object...
    m_pPdbNode->blockWidget( m_pPdbPropDlg );

    // all atoms
    switch(m.getSelectionMode())
    {
        case MODE_ALL:
            switch(m.getGraphicsMode())
            {
                case GM_OFF:
                    m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
                    break;
                case GM_POINT:
                    m_pPdbNode->setDisplayMode(SVT_PDB_POINT);
                    break;
                case GM_LINE:
                    m_pPdbNode->setDisplayMode(SVT_PDB_LINE);
                    break;
                case GM_CPK:
                    if (m.getSphereScale() != 0.0f)
                        m_pPdbNode->setCPKRadiusScaling(m.getSphereScale());
                    m_pPdbNode->setDisplayMode(SVT_PDB_CPK);
                    break;
                case GM_VDW:
                    if (m.getSphereScale() != 0.0f)
                        m_pPdbNode->setVDWRadiusScaling(m.getSphereScale());
                    m_pPdbNode->setDisplayMode(SVT_PDB_VDW);
                    break;
                case GM_DOTTED:
                    if (m.getSphereScale() != 0.0f)
                        m_pPdbNode->setVDWRadiusScaling(m.getSphereScale());
                    m_pPdbNode->setDisplayMode(SVT_PDB_DOTTED);
                    break;
                case GM_LICORIZE:
                    m_pPdbNode->setDisplayMode(SVT_PDB_LICORIZE);
                    break;
                case GM_TUBE:
                    m_pPdbNode->setTubeDiameter(m.getTubeDiameter());
                    m_pPdbNode->setDisplayMode(SVT_PDB_TUBE);
                    break;
                case GM_NEWTUBE:
                    m_pPdbNode->setDisplayMode(SVT_PDB_NEWTUBE);
                    break;
                case GM_CARTOON:
                    m_pPdbNode->setDisplayMode(SVT_PDB_CARTOON);
                    break;
                case GM_SURF:
                    m_pPdbNode->setProbeRadius( m.getProbeRadius() );
                    m_pPdbNode->setDisplayMode(SVT_PDB_SURF);
                    break;
//         case GM_TOON:
//             m_pPdbNode->setDisplayMode(SVT_PDB_TOON);
//             break;
            }

            // adjust the colormapping
            switch(m.getColmapType())
            {
                case COLMAP_TYPE:
                    m_pPdbNode->setAtomColmap(&m_cTypeColmap);
                    break;
                case COLMAP_RESNAME:
                    m_pPdbNode->setAtomColmap(&m_cResnameColmap);
                    break;
                case COLMAP_CHAINID:
                    m_pPdbNode->setAtomColmap(&m_cChainIDColmap);
                    break;
                case COLMAP_SEGID:
                    m_pPdbNode->setAtomColmap(&m_cSegIDColmap);
                    break;
                case COLMAP_SOLID:
                    m_pPdbNode->setAtomColmap(&m_cSolidColmap);
                    if (m_pDLItem != NULL)
                        m_pDLItem->showColor();
                    break;
                case COLMAP_STRUCTURE:
                    m_pPdbNode->setAtomColmap(&m_cStructureColmap);
                    break;
                case COLMAP_MODEL:
                    m_pPdbNode->setAtomColmap(&m_cModelColmap);
                    break;
                case COLMAP_BETA:
                    m_pPdbNode->setAtomColmap(&m_cBetaColmap);
                    break;
            }

            // adjust the text labeling
            switch(m.getLabelMode())
            {
                case LABEL_OFF:
                    m_pPdbNode->setLabelMode(SVT_ATOM_LABEL_OFF);
                    break;
                case LABEL_INDEX:
                    m_pPdbNode->setLabelMode(SVT_ATOM_LABEL_INDEX);
                    break;
                case LABEL_TYPE:
                    m_pPdbNode->setLabelMode(SVT_ATOM_LABEL_TYPE);
                    break;
                case LABEL_RESNAME:
                    m_pPdbNode->setLabelMode(SVT_ATOM_LABEL_RESNAME);
                    break;
                case LABEL_RESID:
                    m_pPdbNode->setLabelMode(SVT_ATOM_LABEL_RESID);
                    break;
                case LABEL_MODEL:
                    m_pPdbNode->setLabelMode(SVT_ATOM_LABEL_MODEL);
                    break;
                case LABEL_CHAINID:
                    m_pPdbNode->setLabelMode(SVT_ATOM_LABEL_CHAINID);
                    break;
            }
            break;

        case MODE_TYPE:
            // some atom type
            m_pPdbNode->resetAtomEnum();
            array = m_pPdbNode->getAtomNames();
            iAtom = m_pPdbNode->enumAtomType(array[m.getSelectionName()]);

            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomType(array[m.getSelectionName()]);
            }
            break;

        case MODE_RESNAME:
            m_pPdbNode->resetAtomEnum();
            array = m_pPdbNode->getAtomResnames();
            iAtom = m_pPdbNode->enumAtomResname(array[m.getSelectionName()]);

            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomResname(array[m.getSelectionName()]);
            }
            break;

        case MODE_INDEX:
            for(i=m.getSelectionName();i<=m.getSelectionTo();i++)
            {
                pOldAtom = pAtom;
                pAtom = m_pPdbNode->getRenAtom(i);
                setAtomGM(pAtom, m);
            }
            break;

        case MODE_CHAINID:
            m_pPdbNode->resetAtomEnum();
            arrayC = m_pPdbNode->getAtomChainIDs();
            iAtom = m_pPdbNode->enumAtomChainID(arrayC[m.getSelectionName()]);

            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomChainID(arrayC[m.getSelectionName()]);
            }
            break;

        case MODE_SEGID:
            m_pPdbNode->resetAtomEnum();
            array = m_pPdbNode->getAtomSegmentIDs();
            iAtom = m_pPdbNode->enumAtomSegmentID(array[m.getSelectionName()]);

            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomSegmentID(array[m.getSelectionName()]);
            }
            break;

        case MODE_RESSEQ:
            arrayI = m_pPdbNode->getAtomResidueSeqs();
            for(i=m.getSelectionName();i<m.getSelectionTo();i++)
            {
                m_pPdbNode->resetAtomEnum();
                iAtom = m_pPdbNode->enumAtomResidueSeq(arrayI[i]);

                while (iAtom != -1)
                {
                    pAtom = m_pPdbNode->getRenAtom(iAtom);
                    setAtomGM(pAtom, m);
                    pOldAtom = pAtom;
                    iAtom = m_pPdbNode->enumAtomResidueSeq(arrayI[i]);
                }
            }
            break;

        case MODE_MODEL:
            m_pPdbNode->resetAtomEnum();
            arrayUI = m_pPdbNode->getAtomModels();
            iAtom = m_pPdbNode->enumAtomModel(arrayUI[m.getSelectionName()]);

            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomModel(arrayUI[m.getSelectionName()]);
            }
            break;

        case MODE_NUCLEOTIDE:
            m_pPdbNode->resetAtomEnum();

            iAtom = m_pPdbNode->enumAtomNucleotide();
            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomNucleotide();
            }
            break;

        case MODE_HET:
            m_pPdbNode->resetAtomEnum();

            iAtom = m_pPdbNode->enumAtomHet();
            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomHet();
            }
            break;

        case MODE_WATER:
            m_pPdbNode->resetAtomEnum();

            iAtom = m_pPdbNode->enumAtomWater();
            while (iAtom != -1)
            {
                pAtom = m_pPdbNode->getRenAtom(iAtom);
                setAtomGM(pAtom, m);
                pOldAtom = pAtom;
                iAtom = m_pPdbNode->enumAtomWater();
            }
            break;
    }

    // add the last atom twice for the tube spline mode
    if (m.getSelectionMode() != MODE_ALL && m.getGraphicsMode() == GM_TUBE && pOldAtom)
        setAtomGM(pOldAtom,m);
}

// set an atom and all connected bonds to a graphics mode
void pdb_document::setAtomGM(svt_atom* atom, pdb_mode m)
{
    int bond_gm = SVT_BOND_LINE;
    svt_atom_colmap* bond_colmap =NULL;

    // adjust atom gm
    switch(m.getGraphicsMode())
    {
        case GM_OFF:
            atom->setDisplayMode(SVT_ATOM_OFF);
            bond_gm = SVT_BOND_OFF;
            break;
        case GM_POINT:
            atom->setDisplayMode(SVT_ATOM_POINT);
            bond_gm = SVT_BOND_OFF;
            break;
        case GM_LINE:
            atom->setDisplayMode(SVT_ATOM_POINT);
            bond_gm = SVT_BOND_LINE;
            break;
        case GM_CPK:
            atom->setDisplayMode(SVT_ATOM_CPK);
            if (m.getSphereScale() != 0.0f)
                atom->setCPKRadiusScaling(m.getSphereScale());
            bond_gm = SVT_BOND_CYLINDER;
            break;
        case GM_VDW:
            atom->setDisplayMode(SVT_ATOM_VDW);
            if (m.getSphereScale() != 0.0f)
                atom->setVDWRadiusScaling(m.getSphereScale());
            bond_gm = SVT_BOND_OFF;
            break;
        case GM_DOTTED:
            atom->setDisplayMode(SVT_ATOM_DOTTED);
            if (m.getSphereScale() != 0.0f)
                atom->setVDWRadiusScaling(m.getSphereScale());
            bond_gm = SVT_BOND_OFF;
            break;
        case GM_LICORIZE:
            atom->setDisplayMode(SVT_ATOM_LICORIZE);
            bond_gm = SVT_BOND_CYLINDER;
            break;
        case GM_TUBE:
            atom->setDisplayMode(SVT_ATOM_TUBE);
            bond_gm = SVT_BOND_OFF;
            break;
        case GM_NEWTUBE:
            atom->setDisplayMode(SVT_ATOM_NEWTUBE);
            bond_gm = SVT_BOND_OFF;
            break;
        case GM_CARTOON:
            atom->setDisplayMode(SVT_ATOM_CARTOON);
            bond_gm = SVT_BOND_OFF;
            break;
        case GM_SURF:
            // the probe radius is always global, even if it was selected for specifically for a single atom...
            m_pPdbNode->setProbeRadius( m.getProbeRadius() );
            atom->setDisplayMode(SVT_ATOM_SURF);
            bond_gm = SVT_BOND_OFF;
            break;
//     case GM_TOON:
//         atom->setDisplayMode(SVT_ATOM_TOON);
//         bond_gm = SVT_BOND_OFF;
//         break;
    }

    // adjust the colormapping
    switch(m.getColmapType())
    {
        case COLMAP_TYPE:
            atom->setAtomColmap(&m_cTypeColmap);
            bond_colmap = &m_cTypeColmap;
            break;
        case COLMAP_RESNAME:
            atom->setAtomColmap(&m_cResnameColmap);
            bond_colmap = &m_cResnameColmap;
            break;
        case COLMAP_SOLID:
            atom->setAtomColmap(&m_cSolidColmap);
            bond_colmap = &m_cSolidColmap;
            break;
        case COLMAP_CHAINID:
            atom->setAtomColmap(&m_cChainIDColmap);
            bond_colmap = &m_cChainIDColmap;
            break;
        case COLMAP_SEGID:
            atom->setAtomColmap(&m_cSegIDColmap);
            bond_colmap = &m_cSegIDColmap;
            break;
        case COLMAP_STRUCTURE:
            atom->setAtomColmap(&m_cStructureColmap);
            bond_colmap = &m_cStructureColmap;
            break;
        case COLMAP_MODEL:
            atom->setAtomColmap(&m_cModelColmap);
            bond_colmap = &m_cModelColmap;
            break;
        case COLMAP_BETA:
            atom->setAtomColmap(&m_cBetaColmap);
            bond_colmap = &m_cBetaColmap;
            break;
    }

    // adjust text labels
    switch(m.getLabelMode())
    {
        case LABEL_OFF:
            atom->setLabelMode(SVT_ATOM_LABEL_OFF);
            break;
        case LABEL_INDEX:
            atom->setLabelMode(SVT_ATOM_LABEL_INDEX);
            break;
        case LABEL_TYPE:
            atom->setLabelMode(SVT_ATOM_LABEL_TYPE);
            break;
        case LABEL_RESNAME:
            atom->setLabelMode(SVT_ATOM_LABEL_RESNAME);
            break;
        case LABEL_RESID:
            atom->setLabelMode(SVT_ATOM_LABEL_RESID);
            break;
        case LABEL_MODEL:
            atom->setLabelMode(SVT_ATOM_LABEL_MODEL);
            break;
        case LABEL_CHAINID:
            atom->setLabelMode(SVT_ATOM_LABEL_CHAINID);
            break;
    };

    // adjust bonds
    vector<unsigned int> bond_array = atom->getBondList();
    unsigned int i;
    svt_atom* pAtomA;
    svt_atom* pAtomB;

    for (i=0;i<bond_array.size();i++)
    {
        svt_bond* pBond = m_pPdbNode->getRenBond(bond_array[i]);

        if (pBond != NULL)
        {
            pAtomA = (svt_atom*)pBond->getAtomA();
            pAtomB = (svt_atom*)pBond->getAtomB();

            if (pAtomA != NULL && pAtomB != 0 )
            {
                pBond->setDisplayMode(bond_gm);
                pBond->setAtomColmap(bond_colmap);
                if ( m.getGraphicsMode() == GM_CPK && m.getSphereScale() != 0.0f )
                    pBond->setCylinderRad( m.getSphereScale() / 0.2 );

            } else
                pBond->setDisplayMode( SVT_BOND_OFF );
        }
    }
}

/// calculate the mode string
QString pdb_document::calcModeString(int i)
{
    QString s;
    QString s2;
    vector<const char*> array;
    vector<char> arrayC;
    vector<int> arrayI;
    vector<unsigned int> arrayUI;

    int sel_mode = m_aModes[i].getSelectionMode();
    int sel_name = m_aModes[i].getSelectionName();
    int sel_to   = m_aModes[i].getSelectionTo();
    int mode     = m_aModes[i].getGraphicsMode();
    int colmap   = m_aModes[i].getColmapType();
    int label    = m_aModes[i].getLabelMode();

    switch(sel_mode)
    {
        case MODE_ALL:
            s += "All";
            break;
        case MODE_TYPE:
            array = m_pPdbNode->getAtomNames();
            s += "Name ( ";
            s += array[sel_name];
            s += " )";
            break;
        case MODE_RESNAME:
            array = m_pPdbNode->getAtomResnames();
            s += "Resname ( ";
            s += array[sel_name];
            s += " )";
            break;
        case MODE_INDEX:
            s += "Index ( ";
            s2.sprintf("%i..%i", sel_name+1, sel_to+1);
            s += s2;
            s += " )";
            break;
        case MODE_CHAINID:
            arrayC = m_pPdbNode->getAtomChainIDs();
            s += "ChainID ( ";
            s += arrayC[sel_name];
            s += " )";
            break;
        case MODE_SEGID:
            array = m_pPdbNode->getAtomSegmentIDs();
            s += "SegID ( ";
            s += array[sel_name];
            s += " )";
            break;
        case MODE_RESSEQ:
            arrayI = m_pPdbNode->getAtomResidueSeqs();
            s.sprintf("ResSeq( %i..%i )",arrayI[sel_name],arrayI[sel_to]);
            break;
        case MODE_MODEL:
            arrayUI = m_pPdbNode->getAtomModels();
            s.sprintf("Model( %i )",arrayUI[sel_name]);
            break;
        case MODE_NUCLEOTIDE:
            s += "Nucleotides";
            break;
        case MODE_HET:
            s += "HET Atoms";
            break;
        case MODE_WATER:
            s += "Water Atoms";
            break;
    }

    s += " - ";

    switch(mode)
    {
        case GM_OFF:
            s += "Off";
            break;
        case GM_POINT:
            s += "Point";
            break;
        case GM_LINE:
            s += "Line";
            break;
        case GM_CPK:
            s += "CPK";
            break;
        case GM_VDW:
            s += "VDW";
            break;
        case GM_DOTTED:
            s += "Dotted";
            break;
        case GM_LICORIZE:
            s += "Licorize";
            break;
        case GM_TUBE:
            s += "Tube";
            break;
        case GM_NEWTUBE:
            s += "New Tube";
            break;
        case GM_CARTOON:
            s += "Cartoon";
            break;
        case GM_SURF:
            s += "Surface";
            break;
//     case GM_TOON:
//         s += "Toon";
//         break;
    }

    s += " - Color:";

    switch(colmap)
    {
        case COLMAP_TYPE:
            s += "Name";
            break;
        case COLMAP_RESNAME:
            s += "Resname";
            break;
        case COLMAP_CHAINID:
            s += "ChainID";
            break;
        case COLMAP_SEGID:
            s += "SegID";
            break;
        case COLMAP_SOLID:
            s += "Solid";
            break;
        case COLMAP_STRUCTURE:
            s += "Structure";
            break;
        case COLMAP_MODEL:
            s += "Model";
            break;
        case COLMAP_BETA:
            s += "Beta";
            break;
    }

    s += " (Label:";

    switch(label)
    {
        case LABEL_OFF:
            s += "Off";
            break;
        case LABEL_INDEX:
            s += "Index";
            break;
        case LABEL_TYPE:
            s += "Name";
            break;
        case LABEL_RESNAME:
            s += "ResName";
            break;
        case LABEL_RESID:
            s += "ResSeq";
            break;
        case LABEL_MODEL:
            s += "Model";
            break;
        case LABEL_CHAINID:
            s += "ChainID";
            break;
    }
    s += ")";

    return s;
}

/**
 * the user has changed something on the combo boxes
 */
void pdb_document::updatePdbMode()
{
    if (m_bAutoApply == true)
    {
        // automatically update
        m_pPdbNode->getNodeLock()->P();
        m_pPdbNode->setDisplayMode( SVT_PDB_OFF );
        m_pPdbNode->blockWidget( m_pPdbPropDlg );

        for(int i=0; i<m_aModes.numElements(); ++i)
            applyPdbMode( m_aModes[i] );

        m_pPdbNode->rebuildDL();
        m_pPdbNode->getNodeLock()->V();
    }

    if (m_pDLItem != NULL)
        m_pDLItem->showColor( getPdbMode()->getColmapType() == COLMAP_SOLID );
}

/**
 * get a pdb_mode. if none specified, get the currently selected
 * \return pointer to the currently selected pdb_mode or NULL if nothing is selected
 */
pdb_mode* pdb_document::getPdbMode(int iIndex)
{
    if (iIndex == -1)
        iIndex = m_pPdbPropDlg->getCurrentPdbModeIndex();

    if ( iIndex < 0 || iIndex > m_aModes.numElements() )
        return NULL;
    else
        return &( m_aModes[iIndex] );
}

///////////////////////////////////////////////////////////////////////////////
// sculptor_document functions
///////////////////////////////////////////////////////////////////////////////

/**
 * get the type of the document
 * \return SENSITUS_DOC_PDB
 */
int pdb_document::getType()
{
    return SENSITUS_DOC_PDB;
}

/**
 * get the position of the pdb document in real space (e.g. for pdb or situs files)
 * \return svt_vector4f with the position
 */
svt_vector4f pdb_document::getRealPos()
{
    svt_vector4f oPos(
        0.0,
        0.0,
        0.0
        );

    if (m_pPdbNode != NULL && m_pPdbNode->getTransformation() != NULL)
    {
        oPos.x( (m_pPdbNode->getTransformation()->getTranslationX() * 1.0E-8f) );
        oPos.y( (m_pPdbNode->getTransformation()->getTranslationY() * 1.0E-8f) );
        oPos.z( (m_pPdbNode->getTransformation()->getTranslationZ() * 1.0E-8f) );
    }

    return oPos;
}

/**
 * get the position of the atom iIndexAtom in real space
 * \param iIndexAtom the index of the atom
 * \param oOrigin the systems origin
 */
svt_vector4f pdb_document::getRealPos(unsigned int iIndexAtom, svt_vector4f oOrigin)
{
    // translate object
    svt_vector4<Real64> oVec( (*m_pPdbNode)[iIndexAtom]);
    svt_matrix4<Real64> oMat( *(m_pPdbNode->getTransformation()) );
    oMat.translate( oOrigin.x(), oOrigin.y(), oOrigin.z() );

    oVec *= 1.0E-2;
    oVec.w( 1.0f );

    oVec = oMat * oVec;

    oVec *= 1.0E2;

    svt_vector4f oPos(oVec.x(),oVec.y(),oVec.z());
    return oPos;

}

/**
 * get the point cloud in real space
 * \return pointer svt_point_cloud_pdb with the coordinates in real space
 */
svt_point_cloud_pdb< svt_vector4< Real64 > >* pdb_document::getPDB()
{
    //get Transformation
    svt_matrix4<Real64> oMat( *(m_pPdbNode->getTransformation()) );
    oMat[0][3] *= 1.0E2; oMat[1][3] *= 1.0E2; oMat[2][3] *= 1.0E2;

    svt_vector4<Real64> oVec;
    svt_point_cloud_pdb< svt_vector4<Real64> >* pPdb = new svt_point_cloud_pdb< svt_vector4<Real64> >();

    unsigned int iSize = (*m_pPdbNode).size();
    for (unsigned int iIndexAtom=0; iIndexAtom < iSize; iIndexAtom++)
    {
        oVec = (*m_pPdbNode)[iIndexAtom] ;
        oVec = oMat * oVec;

        pPdb->addAtom(*(*m_pPdbNode).getAtom(iIndexAtom), oVec);
    }

    return pPdb;
}

/**
 * save the document
 * \param oOrigin origin (the first loaded document sets the origin)
 * \param pFilename pointer to an array of char with the filename
 */
void pdb_document::save(string oFilename)
{
    if (oFilename.size()==0)
        return;
    //
    // if solution/probe in IDG, save a copy, the internal coordinates of the object remain unsaved
    //
    // if the document is a solution for IGD with Eliquos data,just save a copy, don't do anything else
    if  ((this->getSolution() && this->getCopy() && g_pWindow->getProbeDoc() != NULL  && g_pWindow->getProbeDoc()->getEliDataLoaded()) || // IS solution
         (this == g_pWindow->getProbeDoc() &&  g_pWindow->getProbeDoc()->getEliDataLoaded()))                                             // IS Probe
    {
        svtout << "Document is IGD solution and was saved in " << oFilename  << endl;

        svt_pdb* pNewPdbNode = new svt_pdb((svt_pdb&) (*this->getNode()));

        // translate object and save
        svt_vector4<Real64> oVec;
        svt_matrix4<Real64> oMat( *(m_pPdbNode->getTransformation()) ); oMat[0][3] *= 1.0E2; oMat[1][3] *= 1.0E2; oMat[2][3] *= 1.0E2;

        // adjust coordinates
        for( unsigned int i=0; i<pNewPdbNode->size(); i++)
            (*pNewPdbNode)[i] = oMat * (*pNewPdbNode)[i];

        pNewPdbNode->writePDB( oFilename.c_str() );

        delete pNewPdbNode;
        return;
    }

    //
    // Call the base class to adjust filename
    //
    sculptor_document::save(oFilename);


    //
    // Adjust all the transformations stored in the transformation list
    //
    svt_matrix4<Real64> oMatInv( *(m_pPdbNode->getTransformation()) );
    oMatInv.invert();
    for( unsigned int i=0; i<m_aTransforms.size(); i++ )
    {
        svt_matrix4<Real64> oTmpMat = m_aTransforms[i].getMatrix();
        oTmpMat = oTmpMat * oMatInv;
        m_aTransforms[i].setMatrix( oTmpMat );
    };

    //
    // Translate object and save
    //
    svt_matrix4<Real64> oMat( *(m_pPdbNode->getTransformation()) ); oMat[0][3] *= 1.0E2; oMat[1][3] *= 1.0E2; oMat[2][3] *= 1.0E2;
    for( int iTS=0; iTS<m_pPdbNode->getMaxTimestep(); iTS++)
    {
        m_pPdbNode->setTimestep( iTS );
        for(unsigned int j=0;j<m_pPdbNode->size(); j++)
            (*m_pPdbNode)[j] = oMat * (*m_pPdbNode)[j];
    }
    m_pPdbNode->setTimestep( 0 );
    m_pPdbNode->writePDB( oFilename.c_str() );

    //
    // Now re-center the structure. The transformation was applied to the coordinates in the previous step, which means that the center might have changed (rotations).
    //
    m_oCenter = m_pPdbNode->coa();
    svt_matrix4 <Real64> oTranslation;
    oTranslation.loadIdentity();
    oTranslation.setTranslation(-m_oCenter);
    int iCurrentTimestep = m_pPdbNode->getTimestep();
    for(int iTS=0; iTS<m_pPdbNode->getMaxTimestep(); iTS++)
    {
        m_pPdbNode->setTimestep( iTS );
        for(unsigned int j=0;j<m_pPdbNode->size(); j++)
            (*m_pPdbNode)[j] = oTranslation * (*m_pPdbNode)[j];
    }
    m_pPdbNode->setTimestep( iCurrentTimestep );

    //
    // Now we have to calculate a new transformation matrix, based on the new center.
    //
    m_pPdbNode->getTransformation()->setToId();
    m_pPdbNode->getTransformation()->setFromTranslation( (1.0E-2) * m_oCenter.x(), (1.0E-2) * m_oCenter.y(), (1.0E-2) * m_oCenter.z() );

    //
    // Now move all transformations into the new origin
    //
    for( unsigned int i=0; i<m_aTransforms.size(); i++ )
    {
        svt_matrix4<Real64> oTmpMat = m_aTransforms[i].getMatrix();
        svt_matrix4<Real64> oTmpMat2; oTmpMat2.translate( (1.0E-2) * m_oCenter.x(), (1.0E-2) * m_oCenter.y(), (1.0E-2) * m_oCenter.z() );
        oTmpMat = oTmpMat * oTmpMat2;
        m_aTransforms[i].setMatrix( oTmpMat );
    }

    //
    // Adjust codebook vector coordinates
    //
    for(unsigned int j=0; j<m_aCodebook.size(); j++)
        m_aCodebook[j] = (oMat*m_aCodebook[j]) - m_oCenter;

    //
    // Update renderig and store the current position as initial position for this document
    //
    m_pPdbNode->rebuildDL();
    updateCodebookRendering();
    storeInitMat();

}
/**
 * is this document saveable?
 * \return true if the document can be saved to disk
 */
bool pdb_document::getSaveable()
{
    return true;
}

/**
 * Get a representative color for the document
 * \return svt_color object
 */
svt_color pdb_document::getColor()
{
    svt_color oColor;

    if (m_aModes[0].getColmapType() == COLMAP_SOLID)
    {
        oColor.setR( m_cSolidColmap.getMappingRed( NULL ) );
        oColor.setG( m_cSolidColmap.getMappingGreen( NULL ) );
        oColor.setB( m_cSolidColmap.getMappingBlue( NULL ) );
    }

    return oColor;
}

/**
 * Get high contrast palette
 *\return svt_palette the high constrast palette;
 */
svt_palette* pdb_document::getHighContrastPalette()
{
    return s_pHighContrastPalette;
};

/**
 * Get Low contrast palette
 *\return svt_palette the low constrast palette;
 */
svt_palette* pdb_document::getLowContrastPalette()
{
    return s_pLowContrastPalette;
};

/**
 * Get the number of atoms of the molecule
 * \return number of atoms
 */
unsigned int pdb_document::size()
{
    return m_pPdbNode->size();
};

/**
 * calculate the sphericity
 */
void pdb_document::showSphericity()
{
    QString oString;
    oString.sprintf("Sphericity of the atomic structure: %5.2f", m_pPdbNode->getSphericity());

    QMessageBox::information(0, "Sphericity", oString);
}

/**
 * show table with all values
 */
void pdb_document::showTable()
{
    if (m_pAtomTable == NULL)
    {
        m_pAtomTable = new dlg_atomtable(g_pWindow, this);
        m_pAtomTable->setWindowTitle( QString("Atom Details for ") + StdtoQ(getDisplayName()) );
    }

    m_pAtomTable->raise();
    m_pAtomTable->show();
}

///////////////////////////////////////////////////////////////////////////////
// rmsd
///////////////////////////////////////////////////////////////////////////////

/**
 * calculate the rmsd between two pdb documents
 * \param pDoc pointer to the second pdb document
 * \return rmsd
 */
double pdb_document::getRmsd( pdb_document* pDoc )
{
    svt_pdb* pFirst = (svt_pdb*)getNode();
    svt_pdb* pSecond = (svt_pdb*)pDoc->getNode();

    pFirst->getNodeLock()->P();
    pSecond->getNodeLock()->P();

    double fRMSD = pFirst->rmsd( (*pSecond) );

    return fRMSD;
}

/**
 * create a svt_point_cloud object out of the current pdb object
 */
svt_point_cloud_pdb< svt_vector4<Real64> >& pdb_document::getPointCloud()
{
    return *m_pPdbNode;
};

/**
 * get special point cloud for refinement
 */
svt_point_cloud_pdb< svt_vector4<Real64> >& pdb_document::getRefinementPC()
{
    if (m_oRefinementPC.size() == 0)
        getPointCloud();

    return m_oRefinementPC;
};

///////////////////////////////////////////////////////////////////////////////
// subtract this pdb structure from a volumetric map
///////////////////////////////////////////////////////////////////////////////

/**
 * subtract this crystal structure from a volumetric map
 */
/*void pdb_document::subtractFromMap()
  {
  // first step = look for the first two selected pdb documents
  svt_array<sculptor_document*>* pDocuments = g_pWindow->getDocuments();

  situs_document* pSitus = NULL;

  for(int i=1;i < pDocuments->numElements(); i++)
  {
  if (g_pWindow->getSelected(i) && pDocuments->getAt(i)->getType() == SENSITUS_DOC_SITUS)
  {
  pSitus = (situs_document*)pDocuments->getAt(i);
  break;
  }
  }

  // FIXME: Warning MessageBox should appear...
  if (pSitus == NULL)
  {
  QMessageBox::information(0, "Substract from map", "Please select also a volumetric document.");
  return;
  }

  // second step: set parameters (resolution, etc) - get from situs document!
  bool bWater = true;
  Real64 fResolution = 5.0;
  bool bMassWeight = true;
  Real64 fBFactCutoff = 1000.0;
  Real64 fWidth = pSitus->getVoxelWidth();
  int iKernelType = 1;
  Real64 fKernelAmpl = 1.0;
  bool bAmplCorr = true;

  // what is the target resolution?
  bool bOK;
  QString oText = QInputDialog::getText(
  "Sculptor", "Enter estimated resolution of target map:", QLineEdit::Normal,
  QString::null, &bOK );
  if ( bOK && !oText.isEmpty() )
  fResolution = oText.toDouble();
  else
  return;

  fResolution *= -1.0;

  // third step: calculate the 3D kernel and convolute all the atoms with it...
  svt_volume<Real64>* pVol = m_pPdbNode->blur( bWater, bMassWeight, fBFactCutoff, fWidth, fResolution, iKernelType, fKernelAmpl, bAmplCorr );

  pVol->normalize();
  pSitus->normalize();

  // now substract the new volumetric object from the other situs document
  unsigned int iExtX = pVol->getSizeX();
  unsigned int iExtY = pVol->getSizeY();
  unsigned int iExtZ = pVol->getSizeZ();
  // attention: transformation matrix is in opengl coordinates, i.e. 1 GL = 1.0E-2 Angstroem
  svt_vector4<double> oVec;
  svt_vector4<double> oOrigin;

  // calculate the center of the volume data
  oOrigin.x( (fWidth*(Real64)(pSitus->getExtX()) * 0.5E-2));
  oOrigin.y( (fWidth*(Real64)(pSitus->getExtY()) * 0.5E-2));
  oOrigin.z( (fWidth*(Real64)(pSitus->getExtZ()) * 0.5E-2));
  cout << oOrigin << endl;

  Real64 fFact = fWidth*1.0E-2;

  // matrix from pdb to situs coordinates...
  svt_matrix4<Real64> oTransMatrix;
  oTransMatrix.set(*pSitus->getNode()->getTransformation());
  oTransMatrix.invert();
  //oTransMatrix.translate( pVol->getGridX()*1.0E-2, pVol->getGridY()*1.0E-2, pVol->getGridZ()*1.0E-2 );
  svt_matrix4f* pPDBMat = m_pPdbNode->getTransformation();

  //oPDBMat.setTranslation( pVol->getGridX()*1.0E-2, pVol->getGridY()*1.0E-2, pVol->getGridZ()*1.0E-2 );
  //oTransMatrix *= oPDBMat;

  cout << svt_matrix4<double>(*m_pPdbNode->getTransformation()) << endl;

  Real64 fValue, fVoxelX, fVoxelY, fVoxelZ, fA, fB, fC, fOrig;
  int iV0X, iV0Y, iV0Z;
  int iV1X, iV1Y, iV1Z;

  Real64 fGridX = pVol->getGridX()*1.0E-2;
  Real64 fGridY = pVol->getGridY()*1.0E-2;
  Real64 fGridZ = pVol->getGridZ()*1.0E-2;
  fGridX += pPDBMat->getTranslationX();
  fGridY += pPDBMat->getTranslationY();
  fGridZ += pPDBMat->getTranslationZ();
  fGridX -= m_pPdbNode->getUncenterVec().x()*1.0E8;
  fGridY -= m_pPdbNode->getUncenterVec().y()*1.0E8;
  fGridZ -= m_pPdbNode->getUncenterVec().z()*1.0E8;

  cout << "fGridX: " << fGridX << endl;
  cout << "fGridY: " << fGridY << endl;
  cout << "fGridZ: " << fGridZ << endl;

  svt_vector4<Real64> oOriVec( fGridX, fGridY, fGridZ );
  //oOriVec = svt_vector4<Real64>((fWidth*iExtX*0.5E-2), (fWidth*iExtY*0.5E-2), (fWidth*iExtZ*0.5E-2));

  cout << oOriVec << endl;

  for(unsigned int iX=0; iX < iExtX; iX++)
  for(unsigned int iY=0; iY < iExtY; iY++)
  for(unsigned int iZ=0; iZ < iExtZ; iZ++)
  {
  // calculate 3D world position of voxel
  oVec.x( (Real64)(iX) * fFact );
  oVec.y( (Real64)(iY) * fFact );
  oVec.z( (Real64)(iZ) * fFact );
  oVec += oOriVec;

  // move it into the situs coordinate system
  oVec = oTransMatrix * oVec;
  oVec += oOrigin;
  oVec *= 1.0E-8f;

  // get the index of the voxel lying under the cv
  fVoxelX = oVec.x() / (fWidth*1.0E-10);
  fVoxelY = oVec.y() / (fWidth*1.0E-10);
  fVoxelZ = oVec.z() / (fWidth*1.0E-10);

  iV0X = (int)(floor(fVoxelX));
  iV0Y = (int)(floor(fVoxelY));
  iV0Z = (int)(floor(fVoxelZ));
  iV1X = (int)(ceil(fVoxelX));
  iV1Y = (int)(ceil(fVoxelY));
  iV1Z = (int)(ceil(fVoxelZ));
  fA = (Real64)(iV1X) - fVoxelX;
  fB = (Real64)(iV1Y) - fVoxelY;
  fC = (Real64)(iV1Z) - fVoxelZ;

  cout << " (" << fVoxelX << ", " << fVoxelY << ", " << fVoxelZ << ")" << endl;
  cout << " (" << iV0X << ", " << iV0Y << ", " << iV0Z << ")" << endl;
  //cout << " (" << iV1X << ", " << iV1Y << ", " << iV1Z << ")" << endl;
  //cout << " (" << fA << ", " << fB << ", " << fC << ")" << endl;
  //cout << "-" << endl;

  getchar();

  fOrig = pVol->getValue( iX, iY, iZ );

  // 0 0 0
  fValue = pSitus->getAt( iV0X, iV0Y, iV0Z ) - (fA * fB * fC * fOrig );
  if (fValue < 0) fValue = 0;
  //if (fOrig != 0.0)
  //    pSitus->setAt( iV0X, iV0Y, iV0Z, 10.0 );
  pSitus->setAt( iV0X, iV0Y, iV0Z, fValue );
  // 0 0 1
  fValue = pSitus->getAt( iV1X, iV0Y, iV0Z ) - ((1.0 - fA) * fB * fC * fOrig );
  if (fValue < 0) fValue = 0;
  pSitus->setAt( iV1X, iV0Y, iV0Z, fValue );
  // 0 1 0
  fValue = pSitus->getAt( iV0X, iV1Y, iV0Z ) - (fA * (1.0 - fB) * fC * fOrig );
  if (fValue < 0) fValue = 0;
  pSitus->setAt( iV0X, iV1Y, iV0Z, fValue );
  // 0 1 1
  fValue = pSitus->getAt( iV1X, iV1Y, iV0Z ) - ((1.0 - fA) * (1.0 - fB) * fC * fOrig );
  if (fValue < 0) fValue = 0;
  pSitus->setAt( iV1X, iV1Y, iV0Z, fValue );

  // 1 0 0
  fValue = pSitus->getAt( iV0X, iV0Y, iV1Z ) - (fA * fB * (1.0 - fC) * fOrig );
  if (fValue < 0) fValue = 0;
  pSitus->setAt( iV0X, iV0Y, iV1Z, fValue );
  // 1 0 1
  fValue = pSitus->getAt( iV1X, iV0Y, iV1Z ) - ((1.0 - fA) * fB * (1.0 - fC) * fOrig );
  if (fValue < 0) fValue = 0;
  pSitus->setAt( iV1X, iV0Y, iV1Z, fValue );
  // 1 1 0
  fValue = pSitus->getAt( iV0X, iV1Y, iV1Z ) - (fA * (1.0 - fB) * (1.0 - fC) * fOrig );
  if (fValue < 0) fValue = 0;
  pSitus->setAt( iV0X, iV1Y, iV1Z, fValue );
  // 1 1 1
  fValue = pSitus->getAt( iV1X, iV1Y, iV1Z ) - ((1.0 - fA) * (1.0 - fB) * (1.0 - fC) * fOrig );
  if (fValue < 0) fValue = 0;
  pSitus->setAt( iV1X, iV1Y, iV1Z, fValue );

  }

  pSitus->normalize();
  pSitus->dataChanged();
  }*/

/**
 * blur atomic structure by convolving with gaussian kernel
 */
situs_document* pdb_document::blur(bool bAddDoc, Real64 fWidth, Real64 fResolution)
{
    bool bOK;
    QString oInText, oText;

    // what is the target resolution?
    if (fResolution == 0.0)
    {
        oInText.sprintf( "10.0" );
        oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter resolution of the new map:",
                                       QLineEdit::Normal, oInText, &bOK );
        if ( bOK && !oText.isEmpty() )
            fResolution = oText.toDouble();
        else
            return NULL;

        fResolution *= -1.0;
    }

    // what is the voxel width?
    if (fWidth == 0.0)
    {
        oInText.sprintf( "3.0" );
        oText = QInputDialog::getText(g_pWindow, "Sculptor", "Enter voxel width of the new map:",
                                      QLineEdit::Normal, oInText, &bOK );
        if ( bOK && !oText.isEmpty() )
            fWidth = oText.toDouble();
        else
            return NULL;
    }

    // create new pdb document to incorporate the current transformation
    svt_point_cloud_pdb< svt_vector4<Real64> > oPC = *m_pPdbNode;

    svt_vector4<Real64> oVec;
    svt_matrix4<Real64> oMat( *(m_pPdbNode->getTransformation()) );
    //oMat.translate( oOrigin.x(), oOrigin.y(), oOrigin.z() );

    // adjust coordinates
    for( unsigned int i=0; i<m_pPdbNode->size(); i++)
    {
        oVec = oPC[i];
        oVec *= 1.0E-2;
        oVec.w( 1.0f );
        oVec = oMat * oVec;
        oVec *= 1.0E2;

        oPC[i] = oVec;
    }
    Real64 fMinX = oPC.getMinXCoord();
    Real64 fMinY = oPC.getMinYCoord();
    Real64 fMinZ = oPC.getMinZCoord();

    // center the structure
    svt_matrix4 <Real64> oTranslation;
    oTranslation.loadIdentity();
    oTranslation.setTranslation(-oPC.geometricCenter());

    int iCurrentTimestep = oPC.getTimestep();
    for(int iTS=0; iTS<oPC.getMaxTimestep(); iTS++)
    {
        svtout << "Centering timestep: " << iTS << endl;
        oPC.setTimestep( iTS );

        for( unsigned int i=0; i<oPC.size(); i++)
        {
            oPC[i] = oTranslation * oPC[i];
        }
    }
    oPC.setTimestep( iCurrentTimestep );

    svtout << "Blurring to resolution " << fabs(fResolution) << endl;
    // third step: calculate the 3D kernel and convolute all the atoms with it...
    Real64 fDiffX = fMinX - oPC.getMinXCoord();
    Real64 fDiffY = fMinY - oPC.getMinYCoord();
    Real64 fDiffZ = fMinZ - oPC.getMinZCoord();
    // 1D kernel convolution
    svt_volume<Real64>* pVol = oPC.blur1D( fWidth, fResolution, fDiffX, fDiffY, fDiffZ, true );
    // 3D kernel convolution
    //svt_volume<Real64>* pVol = oPC.blur( fWidth, fResolution, fDiffX, fDiffY, fDiffZ, true );


    // situs file
    string oFname( getNameShort() );
    oFname.append(".situs");

    svtout << "Blurring PDB file completed, adding document \"" << oFname.c_str() << "\"" << endl;

    pVol->setWidth( fWidth );
    situs_document* pSD = new situs_document(g_pWindow->getScene()->getDataTopNode(), oFname, pVol);

    svtout << "Done" << endl;
    svtout << "Modify coordinates" << endl;

    // document position
    svt_vector4f oDocOrigin = pSD->getRealPos();
    oDocOrigin.multScalar( 1.0E8f );
    pSD->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
    pSD->storeInitMat();
    pSD->setChanged();

    // add it to the document lists
    if (bAddDoc)
    {
        g_pWindow->addDoc( (sculptor_document*)pSD );
        g_pWindow->setDefaultProbeTargetDoc();
    }
    else
        pSD->setVisible(false);

    return pSD;
}

/**
 * this routine actually changes from high to low contrast palette
 */
void pdb_document::setHighContrast( bool bHighContrast )
{
    m_bHighContrast = bHighContrast;

    if (m_bHighContrast == true)
    {
        m_cResnameColmap.setPalette( s_pHighContrastPalette );
        m_cChainIDColmap.setPalette( s_pHighContrastPalette );
        m_cSegIDColmap.setPalette( s_pHighContrastPalette );
        m_cSolidColmap.setPalette( s_pHighContrastPalette );
        m_cStructureColmap.setPalette( s_pHighContrastPalette );
        m_cModelColmap.setPalette( s_pHighContrastPalette );
        m_cBetaColmap.setPalette( s_pHighContrastPalette );

    } else {

        m_cResnameColmap.setPalette( s_pLowContrastPalette );
        m_cChainIDColmap.setPalette( s_pLowContrastPalette );
        m_cSegIDColmap.setPalette( s_pLowContrastPalette );
        m_cSolidColmap.setPalette( s_pLowContrastPalette );
        m_cStructureColmap.setPalette( s_pLowContrastPalette );
        m_cModelColmap.setPalette( s_pLowContrastPalette );
        m_cBetaColmap.setPalette( s_pLowContrastPalette );
    }

    m_pPdbNode->rebuildDL();
}

///////////////////////////////////////////////////////////////////////////////
// Quantization method applicable just for atomic structure
///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
void pdb_document::showAtomicQuantizationDlg()
{
    if (m_pAtomClusteringDlg == NULL)
	m_pAtomClusteringDlg = new dlg_atomclustering(g_pWindow, this);
    else
	m_pAtomClusteringDlg->resetDlg(this);
    
    m_pAtomClusteringDlg->show();
}

/**
 * 
 */
void pdb_document::atomicQuantization(unsigned int iToSkipCACount)
{
    m_aCodebook.deleteAllAtoms();
    m_aCodebook.deleteAllBonds();

    svt_point_cloud_pdb<svt_vector4 <Real64> > oTrace = m_pPdbNode->getTrace(iToSkipCACount);
    oTrace.deleteAllBonds();
    m_aCodebook = oTrace;

    // render the codebook vectors
    updateCodebookRendering();
    resetCodebookID();
}

/**
 * atomic quantization (no dialog, every CA)
 */
void pdb_document::atomicQuantizationEveryCA(svt_vector4f&)
{

    m_aCodebook.deleteAllAtoms();
    m_aCodebook.deleteAllBonds();

    svt_point_cloud_pdb<svt_vector4 <Real64> > oTrace = m_pPdbNode->getTrace();
    oTrace.deleteAllBonds();
    m_aCodebook = oTrace;

    svtout << m_aCodebook.size() << " vectors created" << endl;

    // render the codebook vectors
    updateCodebookRendering();

}

/**
 * generates steric clashing codebook based on CAs
 */
void pdb_document::generateStericClashingCodebook()
{
    const unsigned int iMaxStericCVs = 1000;

    // delete the old codebook vectors
    m_aCodebookSC.deleteAllAtoms();
    m_aCodebookSC.deleteAllBonds();

    svt_point_cloud_pdb<svt_vector4 <Real64> > oTrace = m_pPdbNode->getTrace();
    oTrace.deleteAllBonds();

    svtout << "Generating a steric clashing codebook based on atom trace:" << endl;

    // Determine if we have too many CVs
    if (oTrace.size() > iMaxStericCVs)
    {
        int iSkip = (int) ceilf((float) oTrace.size() / (float) iMaxStericCVs) - 1;
        svtout << "    " << "Skipping every " << iSkip << " residue(s)" << endl;
        oTrace = m_pPdbNode->getTrace(iSkip);
    }

    m_aCodebookSC = oTrace;

    svtout << "    " << m_aCodebookSC.size() << " steric clashing vectors created" << endl;

    updateCodebookSCRendering();

}

/**
 * Show the Extract from PDB dialog
 */
void pdb_document::showExtractDlg()
{
    if ( m_pExtractDlg == NULL )
        m_pExtractDlg = new dlg_extract(g_pWindow, m_pPdbNode, this, QString(getNameShort().c_str()), g_pWindow);

    m_pExtractDlg->updateExtractDlg();
    m_pExtractDlg->show();
}

/**
 * Show the symmetry dialog
 */
void pdb_document::showSymmetryDlg()
{
    if (m_pSymmetryDlg==NULL)
        m_pSymmetryDlg = new dlg_symmetry(g_pWindow, this);

    // init contents
    m_pSymmetryDlg->init();

    // show dialog
    m_pSymmetryDlg->show();
}

/**
 * create multiple copies of the pdb using symmetry
 */
void pdb_document::applySymmetry(svt_vector3<Real64> oPoint1, svt_vector3<Real64> oPoint2, int iOrder)
{
    // add pdb files
    svt_vector4<Real64> oCenter, oPoint, oRotAxis;

    oCenter.x( oPoint1.x() );
    oCenter.y( oPoint1.y() );
    oCenter.z( oPoint1.z() );

    oPoint.x( oPoint2.x() );
    oPoint.y( oPoint2.y() );
    oPoint.z( oPoint2.z() );


    oRotAxis = oPoint - oCenter;

    svtout << "Axis:" << oRotAxis << endl;

    svt_point_cloud_pdb <svt_vector4<Real64> > oPDBMono, oPDBOlig;

    // adjust coordinates
    svt_vector4<Real64> oVec;
    svt_matrix4<Real64> oMat( *(m_pPdbNode->getTransformation()) ); oMat[0][3] *= 1.0E2; oMat[1][3] *= 1.0E2; oMat[2][3] *= 1.0E2;
    
    for( unsigned int i=0; i<m_pPdbNode->size(); i++)
    {
        oVec = oMat * (*m_pPdbNode)[i];
        oPDBMono.addAtom( *m_pPdbNode->getAtom(i), oVec);
    }
    svt_vector4<Real64> oPdbCenter = oPDBMono.coa();

    oMat.loadIdentity();
    oMat.translate(-oCenter);
    oPDBMono = oMat*oPDBMono;

    //prepare to rotate around the axis
    oMat.loadIdentity();
    oMat.rotate(2,atan2(oRotAxis.x(),oRotAxis.y()) );
    oPDBMono = oMat*oPDBMono;
    oPoint = oMat*oRotAxis;
    oMat.loadIdentity();
    oMat.rotate(0,atan2(oPoint.y(), oPoint.z()) );
    oPDBMono = oMat*oPDBMono;

    oPDBOlig = oPDBMono.applySymmetry( iOrder, SYMMETRY_C,'z', 0, 0);

    oMat.loadIdentity();
    oMat.rotate(0,-atan2(oPoint.y(), oPoint.z()) );
    oPDBOlig = oMat*oPDBOlig;

    oMat.loadIdentity();
    oMat.rotate(2,-atan2(oRotAxis.x(),oRotAxis.y()) );
    oPDBOlig = oMat*oPDBOlig;


    oMat.loadIdentity();
    oMat.translate(oCenter);
    oPDBOlig = oMat*oPDBOlig;

    pdb_document* pDoc = new pdb_document(g_pWindow->getScene()->getDataTopNode(), "", 0, "", 0, &oPDBOlig);

    svt_vector4f oDocOrigin = pDoc->getRealPos();
    oDocOrigin.multScalar( 1.0E8f );
    pDoc->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
    pDoc->storeInitMat();
    pDoc->setChanged();

    // add it to the document lists
    g_pWindow->addDoc( (sculptor_document*)pDoc, QString("new_pdb_document.pdb") ); 
    svtout << "Document \"" << pDoc->getNameShort() <<  "\" has been created to hold the new structure." << endl;

}

/**
 * Return the special svt_sampled object for clustering
 */
svt_sampled< svt_vector4<Real64> >& pdb_document::getSampled()
{
    return *m_pPdbNode;
};

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void pdb_document::setState(TiXmlElement* pElem)
{
    sculptor_document::setState(pElem);

    int i; // used also as bool
    double d;
    TiXmlElement* pChild;
    int j;

    if ((pChild = (TiXmlElement*) pElem->FirstChild( "CENTER" )) != NULL)
    {
        double fCenterX = 0.0;
        double fCenterY = 0.0;
        double fCenterZ = 0.0;

        if (pChild->Attribute("X", &d)) fCenterX = d;
        if (pChild->Attribute("Y", &d)) fCenterY = d;
        if (pChild->Attribute("Z", &d)) fCenterZ = d;

        if (   (fabs(fCenterX-m_oCenter.x()) > 0.0001)
               && (fabs(fCenterY-m_oCenter.y()) > 0.0001)
               && (fabs(fCenterZ-m_oCenter.z()) > 0.0001) )
        {
            // need to undo centering that constructor performed and use center from file

            // translate structure to new center
            svt_matrix4 <Real64> oTranslation;
            oTranslation.loadIdentity();
            oTranslation.setTranslation(m_oCenter.x() - fCenterX,
                                        m_oCenter.y() - fCenterY,
                                        m_oCenter.z() - fCenterZ);

            // save new center
            m_oCenter.x( fCenterX );
            m_oCenter.y( fCenterY );
            m_oCenter.z( fCenterZ );

            // move atoms
            int iCurrentTimestep = m_pPdbNode->getTimestep();
            for(int iTS=0; iTS<m_pPdbNode->getMaxTimestep(); iTS++)
            {
                m_pPdbNode->setTimestep( iTS );

                for( unsigned int i=0; i<m_pPdbNode->size(); i++)
                {
                    (*m_pPdbNode)[i] = oTranslation * (*m_pPdbNode)[i];
                }
            }
            m_pPdbNode->setTimestep( iCurrentTimestep );

            // install new transformation
            m_pPdbNode->setPos( new svt_pos(
                                    (1.0E-2) * m_oCenter.x(),
                                    (1.0E-2) * m_oCenter.y(),
                                    (1.0E-2) * m_oCenter.z()
                                    ) );
            // save transformation
            storeInitMat();
        }
    }
    else
    {
        // Old scene file w/o any center information
        // Was generated by old centering code ( (max-min)/2 )

        // need to undo centering that constructor performed and use geometric center

        // translate structure to new center
        svt_matrix4 <Real64> oTranslation;
        oTranslation.loadIdentity();
        oTranslation.setTranslation(m_oCenter - m_pPdbNode->geometricCenter());

        // save new center
        m_oCenter = m_pPdbNode->geometricCenter();

        // move atoms
        int iCurrentTimestep = m_pPdbNode->getTimestep();
        for(int iTS=0; iTS<m_pPdbNode->getMaxTimestep(); iTS++)
        {
            m_pPdbNode->setTimestep( iTS );

            for( unsigned int i=0; i<m_pPdbNode->size(); i++)
            {
                (*m_pPdbNode)[i] = oTranslation * (*m_pPdbNode)[i];
            }
        }
        m_pPdbNode->setTimestep( iCurrentTimestep );

        // install new transformation
        m_pPdbNode->setPos( new svt_pos(
                                (1.0E-2) * m_oCenter.x(),
                                (1.0E-2) * m_oCenter.y(),
                                (1.0E-2) * m_oCenter.z()
                                ) );
        // save transformation
        storeInitMat();
    }

    if (pElem->Attribute("MODES", &i))
    {
        int iModes = i;
        char pStr[256];

        if (iModes > 0)
            m_aModes.delAllElements();

        for(j=0; j<iModes; j++)
        {
            sprintf( pStr, "MODE%i", j );

            int iSelectionMode  = MODE_ALL;
            int iSelectionName  = 0;
            int iGraphicsMode   = GM_LINE;
            int iColmapType     = COLMAP_TYPE;
            int iSelTo          = 0;
            float fSphereScale  = 0.0;
            float fProbeRadius  = 0.0;
//          float fTubeDiameter = 0.0;
//          int iTubeSegments   = 0;
//             int iTubeCorners    = 0;
            int iLabelMode      = LABEL_OFF;

            if ((pChild = (TiXmlElement*) pElem->FirstChild( pStr )) != NULL)
            {
                if (pChild->Attribute("SELECTIONMODE", &i))
                    iSelectionMode = i;
                if (pChild->Attribute("SELECTIONNAME", &i))
                    iSelectionName = i;
                if (pChild->Attribute("GRAPHICSMODE", &i))
                    iGraphicsMode = i;
                if (pChild->Attribute("COLMAPTYPE", &i))
                    iColmapType = i;
                if (pChild->Attribute("SELECTIONTO", &i))
                    iSelTo = i;
                if (pChild->Attribute("SPHERESCALE", &d))
                    fSphereScale = d;
                if (pChild->Attribute("PROBERADIUS", &d))
                    fProbeRadius = d;
//              if (pChild->Attribute("TUBEDIAMETER", &d))
//                  fTubeDiameter = d;
//              if (pChild->Attribute("TUBESEGMENTS", &i))
//                  iTubeSegments = i;
//              if (pChild->Attribute("TUBEPROFILECORNERS", &i))
//                  iTubeCorners = i;
                if (pChild->Attribute("LABELMODE", &i))
                    iLabelMode = i;
            }

            pdb_mode oMode( iSelectionMode, iSelectionName, iGraphicsMode, iColmapType, iLabelMode );
            if (iSelTo != 0)
                oMode.setSelectionTo( iSelTo );
            if (fSphereScale != 0.0)
                oMode.setSphereScale( fSphereScale );
            if (fProbeRadius != 0.0)
                oMode.setProbeRadius( fProbeRadius );
//          if (fTubeDiameter != 0.0)
//                 oMode.setTubeDiameter( fTubeDiameter );
//          if (iTubeSegments != 0.0)
//                 oMode.setTubeSegments( iTubeSegments );
//              m_pPdbNode->setNewTubeSegs( iTubeSegments );
//          if (iTubeCorners != 0.0)
//                 oMode.setTubeProfileCorners( iTubeCorners );
//              m_pPdbNode->setNewTubeProfileCorners( iTubeCorners );

            m_aModes.addElement( oMode );
        }

        // automatically update
        m_pPdbNode->getNodeLock()->P();
        m_pPdbNode->setDisplayMode(SVT_PDB_OFF);
        m_pPdbNode->blockWidget( m_pPdbPropDlg );

        for(i=0;i<iModes;i++)
            applyPdbMode(m_aModes[i]);

        m_pPdbNode->rebuildDL();
        m_pPdbNode->getNodeLock()->V();
    }

    if (pElem->Attribute("AUTOAPPLY", &i ))
        m_bAutoApply = (bool)i;

    if (pElem->Attribute("SECSTRUCT_CUTOFF", &d))       m_pPdbNode->setTraceCutoff(d);

    if (pElem->Attribute("TUBE_DIAMETER", &d))          m_pPdbNode->setTubeDiameter(d);
    if (pElem->Attribute("TUBE_SEGMENTS", &i))          m_pPdbNode->setTubeSegs (i);

    if (pElem->Attribute("NEWTUBE_DIAMETER", &d))       m_pPdbNode->setNewTubeDiameter(d);
    if (pElem->Attribute("NEWTUBE_SEGMENTS", &i))       m_pPdbNode->setNewTubeSegs (i);
    if (pElem->Attribute("NEWTUBE_PROFILECORNERS", &i)) m_pPdbNode->setNewTubeProfileCorners(i);

    if (pElem->Attribute("CARTOON_USE_SHADER", &i))                   m_pPdbNode->setCartoonParam(CARTOON_USE_SHADER, (Real32)i);
    if (pElem->Attribute("CARTOON_SHEET_HELIX_PROFILE_CORNERS", &i))  m_pPdbNode->setCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS, (Real32)i);
    if (pElem->Attribute("CARTOON_PER_PIXEL_LIGHTING", &i))           m_pPdbNode->setCartoonParam(CARTOON_PER_PIXEL_LIGHTING, (Real32)i);
    if (pElem->Attribute("CARTOON_PEPTIDE_PLANES", &i))               m_pPdbNode->setCartoonParam(CARTOON_PEPTIDE_PLANES, (Real32)i);
    if (pElem->Attribute("CARTOON_SHEET_MIN_RESIDUES", &i)) {
        m_pPdbNode->setCartoonParam(CARTOON_SHEET_MIN_RESIDUES, (Real32)i);
        getAtomStructureColmap()->setMinResiduesPerSheet(i);}
    if (pElem->Attribute("CARTOON_SHEET_WIDTH", &d))                  m_pPdbNode->setCartoonParam(CARTOON_SHEET_WIDTH,  d);
    if (pElem->Attribute("CARTOON_SHEET_HEIGHT", &d))                 m_pPdbNode->setCartoonParam(CARTOON_SHEET_HEIGHT, d);
    if (pElem->Attribute("CARTOON_HELIX_MIN_RESIDUES", &i)) {
        m_pPdbNode->setCartoonParam(CARTOON_HELIX_MIN_RESIDUES, (Real32)i);
        getAtomStructureColmap()->setMinResiduesPerHelix(i);}
    if (pElem->Attribute("CARTOON_HELIX_WIDTH", &d))                  m_pPdbNode->setCartoonParam(CARTOON_HELIX_WIDTH, d);
    if (pElem->Attribute("CARTOON_HELIX_HEIGHT", &d))                 m_pPdbNode->setCartoonParam(CARTOON_HELIX_HEIGHT, d);
    if (pElem->Attribute("CARTOON_TUBE_DIAMETER", &d))                m_pPdbNode->setCartoonParam(CARTOON_TUBE_DIAMETER, d);
    if (pElem->Attribute("CARTOON_HELIX_CYLINDER", &i))               m_pPdbNode->setCartoonParam(CARTOON_HELIX_CYLINDER, (Real32)i);
    if (pElem->Attribute("CARTOON_HELIX_ARROWHEADS", &i))             m_pPdbNode->setCartoonParam(CARTOON_HELIX_ARROWHEADS, (Real32)i);
    if (pElem->Attribute("CARTOON_SPLINE_SEGMENTS", &i))              m_pPdbNode->setCartoonParam(CARTOON_SPLINE_SEGMENTS, (Real32)i);

    if (pElem->Attribute("VDW_RENDERING_MODE", &i))                   m_pPdbNode->setVDWRenderingMode( (int) i );

    m_cTypeColmap.setState( pElem );
    m_cResnameColmap.setState( pElem );
    m_cChainIDColmap.setState( pElem );
    m_cSegIDColmap.setState( pElem );
    m_cSolidColmap.setState( pElem );
    m_cStructureColmap.setState( pElem );
    m_cModelColmap.setState( pElem );
    m_cBetaColmap.setState( pElem );
}

/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void pdb_document::getState(TiXmlElement* pElem)
{
    sculptor_document::getState(pElem);

    pElem->SetAttribute("MODES", m_aModes.numElements());

    char pStr[256];
    int j;

    TiXmlElement* pCenter = new TiXmlElement( "CENTER" );
    pElem->LinkEndChild(pCenter);
    pCenter->SetDoubleAttribute("X", m_oCenter.x() );
    pCenter->SetDoubleAttribute("Y", m_oCenter.y() );
    pCenter->SetDoubleAttribute("Z", m_oCenter.z() );

    for(j=0; j<m_aModes.numElements(); j++)
    {
        sprintf( pStr, "MODE%i", j );

        TiXmlElement* pMode = new TiXmlElement( pStr );
        pElem->LinkEndChild(pMode);

        pMode->SetAttribute("SELECTIONMODE", m_aModes[j].getSelectionMode() );
        pMode->SetAttribute("SELECTIONNAME", m_aModes[j].getSelectionName() );
        pMode->SetAttribute("GRAPHICSMODE",  m_aModes[j].getGraphicsMode()  );
        pMode->SetAttribute("COLMAPTYPE",    m_aModes[j].getColmapType()    );
        pMode->SetAttribute("SELECTIONTO",   m_aModes[j].getSelectionTo()   );
        pMode->SetDoubleAttribute("SPHERESCALE",   m_aModes[j].getSphereScale()   );
        pMode->SetDoubleAttribute("PROBERADIUS",   m_aModes[j].getProbeRadius()   );
        pMode->SetAttribute("LABELMODE",     m_aModes[j].getLabelMode() );
    }

    pElem->SetAttribute("AUTOAPPLY", m_bAutoApply );

    pElem->SetDoubleAttribute("SECSTRUCT_CUTOFF",    m_pPdbNode->getTraceCutoff() );

    pElem->SetDoubleAttribute("TUBE_DIAMETER",  m_pPdbNode->getTubeDiameter() );
    pElem->SetAttribute      ("TUBE_SEGMENTS",  m_pPdbNode->getTubeSegs() );

    pElem->SetDoubleAttribute("NEWTUBE_DIAMETER",       m_pPdbNode->getNewTubeDiameter() );
    pElem->SetAttribute("NEWTUBE_SEGMENTS",        (int)m_pPdbNode->getNewTubeSegs() );
    pElem->SetAttribute("NEWTUBE_PROFILECORNERS",  (int)m_pPdbNode->getNewTubeProfileCorners() );

    pElem->SetAttribute("CARTOON_USE_SHADER",                  (int)m_pPdbNode->getCartoonParam(CARTOON_USE_SHADER) );
    pElem->SetAttribute("CARTOON_SHEET_HELIX_PROFILE_CORNERS", (int)m_pPdbNode->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) );
    pElem->SetAttribute("CARTOON_PER_PIXEL_LIGHTING",          (int)m_pPdbNode->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING) );
    pElem->SetAttribute("CARTOON_PEPTIDE_PLANES",              (int)m_pPdbNode->getCartoonParam(CARTOON_PEPTIDE_PLANES) );
    pElem->SetAttribute("CARTOON_SHEET_MIN_RESIDUES",          (int)m_pPdbNode->getCartoonParam(CARTOON_SHEET_MIN_RESIDUES) );
    pElem->SetDoubleAttribute("CARTOON_SHEET_WIDTH",                m_pPdbNode->getCartoonParam(CARTOON_SHEET_WIDTH) );
    pElem->SetDoubleAttribute("CARTOON_SHEET_HEIGHT",               m_pPdbNode->getCartoonParam(CARTOON_SHEET_HEIGHT) );
    pElem->SetAttribute("CARTOON_HELIX_MIN_RESIDUES",          (int)m_pPdbNode->getCartoonParam(CARTOON_HELIX_MIN_RESIDUES) );
    pElem->SetDoubleAttribute("CARTOON_HELIX_WIDTH",                m_pPdbNode->getCartoonParam(CARTOON_HELIX_WIDTH) );
    pElem->SetDoubleAttribute("CARTOON_HELIX_HEIGHT",               m_pPdbNode->getCartoonParam(CARTOON_HELIX_HEIGHT) );
    pElem->SetDoubleAttribute("CARTOON_TUBE_DIAMETER",              m_pPdbNode->getCartoonParam(CARTOON_TUBE_DIAMETER) );
    pElem->SetAttribute("CARTOON_HELIX_CYLINDER",              (int)m_pPdbNode->getCartoonParam(CARTOON_HELIX_CYLINDER) );
    pElem->SetAttribute("CARTOON_HELIX_ARROWHEADS",            (int)m_pPdbNode->getCartoonParam(CARTOON_HELIX_ARROWHEADS) );
    pElem->SetAttribute("CARTOON_SPLINE_SEGMENTS",             (int)m_pPdbNode->getCartoonParam(CARTOON_SPLINE_SEGMENTS) );

    pElem->SetAttribute("VDW_RENDERING_MODE", (int)m_pPdbNode->getVDWRenderingMode() );

    m_cTypeColmap.getState( pElem );
    m_cResnameColmap.getState( pElem );
    m_cChainIDColmap.getState( pElem );
    m_cSegIDColmap.getState( pElem );
    m_cSolidColmap.getState( pElem );
    m_cStructureColmap.getState( pElem );
    m_cModelColmap.getState( pElem );
    m_cBetaColmap.getState( pElem );
}

///////////////////////////////////////////////////////////////////////////////
// global rendering mode changes
///////////////////////////////////////////////////////////////////////////////

/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderMode( int iMode )
{
    setGlobalMode( iMode );

    if ( m_pPdbPropDlg != NULL )
    {
        rebuildModeList();
        m_pPdbPropDlg->sUpdatePdbModeButtons();
    }

}

/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeOFF( )
{
    changeRenderMode( GM_OFF );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModePOINT( )
{
    changeRenderMode( GM_POINT );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeLINE( )
{
    changeRenderMode( GM_LINE );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeVDW( )
{
    changeRenderMode( GM_VDW );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeCPK( )
{
    changeRenderMode( GM_CPK );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeDOTTED( )
{
    changeRenderMode( GM_DOTTED );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeLICORICE( )
{
    changeRenderMode( GM_LICORIZE );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeTUBE( )
{
    changeRenderMode( GM_NEWTUBE );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeCARTOON( )
{
    changeRenderMode( GM_CARTOON );
}
/**
 * Select a global graphics mode (for pdb documents only)
 */
void pdb_document::changeRenderModeSURF( )
{
    changeRenderMode( GM_SURF );
}
// /**
//  * Select a global graphics mode (for pdb documents only)
//  */
// void pdb_document::changeRenderModeTOON( )
// {
//     changeRenderMode( GM_TOON );
// }

/**
 * Select a global coloring mode (for pdb documents only)
 * \param iMode the mode
 */
void pdb_document::changeColmapMode( int iMode )
{
    if (g_pWindow == NULL)
        return;

    vector<sculptor_document*> aDocs = g_pWindow->getDocumentList()->getDocuments();
    bool bNonChanged = true;

    for (unsigned int i=1; i<aDocs.size(); i++)
    {
        if (aDocs[i]->getSelected() && aDocs[i]->getType() == SENSITUS_DOC_PDB)
        {
            pdb_document* pPDB = (pdb_document*)(aDocs[i]);

            pPDB->setGlobalColmap( iMode, i );

            bNonChanged = false;
        }
    }

    if ( bNonChanged )
    {
        for(unsigned int i=1;i < aDocs.size(); i++)
        {
            if (aDocs[i]->getType() == SENSITUS_DOC_PDB)
            {
                pdb_document* pPDB = (pdb_document*)(aDocs[i]);

                pPDB->setGlobalColmap( iMode, i );
            }
        }
    }
};

/**
 * Select a global coloring mode (for pdb documents only)
 */
void pdb_document::changeColmapModeSOLID()
{
    if (g_pWindow == NULL) return;

    vector<sculptor_document*> aDocs = g_pWindow->getDocumentList()->getDocuments();

    unsigned int i;
    int iCount = 0;
    int iPDBCount = 0;
    int iColor = -1;
    pdb_document* pPDB = NULL;

    for(i=1; i<aDocs.size(); i++)
    {
        if (aDocs[i]->getSelected() && aDocs[i]->getType() == SENSITUS_DOC_PDB)
        {
            pPDB = (pdb_document*)(aDocs[i]);
            iCount++;
        }
        if (aDocs[i]->getType() == SENSITUS_DOC_PDB)
        {
            if (iCount == 0)
                pPDB = (pdb_document*)(aDocs[i]);

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
        for(i=1; i<aDocs.size(); i++)
        {
            if (aDocs[i]->getSelected() && aDocs[i]->getType() == SENSITUS_DOC_PDB)
            {
                pdb_document* pPDB = (pdb_document*)(aDocs[i]);

                pPDB->setGlobalColmap( COLMAP_SOLID, (unsigned int)(iColor) );
            }
        }
    else if (iCount == 1 || iPDBCount == 1)
        pPDB->setGlobalColmapSOLIDINTERACTIVE();

    else {

        for(i=1; i<aDocs.size(); i++)
        {
            if (aDocs[i]->getType() == SENSITUS_DOC_PDB)
            {
                pdb_document* pPDB = (pdb_document*)(aDocs[i]);

                pPDB->setGlobalColmap( COLMAP_SOLID, i );
            }
        }
    }
    if (m_pDLItem != NULL)
        m_pDLItem->showColor();
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void pdb_document::changeColmapModeTYPE()
{
    changeColmapMode( COLMAP_TYPE );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void pdb_document::changeColmapModeRESNAME()
{
    changeColmapMode( COLMAP_RESNAME );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void pdb_document::changeColmapModeCHAINID()
{
    changeColmapMode( COLMAP_CHAINID );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void pdb_document::changeColmapModeSEGID()
{
    changeColmapMode( COLMAP_SEGID );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void pdb_document::changeColmapModeSTRUCTURE()
{
    changeColmapMode( COLMAP_STRUCTURE );
}

/**
 * Select a global coloring mode (for pdb documents only)
 */
void pdb_document::changeColmapModeMODEL()
{
    changeColmapMode( COLMAP_MODEL );
}

///////////////////////////////////////////////////////////////////////////////
// Transformation/Docking Tab in Prop Dialog
///////////////////////////////////////////////////////////////////////////////

/**
 * Transformation/Docking Tab: Add Transform
 */
void pdb_document::sTransAdd()
{
    if (m_pPdbPropDlg != NULL)
    {
        char pName[256];

        sprintf( pName, "%s %i", getNameShort().c_str(), m_pPdbPropDlg->getTransformCount() );

        if ( addTransform(pName) )
            m_pPdbPropDlg->transformAdd( pName );
    }
};

/**
 * Transformation/Docking Tab: Remove Transform
 */
void pdb_document::sTransDel()
{
    if (m_pPdbPropDlg != NULL)
    {
        if (m_pPdbPropDlg->getTransformCount() == 0)
            return;

        unsigned int iIndex = m_pPdbPropDlg->getCurrentTransform();

        removeTransform( iIndex );
        m_pPdbPropDlg->transformRemove( iIndex );
    }
}

/**
 * Transformation/Docking Tab: Transform up in list
 */
void pdb_document::sTransUp()
{
    if (m_pPdbPropDlg != NULL && m_pPdbPropDlg->getCurrentTransform() > 0)
    {
        int iIndex = m_pPdbPropDlg->getCurrentTransform();

        m_pPdbPropDlg->transformUp( iIndex );
        transformUp( iIndex );
    }
}

/**
 * Transformation/Docking Tab: Transform down in list
 */
void pdb_document::sTransDown()
{
    if (m_pPdbPropDlg != NULL && m_pPdbPropDlg->getCurrentTransform() < (int)(m_pPdbPropDlg->getTransformCount())-1)
    {
        int iIndex = m_pPdbPropDlg->getCurrentTransform();

        m_pPdbPropDlg->transformUp( iIndex );
        transformDown( iIndex );
    }
}

/**
 * Transformation/Docking Tab: Transform rename in list
 */
void pdb_document::sTransRename()
{
    if (m_pPdbPropDlg != NULL)
    {
        bool ok;
        int iIndex = m_pPdbPropDlg->getCurrentTransform();
        QString oAlt = m_pPdbPropDlg->getTransformText( iIndex );
        QString oText = QInputDialog::getText( g_pWindow, "Rename Transformation", "Enter new name:", QLineEdit::Normal, oAlt, &ok );
        if ( ok && !oText.isEmpty() )
        {
            setTransformName( iIndex, QtoStd(oText).c_str() );
            m_pPdbPropDlg->setTransformText( iIndex, QtoStd(oText).c_str() );
        }
    }
}

/**
 * Transformation/Docking Tab: Clear transform list
 */
void pdb_document::sTransClear()
{
    if (m_pPdbPropDlg != NULL)
    {
        removeAllTransforms();
        m_pPdbPropDlg->transformsClear();
    }
}

/**
 * Transformation/Docking Tab: Apply current transform
 */
void pdb_document::sTransApply(QListWidgetItem*)
{
    if (m_pPdbPropDlg != NULL)
    {
        int iIndex = m_pPdbPropDlg->getCurrentTransform();
        if ( iIndex != -1 )
            applyTransform( iIndex );
    }
}

/**
 * Show transformation list
 */
void pdb_document::showTransList()
{
    if (m_pPdbPropDlg == NULL)
        return;

    updateTransList();

    // show the dialog
    m_pPdbPropDlg->showDockingTab();
}


/**
 * Update transformation list.
 */
void pdb_document::updateTransList()
{
    if (m_pPdbPropDlg == NULL)
        return;

    // fill in all the current transformations
    m_pPdbPropDlg->transformsClear();
    for( unsigned int i=0; i<getTransformCount(); i++)
        m_pPdbPropDlg->transformAdd( getTransformName(i) );
}

/**
 * adds the PDF of the current pdb document to the
 *\param sculptor_plot_dlg dialog
 *\return status = was curve added 1 or user cancel calculation 0
 */
int pdb_document::addPdf2Plot(dlg_plot* pDlg, svt_color oColor)
{
    QString oName;

    svt_pair_distribution oPDF;

    //compute PDF only for the first frame
    getPointCloud().setTimestep(0);
    oPDF = getPointCloud().getPairDistribution();
    if (oPDF.size()<=1)
    {
        getPointCloud().calcPairDistribution();
        oPDF = getPointCloud().getPairDistribution();
        if (oPDF.size()>0) // cause user has push cancel button
        {
            getPointCloud().normalizePairDistribution();
            oPDF = getPointCloud().getPairDistribution();
        }
    }

    if (oPDF.size()>0) // cause user has push cancel button
    {
        oName.sprintf("%s ", getDisplayName().c_str() );

        sculptor_qwtplotcurve* pCurve = new sculptor_qwtplotcurve(oName, oPDF.getBins(), oPDF.getPdf(), oPDF.size());
        pCurve->setStyle(QwtPlotCurve::Steps);
        pCurve->setColor(int(oColor.getR()*255.0f), int(oColor.getG()*255.0f), int(oColor.getB()*255.0f) );

        pDlg->attachCurve(pCurve);;
    }
    else
    {
        return 0;
    }

    return 1;
};

/**
 * Set center sphere visibility
 * \param bVisible true if the sphere is visible
 */
void pdb_document::setCenterSphereVisible( bool bVisible )
{
    m_pCenterSphere->setVisible( bVisible );
};

/**
 * Set the radius/scaling of the center sphere
 * \param fRadius the new radius (1.0 is default)
 */
void pdb_document::setCenterSphereRadius( float fRadius )
{
    m_pCenterSphere->setRadius( fRadius );
    m_pCenterSphereHighlight->setRadius ( fRadius+0.001f );
};

/**
 * Set center sphere highlight visibility
 * \param bVisible true if highlight is visible
 */
void pdb_document::setCenterSphereHighlightVisible( bool bVisible )
{
    m_pCenterSphereHighlight->setVisible( bVisible );
};

/**
 * Set the color of the center sphere
 * \param fR red component
 * \param fG red component
 * \param fB red component
 */
void pdb_document::setCenterSphereColor( float fR, float fG, float fB )
{
    m_pCenterSphere->getProperties()->getColor()->set( fR, fG, fB );
};

/**
 * Set center sphere highlight color
 * \param fR red component
 * \param fG red component
 * \param fB red component
 */
void pdb_document::setCenterSphereHighlightColor( float fR, float fG, float fB )
{
    m_pCenterSphereHighlight->getProperties()->getWireframeColor()->set( fR, fG, fB );
};

/**
 * Set center text visibility
 * \param bVisible of true the text is visible
 */
void pdb_document::setCenterTextVisible( bool bVisible )
{
    m_pCenterText->setVisible( bVisible );
};

/**
 * Get center text visibility
 * \return true if the text is visible
 */
bool pdb_document::getCenterTextVisible()
{
    return m_pCenterText->getVisible();
};

/**
 * Set the text shown above the center of the structure
 * \param char* pText
 */
void pdb_document::setCenterText( char* pText)
{
    m_pCenterText->setText( pText );
};

/**
 * Get force arrow visibility
 * \return true the force arrow is visible
 */
bool pdb_document::getForceArrowVisible()
{
    return m_pForceArrow->getVisible();
}

/**
 * Set force arrow visibility
 * \param bVisible true if the force arrow is visible
 */
void pdb_document::setForceArrowVisible( bool bVisible )
{
    m_pForceArrow->setVisible( bVisible );
}

/**
 * Get a pointer to the force arrow
 * \return svt_arrow pointer
 */
svt_arrow* pdb_document::getForceArrow()
{
    return m_pForceArrow;
}

/**
 * Get force arrow transformation matrix
 * \return matrix4f* pTransformation
 */
svt_matrix4f* pdb_document::getForceArrowTransformation()
{
    return m_pForceArrow->getTransformation();
}

/**
 * Set force arrow transformation matrix
 * \param matrix4f* pTransformation
 */
void pdb_document::setForceArrowTransformation(svt_matrix4f* pTransformation)
{
    m_pForceArrow->setTransformation(pTransformation);
}

/**
 * get the mode stack
 * \return reference to the mode stack
 */
svt_array<pdb_mode>& pdb_document::getModes()
{
    return m_aModes;
}

/**
 * Get the size of the document
 * \return svt_vector4 with the three dimensions of the size of the document
 */
svt_vector4<Real32> pdb_document::getBoundingBox()
{
    Real32 fSizeX = (m_pPdbNode->getMaxXCoord() - m_pPdbNode->getMinXCoord()) * 0.5f * 1.0E-2f;
    Real32 fSizeY = (m_pPdbNode->getMaxYCoord() - m_pPdbNode->getMinYCoord()) * 0.5f * 1.0E-2f;
    Real32 fSizeZ = (m_pPdbNode->getMaxZCoord() - m_pPdbNode->getMinZCoord()) * 0.5f * 1.0E-2f;

    return svt_vector4<Real32>( fSizeX, fSizeY, fSizeZ );
};

/**
 * Return a pointer to the svt_atom_structure_colmap
 * \return &cStructureColmap
 */
svt_atom_structure_colmap * pdb_document::getAtomStructureColmap()
{
    return &m_cStructureColmap;
};

/**
 * Returns the center of the structure
 */
void pdb_document::setCenter( svt_vector4<Real64> oCenter)
{
    m_oCenter = oCenter;
};

/**
 * Returns the center of the structure
 */
svt_matrix4< Real64 > pdb_document::getCenter()
{
    return m_oCenter;
}

/**
 *
 */
void pdb_document::updatePropDlgDynTPS(int iCurrent)
{
    if (m_pPdbPropDlg != NULL)
        m_pPdbPropDlg->updateDynTPS(iCurrent);
}

/**
 *
 */
void pdb_document::updatePropDlgDynTimestep(int iCurrent, int iMaxTimestep)
{
    if (m_pPdbPropDlg != NULL)
        m_pPdbPropDlg->updateDynTimestep(iCurrent, iMaxTimestep-1);
}

/**
 *
 */
void pdb_document::updatePropDlgDynLoop(int iLoop)
{
    if (m_pPdbPropDlg != NULL)
        m_pPdbPropDlg->updateDynLoop(iLoop);
}

/**
 *
 */
void pdb_document::updatePropDlgDynButtons(int iPlaying)
{
    if (m_pPdbPropDlg != NULL)
        m_pPdbPropDlg->updateDynButtons(iPlaying);
}

/**
 *
 */
void pdb_document::updatePropDlgDynMovieButton(bool bActive)
{
    if (m_pPdbPropDlg != NULL)
        m_pPdbPropDlg->updateDynMovieButton(bActive);
}

/**
 *
 */
void pdb_document::setAllowShaderPrograms()
{
    // disable the shader elements in the dialogs
    if (m_pCartoonDlg != NULL)
        m_pCartoonDlg->updateUseShader();
    if (m_pNewTubeDlg != NULL)
        m_pNewTubeDlg->updateUseShader();
    if (m_pVDWDlg != NULL)
        m_pVDWDlg->updateUseShader();

    svt_forceUpdateGUIWindows();

    // let the svt_pdb use shaders or not
    ((svt_pdb*)getNode())->setAllowShaderPrograms();
}
