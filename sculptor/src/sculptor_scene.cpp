/***************************************************************************
                          sculptor_scene.cpp
                          -------------------
    begin                : 27.03.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// svt includes
#include <svt_properties.h>
#include <svt_const.h>
#include <svt_color.h>
#include <svt_init.h>
#include <svt_system.h>
#include <svt_pipette.h>
#include <svt_sphere.h>
#include <svt_fstream.h>
#include <svt_coord_system.h>
#include <svt_scenegraph.h>
// sculptor includes
#include <sculptor_scene.h>
#include <sculptor_window.h>
#include <sculptor_force_algo.h>
#include <sculptor_window_documentlist.h>


extern sculptor_window* g_pWindow;
extern string QtoStd(QString);


/**
 * Constructor
 */
sculptor_scene::sculptor_scene()
    : svt_scene(),
      m_pPipette( NULL ),
      m_fMWScale( 0.1 ),
      m_bControl( false )
{
};

/**
 * build the scene
 */
void sculptor_scene::buildScene()
{
    // construct a scenegraph
    m_pGraph = new svt_scenegraph();

    // create force tool
    m_pForceTool = new svt_adv_force_tool("Feedback");
    m_pForceTool->setName("(sculptor_scene)m_pForceTool");
    m_pForceTool->setCenterMode(true);
    svt_matrix4f* pMat = new svt_matrix4f("Top Level Matrix");
    m_pForceTool->setGlobalMatrix(pMat);
    m_pForceTool->setActive(false);
    m_pGraph->add(m_pForceTool);

    // create move tool
    m_pMoveTool = new svt_move_tool("Pointer");
    m_pMoveTool->setName( "(sculptor_scene)m_pMoveTool" );
    m_pMoveTool->setCenterMode(true);
    m_pPipette = new svt_pipette();
    svt_properties* pWhite = new svt_properties(new svt_color(1.0f,1.0f,1.0f));
    m_pPipette->setProperties( pWhite );
    m_pPipette->setName("(sculptor_scene)m_pPipette");
    m_pMoveTool->add( m_pPipette );
    m_pPipette->setVisible( false );
    m_pMoveTool->setMoveNode(NULL);
    m_pMoveTool->setGlobalMatrix(pMat);
    m_pForceTool->setActive(true);
    m_pGraph->setIsFogNode(true);
    m_pGraph->add(m_pMoveTool);

    // create a node for all other nodes
    m_pAllNode = new svt_node();
    m_pAllNode->setName( "(sculptor_scene)m_pAllNode" );
    m_pAllNode->setTransformation(pMat);
    m_pGraph->add(m_pAllNode);

    // create node for the clipping plane
    m_pClipNode = new svt_node();
    m_pClipNode->setName( "(sculptor_scene)m_pClipNode" );
    m_pAllNode->add(m_pClipNode);

    // create node for the lua nodes
    m_pLuaNode = new svt_node();
    m_pLuaNode->setName( "(sculptor_scene)m_pLuaNode" );
    m_pAllNode->add(m_pLuaNode);

    // create new node for the data objects
    m_pDataTopNode = new svt_node();
    m_pDataTopNode->setName( "(sculptor_scene)m_pDataTopNode" );
    m_pAllNode->add(m_pDataTopNode);

    // add svt_coord_system to show rotation
    m_pCoordSystem = new svt_coord_system();
    m_pCoordSystem->setName("(sculptor_scene)m_pCoordSystem");
    m_pAllNode->add(m_pCoordSystem);

    // build intro
    buildIntro();

    // construct a lightsource container
    m_pLight = new svt_light();
    m_pLight->setName("(sculptor_scene)m_pLight");
    // construct the lightsource
    m_pLightSource = new svt_lightsource(new svt_pos(0.0f,0.0f,5.0f));
    m_pLightSource->setInfinite( false );
    m_pLightSource->setName("(sculptor_scene)m_pLightSource");
    // add to container
    m_pLight->add(m_pLightSource);
    // activate light
    setLight(m_pLight);

    // switch to the intro mode
    switchToIntro();

    // put the sg into the scene
    setSceneGraph(m_pGraph);
};

