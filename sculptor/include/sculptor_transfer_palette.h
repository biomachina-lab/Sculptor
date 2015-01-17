/***************************************************************************
                          sculptor_transfer_palette
			  -------------------------
    begin                : 03/22/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef __SENSITUS_TRANSFER_PALETTE_H
#define __SENSITUS_TRANSFER_PALETTE_H

// svt includes
class svt_float2mat;
// qt4 includes
#include <QWidget>
class QPaintEvent;

/**
 * a widget that shows the current color palette of a svt_float2mat object
 */
class sculptor_transfer_palette : public QWidget
{
    Q_OBJECT

protected:

    // transfer function
    svt_float2mat* m_pF2M;

    /**
     * paintevent handler for rendering the object
     */
    void paintEvent( QPaintEvent * );

public:

    /**
     * Constructor
     */
    sculptor_transfer_palette(QWidget* pParent = NULL);

    /**
     * Destructor
     */
    virtual ~sculptor_transfer_palette();

    /**
     * set transfer function
     */
    void setFloat2Mat(svt_float2mat* pF2M);
};

#endif
