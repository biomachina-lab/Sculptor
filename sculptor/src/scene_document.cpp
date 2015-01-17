/***************************************************************************
                          scene_document.cpp  -  description
                             -------------------
    begin                : 16.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <scene_document.h>
#include <sculptor_scene.h>
#include <sculptor_window.h>
// svt includes
#include <svt_init.h>
#include <svt_scenegraph.h>
#include <svt_matrix4f.h>

extern sculptor_window* g_pWindow;


/**
 * Constructor
 */
scene_document::scene_document()
    : sculptor_document(NULL),
      m_pGlobalMat( NULL ),
      m_pScene( NULL ),
      m_pSceneGraph( NULL )
{
    m_oDisplayName = QString("Sculptor Scene");
}

/**
 * Destructor
 */
scene_document::~scene_document()
{
}

/**
 * set the svt_scene
 * \param pScene the svt_scene
 */
void scene_document::setScene(sculptor_scene* pScene)
{
    m_pScene = pScene;

    if (m_pScene)
    {
        m_pSceneGraph = m_pScene->getSceneGraph();

        if (m_pSceneGraph)
        {
            m_pGlobalMat = m_pSceneGraph->getTransformation();

            if (!m_pGlobalMat)
            {
                m_pGlobalMat = new svt_matrix4f("global matrix");
                m_pSceneGraph->setTransformation(m_pGlobalMat);
            }
        }
    } 
}

/**
 * get the svt_node the document is providing
 * \return pointer to the svt_node object
 */
svt_node* scene_document::getNode()
{
    return m_pScene->getTopNode();
}

/**
 * zoom in
 */
void scene_document::zoomIn()
{
    if (m_pScene)
        if (m_pScene->getSceneGraph())
        {
            float fSX = m_pScene->getSceneGraph()->getScaleX();
            m_pScene->getSceneGraph()->setScale(fSX*1.2f);
        }
}
/**
 * zoom out
 */
void scene_document::zoomOut()
{
    if (m_pScene)
        if (m_pScene->getSceneGraph())
        {
            float fSX = m_pScene->getSceneGraph()->getScaleX();
            m_pScene->getSceneGraph()->setScale(fSX*0.8f);
        }
}

/**
 * get the type of the document
 * \return SENSITUS_DOC_SCENE
 */
int scene_document::getType()
{
    return SENSITUS_DOC_SCENE;
}
