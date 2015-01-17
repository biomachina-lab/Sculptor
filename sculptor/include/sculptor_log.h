/*-*-c++-*-*****************************************************************
                          sculptor_log
                          ------------
    begin                : 21.02.2008
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_LOG_H
#define SCULPTOR_LOG_H

// svt includes
#include <svt_iostream.h>
// sculptor includes
#include <sculptor_log_event.h>
// qt4 includes
#include <QEvent>
#include <QTextEdit>
#include <QApplication>
//FIXME non sculptor/svt includes
#include <streambuf>
#include <string>


/**
 * A special text edit widget that understands the sculptor_log_events
 */
class sculptor_log_textedit : public QTextEdit
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    sculptor_log_textedit(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~sculptor_log_textedit();

protected:

    /**
     * 
     */
    virtual void customEvent( QEvent * pEvent );
};


/**
 * A special logging widget that redirects the cout to an internal textedit widget
 */
class sculptor_log : public std::basic_streambuf<char>
{

protected:

    std::ostream &m_oCoutStream;
    std::streambuf *m_old_buf;
    std::string m_oString;
    sculptor_log_textedit* m_pLogWidget;
    svt_semaphore m_oSema;

public:

    /**
     * Constructor
     */
    sculptor_log(std::ostream &rCoutStream, QWidget* pParent);
    /**
     * Destructor
     */
    virtual ~sculptor_log();

    /**
     *
     */
    QWidget* getLogWidget();

    /**
     *
     */
    void update();

    /**
     *
     */
    void clear();

protected:

    /**
     *
     */
    virtual int_type overflow(int_type v)
    {
	m_oSema.P();

	if (v == '\n')
	{
	    sculptor_log_event* pEvent = new sculptor_log_event( QString( m_oString.c_str() ) );
	    QApplication::postEvent( m_pLogWidget, pEvent );
	    m_oString.erase(m_oString.begin(), m_oString.end());
	}
	else
	    m_oString += v;

	m_oSema.V();

	return v;
    };

    /**
     *
     */
    virtual std::streamsize xsputn(const char *p, std::streamsize n);
};

#endif