/**
 * adjust some opengl settings here
 */
void sculptor_scene::prepareGL()
{
    // no local lighting and two side lighting
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

    Real32 aMatSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    Real32 aMatShininess[1] = { 100.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, aMatSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, aMatShininess);
}

/**
 * set the node which should be moved around
 * \param pNode pointer to the node
 * \param oOrigin svt_vector4 object with the origin (the pivot point around which the user can rotate everything)
 */
void sculptor_scene::setMovedNode(svt_node* pNode, svt_vector4<Real32> oOrigin)
{
    m_pMoveTool->setPivotPoint( oOrigin );
    m_pMoveTool->setMoveNode(pNode);
}

/**
 * get the currently moved node
 * \return pointer to the node, which is moved around by the user
 */
svt_node* sculptor_scene::getMovedNode()
{
    return m_pMoveTool->getMoveNode();
}

/**
 * get the pivot point around which we rotate
 * \return vvt_vector4 object with the origin (the pivot point around which the user can rotate everything)
 */
svt_vector4<Real32> sculptor_scene::getPivotPoint()
{
    return m_pMoveTool->getPivotPoint();
}

/**
 * get the node that all the lua svt objects are attached to
 * \return pointer to the node
 */
svt_node* sculptor_scene::getLuaNode()
{
    return m_pLuaNode;
}

/**
 * set move tool center mode
 * \param bCenter if true the center mode is enabled
 */
void sculptor_scene::setCenterMode(bool bCenter)
{
    m_pMoveTool->setCenterMode(bCenter);
}
/**
 * get move tool center mode
 * \return if true the center mode is enabled
 */
bool sculptor_scene::getCenterMode()
{
    return m_pMoveTool->getCenterMode();
}
/**
 * set move tool fly mode
 * \param bFly if true the fly mode is enabled
 */
void sculptor_scene::setFlyMode(bool bFly)
{
    m_pMoveTool->setFlyMode(bFly);
}
/**
 * get move tool fly mode
 * \return if true the fly mode is enabled
 */
bool sculptor_scene::getFlyMode()
{
    return m_pMoveTool->flyMode();
}

/**
 * set force tool center mode
 * \param bCenter if true the center mode is enabled
 */
void sculptor_scene::setForceCenterMode(bool bCenter)
{
    m_pForceTool->setCenterMode(bCenter);
}
/**
 * get force tool center mode
 * \return if true the center mode is enabled
 */
bool sculptor_scene::getForceCenterMode()
{
    return m_pForceTool->getCenterMode();
}

/**
 * set translation scaling
 * \param fTranslationScaling (factor with which translations are multiplied)
 */
void sculptor_scene::setForceTranslationScale(Real32 fTranslationScaling)
{
    m_pForceTool->setTranslationScale(fTranslationScaling);
}
/**
 * get translation scaling
 * \return translation scale factor
 */
Real32 sculptor_scene::getForceTranslationScale()
{
    return m_pForceTool->getTranslationScale();
}

/**
 * switch to the intro
 */
void sculptor_scene::switchToIntro()
{
    m_pDataTopNode->setVisible(false);
    m_pClipNode->setVisible(false);

    if (m_pIntro)
        m_pIntro->setVisible(true);
}

/**
 * switch to the data viewer
 */
void sculptor_scene::switchToData()
{
    m_pDataTopNode->setVisible(true);
    m_pClipNode->setVisible(true);

    if (m_pIntro)
        m_pIntro->setVisible(false);
}

/**
 * build the intro
 */
