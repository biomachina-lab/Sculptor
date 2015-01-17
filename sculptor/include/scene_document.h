/***************************************************************************
                          scene_document.h  -  description
                             -------------------
    begin                : 16.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_DOCUMENT_H
#define SCENE_DOCUMENT_H

// sculptor includes
#include <sculptor_document.h>
class sculptor_scene;
// svt includes
#include <svt_core.h>
class svt_matrix4f;
class svt_scene;
class svt_scenegraph;

/**
 * This class stores the informations about the global scene parameters (zooming, rotation, ...)
 * @author Stefan Birmanns
 */
class scene_document : public sculptor_document
{

protected:

    svt_matrix4f* m_pGlobalMat;
    sculptor_scene* m_pScene;
    svt_scenegraph* m_pSceneGraph;

    

public:

    /**
     * Constructor
     */
    scene_document();

    /**
     * Destructor
     */
    virtual ~scene_document();

    /**
     * set the svt_scene
     * \param pScene the svt_scene
     */
    void setScene(sculptor_scene* pScene);

    /**
     * get the svt_node the document is providing
     * \return pointer to the svt_node object
     */
    svt_node* getNode();

    /**
     * zoom in.
     */
    void zoomIn();
    /**
     * zoom out.
     */
    void zoomOut();

    /**
     * get the type of the document
     * \return SENSITUS_DOC_SCENE
     */
    virtual int getType();
};

#endif
