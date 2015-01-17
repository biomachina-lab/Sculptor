/*-*-c++-*-*****************************************************************
                          sculptor_log_event
                          ------------------
    begin                : 27.02.2008
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_LOG_EVENT_H
#define SCULPTOR_LOG_EVENT_H

// qt4 includes
#include <QEvent>
#include <QString>

/**
 * A special custom event class that decouples the text output from the gui thread. Qt only allows
 * GUI commands and updates within a single thread and not from 'outside' for example from another
 * thread.
 */
class sculptor_log_event : public QEvent
{

protected:

    QString m_oStr;

public:

    /**
     * Constructor
     */
    sculptor_log_event( QString oStr );
    /**
     * Destructor
     */
    virtual ~sculptor_log_event();

    /**
     * Return the text
     */
    QString getText();
};

#endif