void sculptor_scene::buildIntro()
{
    // right now there is no intro - perhaps somebody can add a nice animation to show
    // some visualization features of sculptor

    /*
    // create font
    svt_font* pFont = new svt_font("glf/times_new1.glf");

    // construct intro node
    m_pIntro = new svt_node(new svt_pos(-1.0f,0.1f,-1.0f));
    m_pGraph->add(m_pIntro);

    // construct a text
    svt_text* pText = new svt_text(new svt_pos(0.0f,0.0f,0.0f), "Sculptor", 20, pFont);
    pText->setProperties(svt_properties_solid_white);
    pText->setRotation(new svt_rotation(0.0f,0.0f,0.0f));
    m_pIntro->add(pText);
    */

    m_pIntro = NULL;
}

/**
 * get the data top node (for connecting new volume/pdb/... files to it)
 */
svt_node* sculptor_scene::getDataTopNode()
{
    return m_pDataTopNode;
};

/**
 * get the clip top node (for connecting new clipping plane to it)
 */
svt_node* sculptor_scene::getClipTopNode()
{
    return m_pClipNode;
};

/**
 * get the top node for all other nodes
 * \return pointer to the top node
 */
svt_node* sculptor_scene::getTopNode()
{
    return m_pAllNode;
}

/**
 * delete a data object
 */
void sculptor_scene::delDataNode(svt_node* pNode)
{
    svt_setStop(true);
    m_pDataTopNode->del(pNode);
    svt_setStop(false);
}

/**
 * delete a clip-plane object
 */
void sculptor_scene::delClipNode(svt_node* pNode)
{
    svt_setStop(true);
    m_pClipNode->del(pNode);
    svt_setStop(false);
}

/**
 * reset the viewpoint
 */
void sculptor_scene::resetViewpoint()
{
    m_pAllNode->getPos()->setToId();
}

///////////////////////////////////////////////////////////////////////////////
// view manipulation
///////////////////////////////////////////////////////////////////////////////

/**
 * save a screenshot
 * \param pFilename pointer to array of char with the filename
 */
void sculptor_scene::saveScreenshot(QString oFilename)
{
    svt_setStop(true);
    svt_saveScreenshot( QtoStd(oFilename) );
    svt_setStop(false);
}

/**
 * export as Alias/Wavefront file
 * \param pFilename pointer to array of char with the base of the filename (no extension)
 */
void sculptor_scene::renderOBJ(const char* pFilename)
{
    char pOBJ[1024];
    char pMAT[1024];

    sprintf(pOBJ, "%s.obj", pFilename );
    sprintf(pMAT, "%s.mtl", pFilename );

    outputOBJ(pOBJ);
    outputMAT(pMAT);

    return;
}

/**
 * render with povray
 */
void sculptor_scene::renderPOV()
{
/*    QString oStr;
    povrayDlg oDlg;

    // init output size to size of first canvas
    oDlg.m_pWidth->setText(oStr.setNum(svt_getDisplayWidth(0)));
    oDlg.m_pHeight->setText(oStr.setNum(svt_getDisplayHeight(0)));

    if (oDlg.exec() == QDialog::Accepted)
    {
	    int iWidth = oDlg.m_pWidth->text().toInt();
	    int iHeight = oDlg.m_pHeight->text().toInt();
	    float fAA = oDlg.m_pAA->text().toFloat();

        if (oDlg.m_pPovOut->isChecked())
        {
            // open file chooser dialog
            Q3FileDialog oFD(NULL);
            QString oFilter( "PovRay Scene Description (*.pov)" );
            oFD.setMode( Q3FileDialog::AnyFile );
            oFD.setFilter( oFilter );
            oFD.setViewMode( Q3FileDialog::List );
            oFD.setPreviewMode( Q3FileDialog::NoPreview );
            oFD.setSelectedFilter( oFilter );
            
            QString oFileName;
            
            if ( oFD.exec() == QDialog::Accepted )
            {
                oFileName = oFD.selectedFile();
                ofstream oFile(oFileName.ascii());
                streambuf* pBuffer = cout.rdbuf();
                cout.rdbuf(oFile.rdbuf());
                outputPOV(oDlg.m_pRadiosity->isChecked(), iWidth, iHeight);
                oFile.close();
                cout.rdbuf(pBuffer);
            }
            return;
            
        } else {
            // render to povray file
            ofstream oFile("povray.pov");
            streambuf* pBuffer = cout.rdbuf();
            cout.rdbuf(oFile.rdbuf());
            outputPOV(oDlg.m_pRadiosity->isChecked(), iWidth, iHeight);
            oFile.close();
            cout.rdbuf(pBuffer);
            
            // create system call
            char pCall[1024];
#ifndef WIN32
            sprintf(pCall, "povray +P -W%i -H%i +A%f +X povray.pov -O%s",iWidth, iHeight, fAA, oDlg.m_pFileOut->text().ascii() );
#else
            // +FN for direct PNG output
            // /NR does not open all files sometimes were opened by povray
            // /EDIT does open povray.pov to edit and change the pov file
            // you have to have pvengine in your PATH (C:\Programme\POV-Ray for Windows v3.6\bin)
            sprintf(pCall, "pvengine /NR /EDIT povray.pov -W%i -H%i +A%f +X povray.pov +FN -O%s",iWidth, iHeight, fAA, oDlg.m_pFileOut->text().ascii() );
#endif
            
#ifndef WIN32
            pid_t pID = fork();
            if (pID == 0)
            {
#endif
                svtout << pCall << endl;
                svt_system(pCall);
                
#ifndef WIN32
                _exit(0);
            }
#endif
        }
    }
*/
}

