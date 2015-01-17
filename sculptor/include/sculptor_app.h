/***************************************************************************
                          sculptor_app.h
                          -------------------
    begin                : 26.04.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_APP_H
#define SCULPTOR_APP_H

// qt4 includes
#include <QApplication>

class sculptor_app : public QApplication
{
    Q_OBJECT

public:
    sculptor_app(int * argc, char** argv[]);
    virtual ~sculptor_app() {};

    /**
     * Get the name of the current style
     */
    QString getStyleName();

public slots:

    /**
     * Create and set a new style
     * \param style string with the style name, e.g. Sculptor or Windows
     */
    void makeStyle(const QString &style);

    void quit();

protected:

    QString m_oStyleName;

};

#endif // SCULPTOR_APP_H
