/***************************************************************************
                          sculptor_dlg_colorselect.cpp
                          ----------------------------
    begin                : 10-27-2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_colorselect.h>
// svt includes
#include <svt_palette.h>
#include <svt_color.h>
// qt4 includes
#include <QPixmap>
#include <QColorDialog>
#include <QScrollBar>


/**
 * Constructor
 */
dlg_colorselect::dlg_colorselect(QWidget* pParent, svt_palette* pPalette)
    : QDialog( pParent ),
      m_pPalette( pPalette )
{
    setupUi(this);

    // ok, cancel, and new color button connections
    connect( m_pButtonApply,  SIGNAL(clicked()), this, SLOT(accept()) );
    connect( m_pButtonCancel, SIGNAL(clicked()), this, SLOT(reject()) );
    connect( m_pButtonNew, SIGNAL(clicked()), SLOT(addItemToPalette()) );

    // set an icon size (default is zero...)
    m_pColorList->setIconSize( QSize(162, 25) );

    // for each color in the palette, add an item to the listwidget
    for (unsigned int i=0; i<m_pPalette->size(); ++i)
        addItemFromPalette(i);
    
    //modify the highlight color from the default dark blue
    QPalette qp = m_pColorList->palette();
    //check the rgb color values ~ http://kb.iu.edu/data/aetf.html
    qp.setColor(QPalette::Active, QPalette::Highlight, QColor::fromRgb(173, 216, 230, 128) );
    m_pColorList->setPalette(qp);
    
}

/**
 * Destructor
 */
dlg_colorselect::~dlg_colorselect()
{
}

/**
 *
 */
void dlg_colorselect::addItemFromPalette(int iIndex)
{
    //create a new ListWidgetItem
    QListWidgetItem* pItem = new QListWidgetItem( m_pColorList );

    // let the item know about its height
    pItem->setSizeHint( QSize(-1, 33) );

    // create a pixmap to be put into the item
    QPixmap oPixmap( m_pColorList->iconSize() );
    oPixmap.fill( QColor((int)(m_pPalette->getColor(iIndex)->getR()*255.0f),
                         (int)(m_pPalette->getColor(iIndex)->getG()*255.0f),
                         (int)(m_pPalette->getColor(iIndex)->getB()*255.0f)) );
    
    //The following code intends to trick the user in believing that color items are placed at the center of QListWidgetItem 
    //convert QPixMap to QImage
    QImage image(oPixmap.toImage());
    int border = 0;
    
    //create a transparent border
        for (int j=0; j < image.height(); ++j)
        {
                for (int i=0; i < image.width(); ++i)
                {
                  if ( ( j < border ) || ( ( image.height() - j ) <= border ) || ( i < border + 10 ) ||   ( image.width() - i ) <= border + 1 )
                    image.setPixel(i, j, qRgba(255, 255, 255, 255));
                }
        }

    //convert the image back to QPixMap
    QPixmap pixmap = QPixmap::fromImage(image);
    
    // and add it to the item
    pItem->setIcon( QIcon(pixmap) );

}

/**
 *
 */
void dlg_colorselect::addItemToPalette()
{
    svt_color oColSVT;
    QColor oCol = QColorDialog::getColor();

    if (oCol.isValid())
    {
        // compute the svt color from the qcolor
        oColSVT.setR( (float)(oCol.red()) / 255.0f );
        oColSVT.setG( (float)(oCol.green()) / 255.0f);
        oColSVT.setB( (float)(oCol.blue())  / 255.0f);

        // and add it to the palette
        m_pPalette->addColor(oColSVT);

        // add a corresponding item to the listwidget
        addItemFromPalette(m_pPalette->size()-1);

        // make item current and scroll to it
        m_pColorList->setCurrentRow(m_pColorList->count()-1);
        m_pColorList->scrollToItem(m_pColorList->item(m_pColorList->count()-1));

        // hide dialog and apply color
        accept();
    }
}

/**
 *
 */
void dlg_colorselect::setCurrent(int iIndex)
{
    m_pColorList->setCurrentRow( iIndex );
}

/**
 *
 */
int dlg_colorselect::getCurrent()
{
    return (int)m_pColorList->currentRow();
}
