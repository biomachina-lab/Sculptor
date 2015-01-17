/***************************************************************************
			  sculptor_prop_clip.cpp
			  -------------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor include
#include <sculptor_prop_clip.h>
#include <clip_document.h>

/**
 *
 */
prop_clip::prop_clip( QWidget *parent, clip_document* pDoc )
    : QWidget( parent ),
      m_pDoc( pDoc )
{
    setupUi(this);
    connect( m_pRectVisible, SIGNAL(toggled(bool)), m_pDoc, SLOT(setRectVisible(bool)) );
    update();
}

/**
 *
 */
prop_clip::~prop_clip()
{
}

/**
 *
 */
void prop_clip::update()
{
    m_pRectVisible->blockSignals( TRUE );
    m_pRectVisible->setChecked( m_pDoc->getRectVisible() );
    m_pRectVisible->blockSignals( FALSE );
}
