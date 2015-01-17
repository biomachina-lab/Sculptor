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

#ifndef SCULPTOR_PALETTE_ITEM_H
#define SCULPTOR_PALETTE_ITEM_H

#include <svt_basics.h>
#include <svt_color.h>
#include <q3listbox.h>
#include <qpainter.h>

class sculptor_palette_item : public Q3ListBoxItem
{
    svt_color m_oColor;

public:

    /**
     * Constructor
     */
    sculptor_palette_item( svt_color oColor );

protected:

    /**
     * Paint routine
     */
    virtual void paint( QPainter * );

    virtual int width( const Q3ListBox* ) const;
    virtual int height( const Q3ListBox* ) const;

    /**
     * Set the color of this list box item
     * \param oColor svt_color object
     */
    void setColor( svt_color oColor );
    /**
     * Get the color of this list box item
     * \return svt_color object
     */
    svt_color getColor( ) const;
};

#endif