///////////////////////////////////////////////////////////////////////////////
// force feedback
///////////////////////////////////////////////////////////////////////////////

/**
 * set the force algorithm
 * \param pForceAlgo pointer to the sculptor_force_algo object
 */
void sculptor_scene::setForceAlgo(sculptor_force_algo* pForceAlgo)
{
    m_pForceTool->setForceCalc((svt_adv_force_calc*)pForceAlgo);
}

/**
 * get the force algorithm
 * \return pointer to the sculptor_force_algo object
 */
sculptor_force_algo* sculptor_scene::getForceAlgo()
{
    return (sculptor_force_algo*) m_pForceTool->getForceCalc();
}

/**
 * Set pipette visible
 * \param bVisible if true the 3D cursor is shown
 */
void sculptor_scene::setPipetteVisible( bool bVisible )
{
    if ( m_pPipette != NULL )
        m_pPipette->setVisible( bVisible );
}
/**
 * Get pipette visible
 * \return true if the 3D cursor is shown
 */
bool sculptor_scene::getPipetteVisible( )
{
    if ( m_pPipette != NULL )
        return m_pPipette->getVisible();
    else
        return false;
}

/**
 * Set coordinate system visiblity
 * \param bShowCoordSystem determines if visible or not
 */
void sculptor_scene::setCoordinateSystemVisible(bool bShowCoordSystem)
{
    m_pCoordSystem->setVisible(bShowCoordSystem);
}

/**
 * Get coordinate system visiblity
 * \return true if coordinate system visible
 */
bool sculptor_scene::getCoordinateSystemVisible()
{
    return m_pCoordSystem->getVisible();
}

/**
 * get the current force updates per second rate
 */
int sculptor_scene::getFUS()
{
    return m_pForceTool->getFUS();
}

/**
 * get the move tool
 * \return pointer to the move tool
 */
svt_move_tool* sculptor_scene::getMoveTool()
{
    return m_pMoveTool;
}

/**
 * get the force tool
 * \return pointer to the force tool
 */
svt_adv_force_tool* sculptor_scene::getForceTool()
{
    return m_pForceTool;
}

/**
 * up mouse wheel button pressed
 */
void sculptor_scene::upMouseWheel(int, int)
{
    svt_node* pNode = m_pGraph;

    svt_matrix4<Real64> oTrans;
    oTrans.translate( 0.0, 0.0, 0.5 );
    svt_matrix4<Real64> oMat( *pNode->getTransformation());
    oMat = oTrans * oMat;
    oMat.copyToMatrix4f( *pNode->getTransformation() );
}
/**
 * down mouse wheel button pressed
 */
