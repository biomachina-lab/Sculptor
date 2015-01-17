/***************************************************************************
                          sculptor_style
                          --------------
    begin                : 15.01.2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#include <sculptor_style.h>

#ifndef QT_NO_STYLE_WINDOWS

#include <QApplication>
#include <QGroupBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTabBar>
#include <QColorGroup>


sculptor_style::sculptor_style()
    : m_oNewPalette( QColor( 100, 100, 100 ) , QColor( 120, 120, 120 ) )
{
}

int sculptor_style::pixelMetric ( PixelMetric metric, const QWidget * widget ) const
{
    int ret;

    switch(metric)
    {
    case PM_DialogButtonsSeparator:
	ret = 2;
	break;
    case PM_DialogButtonsButtonWidth:
	ret = 5;
	break;
    case PM_DialogButtonsButtonHeight:
	ret = 5;
	break;
    case PM_DefaultFrameWidth:
        ret = 1;
        break;

    case PM_ButtonMargin:
        ret = 1;
        break;
    case PM_ButtonDefaultIndicator:
        ret = 1;
        break;

    case PM_TabBarTabOverlap:
	ret = 1;
	break;

    case PM_TabBarBaseHeight:
	ret = 0;
	break;

    case PM_TabBarBaseOverlap:
	ret = 0;
	break;

    case PM_TabBarTabHSpace:
	ret = 20;
	break;

    case PM_TabBarTabShiftHorizontal:
    case PM_TabBarTabShiftVertical:
	ret = 2;
	break;

#ifndef QT_NO_TABBAR
    case PM_TabBarTabVSpace:
	{
	    const QTabBar * tb = (const QTabBar *) widget;
	    if ( tb && ( tb->shape() == QTabBar::RoundedNorth ||
			 tb->shape() == QTabBar:: RoundedSouth ) )
		ret = 5;
	    else
		ret = 0;
	    break;
	}
#endif

    default:

// QT3FIXME         ret = QWindowsStyle::pixelMetric( metric, widget );
        break;
    }

    return ret;
};

void sculptor_style::polish( QApplication *app)
{
    m_oOldPalette = app->palette();

    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Base,            QColor( 120, 120, 120 ) );
    m_oNewPalette.setColor( QPalette::Inactive, QColorGroup::Base,            QColor( 120, 120, 120 ) );
    m_oNewPalette.setColor( QPalette::Disabled, QColorGroup::Base,            QColor( 120, 120, 120 ) );

    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Shadow,          QColor( 120, 120, 120 ) );

    /*
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Light,           QColor( 150, 150, 150 ) );
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Midlight,        QColor( 150, 150, 150 ) );
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Dark,            QColor( 150, 150, 150 ) );
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Shadow,          QColor( 150, 150, 150 ) );
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Mid,             QColor( 150, 150, 150 ) );
    */

    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Button,          QColor( 150, 150, 150 ) );
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::ButtonText,      QColor( 250, 250, 250 ) );

    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Highlight,       QColor( 255, 117, 29 ) );
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::HighlightedText, QColor( 77, 77, 77 ) );
    m_oNewPalette.setColor( QPalette::Inactive, QColorGroup::Highlight,       QColor( 255, 117, 29 ) );
    m_oNewPalette.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 77, 77, 77 ) );
    m_oNewPalette.setColor( QPalette::Disabled, QColorGroup::Highlight,       QColor( 255, 117, 29 ) );
    m_oNewPalette.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 77, 77, 77 ) );

    /*
    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Foreground,      Qt::white );

    m_oNewPalette.setColor( QPalette::Active,   QColorGroup::Text,            QColor( 100, 100, 100 ) );
    m_oNewPalette.setColor( QPalette::Inactive, QColorGroup::Foreground,      Qt::white );
    m_oNewPalette.setColor( QPalette::Inactive, QColorGroup::Text,            Qt::white );
    m_oNewPalette.setColor( QPalette::Inactive, QColorGroup::ButtonText,      Qt::white );
    m_oNewPalette.setColor( QPalette::Disabled, QColorGroup::Foreground,      Qt::lightGray );
    m_oNewPalette.setColor( QPalette::Disabled, QColorGroup::Text,            Qt::lightGray );
    m_oNewPalette.setColor( QPalette::Disabled, QColorGroup::ButtonText,      Qt::lightGray );
    */
    qApp->setPalette( m_oNewPalette, TRUE );
};

void sculptor_style::unPolish( QApplication *app)
{
    app->setPalette(m_oOldPalette, TRUE);
};

void sculptor_style::polish( QWidget* w)
{
//    if (w->inherits("QTextEdit") )
//    {
//    }

/*    if (w->inherits("QMenuBar") || w->inherits("QPopupMenu"))
    {
        w->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
        return;
    }
    if (w->inherits("QDockArea") || w->inherits("QDockWindow") || w->inherits("QToolButton") || w->inherits("QGrid"))
    {
        w->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
        w->setPaletteForegroundColor( QColor( 120, 120, 120 ) );
        return;
    }
    if (w->inherits("QLabel") )
    {
        w->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
        w->setPaletteForegroundColor( QColor( 200, 200, 200 ) );
        return;
    }
*/
    /*if (w->inherits("QListBox") )
    {
        w->setBackgroundMode( PaletteBackground );
        w->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
    }*/

/*    if (w->inherits("QSplitter") )
    {
        w->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
        w->setPaletteForegroundColor( QColor( 120, 120, 120 ) );
        return;
    }
*/
    /*if (w->inherits("QPushButton"))
    {
        QPushButton* pPushButton = (QPushButton*)w;
        //pPushButton->setFlat( TRUE );
        pPushButton->setPaletteBackgroundColor( QColor( 150, 150, 150 ) );
        pPushButton->setPaletteForegroundColor( QColor( 250, 250, 250 ) );
        return;
    }*/
/*
    if (w->inherits("QGroupBox"))
    {
        QGroupBox* pGroupBox = (QGroupBox*)w;
        pGroupBox->setLineWidth( 0 );
        pGroupBox->setFrameShape( QFrame::Box );
        pGroupBox->setFrameShadow( QFrame::Plain );
        pGroupBox->setPaletteBackgroundColor( QColor( 150, 150, 150 ) );
        pGroupBox->setPaletteForegroundColor( QColor( 180, 180, 180 ) );
        return;
     }

    if (!w->inherits("QTabBar") && w->parentWidget() != NULL && w->parentWidget()->inherits("QTabWidget"))
    {
        w->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
        return;
    }
    if (w->inherits("QTabBar") || w->inherits("QTabWidget") || w->inherits("QTabDialog"))
    {
        w->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
        w->setPaletteForegroundColor( QColor( 200, 200, 200 ) );
        return;
    }
*/
    /*if (w->inherits("QFrame"))
    {
        QFrame* pFrame = (QFrame*)w;
        pFrame->setLineWidth( 0 );
        pFrame->setFrameShape( QFrame::Box );
        pFrame->setFrameShadow( QFrame::Plain );
        pFrame->setPaletteBackgroundColor( QColor( 120, 120, 120 ) );
        //pFrame->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
        return;
    }*/
}

void sculptor_style::unPolish( QWidget* )
{
/*    if (w->inherits("QGroupBox"))
    {
        w->setBackgroundMode( QWidget::PaletteButton );
        return;
    }*/
}

#endif
