/***************************************************************************
                          main.cpp
                          --------
    begin                : 27.03.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////


// qt4 includes
#include <QApplication>
#include <QX11Info>
#include <QString>
// sculptor includes
#include <sculptor_scene.h>
#include <sculptor_window.h>
#include <sculptor_version.h>
#include <sculptor_splitter.h>
// svt includes
#include <svt_core.h>
#include <svt_system.h>
#include <svt_init.h>
#include <svt_threads.h>
#include <svt_time.h>
#include <svt_exception.h>
#include <svt_qtMessages.h>
#include <svt_opengl.h>
#include <svt_string.h>
#include <svt_window.h>
#include <svt_window_int.h>
#ifdef SVT_SYSTEM_QT
#include <svt_window_qt.h>
#endif

sculptor_scene* g_pScene;
sculptor_window* g_pWindow;

int svt_findCmdArg(int argc, char** argv, const char* pString)
{
    for(int i=0;i<argc;i++)
    {
        if (strcmp(argv[i],pString) == 0)
            return i;
    }

    return -1;
}

#define SCL cout << svt_trimName("sculptor")

///////////////////////////////////////////////////////////////////////////////
// Code for Qt backend
///////////////////////////////////////////////////////////////////////////////

#include <sculptor_app.h>
sculptor_app* g_qApp = NULL;

void svtQtCallback()
{
    g_qApp->processEvents();
}

///////////////////////////////////////////////////////////////////////////////
// Code for all version except the Qt backend
///////////////////////////////////////////////////////////////////////////////

/*
void** g_aMemory;
unsigned int iSize   = 0;
unsigned int iAllocs = 0;

void* operator new (size_t sz)
{
    void* pointer = malloc( sz);

    //printf("DEBUG: Allocating: %i\n", sz );

    iAllocs++;

    if (iSize > 0 && iSize < iAllocs)
    {
        g_aMemory = (void**)realloc( g_aMemory, sizeof( size_t ) * (iSize*2) );
        iSize = iSize * 2;
    }
    if (iSize == 0)
    {
        g_aMemory = (void**)malloc( sizeof( size_t ) * 512 );
        iSize = 512;
    }

    g_aMemory[ iAllocs-1 ] = pointer;
    return pointer;
}
*/

void dumpMemTable()
{
    svt_node::dumpMemTable();
}

//This function allocates memory for a pointer
//void* operator new [] (size_t sz) {
//	return myNew(sz); //call my allocation function
//}

