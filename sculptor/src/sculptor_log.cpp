/***************************************************************************
                          sculptor_log
                          ------------
    begin                : 21.02.2008
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_log.h>
#include <sculptor_log_event.h>
#include <streambuf>


///////////////////////////////////////////////////////////////////////////////
// The Custom Qt TextEdit that receives these events
///////////////////////////////////////////////////////////////////////////////


/**
 * Constructor
 */
sculptor_log_textedit::sculptor_log_textedit(QWidget* pParent)
    : QTextEdit( pParent )
{
    // set minimum height to a low value
    setMinimumSize(0, 20);

    // no editing
    setReadOnly( true );

    // get the original font of this widget
    QFont oFont = font();

#ifdef MACOSX
    oFont.setFamily(QString::fromUtf8("Courier"));
#else
    oFont.setFamily(QString::fromUtf8("Courier New"));
#endif

    setFont(oFont);
};

/**
 * Destructor
 */
sculptor_log_textedit::~sculptor_log_textedit()
{
}

void sculptor_log_textedit::customEvent( QEvent * pEvent )
{
    if ( pEvent->type() == (QEvent::Type)65432 )
    {
        sculptor_log_event* pLogEvent = (sculptor_log_event*)pEvent;

        append( pLogEvent->getText() );
    }
};


/**
 * Constructor
 */
sculptor_log::sculptor_log(std::ostream &rCoutStream, QWidget* pParent)
    : std::basic_streambuf<char>(),
      m_oCoutStream(rCoutStream)
{
    m_pLogWidget = new sculptor_log_textedit( pParent );
    m_old_buf = rCoutStream.rdbuf();
    rCoutStream.rdbuf(this);
}

/**
 * Destructor
 */
sculptor_log::~sculptor_log()
{
    // output anything that is left
    if (!m_oString.empty())
	m_pLogWidget->append(m_oString.c_str());

    m_oCoutStream.rdbuf(m_old_buf);
}

/**
 *
 */
QWidget* sculptor_log::getLogWidget()
{
    return m_pLogWidget;
}

/**
 *
 */
void sculptor_log::update()
{
    m_pLogWidget->update();
}

/**
 *
 */
void sculptor_log::clear()
{
    m_pLogWidget->setText(QString(""));
}

/**
 *
 */
std::streamsize sculptor_log::xsputn(const char *p, std::streamsize n)
{
    m_oSema.P();

    m_oString.append(p, p + n);

    int pos = 0;
    while (pos != (int)std::string::npos)
    {
	pos = m_oString.find('\n');
	if (pos != (int)std::string::npos)
	{
	    std::string tmp(m_oString.begin(), m_oString.begin() + pos);

	    // send the lines of text as events to the widget
	    sculptor_log_event* pEvent = new sculptor_log_event( QString( tmp.c_str() ) );
	    QApplication::postEvent( m_pLogWidget, pEvent );
	    m_oString.erase(m_oString.begin(), m_oString.begin() + pos + 1);
	}
    }

    m_oSema.V();
    return n;
}
