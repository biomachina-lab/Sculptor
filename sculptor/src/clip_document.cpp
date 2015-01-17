/***************************************************************************
                          clip_document.cpp  -  description
                             -------------------
    begin                : 30.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// svt includes
#include <svt_rectangle.h>
#include <svt_clipplane.h>
#include <svt_node.h>
// sculptor includes
#include <clip_document.h>
// uic-qt4 includes
#include <sculptor_prop_clip.h>


/*
 *
 */
clip_document::clip_document(svt_node* pNode)
    : sculptor_document( pNode ),
      m_pClipPropDlg( NULL )
{
    m_pClipNode = new svt_node();
    m_pClipplane = new svt_clipplane();

    svt_matrix4f* pMatClip = new svt_matrix4f;
    pMatClip->setFromTranslation( -0.001f, 0.0f, 0.0f );
    m_pClipplane->setTransformation( pMatClip );
    m_pClipplane->setName("(clip_document)m_pClipplane");

    m_pClipNode->setName("(clip_document)m_pClipNode");
    m_pClipNode->add( m_pClipplane );

    m_pGraphNode->add(m_pClipNode);
}

/*
 *
 */
clip_document::~clip_document()
{
}

/*
 *
 */
svt_node* clip_document::getNode()
{
    return m_pClipNode;
}

/**
 * returns the property dialog for the document
 * \param QWidget* the dialog's parent
 * \return QWidget* the property dialog
 */
QWidget* clip_document::createPropDlg(QWidget* pParent)
{
    if ( m_pClipPropDlg == NULL )
        m_pClipPropDlg = new prop_clip( pParent, this );

    return (QWidget*)m_pClipPropDlg;
}

/**
 * set if clipping plane rectangle visible
 * \param bVisible if true rectangle is visible
 */
void clip_document::setRectVisible(bool bVisible)
{
    m_pClipplane->setRectVisible( bVisible );
}

/**
 * get if clipping plane rectangle visible
 */
bool clip_document::getRectVisible()
{
    return m_pClipplane->getRectVisible();
}

/**
 * get the type of the document
 * \return SENSITUS_DOC_CLIP
 */
int clip_document::getType()
{
    return SENSITUS_DOC_CLIP;
}

/**
 * disable that one can change the status of the clipplanes to "changed" - does not make any sense here
 */
void clip_document::setChanged()
{
}
