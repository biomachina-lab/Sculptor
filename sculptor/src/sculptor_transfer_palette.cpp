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

// sculptor includes
#include <sculptor_transfer_palette.h>
// svt includes
#include <svt_float2mat_points.h>
#include <svt_float2mat.h>
#include <svt_basics.h>
//qt4 includes
#include <QPainter>
#include <QWidget>
#include <QBrush>
#include <QPen>

/**
 * Constructor
 */
sculptor_transfer_palette::sculptor_transfer_palette(QWidget *pParent)
    : QWidget( pParent ),
      m_pF2M( NULL )
{
}

/**
 * Destructor
 */
sculptor_transfer_palette::~sculptor_transfer_palette()
{
}

/**
 * paintevent handler for rendering the object
 */
void sculptor_transfer_palette::paintEvent( QPaintEvent * )
{
    if (m_pF2M == NULL)
        return;

    QPainter p( this );

    int iWidth = width();
    int iHeight = height();
    int iHeightColor = iHeight/2;

    float fValue;
    float fMaxMin = m_pF2M->getMax() - m_pF2M->getMin();
    float fMin = m_pF2M->getMin();

    for(int iX=0; iX<iWidth; iX++)
    {
        fValue = fMin + (fMaxMin*((float)(iX) / (float)(iWidth)));

	p.setPen( QColor(
			 (int)(m_pF2M->getR(fValue)),
			 (int)(m_pF2M->getG(fValue)),
			 (int)(m_pF2M->getB(fValue))
			)
		);

	p.drawLine(iX, 0, iX, iHeightColor);

	p.setPen( QColor(
			 (int)(m_pF2M->getOpacity(fValue)),
			 (int)(m_pF2M->getOpacity(fValue)),
			 (int)(m_pF2M->getOpacity(fValue))
			)
		);

	p.drawLine(iX, iHeightColor+1, iX, iHeight);

    }

    p.setPen( QColor( 0, 0, 0 ) );
    p.drawRect( 0, 0, iWidth, iHeight);

}

/**
 * set transfer function
 */
void sculptor_transfer_palette::setFloat2Mat(svt_float2mat* pF2M)
{
    m_pF2M = pF2M;
}
