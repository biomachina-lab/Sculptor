/***************************************************************************
                          sculptor_log_event
                          ------------------
    begin                : 27.02.2008
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#include <sculptor_log_event.h>

///////////////////////////////////////////////////////////////////////////////
// The Custom Qt Event
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
sculptor_log_event::sculptor_log_event( QString oStr )
    : QEvent( (QEvent::Type)65432 ),
      m_oStr( oStr )
{
}

/**
 * Destructor
 */
sculptor_log_event::~sculptor_log_event()
{
}

/**
 * Return the text
 */
QString sculptor_log_event::getText()
{
    return m_oStr;
}
