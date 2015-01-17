/***************************************************************************
                          tempmotors.cpp  -  description
                             -------------------
    begin                : 06.11.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

//#include <sculptor_scene.h>
#include <tempmotors.h>
// qt includes
#include <qlabel.h>
#include <q3progressbar.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <q3groupbox.h>
#include <qtimer.h>
//Added by qt3to4:
#include <Q3GridLayout>

/* 
 *  Constructs a TempMotorsDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TempMotorsDlg::TempMotorsDlg( sculptor_scene* pScene, QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "TempMotorsDlg" );
    resize( 338, 329 ); 
    setProperty( "caption", tr( "Temperature of Motors of Force Feedback Device" ) );
    TempMotorsDlgLayout = new Q3GridLayout( this ); 
    TempMotorsDlgLayout->setSpacing( 6 );
    TempMotorsDlgLayout->setMargin( 11 );

    TextLabel4_2 = new QLabel( this, "TextLabel4_2" );
    TextLabel4_2->setProperty( "text", tr( "Temperature of Motor for Translational Forces Y Axis:" ) );

    TempMotorsDlgLayout->addWidget( TextLabel4_2, 2, 0 );

    TextLabel4_2_2 = new QLabel( this, "TextLabel4_2_2" );
    TextLabel4_2_2->setProperty( "text", tr( "Temperature of Motor for Translational Forces Z Axis:" ) );

    TempMotorsDlgLayout->addWidget( TextLabel4_2_2, 4, 0 );

    TextLabel4_3 = new QLabel( this, "TextLabel4_3" );
    TextLabel4_3->setProperty( "text", tr( "Temperature of Motor for Rotational Forces X Axis:" ) );

    TempMotorsDlgLayout->addWidget( TextLabel4_3, 6, 0 );

    TextLabel4_3_2 = new QLabel( this, "TextLabel4_3_2" );
    TextLabel4_3_2->setProperty( "text", tr( "Temperature of Motor for Rotational Forces Y Axis:" ) );

    TempMotorsDlgLayout->addWidget( TextLabel4_3_2, 8, 0 );

    TextLabel4_3_3 = new QLabel( this, "TextLabel4_3_3" );
    TextLabel4_3_3->setProperty( "text", tr( "Temperature of  Motor for Rotational Forces Z Axis:" ) );

    TempMotorsDlgLayout->addWidget( TextLabel4_3_3, 10, 0 );

    m_pProgTempRotX = new Q3ProgressBar( this, "m_pProgTempRotX" );
    m_pProgTempRotX->setProperty( "progress", 0 );

    TempMotorsDlgLayout->addWidget( m_pProgTempRotX, 7, 0 );

    m_pProgTempRotY = new Q3ProgressBar( this, "m_pProgTempRotY" );
    m_pProgTempRotY->setProperty( "progress", 0 );

    TempMotorsDlgLayout->addWidget( m_pProgTempRotY, 9, 0 );

    m_pProgTempRotZ = new Q3ProgressBar( this, "m_pProgTempRotZ" );
    m_pProgTempRotZ->setProperty( "progress", 0 );

    TempMotorsDlgLayout->addWidget( m_pProgTempRotZ, 11, 0 );

    m_pProgTempY = new Q3ProgressBar( this, "m_pProgTempY" );
    m_pProgTempY->setProperty( "progress", 0 );

    TempMotorsDlgLayout->addWidget( m_pProgTempY, 3, 0 );

    m_pProgTempZ = new Q3ProgressBar( this, "m_pProgTempZ" );
    m_pProgTempZ->setProperty( "progress", 0 );

    TempMotorsDlgLayout->addWidget( m_pProgTempZ, 5, 0 );

    test = new QLabel( this, "test" );
    test->setProperty( "text", tr( "Temperature of Motor for Translational Forces X Axis:" ) );

    TempMotorsDlgLayout->addWidget( test, 0, 0 );

    m_pProgTempX = new Q3ProgressBar( this, "m_pProgTempX" );
    m_pProgTempX->setProperty( "progress", 0 );

    TempMotorsDlgLayout->addWidget( m_pProgTempX, 1, 0 );

    m_pScene = pScene;

    QTimer *tim = new QTimer( this );
    connect( tim, SIGNAL(timeout()), SLOT(updateTemp()) );
    tim->start( 1000, FALSE );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TempMotorsDlg::~TempMotorsDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/**
 * update the temperatures of the motors
 */
void TempMotorsDlg::updateTemp()
{
    /*
    m_pProgTempRotX->setProgress( (int)((m_pScene->getForceTool()->getTorqueXTemp()*100.0f) / 100.0f) );
    m_pProgTempRotY->setProgress( (int)((m_pScene->getForceTool()->getTorqueYTemp()*100.0f) / 100.0f) );
    m_pProgTempRotZ->setProgress( (int)((m_pScene->getForceTool()->getTorqueZTemp()*100.0f) / 100.0f) );
    m_pProgTempX->setProgress( (int)((m_pScene->getForceTool()->getForceXTemp()*100.0f) / 100.0f) );
    m_pProgTempY->setProgress( (int)((m_pScene->getForceTool()->getForceYTemp()*100.0f) / 100.0f) );
    m_pProgTempZ->setProgress( (int)((m_pScene->getForceTool()->getForceZTemp()*100.0f) / 100.0f) );
    */
    // cout << ((m_pScene->getForceTool()->getTorqueZTemp()*100.0f) / 100.0f) << endl;
}
