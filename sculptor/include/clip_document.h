/*-*-c++-*-*****************************************************************
                          clip_document.h  -  description
                             -------------------
    begin                : 30.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef CLIP_DOCUMENT_H
#define CLIP_DOCUMENT_H

// sculptor includes
#include <sculptor_document.h>
// svt includes
class svt_node;
class svt_clipplane;
// qt4 includes
#include <QObject>
// forward declarations
class svt_rectangle;
class prop_clip;


/**
 * This class stores a single clipping plane document
 * @author Stefan Birmanns
 */
class clip_document : public sculptor_document
{
    Q_OBJECT

protected:

    svt_node* m_pClipNode;
    svt_clipplane* m_pClipplane;
    prop_clip* m_pClipPropDlg;

public:

    /**
     * Constructor
     * \param svt_node* pointer to svt node
     */
    clip_document(svt_node* pNode);

    /**
     * Destructor
     */
    virtual ~clip_document();

    /**
     * return the svt_node the document is providing
     *\return svt_node*  the document's svt_node
     */
    svt_node* getNode();

    /**
     * returns the property dialog for the document
     * \param QWidget* the dialog's parent
     * \return QWidget* the property dialog
     */
    virtual QWidget* createPropDlg(QWidget* pParent);

    /**
     * get the type of the document
     * \return SENSITUS_DOC_CLIP
     */
    virtual int getType();

    /**
     * disable that one can change the status of the clipplanes to "changed" - does not make any sense here
     */
    void setChanged();

    /**
     * get if clipping plane rectangle visible
     */
    bool getRectVisible();

public slots:

    /**
     * set if clipping plane rectangle visible
     * \param bVisible if true rectangle is visible
     */
    void setRectVisible(bool bVisible);
};

#endif
