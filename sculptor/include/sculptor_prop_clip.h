/*-*-c++-*-*****************************************************************
			  sculptor_prop_clip.h
			  -------------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_PROP_CLIP_H
#define SCULPTOR_PROP_CLIP_H

// uic-qt4 includes
#include <ui_prop_clip.h>
// forward declarations
class clip_document;

/*
 * This class inherits from the QT4 Designer prop_clip class and implements its functionality.
 * \author Manuel Wahle
 */
class prop_clip : public QWidget, private Ui::prop_clip
{
    Q_OBJECT

    // the sculptor document
    clip_document* m_pDoc;

public:

    /**
     * Constructor
     */
    prop_clip( QWidget *parent, clip_document* pDoc );

    /**
     * Destructor
     */
    virtual ~prop_clip();

    /**
     * Update the dialog to match the state of the document and its svt_node
     */
    void update();

};
#endif // SCULPTOR_PROP_CLIP_H