void sculptor_scene::downMouseWheel(int, int)
{
    svt_node* pNode = m_pGraph;

    svt_matrix4<Real64> oTrans;
    oTrans.translate( 0.0, 0.0, -0.5 );
    svt_matrix4<Real64> oMat( *pNode->getTransformation());
    oMat = oTrans * oMat;
    oMat.copyToMatrix4f( *pNode->getTransformation() );
}

/**
 * key event
 */
void sculptor_scene::keyPressed(unsigned char key, int, int)
{
    sculptor_document* pDoc;
    pdb_document* pPdbDoc;

    switch(key)
    {
    case 'm':
    case 'M':
        g_pWindow->toggleMovedDoc();
        break;

    case 'f':
    case 'F':
        g_pWindow->toggleForces();
        break;

    case 'g':
    case 'G':
        g_pWindow->toggleForceDlg();
        break;

    case 'x':
    case 'X':
        g_pWindow->makeNewSolution();
        break;

    case 'c':
    case 'C':
        g_pWindow->toggleStericClashing();
        break;

    case 'a':
    case 'A':
	pDoc = g_pWindow->getDocumentList()->getCurrentItem()->getDoc();
	if (!pDoc || pDoc->getType() != SENSITUS_DOC_PDB) 
	{
	  pDoc = g_pWindow->getProbeDoc();
	}
        if (pDoc && pDoc->getType() == SENSITUS_DOC_PDB)
        {
	    pPdbDoc = (pdb_document*) pDoc;
	    pPdbDoc->sTransAdd();
	    pPdbDoc->showTransList();
        }
        break;

    case 'p':
    case 'P':
        g_pWindow->showRefinementDlg();
        break;

    case 'r':
    case 'R':
        svt_setMouseMode( SVT_MOUSE_ROT );
        break;

    case 't':
    case 'T':
        svt_setMouseMode( SVT_MOUSE_TRANS );
        break;

    case 's':
    case 'S':
        svt_setMouseMode( SVT_MOUSE_ZOOM );
        break;

    case '1':
        setSelection( true );
        svt_redraw();
        break;

    case 27:
        svt_exit(0);
        break;
    default:
        break;
    }
}
/**
 * special key event
 */
void sculptor_scene::specialKeyPressed(int key, int, int)
{
    switch(key)
    {
    case SVT_KEY_CONTROL:
        m_bControl = true;
        break;
    }
}
/**
 * special key event
 */
void sculptor_scene::specialKeyReleased(int key, int, int)
{
    switch(key)
    {
    case SVT_KEY_CONTROL:
        m_bControl = false;
        break;
    }
}

/**
 * Set scale of mouse wheel translations
 * \param fMWScale new scale
 */
void sculptor_scene::setMouseWheelScale(Real32 fMWScale)
{
    m_fMWScale = fMWScale;
};

/**
 * left mouse button pressed
 */
void sculptor_scene::leftMouseButton(int, int)
{
    if (m_bControl)
    {
        sculptor_document* pDoc = g_pWindow->getDocumentList()->getCurrentItem()->getDoc();
        if (pDoc != NULL)
        {
            vector< int > aIDs = pDoc->getCodebookIDs();
            if (aIDs.size() == 0)
                return;

            setSelection( true );
            setSelectionArray( aIDs );
            svt_getScene()->drawGL();

            svt_node* pNode = svt_getScene()->getSelectedNode();
            if (pNode)
            {
                vector<sculptor_document*> aDocs = g_pWindow->getDocumentList()->getDocuments();

                for(unsigned int i=0; i<aDocs.size(); ++i)
                    aDocs[i]->selectCodebookVector(pNode);
            }
        }
    }
}

/**
 * right mouse button pressed
 */
void sculptor_scene::rightMouseButton(int, int)
{
    if (m_bControl)
    {
	vector<sculptor_document*> aDocs = g_pWindow->getDocumentList()->getDocuments();

	for(unsigned int i=0; i<aDocs.size(); ++i)
	    aDocs[i]->clearCVSelection();
    }
}