// global method to convert a QString to an std string
string QtoStd( QString oQ )
{
    return string( oQ.toAscii().constData(), oQ.length() );
}
// global method to convert an std string to a QString
QString StdtoQ( string oStd )
{
    return QString( oStd.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
// Main routine
///////////////////////////////////////////////////////////////////////////////

// main
int main(int argc, char** argv)
{
    int i;

    //
    // Command-line parameters?
    //
    if (svt_findCmdArg( argc, argv, "--help" ) != -1)
    {
        cout << endl;
        cout << "Usage: sculptor [options] <files>" << endl;
        cout << endl;
        cout << "Options:" << endl;
        cout << "  -stereo    = start sculptor using a stereo visual" << endl;
        cout << "  -aa        = enable antialiasing" << endl;
        cout << "  -no-vbo    = disable vertex buffer objects" << endl;
        cout << "  -no-fbo    = disable framebuffer objects" << endl;
        cout << "  -no-ao     = disable ambient occlusion" << endl;
        cout << "  -no-shader = disable use of vertex and fragment shader programs" << endl;
        cout << "  -gray      = use a dark gray ui-theme" << endl;
#ifdef SVT_SYSTEM_QT
        cout << "  -text      = collapse the console area on start" << endl;
#endif
        cout << "  -falcon    = use the Novint Falcon haptic device" << endl;
        cout << "  -phantom   = use the Phantom haptic device" << endl;
        cout << endl;
        exit(0);
    }

    //
    // Load the Configuration File
    //
    svt_loadConfig(argc,argv);

    // stereo mode?
    if (svt_findCmdArg( argc, argv, "-stereo" ) != -1)
        svt_getConfig()->setValue("STEREO", true );

    // antialiasing mode?
    if (svt_findCmdArg( argc, argv, "-aa" ) != -1)
        svt_getConfig()->setValue("SAMPLES", 4 );

    // Falcon haptic device?
    if (svt_findCmdArg( argc, argv, "-falcon" ) != -1)
    {
        svt_getConfig()->setValue("FeedbackDevice", "falcon" );
        svt_getConfig()->setValue("FeedbackMatrixChannel", 1 );
        svt_getConfig()->setValue("FeedbackButtonChannel", 2 );
        svt_getConfig()->setValue("FeedbackAuxButton1Channel", 3 );
        svt_getConfig()->setValue("FeedbackAuxButton2Channel", 4 );
        svt_getConfig()->setValue("FeedbackForceXChannel", 5 );
        svt_getConfig()->setValue("FeedbackForceYChannel", 6 );
        svt_getConfig()->setValue("FeedbackForceZChannel", 7 );
    }

    // Phantom haptic device?
    if (svt_findCmdArg( argc, argv, "-phantom" ) != -1)
    {
        svt_getConfig()->setValue("FeedbackDevice", "openhaptics" );
        svt_getConfig()->setValue("FeedbackMatrixChannel", 1 );
        svt_getConfig()->setValue("FeedbackButtonChannel", 2 );
        svt_getConfig()->setValue("FeedbackForceXChannel", 3 );
        svt_getConfig()->setValue("FeedbackForceYChannel", 4 );
        svt_getConfig()->setValue("FeedbackForceZChannel", 5 );
        svt_getConfig()->setValue("FeedbackTorqueForceXChannel", 6 );
        svt_getConfig()->setValue("FeedbackTorqueForceYChannel", 7 );
        svt_getConfig()->setValue("FeedbackTorqueForceZChannel", 8 );
        svt_getConfig()->setValue("FeedbackForceXTempChannel", 9 );
        svt_getConfig()->setValue("FeedbackForceYTempChannel", 10 );
        svt_getConfig()->setValue("FeedbackForceZTempChannel", 11 );
        svt_getConfig()->setValue("FeedbackTorqueXTempChannel", 12 );
        svt_getConfig()->setValue("FeedbackTorqueYTempChannel", 13 );
        svt_getConfig()->setValue("FeedbackTorqueZTempChannel", 14 );
    }

    //
    // Now Init Qt
    //

    // create application and dialog
    g_qApp = new sculptor_app( &argc, &argv );

#ifdef SVT_SYSTEM_X11
    __svt_setOtherX11Display( QX11Info::display() );
#endif

    // create main window
    g_pWindow = new sculptor_window(g_pScene);
    g_pWindow->setWindowTitle( QString("Sculptor") );

    // check settings from .sculptorrc
    if ( !g_pWindow->loadConfigValue("ALLOW_SHADER_PROGRAMS", 1) )
        svt_setAllowShaderPrograms(false);
    if ( !g_pWindow->loadConfigValue("ALLOW_AMBIENT_OCCLUSION", 1) )
        svt_setAllowAmbientOcclusion(false);


    svt_setGUICallback( svtQtCallback );

    // print Sculptor version
    SCL << "Sculptor " << SCULPTOR_VERSION << endl;
    SCL << "---------";
    for (i=0; i<(int)strlen(SCULPTOR_VERSION); ++i)
      cout << "-";
    cout << endl;
    SCL << endl;

    //
    // Now Init SVT
    //

    // test if fbo/vbo should be disabled
    if (svt_findCmdArg( argc, argv, "-no-fbo" ) != -1)
    {
	svt_disableEXT_FBO( true );
	SCL << "FBO extension disabled!" << endl;
    } else {
	svt_disableEXT_FBO( false );
    }
    if (svt_findCmdArg( argc, argv, "-no-vbo" ) != -1)
    {
	svt_disableEXT_VBO( true );
	SCL << "VBO extension disabled!" << endl;
    } else {
	svt_disableEXT_VBO( false );
    }
    if (svt_findCmdArg( argc, argv, "-no-shader" ) != -1)
    {
	svt_setAllowShaderPrograms( false );
	SCL << "Customized vertex and fragment shader programs disabled!" << endl;
    }
    if (svt_findCmdArg( argc, argv, "-no-ao" ) != -1)
    {
	svt_setAllowAmbientOcclusion( false );
	SCL << "Ambient occlusion disabled!" << endl;
    }

    // set window title
    svt_setWindowCaption("Sculptor");

#ifdef SVT_SYSTEM_QT
    // set workspace widget
    svt_qtSetMainWidget( g_pWindow->getMainWidgetArea() );
#endif

    // init svt
    svt_init(argc,argv);

    // create a new scene object
    g_pScene = new sculptor_scene();
    // and build the scene
    g_pScene->buildScene();
    // add it to the sculptor window
    g_pWindow->setScene( g_pScene );

    //
    // Now add the documents
    //
    // In this method, the window state will also be restored
    //
    g_pWindow->addDefaultDoc();

    // show main window
    g_pWindow->show();

    // exchange the default text console ui elements with the qt implementation
    svt_qtMessages* g_pQtMessages = new svt_qtMessages(g_pWindow);
    svt_exception::set_ui(g_pQtMessages);

    // if any filenames were given at the command-line, try to load them
    if (argc > 1)
	for (i=1; i < argc; i++)
	    if (argv[i][0] != '-')
		g_pWindow->loadDoc( argv[i], "NONE" );

#ifdef SVT_SYSTEM_QT
    if (svt_findCmdArg( argc, argv, "-text" ) != -1)
    {
        ((sculptor_splitter*) g_pWindow->getMainWidgetArea())->setCollapsed();
    }
#endif

    //g_pWindow->selectDoc();
    g_pWindow->setDefaultProbeTargetDoc();

#ifndef SVT_SYSTEM_QT
    g_qApp->connect( g_qApp, SIGNAL(lastWindowClosed()), g_qApp, SLOT(quit()) );
    g_pWindow->connect( g_pWindow, SIGNAL(destroyed()), g_qApp, SLOT(quit()));
#endif

    // cite STRIDE as the authors want it
    SCL << "For secondary structure assignment, Sculptor uses STRIDE:"<< endl;
    SCL << "  Knowledge-based protein secondary structure assignment."<< endl;
    SCL << "  Frishman D, Argos P. Proteins. 1995 Dec;23(4):566-79." << endl;

    // set scene
    svt_setScene(g_pScene);

#ifndef SVT_SYSTEM_QT
    g_qApp->setMainWidget( g_pWindow );
    g_qApp->processEvents();
#endif

    svt_start(g_pScene);

#ifndef SVT_SYSTEM_QT
    g_qApp->exit();
#endif

}
