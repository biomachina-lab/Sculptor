/***************************************************************************
                          fileform.cpp  -  description
                             -------------------
    begin                : 30.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#include <fileform.h>

#include <svt_iostream.h>

#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qgroupbox.h>
#include <qpixmap.h>

#include <raise.xpm>
#include <lower.xpm>
#include <delete.xpm>
#include <properties.xpm>
#include <moveSmall.xpm>
#include <eyeSmall.xpm>
#include <top.xpm>
#include <save.xpm>

/*
 *  Constructs a FileForm which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FileForm::FileForm( QWidget* parent,  const char* name, WFlags fl )
    : QGrid( 1, parent, name , fl)
{
    if ( !name )
        setName( "FileForm" );

    resize( 469, 233 );

    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, sizePolicy().hasHeightForWidth() ) );
    setCaption( tr( "Files" ) );

    QGroupBox* gb = new QGroupBox(this);

    FileFormLayout = new QGridLayout( gb );
    FileFormLayout->setSpacing( 6 );
    FileFormLayout->setMargin( 11 );

    Layout2 = new QGridLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    Layout1 = new QHBoxLayout; 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    // delete
    m_pButtonDelete = new QPushButton( gb, "m_pButtonDelete" );
    m_pButtonDelete->setPixmap( QPixmap(deleteIcon) );
    m_pButtonDelete->setFlat( true );
    QToolTip::add( m_pButtonDelete, "Delete the selected documents!" );
    Layout1->addWidget( m_pButtonDelete );
    // spacer
    Layout1->addItem( new QSpacerItem( 8, 0, QSizePolicy::Maximum ) );
    // move
    m_pButtonMove = new QPushButton( gb, "m_pButtonMove" );
    m_pButtonMove->setPixmap( QPixmap(moveSmallIcon) );
    m_pButtonMove->setFlat( true );
    QToolTip::add( m_pButtonMove, "Attach selected document to the mouse" );
    Layout1->addWidget( m_pButtonMove );
    // visible
    m_pButtonVisible = new QPushButton( gb, "m_pButtonVisible" );
    m_pButtonVisible->setPixmap( QPixmap(eyeSmallIcon) );
    m_pButtonVisible->setFlat( true );
    QToolTip::add( m_pButtonVisible, "Make selected documents visible / invisible" );
    Layout1->addWidget( m_pButtonVisible );
#ifdef SVT_SYSTEM_X11
    // properties - only in X11
    m_pButtonProperties = new QPushButton( gb, "m_pButtonProperties" );
    m_pButtonProperties->setPixmap( QPixmap(propIcon) );
    m_pButtonProperties->setFlat( true );
    QToolTip::add( m_pButtonProperties, "Show properties dialog for the selected document" );
    Layout1->addWidget( m_pButtonProperties );
#endif
    // spacer
    Layout1->addItem( new QSpacerItem( 8, 0, QSizePolicy::Maximum ) );
    // up
    m_pButtonUp = new QPushButton( gb, "m_pButtonUp" );
    m_pButtonUp->setPixmap( QPixmap(raiseIcon) );
    m_pButtonUp->setFlat( true );
    QToolTip::add( m_pButtonUp, "Move selected document up in the list" );
    Layout1->addWidget( m_pButtonUp );
    // down
    m_pButtonDown = new QPushButton( gb, "m_pButtonDown" );
    m_pButtonDown->setPixmap( QPixmap(lowerIcon) );
    m_pButtonDown->setFlat( true );
    QToolTip::add( m_pButtonDown, "Move selected document up in the list" );
    Layout1->addWidget( m_pButtonDown );
    // top
    m_pButtonTop = new QPushButton( gb, "m_pButtonTop" );
    m_pButtonTop->setPixmap( QPixmap(top) );
    m_pButtonTop->setFlat( true );
    QToolTip::add( m_pButtonTop, "Make selected document the first one in the list (provides origin of camera)" );
    Layout1->addWidget( m_pButtonTop );
    // spacer
    Layout1->addItem( new QSpacerItem( 8, 0, QSizePolicy::Maximum ) );
    // save
    m_pButtonSave = new QPushButton( gb, "m_pButtonSave" );
    m_pButtonSave->setPixmap( QPixmap(saveIcon) );
    m_pButtonSave->setFlat( true );
    QToolTip::add( m_pButtonSave, "Save selected documents to disk" );
    Layout1->addWidget( m_pButtonSave );

    // add the entire layout now
    Layout2->addLayout( Layout1, 1, 0 );

    m_pListFile = new QListBox( gb, "m_pListFile" );
    m_pListFile->setSelectionMode(QListBox::Extended);
    m_pListFile->setLineWidth( 2 );

    setAcceptDrops(TRUE);

    Layout2->addWidget( m_pListFile, 0, 0 );

    FileFormLayout->addLayout( Layout2, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FileForm::~FileForm()
{
    // no need to delete child widgets, Qt does it all for us
}

void FileForm::dropEvent(QDropEvent* event)
{
    QString text;

    if ( QTextDrag::decode(event, text) )
    {
        if ( text.contains("\n") == 0 )
            emit drop( text.remove("\r") );
        else {

            if ( text.contains("\n") )
            {
                for(unsigned int i=0; ; i++)
                {
                    QString oSection = text.section("\n", i, i);
                    oSection.remove("\r");

                    if (oSection.isEmpty())
                        return;

                    emit drop( oSection );
                }
            }

        }
    }
}

void FileForm::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept( QTextDrag::canDecode(event) );
}

QSize FileForm::sizeHint() const
{
    return QSize( 350, 200 );
}
