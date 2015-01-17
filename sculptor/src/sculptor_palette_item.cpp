/***************************************************************************
                          sculptor_palette_item
                          ----------------
    begin                : 10/10/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#include <sculptor_palette_item.h>
#include <qstyle.h>

sculptor_palette_item::sculptor_palette_item( svt_color oColor ) : Q3ListBoxItem(),
    m_oColor( oColor )
{
    setCustomHighlighting( TRUE );
}

void sculptor_palette_item::paint( QPainter *painter )
{
    // evil trick: find out whether we are painted onto our listbox
    bool in_list_box = listBox() && listBox()->viewport() == painter->device();

    QRect r ( 0, 0, width( listBox() ), height( listBox() ) );
    if ( in_list_box && isSelected() )
        painter->eraseRect( r );

    QColor oColor;
    oColor.setRgb( (int)(255.0f*m_oColor.getR()), (int)(255.0f*m_oColor.getG()), (int)(255.0f*m_oColor.getB()) );

    painter->fillRect( 5, 5, width( listBox() ) - 10, height( listBox() ) - 10, oColor );
// QT3FIXME     if ( in_list_box && isCurrent() )
// QT3FIXME         listBox()->style().drawPrimitive( QStyle::PE_FocusRect, painter, r, listBox()->colorGroup() );
}

int sculptor_palette_item::width( const Q3ListBox* lb) const
{
    return lb->visibleWidth();
}
int sculptor_palette_item::height( const Q3ListBox* ) const
{
    return 32;
}

/**
 * Set the color of this list box item
 * \param oColor svt_color object
 */
void sculptor_palette_item::setColor( svt_color oColor )
{
    m_oColor = oColor;
}
/**
 * Get the color of this list box item
 * \return svt_color object
 */
svt_color sculptor_palette_item::getColor( ) const
{
    return m_oColor;
}
