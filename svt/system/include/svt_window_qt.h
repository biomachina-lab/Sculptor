/*-*-c++-*-*****************************************************************
                          svt_window_qt
                          -------------
    begin                : 12/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_QTGLWIDGET_H
#define __SVT_QTGLWIDGET_H

#ifdef SVT_SYSTEM_QT

// qt4 includes
#include <QApplication>
#include <QGLWidget>
//Added by qt3to4:
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QCloseEvent>

class QImage;

///////////////////////////////////////////////////////////////////////////////
// SVT Qt Application
///////////////////////////////////////////////////////////////////////////////

class svt_app : public QApplication
{
    Q_OBJECT

public:

    svt_app(int argc, char* argv[]);

public slots:

    void quit();
};

///////////////////////////////////////////////////////////////////////////////
// SVT Qt Widget
///////////////////////////////////////////////////////////////////////////////

class svt_qtGLWidget : public QGLWidget
{
    Q_OBJECT

    int m_iWin;

public:

    svt_qtGLWidget( QWidget* pParent, int iWin, QGLFormat oFormat );

    virtual ~svt_qtGLWidget();

protected:

    virtual void resizeGL( int iW, int iH );

    virtual void paintGL( );

    virtual void mousePressEvent( QMouseEvent* pEvent );

    virtual void mouseReleaseEvent( QMouseEvent* pEvent );

    virtual void mouseMoveEvent( QMouseEvent* pEvent );

    virtual void wheelEvent( QWheelEvent * pEvent );

    virtual void keyPressEvent( QKeyEvent* pEvent );

    virtual void keyReleaseEvent( QKeyEvent* pEvent );

    virtual void closeEvent ( QCloseEvent * pEvent );
};

/**
 * Get the main qt widget
 */
QWidget* svt_qtGetWidget();

/**
 * Set workspace/parent widget
 */
void svt_qtSetMainWidget(QWidget* );

/**
 * Get the framebuffer
 */
QImage* svt_qtGetFramebuffer();

#endif

#endif
